
#include "StdAfx.h"
#include "CPUCompositeTextureManager.h"
#include "CompositeTextureManager.h"
#include "GlobalSingletonStorage.h"

CCPUCompositeTextureManager::CCPUCompositeTextureManager():
	m_obj3d(NULL)
{
}

void CCPUCompositeTextureManager::SetMainManager(void* manager)
{
	m_manager = manager;
}

template <int T, bool C, bool ignoreAlpha>
void DrawImage(unsigned char* src, unsigned int srcWidth, unsigned int srcHeight,
											unsigned char* dest, unsigned int destWidth, unsigned int destHeight,
											int destX, int destY,
											unsigned char alphaValue,
											unsigned char* colors, unsigned char scale)
{
	if ((!src) || (0 == srcWidth) || (0 == srcHeight))
	{
		assert(false);
		return;
	}

	if ((!dest) || (0 == destWidth) || (0 == destHeight))
	{
		assert(false);
		return;
	}

	if (destX + destWidth > srcWidth)
	{
		destX = 0;
	}

	if (destY != 0)
	{
		if (destY < 0)
		{
			destHeight += destY;
			destY = 0;
		}

		destY = srcHeight - destY - 1 - destHeight / scale;		
	}

	unsigned int lineOffset = srcWidth * 4;
	unsigned int destOffset = destX * 4 + destY * lineOffset;
	unsigned int srcLineSize = destWidth * 4 / scale;
	unsigned int destLineSize = destWidth * T;
	destX = 0;
	destHeight /= scale;
	for (unsigned int k = 0; k < destHeight; k++)
	{
		int endX = destOffset + srcLineSize; 
		for (int x = destOffset; x < endX; x += 4, destX += T * scale)
		if (T < 4)
		{
			if (C)
			{
				src[x] = (dest[destX] * colors[0]) >> 8;
				src[x + 1] = (dest[destX + 1] * colors[1]) >> 8;
				src[x + 2] = (dest[destX + 2] * colors[2]) >> 8;			
			}
			else
			{
				src[x] = (dest[destX + 2] * colors[0]) >> 8;
				src[x + 1] = (dest[destX + 1] * colors[1]) >> 8;
				src[x + 2] = (dest[destX] * colors[2]) >> 8;
			}
			src[x + 3] = 255;			
		}
		else if (dest[destX + 3] >= alphaValue)
		{			
			unsigned char alpha = !ignoreAlpha ? dest[destX + 3] : 255;
			unsigned char invAlpha = !ignoreAlpha ? (255 - alpha) : 0;
			if (C)
			{
				src[x] = (dest[destX] * colors[0]  + src[x] * invAlpha) >> 8;
				src[x + 1] = (dest[destX + 1] * colors[1]  + src[x + 1] * invAlpha) >> 8;
				src[x + 2] = (dest[destX + 2] * colors[2]  + src[x + 2] * invAlpha) >> 8;
			}
			else
			{
				src[x] = (dest[destX + 2] * colors[0]   + src[x] * invAlpha) >> 8;
				src[x + 1] = (dest[destX + 1] * colors[1]  + src[x + 1] * invAlpha) >> 8;
				src[x + 2] = ( dest[destX] * colors[2]  + src[x + 2] * invAlpha) >> 8;
			}
			src[x + 3] = 255;			
		}

		destX += destLineSize * (scale - 1);

		destOffset += lineOffset;
	}
}

void CCPUCompositeTextureManager::UpdateTexture(unsigned int textureID)
{
	//int dt = 0;
	//int t1 = g->tp.GetMicroTickCount();

	CCompositeTextureManager* manager = (CCompositeTextureManager*)m_manager;

	int _textureID = manager->m_textures[textureID].resultTextureID;
	CTexture* texture = g->tm.GetObjectPointer(_textureID);

	unsigned int srcWidth = texture->GetTextureWidth();
	unsigned int srcHeight = texture->GetTextureHeight();
	int srcSize = srcWidth * srcHeight * 4;
	unsigned char* src = MP_NEW_ARR(unsigned char, srcSize);
	
	int renderNum = 0;
	for (int i = 0; i < manager->m_textures[textureID].levelCount; i++)
	{
		DWORD tmp = manager->m_textures[textureID].colors[i];	
		unsigned char* colors = (unsigned char*)&tmp;		

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

		bool isAlphaTest = false;		
		unsigned char alphaValue = 0;

		if (i != 0)
		{
			if (!((renderNum == 1) && (manager->m_textures[textureID].isAddEnabled)))
			{
				if (manager->m_textures[textureID].alphaRefs[i] > 0)
				{
					isAlphaTest = true;
					alphaValue = manager->m_textures[textureID].alphaRefs[i];					
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
				if (i > 1)
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

			//manager->m_textures[textureID].sprites[i]->SetColor(colors[0], colors[1], colors[2]);
			//manager->m_textures[textureID].sprites[i]->Draw(NULL);			
//			int _t1 = g->tp.GetMicroTickCount();
			unsigned char* dest = (unsigned char *)_texture->GetRawData();
//			int _t2 = g->tp.GetMicroTickCount();
//			dt += _t2 - _t1;
			unsigned char sizeDiv = (i < 2) ? 1 : (unsigned char)g->rs.GetInt(TEXTURE_SIZE_DIV);
			if (((unsigned int)_texture->GetTextureWidth() > srcWidth) && (i < 2))
			{
				sizeDiv = (unsigned char)g->rs.GetInt(TEXTURE_SIZE_DIV);
			}

			if (i != 1)
			if (_texture->IsTransparent())
			{
				if (_texture->IsRawDirectChannelOrder())
				{
					DrawImage<4, true, false>(src, srcWidth, srcHeight, dest, _texture->GetTextureWidth(), _texture->GetTextureHeight(),
						sprite->GetX(), sprite->GetY(), alphaValue, colors, sizeDiv);
				}
				else
				{
					DrawImage<4, false, false>(src, srcWidth, srcHeight, dest, _texture->GetTextureWidth(), _texture->GetTextureHeight(),
						sprite->GetX(), sprite->GetY(), alphaValue, colors, sizeDiv);
				}
			}
			else
			{
				if (_texture->IsRawDirectChannelOrder())
				{
					DrawImage<3, true, false>(src, srcWidth, srcHeight, dest, _texture->GetTextureWidth(), _texture->GetTextureHeight(),
						sprite->GetX(), sprite->GetY(), alphaValue, colors, sizeDiv);
				}
				else
				{
					DrawImage<3, false, false>(src, srcWidth, srcHeight, dest, _texture->GetTextureWidth(), _texture->GetTextureHeight(),
						sprite->GetX(), sprite->GetY(), alphaValue, colors, sizeDiv);
				}
			}		
			if (!_texture->IsKeepInSystemMemory())
			{
				MP_DELETE_ARR(dest);
			}

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
	}	

	//int t2 = g->tp.GetMicroTickCount();
	
	GLFUNC(glBindTexture)(GL_TEXTURE_2D, texture->GetNumber());
	GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, GL_RGBA, srcWidth, srcHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, src);
	GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	texture->SetHasNoMipmaps(true);

	//int t3 = g->tp.GetMicroTickCount();

	//if (!manager->m_isMyAvatar)
	{				
		std::string fileName = manager->GetCacheFileName(textureID);
		manager->CompressTexture(src, texture, fileName);
	}		

	g->tm.RefreshTextureOptimization();

	//int t4 = g->tp.GetMicroTickCount();

	//g->lw.WriteLn("UpdateTexture ", t2 - t1, " ", t3 - t2, " ", t4 - t3, " ", dt, " ", m_obj3d->GetName());
}

void CCPUCompositeTextureManager::SetObject3D(C3DObject* obj3d)
{
	m_obj3d = obj3d;
}

CCPUCompositeTextureManager::~CCPUCompositeTextureManager()
{
}