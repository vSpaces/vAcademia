
#include "StdAfx.h"
#include "Common.h"
#include "FakeObject.h"
#include "ColorConvert.h"
#include "CoreMaterial.h"
#include "HelperFunctions.h"
#include "VAListMessageObject.h"
#include "SkeletonAnimationObject.h"
#include "StringListMessageObject.h"
#include "LoadingCal3DObjectsTask.h"
#include "GlobalSingletonStorage.h"

#define DEFAULT_VIEWPORT_WIDTH	256
#define DEFAULT_VIEWPORT_HEIGHT	256
#define SPECIAL_COLOR_STRING	"255;128;128"

_SCommandsParam::_SCommandsParam(std::string _commandID, std::string _commandPath, std::string _commandParam, std::string _commandValue):
	MP_STRING_INIT(commandID),
	MP_STRING_INIT(commandPath),
	MP_STRING_INIT(commandParam),
	MP_STRING_INIT(commandValue)
{
	commandID = _commandID;
	commandPath = _commandPath;
	commandParam = _commandParam;
	commandValue = _commandValue;	 
}

_SCommandsParam::_SCommandsParam(const _SCommandsParam& other):
	MP_STRING_INIT(commandID),
	MP_STRING_INIT(commandPath),
	MP_STRING_INIT(commandParam),
	MP_STRING_INIT(commandValue)
{
	commandID = other.commandID;
	commandPath = other.commandPath;
	commandParam = other.commandParam;
	commandValue = other.commandValue;	
}

CAvatarObject::CAvatarObject():
	m_viewportHeight(DEFAULT_VIEWPORT_HEIGHT),
	m_viewportWidth(DEFAULT_VIEWPORT_WIDTH),
	m_isDefaultSettingsApplyed(false),	
	m_isDefineSnapshotMode(false),		
	m_isFinalizeRequired(false),
	m_isNeedToMakePhoto(false),
	m_isFinalizeTimed(false),
	m_isSpecialColor(false),
	m_dwSnaphotBgColorR(070),
	m_dwSnaphotBgColorG(128),
	m_dwSnaphotBgColorB(203),
	m_dwSnaphotBgColorA(255),
	m_isEditorVisible(false),	
	m_isMyAvatar(false),
	m_fCameraFov(0.75f),
	m_finalizeTime(0),	
	m_viewportX(0),
	m_viewportY(0),
	m_obj3d(NULL),
	m_headID(1),
	m_fbo(NULL),
	MP_VECTOR_INIT(m_avatarsObjPartsInfo),
	MP_VECTOR_INIT(m_rgbvColorCommands),
	MP_VECTOR_INIT(m_blendCommands),
	MP_VECTOR_INIT(m_loadingCal3DObjectsTasks),
	MP_MAP_INIT(m_clothesBrightness),
	MP_STRING_INIT(m_currentCameraMode),
	MP_STRING_INIT(m_bodyTexture),
	MP_VECTOR_INIT(m_avatarParts),
	MP_VECTOR_INIT(m_savedParameters),
	MP_MAP_INIT(m_newTexturesMap),
	MP_MAP_INIT(m_oldTexturesMap),
	MP_MAP_INIT(m_changeTextureCommands),
	MP_VECTOR_INIT(m_asynchLoadedModels)
{
	m_currentCameraMode = "avatar_0";
	m_compositeTextureManager = MP_NEW(CCompositeTextureManager);
	m_commandCache = MP_NEW(CCommandCache);
	m_commandCache->SetAvatar(this);
	m_commandCache->SetCompositeTextureManager(m_compositeTextureManager);

	for (int i = 0; i < MAX_LEVELS; i++)
	{
		m_brightnessLevel[i] = 60;
	}

	m_isClothesLoadAsynch = (g->ci.GetProcessorCount() != 1);	
	
	m_hasNoParent = true;

	assert(g->scm.GetActiveScene());
	if (g->scm.GetActiveScene())
	{
		g->scm.GetActiveScene()->AddPlugin(this);
	}

	m_billboardTask = NULL;

	m_bodyTexture = "";
}

bool CAvatarObject::IsClothesUsed(std::string src) 
{
	std::vector<SAvatarObjPart>::iterator it = m_avatarParts.begin();
	std::vector<SAvatarObjPart>::iterator itEnd = m_avatarParts.end();

	for ( ; it != itEnd; it++)
	{
		std::string& partSRC = (*it).objSrc;
		if (partSRC.find(src) != -1)
		{
			return true;
		}
	}

	return false;
}

IPlugin* CAvatarObject::GetPluginInterface()
{
	return this;
}

void CAvatarObject::BeforeUpdate()
{
}

void CAvatarObject::AfterUpdate()
{	
}

void CAvatarObject::BeforeRender()
{
}

void CAvatarObject::AfterRender()
{
}

void CAvatarObject::SetObject(void* object)
{	
	assert(object);

	m_obj3d = (C3DObject *)object; 
	//m_clipBoxHandler.SetObject3D(m_obj3d);

	if (m_avatarParts.size() == 0)
	{
		SAvatarObjPart bodyMesh;
		bodyMesh.objName = HEAD_DEFAULT_NAME;
		bodyMesh.obj3d = m_obj3d;
		m_avatarParts.push_back(bodyMesh);
	}
}

void CAvatarObject::EndRendering()
{
	if ((m_obj3d->GetParent() == NULL) != m_hasNoParent)
	{
		m_hasNoParent = (m_obj3d->GetParent() == NULL);
		m_isFinalizeRequired = true;
	}
	
	if (m_commandCache)
	{
		m_commandCache->ExecuteQueue();
	}

	IAnyDataContainer*	anyDataContainer = rGet("__SnapshotImage");
	if (anyDataContainer)	
	{
		CSprite* sprite = (CSprite*)anyDataContainer->GetMediaPointer();
		if (sprite)
		{
			// получаем текущие параметры картинки
			int	currentWidth = 0;
			int	currentHeight = 0;
			int	currentX = 0;
			int	currentY = 0;
			sprite->GetSize(currentWidth, currentHeight);
			sprite->GetCoords(currentX, currentY);

			int newWidth = 2;
			int newHeight = 2;
			while (newWidth < currentWidth)
			{
				newWidth *= 2;
			}
			while (newHeight < currentHeight)
			{
				newHeight *= 2;
			}

			int textureID = sprite->GetTextureID(0);
			assert(textureID != -1);
			CTexture* texture = g->tm.GetObjectPointer(textureID);

			if ((currentWidth != newWidth) || (currentHeight != newHeight))
			{
				if (!sprite->IsUniqueTextureUsed())
				{
					unsigned int textureID = CFrameBuffer::CreateColorTexture(GL_TEXTURE_2D, newWidth, newHeight, GL_RGBA, GL_RGBA, false);

					int engineTextureID = g->tm.ManageGLTexture("avatar_photo", textureID, newWidth, newWidth);
					CTexture* newTexture = g->tm.GetObjectPointer(engineTextureID);
					newTexture->UpdateCurrentFiltration(GL_NEAREST, GL_NEAREST);
					newTexture->SetTextureWidth(currentWidth);
					newTexture->SetTextureHeight(currentHeight);
					newTexture->SetTextureRealWidth(newWidth);
					newTexture->SetTextureRealHeight(newHeight);
					newTexture->SetTransparentStatus(true);
					newTexture->SetXInvertStatus(texture->IsXInvert());
					newTexture->SetYInvertStatus(!texture->IsYInvert());
					currentWidth = newWidth;
					currentHeight = newHeight;
		
					sprite->SetUniqueTextureStatus(true);
					sprite->SetTextureID(engineTextureID);

					texture = newTexture;
				}
				else
				{
					currentWidth = newWidth;
					currentHeight = newHeight;
				}
			}

			sprite->SetWorkRect(0, currentHeight - m_viewportHeight, m_viewportWidth, currentHeight);

			GLFUNC(glBindTexture)(GL_TEXTURE_2D, texture->GetNumber());
			if ((g->ei.IsExtensionSupported(GLEXT_ext_framebuffer_multisample)) &&
				(g->ei.IsExtensionSupported(GLEXT_ext_framebuffer_blit)))
			{
				GLFUNC(glBindFramebufferEXT)(GL_READ_FRAMEBUFFER_EXT, 0);
				GLFUNC(glBindFramebufferEXT)(GL_DRAW_FRAMEBUFFER_EXT, 0);
			}
			GLFUNC(glCopyTexSubImage2D)(GL_TEXTURE_2D, 0, 0, 0, m_viewportX, g->stp.GetCurrentHeight() - m_viewportY - m_viewportHeight, m_viewportWidth, m_viewportHeight);			
			g->tm.RefreshTextureOptimization();		

			if (m_isNeedToMakePhoto)
			{
				dSet("__isPhotoTaked", true);
				/*std::string fileName = "c:\\avatar_photo.png";
				g->tm.SaveTexture(texture, fileName);*/

				//m_isNeedToMakePhoto = false;
			}
		}
	}

	if (m_isFinalizeRequired)
	{	
		if (m_avatarsObjPartsInfo.size() > 0)
		{
			return;
		}

		if (m_loadingCal3DObjectsTasks.size() > 0)
		{
			return;
		}		

		std::vector<SAvatarObjPart>::iterator it = m_avatarParts.begin();
		std::vector<SAvatarObjPart>::iterator itEnd = m_avatarParts.end();

		for ( ; it != itEnd; it++)
		if ((*it).obj3d)
		{
			CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)(*it).obj3d->GetAnimation();
			if (!sao)
			{
				continue;
			}
			CModel* model = sao->GetModel();

			if (model)
			for (int i = 0; i < model->GetTextureCount() - 1; i++)
			{
				int textureID = model->GetTextureNumber(i);
				CTexture* texture = g->tm.GetObjectPointer(textureID);
				if (texture->GetLoadedStatus() != TEXTURE_LOADED)
				{
					return;
				}
			}
		}

		m_finalizeTime = g->ne.time;
		m_isFinalizeTimed = true;

		m_isFinalizeRequired = false;
	}	
}

void CAvatarObject::EndRendering2D()
{
	if (!m_obj3d)
	{
		return;
	}

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();
	if (!sao)
	{
		return;
	}

	if ((sao->IsLoaded()) && (!sao->IsDeleted()) && (!m_obj3d->IsDeleted()) && (sao->GetCalModel()) &&
		(m_obj3d->GetLODGroup()) && (m_obj3d->GetLODGroup()->GetLodLevelCount() > 0))
	if ((m_isFinalizeTimed) && (g->ne.time - m_finalizeTime > 10000))
	{
		if ((m_billboardTask) && (m_billboardTask->GetFileName() != GetBillboardCacheName()))
		{
			if ((m_billboardTask->IsPerformed()) || (g->taskm.RemoveTask(m_billboardTask)))
			{				
				if (m_billboardTask->GetData())
				{
					MP_DELETE_ARR_UNSAFE(m_billboardTask->GetData());
				}
				MP_DELETE(m_billboardTask);
			}
			else
			{
				m_billboardTask->DestroyResultOnEnd();
			}

			m_billboardTask = NULL;
		}
		
		if (!m_billboardTask)
		{
			if (CanTakeBillboardFromCache())
			{
				std::string billboardName = GetBillboardCacheName();
				m_billboardTask = g->dc.GetDataAsynch(billboardName);
			}
		}

		if (!m_billboardTask)
		{
			std::vector<SAvatarObjPart>::iterator it = m_avatarParts.begin();
			std::vector<SAvatarObjPart>::iterator itEnd = m_avatarParts.end();

			bool isApplyIdle = false;

			for ( ; it != itEnd; it++)
			if ((*it).obj3d)
			{
				CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)(*it).obj3d->GetAnimation();
				if (sao)
				{
					sao->BeginAsynchUpdate(NULL);
					sao->Update(1000, 1);
					sao->EndAsynchUpdate();
					isApplyIdle = sao->IsEndApplyIdle();
				}
			} 
	
			if (isApplyIdle)
			{
				if (!g->ne.isIconicAndIntel)
				{
					MakeBillboard(NULL);
				}
				m_isFinalizeTimed = false;
			}				
		}
		else if (m_billboardTask->IsPerformed())
		{
			MakeBillboard(m_billboardTask);
			m_billboardTask = NULL;
			m_isFinalizeTimed = false;
		}
	}

	if (m_compositeTextureManager)
	{
		m_compositeTextureManager->LoadAsynchCompressedTexture();
	}
}

void CAvatarObject::ApplyBlends()
{
	std::vector<SCommandsParam*>::iterator itB = m_blendCommands.begin(); 

	for ( ; itB != m_blendCommands.end(); itB++)
	{
		int partID = GetIndexPart((*itB)->commandPath);
		if (partID != -1)
		{
			HandleSetParameterSafely((*itB)->commandID, (*itB)->commandPath,(*itB)->commandParam, (*itB)->commandValue);
			MP_DELETE(*itB);
			itB = m_blendCommands.erase(itB);
			itB--;
		}
	}
}

void CAvatarObject::ApplyClothesColors()
{
	std::vector<SCommandsParam*>::iterator itRGBV = m_rgbvColorCommands.begin(); 

	while (itRGBV !=m_rgbvColorCommands.end())
	{
		m_isComeToEndSetRGBVColor = false;
		HandleSetParameterSafely((*itRGBV)->commandID, (*itRGBV)->commandPath,(*itRGBV)->commandParam, (*itRGBV)->commandValue);
		if (m_isComeToEndSetRGBVColor)
		{
			MP_DELETE(*itRGBV);
			itRGBV = m_rgbvColorCommands.erase(itRGBV);			
		}
		else
		{
			itRGBV++;
		}
	}

	MP_MAP<MP_STRING, MP_WSTRING>::iterator it = m_changeTextureCommands.begin(); 

	for ( ; it != m_changeTextureCommands.end(); it++)
	{
		if (ChangeTexture((*it).first, (*it).second))
		{
			it = m_changeTextureCommands.erase(it);
			it--;
		}
	}

}

void CAvatarObject::OnAvatarPartLoaded(C3DObject* obj3d)
{
	if (m_isSpecialColor)
	if ((obj3d) && (obj3d->GetAnimation()) && (obj3d->GetAnimation()->GetModel()))
	{
		int materialID = obj3d->GetAnimation()->GetModel()->GetMaterial(0);
		CMaterial* mat = g->matm.GetObjectPointer(materialID);
		mat->AddState("color", SPECIAL_COLOR_STRING);		
	}
}

void CAvatarObject::MakeUseOfSavingAvatarsObjPartsInfo()
{
	if ((g->stat.GetClothesQueueTime() > g->rs.GetInt(MAX_CLOTHES_LOADING_FRAME_TIME)) && (!m_isMyAvatar))
	{
		return;
	}

	__int64 t1 = g->tp.GetTickCount();

	if (!m_isClothesLoadAsynch)
	{
		if (m_avatarsObjPartsInfo.size() != 0)
		{
			SAvatarObjPart newMesh;
			std::vector<SAvatarsObjNameAndSrc*>::iterator it = m_avatarsObjPartsInfo.begin();

			newMesh.objName = (*it)->objName;
			newMesh.objSrc = (*it)->objSrc;

			if (LoadAndBindDevided(newMesh.objSrc, &newMesh.obj3d, (*it)->data, (*it)->size))
			{
				m_avatarParts.push_back(newMesh);
				CheckDressAndSuperDress();

				ApplyClothesColors();
				ApplyBlends();

				OnAvatarPartLoaded(newMesh.obj3d);
			}

			MP_DELETE(*it);
			m_avatarsObjPartsInfo.erase(it);

			if (m_avatarsObjPartsInfo.size() == 0)
			{
				m_avatarParts[0].obj3d->GetLODGroup()->SetDrawState(true);
				float scale = m_avatarParts[0].obj3d->GetScale();
				m_avatarParts[0].obj3d->GetLODGroup()->SetCommonScale(scale, scale, scale);
			}
		}
	}
	else
	{	
		if (m_loadingCal3DObjectsTasks.size() > 0)	
		{
			std::vector<CLoadingCal3DObjectsTask*>::iterator it = m_loadingCal3DObjectsTasks.begin();
			for (; it != m_loadingCal3DObjectsTasks.end(); it++)
			{
				CLoadingCal3DObjectsTask* task = *it;
				if (task->IsPerformed())				
				{
					if (task->IsLoaded())
					{
						SAvatarObjPart newMesh;

						newMesh.objName = task->GetName();
						newMesh.objSrc = task->GetPath();

						LoadAndBindEngineObjects(task->GetTransitionalInfo(), newMesh.objSrc, &newMesh.obj3d);

						m_avatarParts.push_back(newMesh);
						CheckDressAndSuperDress();

						ApplyClothesColors();
						ApplyBlends();

						OnAvatarPartLoaded(newMesh.obj3d);
					}
					else
					{
						STransitionalInfo* info = task->GetTransitionalInfo();
						if (info)
						{
							if (info->newSAO)
							{
								info->newSAO->FreeResources();
								if (info->newSAO->GetID() == 0xFFFFFFFF)
								{
									MP_DELETE(info->newSAO);
								}
							}
							info->Clear();							
						}
					}
				
					MP_DELETE(*it);
					m_loadingCal3DObjectsTasks.erase(it);

					if (m_loadingCal3DObjectsTasks.size() == 0)
					{
						m_avatarParts[0].obj3d->GetLODGroup()->SetDrawState(true);
						float scale = m_avatarParts[0].obj3d->GetScale();
						m_avatarParts[0].obj3d->GetLODGroup()->SetCommonScale(scale, scale, scale); 
					}

					break;
				}
			}
		}
	}

	__int64 t2 = g->tp.GetTickCount();
	g->stat.AddClothesQueueTime((int)(t2 - t1));
}

void CAvatarObject::EndRendering3D()
{
	MakeUseOfSavingAvatarsObjPartsInfo();

	if ((!m_isMyAvatar) && (!m_isNeedToMakePhoto))
	{
		return;
	}

	if ((!m_isEditorVisible) && (!m_isNeedToMakePhoto))
	{
		return;
	}

	if (m_isNeedToMakePhoto)
	{
		m_currentCameraMode = "head_187";		
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();
		if (sao)
		{
			if (sao->GetCompatabilityID() < 0)
			{
				m_currentCameraMode = "head_277";
			}
		}		
		CRotationPack rot;
		rot.SetAsDXQuaternion(0, 0, 1, 0);
		m_obj3d->SetRotation(&rot);
	}

	g->ne.SetWorkMode(WORK_MODE_SNAPSHOTRENDER);

	float koef = (float)m_viewportHeight / (float)g->stp.GetCurrentHeight();
	int realViewportWidth = (int)(g->stp.GetCurrentWidth() * koef);
	int offsetX = (m_viewportWidth - realViewportWidth) / 2;

	GLFUNC(glClear)(GL_DEPTH_BUFFER_BIT);
	g->stp.PushMatrix();
	int baseOffsetX = m_viewportX + offsetX;
	int baseOffsetY = g->stp.GetCurrentHeight() - m_viewportY - m_viewportHeight;	

	g->stp.SetViewport(baseOffsetX, baseOffsetY, realViewportWidth, m_viewportHeight);
	g->stp.EnableScissors(m_viewportX, -baseOffsetY, m_viewportWidth, m_viewportHeight);

	g->stp.PushMatrix();

	g->stp.SetDefaultStates();
	g->stp.SetMaterial(-1);
	g->stp.PrepareForRender();

	if (g->rs.GetBool(SHADERS_USING))
	{
		g->sm.UnbindShader();
	}

	GLFUNC(glLoadIdentity)();
	g->cm.SetCamera2d();
	g->stp.SetState(ST_ZTEST, false);
	g->stp.SetState(ST_CULL_FACE, false);

	CColor3 clr(255, 255, 255);
	g->sp.PutAlphaSprite(-offsetX,0,0,0,m_viewportWidth,m_viewportHeight, "ui:\\resources\\avatars\\editor\\new\\bg_avatar.png", 255, clr);
	//g->sp.PutOneColorQuad(-1.0f, -1.0f, 1.0f, 1.0f, m_dwSnaphotBgColorR, m_dwSnaphotBgColorG, m_dwSnaphotBgColorB);
	g->stp.SetState(ST_ZTEST, true);
	g->stp.SetState(ST_CULL_FACE, true);
	g->stp.PopMatrix();

	CCamera3D* camera = CreateRenderCamera(m_currentCameraMode, -1.0f);
	camera->Bind();
	
	{
		g->cm.SetFov((float)(g->rs.GetInt(AE_FOV) - 45));
	}
	g->cm.ExtractFrustum();

	g->tm.EnableTexturing();
	g->tm.RefreshTextureOptimization();
//	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	g->stp.SetColor(255, 255, 255);

	g->shdw.DisableShadows();

	g->mr.SetMode(MODE_RENDER_ALL);

	m_obj3d->GetLODGroup()->UpdateAnimations();
	m_obj3d->GetLODGroup()->Update(true, true);
	m_obj3d->GetLODGroup()->ClearVisibilityCheckResults();

	g->mr.SetMode(MODE_RENDER_SOLID);
	m_obj3d->GetLODGroup()->Draw();
	g->mr.SetMode(MODE_RENDER_ALPHATEST);
	m_obj3d->GetLODGroup()->Draw();
	g->mr.SetMode(MODE_RENDER_TRANSPARENT);
	m_obj3d->GetLODGroup()->Draw();
	g->mr.SetMode(MODE_RENDER_ALL);

	g->shdw.EnableShadows();

	GLFUNC(glColorMask)(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
	GLFUNC(glLoadIdentity)();
	g->cm.SetCamera2d();
	g->stp.SetBlendFunc(GL_ONE, GL_ONE);
	g->stp.SetState(ST_BLEND, true);
	g->stp.SetState(ST_ZTEST, false);	
	g->stp.SetState(ST_CULL_FACE, false);
	g->sp.PutOneColorQuad(-1.0f, -1.0f, 1.0f, 1.0f, m_dwSnaphotBgColorR, m_dwSnaphotBgColorG, m_dwSnaphotBgColorB, 255);
	g->stp.SetState(ST_ZTEST, true);	
	g->stp.SetState(ST_CULL_FACE, true);
	g->stp.SetState(ST_BLEND, false);
	GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	
	g->stp.RestoreViewport();
	g->stp.PopMatrix();

	g->stp.DisableScissors();

	g->ne.SetWorkMode(WORK_MODE_NORMALRENDER);

//	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);	
}

void	CAvatarObject::HandleSetBodyHeight(std::string commandValue)
{
	float value = (StringToFloat(commandValue.c_str()) / 100.0f);

	if (m_obj3d)
	{
		m_obj3d->SetScale(value, value, value);
	}
}

void	CAvatarObject::SetSkinColor(int textureID, DWORD color1, DWORD color2, float koef, int maxTextures)
{
	if (!m_compositeTextureManager)
	{
		return;
	}

#ifdef AVED_LOGGING
	g->lw.WriteLn("[AVED]CAvatarObject::SetSkinColor ");
#endif

	bool found = false;
	
	int i;
	for (i = 0; i < maxTextures; i++)
	{
		m_compositeTextureManager->SetDiffuse(textureID, i, 0);
	}

	i = 0;

	while (!found)
	{
		if ((koef >= 0 + (float)i) && (koef < 1.0f + (float)i))
		{			
			m_compositeTextureManager->SetDiffuse(textureID, i, color1);

			if (i + 1 < maxTextures)
			{
				m_compositeTextureManager->SetDiffuse(textureID, i + 1, color2);
			}
			
			found = true;
		}

		i++;
	}
	
	m_compositeTextureManager->EnableAddMode(textureID);
	m_compositeTextureManager->UpdateTexture(textureID);	
}

void	CAvatarObject::HandleInterpolateTexture(std::string commandValue, std::string commandParam, std::string commandPart)
{
	if (!m_compositeTextureManager)
	{
		return;
	}

#ifdef AVED_LOGGING
	g->lw.WriteLn("[LOG]CAvatarObject::HandleInterpolateTexture commandValue = ", commandValue.GetBuffer(), " commandParam = ", commandParam.GetBuffer(), " commandPart = ", commandPart.GetBuffer() );
#endif

	int len = commandPart.size() - 1;
	while (commandPart[len] != '.')
	{
		len--;
	}
	commandPart.erase(0, len + 1);

	unsigned int k = 0;
	std::string tmp = "";

	std::vector<int> textureIDs;

	while (commandPart.size() > k)
	{
		if (';' == commandPart[k])
		{
			textureIDs.push_back(m_compositeTextureManager->GetCompositeTextureID(tmp));
			tmp = "";
		}
		else
		{
			char temp[2];
			temp[0] = commandPart[k];
			temp[1] = 0;
			tmp = tmp + temp;
		}
		
		k++;
	}

	if (tmp.length() != 0)
	{
		textureIDs.push_back(m_compositeTextureManager->GetCompositeTextureID(tmp));
	}

	StringReplace(commandValue, ".", ",");	
	float koef = StringToFloat(commandValue.c_str());

	std::vector<int>::iterator iter = textureIDs.begin();
	for (; iter != textureIDs.end(); iter++)
	{
		m_compositeTextureManager->SetSpecialDiffuseKoef(*iter, koef);
		
		m_compositeTextureManager->EnableAddMode(*iter);
		m_compositeTextureManager->UpdateTexture(*iter);
	}
}

void	CAvatarObject::ApplyDefaultSettings()
{
	if (m_isDefaultSettingsApplyed)
	{
		return;
	}

	if (m_obj3d && !m_avatarParts.empty() )
	if (m_compositeTextureManager)
	{		
		m_compositeTextureManager->UpdateLinksToCompositeTextures(m_obj3d, m_avatarParts);
	}
}

bool	CAvatarObject::ChangeTexture(std::string meshSrc, std::wstring textureSrc)
{
	StringReplace(meshSrc, "ui:resources\\avatars\\editor\\", "");
	StringReplace(meshSrc, ":geometry\\avatars\\editor\\", "");

	std::vector<C3DObject*> objs = m_obj3d->GetLODGroup()->GetObjects();
	USES_CONVERSION;
	for (unsigned int i = 0; i < objs.size(); i++)
	{
		std::string name = WCharStringToString(objs[i]->GetName());

		if (!objs[i]->GetAnimation())
		{
			continue;
		}

		if (!objs[i]->GetAnimation()->GetModel())
		{
			continue;
		}

		if (name.find(meshSrc) != -1)
		{				
			MP_MAP<MP_STRING, MP_WSTRING>::iterator it = m_oldTexturesMap.find(MAKE_MP_STRING(meshSrc));
			if (it == m_oldTexturesMap.end())
			{
				int textureID = objs[i]->GetAnimation()->GetModel()->GetTextureNumber(0);
				CTexture* texture = g->tm.GetObjectPointer(textureID);		
				m_oldTexturesMap.insert(MP_MAP<MP_STRING, MP_WSTRING>::value_type(MAKE_MP_STRING(meshSrc), MAKE_MP_WSTRING(texture->GetPath())));
			}

			int textureID = g->tm.GetObjectNumber( textureSrc);
			if (textureID == -1)
			{				
				textureID = g->tm.LoadAsynch( textureSrc, textureSrc, NULL);
			}
			objs[i]->GetAnimation()->GetModel()->ChangeTexture(0, textureID);

			int materialID = objs[i]->GetAnimation()->GetModel()->GetMaterial(0);
			CMaterial* mat = g->matm.GetObjectPointer(materialID);
			STexture* tex = MP_NEW(STexture);
			tex->textureID = textureID;
			tex->isAlphaBlend = false;
			mat->SetTextureLevel(0, TEXTURE_SLOT_TEXTURE_2D, tex);

			MP_MAP<MP_STRING, MP_WSTRING>::iterator it2 = m_newTexturesMap.find(MAKE_MP_STRING(meshSrc));
			if (it2 != m_newTexturesMap.end())
			{	
				m_newTexturesMap.erase(it2);
			}
			m_newTexturesMap.insert(MP_MAP<MP_STRING, MP_WSTRING>::value_type(MAKE_MP_STRING(meshSrc), MAKE_MP_WSTRING(textureSrc)));
				
			return true;
		}
	}

	return false;
}

void	CAvatarObject::ApplyCommonSettings()
{
	if (m_avatarParts.size() == 0)
	{
		return;
	}

	if (m_compositeTextureManager)
	{
		m_compositeTextureManager->UpdateLinksToCompositeTextures(m_obj3d, m_avatarParts);
	}
	
	m_avatarParts[m_avatarParts.size() - 1].RestoreParams();

	MP_MAP<MP_STRING, SAvatarParam>::iterator it = m_commonParameters.parameters.find(MAKE_MP_STRING("set_body_height:common:0"));
	if (it != m_commonParameters.parameters.end())
	{
		HandleSetBodyHeight((*it).second.commandValue.c_str());
	}
}

unsigned long	CAvatarObject::ProcessMessage(IMessageObject* message)
{
	int argumentCount = message->GetArgumentCount();
	if (0 == argumentCount)
	{
		return 0;
	}
	
	std::string command = message->GetNextString();
	StringToLower(command);	

	if ("free_all" == command)
	{
		UndressAvatar();
		MP_DELETE(m_compositeTextureManager);
	}
	else if ("get_diffuse_color" == command)
	{
		assert(argumentCount == 2);	
		std::string levelStr = message->GetNextString();
		int level = rtl_atoi(levelStr.c_str());

		dSet("__currentDiffuseR", m_diffuseColors[level][0]);
		dSet("__currentDiffuseG", m_diffuseColors[level][1]);
		dSet("__currentDiffuseB", m_diffuseColors[level][2]);
	}
	else if ("get_clothes_color" == command)
	{
		assert(argumentCount == 2);	
		std::string partName = message->GetNextString();
		unsigned int partID = (unsigned int)GetIndexPart(partName);

		int materialID = -1;

		if ((partID >= 0) && (partID < m_avatarParts.size()))
		if (m_avatarParts[partID].obj3d)
		if (m_avatarParts[partID].obj3d->GetAnimation())
		if (m_avatarParts[partID].obj3d->GetAnimation()->GetModel())
		{
			materialID = m_avatarParts[partID].obj3d->GetAnimation()->GetModel()->GetMaterial(0);
		}
		
		CMaterial* material = g->matm.GetObjectPointer(materialID);
		unsigned int* color = (unsigned int*)material->GetState(STATE_COLOR);

		if ((color) && (materialID != -1))
		{
			dSet("__currentClothesR", color[0]);
			dSet("__currentClothesG", color[1]);
			dSet("__currentClothesB", color[2]);
		}
		else
		{
			dSet("__currentClothesR", 255);
			dSet("__currentClothesG", 255);
			dSet("__currentClothesB", 255);
		}
	}
	else if ("get_clothes_brightness" == command)
	{
		assert(argumentCount == 2);	
		std::string partName = message->GetNextString();
		MP_MAP<MP_STRING, int>::iterator it = m_clothesBrightness.find(MAKE_MP_STRING(partName));
		if (it != m_clothesBrightness.end())
		{
			dSet("__currentClothesBrightness", (*it).second);
		}
		else
		{
			dSet("__currentClothesBrightness", 50);
		}
	}
	else if ("get_blend_value" == command)
	{
		assert(argumentCount == 3);	
		std::string partName = message->GetNextString();
		std::string blendName = message->GetNextString();
		int partIndex = GetIndexPart(partName);
		if (partIndex != -1) 
		{
			dSet("__currentBlendValue", GetPartBlendWeight(m_avatarParts[partIndex], blendName));
		}
	}
	else if ("get_current_brightness_value" == command )
	{
		assert(argumentCount == 2);	
		std::string levelStr = message->GetNextString();
		int level = rtl_atoi(levelStr.c_str());
		if ((level >= 0) && (level < MAX_LEVELS))
		{
			dSet("__currentBrightness", (float)m_brightnessLevel[level]);
		}
		else
		{
			dSet("__currentBrightness", 0.0f);
		}
	}
	else if ("get_current_head" == command )
	{
		assert(argumentCount == 1);	
		
		dSet("__currentHeadID", (float)m_headID);
	}
	else if ("is_avatar_loaded" == command )
		{
			assert(argumentCount == 1);	
			dSet("__isAvatarLoaded", IsAvatarLoaded() ? 1.0f : 0.0f);
	}
	else if ("is_clothes_used" == command )
	{
		assert(argumentCount == 2);
		std::string src = message->GetNextString();
		
		dSet("__isClothesUsed", IsClothesUsed(src) ? 1.0f : 0.0f);
	}
	else if ("get_skin_color" == command )
	{
		assert(argumentCount == 1);
				
		float res = -1.0f;
		if (m_compositeTextureManager)
		for (int k = 0; k < 4; k++)
		{
			unsigned char color = (unsigned char)(m_compositeTextureManager->GetDiffuse(0, k)%255);
			if (color != 0)
			{
				res = (float)k + (float)(255 - color) / 255.0f;				
				break;
			}
		}

		if (-1.0f == res)
		{
			std::string _res = m_commandCache ? m_commandCache->GetValueLastCachedCommand("interpolate_texture") : "";
			if (_res != "")
			{
				res = StringToFloat(_res.c_str());
			}
		}
		
		dSet("__skinColor", res);
	}
	else if ("is_template_load" == command )
	{
		assert(argumentCount == 2);
		std::string csTemplate = message->GetNextString();
		bool bTemplateLoad = false;
		if (CheckTemplateLoad(csTemplate))
		{
			bTemplateLoad = true;
		}
		dSet("__isLoad", bTemplateLoad);
	}	
	else if ("set_shapshot_color" == command)
	{
		assert(message->GetArgumentCount() == 5);
		std::string strR = message->GetNextString();
		std::string strG = message->GetNextString();
		std::string strB = message->GetNextString();
		std::string strA = message->GetNextString();
		setSnapshotColor(strR,strG,strB,strA);
	}
	else	if ("my_avatar" == command)
	{
		m_isMyAvatar = true;
		if (m_compositeTextureManager)
		{
			m_compositeTextureManager->SetMyAvatar(true);
		}
	}
	else	if ("make_snapshot" == command)
	{
		// сделать снимок аватара и поместить его
		// в image по ссылке в переменной avatar.__SnapshotImage
		assert(argumentCount == 2);
		std::string value = message->GetNextString();
		MakeSnapshot(value);
	}
	else	if ( "set_fov" == command )
	{
			std::string value = message->GetNextString();
			m_fCameraFov = (float)(rtl_atoi(value.c_str())) / 100.0f;
	}
	else	if ("change_texture" == command)
	{
		assert(argumentCount == 3);
		std::string meshSrc = message->GetNextString();
		std::string textureSrc = message->GetNextString();

		USES_CONVERSION;
		ChangeTexture(meshSrc, A2W(textureSrc.c_str()));
	}
	else	if ("get_current_texture" == command)
	{
		assert(argumentCount == 2);
		std::string meshSrc = message->GetNextString();
		if (meshSrc == "main")
		{
			sSet("__currentTexture", m_bodyTexture.c_str());	
			return 0;
		}
		StringReplace(meshSrc, "ui:resources\\avatars\\editor\\", "");
		StringReplace(meshSrc, ":geometry\\avatars\\editor\\", "");
		g->lw.WriteLn("meshSrc = ", meshSrc);
		std::vector<C3DObject*> objs = m_obj3d->GetLODGroup()->GetObjects();
		bool isFound = false;
		USES_CONVERSION;
		for (unsigned int i = 0; i < objs.size(); i++)
		{			
			std::string name = WCharStringToString(objs[i]->GetName());

			if (!objs[i]->GetAnimation())
			{
				continue;
			}

			if (!objs[i]->GetAnimation()->GetModel())
			{
				continue;
			}

			g->lw.WriteLn("meshSrc ", i, " = ", name);

			if (name.find(meshSrc) != -1)
			{				
				int textureID = objs[i]->GetAnimation()->GetModel()->GetTextureNumber(0);
				CTexture* texture = g->tm.GetObjectPointer(textureID);				
				sSet("__currentTexture", texture->GetPath().c_str());		
				g->lw.WriteLn(L"currentTexture = ", texture->GetPath());
				isFound = true;
				break;
			}
		}
		if (!isFound)
		{
			sSet("__currentTexture", "");		
		}
	}
	else	if ("get_old_texture" == command)
	{
		assert(argumentCount == 2);
		std::string meshSrc = message->GetNextString();
		StringReplace(meshSrc, "ui:resources\\avatars\\editor\\", "");
		MP_MAP<MP_STRING, MP_WSTRING>::iterator it = m_oldTexturesMap.find(MAKE_MP_STRING(meshSrc));
		if (it == m_oldTexturesMap.end())		
		{
			sSet("__oldTexture", "");		
		}
		else
		{
			sSet("__oldTexture", (*it).second.c_str());		
		}
	}
	else	if ("load_mesh" == command)
	{
		assert(argumentCount == 4);
		std::string partName = message->GetNextString();
		std::string partSrc = message->GetNextString();
		std::string clothDist = message->GetNextString();
		  
		if ((partSrc.find(".png") == -1) && (partSrc.find(".jpg") == -1))
		{
			HandleLoadMesh(partName, partSrc);
		}
		else
		{
			HandleChangeBodyTexture(partSrc);
		}

		ApplyCommonSettings();
	}
	else	if ("delete_mesh" == command)
	{
		assert(argumentCount == 2);
		std::string partName = message->GetNextString();
		DeleteEditparam(partName);
	}
	else	if ("set_special_color" == command)
	{
		assert(argumentCount == 1);
		m_isSpecialColor = true;
		OnAvatarPartLoaded(m_obj3d);		
	}
	else	if ("set_parameter" == command)
	{
		assert(argumentCount == 5);
		std::string commandID = message->GetNextString();
		std::string commandPath = message->GetNextString();
		std::string commandParam = message->GetNextString();
		std::string commandValue = message->GetNextString();

		while (commandPath.find(";") != -1)
		{
			int i = commandPath.find(";");
			std::string _tmpStrPart = commandPath;
			_tmpStrPart.erase(i, commandPath.size() - i);
			std::string tmpStrPart = _tmpStrPart;

			HandleSetParameterSafely(commandID, tmpStrPart, commandParam, commandValue);

			int j = i;
			while (commandPath[i] != '.')
			{
				i--;
			}
			commandPath.erase(i + 1, j-i);
		}
		
		std::string _commandPath = commandPath;
		HandleSetParameterSafely(commandID, _commandPath, commandParam, commandValue);
	}
	else	if ("create_composite_texture" == command)
	{
		assert(argumentCount == 4);
		
		std::string head = message->GetNextString();
		std::string str = message->GetNextString();
		std::string fileName = message->GetNextString();
		if ((m_compositeTextureManager) && (m_commandCache))
		{
			m_compositeTextureManager->LoadCompositeTextureStrings(fileName, m_commandCache);
		}

		if (m_commandCache)
		{
			m_commandCache->RefreshTime();
			m_commandCache->ExecuteQueue();
		}
	}
	else if ("photo_taked" == command)
	{
		assert(argumentCount == 1);
		m_isNeedToMakePhoto = false;
	}
	else if ("set_viewport" == command)
	{
		assert(argumentCount == 5);
		std::string sValueX = message->GetNextString();
		std::string sValueY = message->GetNextString();
		std::string sValueHeight = message->GetNextString();
		std::string sValueWidth = message->GetNextString();
		m_viewportHeight = rtl_atoi(sValueHeight.c_str());
		m_viewportWidth = rtl_atoi(sValueWidth.c_str());
		m_viewportX = rtl_atoi(sValueX.c_str());
		m_viewportY = rtl_atoi(sValueY.c_str());
	}
	else if ("set_viewport_visible" == command)
	{
		assert(argumentCount == 2);
		std::string sValue = message->GetNextString();
		if ( sValue == "1" || sValue == "true" )
		{
			m_isEditorVisible = true;
			m_isMyAvatar = true;					
		}
		else
		{
			m_isEditorVisible = false;
			m_isMyAvatar = false;
			m_isNeedToMakePhoto = true;
		}

		if (m_commandCache)
		{
			m_commandCache->SetEditorVisibleStatus(m_isEditorVisible);
		}

		if (m_compositeTextureManager)
		{
			m_compositeTextureManager->SetEditorVisible(m_isEditorVisible);
		}
	}
	else if ("set_viewport_camera" == command )
	{
		assert(argumentCount == 2);
		m_currentCameraMode = message->GetNextString();
	}
	else if ("import" == command)
	{
		assert(argumentCount == 3);	

		if (m_commandCache)
		{
			m_commandCache->RefreshTime();
		}
		std::string value = message->GetNextString();
		std::string male = message->GetNextString();
		UndressAvatar();
		InitAvatar(male == "male" ? true : false);
		if (m_obj3d)
		if (m_obj3d->GetLODGroup())
		{
			m_obj3d->GetLODGroup()->SetDrawState(true);
		}

		// затычка
		// women2 заменили всю одежду, поэтому неодетых women2 с неправильной одеждой подменяем на с дефолтной одеждой
		if (value.find("\\women2\\") != -1)
		if ((value.find("wom4_") == -1) && (value.find("Wom4_") == -1))
		if (value.find("<part") != -1)
		{
			value = "<avatar name=\"ui:resources\\avatars\\editor\\women2\\body\\mesh.xml\" head=\"1\"><part param_id=\"head\" template_id=\"0\" part_id=\"1\" /><part param_id=\"hair\" template_id=\"Wom4_hair02\" part_id=\"part01\" src=\"ui:resources\\avatars\\editor\\women2\\hair\\Wom4_hair02\\mesh.xml\"><params><param command=\"set_rgbv_color\" part=\"hair.Wom4_hair02.part01\" param=\"RGBV\" value=\"101 95 95 49\" /></params></part><part param_id=\"dress\" template_id=\"Wom4_suite01\" part_id=\"part01\" src=\"ui:resources\\avatars\\editor\\women2\\clothes\\Wom4_suite01\\mesh.xml\"><params><param command=\"set_rgbv_color\" part=\"dress.Wom4_suite01.part01\" param=\"RGBV\" value=\"255 255 255 50\" /></params></part><part param_id=\"boots\" template_id=\"Wom4_shoes09\" part_id=\"part01\" src=\"ui:resources\\avatars\\editor\\women2\\clothes\\Wom4_shoes09\\mesh.xml\"><params><param command=\"set_rgbv_color\" part=\"boots.Wom4_shoes09.part01\" param=\"RGBV\" value=\"255 255 255 14\" /></params></part><params><param command=\"interpolate_texture\" part=\"body.skincolor.skinColour2\" param=\"4\" value=\"0.00\" /><param command=\"set_diffuse_color\" part=\"head.0.skinColour2\" param=\"6\" value=\"137 96 255 80\" /><param command=\"set_diffuse_color\" part=\"head.eyecolor.skinColour2\" param=\"4\" value=\"17 170 214 48\" /></params></avatar>";
		}

		ImportTuneString(value);
		if (m_commandCache)
		{
			m_commandCache->ExecuteQueue();		
		}
	}
	else if ("finalize" == command)
	{
		m_isFinalizeRequired = true;
	}
	else if ("export" == command)
	{
		assert(argumentCount == 2);
		std::string value = message->GetNextString();
		ExportTuneString(value);
	}
	else if ("afterdelete" == command)
	{
#ifdef AVED_LOGGING
		g->lw.WriteLn("[LOG]CAvatarObject:: afterdelete");
#endif
		UndressAvatar();
		if (m_compositeTextureManager)
		{
			m_compositeTextureManager->DeleteAllCompositeTextures();
		}
	} 
	else if ("undress" == command)
	{
		UndressAvatar();
	}
	else if ("change_head" == command)
	{
		int headID = rtl_atoi(message->GetNextString().c_str());
		ChangeHead(headID);
		ApplyCommonSettings();
	}
	else if ("set_face_deformation" == command)
	{
		assert(argumentCount == 3);
		std::string animationName = message->GetNextString();
		std::string animationWeight = message->GetNextString();
		SetFaceDeformation(animationName, rtl_atoi(animationWeight.c_str()));
	}

	return 0;
}

void CAvatarObject::ChangeHead(int headID)
{	
	if (headID == m_headID)
	{
		if (m_compositeTextureManager)
 		if (m_compositeTextureManager->GetHeadID() != headID)
		{
			m_compositeTextureManager->SetHeadID(m_avatarParts[0].obj3d, headID);
			ApplyCommonSettings();
		}

		return;
	}

	m_headID = headID;

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)m_avatarParts[0].obj3d->GetAnimation();
	if (!sao)
	{
		return;
	}

	std::string path = sao->GetPath();
	int pos = path.find("\\body");

	if (pos != -1)
	{
		path = path.substr(0, pos);
		path += "\\body";
		if (headID != 1)
		{
			path += IntToStr(headID);
		}
		path += "\\mesh.xml";

		CSkeletonAnimationObject* oldSAO = sao;

		int saoID = g->skom.GetObjectNumber(m_avatarParts[0].obj3d, path);
		if (saoID != -1)
		{
			g->skom.DeleteObject(saoID);
		}

		saoID = g->skom.AddObject(m_avatarParts[0].obj3d, path);
		sao = g->skom.GetObjectPointer(saoID);
		bool isLoaded = sao->Load(path, false, NULL);
		if (!isLoaded)
		{
			return;
		}

		oldSAO->CopyMotionsTo(sao);
		
		m_avatarParts[0].obj3d->SetAnimation(sao);

		std::vector<C3DObject*> objs = m_avatarParts[0].obj3d->GetLODGroup()->GetObjects();
		std::vector<C3DObject*>::iterator it = objs.begin();
		std::vector<C3DObject*>::iterator itEnd = objs.end();

		for ( ; it != itEnd; it++)
		if (*it != m_avatarParts[0].obj3d)
		{
			CSkeletonAnimationObject* _sao = (CSkeletonAnimationObject*)((*it)->GetAnimation());
			if (!_sao)
			{
				continue;
			}
			sao->BindToSkeleton(_sao);
			_sao->SetParent(sao);
		}

		if (m_compositeTextureManager)
		{
			m_compositeTextureManager->SetHeadID(m_avatarParts[0].obj3d, headID);
		}
		ApplyCommonSettings();		

		oldSAO->CopyClipboxsTo(sao);
		g->skom.DeleteObject(oldSAO->GetID());
	}
}

void CAvatarObject::SetFaceDeformation(std::string animationName, int animationWeight )
{
	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject *)m_avatarParts[0].obj3d->GetAnimation();
	if (!sao)
	{
		return;
	}
	
	int motionID = g->motm.GetObjectNumber(animationName);
	if (motionID != -1 )
	{
		sao->BlendCycle(motionID,(float)animationWeight/100,0,true);
	}
}

unsigned long	CAvatarObject::ProcessMessage(int argumentCount, va_list arguments)
{
	assert(argumentCount > 0);
	if (argumentCount <= 0)	
	{
		return 0;
	}

	CVAListMessageObject message(arguments, argumentCount);
	return ProcessMessage(&message);
}

void	CAvatarObject::HandleSetDiffuse(std::string commandValue, std::string commandParam, std::string commandPart)
{
	if ((m_obj3d) && (m_obj3d->GetAnimation()))
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_obj3d->GetAnimation();
		if ((sao->GetPath().find("\\man") != -1) || (sao->GetPath().find("/man") != -1))
		if (commandPart.find("head.0.skin") != -1)
		{
			return;
		}
	}

	if (!m_compositeTextureManager)
	{
		return;
	}

	std::string _commandPart = commandPart;

	int len = commandPart.size() - 1;
	while (commandPart[len] != '.')
	{
		len--;
	}
	commandPart.erase(0, len + 1);

	int textureID = m_compositeTextureManager->GetCompositeTextureID(commandPart);
	if (textureID == -1)	
	{
		std::string commandID = "set_diffuse_color";
		std::string commandPart = _commandPart;
		std::string _commandParam = commandParam;
		std::string _commandValue = commandValue;
		if (m_commandCache)
		{
			m_commandCache->CacheApplyCommand(commandID, commandPart, _commandParam, _commandValue);
		}

		return;
	}

	int level = rtl_atoi(commandParam.c_str());

	int brightness;
	DWORD colour = GetDWORDFromRGBV(commandValue, brightness, m_diffuseColors[level][0], m_diffuseColors[level][1], m_diffuseColors[level][2]);
	DWORD checkColor = colour&0x00FFFFFF;
	if (0 == checkColor)
	{
		//colour = m_compositeTextureManager.GetDiffuse(textureID, level);
	} 
	m_brightnessLevel[level] = brightness;

	if (m_compositeTextureManager)
	{
		m_compositeTextureManager->SetDiffuse(textureID, level, colour);
		m_compositeTextureManager->UpdateTexture(textureID);
	}
}

void	CAvatarObject::HandleSetAlpharef(std::string commandValue, std::string commandParam, std::string commandPart)
{
	if (!m_compositeTextureManager)
	{
		return;
	}

	std::string _commandPart = commandPart;

	int len = commandPart.size() - 1;
	while (commandPart[len] != '.')
	{
		len--;
	}
	commandPart.erase(0, len + 1);

	int textureID = m_compositeTextureManager->GetCompositeTextureID(commandPart);
	if (textureID == -1)	
	{
		std::string commandID = "set_alpharef";
		std::string commandPart = _commandPart;
		std::string _commandParam = commandParam;
		std::string _commandValue = commandValue;
		if (m_commandCache)
		{
			m_commandCache->CacheApplyCommand(commandID, commandPart, _commandParam, _commandValue);
		}
			
		return;
	}

	int alphaRef = rtl_atoi(commandValue.c_str());
	int level = rtl_atoi(commandParam.c_str());
	
	if (m_compositeTextureManager)
	{
		m_compositeTextureManager->SetAlphaRef(textureID, level, (BYTE)alphaRef);
		m_compositeTextureManager->UpdateTexture(textureID);

		if ("faceTexture" == commandPart)
		{
			textureID = m_compositeTextureManager->GetCompositeTextureID("headTexture");
			m_compositeTextureManager->UpdateTexture(textureID);
		}
	}
}

void	CAvatarObject::HandleSetParameterSafely(std::string commandID, std::string commandPath, std::string commandParam, std::string commandValue) 
{
    if ((m_commandCache) && (m_commandCache->IsCommandMustBeCached(commandID)))
	{
		m_commandCache->CacheApplyCommand(commandID, commandPath, commandParam, commandValue);
	}
	else
	{
		HandleSetParameter(commandID, commandPath, commandParam, commandValue);
	}
}

void	CAvatarObject::HandleSetParameter(std::string commandID, std::string partPath, std::string commandParam, std::string commandValue)
{
	std::string _partPath = partPath;
	std::string _commandID = commandID;
	std::string _commandParam = commandParam;
	std::string _commandValue = commandValue;
	bool recordParam = true;

	unsigned int partNum = GetIndexPart(partPath);
	if (partNum < m_avatarParts.size())
	{
		m_avatarParts[partNum].RegisterParamChange(commandID, partPath, commandParam, commandValue);
		HandleSetParameter(m_avatarParts[partNum], _commandID, partPath, commandParam, commandValue);
	}
	// эти команды не могут относится к любой части тела
	else 
	{
		if ("set_body_height" == _commandID)
		{
			HandleSetBodyHeight(std::string(commandValue.c_str()));
		}
		else if ("set_alpharef" == _commandID)
		{
			HandleSetAlpharef(std::string(commandValue.c_str()), std::string(commandParam.c_str()), std::string(partPath.c_str()));
		}
		else if ("set_diffuse_color" == _commandID)
		{
			HandleSetDiffuse(std::string(commandValue.c_str()), std::string(commandParam.c_str()), std::string(partPath.c_str()));
		}
		else if ("interpolate_texture" == _commandID)
		{
			HandleInterpolateTexture(std::string(commandValue.c_str()), std::string(commandParam.c_str()), std::string(partPath.c_str()));
		}
		else
		{
			recordParam = false;
		}

		if (recordParam)
		{
			if (("set_alpharef" != _commandID) && 
				("set_diffuse_color" != _commandID) &&
				("interpolate_texture" != _commandID))
			{
				m_commonParameters.RegisterParamChange(_commandID, "common", "0", _commandValue);
			}
			else
			{
				m_commonParameters.RegisterParamChange(_commandID, _partPath, _commandParam, _commandValue);
			}
		}
	}

	if ((!recordParam) && (m_avatarParts.size() > 0))
	{
		if (_commandID != "set_rgbv_color")
		{			
			m_avatarParts[0].RegisterParamChange(_commandID, _partPath, _commandParam, _commandValue);
		}		
	}
}

void	CAvatarObject::HandleSetParameter(SAvatarObjPart& part, std::string commandID, std::string partPath, std::string commandParam, std::string commandValue)
{
	if ("set_blend_value" == commandID)
	{
		HandleSetBlendValue(part, commandParam.c_str(), commandValue.c_str());
	}
	else	if ("set_alpha_value" == commandID)
	{
		HandleSetAlphaValue(part, commandParam.c_str(), commandValue.c_str());
	}
	else	if ("set_rgbv_color" == commandID)
	{
		HandleSetRGBVColor(part, partPath.c_str(), commandParam.c_str(), commandValue.c_str());
	}
	else
	{
		assert(FALSE);
	}
}

float CAvatarObject::GetPartBlendWeight(SAvatarObjPart& part, const std::string& blendName)
{
	CSkeletonAnimationObject* partSAO = (CSkeletonAnimationObject*)part.obj3d->GetAnimation();
	if (partSAO)
	{
		std::string path = partSAO->GetPath();
		StringReplace(path, "\\mesh.xml", blendName);
		int blendID = g->bm.GetObjectNumber(path);
		if (-1 == blendID)
		{
			return 0;
		}

		CSkeletonAnimationObject* sao = g->bm.GetObjectPointer(blendID);
		return partSAO->GetBlendWeight(sao);
	}
	else
	{
		return 0;
	}
}

void	CAvatarObject::SetPartBlendWeight(SAvatarObjPart& part, const std::string& blendName, float weight)
{
	CSkeletonAnimationObject* partSAO = (CSkeletonAnimationObject*)part.obj3d->GetAnimation();
	if (partSAO)
	{
		std::string path = partSAO->GetPath();
		StringReplace(path, "\\mesh.xml", blendName);
		int blendID = g->bm.GetObjectNumber(path);
		if (-1 == blendID)
		{
			return;
		}

		CSkeletonAnimationObject* sao = g->bm.GetObjectPointer(blendID);
		partSAO->SetBlendWeight(sao, weight);
	}
}

void	CAvatarObject::HandleSetBlendValue(SAvatarObjPart& part, std::string commandParam, const std::string& commandValue)
{
	if (part.obj3d == NULL)	
	{
		return;
	}

	float blendWeight = StringToFloat(commandValue.c_str()) / 100.0f;
	if ( blendWeight > 1 )
	{
		blendWeight = 1;
	}
	if ( blendWeight < 0 )
	{
		blendWeight = 0;
	}

	std::string _commandParam = commandParam;
	SetPartBlendWeight(part, _commandParam, blendWeight);
}

void	CAvatarObject::HandleSetAlphaValue(SAvatarObjPart& part, const std::string& /*commandParam*/, const std::string& commandValue)
{
	// если существует базовый меш то
	if (part.obj3d != NULL)
	{		
		int alphaWeight = (int)rtl_atoi(commandValue.c_str());

		if (alphaWeight > 254)
		{
			alphaWeight = 254;
		}

		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)part.obj3d->GetAnimation();
		if (!sao)
		{
			return;
		}

		CModel* model = sao->GetModel();
		if (!model)
		{
			return;
		}

		for (int i = 0; i < model->GetTextureCount() - 1; i++)
		{
			int materialID = model->GetMaterial(i);

			if (-1 == materialID)
			{
				materialID = g->matm.AddObject("avatar_material_" + IntToStr(g->matm.GetCount()));
				model->ChangeMaterial(i, materialID);

				CMaterial* material = g->matm.GetObjectPointer(materialID);
				material->AddState("alphatest", "true");
				material->AddState("alphafunc", "greater");
				material->AddState("alphavalue", IntToStr(alphaWeight));
			}
			else
			{
				CMaterial* material = g->matm.GetObjectPointer(materialID);
				material->AddState("alphatest", "true");
				material->AddState("alphafunc", "greater");
				material->AddState("alphavalue", IntToStr(alphaWeight));
			}
		}
	}
}

void	CAvatarObject::HandleSetRGBVColor(SAvatarObjPart& part, const std::string& partPath, const std::string& /*commandParam*/, const std::string& commandValue)
{
	if (!part.obj3d)
	{
		return;
	}

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)part.obj3d->GetAnimation();
	if (!sao)
	{
		return;
	}

	CModel* model = sao->GetModel();
	if (!model)
	{
		return;
	}

	int brightness;
	unsigned char r, _g, b;
	DWORD colour = GetDWORDFromRGBV(commandValue, brightness, r, _g, b);

	MP_MAP<MP_STRING, int>::iterator it = m_clothesBrightness.find(MAKE_MP_STRING(partPath));
	if (it != m_clothesBrightness.end())
	{
		m_clothesBrightness.erase(it);
	}

	m_clothesBrightness.insert(MP_MAP<MP_STRING, int>::value_type(MAKE_MP_STRING(partPath), brightness));

	unsigned char* color = (unsigned char*)&colour;
	std::string colorString = IntToStr(color[0]);
	colorString += ";";
	colorString += IntToStr(color[1]);
	colorString += ";";
	colorString += IntToStr(color[2]);
	
	for (int i = 0; i < model->GetTextureCount() - 1; i++)
	{
		int materialID = model->GetMaterial(i);

		if (-1 == materialID)
		{
			materialID = g->matm.AddObject("avatar_material_" + IntToStr(g->matm.GetCount()));
			model->ChangeMaterial(i, materialID);

			CMaterial* material = g->matm.GetObjectPointer(materialID);
			material->AddState("color", colorString);
		}
		else
		{
			CMaterial* material = g->matm.GetObjectPointer(materialID);
			material->AddState("color", colorString);
		}
	}

	m_isComeToEndSetRGBVColor = true;

}

void	CAvatarObject::InitAvatar(bool isMaleAvatar)
{
	if (m_avatarParts.size() == 0)	
	{
		return;
	}

	if (isMaleAvatar)
	{
		m_isMale = true;
		ApplyCommonSettings();
	}
	else
	{
		m_isMale = false;
		ApplyCommonSettings();
	}

	if (m_compositeTextureManager)
	{
		m_compositeTextureManager->UpdateLinksToCompositeTextures(m_obj3d, m_avatarParts);
	}
}

void	CAvatarObject::ClearUserResources(SAvatarObjPart& desc)
{
	assert(desc.obj3d);

	desc.obj3d->GetLODGroup()->RemoveObject(desc.obj3d);
	g->o3dm.DeleteObject(g->o3dm.GetObjectNumber(desc.obj3d->GetName()));
	desc.obj3d = NULL;
}

void	CAvatarObject::DeleteEditparam(std::string editParamName)
{
	// не с 0 так как 0 - тело
	unsigned int i = 0;

	bool isFound = false;

	while (i < m_avatarParts.size())
	{
		std::string objName = m_avatarParts[i].objName.c_str();
		int j = objName.find(".");
		if (objName.substr(0, j) == editParamName)
		{
			std::string meshSrc = m_avatarParts[i].objSrc;
			StringReplace(meshSrc, "ui:resources\\avatars\\editor\\", "");
			
			{
				MP_MAP<MP_STRING, MP_WSTRING>::iterator it = m_oldTexturesMap.find(MAKE_MP_STRING(meshSrc));
				if (it != m_oldTexturesMap.end())
				{
					m_oldTexturesMap.erase(it);
				}

				it = m_newTexturesMap.find(MAKE_MP_STRING(meshSrc));
				if (it != m_newTexturesMap.end())
				{
					m_newTexturesMap.erase(it);
				}
			}

			ClearUserResources(m_avatarParts[i]);
			m_avatarParts.erase(m_avatarParts.begin() + i);

			isFound = true;
		}

		else
		{
			i++;
		}
	}

	if (!isFound)
	{
		std::vector<CLoadingCal3DObjectsTask*>::iterator it = m_loadingCal3DObjectsTasks.begin();
		for (; it != m_loadingCal3DObjectsTasks.end(); it++)
		{
			std::string objName = (*it)->GetName();
			int j = objName.find(".");
			if (objName.substr(0, j) == editParamName)
			{
				ClearTask(*it);
				m_loadingCal3DObjectsTasks.erase(it);
				return;
			}
		}		

		std::vector<SAvatarsObjNameAndSrc*>::iterator it2 = m_avatarsObjPartsInfo.begin();
		std::vector<SAvatarsObjNameAndSrc*>::iterator itEnd2 = m_avatarsObjPartsInfo.end();

		for ( ; it2 != itEnd2; it2++)
		{
			std::string objName = (*it2)->objName;
			int j = objName.find(".");
			if (objName.substr(0, j) == editParamName)
			{
				MP_DELETE(*it2);
				m_avatarsObjPartsInfo.erase(it2);
				break;
			}		
		}

		std::vector<SAsynchModel>::iterator it3 = m_asynchLoadedModels.begin();
		std::vector<SAsynchModel>::iterator it3End = m_asynchLoadedModels.end();
		for ( ; it3 != it3End; it3++)
		{
			std::string objName = (*it3).partName;
			int j = objName.find(".");
			if (objName.substr(0, j) == editParamName)
			{				
				g->rl->GetAsynchResMan()->RemoveLoadHandler((*it3).asynch, this/*, g->rl*/);
				m_asynchLoadedModels.erase(it3);
				break;
			}
		}
	}
}
bool	CAvatarObject::CheckTemplateLoad(std::string TemplateName)
{
	unsigned int i = 0;
	while (i < m_avatarParts.size())
	{
		std::string name = m_avatarParts[i].objName.c_str();
		int j = name.find(".");
		std::string editParamName = name.substr(0, j);		
		if ( editParamName == TemplateName)
		{
			return true;
		}
		else
		{
			i++;
		}
	}

	std::vector<CLoadingCal3DObjectsTask*>::iterator it = m_loadingCal3DObjectsTasks.begin();
	for (; it != m_loadingCal3DObjectsTasks.end(); it++)
	{
		std::string name = (*it)->GetName();
		int j = name.find(".");
		std::string editParamName = name.substr(0, j);		
		if ( editParamName == TemplateName)
		{
			return true;
		}
	}

	std::vector<SAvatarsObjNameAndSrc*>::iterator it2 = m_avatarsObjPartsInfo.begin();
	std::vector<SAvatarsObjNameAndSrc*>::iterator itEnd2 = m_avatarsObjPartsInfo.end();

	for ( ; it2 != itEnd2; it2++)
	{
		std::string name = (*it2)->objName;
		int j = name.find(".");
		std::string editParamName = name.substr(0, j);
		if ( editParamName == TemplateName)
		{
			return true;
		}
	}

	return false;
}

int		CAvatarObject::GetIndexPart(std::string partName)
{
	if (partName.find(".body") != -1)
	{
		return ((m_avatarParts.size() > 0) ? 0 : -1);
	}

	for (unsigned int i = 0; i < m_avatarParts.size(); i++)
	{
		SAvatarObjPart& part = m_avatarParts[i];
		if (part.objName == partName)
		{
			return i;
		}
	}

	return -1;
}

void	CAvatarObject::OnSetPropsContainer()
{
}

void CAvatarObject::FreeObject() 
{ 
	MP_DELETE_THIS;
}

CAvatarObject::~CAvatarObject()
{	
	m_obj3d->SetPlugin(NULL);
	UndressAvatar();
	if (m_compositeTextureManager)
	{
		MP_DELETE(m_compositeTextureManager);
}	
	if (m_commandCache)
	{
		MP_DELETE(m_commandCache);
	}
	if (m_fbo)
	{
		MP_DELETE(m_fbo);
	}

	//assert(g->scm.GetActiveScene());
	if (g->scm.GetActiveScene())
	{
		g->scm.GetActiveScene()->DeletePlugin(this);
	}
	
	ClearTasks();

	if (m_billboardTask)
	if ((m_billboardTask->IsPerformed()) || (g->taskm.RemoveTask(m_billboardTask)))
	{		
		if (m_billboardTask->GetData())
		{
			MP_DELETE_ARR_UNSAFE(m_billboardTask->GetData());
		}
		MP_DELETE(m_billboardTask);
	}
	else
	{
		m_billboardTask->DestroyResultOnEnd();
	}
}