
#include "StdAfx.h"
#include <userdata.h>
#include "PlatformDependent.h"
#include "GraphicsDebugSupporter.h"
#include "GlobalSingletonStorage.h"
#include "RMContext.h"
#include "nrmCharacter.h"
#include "nrmCamera.h"
#include "vs_wrappers.h"

CGraphicsDebugSupporter::CGraphicsDebugSupporter():
	m_isGroundVisible(true),
	MP_MAP_INIT(m_existingModels),
	MP_MAP_INIT(m_existingSAO)
{
}

void CGraphicsDebugSupporter::PlayCameraTrack()
{
	if (g->cm.GetActiveCamera())
	{
		nrmCamera* cam = (nrmCamera*)g->cm.GetActiveCamera()->GetUserData(USER_DATA_NRMOBJECT);

		if (cam)
		if (cam->GetCameraController())
		{
			cam->GetCameraController()->PlayCameraTrack();
		}
	}
}

void CGraphicsDebugSupporter::CopyCameraToClipboard()
{
	CVector3D eye = g->cm.GetActiveCamera()->GetEyePosition();
	CVector3D lookAt = g->cm.GetActiveCamera()->GetLookAt();	

	std::string text;
	text = "<keyframe eyex=\"";
	text += FloatToStr(eye.x);
	text += "\" eyey=\"";
	text += FloatToStr(eye.y);
	text += "\" eyez=\"";
	text += FloatToStr(eye.z);
	text += "\" lookx=\"";
	text += FloatToStr(lookAt.x);
	text += "\" looky=\"";
	text += FloatToStr(lookAt.y);
	text += "\" lookz=\"";
	text += FloatToStr(lookAt.z);
	text += "\" time=\"0\" />";

	if(OpenClipboard(g->ne.ghWnd))
	{
		HGLOBAL hgBuffer;
		char* chBuffer;
		EmptyClipboard();
		int bufferSize = text.size()+1;
		hgBuffer = GlobalAlloc(GMEM_DDESHARE, bufferSize);
		chBuffer = (char*)GlobalLock(hgBuffer);
		va_strcpy(chBuffer, text.c_str(), bufferSize);
		GlobalUnlock(hgBuffer);
		SetClipboardData(CF_TEXT, hgBuffer);
		CloseClipboard();
	}
}

void CGraphicsDebugSupporter::SaveGoTo()
{
	float x1 = 0.0f, y1 = 0.0f, z1 = 0.0f, x2 = 0.0f, y2 = 0.0f, z2 = 0.0f;

	if (g->phom.GetControlledObject())
	if (g->phom.GetControlledObject()->GetObject3D())
	{
		nrmCharacter* character = (nrmCharacter*)g->phom.GetControlledObject()->GetObject3D()->GetUserData(USER_DATA_NRMOBJECT);
		if ((character) && (character->GetWalkController()))
		{
			character->GetWalkController()->GetLastGoToCoords(x1, y1, z1, x2, y2, z2);
		}
	}

	std::wstring fileName = GetApplicationRootDirectory();
	fileName += L"goto.txt";

	FILE* fl = _wfopen(fileName.c_str(), L"w+");
	if (fl)
	{
		fprintf(fl, "%f\n", x1);
		fprintf(fl, "%f\n", y1);
		fprintf(fl, "%f\n", z1);
		fprintf(fl, "%f\n", x2);
		fprintf(fl, "%f\n", y2);
		fprintf(fl, "%f\n", z2);
		fclose(fl);
	}

	char tmp[2000];
	sprintf(tmp, "%f\n%f\n%f\n%f\n%f\n%f\n", x1, y1, z1, x2, y2, z2);

	USES_CONVERSION;
	std::string text = tmp;
	if (text.empty())
	{
		return;
	}

	if(OpenClipboard(g->ne.ghWnd))
	{
		HGLOBAL hgBuffer;
		char* chBuffer;
		EmptyClipboard();
		int sizeBuffer = text.size()+1;
		hgBuffer = GlobalAlloc(GMEM_DDESHARE, sizeBuffer);
		chBuffer = (char*)GlobalLock(hgBuffer);
		rtl_strcpy(chBuffer, sizeBuffer,text.c_str());
		GlobalUnlock(hgBuffer);
		SetClipboardData(CF_TEXT, hgBuffer);
		CloseClipboard();
	}
}

void CGraphicsDebugSupporter::ReplayGoTo()
{
	std::wstring fileName = GetApplicationRootDirectory();
	fileName += L"goto.txt";

	FILE* fl = _wfopen(fileName.c_str(), L"r");
	if (fl)
	{
		float x1, y1, z1, x2, y2, z2;
		fscanf(fl, "%f\n", &x1);
		fscanf(fl, "%f\n", &y1);
		fscanf(fl, "%f\n", &z1);
		fscanf(fl, "%f\n", &x2);
		fscanf(fl, "%f\n", &y2);
		fscanf(fl, "%f\n", &z2);

		CVector3D p1(x1, y1, z1), p2(x2, y2, z2);
		CreatePassageway(&p1, &p2);		

		fclose(fl);
	}
}

void CGraphicsDebugSupporter::MeasureAllPathesTime()
{
	/*std::string fileName = GetApplicationRootDirectory();
	fileName += "goto_all.txt";

	FILE* fl = fopen(fileName.c_str(), "r");

	if (fl)
	{
		float x1, y1, z1, x2, y2, z2;

		__int64 startTime = g->tp.GetMicroTickCount();

		while (!feof(fl))
		{
			fscanf(fl, "%f\n", &x1);
			fscanf(fl, "%f\n", &y1);
			fscanf(fl, "%f\n", &z1);
			fscanf(fl, "%f\n", &x2);
			fscanf(fl, "%f\n", &y2);
			fscanf(fl, "%f\n", &z2);
	
			CPathLine pathLine;
			g->pf.FindPath(&pathLine, CVector3D(x1, y1, z1), CVector3D(x2, y2, z2));
		}
		
		__int64  endTime = g->tp.GetMicroTickCount();

		unsigned int delta = endTime - startTime;

		fclose(fl);
	}*/
	
}


void CGraphicsDebugSupporter::GoToSegment(CVector3D* vBegin, CVector3D* vEnd)
{
	if (g->phom.GetControlledObject())
		if (g->phom.GetControlledObject()->GetObject3D())
		{
			nrmCharacter* character = (nrmCharacter*)g->phom.GetControlledObject()->GetObject3D()->GetUserData(USER_DATA_NRMOBJECT);
			if (character)
			{
				character->m_obj3d->SetCoords(vBegin->x, vBegin->y, vBegin->z);
			}

			ml3DPosition pos[2];
			pos[0].x = vBegin->x;
			pos[0].y = vBegin->y;
			pos[0].z = vBegin->z;
			pos[1].x = vEnd->x;
			pos[1].y = vEnd->y;
			pos[1].z = vEnd->z;
			character->goPath(&pos[0], 2, 0);
		}
}

void CGraphicsDebugSupporter::CreatePassageway(CVector3D* vBegin, CVector3D* vEnd)
{
	if (g->phom.GetControlledObject())
		if (g->phom.GetControlledObject()->GetObject3D())
		{
			nrmCharacter* character = (nrmCharacter*)g->phom.GetControlledObject()->GetObject3D()->GetUserData(USER_DATA_NRMOBJECT);
			if (character)
			{
				character->m_obj3d->SetCoords(vBegin->x, vBegin->y, vBegin->z);
			}
			ml3DPosition from;
			from.x = vBegin->x;
			from.y = vBegin->y;
			from.z = vBegin->z;
			ml3DPosition pos;
			pos.x = vEnd->x;
			pos.y = vEnd->y;
			pos.z = vEnd->z;
			ml3DPosition* path = NULL;
			int len = 0;
			character->findPathTo(from, pos, path, len);
			character->goPath(path, len, 0);
		}		
}

void CGraphicsDebugSupporter::ToogleGroundVisibility()
{
	g->lw.WriteLn("ToogleGroundVisibility");

	m_isGroundVisible = !m_isGroundVisible;

	std::vector<C3DObject *>::iterator it = g->o3dm.GetLiveObjectsBegin();
	std::vector<C3DObject *>::iterator itEnd = g->o3dm.GetLiveObjectsEnd();

	for ( ; it != itEnd; it++)
	{
		if ((int)((*it)->GetUserData(USER_DATA_LEVEL)) == LEVEL_GROUND)
		{
			(*it)->SetVisible(m_isGroundVisible);
		}
	}
}

void CGraphicsDebugSupporter::ShowTexturesList()
{
	std::wstring path = GetApplicationRootDirectory();
	path += L"textures.txt";

	FILE* fl = _wfopen(path.c_str(), L"w");
	if (!fl)
	{
		return;
	}

	std::vector<CTexture *>::iterator it = g->tm.GetLiveObjectsBegin();
	std::vector<CTexture *>::iterator itEnd = g->tm.GetLiveObjectsEnd();

	int allSize = 0;

	USES_CONVERSION;
	for ( ; it != itEnd; it++)
	{
		std::string s = "[";
		s += IntToStr((*it)->GetNumber());
		s += "] ";
		s += W2A((*it)->GetPath().c_str());
		if ((*it)->GetPath().empty())
		{
			s += W2A((*it)->GetName());
		}
		s += "  /";
		s +=  W2A((*it)->GetName());
		s += "/ ";
		s += IntToStr((*it)->GetTextureRealWidth());
		s += "x";
		s += IntToStr((*it)->GetTextureRealHeight());
		s += "x";
		s += IntToStr((*it)->IsTransparent() ? 4 : 3);
		if ((*it)->GetLoadingInfo()->IsMipMap())
		{
			s += "+mipmaps";
		}
		s += " filters [";
		GLint a;
		glBindTexture(GL_TEXTURE_2D, (*it)->GetNumber());
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &a);
		if (a == GL_NEAREST)
		{
			s += "nearest";
		}
		else if (a == GL_LINEAR)
		{
			s += "linear";
		}
		else 
		{
			s += "mipmap_linear";
		}
		s += ", ";
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &a);
		if (a == GL_NEAREST)
		{
			s += "nearest";
		}
		else if (a == GL_LINEAR)
		{
			s += "linear";
		}
		else 
		{
			s += "mipmap_linear";
		}
		s += "] [";
		a = (*it)->GetMinFilter();
		if (a == GL_NEAREST)
		{
			s += "nearest";
		}
		else if (a == GL_LINEAR)
		{
			s += "linear";
		}
		else 
		{
			s += "mipmap_linear";
		}
		s += ", ";
		a = (*it)->GetMagFilter();
		if (a == GL_NEAREST)
		{
			s += "nearest";
		}
		else if (a == GL_LINEAR)
		{
			s += "linear";
		}
		else 
		{
			s += "mipmap_linear";
		}
		s += "]\n";
		fprintf(fl, s.c_str());

		int size = (*it)->GetTextureRealWidth() * (*it)->GetTextureRealHeight() * ((*it)->IsTransparent() ? 4 : 3);
		if ((*it)->GetLoadingInfo()->IsMipMap())
		{
			size = (int)(size * 1.33f);
		}

		allSize += size;
	}

	std::string s = "Summary uncompressed size = ";
	s += IntToStr(allSize);
	fprintf(fl, s.c_str());

	fclose(fl);
}

void CGraphicsDebugSupporter::SaveObjectVisibilityList()
{
	CCamera3D* camera = g->cm.GetActiveCamera();
	if (camera)
	{
		g->lw.WriteLn("Camera");
		g->lw.WriteLn("=============");
		g->lw.WriteLn("Eye: ", camera->GetEyePosition().x, ", ", camera->GetEyePosition().y, ", ",
			camera->GetEyePosition().z);
		g->lw.WriteLn("Look at: ", camera->GetLookAt().x, ", ", camera->GetLookAt().y, ", ",
			camera->GetLookAt().z);
		g->lw.WriteLn("Up: ", camera->GetUp().x, ", ", camera->GetUp().y, ", ",
			camera->GetUp().z);
	}

	g->lw.WriteLn("3D Scene (", g->o3dm.GetLiveObjectsCount(), ")");
	g->lw.WriteLn("=============");

	std::vector<C3DObject *>::iterator it = g->o3dm.GetLiveObjectsBegin();
	std::vector<C3DObject *>::iterator itEnd = g->o3dm.GetLiveObjectsEnd();

	for ( ; it != itEnd; it++)
	{
		CLogValue part1("name [", (*it)->GetName(), "] visible [", (*it)->IsVisible() ? " visible " : " invisible ", "] visible now [", (*it)->IsVisibleNow() ? " visible " : " invisible ", "] ");

		CLogValue part2("res [", (*it)->GetLODGroup() ? (*it)->GetLODGroup()->GetName() : L"lodgroup null", "] distsq [", (*it)->GetDistSq(), "] lodnum [", (int)(*it)->GetLODNumber(), "] ");

		float x, y, z;
		(*it)->GetCoords(&x, &y, &z);
		CLogValue part3("x, y, z [", x, ", ", y, ", ", z, "] ");

		(*it)->GetScale(&x, &y, &z);
		CLogValue part4("scale [", x, ", ", y, ", ", z, "] ");

		CQuaternion q = (*it)->GetRotation()->GetAsDXQuaternion();
		CLogValue part5("rotation [", q.x, ", ", q.y, ", ", q.z, ", ", q.w, "] ");

		bool isPhysics = (*it)->IsPhysicsObject();
		bool isPhysicsModel = (isPhysics) && ((*it)->GetPhysicsObjID() == -1);

		CLogValue part6("isPhysics [", isPhysics, "] isPhysicsModel [", isPhysicsModel, "] IsFrozen [", (*it)->IsFrozen(), "] ");

		float minX, minY, minZ, maxX, maxY, maxZ;
		(*it)->GetBoundingBox(minX, minY, minZ, maxX, maxY, maxZ);
		CLogValue part7("bb-min [", minX , ", ", minY, ", ", minZ, "] ");
		CLogValue part8("bb-max [", maxX , ", ", maxY, ", ", maxZ, "] ");

		float dist = sqrtf((*it)->CalculateDistSq());	
		int hasDistancecheck = 0;
		if ((*it)->GetLODGroup())
			(dist < (*it)->GetLODGroup()->GetInvisibleDist()) ? 1 : 0;
		CLogValue part9("distance check [", hasDistancecheck, "] bb in frustum [", (*it)->IsBoundingBoxInFrustum(), "] ");

		g->lw.WriteLn(part1, part2, part3, part4, part5, part6, part7, part8, part9);
	}

	g->lw.WriteLn("=============");
	g->lw.WriteLn("LOD Groups (", g->lod.GetCount(), ")");
	g->lw.WriteLn("=============");

	std::vector<CLODGroup *>::iterator lit = g->lod.GetLiveObjectsBegin();
	std::vector<CLODGroup *>::iterator litEnd = g->lod.GetLiveObjectsEnd();

	for ( ; lit != litEnd; lit++)
	{
		CLogValue part1("name [", (*lit)->GetName(), "] count [", (*lit)->GetLodLevelCount(), "] ");
		g->lw.WriteLn(part1);
		
		if ((*lit)->IsDynamic())
		{
			std::vector<C3DObject *>& objects = (*lit)->GetObjects();

			for (unsigned int i = 0; i < objects.size(); i++)
			{
				C3DObject* obj = objects[i];

				if (obj)
				if (obj->GetAnimation())
				{
					CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)obj->GetAnimation();
					if (sao->GetModel())
					{
						CLogValue part("part [", obj->GetName(), "] renderMode [", sao->GetModel()->GetRenderMode(), "]");
						g->lw.WriteLn(part);
					}
				}
			}
		}
		else
		{
			for (int k = 0; k < (*lit)->GetLodLevelCount(); k++)
			{
				SLODLevel* level = (*lit)->GetLodLevel(k);						
				CLogValue part("lod [", k, "] isLoaded [", level->isLoaded, "] renderMode [", level->renderMode, "] dist [", level->lodMinDist, ", ", level->lodMaxDist);
				g->lw.WriteLn(part, "]");
			}
		}
	}

	g->lw.WriteLn("=============");
	g->lw.WriteLn("3D Models (", g->mm.GetCount(), ")");
	g->lw.WriteLn("=============");

	std::vector<CModel *>::iterator mit = g->mm.GetLiveObjectsBegin();
	std::vector<CModel *>::iterator mitEnd = g->mm.GetLiveObjectsEnd();
	
	for ( ; mit != mitEnd; mit++)
	{
		CModel* model = *mit;		
		g->lw.WriteLn("model name [", model->GetName(), "] texture count [", model->GetTextureCount() - 1, "] loading status [", model->GetLoadingState(), "] vertex count [", model->GetVertexCount(), "]");
		g->lw.WriteLn("faceCount [", model->GetFaceCount(), "] isOk [", (int)model->IsOk(), "] loadPriority [", (int)model->GetLoadPriority(), "] errorCode [", (int)model->GetLoadingErrorCode(), "]");
		for (int k = 0; k < model->GetTextureCount() - 1; k++)
		{
			int textureID = model->GetTextureNumber(k);		
			std::string slotType = "slot(texture) [";

			if (model->GetMaterial(k) != -1)
			{
				int materialID = model->GetMaterial(k);
				CMaterial* mat = g->matm.GetObjectPointer(materialID);
				textureID = mat->GetTextureID(0);
				slotType = "slot(material) [";
			}
			
			CTexture* texture = g->tm.GetObjectPointer(textureID);
			CLogValue part1(slotType, k, "] textureID [", textureID, "] texture name [", texture->GetName(), "] isdeleted [", texture->IsDeleted());
			CLogValue part2("] width [", texture->GetTextureRealWidth(), "] height [", texture->GetTextureRealHeight(), "] twosided [", model->m_params[k].twoSidedMode, "], ");

			int w, h;
			texture->QuerySizeFromOpenGL(w, h);
			CLogValue part3(" OGL width [", w, "] OGL height [", h, "]");

            g->lw.WriteLn(part1, part2, part3);
		}
	}


	/*g->lw.WriteLn("=============");
	g->lw.WriteLn("Characters (", g->skom.GetLiveObjectsCount(), ")");
	g->lw.WriteLn("=============");

	std::vector<CSkeletonAnimationObject *>::iterator cit = g->skom.GetLiveObjectsBegin();
	std::vector<CSkeletonAnimationObject *>::iterator citEnd = g->skom.GetLiveObjectsEnd();

	for ( ; cit != citEnd; cit++)
	{
		if( !(*cit))
			continue;
		CLogValue part1("name [", (*cit)->GetName(), "] parent [", (*cit)->GetParent() ? (*cit)->GetParent()->GetName() : "NULL", "] ");		
		CLogValue part2("path [", (*cit)->GetPath(), "] calmodel [", (int)(*cit)->GetCalModel(), "] 3d model [", g->mm.GetObjectPointer((*cit)->GetModelID())->GetName(), "] ");
		CLogValue part3("external textures [", (*cit)->IsContainsExternalTextures() ? "true" : "false", "] vertex count [", (*cit)->GetVertexCount(), "] ");

		std::string clipboxStr;
		std::vector<int> clipboxArray;
		(*cit)->GetUsedClipboxIDs(clipboxArray);

		for (unsigned int i = 0; i < clipboxArray.size(); i++)
		{
			if (i != 0)
			{
				clipboxStr += ", ";
			}

			clipboxStr += IntToStr(clipboxArray[i]);
		}

		CLogValue part4("face count [", (*cit)->GetFaceCount(), "] clipboxs [", clipboxStr, "] ");

		g->lw.WriteLn(part1, part2, part3, part4);		
	}*/

	g->lw.WriteLn("++++++++++++");
	g->lw.WriteLn("2D Scene");
	g->lw.WriteLn("=============");

	gRM->scene2d->PrintDebugInfo();

	gRM->wbMan->PrintDebugInfo();
}

void CGraphicsDebugSupporter::ReloadMasks()
{
	std::wstring path = GetApplicationRootDirectory();
	path += L"masks.txt";
	FILE* fl = _wfopen(path.c_str(), L"r");
	if (!fl)
	{
		return;
	}

	while (!feof(fl))
	{
		char mask[300];
		char vis[10];
		fscanf(fl, "%s", mask);
		fscanf(fl, "%s", vis);

		bool isVisible = strcmp(vis, "1") == 0;

		bool isScripted = strcmp(mask, "scripted") == 0;
		bool isAvatars = strcmp(mask, "avatars") == 0;

		std::vector<C3DObject *>::iterator it = g->o3dm.GetLiveObjectsBegin();
		std::vector<C3DObject *>::iterator itEnd = g->o3dm.GetLiveObjectsEnd();

		for ( ; it != itEnd; it++)
		{
			if (isScripted)
			{
				if ((!(*it)->GetAnimation()) && ((*it)->GetUserData(USER_DATA_NRMOBJECT)))
				{
					(*it)->SetVisible(isVisible);
				}
			}
			else if (isAvatars)
			{
				if ((*it)->GetAnimation())
				{
					(*it)->SetVisible(isVisible);
				}
			}
			else
			{
				USES_CONVERSION;
				std::string name = W2A((*it)->GetName());
				if (name.find(mask) != -1)
				{
					if ((*it)->GetUserData(USER_DATA_LEVEL) != (void*)LEVEL_CAMERA_COLLISION_ONLY)
					{
						(*it)->SetVisible(isVisible);
					}
				}
			}
		}
	}
	
	fclose(fl);
}

void CGraphicsDebugSupporter::EnableTextures()
{
	g->rs.SetBool(DISABLE_TEXTURES, false);
}

void CGraphicsDebugSupporter::DisableTextures()
{
	g->rs.SetBool(DISABLE_TEXTURES, true);
}

void CGraphicsDebugSupporter::SaveAvatarsTextures()
{
	std::vector<CSkeletonAnimationObject *>::iterator it = g->skom.GetLiveObjectsBegin();
	std::vector<CSkeletonAnimationObject *>::iterator itEnd = g->skom.GetLiveObjectsEnd();

	USES_CONVERSION;
	for ( ; it != itEnd; it++)
	{
		if ((*it)->GetPath().find("body") == -1)
		{
			continue;
		}		

		CModel* model = (*it)->GetModel();
		if (model)
		{
			int textureID = model->GetTextureNumber(0);
			CTexture* texture = g->tm.GetObjectPointer(textureID);
			if ((texture->IsDeleted()) || (!glIsTexture(texture->GetNumber())))
			{
				std::string error = "У аватара ";
				error +=  W2A((*it)->GetObject3D()->GetName());
				error += " удалена текстура тела!";
				MessageBox(g->ne.ghWnd, error.c_str(), "Внимание!", MB_ICONERROR);
			}

			std::wstring fileName = L"c:\\skin_texture_";			
			fileName += (*it)->GetObject3D()->GetName();
			fileName += L".jpg";

			g->tm.SaveTexture(texture, fileName);

			g->lw.WriteLn("Avatar ", (*it)->GetObject3D()->GetName(), " has skin texture ", texture->GetName(), " ", texture->GetNumber(), " ", texture->GetID());

			int size, format;
			glBindTexture(GL_TEXTURE_2D, texture->GetNumber());
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB, &size);
			int glErr = glGetError();
			if (glErr != 0)
			{
				size = 0;
			}
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
			g->lw.WriteLn("compressedSize: ", size, " format: ", format);
		}
	}

	g->tm.RefreshTextureOptimization();
}

void CGraphicsDebugSupporter::SaveObjectsComparisonPoint()
{
	m_existingModels.clear();
	m_existingSAO.clear();
	USES_CONVERSION;

	{
		std::vector<CModel *>::iterator it = g->mm.GetLiveObjectsBegin();
		std::vector<CModel *>::iterator itEnd = g->mm.GetLiveObjectsEnd();
		
		for ( ; it != itEnd; it++)
		{
			m_existingModels.insert(std::map<MP_WSTRING, int>::value_type(MAKE_MP_WSTRING( (*it)->GetName()), (*it)->GetID()));		
		}
	}

	{
		std::vector<CSkeletonAnimationObject *>::iterator it = g->skom.GetLiveObjectsBegin();
		std::vector<CSkeletonAnimationObject *>::iterator itEnd = g->skom.GetLiveObjectsEnd();

		for ( ; it != itEnd; it++)
		{
			m_existingSAO.insert(std::map<MP_WSTRING, int>::value_type(MAKE_MP_WSTRING((*it)->GetName()), (*it)->GetID()));		
		}
	}
}

void CGraphicsDebugSupporter::CompareObjectsWithSavedPoint()
{
	g->lw.WriteLn("Objects COMPARE:");
	g->lw.WriteLn("=========================");

	{
		std::vector<CModel *>::iterator it = g->mm.GetLiveObjectsBegin();
		std::vector<CModel *>::iterator itEnd = g->mm.GetLiveObjectsEnd();

		USES_CONVERSION;
		for ( ; it != itEnd; it++)
		{
			if (m_existingModels.find(MAKE_MP_WSTRING((*it)->GetName())) == m_existingModels.end())
			{
				g->lw.WriteLn("NEW MODEL: ", (*it)->GetID(), " ", W2A((*it)->GetName()));
			}
		}	

		std::map<MP_WSTRING, int>::iterator it1 = m_existingModels.begin();
		std::map<MP_WSTRING, int>::iterator it1End = m_existingModels.end();

		for ( ; it1 != it1End; it1++)
		{
			int id = g->mm.GetObjectNumber((*it1).first);
			if (id == -1)
			{
				id = (*it1).second;
				CModel* model = g->mm.GetObjectPointer(id);
				g->lw.WriteLn("DELETED model: ", model->GetID(), " ", model->GetName());
			}
		}
	}

	{
		std::vector<CSkeletonAnimationObject *>::iterator it = g->skom.GetLiveObjectsBegin();
		std::vector<CSkeletonAnimationObject *>::iterator itEnd = g->skom.GetLiveObjectsEnd();

		for ( ; it != itEnd; it++)
		{
			if (m_existingSAO.find(MAKE_MP_WSTRING((*it)->GetName())) == m_existingSAO.end())
			{
				g->lw.WriteLn("NEW SAO: ", (*it)->GetID(), " ", (*it)->GetName(), " ", (*it)->GetPath());
			}
		}	

		std::map<MP_WSTRING, int>::iterator it1 = m_existingSAO.begin();
		std::map<MP_WSTRING, int>::iterator it1End = m_existingSAO.end();

		for ( ; it1 != it1End; it1++)
		{
			std::wstring name = (*it1).first;
			int id = g->skom.GetObjectNumber(NULL, name);
			if (id == -1)
			{
				id = (*it1).second;
				CSkeletonAnimationObject* sao = g->skom.GetObjectPointer(id);
				g->lw.WriteLn("DELETED sao: ", sao->GetID(), " ", sao->GetName(), " ", sao->GetPath());
			}
		}

		/*std::vector<CalCoreModel*> names;
		names.reserve(1000);

		g->lw.WriteLn("CAL MODELS: ");
		g->lw.WriteLn("================");

		CalCoreModel::GetModelInstancesList(names);
		
		std::vector<CalCoreModel*>::iterator it2 = names.begin();
		std::vector<CalCoreModel*>::iterator it2End = names.end();

		for ( ; it2 != it2End; it2++)
		{
			CalCoreModel* model = *it2;

			bool isFound = false;

			std::vector<CSkeletonAnimationObject *>::iterator it = g->skom.GetLiveObjectsBegin();
			std::vector<CSkeletonAnimationObject *>::iterator itEnd = g->skom.GetLiveObjectsEnd();

			for ( ; it != itEnd; it++)
			if (((*it)->GetCalModel()) && ((*it)->GetCalModel()->getCoreModel() == model))
			{
				isFound = true;
			}

			if (!isFound)
			{
				g->lw.WriteLn(model->getName());
			}
		}*/
	}
}

void CGraphicsDebugSupporter::MakeUrlToClipboard()
{
	if (!g->phom.GetControlledObject())
	{
		return;
	}

	if (!g->phom.GetControlledObject()->GetObject3D())
	{
		return;
	}

	CVector3D coords = g->phom.GetControlledObject()->GetObject3D()->GetCoords();

	std::string url = "vacademia:///?x=";
	url += FloatToStr(coords.x);
	url += "&y=";
	url += FloatToStr(coords.y);
	url += "&z=";
	url += FloatToStr(coords.z);
	url += "&rn=0&ra=0&rx=0&ry=0&rz=1";

	if(OpenClipboard(g->ne.ghWnd))
	{
		HGLOBAL hgBuffer;
		char* chBuffer;
		EmptyClipboard();
		int bufferSize = url.size()+1;
		hgBuffer = GlobalAlloc(GMEM_DDESHARE, bufferSize);
		chBuffer = (char*)GlobalLock(hgBuffer);
		va_strcpy(chBuffer, url.c_str(), bufferSize);
		GlobalUnlock(hgBuffer);
		SetClipboardData(CF_TEXT, hgBuffer);
		CloseClipboard();
	}
}

CGraphicsDebugSupporter::~CGraphicsDebugSupporter()
{
}