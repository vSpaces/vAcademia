
#include "StdAfx.h"
#include "SnapshotMaker.h"
#include "GlobalSingletonStorage.h"
#include "nrmLine.h"
#include "nrmText.h"
#include "nrmAnim.h"
#include <algorithm>
#include "nrmVideo.h"
#include "nrmImage.h"
#include "nrmPlugin.h"
#include "LastScreen.h"
#include "nrmViewport.h"
#include "nrm3DObject.h"
#include "RenderTarget.h"
#include "nrmImageFrame.h"
#include "HelperFunctions.h"
#include "MouseController.h"
#include "PlatformDependent.h"
#include "WhiteboardManager.h"

bool MakeSimpleSnapshot(CSprite* sprite, SSnapshotParams& params)
{
	int textureID = sprite->MakeSnapshot(params.toSystemMemory);		

	CTexture* texture = g->tm.GetObjectPointer(textureID); 
	if (params.fileName.size() > 0)
	{
		USES_CONVERSION;
		std::wstring wFileName = A2W( params.fileName.c_str());
		g->tm.SaveTexture(texture, wFileName);
	}			

	return true;
}

void StartRender(SSnapshotParams& params, std::vector<C3DObject *>& visible3DObjects)
{
	float sizeX, sizeY, sizeZ;
	if (params.isMiniMap)
	{
		std::vector<C3DObject *>::iterator it = g->o3dm.GetLiveObjectsBegin();
		std::vector<C3DObject *>::iterator itEnd = g->o3dm.GetLiveObjectsEnd();

		for ( ; it != itEnd; it++)
		{
			(*it)->SaveVisibility();

			if ((*it)->GetAnimation())
			{
				(*it)->SetVisible(false);
			}
			else
			{
				(*it)->GetSizes(sizeX, sizeY, sizeZ);
				const float maxSmallSize = 200.0f;
				if ((sizeX < maxSmallSize) && (sizeY < maxSmallSize) && (sizeZ < maxSmallSize))
				{
					(*it)->SetVisible(false);
				}
			}
		}
	}

	if (params.isNoDynamicObjects)
	{
		g->rs.SetBool(NO_DYNAMIC_OBJECTS, true);
	}

	if (!params.isMiniMap)
	if (visible3DObjects.size() > 0)
	{
		// фото одежды без аватара
		bool isPhotoWithoutDress = (params.fileName.find("dress_av_photo.png") == -1);

		std::vector<C3DObject *>::iterator it = g->o3dm.GetLiveObjectsBegin();
		std::vector<C3DObject *>::iterator itEnd = g->o3dm.GetLiveObjectsEnd();

		for ( ; it != itEnd; it++)
		{
			(*it)->SaveVisibility();
			(*it)->SetVisible(false);
		}

		it = visible3DObjects.begin();
		itEnd = visible3DObjects.end();

		for ( ; it != itEnd; it++)
		{
			(*it)->SetVisible(true);
			
			if ((*it)->GetAnimation())
			{
				(*it)->GetLODGroup()->SetVisibleAll(true);
				if ((*it)->GetLODGroup()->IsSetToInitialPose())
				{
					(*it)->GetLODGroup()->ClearVisibilityCheckResults();
				}	
			}

			if (!isPhotoWithoutDress) 
			{
				(*it)->SetVisible(false);
			}
		}
	}
}

void EndRender(SSnapshotParams& params, bool isNeedToRestoreVisibility)
{
	if ((params.isMiniMap) || (isNeedToRestoreVisibility))
	{
		std::vector<C3DObject *>::iterator it = g->o3dm.GetLiveObjectsBegin();
		std::vector<C3DObject *>::iterator itEnd = g->o3dm.GetLiveObjectsEnd();

		for ( ; it != itEnd; it++)
		{
			(*it)->RestoreVisibility();
		}
	}

	g->rs.SetBool(NO_DYNAMIC_OBJECTS, false);
	g->rs.SetBool(SHOW_SKY, true);
	g->stp.SetDefaultStates();

	if (!params.isNo2D)
	{
		gRM->scene2d->RestoreVisibility();
	}
	else
	{
		gRM->scene2d->SetVisibleNow(true);
	}
}

CViewport* CollectVisibleObjects(mlMedia** appVisibles, std::vector<C3DObject *>& visible3DObjects, SSnapshotParams& params)
{
	CViewport* viewport = NULL;

	while (*appVisibles)
	{
		mlMedia*	pVisible = *appVisibles;
		bool isVisible = true;
			
		if (pVisible->GetIVisible())
			isVisible = pVisible->GetIVisible()->GetAbsVisible();
		
		if (pVisible->GetIViewport())
		{
			nrmViewport* rmViewport = (nrmViewport*)pVisible->GetSafeMO();
			viewport = rmViewport->m_viewport;

			if (pVisible->GetBooleanProp("shadowsDisabled"))
			{
				params.isShadowsDisabled = true;
			}

			if (pVisible->GetBooleanProp("noSky"))
			{
				g->rs.SetBool(SHOW_SKY, false);
			}

			if (pVisible->GetBooleanProp("transparentBackground"))
			{
				params.isTransparentBackground = true;
				g->rs.SetBool(SHOW_SKY, false);
			}
		}
		if (isVisible)
		if ((pVisible->GetType() == MLMT_CHARACTER) || (pVisible->GetType() == MLMT_OBJECT)
			|| (pVisible->GetType() == MLMT_COMMINICATION_AREA))
		{
			nrm3DObject* rm3DObj = (nrm3DObject*)pVisible->GetSafeMO();
			if (rm3DObj)
			if (rm3DObj->m_obj3d)
			{
				visible3DObjects.push_back(rm3DObj->m_obj3d);
			}
		}			
		else if ((pVisible->GetType() == MLMT_IMAGE) || (pVisible->GetType() == MLMT_IMAGE_FRAME) || 
				(pVisible->GetType() == MLMT_VIDEO) || (pVisible->GetType() == MLMT_ANIMATION) || 
				(pVisible->GetType() == MLMT_TEXT) || (pVisible->GetType() == MLMT_BROWSER) || (pVisible->GetType() == MLMT_LINE))
		{
			if (pVisible->GetType() == MLMT_IMAGE)
			{
				nrmImage* rmImage = (nrmImage*)pVisible->GetSafeMO();
				if (rmImage->m_sprite)
				{
#ifndef USE_VISIBLE_DESTINATION
					rmImage->m_sprite->SetVisible(true);
#else
					rmImage->m_sprite->SetVisible(true, 10);
#endif
				}
			}
			else if (pVisible->GetType() == MLMT_IMAGE_FRAME)
			{
				nrmImageFrame* rmImageFrame = (nrmImageFrame*)pVisible->GetSafeMO();
				if (rmImageFrame->m_paper)
				{
#ifndef USE_VISIBLE_DESTINATION
					rmImageFrame->m_paper->SetVisible(true);
#else
					rmImageFrame->m_paper->SetVisible(true, 9);
#endif
					}
				}
				else if (pVisible->GetType() == MLMT_ANIMATION)
				{
					nrmAnim* rmAnimation = (nrmAnim*)pVisible->GetSafeMO();
					if (rmAnimation->m_sprite)
					{
#ifndef USE_VISIBLE_DESTINATION
						rmAnimation->m_sprite->SetVisible(true);
#else
						rmAnimation->m_sprite->SetVisible(true, 11);
#endif
					}
				}
				else if (pVisible->GetType() == MLMT_VIDEO)
				{
//					nrmVideo* rmVideo = (nrmVideo*)pVisible->GetSafeMO();
//					rmVideo->m_sprite->SetVisible(true);
				}
				else if (pVisible->GetType() == MLMT_TEXT)
				{
					nrmText* rmText = (nrmText*)pVisible->GetSafeMO();
					if (rmText->m_text)
					{
#ifndef USE_VISIBLE_DESTINATION
						rmText->m_text->SetVisible(true);
#else
						rmText->m_text->SetVisible(true, 12);
#endif
					}
				}
				else if (pVisible->GetType() == MLMT_LINE)
				{
					nrmLine* rmLine = (nrmLine*)pVisible->GetSafeMO();
					if (rmLine->m_line)
					{
#ifndef USE_VISIBLE_DESTINATION
						rmLine->m_line->SetVisible(true);
#else
						rmLine->m_line->SetVisible(true, 12);
#endif
					}
				}
				else if (pVisible->GetType() == MLMT_BROWSER)
				{
					nrmBrowser* rmBrowser = (nrmBrowser*)pVisible->GetSafeMO();
					if (rmBrowser->m_sprite)
					{ 
#ifndef USE_VISIBLE_DESTINATION
						rmBrowser->m_sprite->SetVisible(true);
#else
						rmBrowser->m_sprite->SetVisible(true, 13);
#endif
					}
				}
			}	

			appVisibles++;
	}

	return viewport;
}

bool IsVideoCardGood()
{
	return (g->gi.GetVendorID() != VENDOR_INTEL) && (g->gi.GetVendorID() != VENDOR_SIS);
}

bool MakeComplexSnapshot(mlMedia** appVisibles, CSprite* sprite, SSnapshotParams& params)
{
	int startTime = g->tp.GetMicroTickCount();

	COcclusionQuery _occlusionQuery;
	COcclusionQuery* occlusionQuery = ((g->gi.GetVendorID() != VENDOR_NVIDIA) || (params.isOnly2D)) ? NULL : &_occlusionQuery;
	
	if (!g->rs.GetBool(SHADERS_USING))
	{
		params.isPostEffectEnabled = false;
	}

	if (g->ne.isIconicAndIntel)
	{
		return false;		
	}

	bool isShadowsDisabled = false;
	bool isTransparentBackground = false;
	bool isSnapshotMaked = false;
	std::vector<C3DObject *> visible3DObjects;	    	
	unsigned int textureID;

	if (!params.isNo2D)
	{
		gRM->scene2d->SaveVisibility();
		gRM->scene2d->HideAll();
	}
	else
	{
		gRM->scene2d->SetVisibleNow(false);
	}

	CViewport* viewport = CollectVisibleObjects(appVisibles, visible3DObjects, params);	

	StartRender(params, visible3DObjects);	

	int sizeDivKoef = 1;
	if ((params.isOnly2D) && (g->gi.GetVendorID() == VENDOR_INTEL) && (viewport) && ((viewport->GetWidth() > 512) || (viewport->GetHeight() > 512)))
	{
		sizeDivKoef = 2;
	}

	if ((viewport) && (viewport->GetWidth() > 0) && (viewport->GetHeight() > 0))
	{
        unsigned int width = 0;
		unsigned int height = 0;
		int x = 0;
		int y = 0;
		g->stp.GetSize(x, y, width, height);
		g->stp.SetSize(0, 0, viewport->GetWidth(), viewport->GetHeight());

		if (sizeDivKoef != 1)
		{
			viewport->SetSize(viewport->GetWidth() / sizeDivKoef, viewport->GetHeight() / sizeDivKoef);
		}

		if (g->rs.GetInt(FOV) == 90) // съемка панорамы
			viewport->setPanoramaMode(true);

		int textureWidth = GetMinBigger2st(viewport->GetWidth());
		int textureHeight = GetMinBigger2st(viewport->GetHeight());
		CRenderTarget* renderTarget = NULL;
		if (IsVideoCardGood())
		{
			MP_NEW_V3(renderTarget, CRenderTarget, textureWidth, textureHeight, CFrameBuffer::depth32);
		}

		if (g->rs.GetInt(FOV) == 90)
		{
			CFrameBuffer* frameBuffer = renderTarget->GetFrameBuffer();
			frameBuffer->SetMultisamplePower( 4);
			g->lod.SetLODScale(0.01);
		}

		if (IsVideoCardGood())
		{				
			renderTarget->Create(true);
		}

		bool isNeededToCreateTexture = true; 
		bool isNeededToSetSpriteParams = true;

		if (sprite->GetTextureID(0) != -1)
		{
			CTexture* texture = g->tm.GetObjectPointer(sprite->GetTextureID(0));
			if ((texture->IsTransparent()) && (texture->GetTextureRealWidth() == textureWidth)
				&& (texture->GetTextureRealHeight() == textureHeight))
			{
				isNeededToCreateTexture = false;					
				isNeededToSetSpriteParams = false;
			}
		}
		//isNeededToCreateTexture = true; // nap
			
		CHDRPostEffect* postEffect = NULL;
		if ((params.isPostEffectEnabled) && (IsVideoCardGood()))
		{
			MP_NEW_V2(postEffect, CHDRPostEffect, textureWidth, textureHeight);				
		}
		
		if (IsVideoCardGood())
		{
			textureID = isNeededToCreateTexture ? renderTarget->CreateColorTexture(GL_BGRA_EXT, GL_RGBA, true) : g->tm.GetObjectPointer(sprite->GetTextureID(0))->GetNumber();
			renderTarget->Bind();
			renderTarget->AttachColorTexture(textureID);
			renderTarget->Unbind(); 
		}
		else
		{
			textureID = isNeededToCreateTexture ? CFrameBuffer::CreateColorTexture(GL_TEXTURE_2D, textureWidth, textureHeight, GL_BGR_EXT, GL_RGB, true) : sprite->GetTextureID(0);
		}

		if (isNeededToCreateTexture)
		{
			if (sprite->GetTextureID(0) != -1)
			{
				CTexture* texture = g->tm.GetObjectPointer(sprite->GetTextureID(0));
				texture->MarkDeleted();
			}

			textureID = g->tm.ManageGLTexture("script_snapshot", textureID, viewport->GetWidth(), viewport->GetHeight());
			sprite->SetTextureID(textureID);
				
			if (isNeededToSetSpriteParams)
			{
				sprite->SetSize(viewport->GetWidth(), viewport->GetHeight());
				sprite->SetFullWorkRect();
			}
		}
		else
		{
			textureID = sprite->GetTextureID(0);
		}

		CTexture* texture = g->tm.GetObjectPointer(textureID);
		texture->UpdateCurrentFiltration(GL_LINEAR, GL_LINEAR);
		isSnapshotMaked = true;

		if (sizeDivKoef != 1)
		{
			texture->SetMaxDecreaseKoef(sizeDivKoef);
			g->rs.SetInt(SCISSORS_SCALE, sizeDivKoef);
		}
		else
		{
			texture->SetMaxDecreaseKoef(1);
		}

		if (isNeededToCreateTexture)
		{
			texture->SetTransparentStatus(true);
			texture->SetTextureRealWidth(textureWidth);
			texture->SetTextureRealHeight(textureHeight);
			texture->AddRef();
			texture->SetHasNoMipmaps(true);
			if (params.isOnly2D)
			{
				texture->SetYInvertStatus(false);
				texture->SetXInvertStatus(false);
			}
			else
			{
				texture->SetYInvertStatus(true);
			}
		}

		if (renderTarget)
		{
			bool isOk = renderTarget->IsOk();
			if ((postEffect) && (!postEffect->IsOk()))
			{
				isOk = false;
			}
			if (!isOk)
			{
				MP_DELETE(renderTarget);
				renderTarget = NULL;
				if (postEffect)
				{
					MP_DELETE(postEffect);
					postEffect = NULL;
				}
			}
		}

		{
			g->ne.SetWorkMode(WORK_MODE_SNAPSHOTRENDER);

			bool oldShadowSettings = false;
			bool isShadersUsing = false;
			if (isShadowsDisabled)
			{
				oldShadowSettings = g->rs.GetBool(SHADOWS_ENABLED);
				isShadersUsing = g->rs.GetBool(SHADERS_USING);
				g->rs.SetBool(SHADOWS_ENABLED, false);
				g->rs.SetBool(SHADERS_USING, false);
			}

			if (g->rs.GetBool(SHADOWS_ENABLED))
			{
				g->shdw.CheapSoftShadowSupport(false);
			}

			CViewport* lastViewport = NULL;

			int lastViewportX = viewport->GetX();
			int lastViewportY = viewport->GetY();
			viewport->SetOffset(0, 0);

			std::vector<CScene*>::iterator it = g->scm.GetLiveObjectsBegin();
			lastViewport = (*it)->GetViewport();
			(*it)->SetViewport(viewport);
			CBasePostEffect* lastPostEffect = (*it)->GetPostEffect();
			(*it)->SetPostEffect(NULL);
			(*it)->DisableOnEnd3DEventsForNextFrame();			
				
			if (renderTarget)
			{
				if (postEffect)
				{
					postEffect->OnSceneDrawStart(viewport);
					g->cm.SetFov((float)(g->rs.GetInt(FOV) - 45), (float)viewport->GetWidth() / (float)viewport->GetHeight());
				}
				else
				{
					renderTarget->Bind();
				}					
			}
			else
			{
				g->ne.EndFrame();
			}

			g->stp.SaveStates();

			glStencilMask(0xFFFFFFFF);
			glClearStencil(127);
			if (!isTransparentBackground)
			{
				if (params.useBackgroundColor)
				{
					glClearColor(params.colorR, params.colorG, params.colorB, 1.0f);
				}
				else
				{
					glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
				}
			}
			else
			{
				glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
			}			

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);				
			if (params.isDisableTransparent)
			{
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
			}

			// начало отрисовки
			if (occlusionQuery)
			{
				occlusionQuery->StartQuery();
			}

			g->shdw.NewFrame();
			g->stp.SetDefaultStates();
			g->tm.RefreshTextureOptimization();
			g->stp.SetMaterial(-1);
			g->stp.PrepareForRender();

			g->mr.SetMode(MODE_RENDER_ALL);

			CCamera3D* old = g->cm.GetActiveCamera();
			g->cm.SetActiveCamera(viewport->GetCamera());

			gRM->nrMan->SaveCloudsVisibility();

			if (params.isOnly2D)
			{	
				if (!IsVideoCardGood())
				{
					gRM->nrMan->RenderScene(false);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
				}
				g->stp.PushMatrix();
				g->cm.SetFov(0.0f, (float)viewport->GetWidth() / (float)viewport->GetHeight());
				g->stp.SetViewport(0, 0, viewport->GetWidth(), viewport->GetHeight());					
				g->stp.SetSize(0, 0, viewport->GetWidth() * sizeDivKoef, viewport->GetHeight() * sizeDivKoef);
				glScalef(1, -1, 1);
				gRM->scene2d->Draw();
				g->stp.PopMatrix();
			}
			else
			{
				if (postEffect)
				{
					gRM->scene2d->SetVisibleNow(false);
				}					
				gRM->nrMan->RenderScene(false);						
				if (postEffect)
				{
					gRM->scene2d->SetVisibleNow(!params.isNo2D);						
				}
			}		

			if (occlusionQuery)
			{
				occlusionQuery->EndQuery();
			}

			if ((isTransparentBackground) || (params.useBackgroundColor))
			{
				glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
			}

			if (renderTarget)
			{
				if (postEffect)
				{
					postEffect->OnSceneDrawEnd(viewport, renderTarget);

					renderTarget->Bind();
					g->stp.PushMatrix();
					g->cm.SetFov((float)(g->rs.GetInt(FOV) - 45), (float)viewport->GetWidth() / (float)viewport->GetHeight());
					g->stp.SetViewport(0, 0, viewport->GetWidth(), viewport->GetHeight());											
					gRM->scene2d->Draw();
					g->stp.PopMatrix();
					renderTarget->DetachColorTexture();
					renderTarget->Unbind();
				}
				else
				{
					renderTarget->DetachColorTexture();
					renderTarget->Unbind();
				}
			}
			else
			{				
				g->tm.BindTexture(texture->GetID());								
				glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, texture->GetTextureRealWidth(), texture->GetTextureRealHeight());				
				glBindTexture(GL_TEXTURE_2D, 0);
				g->tm.RefreshTextureOptimization();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);				
			}

			// конец отрисовки
			gRM->nrMan->RestoreCloudsVisibility();				
				
			g->cm.SetActiveCamera(old);

			g->stp.RestoreStates();

			if (isShadowsDisabled)
			{
				g->rs.SetBool(SHADOWS_ENABLED, oldShadowSettings);
				g->rs.SetBool(SHADERS_USING, isShadersUsing);
			}

			g->ne.SetWorkMode(WORK_MODE_NORMALRENDER);

			(*it)->SetPostEffect(lastPostEffect);
			(*it)->SetViewport(lastViewport);

			viewport->SetOffset(lastViewportX, lastViewportY);

			if (g->rs.GetBool(SHADOWS_ENABLED))
			{
				g->shdw.CheapSoftShadowSupport(true);
			}
		}

		if (isNeededToCreateTexture)
		{
			sprite->OnChanged(0);
		}

		if (renderTarget)
		{				
			MP_DELETE(renderTarget);							
		}			

		if (postEffect)
		{				
			MP_DELETE(postEffect);
		}		

		g->stp.SetSize(x, y, width, height);
		if (sizeDivKoef != 1)
		{
			viewport->SetSize(viewport->GetWidth() * sizeDivKoef, viewport->GetHeight() * sizeDivKoef);
			g->rs.SetInt(SCISSORS_SCALE, 1);
		}

		if (g->rs.GetInt(FOV) == 90) // съемка панорамы
		{
			viewport->setPanoramaMode(false);
			g->lod.SetLODScale(1);
		}
		if (params.isDisableTransparent)
		{
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		}
	}

	if (isSnapshotMaked)
	{
		CTexture* texture = g->tm.GetObjectPointer(textureID);

		if (params.fileName.size() > 0)
		{
			USES_CONVERSION;
			std::wstring wFileName = A2W( params.fileName.c_str());
			g->tm.SaveTexture(texture, wFileName);
		}			
	}	

	EndRender(params, visible3DObjects.size() > 0);

	int endTime = g->tp.GetMicroTickCount();

	if (endTime - startTime > 10000)
	{
		g->lw.WriteLn("MakeSnapshot in ", endTime - startTime, " mcs minimap: ", params.isMiniMap ? "yes" : "no");
	}
	
	return	((!occlusionQuery) || (occlusionQuery->GetFragmentCount() > 0));	
}

bool MakeSnapshotImpl(mlMedia** appVisibles, CSprite*& sprite, SSnapshotParams& params)
{
	if (!sprite)
	{
		std::string name = params.imageName;
		
		if (name.size() == 0)
		{
			name = "snapshot_";
			name += IntToStr(g->sprm.GetCount()).c_str();
		}
		
		int spriteID = g->sprm.AddObject(name);
		sprite = g->sprm.GetObjectPointer(spriteID);
		gRM->scene2d->AddSprite(spriteID, 0, 0);		
	}	
			
	if (NULL == *appVisibles)
	{
		if (g->ne.isIconicAndIntel)
		{
			return false;
		}

		return MakeSimpleSnapshot(sprite, params);		
	}
	else
	{	
		return MakeComplexSnapshot(appVisibles, sprite, params);
	}
}
