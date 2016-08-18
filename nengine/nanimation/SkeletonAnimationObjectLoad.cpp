
#include "StdAfx.h"
#include "PlatformDependent.h"
#include "GlobalSingletonStorage.h"
#include "SkeletonAnimationObject.h"
#include "LoadingSAOTask.h"
#include "Cal3DModelAccessObject.h"

#include "cal3d\animation_action.h"
#include "cal3d\animation_cycle.h"
#include "cal3d\CoreMaterial.h"
#include "cal3d\CoreSubmesh.h"
#include "cal3d\Submesh.h"
#include "cal3d\Skeleton.h"
#include "cal3d\Renderer.h"
#include "cal3d\Mixer.h"
#include "cal3d\Mesh.h"
#include "cal3d\Bone.h"

#define LOAD_ASYNCH 1

void CSkeletonAnimationObject::OnChanged(int /*eventID*/) 
{
	OnAfterChanges();
}

void CSkeletonAnimationObject::SetLoadedStatus(bool isLoaded)
{
	m_isLoaded = isLoaded;

	if (m_isLoaded)
	if (m_lookAtPos != CVector3D(0.0f, 0.0f, 0.0f))
	{
		LookAt(m_lookAtPos, 0, 0);
		m_lookAtPos.Set(0, 0, 0);
	}
}

bool CSkeletonAnimationObject::IsLoaded()const
{
	return m_isLoaded;
}

void CSkeletonAnimationObject::LoadAsynch(const std::string& path, const bool isBlend, CSkeletonAnimationObject* const parent)
{
	CLoadingSAOTask* task = MP_NEW(CLoadingSAOTask);
	task->SetParams(this, path, isBlend, parent);
	
	g->taskm.AddTask(task, MAY_BE_MULTITHREADED, PRIORITY_HIGH, false);
	g->skom.AddAsynchLoadingTask(task);
}

bool CSkeletonAnimationObject::Load(const std::string& path, const bool isBlend)
{
	return Load(path, isBlend, NULL);
}

bool CSkeletonAnimationObject::Load(const std::string& path, const bool isBlend, CSkeletonAnimationObject* const parent)
{
	// in main thread

	STransitionalInfo* info = MP_NEW(STransitionalInfo);//new STransitionalInfo();

	__int64 t1 = g->tp.GetTickCount();

	if (!LoadCal3DObjects(path, info, isBlend, parent))
	{
		return false;
	}	

	LoadEngineObjects(parent, info);

	MP_DELETE(info);

	__int64 t3 = g->tp.GetTickCount();
	stat_allCalTime += (int)(t3 - t1);
	SetLoadedStatus(true);

	return true;
}

void  CSkeletonAnimationObject::StartCal3DModelAccess()
{
	EnterCriticalSection(&m_calModelAccessCS);			
}

void  CSkeletonAnimationObject::EndCal3DModelAccess()
{
	LeaveCriticalSection(&m_calModelAccessCS);			
}

bool  CSkeletonAnimationObject::LoadCal3DObjects(const std::string& path, STransitionalInfo* info, const bool isBlend, CSkeletonAnimationObject* const parent, void* data, unsigned int size)
{	
	if (!IsPathCorrect(path))
	{
		return false;
	}

	CCal3DModelAccessObject accessObj(this);

	m_parent = parent;
	if (parent)
	{
		m_parentObj = parent->GetObject3D();
	}
	else
	{
		m_parentObj = NULL;
	}

	m_path = path;
	m_isBlend = isBlend;

	if (!m_pathObject.Parse(m_path, data, size))
	{
		std::string newPath = m_path;
		StringReplace(newPath, ".xml", ".pck");
		bool error = false;
		if (newPath != m_path)
		{
			m_path = newPath;
			if (!m_pathObject.Parse(m_path, data, size))
			{
				error = true;
			}
		}
		else
		{
			error = true;
		}

		if (error)
		{
			g->lw.WriteLn("[ERROR] failed to load due to unknown extension or missing file: ", path);
			return false;
		}
	}


	MP_NEW_P(doc, TiXmlDocument, path.c_str());	
	if (!LoadXML(doc, info))
	{
		return false;
	}

	TiXmlNode* object = doc->FirstChild("object");

	if ((object) && (object->ToElement()->Attribute("inverse")))
	{
		m_isUsualOrder = false;
	}

	m_calCoreModel = MP_NEW(CalCoreModel);
	USES_CONVERSION;
	if (!m_calCoreModel->create( W2A(GetName())))
	{
		return false;
	}

	if ((!m_parent) && (!m_isBlend))
	if (!LoadSkeleton(object))
	{
		return false;
	}

	if (!LoadMesh())
	{
		return false;
	}

	if (!LoadMaterials(object, info->singleMaterials, info->baseMaterial))
	{
		return false;
	}	

	m_calModel = MP_NEW(CalModel);
	if (!m_calModel->create(m_calCoreModel))
	{
		g->lw.WriteLn("[LOADWARNING] Error creating CAL3D model when ", path, " loading...");
		return false;
	}

	for(int meshId = 0; meshId < m_calCoreModel->getCoreMeshCount(); meshId++)
	{
		m_calModel->attachMesh(meshId);
	}

	m_calModel->setMaterialSet(0);

	CalculateTangents();

	//save blends in info

	TiXmlNode* blends = object->FirstChild("blends");

	if (blends)
	{
		TiXmlNode* blend = blends->FirstChild("blend");

		while (blend != NULL)
		{
			if (!blend->ToElement()->Attribute("src"))
			{
				g->lw.WriteLn("[LOADWARNING] Loading skeleton model failed... One of blends has empty src!");
			}

			if (!blend->ToElement()->Attribute("target"))
			{
				g->lw.WriteLn("[LOADWARNING] Loading skeleton model failed... One of blends has empty target!");
			}

			std::string src = m_dir;
			src += blend->ToElement()->Attribute("src");
			std::string target = blend->ToElement()->Attribute("target");
			std::string param = "";
			if (blend->ToElement()->Attribute("param"))
			{
				param = blend->ToElement()->Attribute("param");
			}

			MP_NEW_P3(bld, SBlends, src, param, target);
			info->blends.push_back(bld);

			blend = blends->IterateChildren(blend);		
		}
	}

	m_xml = doc;

	if (path.find("body") == -1)
	{
		std::vector<SBlends *>::iterator it = info->blends.begin();
		std::vector<SBlends *>::iterator itEnd = info->blends.end();

		for ( ; it != itEnd; it++)
		{
			CSkeletonAnimationObject* sao = MP_NEW(CSkeletonAnimationObject);
			if (!sao->Load((*it)->src, true, NULL))
			{
				MP_DELETE(sao);
				sao = NULL;
			}
			(*it)->sao = sao;
		}
	}

	if (!m_isBlend)
	{
		m_model = MP_NEW(CModel);
	}

	LoadMaterialFiles(info->singleMaterials, info->baseMaterial, info->materialFiles);	

	if (!m_isBlend)
	{		
		m_model->SetVertexCount(m_vertexCount);
		m_model->SetFaceCount(m_faceCount);
		
		m_model->SetUpdateType(UPDATE_TYPE_DYNAMIC_VERTEX_NORMAL);

		if (g->rs.IsNotOnlyLowQualitySupported())
			m_model->tns = MP_NEW_ARR(float, m_vertexCount * 4);
	}

	// ƒелаем дл€ блендов: чтобы брать оттуда вершины дл€ блендинга
	// дл€ неблендов: чтобы восстанавливать коорд-ты вершин до блендинга
	{
		m_savedVertices = MP_NEW_ARR(float, m_vertexCount * 3);
		float* currentVertex = m_savedVertices;		
		
		for (int meshID = 0; meshID < m_calCoreModel->getCoreMeshCount(); meshID++)
		{
			CalCoreMesh* mesh = m_calCoreModel->getCoreMesh(meshID);

			int submeshCount = mesh->getCoreSubmeshCount();

			for (int submeshID = 0; submeshID < submeshCount; submeshID++)
			{
				CalCoreSubmesh* submesh = mesh->getCoreSubmesh(submeshID);
				std::vector<CalCoreSubmesh::Vertex>& vx = submesh->getVectorVertex();
			
				std::vector<CalCoreSubmesh::Vertex>::iterator iter = vx.begin();
				std::vector<CalCoreSubmesh::Vertex>::iterator iterEnd = vx.end();

				for ( ; iter != iterEnd; iter++)
		        {
					*currentVertex = (*iter).position.x;
					currentVertex++;
					*currentVertex = (*iter).position.y;
					currentVertex++;
					*currentVertex = (*iter).position.z;
					currentVertex++;
				}
			}
		}
	}
		
	return true;
}

void CSkeletonAnimationObject::LoadEngineObjects(CSkeletonAnimationObject* const parent, STransitionalInfo* info)
{
	__int64 t1 = g->tp.GetTickCount();

	if ((!m_isBlend) && (m_model))
	{
		std::string name = "animation_model_";
		name += IntToStr(g->mm.GetCount());
		USES_CONVERSION;
		m_model->SetName( A2W(name.c_str()));
		g->mm.ManageObject(m_model);
		m_modelID = m_model->GetID();
	}
	
	LoadEngineMaterials(info->materialFiles);

	if (parent)
	{
		parent->BindToSkeleton(this);
		
		for (int i = 0; i < CLIPPING_VOLUME_COUNT; i++)
		if (info->clipboxs[i] != NULL)
		{
			parent->SetClipBox(	info->clipboxs[i]->type, info->clipboxs[i]->valueID, info->clipboxs[i]->valueOrt,  info->clipboxs[i]->valuePlaneStart,  info->clipboxs[i]->valuePlaneEnd, 
				info->clipboxs[i]->valueOrt2, info->clipboxs[i]->valuePlaneStart2, info->clipboxs[i]->valuePlaneEnd2,
				info->clipboxs[i]->valueOrt3, info->clipboxs[i]->valuePlaneStart3, info->clipboxs[i]->valuePlaneEnd3);
		}		
	}	

	Build3DModel();	

	if (info->blends.size() != 0)
	{
		LoadBlends(&info->blends);
	}	

	if (m_model)
	{
		m_model->SetLoadingState(MODEL_LOADED, 0);
		m_model->OnChanged(0);
	}	

	if (m_isBlend)
	{
		LoadMask();

		unsigned char* mask = m_mask;
		assert(m_mask);
		unsigned int maskSize = m_maskSize;
		assert(m_maskSize > 0);
		float* savedVertices = m_savedVertices;
		assert(savedVertices);
		SBlendOptimizationInfo* blendOptimizationInfo = m_blendOptimizationInfo;		

		m_savedVertices = NULL;
		m_maskSize = 0;
		m_mask = NULL;
		m_blendOptimizationInfo = NULL;

		FreeResources();

		m_blendOptimizationInfo = blendOptimizationInfo;
		m_savedVertices = savedVertices;
		m_mask = mask;
		m_maskSize = maskSize;
	}	

	SetParent(parent);	

	m_pathObject.ClearAll();

	__int64 t8 = g->tp.GetTickCount();
	stat_allCalTime += (int)(t8 - t1);
}

bool CSkeletonAnimationObject::LoadXML(TiXmlDocument* doc, STransitionalInfo* info)
{
	bool isLoaded = false;	

	if (!isLoaded)
	{
		char* data;
		int size;
		bool isNeedToDelete;
		if (m_pathObject.GetXmlData(data, size, isNeedToDelete))
		{
			doc->Parse((const char*)data);
			isLoaded = true;
			if (isNeedToDelete)
			{
				MP_DELETE_ARR(data);
			}
		}
	}

	if (!isLoaded)
	{
		g->lw.WriteLn("[LOADWARNING] Loading skeleton model ", m_path, " failed...");
		return false;
	}

	TiXmlNode* object = doc->FirstChild("object");


	if (!object)
	{
		g->lw.WriteLn("[LOADWARNING] Loading skeleton model ", m_path, " failed... XML description is empty");
		return false;
	}

	m_compatabilityID = 0;
	if (object->ToElement()->Attribute("id"))
	{
		m_compatabilityID = rtl_atoi(object->ToElement()->Attribute("id"));		
	}

	if (object->ToElement()->Attribute("mid"))
	{
		m_motionCompatabilityID = rtl_atoi(object->ToElement()->Attribute("mid"));		
	}
	else
	{
		m_motionCompatabilityID = m_compatabilityID;
	}

	if (object->ToElement()->Attribute("z_offset"))
	{
		m_zOffset = rtl_atoi(object->ToElement()->Attribute("z_offset"));		
	}

	m_baseTexturesPath = "";
	if (object->ToElement()->Attribute("texturespath"))
	{
		m_baseTexturesPath = object->ToElement()->Attribute("texturespath");
	}

	m_dir = "";
	int pos = m_path.find_last_of("\\");
	int pos2 = m_path.find_last_of("/");
	if (pos2 > pos)
	{
		pos = pos2;
	}

	if (pos != -1)
	{
		m_dir = m_path.substr(0, pos + 1);
	}

	TiXmlNode* clipboxs = object->FirstChild("clipboxs");

	if (clipboxs)
	{
		TiXmlNode* clipbox = clipboxs->FirstChild("clipbox");

		while (clipbox != NULL)
		{

			std::string valueID = clipbox->ToElement()->Attribute("id");

			m_clipboxsId.push_back(rtl_atoi(valueID.c_str()) -1 );

			std::string valueOrt = clipbox->ToElement()->Attribute("ort");
			std::string valuePlaneStart = clipbox->ToElement()->Attribute("start");
			std::string valuePlaneEnd = clipbox->ToElement()->Attribute("end");

			std::string valueOrt2 = clipbox->ToElement()->Attribute("ort2");
			std::string valuePlaneStart2 = clipbox->ToElement()->Attribute("start2");
			std::string valuePlaneEnd2 = clipbox->ToElement()->Attribute("end2");

			std::string valueOrt3 = "-1";
			std::string valuePlaneStart3 = "";
			std::string valuePlaneEnd3 = "";

			if (clipbox->ToElement()->Attribute("ort3"))
			{
				valueOrt3 = clipbox->ToElement()->Attribute("ort3");
				valuePlaneStart3 = clipbox->ToElement()->Attribute("start3");
				valuePlaneEnd3 = clipbox->ToElement()->Attribute("end3");
			}

			int id = rtl_atoi(valueID.c_str());

			info->clipboxs[id] = MP_NEW(SClipboxs);//new SClipboxs();
			info->clipboxs[id]->type = (valueID == "9") ? CLIPPING_VOLUME_TYPE_SPHERE : CLIPPING_VOLUME_TYPE_BOX;
			info->clipboxs[id]->valueID = valueID;
			info->clipboxs[id]->valueOrt = valueOrt;
			info->clipboxs[id]->valueOrt2 = valueOrt2;
			info->clipboxs[id]->valueOrt3 = valueOrt3;
			info->clipboxs[id]->valuePlaneEnd = valuePlaneEnd;
			info->clipboxs[id]->valuePlaneEnd2 = valuePlaneEnd2;
			info->clipboxs[id]->valuePlaneEnd3 = valuePlaneEnd3;
			info->clipboxs[id]->valuePlaneStart = valuePlaneStart;
			info->clipboxs[id]->valuePlaneStart2 = valuePlaneStart2;
			info->clipboxs[id]->valuePlaneStart3 = valuePlaneStart3;

			/*if (parent)
			{
			parent->SetClipBox((valueID == "9") ? CLIPPING_VOLUME_TYPE_SPHERE : CLIPPING_VOLUME_TYPE_BOX,
			valueID,  valueOrt,  valuePlaneStart,  valuePlaneEnd, valueOrt2, valuePlaneStart2, valuePlaneEnd2, valueOrt3, valuePlaneStart3, valuePlaneEnd3);
			}*/

			clipbox = clipboxs->IterateChildren(clipbox);		
		}
	}
	return true;
}


bool CSkeletonAnimationObject::LoadSkeleton(TiXmlNode* const object)
{
	TiXmlNode* skeleton = object->FirstChild("skeleton");

	std::string skeletonPath = m_dir;
	if (skeleton)
	{
		skeletonPath += skeleton->ToElement()->Attribute("src");
	}
	
	/*ifile* file = g->rl->GetResMemFile(skeletonPath.c_str());
	if (file)
	{
		int size = file->get_file_size();
		unsigned char* data = MP_NEW_ARR(unsigned char, size);//new unsigned char[size];
		file->read(data, size);
		g->rl->DeleteResFile(file);
		
		if (!m_calCoreModel->loadCoreSkeleton(data, size))
		{
			MP_DELETE_ARR(data);//delete[] data;

			g->lw.WriteLn("[LOADWARNING] Loading skeleton model ", m_path, " failed... Skeleton ", skeletonPath, " loading failed!");
			return false;				
		}

		MP_DELETE_ARR(data);//delete[] data;
	}*/
	char* data;
	int size;
	bool isNeedToDelete;
	if (m_pathObject.GetSkeletonData(data, size, isNeedToDelete, skeletonPath))
	{
		if (!m_calCoreModel->loadCoreSkeleton(data, size))
		{
			if (isNeedToDelete)
			{
				MP_DELETE_ARR(data);
			}

			g->lw.WriteLn("[LOADWARNING] Loading skeleton model ", m_path, " failed... Skeleton ", skeletonPath, " loading failed!");
			return false;				
		}

		MP_DELETE_ARR(data);
	}
	else
	{
		g->lw.WriteLn("[LOADWARNING] Loading skeleton model ", m_path, " failed... Skeleton ", skeletonPath, " loading failed!");
		return false;
	}

	return true;
}

bool CSkeletonAnimationObject::LoadMesh()
{
	char* data;
	int size;
	bool isNeedToDelete;
	if (m_pathObject.GetMeshData(data, size, isNeedToDelete))
	{
		if (m_calCoreModel->loadCoreMesh(data, size) == -1)
		{
			if (isNeedToDelete)
			{
				MP_DELETE_ARR(data);
			}

			g->lw.WriteLn("[LOADWARNING] Loading skeleton model ", m_path, " failed... Mesh loading failed!");
			return false;
		}

		if (isNeedToDelete)
		{
			MP_DELETE_ARR(data);
		}
	}
	else
	{
		g->lw.WriteLn("[LOADWARNING] Loading skeleton model ", m_path, " failed... Mesh loading failed!");
		return false;
	}

	return true;
}

bool CSkeletonAnimationObject::LoadMaterials(TiXmlNode* const object,
											 MP_VECTOR<MP_STRING>& singleMaterials,
											 std::string& baseMaterial)
{
	std::string baseMaterialPath = "";
	if (object->ToElement()->Attribute("matpath"))
	{
		baseMaterialPath = object->ToElement()->Attribute("matpath");
	}
	
	TiXmlNode* materials = object->FirstChild("materials");

	if (!m_isBlend)
	{
		if (!materials)
		{
			g->lw.WriteLn("[LOADWARNING] Loading skeleton model ", m_path, " failed... Materials descriptions not found!");
			return false;
		}

		if (materials->ToElement()->Attribute("base"))
		{
			baseMaterial = m_dir;
			baseMaterial += materials->ToElement()->Attribute("base");
		}
		
		TiXmlNode* material = materials->FirstChild("material");

		while (material != NULL)
		{
			std::string path = "";
			// берутс€ ли  отдельные текстуры одежды из папки другой одежды
			if (material->ToElement()->Attribute("path"))
			{
				path = material->ToElement()->Attribute("path");
			}

			if (material->ToElement()->Attribute("base"))
			{
				std::string singleMaterial = m_dir;
				singleMaterial +=material->ToElement()->Attribute("base");
				singleMaterials.push_back(MAKE_MP_STRING(singleMaterial));
				if (path!="")
					m_texturesPath.insert(std::map<MP_STRING, MP_STRING>::value_type(MAKE_MP_STRING(material->ToElement()->Attribute("base")), MAKE_MP_STRING(path)));
			}
			else
			{
				singleMaterials.push_back(MAKE_MP_STRING(""));
			}
			
			if (material->ToElement()->Attribute("src")) 
			{				
				std::string matPath = m_dir + baseMaterialPath;
				matPath += material->ToElement()->Attribute("src");
			
				ifile* file = g->rl->GetResMemFile(matPath.c_str());
				if (file)
				{
					int size = file->get_file_size();
					unsigned char* data = MP_NEW_ARR(unsigned char, size);//new unsigned char[size];
					file->read(data, size);
					g->rl->DeleteResFile(file);

					if (m_calCoreModel->loadCoreMaterial(data, size) == -1)
					{
						MP_DELETE_ARR(data);//delete[] data;

						g->lw.WriteLn("[LOADWARNING] Loading  model ", m_path, " failed... Material ", matPath, " loading failed!");
						return false;
					}
			
					MP_DELETE_ARR(data);//delete[] data;
				}
				else
				{
					g->lw.WriteLn("[LOADWARNING] Loading  model ", m_path, " failed... Material ", matPath, " loading failed!");
					return false;
				}
			}
			else if (material->ToElement()->Attribute("texture"))
			{
				std::string texture = material->ToElement()->Attribute("texture");
				if (path!="")
					m_texturesPath.insert(std::map<MP_STRING, MP_STRING>::value_type(MAKE_MP_STRING(texture), MAKE_MP_STRING(path)));
				m_calCoreModel->loadFakeMaterial(material->ToElement()->Attribute("texture"));
			}
			else
			{
				g->lw.WriteLn("[LOADWARNING] Loading skeleton model ", m_path, " failed... One of material descriptions not found!");
				return false;
			}
						
			material = materials->IterateChildren(material);		
		}

		int materialId;
		for(materialId = 0; materialId < m_calCoreModel->getCoreMaterialCount(); materialId++)
		{
			// create the a material thread
			m_calCoreModel->createCoreMaterialThread(materialId);

			// initialize the material thread
			m_calCoreModel->setCoreMaterialId(materialId, 0, materialId);
		}
	}

	return true;
}

void CSkeletonAnimationObject::Build3DModel()
{
	m_isInitialized = false;

	if (m_model)
	{
		if (!m_isBlend)
		{	
			m_model->AddTexture(0, "");
			m_model->AddMaterial("-1");		
	
			SetFreeze(m_isFrozen);		
			m_isInitialized = false;

			BeginAsynchUpdate(NULL);
			Update(0, 0);
			EndAsynchUpdate();

			LinkTextures(m_dir + m_baseTexturesPath);		

			g->mm.PrepareOptimalModel(m_model->GetID());
			m_vbo[m_currentVBONum] = m_model->m_vbo;

			for (int bufferID = 1; bufferID < BUFFER_COUNT; bufferID++)
			{
				m_currentVBONum = (m_currentVBONum + 1)%BUFFER_COUNT;
				m_model->m_vbo = NULL;
				g->mm.MakeVBO(m_model->GetID());
				m_vbo[m_currentVBONum] = m_model->m_vbo;
			}

			g->mm.SwitchVBO(m_model->GetID());
			m_model->SetFaceOrient(GL_CCW);
		}

		std::string newName = "animation_model_" + m_path;
		newName += "_";
		newName += IntToStr(g->mm.GetCount());
		g->mm.RenameObject(m_model->GetID(), newName);
	}
}

bool CSkeletonAnimationObject::IsTextureExists(const std::string& path)
{
	return g->rl->GetAsynchResMan()->IsLocalFileExists( path.c_str());
}

void CSkeletonAnimationObject::LinkTextures(const std::string& path)
{
	std::string newPath = CorrectPath(path);
	std::string dir = m_dir;
	if (dir.find(":geometry") != -1)
	{
		StringReplace(dir, ":geometry/", "ui:resources/");
		StringReplace(dir, ":geometry\\", "ui:resources\\");
		StringReplace(newPath, ":geometry/", "ui:resources/");
		StringReplace(newPath, ":geometry\\", "ui:resources\\");
	}

	for (int k = 0; k < m_model->GetTextureCount() - 1; k++)
	{
		std::string texturePath = "";

		std::map<MP_STRING, MP_STRING>::iterator it = m_texturesPath.find(MAKE_MP_STRING(m_model->GetTextureName(k)));

		if ( it!=m_texturesPath.end())
		{			
			texturePath = CorrectPath(dir + (*it).second);
		}
		else
		{
			texturePath = newPath.c_str();
		}

		int pos = texturePath.find_last_of("\\");
		if (pos != -1)
		{
			texturePath = texturePath.substr(0, pos);
			texturePath += "\\";
		}
		
		texturePath += m_model->GetTextureName(k).c_str();

		std::string textureName;
		if (m_parent)
		{
			textureName = texturePath;
		}
		else
		{
			textureName = m_model->GetTextureName(k).c_str();
		}

		int textureID = g->tm.GetObjectNumber( textureName);
		if (textureID == -1)
		{
			std::vector<void*> owners;
			if (m_obj3d)
			{
				owners.push_back(m_obj3d);
			}
			else if (m_parent)
			{
				owners.push_back(m_parent->m_obj3d);
			}

			if (IsTextureExists(texturePath))
			{			
				textureID = g->tm.LoadAsynch( textureName, texturePath, &owners);
				//textureID = g->tm.LoadFromIFile();
			}
			else
			{
				StringReplace(texturePath, "ui:resources\\", ":geometry\\");
				StringReplace(texturePath, "ui:resources/", ":geometry/");
				textureID = g->tm.LoadAsynch( textureName, texturePath, &owners);
				m_isContainsExternalTextures = true;
			}
		}
		
		m_model->ChangeTexture(k, textureID);
	}

	m_model->OnAfterChanges();
}

int CSkeletonAnimationObject::LinkTexture(const std::string& _texturePath, const std::string& path, const bool canBeCompressed)
{
	std::string texturePath = path;
	if (texturePath.find(":geometry") != -1)
	{
		StringReplace(texturePath, ":geometry/", "ui:resources/");
		StringReplace(texturePath, ":geometry\\", "ui:resources\\");		
	}
	
	int pos = texturePath.find_last_of("\\");
	texturePath = texturePath.substr(0, pos);
	texturePath += "\\";
	
	texturePath += _texturePath;

	texturePath = CorrectPath(texturePath);

	int textureID = -1;
	std::string textureName = _texturePath;
	if (!textureName.empty())
	{
		textureID = g->tm.GetObjectNumber(textureName);
		if (textureID == -1)
		{
			std::vector<void*> owners;
			if (m_obj3d)
			{
				owners.push_back(m_obj3d);
			}
			else if (m_parent)
			{
				owners.push_back(m_parent->m_obj3d);
			}

			std::string name = _texturePath;
			if (IsTextureExists(texturePath))
			{
				textureID = g->tm.LoadAsynch(name, texturePath, &owners);
			}
			else
			{
				StringReplace(texturePath, "ui:resources\\", ":geometry\\");
				StringReplace(texturePath, "ui:resources/", ":geometry/");
				textureID = g->tm.LoadAsynch(name, texturePath, &owners);
				m_isContainsExternalTextures = true;
			}
			CTexture* texture = g->tm.GetObjectPointer(textureID);
			texture->GetLoadingInfo()->SetCanBeCompressed(canBeCompressed);
		}
	}

	return textureID;
}

void CSkeletonAnimationObject::LoadMaterialFiles(MP_VECTOR<MP_STRING>& singleMaterials,
												   const std::string& baseMaterial,
												   std::vector<SMaterialFile *>& materialFiles)
{
	MP_VECTOR<MP_STRING>::iterator baseMaterialIt = singleMaterials.begin();
	MP_VECTOR<MP_STRING>::iterator baseMaterialItEnd = singleMaterials.end();

	m_faceCount = 0;
	m_vertexCount = 0;

	int matIndex = 0;
	int maxUsedMaterial = (baseMaterial.empty()) ? 0 : 1;
	
	ifile* file = NULL;
	std::string oldMatPath = "";
	for (int i = 0; i < m_calCoreModel->getCoreMeshCount(); i++)
	{
		CalCoreMesh* mesh = m_calCoreModel->getCoreMesh(i);

		int submeshCount = mesh->getCoreSubmeshCount();
		if (submeshCount != singleMaterials.size())
			g->lw.WriteLn("[LOADWARNING] Loading skeleton model failed... LoadMaterialFiles - submeshCount != singleMaterials.size() memory damage and may be crash");
		for (int k = 0; k < submeshCount; k++)
		{
			CalCoreSubmesh* submesh = mesh->getCoreSubmesh(k);
			int materialID = submesh->getCoreMaterialThreadId();
			CalCoreMaterial* mat = m_calCoreModel->getCoreMaterial(materialID);

			if (mat != NULL)
			{
				m_model->AddTexture(m_faceCount, mat->getMapFilename(0));
			}

			if ((baseMaterial.empty()) && ((singleMaterials.size() == 0) || ((*baseMaterialIt) == "")))
			{
				MP_NEW_P4(materialFile, SMaterialFile, "-1", oldMatPath, NULL, false);
				materialFiles.push_back(materialFile);			
			}
			else
			{
				std::string newName = m_path;
				newName += IntToStr((int)this);
				newName += "_";
				newName += IntToStr(i);
				newName += "_";
				newName += IntToStr(k);

				std::string matPath = "";
				if (!((*baseMaterialIt) == ""))
				{
					matPath = *baseMaterialIt;
					matIndex = maxUsedMaterial;
					maxUsedMaterial++;
				}
				else
				{
					matPath = baseMaterial;
					matIndex = 0;
				}

				if (oldMatPath != matPath)
				{
					/*if (file)
					{
						g->rl->DeleteResFile(file);
					}

					file = g->rl->GetResMemFile(matPath.c_str());*/
					
					oldMatPath = matPath;
				}
				else
				{
					//file->seek_to_begin();
				}
				
				/*if (file)
				{					
					int size = file->get_file_size();
					char* data = MP_NEW_ARR(char, size+1);//new char[size + 1];
					data[size] = 0;
					file->read((BYTE *)data, size);
					MP_NEW_P3(materialFile, SMaterialFile, newName, matPath, data);
					materialFiles.push_back(materialFile);								
				}*/


				char* data;
				int size;
				bool isNeedToDelete;
				if (m_pathObject.GetMaterialData(matPath, matIndex, data, size, isNeedToDelete))
				{
					MP_NEW_P4(materialFile, SMaterialFile, newName, matPath, data, isNeedToDelete);
					materialFiles.push_back(materialFile);
				}
				else
				{
					MP_NEW_P4(materialFile, SMaterialFile, "-1", matPath, NULL, isNeedToDelete);
					materialFiles.push_back(materialFile);	
				}
			}

			m_vertexCount += submesh->getVertexCount();
			m_faceCount += submesh->getFaceCount();	
			// Vladimir
			if ( baseMaterialIt != baseMaterialItEnd)
				baseMaterialIt++;			
		}
	}

	if (file)
	{
		g->rl->DeleteResFile(file);
	}
}

void CSkeletonAnimationObject::LoadEngineMaterials(std::vector<SMaterialFile *>& materialFiles)
{
	//int dt = 0;
	//int t1 = g->tp.GetTickCount();

	std::vector<CMaterial *> engineMaterials;
	engineMaterials.reserve(100);
	
	std::vector<SMaterialFile *>::iterator mIt = materialFiles.begin();
	std::vector<SMaterialFile *>::iterator mItEnd = materialFiles.end();
	for (int i = 0; i < m_calCoreModel->getCoreMeshCount(); i++)
	{
		CalCoreMesh* mesh = m_calCoreModel->getCoreMesh(i);

		int submeshCount = mesh->getCoreSubmeshCount();

		for (int k = 0; k < submeshCount; k++)
		{
			// Vladimir
			if ( mIt == mItEnd)
				break;
			if ((*mIt)->data)
			{
				USES_CONVERSION;
				CMaterial* mat = g->matm.LoadUniqueMaterialFromMemory((*mIt)->name, (*mIt)->path, (*mIt)->data, true);
				engineMaterials.push_back(mat);
				if ((*mIt)->isNeedToDelete)
				{
					MP_DELETE_ARR((*mIt)->data);
				}
			}

			if (m_model)
			{
				m_model->AddMaterial((*mIt)->name);
			}
							
			MP_DELETE(*mIt);//delete (*mIt);
			mIt++;
		}
	}

	materialFiles.clear();

	//int t2 = g->tp.GetTickCount();

	std::vector<CMaterial *>::iterator matIt = engineMaterials.begin();
	std::vector<CMaterial *>::iterator matItEnd = engineMaterials.end();

	for ( ; matIt != matItEnd; matIt++)
	{
		int textureID;
		for (textureID = 0; textureID < (*matIt)->GetNotFoundTexturesCount(); textureID++)
		{
			std::string path = m_dir;
			std::string textureName =(*matIt)->GetNotFoundTextureName(textureID);
			bool isFound = false;

			std::string materialName = "";
			int id = 0;
			(*matIt)->GetSource(id, materialName);

			int pos = materialName.find_last_of("\\");
			if (pos != -1)
			{
				materialName = materialName.substr(pos+1, materialName.size());
			}

			std::map<MP_STRING, MP_STRING>::iterator it = m_texturesPath.find(MAKE_MP_STRING(materialName));

			if (it != m_texturesPath.end())
			{
				isFound = true;
				path += (*it).second;
			}

			if (!isFound)
				path += m_baseTexturesPath;			
			
			int _textureID = LinkTexture(textureName, path, (*matIt)->GetNotFoundTexturePackedStatus(textureID));						
			(*matIt)->UpdateNotFoundTexture(textureID, _textureID);			
		}
	}	

	//int t3 = g->tp.GetTickCount();

	//g->lw.WriteLn("LoadEngineMaterials: ", t2 - t1, " ", t3 - t2, " ", dt);
}

void CSkeletonAnimationObject::CalculateTangents()
{
	if (!m_isBlend)
	{
		int meshCount = m_calCoreModel->getCoreMeshCount();
		for (int i = 0; i < meshCount; i++)
		{
			CalMesh* mesh = m_calModel->getMesh(i);

			int submeshCount = mesh->getSubmeshCount();

			for (int k = 0; k < submeshCount; k++)
			{
				CalSubmesh* submesh = mesh->getSubmesh(k);

				submesh->enableTangents(0, true);
			}
		}
	}
}

bool CSkeletonAnimationObject::IsPathCorrect(const std::string& path)const
{
	if (path.size() < 3)
	{
		return false;
	}

	std::string ext = path.substr(path.size() - 3, 3);
	if (StringToLower(ext) != "xml")
	if (StringToLower(ext) != "pck")
	{
		return false;
	}

	return true;
}

void CSkeletonAnimationObject::LoadBlends(std::vector<SBlends*>* blends)
{
	std::vector<SBlends*>::iterator it = blends->begin();
	std::vector<SBlends*>::iterator itEnd = blends->end();

	for ( ; it != itEnd; it++)
	{
        int errorCode = GetMimicController()->AddBlend((*it)->src, (*it)->target, (*it)->param);

		switch (errorCode)
		{
		case ADD_BLEND_LOAD_ERROR:
			g->lw.WriteLn("[ERROR] Load blend for mimic error ", (*it)->src);
			break;

		case ADD_BLEND_TARGET_NOT_FOUND:
			g->bm.LoadBlend((*it)->src, (*it)->sao);
			break;
		}	
	}		
}

void CSkeletonAnimationObject::LoadMask()
{
	std::string filePath = m_dir + MASK_FILENAME;	

	ifile* file = g->rl->GetResMemFile(filePath.c_str());
	unsigned char* mask = MP_NEW_ARR(unsigned char, m_vertexCount);//new unsigned char[m_vertexCount];

	if (file)
	{
		if (file->get_file_size() != m_vertexCount)
		{
			memset(mask, 1, m_vertexCount);
		}
		else
		{
			file->read(mask, m_vertexCount);
		}

		g->rl->DeleteResFile(file);
	}
	else
	{
		memset(mask, 1, m_vertexCount);
	}

	SetMask(mask, m_vertexCount);
}

void CSkeletonAnimationObject::SetClipBox(int type, std::string& valueID,std::string& valueOrt, std::string& valuePlaneStart, std::string& valuePlaneEnd,std::string& valueOrt2,std::string& valuePlaneStart2, std::string& valuePlaneEnd2, std::string& valueOrt3,std::string& valuePlaneStart3, std::string& valuePlaneEnd3)
{
	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)m_obj3d->GetAnimation();
	if (sao)
	{
		int iValueID = rtl_atoi(valueID.c_str()) - 1;

		if (iValueID <0 || iValueID >=CLIPPING_VOLUME_COUNT )
		{
			return;
		}
		int iValueOrt = rtl_atoi(valueOrt.c_str());
		int iValuePlaneStart = rtl_atoi(valuePlaneStart.c_str());
		int iValuePlaneEnd = rtl_atoi(valuePlaneEnd.c_str());

		int iValueOrt2 = rtl_atoi(valueOrt2.c_str());
		int iValuePlaneStart2 = rtl_atoi(valuePlaneStart2.c_str());
		int iValuePlaneEnd2 = rtl_atoi(valuePlaneEnd2.c_str());

		int iValueOrt3 = rtl_atoi(valueOrt3.c_str());
		int iValuePlaneStart3 = rtl_atoi(valuePlaneStart3.c_str());
		int iValuePlaneEnd3 = rtl_atoi(valuePlaneEnd3.c_str());
		sao->SetClippingVolume(type, iValueID, iValueOrt, (float)iValuePlaneStart, (float)iValuePlaneEnd,
			iValueOrt2, (float)iValuePlaneStart2, (float)iValuePlaneEnd2,
			iValueOrt3, (float)iValuePlaneStart3, (float)iValuePlaneEnd3);
	}
}

SIdlesPackDesc* CSkeletonAnimationObject::LoadIdlesFromFile(std::string& path)
{
	std::string dir;
	int pos = path.find_last_of("\\");
	int pos2 = path.find_last_of("/");
	if (pos2 > pos)
	{
		pos = pos2;
	}
	dir = path.substr(0, pos + 1);

	TiXmlDocument doc;

	ifile* file = g->rl->GetResMemFile(path.c_str());
	if (file)
	{
		unsigned char* data = MP_NEW_ARR(unsigned char,file->get_file_size() + 1);//new unsigned char[file->get_file_size() + 1];
		data[file->get_file_size()] = 0;
		file->read(data, file->get_file_size());
		doc.Parse((const char*)data);		
		g->rl->DeleteResFile(file);

		MP_DELETE_ARR(data);//delete[] data;
	}
	else
	{
		return NULL;
	}

#define MAX_MOTIONS_COUNT 20

	std::string names[MAX_MOTIONS_COUNT];
	std::string srcs[MAX_MOTIONS_COUNT];
	int durations[MAX_MOTIONS_COUNT];

	int motionCount = 0;
	
	TiXmlNode* motion = doc.FirstChild("motion");
	while (motion != NULL)
	{
		if ((!motion->ToElement()->Attribute("name")) ||
			(!motion->ToElement()->Attribute("src")) ||
			(!motion->ToElement()->Attribute("duration")))
		{
			return NULL;
		}

		names[motionCount] = motion->ToElement()->Attribute("name");
		srcs[motionCount] = motion->ToElement()->Attribute("src");
		durations[motionCount] = rtl_atoi(motion->ToElement()->Attribute("duration"));
		motionCount++;

		motion = doc.IterateChildren("motion", motion);
	}

	if (0 == motionCount)
	{
		return NULL;
	}

	TiXmlNode* idles = doc.FirstChild("idles");
	if (!idles)
	{
		return NULL;
	}

	MP_NEW_P(desc, SIdlesPackDesc, motionCount);	

	TiXmlNode* idle = idles->FirstChild("idle");
	while (idle != NULL)
	{
		if (!idle->ToElement()->Attribute("motion"))
		{
			return NULL;
		}

		std::string motionName = idle->ToElement()->Attribute("motion");
		int freq = idle->ToElement()->Attribute("freq") ? rtl_atoi(idle->ToElement()->Attribute("freq")) : 1;
		int between = 0;
		if (idle->ToElement()->Attribute("between"))
		{
			between = rtl_atoi(idle->ToElement()->Attribute("between"));
		}
		
		for (int k = 0; k < motionCount; k++)
		if (motionName == names[k])
		{
			std::string motionPath = dir + srcs[k];
			g->motm.CorrectPath(motionPath);
			int motionID = g->motm.GetObjectNumber(motionPath);
			if (-1 == motionID)
			{
				motionID = g->motm.AddObject(motionPath);
				CMotion* motion = g->motm.GetObjectPointer(motionID);				

#ifdef LOAD_ASYNCH
				motion->LoadAsynch(motionPath);
				motion->SetDuration((float)durations[k]);
#else
				if (motion->Load(motionPath))
				{
					motion->SetDuration((float)durations[k]);
				}
				else
				{
					g->motm.DeleteObject(motionID);
					motionID = -1;
				}
#endif				
			}			

			AddIdle(motionID, between, freq);
			desc->AddIdle(motionID, between, freq);
		}		

		idle = idles->IterateChildren("idle", idle);
	}

	return desc;
}

void CSkeletonAnimationObject::LoadIdles(std::string& path)
{
	static std::map<std::string, SIdlesPackDesc*> idlesList;

	std::map<std::string, SIdlesPackDesc*>::iterator it = idlesList.find(path);

	if (it != idlesList.end())
	{
		SIdlesPackDesc* pack = (*it).second;

		for (unsigned int i = 0; i < pack->motionIDs.size(); i++)
		{
			AddIdle(pack->motionIDs[i], pack->between[i], pack->freq[i]);
		}		

		return;
	}

	SIdlesPackDesc* pack = LoadIdlesFromFile(path);
	if (pack)
	{
		idlesList.insert(std::map<std::string, SIdlesPackDesc*>::value_type(path, pack));
	}
}

_STransitionalInfo::~_STransitionalInfo()
{
	for (int i = 0; i < CLIPPING_VOLUME_COUNT; i++)
	if (clipboxs[i])
	{
		MP_DELETE(clipboxs[i]);
	}

	std::vector<SBlends*>::iterator it = blends.begin();
	std::vector<SBlends*>::iterator itEnd = blends.end();

	for ( ; it != itEnd; it++)
	{
		MP_DELETE(*it);
	}
}