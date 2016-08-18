
#include "StdAfx.h"
#include "GPUCompositeTextureManager.h"
#include "CompositeTextureManager.h"
#include "GlobalSingletonStorage.h"

CGPUCompositeTextureManager::CGPUCompositeTextureManager():
	m_obj3d(NULL)
{
}

void CGPUCompositeTextureManager::SetObject3D(C3DObject* obj3d)
{
	m_obj3d = obj3d;
}

void CGPUCompositeTextureManager::SetMainManager(void* manager)
{
	m_manager = manager;
}

void CGPUCompositeTextureManager::UpdateTexture(unsigned int textureID)
{
	CCompositeTextureManager* manager = (CCompositeTextureManager*)m_manager;
	//g->lw.WriteLn("UpdateTexture for ", manager->GetAvatarName());

	g->stp.SetMaterial(-1);
	g->stp.PrepareForRender();

	int _textureID = manager->m_textures[textureID].resultTextureID;
	CTexture* texture = g->tm.GetObjectPointer(_textureID);

	if (g->rs.GetBool(USE_VBO))
	{
		g->stp.SetVertexBuffer(-1, 0);
		g->stp.SetIndexBuffer(-1);
	}

	GLFUNC(glBindTexture)(GL_TEXTURE_2D, 0);
	g->tm.RefreshTextureOptimization();

	MP_NEW_P3(renderTarget, CRenderTarget, texture->GetTextureWidth(), texture->GetTextureHeight(), MODE_ALPHA);
	renderTarget->Create(true);
	renderTarget->Bind();
	if (!renderTarget->AttachColorTexture(texture->GetNumber()))
	{
		g->lw.WriteLn("AttachColorTexture failed for ", manager->GetAvatarName());
		MP_DELETE(renderTarget);
		return;
	}
	
	bool res = renderTarget->IsOk();
	assert(res);
	if (!res)
	{
		g->lw.WriteLn("renderTarget->IsOk failed for ", manager->GetAvatarName());
		MP_DELETE(renderTarget);
		return;
	}

	GLFUNC(glBindTexture)(GL_TEXTURE_2D, 0);

	GLFUNC(glClearColor)(1, 0, 0, 1);
	GLFUNC(glClear)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	g->stp.PushMatrix();

	g->cm.SetCamera2d();
	GLFUNC(glLoadIdentity)();
	g->stp.SetViewport(0, 0, texture->GetTextureWidth(), texture->GetTextureHeight());

	g->tm.EnableTexturing();
	g->tm.RefreshTextureOptimization();
	GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	
	g->stp.SetState(ST_ZWRITE, false);
	g->stp.SetState(ST_ZTEST, false);
	g->stp.SetState(ST_CULL_FACE, false);
	g->stp.SetDefaultStates();

	GLFUNC(glEnable)(GL_TEXTURE_2D);

	int renderNum = 0;
	for (int i = 0; i < manager->m_textures[textureID].levelCount; i++)
	{
		DWORD tmp = manager->m_textures[textureID].colors[i];	
		unsigned char* colors = (unsigned char*)&tmp;		

		if (i == 1)
		{
			continue;
		}

		if (!manager->m_textures[textureID].sprites[i])
		{
			continue;
		}
		
		if ((colors[0] == 0) && (colors[1] == 0) && (colors[2] == 0))
		{
			continue;
		}
		else
		{
			renderNum++;
		}

		if (i != 0)
		{
			if (!((renderNum == 1) && (manager->m_textures[textureID].isAddEnabled)))
			{
				if (manager->m_textures[textureID].alphaRefs[i] > 0)
				{
					g->stp.SetState(ST_ALPHATEST, true);
					g->stp.SetAlphaFunc(manager->m_textures[textureID].alphaFuncs[i], (float)manager->m_textures[textureID].alphaRefs[i] / 255.0f);
				}
				else
				{
					g->stp.SetState(ST_BLEND, true);
					g->stp.SetBlendFunc(GL_ONE, GL_ONE);
				}
			}
		}
				
		int _ix = manager->m_textures[textureID].texturePosX[i];
		int _iy = manager->m_textures[textureID].texturePosY[i];		
		
		if (g->rs.GetInt(TEXTURE_SIZE_DIV) != 1)
		{
			_ix /= g->rs.GetInt(TEXTURE_SIZE_DIV);
			_iy /= g->rs.GetInt(TEXTURE_SIZE_DIV);

			if (manager->m_textures[textureID].sprites[i])
			{
				CTexture* _texture = g->tm.GetObjectPointer(manager->m_textures[textureID].sprites[i]->GetTextureID(0));
				if (manager->m_textures[textureID].sprites[i]->GetWidth() == _texture->GetTextureWidth())
				if (manager->m_textures[textureID].sprites[i]->GetWidth() != texture->GetTextureWidth())
				{
					manager->m_textures[textureID].sprites[i]->SetSize(_texture->GetTextureWidth() / g->rs.GetInt(TEXTURE_SIZE_DIV),
						_texture->GetTextureHeight() / g->rs.GetInt(TEXTURE_SIZE_DIV));
				}
			}
		}
		
		if ((manager->m_textures[textureID].texturePosYNeedInvert[i]) && (manager->m_textures[textureID].sprites[i]))
		{
			_iy = texture->GetTextureHeight() - _iy - manager->m_textures[textureID].sprites[i]->GetHeight();
		}

		if (manager->m_textures[textureID].sprites[i])
		{
			manager->m_textures[textureID].sprites[i]->SetCoords(_ix,_iy);
		}
		
		g->tm.EnableTexturing();
		g->tm.RefreshTextureOptimization();
		GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		
		CSprite* sprite = manager->m_textures[textureID].sprites[i];		
		if (sprite)
		{
			int _textureID = sprite->GetTextureID(0);
			CTexture* _texture = g->tm.GetObjectPointer(_textureID);			
			_texture->SetYInvertStatus(true);
			
			int oldWidth = -1;
			int oldHeight = -1;

			sprite->SetCoords(sprite->GetX(), sprite->GetY() - (_texture->GetTextureRealHeight() - _texture->GetTextureHeight()) / g->rs.GetInt(TEXTURE_SIZE_DIV));
		
			if (_texture->GetTextureRealHeight() != _texture->GetTextureHeight())
			{
				oldWidth = _texture->GetTextureWidth();
				oldHeight = _texture->GetTextureHeight();
				_texture->SetTextureWidth(_texture->GetTextureRealWidth());
				_texture->SetTextureHeight(_texture->GetTextureRealHeight());
				//sprite->SetFullWorkRect();
			}

			if ((i >= 0) && (i <= 3))
			if ((colors[0] != colors[1]) || (colors[0] != colors[2]) || (colors[1] != colors[2]))
			{
				if (colors[0] == colors[1])
				{
					colors[2] = colors[0];
				}

				if (colors[2] == colors[1])
				{
					colors[0] = colors[2];
				}
			}	

			manager->m_textures[textureID].sprites[i]->SetColor(colors[0], colors[1], colors[2]);
			manager->m_textures[textureID].sprites[i]->Draw(NULL);

			if (oldWidth != -1)
			{
				int realWidth = _texture->GetTextureRealWidth();
				int realHeight = _texture->GetTextureRealHeight();
				_texture->SetTextureWidth(oldWidth);
				_texture->SetTextureHeight(oldHeight);
				_texture->SetTextureRealWidth(realWidth);
				_texture->SetTextureRealHeight(realHeight);
			}
		}

		g->stp.SetState(ST_ALPHATEST, false);
		g->stp.SetState(ST_BLEND, false);
	}

	g->stp.SetState(ST_ZWRITE, true);
	g->stp.SetState(ST_ZTEST, true);
	g->stp.SetState(ST_CULL_FACE, true);
	
	g->stp.SetState(ST_BLEND, true);
	g->stp.SetState(ST_ZWRITE, true);
	g->stp.SetState(ST_ZTEST, true);
	g->stp.SetState(ST_CULL_FACE, true);
	GLFUNC(glFrontFace)(GL_CCW);
	g->stp.PrepareForRender();

	g->stp.RestoreViewport();
	g->stp.PopMatrix();

	unsigned char* pixels = MP_NEW_ARR(unsigned char, texture->GetTextureWidth() * texture->GetTextureHeight() * 3);
	GLFUNC(glReadPixels)(0, 0, texture->GetTextureWidth(), texture->GetTextureHeight(), GL_RGB, GL_UNSIGNED_BYTE, pixels);

	renderTarget->DetachColorTexture();
	renderTarget->Unbind();
	
	GLFUNC(glBindTexture)(GL_TEXTURE_2D, texture->GetNumber());
	GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	texture->SetHasNoMipmaps(true);

	std::string fileName = manager->GetCacheFileName(textureID);
	manager->CompressTexture(pixels, texture, fileName, false);
	
	g->tm.RefreshTextureOptimization();
	
	MP_DELETE(renderTarget);

	g->stp.SetDefaultStates();
}

CGPUCompositeTextureManager::~CGPUCompositeTextureManager()
{
}