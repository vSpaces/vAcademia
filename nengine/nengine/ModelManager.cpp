
#include "StdAfx.h"

#ifdef USING_GEOMETRY_OPTIMIZATION
	#include "include/tootlelib.h"
#endif

#include "GlobalSingletonStorage.h"
#include "PlatformDependent.h"
#include "ResourceProcessor.h"
#include "HelperFunctions.h"
#include "MemFileResource.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "VBOSubObject.h"
#include "FileResource.h"

#ifdef USING_GEOMETRY_OPTIMIZATION
	#include "NvTriStrip.h"
#endif

#include "Matrix4D.h"
#include "NEngine.h"
#include <algorithm>
#include "Math.h"

#include "IAsynchResource.h"
#include "IAsynchResourcemanager.h"

#include "zlibengn.h"
#include <malloc.h>
#include "AddNCacheDataTask.h"
#include "cal3d/memory_leak.h"

BOOL last_multitex;

#define BEGIN_END_DRAW(startOffset, endOffset)	GLFUNC(glBegin)(GL_TRIANGLES);\
			for (int k = startOffset; k < endOffset; k++)\
			{\
				int index = model->fs[k];\
				if (model->tx[0])\
				{\
					GLFUNC(glMultiTexCoord2fARB)(GL_TEXTURE0_ARB, model->tx[0][index * 2], model->tx[0][index * 2 + 1]);\
				}\
				if (model->tns)\
				{\
					GLFUNC(glMultiTexCoord4fARB)(GL_TEXTURE1_ARB, model->tns[index * 4], model->tns[index * 4 + 1], model->tns[index * 4 + 2], model->tns[index * 4 + 3]);\
				}\
				else if (model->tx[1])\
				{\
					GLFUNC(glMultiTexCoord2fARB)(GL_TEXTURE1_ARB, model->tx[1][index * 2], model->tx[1][index * 2 + 1]);\
				}\
				GLFUNC(glNormal3f)(model->vn[index * 3], model->vn[index * 3 + 1], model->vn[index * 3 + 2]);\
				GLFUNC(glVertex3f)(model->vx[index * 3], model->vx[index * 3 + 1], model->vx[index * 3 + 2]);\
			}\
			GLFUNC(glEnd)();

//#define USE_DISPLAY_LISTS			1
#define DISPLAY_LIST_MAX_FACES		5000

_SNeighborPair::_SNeighborPair():
	MP_STRING_INIT(sign1), 
	MP_STRING_INIT(sign2), 
	MP_STRING_INIT(path1), 
	MP_STRING_INIT(path2)
{
}

_SNeighborPair::_SNeighborPair(const _SNeighborPair& other):
	MP_STRING_INIT(sign1), 
	MP_STRING_INIT(sign2), 
	MP_STRING_INIT(path1), 
	MP_STRING_INIT(path2)
{
	sign1 = other.sign1;
	sign2 = other.sign2;
	path1 = other.path1;
	path2 = other.path2;
}

_SLoadingModel::_SLoadingModel():
	MP_STRING_INIT(cacheFileName)
{
	model = NULL;
}

_SSaveLoadingModelTaskInfo::_SSaveLoadingModelTaskInfo(CModel* _model, std::string& _path, ifile* _file,
									   std::vector<CMaterial *>& _materials, std::vector<void *>* _owners):
	MP_STRING_INIT(path),
	MP_VECTOR_INIT(materials),
	MP_VECTOR_INIT(owners)
{
	model = _model;
	path = _path;
	file = _file;

	std::vector<CMaterial *>::iterator it = _materials.begin();
	std::vector<CMaterial *>::iterator itEnd = _materials.end();

	for ( ; it != itEnd; it++)
	{
		materials.push_back(*it);
	}

	if (_owners != NULL)
	{
		std::vector<void *>::iterator it = _owners->begin();
		std::vector<void *>::iterator itEnd = _owners->end();

		for ( ; it != itEnd; it++)
		{
			owners.push_back(*it);
		}		
	}	
}

CModelManager::CModelManager():
	m_isNeedInvertCCW(false),
	m_currentRenderingModel(NULL),
	MP_VECTOR_INIT(m_neighbors),
	MP_MAP_INIT(m_propertyBlockLoadHandlerList),
	MP_VECTOR_INIT(m_loadingModelTasks),
	MP_VECTOR_INIT(m_readUncompressedModelsTasks),
	MP_VECTOR_INIT(m_loadingModelTaskInfo)
{
#ifdef USING_GEOMETRY_OPTIMIZATION
	bool isTootleInitSuccessfully = (TootleInit() == TOOTLE_OK);
	assert(isTootleInitSuccessfully);
#endif

	ReserveMemory(1000);
}

CModel* CModelManager::GetCurrentRenderingModel()const
{
	return m_currentRenderingModel;
}

void CModelManager::RelinkMaterials()
{
	for (int i = 0; i < GetCount(); i++)
	{
		CModel* model = g->mm.GetObjectPointer(i);
		if (model->IsDeleted())
		{
			continue;
		}

		model->RelinkMaterials();
	}
}

void CModelManager::LoadNeighbors()
{
	std::wstring fileName = GetApplicationRootDirectory();
	fileName += L"neighbors.xml";
	USES_CONVERSION;
	TiXmlDocument doc( W2A(fileName.c_str()));
	bool isLoaded = g->rl->LoadXml( (void*)&doc, fileName);

	if (!isLoaded)
	{
		g->lw.WriteLn("[ERROR] Loading neighbors failed...");
		return;
	}

	TiXmlNode* neighbors = doc.FirstChild();

	if ((neighbors->NoChildren()) || (neighbors == NULL))
	{
		return;
	}

	TiXmlNode* neighborsPair = neighbors->FirstChild();

	int i = 0;

	while (neighborsPair != NULL)
	{
		i++;

		if ((!neighborsPair->ToElement()->Attribute("sign1")) ||
			(!neighborsPair->ToElement()->Attribute("path1")) ||
			(!neighborsPair->ToElement()->Attribute("sign2")) ||
			(!neighborsPair->ToElement()->Attribute("path2")))
		{
			g->lw.WriteLn("[ERROR] Loading neighbors failed... Incomplite info about pair.");
			return;
		}

		SNeighborPair pair;
		pair.sign1 = neighborsPair->ToElement()->Attribute("sign1");
		pair.sign2 = neighborsPair->ToElement()->Attribute("sign2");
		pair.path1 = neighborsPair->ToElement()->Attribute("path1");
		pair.path2 = neighborsPair->ToElement()->Attribute("path2");

		m_neighbors.push_back(pair);

		neighborsPair = neighbors->IterateChildren(neighborsPair);		
	}
}

void CModelManager::AlignModel(int nModel, int CenterX, int CenterY, int CenterZ)
{
	CModel* model = GetObjectPointer(nModel);

	float dx = model->GetModelMinX() + (model->GetModelMaxX() - model->GetModelMinX()) / 2.0f;
	float dy = model->GetModelMinY() + (model->GetModelMaxY() - model->GetModelMinY()) / 2.0f;
	float dz = model->GetModelMinZ() + (model->GetModelMaxZ() - model->GetModelMinZ()) / 2.0f;
	int str2 = model->GetVertexCount();

	if (CenterX == 1)
	for (int i = 0; i < str2; i++)
	{
		model->vx[i * 3 + 0] = model->vx[i * 3 + 0]-dx;
	}

	if (CenterX == 2)
	for (int i = 0; i < str2; i++)
	{
		model->vx[i * 3 + 0] = model->vx[i * 3 + 0] - model->GetModelMinX();
	}

	if (CenterY == 1)
	for (int i = 0; i < str2; i++)
	{
		model->vx[i * 3 + 1] = model->vx[i * 3 + 1]-dy;
	}

	if (CenterY == 2)
	for (int i = 0; i < str2; i++)
	{
		model->vx[i * 3 + 1] = model->vx[i * 3 + 1] - model->GetModelMinY();
	}
	
	if (CenterZ == 1)
	for (int i = 0; i < str2; i++)
	{
		model->vx[i * 3 + 2] = model->vx[i * 3 + 2]-dz;	
	}

	if (CenterZ == 2)
	for (int i = 0; i < str2; i++)
	{
		model->vx[i * 3 + 2] = model->vx[i * 3 + 2] - model->GetModelMinZ();
	}

	model->ReCountBoundingBox();
}

void CModelManager::LoadAll()
{
	g->rp.OpenResFile(g->ne.GetModelsResourceFileID());
	g->lw.WriteLn("Loading models...");
	
	int i = 0, kfiles = 0;

	std::vector<std::string> fileNames;
	std::vector<int> fileSizes;

	kfiles = g->rp.GetShort();
	for (i = 0; i < kfiles; i++)
	{
		std::string fileName;

		char ch = 2;
		char tmp[2];
		tmp[1] = 0;
		while (ch != 0)
		{
			ch = g->rp.GetChar();
			tmp[0] = ch;
			fileName += tmp;
		}

		int ml = AddObject(fileName);
		CModel* model = GetObjectPointer(ml);

		model->SetCullFaceSetting(BoolFromUShort(g->rp.GetShort()));
		model->SetRColor((unsigned char)g->rp.GetShort());
		model->SetGColor((unsigned char)g->rp.GetShort());
		model->SetBColor((unsigned char)g->rp.GetShort());
		model->SetAlignX((unsigned char)g->rp.GetShort());
		model->SetAlignY((unsigned char)g->rp.GetShort());
		model->SetAlignZ((unsigned char)g->rp.GetShort());

		fileNames.push_back(fileName);
		fileSizes.push_back(g->rp.GetLong());
	}

	for (i = 0; i < kfiles; i++)
	{
		g->lw.Write("Loading ");
		g->lw.Write(fileNames[i].c_str());
		g->lw.WriteLn(" - ok.");

		int modelID = GetObjectNumber(fileNames[i]);
		CModel* model = GetObjectPointer(modelID);

		m_modelLoader.LoadModel(fileNames[i], &g->rp, model);
		m_modelLoader.LoadPhysics(fileNames[i], &g->rp, model);

		AlignModel(i, model->GetAlignX(), model->GetAlignY(), model->GetAlignZ());
		OptimizeModel(i, MODEL_OPTIMIZATION_VERTEX_CACHE_AND_OVERDRAW);
		PrepareOptimalModel(i);
	}
}

int CModelManager::LoadAsynch(std::string path, std::string name)
{
	int modelID = g->mm.AddObject(name);
	CModel* model = g->mm.GetObjectPointer(modelID);

	std::vector<void *> owners;
	if (g->rl->GetAsynchResMan()->GetLoadingMesh())
	{
		owners.push_back(g->rl->GetAsynchResMan()->GetLoadingMesh());
	}

	IAsynchResource* asynchRes = g->rl->GetAsynchResMan()->GetAsynchResource(path.c_str(), true, RES_TYPE_MESH, model, g->rl, &owners, model);
	
	if (asynchRes)
	{
		// resource not found on local disk
		if (g->rl->GetAsynchResMan()->GetLoadingMesh())
		{			
			C3DObject* obj = (C3DObject *)g->rl->GetAsynchResMan()->GetLoadingMesh();
			asynchRes->SetLOD((int)obj->GetUserData(2));	
		}
	}
	else
	{		
		ifile* file = g->rl->GetResMemFile(path.c_str());
		LoadFromIFile(model, file, &owners);
		g->rl->DeleteResFile(file);
	}

	return modelID;
}

int CModelManager::LinkTextureToModel(std::string _texturePath, std::string path, bool isNeedExpand, std::vector<void*>* owners, bool canBeCompressed,
									  bool isNormalMap)
{
	int pos = path.find_last_of("\\");
	path = path.substr(0, pos);
		
	if (isNeedExpand)
	{
		pos = path.find_last_of("\\");
		path = path.substr(0, pos);
		path += "\\textures\\";
	}
	else
	{
		path += "\\";
	}


	std::string texturePath = path;
	texturePath += _texturePath;

	if (_texturePath.substr(0, 3) == "CMN")
	{
		texturePath = ":geometry\\common\\textures\\";
		texturePath += _texturePath;
	}

	texturePath = CorrectPath(texturePath);

	int textureID = -1;
	std::string textureName = _texturePath;

	if (!textureName.empty())
	{
		if ('#' == textureName[0])
		{
			textureID =	g->tm.AddColorTexture(textureName);
		}
		else
		{
			textureID = g->tm.GetObjectNumber(textureName);
			if (textureID == -1)
			{
				CNTextureLoadingInfo info;
				info.SetCanBeCompressed(canBeCompressed);
				if (isNormalMap)
				{
					//info.SetCanBeCompressed(false);
			}
			textureID = g->tm.LoadAsynch(_texturePath, texturePath, owners, false, &info);			
		}
	}
	}

	return textureID;	
}


void CModelManager::LinkTexturesToModel(CModel* model, std::string path, bool isNeedExpand, std::vector<void*>* owners)
{
	model->CheckMaterials();

	path = CorrectPath(path);

	for (int k = 0; k < model->GetTextureCount() - 1; k++)
	{		
		int pos = path.find_last_of("\\");		
		path = path.substr(0, pos);
		
		if (isNeedExpand)
		{
			pos = path.find_last_of("\\");
			path = path.substr(0, pos);
			path += "\\textures\\";
		}
		else
		{
			path += "\\";
		}

		std::string texturePath = path;
		texturePath += model->GetTextureName(k);

		if (model->GetTextureName(k).substr(0, 3) == "CMN")
		{
			texturePath = ":geometry\\common\\textures\\";
			texturePath += model->GetTextureName(k);
		}

		int textureID;
		std::string textureName = model->GetTextureName(k);

		if (!textureName.empty())
		{
			if ('#' == textureName[0])
			{
				textureID =	g->tm.AddColorTexture(textureName);
			}
			else
			{
				textureID = g->tm.GetObjectNumber(textureName);
				if (textureID == -1)
				{					
					std::string textureName = model->GetTextureName(k);
					if (!model->IsTextureCompressionPermitted())
					{
						CNTextureLoadingInfo info;
						info.SetCanBeCompressed(false);						
						textureID = g->tm.LoadAsynch(textureName, texturePath, owners, false, &info);
					}
					else
					{
						textureID = g->tm.LoadAsynch(textureName, texturePath, owners);
					}
				}
			}
			model->ChangeTexture(k, textureID);
		}
		model->OnAfterChanges();
	}
}

void CModelManager::Update()
{
	int readingMaxTime = 2;
	int creatingMaxTime = 10;
	
	USES_CONVERSION;
	__int64 t1 = g->tp.GetTickCount();
	std::vector<CReadingCacheResourceTask*>::iterator itU = m_readUncompressedModelsTasks.begin();
	for (; (itU != m_readUncompressedModelsTasks.end()) && ((g->tp.GetTickCount() - t1) <= readingMaxTime);)
	{
		CReadingCacheResourceTask* taskRC = *itU;
		// если подошла очередь 
		if (taskRC->IsPerformed())				
		{
			SSaveLoadingModelTaskInfo* info = (SSaveLoadingModelTaskInfo*)taskRC->GetUserData();

			if ((taskRC->GetData()) && (taskRC->GetDataSize() != 0))
			{
				//создаем асинхронный таск
				CLoadingModelTask* task = MP_NEW(CLoadingModelTask);
				m_loadingModelTasks.push_back(task);

				// устанавливаем параметры таска 
				task->SetParams(info->model, info->path, info->materials, &info->owners);
				task->SetFileInfo(taskRC->GetData(), taskRC->GetDataSize());

				//добавляем таск в очередь на выполнение
				g->taskm.AddTask(task, MAY_BE_MULTITHREADED, info->model->GetLoadPriority());
			}
			else
			{
				std::string fileName = taskRC->GetFileName();
				g->dc.DeleteFromMemoryList(fileName);
				info->model->SetLoadingState(MODEL_NOT_LOADED, MODEL_LOAD_ERROR_CACHE_DAMAGED);				
				if (info->owners.size() > 0)
				{
					g->rl->GetAsynchResMan()->SetLoadingMesh(info->owners[0]);
				}
				g->mm.LoadAsynch(info->path + (info->model->IsTextureCompressionPermitted() ? ".zip" : ".xzip"), W2A(info->model->GetName()));
				g->rl->GetAsynchResMan()->SetLoadingMesh(NULL);
			}
			
			MP_DELETE(*itU);
			itU = m_readUncompressedModelsTasks.erase(itU);	
		}
		else
		{
			itU++;
		}
	}

	__int64 t2 = g->tp.GetTickCount();
	std::vector<CLoadingModelTask*>::iterator itM = m_loadingModelTasks.begin();
	for (; (itM != m_loadingModelTasks.end()) && ((g->tp.GetTickCount() - t2) <= creatingMaxTime);)
	{
		bool taskRemoved = false;
		CLoadingModelTask* task = *itM;
		// если подошла очередь 
		if (task->IsPerformed())				
		{
			int t3 = (int)g->tp.GetTickCount();
			
			if (task->IsLoaded())
			{
				// берем данные из таска
				CModel* model = task->GetModel();

				if (model->GetFaceCount() > 0)
				{
					model->FinalizeMaterials();

					std::string path = task->GetPath();
					std::vector<void*>* owners = task->GetOwners();
					std::vector<CMaterial*> materials = task->GetMaterials();

					LinkTexturesToModel(model, path, true, owners);

					std::vector<CMaterial *>::iterator it = materials.begin();
					std::vector<CMaterial *>::iterator itEnd = materials.end();

					for ( ; it != itEnd; it++)
					{
						int textureID;
						for (textureID = 0; textureID < (*it)->GetNotFoundTexturesCount(); textureID++)
						{
							int _textureID = LinkTextureToModel((*it)->GetNotFoundTextureName(textureID), path, true, owners, (*it)->GetNotFoundTexturePackedStatus(textureID), (*it)->IsNotFoundTextureNormalMap(textureID));
							(*it)->UpdateNotFoundTexture(textureID, _textureID);
						}
					}

					for (int i = 0; i < model->GetTextureCount() - 1; i++)
					{	
						int materialID = model->GetMaterial(i);
						CMaterial* mat = NULL;
						if (materialID != -1)
						{
							mat = g->matm.GetObjectPointer(materialID);
						}

						if (mat)
						if (mat->IsAlphaBlendExists())
						{
							if (!model->tx[1])
							{
								int vertexCount = model->GetVertexCount();
								model->tx[1] = MP_NEW_ARR(float, vertexCount * 2);
								memcpy(model->tx[1], model->tx[0], vertexCount * 2 * 4);
							}
						}
					}

					int modelID = model->GetID();

					GenerateNeighborDataIfNeeded(model, path);

					OptimizeModel(modelID, MODEL_OPTIMIZATION_VERTEX_CACHE_AND_OVERDRAW);
					PrepareOptimalModel(modelID);				

					model->SetLoadingState(MODEL_LOADED, 0);
				}
				else
				{
					model->SetLoadingState(MODEL_NOT_LOADED, MODEL_LOAD_ERROR_PARSE_FAILED);
				}
				model->OnChanged(0);

				int t4 = (int)g->tp.GetTickCount();
				stat_allModelsTime += (t4 - t3);
				stat_allModelsNormalsTime += 0;
				stat_allModelsZlibTime += (int)(t2 - t1);
				MP_DELETE(*itM);
				itM = m_loadingModelTasks.erase(itM);
				taskRemoved = true;
			}
			else
			{
				if (task->GetModel())
				{
					task->GetModel()->SetLoadingState(MODEL_NOT_LOADED, MODEL_LOAD_ERROR_TASK_FAILED);
					task->GetModel()->OnChanged(0);
				}

				MP_DELETE(*itM);
				itM = m_loadingModelTasks.erase(itM);
				taskRemoved = true;
			}
		}
		if( !taskRemoved)
			itM++;
	}

	//g->lw.WriteLn("g->tp.GetTickCount() - t2 = ", g->tp.GetTickCount() - t2);
}

void CModelManager::LoadFromIFile(CModel* model, ifile* file, std::vector<void *>* owners)
{
	if (!file)
	{
		return;
	}

	if (!file->get_file_path())
	{
		return;
	}

	if ((model->GetLoadingState() != MODEL_EMPTY) &&
		(!((model->GetLoadingState() == MODEL_NOT_LOADED) && (model->GetLoadingErrorCode() == MODEL_LOAD_ERROR_CACHE_DAMAGED))))
	{		
		model->OnAfterChanges();
		return;
	}

	model->SetSourceSize(file->get_file_size());

	std::vector<CMaterial *> materials;

	CMemFileResource ifileResource;

	USES_CONVERSION;
	std::string path = W2A(file->get_file_path());
	bool isPacked = ((path.rfind(".zip") != std::string::npos));
	bool isNotTextureCompressionPermitted = ((path.rfind(".xzip") != std::wstring::npos));
	if (isNotTextureCompressionPermitted)
	{
		model->PermitTextureCompression(!isNotTextureCompressionPermitted);
		isPacked = true;
	}

	unsigned char *decompressedData = NULL;
	if (isPacked)
	{
		//распаковка
		unsigned char *fileData;
		unsigned long fileSize = 0;	

		fileSize = file->get_file_size();
		if (0 == fileSize)
		{
			model->SetLoadingState(MODEL_NOT_LOADED, MODEL_LOAD_ERROR_FILE_EMPTY);				
			model->OnChanged(0);
			return;
		}
		fileData = MP_NEW_ARR(unsigned char, fileSize);
		unsigned int uncompressedSize = 0;
		if (file->read((unsigned char*)&uncompressedSize, 4) != 4)
		{			
			MP_DELETE_ARR(fileData);
			model->SetLoadingState(MODEL_NOT_LOADED, MODEL_LOAD_ERROR_FILE_DAMAGED);				
			model->OnChanged(0);
			return;
		}		
		if (0 == uncompressedSize)
		{
			MP_DELETE_ARR(fileData);
			model->SetLoadingState(MODEL_NOT_LOADED, MODEL_LOAD_ERROR_FILE_DAMAGED);				
			model->OnChanged(0);
			return;
		}
		if (file->read(fileData, fileSize - 4) != fileSize - 4)
		{
			MP_DELETE_ARR(fileData);
			model->SetLoadingState(MODEL_NOT_LOADED, MODEL_LOAD_ERROR_FILE_DAMAGED);				
			model->OnChanged(0);
			return;
		}

		unsigned int offset = LoadMaterials(model, uncompressedSize, fileData, materials);		
		if (offset == 0xFFFFFFFF)
		{
			MP_DELETE_ARR(fileData);
			model->SetLoadingState(MODEL_NOT_LOADED, MODEL_LOAD_ERROR_MATERIALS_DAMAGED);				
			model->OnChanged(0);			
			return;
		}

		fileSize -= offset;
		fileSize -= 4;

		__int64 modifyTime = file->get_modify_time();

		//получаем имя файла в кеше
		std::string cacheFileName = GetCacheName(path, modifyTime, false);

		//если в кеше, то берем данные файла из кеша в таске
		if (g->dc.CheckIfDataExists(cacheFileName))
		{
			MP_DELETE_ARR(fileData);

			CReadingCacheResourceTask* task = MP_NEW(CReadingCacheResourceTask);
			task->SetFileName(cacheFileName);
			int pos = path.rfind(".xzip");
			if (pos == -1)
			{
				pos = path.rfind(".zip");
			}
			path = path.substr(0, pos);

			MP_NEW_P5(modelLoadingData, SSaveLoadingModelTaskInfo, model, path, file, materials, owners);

			m_loadingModelTaskInfo.push_back(modelLoadingData);

			task->SetUserData(m_loadingModelTaskInfo[m_loadingModelTaskInfo.size()-1]);

			m_readUncompressedModelsTasks.push_back(task);

			//добавляем таск в очередь на выполнение
			g->taskm.AddTask(task, MAY_BE_MULTITHREADED, model->GetLoadPriority(), true);

			t1 = 0;
			t2 = t1;
		}
		//если в кеше нет, то распаковываем в главном потоке
		else
		{
			t1 = (int)g->tp.GetTickCount();

			unsigned long decompBufSize = uncompressedSize;
			decompressedData = MP_NEW_ARR(unsigned char, decompBufSize);

			ZlibEngine zip;
			int decompResult = zip.decompmem((char*)(fileData + offset), &fileSize, (char*)decompressedData, &decompBufSize);
			if (decompResult < 0)
			{
				model->SetLoadingState(MODEL_NOT_LOADED, MODEL_LOAD_ERROR_DECOMPRESS_FAILED);				
				model->OnChanged(0);			
				return;
			}		

			// добавляем данные в кеш
			g->dc.AddDataAsynch(decompressedData, decompBufSize, cacheFileName);

			//file->attach(decompressedData, decompBufSize);

			MP_DELETE_ARR(fileData);	

			t2 = (int)g->tp.GetTickCount();

			ifileResource.Init(file);
			g_tp = &g->tp;

			int pos = path.rfind(".xzip");
			if (pos == -1)
			{
				pos = path.rfind(".zip");
			}
			path = path.substr(0, pos);

			//создаем асинхронный таск
			CLoadingModelTask* task = MP_NEW(CLoadingModelTask);
			m_loadingModelTasks.push_back(task);

			// устанавливаем параметры таска 
			task->SetParams(model, path, NULL, materials, owners);
			task->SetFileInfo(decompressedData, decompBufSize);

			//добавляем таск в очередь на выполнение
			g->taskm.AddTask(task, MAY_BE_MULTITHREADED, model->GetLoadPriority());
		}
		model->SetLoadingState(MODEL_LOADING, 0);
	}	
}

void CModelManager::LoadModel(CModel* model, std::string& path, CMemFileResource* /*fileResource*/, ifile* file,
							  std::vector<CMaterial *>& materials, std::vector<void *>* owners)
{
	//создаем асинхронный таск
	CLoadingModelTask* task = MP_NEW(CLoadingModelTask);
	m_loadingModelTasks.push_back(task);

	// устанавливаем параметры таска 
	task->SetParams(model, path, file, materials, owners);

	//добавляем таск в очередь на выполнение
	g->taskm.AddTask(task, MAY_BE_MULTITHREADED, model->GetLoadPriority());

	model->SetLoadingState(MODEL_LOADING, 0);
}

unsigned int CModelManager::LoadMaterials(CModel* model, unsigned int& sign, unsigned char* fileData, std::vector<CMaterial *>& materials)
{
	char tmp[5];
	memcpy(&tmp[0], &sign, 4);
	tmp[4] = 0;

	int offset = 0;

	if (strcmp(tmp, "EBMF") == 0)
	{
		int matCount = fileData[0];
		if (matCount >= 64)
		{
			return 0xFFFFFFFF;
		}
		offset = 1;
			
		for (int i = 0; i < matCount; i++)
		{
			std::string fileName;
			
			unsigned short nameSize = *(unsigned short *)(fileData + offset);
			offset += 2;

			if (nameSize >= 300)
			{
				return 0xFFFFFFFF;
			}

			char* str = (char *)_alloca(nameSize + 1);
			str[nameSize] = 0;
			memcpy(str, fileData + offset, nameSize);
			fileName = str;

			offset += nameSize;

			unsigned short size = *(unsigned short*)(fileData + offset);

			if (nameSize >= 4096)
			{
				return 0xFFFFFFFF;
			}

			offset += 2;

			unsigned char* materialData = (unsigned char *)_alloca(size + 1);
			memcpy(materialData, fileData + offset, size);
			materialData[size] = 0;

			MP_MAP<MP_STRING, IPropertyBlockLoadHandler *>::iterator it = m_propertyBlockLoadHandlerList.find(MAKE_MP_STRING(fileName));
            if (it != m_propertyBlockLoadHandlerList.end())
			{
				((*it).second)->OnPropertyBlockLoaded(materialData, size, fileName, model->GetID());
			}
			else
			{
				CMaterial* mat = g->matm.LoadMaterial("external\\materials\\" + fileName + model->GetPostfix(), materialData);
				if (model->GetPostfix().size() > 0)
				{
					mat->SetUniqueStatus(true);
				}
				materials.push_back(mat);
			}			

			offset += size;
			}

		sign = *(unsigned int*)(fileData + offset);
		offset += 4;
	}

	return offset;
}

void CModelManager::GenerateNeighborDataIfNeeded(CModel* model, std::string& path)
{
	std::vector<SNeighborPair>::iterator it = m_neighbors.begin();
	std::vector<SNeighborPair>::iterator itEnd = m_neighbors.end();

	USES_CONVERSION;
	for ( ; it != itEnd; it++)
	if ((path.find((*it).sign1) != -1) || (path.find((*it).sign2) != -1))
		{
			CModel* _model = NULL;
			CModel* model2 = NULL;
			int _vertexCount = 0, vertexCount = 0;
			float* vx = NULL;
			float* _vx = NULL;

			if (path.find("sea") != -1)
			{
				int modelID = GetObjectNumber((*it).path2);
				if (modelID != -1)
				{
					_model = g->mm.GetObjectPointer(modelID);
					if (_model->GetLoadingState()  == MODEL_LOADED)
					{
						_vertexCount = _model->GetVertexCount();
						_vx = _model->vx;
					}
				}
								
				model2 = model;
				vertexCount = model2->GetVertexCount();
				vx = model2->vx;				
			}
			else
			{
				_model = model;
				_vertexCount = _model->GetVertexCount();
				_vx = _model->vx;

				int modelID = GetObjectNumber((*it).path1);
				if (modelID != -1)
				{
					model2 = g->mm.GetObjectPointer(modelID);
					if (model2->GetLoadingState()  == MODEL_LOADED)
					{
						vertexCount = model2->GetVertexCount();
						vx = model2->vx;			
					}
				}		
			}

			float koef = 0.975f;

			if ((model2) && (!model2->tx[1]) && (vx) && (_vx))
			{
				model2->tx[1] = MP_NEW_ARR(float, vertexCount * 2);
				int* neigbCount = MP_NEW_ARR(int, vertexCount);
				memset(neigbCount, 0, vertexCount * 4);
				int i;
				for (i = 0; i < model2->GetFaceCount() * 3; i++)
				{
					neigbCount[model2->fs[i]]++;
				}

				for (i = 0; i < vertexCount; i++)
				{
					int l = -1;
					float r = 10000000000000.0f;
					for (int k = 0; k < _vertexCount; k++)
					{
						float _r = (vx[i * 3] - _vx[k * 3] * koef) * (vx[i * 3] - _vx[k * 3] * koef) + 
							(vx[i * 3 + 1] - _vx[k * 3 + 1] * koef) * (vx[i * 3 + 1] - _vx[k * 3 + 1] * koef) + 
							(vx[i * 3 + 2] - _vx[k * 3 + 2] * koef) * (vx[i * 3 + 2] - _vx[k * 3 + 2] * koef);

						if ((_r < r) && (!((vx[i * 3] == _vx[k * 3]) && (vx[i * 3 + 1] == _vx[k * 3 + 1]))) )
						{
							r = _r;
							l = k;
						}
					}
				
					float _x = vx[i * 3] - _vx[l * 3] * koef ;
					float _y = vx[i * 3 + 1] - _vx[l * 3 + 1] * koef;
					float _r = sqrtf(_x * _x + _y * _y);
					if (_r < 1000.0f)
					{
						model2->tx[1][i * 2 + 0] = -_x / _r;
						model2->tx[1][i * 2 + 1] = -_y / _r;
					}
					else
					{
						model2->tx[1][i * 2 + 0] = 0.0f;
						model2->tx[1][i * 2 + 1] = 0.0f;
					}
				}

				MP_DELETE_ARR(neigbCount);
				
				if ( (*it).path1 != W2A(model->GetName()))
				{
					int modelID = GetObjectNumber((*it).path1);
					PrepareOptimalModel(modelID);
				}
			}
		}
}

/*int CModelManager::LoadFromFile(std::string name, std::string path)
{
	int modelID = g->mm.AddObject(name);
	CModel* model = g->mm.GetObjectPointer(modelID);

	CFileResource fileResource;
	fileResource.Open(path);

	if (m_modelLoader.LoadModel(path, &fileResource, model))
	{
		m_modelLoader.LoadPhysics(path, &fileResource, model);

		int pos = path.find_last_of("\\");
		path = path.substr(0, pos);
		pos = path.find_last_of("\\");
		path = path.substr(0, pos);
		path += "\\textures\\";

		for (int k = 0; k < model->GetTextureCount() - 1; k++)
		{
			std::string texturePath = path;
			texturePath += model->GetTextureName(k);

			if (model->GetTextureName(k).substr(0, 3) == "CMN")
			{
				texturePath = GetApplicationRootDirectory();
				texturePath += "\\geometry\\common\\textures\\";
				texturePath += model->GetTextureName(k);
			}

			int textureID = g->tm.GetObjectNumber(model->GetTextureName(k));
			if (textureID == -1)
			{
				textureID = g->tm.LoadTextureFromFile(model->GetTextureName(k), texturePath);
				if (textureID == -1)
				{
					textureID = 0;
				}
			}
			model->ChangeTexture(k, textureID);
		}
		
		OptimizeModel(modelID, MODEL_OPTIMIZATION_VERTEX_CACHE_AND_OVERDRAW);
		PrepareOptimalModel(modelID);
	
		return modelID;
	}
	else
	{
		return -1;
	}
}*/

void CModelManager::SwitchVBO(int nModel)
{
	assert((nModel >= 0) && (nModel < GetCount()));

	CModel* model = GetObjectPointer(nModel);

	for (int i = 0; i < model->GetTextureCount() - 1; i++)
	if (model->m_vbos[i])
	{
		model->m_vbos[i]->SetBufferObject(model->m_vbo);
	}
}

void CModelManager::PrepareOptimalModel(int nModel)
{
	assert((nModel >= 0) && (nModel < GetCount()));

	CModel* model = GetObjectPointer(nModel);
	
	MakeVBO(nModel);

	int i;
	for (i = 0; i < model->GetTextureCount() - 1; i++)
	{
		int startOffset = model->GetTextureOffset(i);
		int endOffset;

		if (i + 1 == model->GetTextureCount() - 1)
		{
			endOffset = model->GetFaceCount();	
		}
		else
		{
			endOffset = model->GetTextureOffset(i + 1);
		}

		int count = endOffset - startOffset;
		if (count < 0)
		{
			count = -count;
		}

		if (model->m_vbos[i])
		{
			MP_DELETE(model->m_vbos[i]);
		}

		model->m_vbos[i] = MP_NEW(CVBOSubObject);
		model->m_vbos[i]->SetIndexArray(model->fs, startOffset, count);
		model->m_vbos[i]->SetBufferObject(model->m_vbo);

#ifdef USE_DISPLAY_LISTS
		if ((count < DISPLAY_LIST_MAX_FACES) && (model->GetUpdateType() == UPDATE_TYPE_STATIC))
		{
			std::string name = IntToStr(nModel);
			name += "_mdl";
			name += IntToStr(i);

			int num = g->dlm.CreateDisplayList(name);

			g->dlm.BindDisplayList(num);

			BEGIN_END_DRAW(startOffset * 3, (startOffset + count) * 3);

			g->dlm.UnbindDisplayList();

			model->m_dls[i] = num;
		}
#endif
	}

	/*model->m_noVBO = true;
	for (i = 0; i < model->GetTextureCount() - 1; i++)
	if (model->m_dls[i] == -1)
	{
		model->m_noVBO = false;
	}*/
	
	/*for (int i = 0; i < model->GetTextureCount() - 1; i++)
	{
		int startOffset = model->GetTextureOffset(i);
		int endOffset;

		if (i + 1 == model->GetTextureCount() - 1)
		{
			endOffset = model->GetFaceCount();	
		}
		else
		{
			endOffset = model->GetTextureOffset(i + 1);
		}

		int count = endOffset - startOffset;

		if (count < g->rs.GetInt(MIN_FACES_FOR_VBO))
		{
			std::string name = IntToStr(nModel);
			name += "_mdl";
			name += IntToStr(i);

			int num = g->dlm.CreateDisplayList(name);

			g->dlm.BindDisplayList(num);

			int textureInfo[4];

			for (int j = 0; j < 4; j++)
			{
				textureInfo[j] = 0;
			}

			g->tm.DisableTexturing();
			g->mr.SelectRenderer(STANDARD_RENDERER);
			g->mr.Draw(nModel, model->fs + startOffset * 3, model->vx, model->tx[0], 
				model->vn, &textureInfo[0], count, GL_TRIANGLES);
			g->mr.SelectRenderer(VA_RENDERER);
			g->tm.EnableTexturing();

			g->dlm.UnbindDisplayList();

			model->m_dls[i] = num;
		}
	}*/
}

void CModelManager::OptimizeModel(int /*nModel*/, int /*method*/)
{
#ifdef USING_GEOMETRY_OPTIMIZATION
	if (MODEL_OPTIMIZATION_NONE == method)
	{
		return;
	}

	assert((nModel >= 0) && (nModel < GetCount()));

	CModel* model = GetObjectPointer(nModel);

	if (MODEL_OPTIMIZATION_VERTEX_CACHE_AND_OVERDRAW == method)
	{
		unsigned int* ib = MP_NEW_ARR(unsigned int, model->GetFaceCount() * 3);
		unsigned int* ib2 = MP_NEW_ARR(unsigned int, model->GetFaceCount() * 3);
		
		int k;
		for (k = 0; k < model->GetFaceCount() * 3; k++)
		{	
			ib[k] = model->fs[k];
		}

		int i;
		for (i = 0; i < model->GetTextureCount() - 1; i++)
		{
			int startOffset = model->GetTextureOffset(i);
			int endOffset;

			if (i + 1 == model->GetTextureCount() - 1)
			{
				endOffset = model->GetFaceCount();	
			}
			else
			{
				endOffset = model->GetTextureOffset(i + 1);
			}

			int count = endOffset - startOffset;

			assert(count > 0);

			if (count > 20)
			{
				int res = TootleFastOptimize(model->vx, ib + startOffset * 3, model->GetVertexCount(), count, 12, TOOTLE_DEFAULT_VCACHE_SIZE, TOOTLE_CW, ib2, NULL, TOOTLE_DEFAULT_ALPHA);
				assert(res == TOOTLE_OK);

				if (res == TOOTLE_OK)
				for (k = startOffset * 3; k < (startOffset + count) * 3; k++)
				{
					model->fs[k] = ib2[k - startOffset * 3];
				}

				TootleCleanup();
			}
		}
	}
	else if (MODEL_OPTIMIZATION_VERTEX_CACHE == method)
	{
		SetCacheSize(24);
		SetListsOnly(true);

		PrimitiveGroup* primGroups[200];
	
		int i;
		for (i = 0; i < model->GetTextureCount() - 1; i++)
		{
			int startOffset = model->GetTextureOffset(i);
			int endOffset;

			if (i + 1 == model->GetTextureCount() - 1)
			{
				endOffset = model->GetFaceCount();	
			}
			else
			{
				endOffset = model->GetTextureOffset(i + 1);
			}

			int count = endOffset - startOffset;

			unsigned short numGroups;

			assert(count > 0);

			GenerateStrips(model->fs + startOffset * 3, count * 3, &primGroups[i], &numGroups);

			assert(numGroups == 1);
		}

		for (i = 0; i < model->GetTextureCount() - 1; i++)
		{
			int startOffset = model->GetTextureOffset(i);
		
			for (int k = 0; k < (int)(primGroups[i]->numIndices / 3); k++)
			{
				assert(primGroups[i]->indices[k * 3] >= 0);
				assert(primGroups[i]->indices[k * 3 + 1] >= 0);
				assert(primGroups[i]->indices[k * 3 + 2] >= 0);
				assert(primGroups[i]->indices[k * 3] < model->GetVertexCount());
				assert(primGroups[i]->indices[k * 3 + 1] < model->GetVertexCount());
				assert(primGroups[i]->indices[k * 3 + 2] < model->GetVertexCount());
	
				model->fs[(startOffset + k) * 3 + 0] = primGroups[i]->indices[k * 3];
				model->fs[(startOffset + k) * 3 + 1] = primGroups[i]->indices[k * 3 + 1];
				model->fs[(startOffset + k) * 3 + 2] = primGroups[i]->indices[k * 3 + 2];
			}

			if (i != model->GetTextureCount() - 2)
			{
				assert(model->GetTextureOffset(i + 1) == startOffset + (int)(primGroups[i]->numIndices / 3));
				model->SetTextureOffset(i + 1, startOffset + (int)(primGroups[i]->numIndices / 3));
			}
			else
			{
				int size = startOffset + (int)(primGroups[i]->numIndices / 3);
				assert(model->GetFaceCount() == size);
				model->SetFaceCount(size);
			}

			MP_DELETE_ARR(primGroups[i]);
		}
	}
#endif
}

void CModelManager::ReInitialize()
{
	std::vector<CModel *>::iterator it = GetLiveObjectsBegin();
	std::vector<CModel *>::iterator itEnd = GetLiveObjectsEnd();

	for ( ; it != itEnd; it++)
	{
		for (int k = 0; k < (*it)->GetTextureCount() - 1; k++)
		{
			if ((*it)->m_vbos[k])
				(*it)->m_vbos[k]->UnmarkCorrectness();
		}

		(*it)->OnChanged(0);
	}
}

int CModelManager::ShowModelWithMaterialDetect(int modelID, float d_x, float d_y, float d_z, CRotationPack* rotation, float scaleX, float scaleY, float scaleZ, int mouseX, int mouseY)
{
	g->stp.PushMatrix();

	GLFUNC(glTranslatef)(d_x, d_y, d_z);
		
	//assert(rotation);
	if (rotation)
	{
		rotation->ApplyRotation();
	}

	if ((scaleX != 1.0f) || (scaleY != 1.0f) || (scaleZ != 1.0f))
	{
		GLFUNC(glScalef)(scaleX, scaleY, scaleZ);
	}
	
	CModel* model = GetObjectPointer(modelID);

	g->stp.SetState(ST_CULL_FACE, true);

	if (!model->m_vbo)
	{
		return -1;
	}

	if (!model->m_noVBO)
	{
		model->m_vbo->PreRender();
	}
	CVBOSubObject** vbo = &model->m_vbos[0];

	int modeIndex = g->mr.GetModeIndex();;
	SMaterialParams* params = &model->m_params[0];

	int detectTextureID = -1;
	float pixelZ = -9000.0f, oldPixelZ = -10000.0f;
	GLFUNC(glReadPixels)(GLint(mouseX), GLint(mouseY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &pixelZ);

	if (pixelZ != oldPixelZ)
	{
		detectTextureID = -1;
	}

	oldPixelZ = pixelZ;
	
	// НЕ ПРАВИТЬ WARNING!!! рендеринг съедет нафиг.
	for (int partID = 0; partID < (int)model->m_textureCount - 1; partID++, params++, vbo++)
	if (params->isMaterialSuited[modeIndex])
	{
		g->tm.BindTexture(params->textureID);						
		int materialID = params->materialID;
		if (-1 == materialID)
		{
			materialID = g->rs.GetGlobalMaterialID();
		}
		g->stp.SetMaterial(materialID);

		if ((params->isBillBoardOnCPU) && (!g->rs.GetBool(SHADERS_USING)))
		{
			(*vbo)->UpdateBillboardOrient(model->vx, model->tx[0], model->GetVertexCount(), model->fs, true);

			g->stp.PrepareForRender();
			BEGIN_END_DRAW(0, model->GetFaceCount() * 3);
			
			continue;
		}

#ifndef USE_DISPLAY_LISTS
		switch (params->twoSidedMode)
		{
		case 0:
			(*vbo)->Render();
			break;

		case 1:
			g->stp.SetState(ST_FRONT_FACE, GL_CW);
			(*vbo)->Render();
			g->stp.SetState(ST_FRONT_FACE, GL_CCW);
			(*vbo)->Render();
			break;

		case 2:
		case 3:
			g->stp.SetState(ST_CULL_FACE, false);
			(*vbo)->Render();
			g->stp.SetState(ST_CULL_FACE, true);
			break;
		}
#else
		switch (params->twoSidedMode)
		{
		case 0:
			if (model->m_dls[partID] == -1)
			{
				(*vbo)->Render();
			}
			else
			{
				g->dlm.CallDisplayList(model->m_dls[partID]);
			}
			break;

		case 1:
			g->stp.SetState(ST_FRONT_FACE, GL_CW);
			if (model->m_dls[partID] == -1)
			{
				(*vbo)->Render();
			}
			else
			{
				g->dlm.CallDisplayList(model->m_dls[partID]);
			}
			g->stp.SetState(ST_FRONT_FACE, GL_CCW);
			if (model->m_dls[partID] == -1)
			{
				(*vbo)->Render();
			}
			else
			{
				g->dlm.CallDisplayList(model->m_dls[partID]);
			}
			break;

		case 2:
		case 3:
			g->stp.SetState(ST_CULL_FACE, false);
			if (model->m_dls[partID] == -1)
			{
				(*vbo)->Render();
			}
			else
			{
				g->dlm.CallDisplayList(model->m_dls[partID]);
			}
			g->stp.SetState(ST_CULL_FACE, true);
			break;
		}
#endif

		GLFUNC(glReadPixels)(GLint(mouseX), GLint(mouseY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &pixelZ);

		if (pixelZ != oldPixelZ)
		{
			detectTextureID = params->textureID;
			oldPixelZ = pixelZ;
		}

		g->stp.SetMaterial(-1);
	}	
	
	if (!model->m_noVBO)
	{
		model->m_vbo->PostRender();
	}
	
	g->stp.SetState(ST_CULL_FACE, false);

	g->stp.PopMatrix();

	return detectTextureID;
}

void CModelManager::ShowModelOptimal(int nModel, float d_x, float d_y, float d_z, CRotationPack* rotation, float scaleX, float scaleY, float scaleZ)
{
	g->stp.PushMatrix();

	GLFUNC(glTranslatef)(d_x, d_y, d_z);
		
	//assert(rotation);
	if (rotation)
	{
		if (!rotation->IsEmptyRotation())
		{
			rotation->ApplyRotation();
		}		
	}
	

	if ((scaleX != 1.0f) || (scaleY != 1.0f) || (scaleZ != 1.0f))
	{
		GLFUNC(glScalef)(scaleX, scaleY, scaleZ);
	}
	ShowModelOptimal(nModel);

	g->stp.PopMatrix();
}

void CModelManager::SetInvertCCWFlag(const bool isNeedInvert)
{
	m_isNeedInvertCCW = isNeedInvert;
}

void CModelManager::ShowModelOptimal(int modelID)
{
//	int drawCount = 0;
//
	CModel* model = GetObjectPointer(modelID);

	m_currentRenderingModel = model;

	if (!m_isNeedInvertCCW)
		g->stp.SetState(ST_CULL_FACE, true);

	if (!model->m_vbo)
	{
		return;
	}

	if (!model->m_noVBO)
	{
		model->m_vbo->PreRender();
	}
	CVBOSubObject** vbo = &model->m_vbos[0];

	int modeIndex = g->mr.GetModeIndex();;
	SMaterialParams* params = &model->m_params[0];

	// НЕ ПРАВИТЬ WARNING!!! рендеринг съедет нафиг.
	for (int partID = 0; partID < (int)model->m_textureCount - 1; partID++, params++, vbo++)
	if (params->isMaterialSuited[modeIndex])
	{
		g->tm.BindTexture(params->textureID);						
		int materialID = params->materialID;
		if (-1 == materialID)
		{
			materialID = g->rs.GetGlobalMaterialID();
		}
		g->stp.SetMaterial(materialID);

		if ((params->isBillBoardOnCPU) && (!g->rs.GetBool(SHADERS_USING)))
		{
			(*vbo)->UpdateBillboardOrient(model->vx, model->tx[0], model->GetVertexCount(), model->fs, true);

			g->stp.PrepareForRender();
			BEGIN_END_DRAW(0, model->GetFaceCount() * 3);
			
			continue;
		}

#ifndef USE_DISPLAY_LISTS
		switch (params->twoSidedMode)
		{
		case 0:
			(*vbo)->Render();
//			drawCount++;
			break;

		case 1:
			if (!m_isNeedInvertCCW)
				g->stp.SetState(ST_FRONT_FACE, GL_CW);
			(*vbo)->Render();
//			drawCount++;
			if (!m_isNeedInvertCCW)
				g->stp.SetState(ST_FRONT_FACE, GL_CCW);
			(*vbo)->Render();
//			drawCount++;
			break;

		case 2:
		case 3:
			if (!m_isNeedInvertCCW)
			g->stp.SetState(ST_CULL_FACE, false);
			(*vbo)->Render();
			if (!m_isNeedInvertCCW)
			g->stp.SetState(ST_CULL_FACE, true);
//			drawCount++;
			break;
		}
#else
		switch (params->twoSidedMode)
		{
		case 0:
			if (model->m_dls[partID] == -1)
			{
				(*vbo)->Render();
			}
			else
			{
				g->dlm.CallDisplayList(model->m_dls[partID]);
			}
			break;

		case 1:
			g->stp.SetState(ST_FRONT_FACE, GL_CW);
			if (model->m_dls[partID] == -1)
			{
				(*vbo)->Render();
			}
			else
			{
				g->dlm.CallDisplayList(model->m_dls[partID]);
			}
			g->stp.SetState(ST_FRONT_FACE, GL_CCW);
			if (model->m_dls[partID] == -1)
			{
				(*vbo)->Render();
			}
			else
			{
				g->dlm.CallDisplayList(model->m_dls[partID]);
			}
			break;

		case 2:
		case 3:
			g->stp.SetState(ST_CULL_FACE, false);
			if (model->m_dls[partID] == -1)
			{
				(*vbo)->Render();
			}
			else
			{
				g->dlm.CallDisplayList(model->m_dls[partID]);
			}
			g->stp.SetState(ST_CULL_FACE, true);
			break;
		}
#endif

		g->stp.SetMaterial(-1);
	}	
	
	if (!model->m_noVBO)
	{
		model->m_vbo->PostRender();
	}

	/*static int allDrawCount = 0;
	allDrawCount += drawCount;
	g->lw.WriteLn("model->GetName(): ", model->GetName(), " draw count: ", drawCount, " allDrawCount: ", allDrawCount);*/
	
	if (!m_isNeedInvertCCW)
		g->stp.SetState(ST_CULL_FACE, false);
}

int CModelManager::GetSupportedRenderModes(int modelID)
{
	assert(modelID != -1);
	CModel* model = GetObjectPointer(modelID);
	assert(model);

	int renderMode = MODE_RENDER_ALL;
	bool isSolid = false;
	bool isTransparent = false;
	bool isAlphaTest = false;
	
	for (int i = 0; i < model->GetTextureCount() - 1; i++)
	{
		int materialID = model->GetMaterial(i);

		CTexture* texture = g->tm.GetObjectPointer(model->GetTextureNumber(i));
		assert(texture);

		if (materialID != -1)
		{
			g->mr.SetMode(MODE_RENDER_SOLID);

			if (g->matm.IsMaterialCorrespondsForRenderingMode(texture, materialID))
			{
				isSolid = true;
			}
			else
			{
				g->mr.SetMode(MODE_RENDER_TRANSPARENT);

				if (g->matm.IsMaterialCorrespondsForRenderingMode(texture, materialID))
				{
					isTransparent = true;
				}
				else
				{
					isAlphaTest = true;
				}
			}
		}
		else
		{
			if (texture->IsTransparent())
			{
				isTransparent = true;
			}
			else
			{
				isSolid = true;
			}
		}
	}

	if (isSolid)
	{
		renderMode += MODE_RENDER_SOLID;
	}

	if (isTransparent)
	{
		renderMode += MODE_RENDER_TRANSPARENT;
	}

	if (isAlphaTest)
	{
		renderMode += MODE_RENDER_ALPHATEST;
	}

	return renderMode;
}

int CModelManager::CloneModel(int num)
{
	CModel* mdl;
	mdl = GetObjectPointer(num);

	int num2 = AddObject(std::wstring(mdl->GetName()) + L"_clone");

	mdl = GetObjectPointer(num);

	CModel* mdl2;
	mdl2 = GetObjectPointer(num2);

	mdl2->SetFaceCount(mdl->GetFaceCount());
	mdl2->SetVertexCount(mdl->GetVertexCount());

	int i;
	for (i = 0; i < mdl->GetTextureCount(); i++)
	{
		mdl2->AddTexture(mdl->GetTextureOffset(i), mdl->GetTextureName(i));
	}

	for (i = 0; i < mdl->GetVertexCount(); i++)
	{
		int k;
		for (k = 0; k < 3; k++)
		{
			mdl2->vx[i * 3 + k] = mdl->vx[i * 3 + k];
			mdl2->vn[i * 3 + k] = mdl->vn[i * 3 + k];
		}

		for (k = 0; k < 2; k++)
		{
			mdl2->tx[0][i * 2 + k] = mdl->tx[0][i * 2 + k];
		}
	}

	for (i = 0; i < mdl->GetFaceCount(); i++)
	{
		for (int k = 0; k < 3; k++)
		{
			mdl2->fs[i * 3 + k] = mdl->fs[i * 3 + k];
		}
	}

	for (int k = 0; k < mdl->GetTextureCount(); k++)
	{
		mdl2->SetTextureNumber(k, mdl->GetTextureNumber(k));
	}

	mdl2->SetRColor(mdl->GetRColor());
	mdl2->SetGColor(mdl->GetGColor());
	mdl2->SetBColor(mdl->GetBColor());

	return num2;
}

void CModelManager::MakeVBO(int nModel)
{
	CModel* model = GetObjectPointer(nModel);

	if (model->m_vbo == NULL)
	{
		model->m_vbo = MP_NEW(CBaseBufferObject);
	}

	if (model->GetUpdateType() != UPDATE_TYPE_STATIC)
	{
		model->m_vbo->SetDynamicContentStatus(true);
	}
	// static is the fastest approach even for dynamic models
	model->m_vbo->SetUpdateType(UPDATE_TYPE_STATIC);
	model->m_vbo->SetPrimitiveType(GL_TRIANGLES);
	model->m_vbo->SetVertexArray(model->vx, model->GetVertexCount());
	for (int i = 0; i < 8; i++)
	if (model->tx[i])
	{
		model->m_vbo->SetTextureCoordsArray(i, model->tx[i]);
	}
	model->m_vbo->SetNormalArray(model->vn);
	if (model->tns)
	{
		model->m_vbo->SetTangentsArray(model->tns);
	}
	model->m_vbo->SetFaceArray(model->fs, model->GetFaceCount());
}

void CModelManager::UpdateNotFoundTextures(int modelID)
{
	CModel* model = g->mm.GetObjectPointer(modelID);

	USES_CONVERSION;
	for (int i = 0; i < model->GetTextureCount() - 1; i++)
	{
		CMaterial* material = g->matm.GetObjectPointer(model->GetMaterial(i));

		int textureID;
		for (textureID = 0; textureID < material->GetNotFoundTexturesCount(); textureID++)
		{
			int _textureID = LinkTextureToModel(material->GetNotFoundTextureName(textureID), W2A(model->GetName()), true, NULL, material->GetNotFoundTexturePackedStatus(textureID), material->IsNotFoundTextureNormalMap(textureID));
			material->UpdateNotFoundTexture(textureID, _textureID);
		}
	}	
}

void CModelManager::AddPropertyBlockLoadHandler(const std::string& fileName, IPropertyBlockLoadHandler* const propertyBlockLoadHandler)
{
	m_propertyBlockLoadHandlerList.insert(MP_MAP<MP_STRING, IPropertyBlockLoadHandler *>::value_type(MAKE_MP_STRING(fileName), propertyBlockLoadHandler));
}

void CModelManager::RemovePropertyBlockLoadHandler(const std::string& fileName)
{
	MP_MAP<MP_STRING, IPropertyBlockLoadHandler *>::iterator it = m_propertyBlockLoadHandlerList.find(MAKE_MP_STRING(fileName));
	if (it != m_propertyBlockLoadHandlerList.end())
	{
		m_propertyBlockLoadHandlerList.erase(it);
	}
}

CModelManager::~CModelManager()
{
	std::vector<SSaveLoadingModelTaskInfo *>::iterator it = m_loadingModelTaskInfo.begin();
	for ( ; it != m_loadingModelTaskInfo.end(); it++)
	{
		MP_DELETE(*it);
	}
}