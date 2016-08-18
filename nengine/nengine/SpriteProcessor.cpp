
#include "stdafx.h"
#include "GlobalSingletonStorage.h"
#include "SpriteProcessor.h"
#include "TextureManager.h"
#include "NEngine.h"
#include "cal3d/memory_leak.h"

/////////////
#include <math.h>
/////////////////

CSpriteProcessor::CSpriteProcessor() :
	m_distortionMesh(nullptr)
{
	for (int i = 0; i != 361; ++i)
	{
		m_cosArray[i] = cosf(i*3.14159265f/180.0f);
	}

	for (int i = 0; i != 361; ++i)
	{
		m_sinArray[i] = sinf(i*3.14159265f/180.0f);
	}
}

void CSpriteProcessor::PutNormalSprite(int x, int y, int texX1, int texY1, int texX2, int texY2, std::string fileName, CColor3& color, int angle, bool tiling)
{
	int fl = g->tm.GetObjectNumber(fileName);
	
	PutNormalSprite(x, y, texX1, texY1, texX2, texY2, fl, color, angle, tiling);
}


void CSpriteProcessor::PutNormalSprite(int x, int y, int texX1, int texY1, int texX2, int texY2, int fl, CColor3& color, int angle, bool /*tiling*/)
{
	CTexture* texture = g->tm.GetObjectPointer(fl);

	g->tm.BindTexture(fl);

	int width = texture->GetTextureRealWidth();
	int height = texture->GetTextureRealHeight();

	float x1 = GetOpenGLX(x);
	float y1 = GetOpenGLY(y);
	float x2 = GetOpenGLX(x + (texX2 - texX1 + 1));
	float y2 = GetOpenGLY(y + (texY2 - texY1 + 1));

	float tx1 = float(texX1) / float(width);
	float ty1 = float(texY1) / float(height);
	float tx2 = float(texX2 + 1) / float(width);
	float ty2 = float(texY2 + 1) / float(height);

	if (texture->IsYInvert())
	{
		ty1 = 1.0f - ty1;
		ty2 = 1.0f - ty2;
	}

	g->stp.SetColor(color.r, color.g, color.b);

	g->stp.PrepareForRender();

	if (0 == angle)
	{
		GLFUNC(glBegin)(GL_QUADS);

		GLFUNC(glTexCoord2f)(tx1, ty1);
		GLFUNC(glVertex2f)(x1, y1);

		GLFUNC(glTexCoord2f)(tx1, ty2);
		GLFUNC(glVertex2f)(x1, y2);

		GLFUNC(glTexCoord2f)(tx2, ty2);
		GLFUNC(glVertex2f)(x2, y2);

		GLFUNC(glTexCoord2f)(tx2, ty1);
		GLFUNC(glVertex2f)(x2, y1);

		GLFUNC(glEnd)();
	}
	else
	{

		////////////////////////////////
		// центры
		float cx = x1 + (x2-x1)/2;
		float cy = y2 + (y1-y2)/2;

		float rotateX1 = (x1-cx)*m_cosArray[angle] + (y1-cy)*m_sinArray[angle] + cx;
		float rotateY1 = -(x1-cx)*m_sinArray[angle] + (y1-cy)*m_cosArray[angle] + cy;

		float rotateX2 = (x1-cx)*m_cosArray[angle] + (y2-cy)*m_sinArray[angle] + cx;
		float rotateY2 = -(x1-cx)*m_sinArray[angle] + (y2-cy)*m_cosArray[angle] + cy;

		float rotateX3 = (x2-cx)*m_cosArray[angle] + (y2-cy)*m_sinArray[angle] + cx;
		float rotateY3 = -(x2-cx)*m_sinArray[angle] + (y2-cy)*m_cosArray[angle] + cy;
		
		float rotateX4 = (x2-cx)*m_cosArray[angle] + (y1-cy)*m_sinArray[angle] + cx;
		float rotateY4 = -(x2-cx)*m_sinArray[angle] + (y1-cy)*m_cosArray[angle] + cy;

		/////////////////////////////////

		GLFUNC(glBegin)(GL_QUADS);
		GLFUNC(glTexCoord2f)(tx1, ty1);
		GLFUNC(glVertex2f)(rotateX1, rotateY1);
		GLFUNC(glTexCoord2f)(tx1, ty2);
		GLFUNC(glVertex2f)(rotateX2, rotateY2);
		GLFUNC(glTexCoord2f)(tx2, ty2);
		GLFUNC(glVertex2f)(rotateX3, rotateY3);
		GLFUNC(glTexCoord2f)(tx2, ty1);
		GLFUNC(glVertex2f)(rotateX4, rotateY4);
		GLFUNC(glEnd)();

	}

	g->tm.RestoreDefaultSettings();

}

void CSpriteProcessor::PutNormalSprite(int x, int y, int texX1, int texY1, int texX2, int texY2, int fl, CColor3& color, CPolygons2DAccum* accum, int angle, bool /*tiling*/)
{
	CTexture* texture = g->tm.GetObjectPointer(fl);

	int width = texture->GetTextureRealWidth();
	int height = texture->GetTextureRealHeight();

	float x1 = GetOpenGLX(x);
	float y1 = GetOpenGLY(y);
	float x2 = GetOpenGLX(x + (texX2 - texX1 + 1));
	float y2 = GetOpenGLY(y + (texY2 - texY1 + 1));

	float tx1 = float(texX1) / float(width);
	float ty1 = float(texY1) / float(height);
	float tx2 = float(texX2 + 1) / float(width);
	float ty2 = float(texY2 + 1) / float(height);

	if (texture->IsYInvert())
	{
		ty1 = 1.0f - ty1;
		ty2 = 1.0f - ty2;
	}

	accum->ChangeMaterialExt(texture->GetNumber(), fl, BLEND_MODE_NO_BLEND, color.r, color.g, color.b, 255);

	if (0 == angle)
	{
		accum->AddTextureCoords(tx1, ty1);
		accum->AddVertex(x1, y1);
		accum->AddTextureCoords(tx1, ty2);
		accum->AddVertex(x1, y2);
		accum->AddTextureCoords(tx2, ty2);
		accum->AddVertex(x2, y2);
		accum->AddTextureCoords(tx2, ty1);
		accum->AddVertex(x2, y1);
	}
	else
	{

		////////////////////////////////
		// центры
		float cx = x1 + (x2-x1)/2;
		float cy = y2 + (y1-y2)/2;

		float rotateX1 = (x1-cx)*m_cosArray[angle] + (y1-cy)*m_sinArray[angle] + cx;
		float rotateY1 = -(x1-cx)*m_sinArray[angle] + (y1-cy)*m_cosArray[angle] + cy;

		float rotateX2 = (x1-cx)*m_cosArray[angle] + (y2-cy)*m_sinArray[angle] + cx;
		float rotateY2 = -(x1-cx)*m_sinArray[angle] + (y2-cy)*m_cosArray[angle] + cy;

		float rotateX3 = (x2-cx)*m_cosArray[angle] + (y2-cy)*m_sinArray[angle] + cx;
		float rotateY3 = -(x2-cx)*m_sinArray[angle] + (y2-cy)*m_cosArray[angle] + cy;
		
		float rotateX4 = (x2-cx)*m_cosArray[angle] + (y1-cy)*m_sinArray[angle] + cx;
		float rotateY4 = -(x2-cx)*m_sinArray[angle] + (y1-cy)*m_cosArray[angle] + cy;

		/////////////////////////////////

		accum->AddTextureCoords(tx1, ty1);
		accum->AddVertex(rotateX1, rotateY1);
		accum->AddTextureCoords(tx1, ty2);
		accum->AddVertex(rotateX2, rotateY2);
		accum->AddTextureCoords(tx2, ty2);
		accum->AddVertex(rotateX3, rotateY3);
		accum->AddTextureCoords(tx2, ty1);
		accum->AddVertex(rotateX4, rotateY4);
	}
}

void CSpriteProcessor::PutAlphaSprite(int x, int y, int texX1, int texY1, int texX2, int texY2, int fl, unsigned char alpha, CColor3& color, CPolygons2DAccum* accum, int angle, bool /*tiling*/)
{
	CTexture* texture = g->tm.GetObjectPointer(fl);

	int width = texture->GetTextureRealWidth();
	int height = texture->GetTextureRealHeight();

	float x1 = GetOpenGLX(x);
	float y1 = GetOpenGLY(y);
	float x2 = GetOpenGLX(x + (texX2 - texX1 + 1));
	float y2 = GetOpenGLY(y + (texY2 - texY1 + 1));

	float tx1 = float(texX1) / float(width);
	float ty1 = float(texY1) / float(height);
	float tx2 = float(texX2 + 1) / float(width);
	float ty2 = float(texY2 + 1) / float(height);

	if (texture->IsYInvert())
	{
		ty1 = 1.0f - ty1;
		ty2 = 1.0f - ty2;
	}

	accum->ChangeMaterialExt(texture->GetNumber(), fl, BLEND_MODE_SPRITE, color.r, color.g, color.b, alpha);

	if (0 == angle)
	{
		accum->AddTextureCoords(tx1, ty1);
		accum->AddVertex(x1, y1);
		accum->AddTextureCoords(tx1, ty2);
		accum->AddVertex(x1, y2);
		accum->AddTextureCoords(tx2, ty2);
		accum->AddVertex(x2, y2);
		accum->AddTextureCoords(tx2, ty1);
		accum->AddVertex(x2, y1);
	}
	else
	{

		////////////////////////////////
		// центры
		float cx = x1 + (x2-x1)/2;
		float cy = y2 + (y1-y2)/2;

		float rotateX1 = (x1-cx)*m_cosArray[angle] + (y1-cy)*m_sinArray[angle] + cx;
		float rotateY1 = -(x1-cx)*m_sinArray[angle] + (y1-cy)*m_cosArray[angle] + cy;

		float rotateX2 = (x1-cx)*m_cosArray[angle] + (y2-cy)*m_sinArray[angle] + cx;
		float rotateY2 = -(x1-cx)*m_sinArray[angle] + (y2-cy)*m_cosArray[angle] + cy;

		float rotateX3 = (x2-cx)*m_cosArray[angle] + (y2-cy)*m_sinArray[angle] + cx;
		float rotateY3 = -(x2-cx)*m_sinArray[angle] + (y2-cy)*m_cosArray[angle] + cy;
		
		float rotateX4 = (x2-cx)*m_cosArray[angle] + (y1-cy)*m_sinArray[angle] + cx;
		float rotateY4 = -(x2-cx)*m_sinArray[angle] + (y1-cy)*m_cosArray[angle] + cy;

		/////////////////////////////////

		accum->AddTextureCoords(tx1, ty1);
		accum->AddVertex(rotateX1, rotateY1);
		accum->AddTextureCoords(tx1, ty2);
		accum->AddVertex(rotateX2, rotateY2);
		accum->AddTextureCoords(tx2, ty2);
		accum->AddVertex(rotateX3, rotateY3);
		accum->AddTextureCoords(tx2, ty1);
		accum->AddVertex(rotateX4, rotateY4);
	}
}

void CSpriteProcessor::PutNormalSprite(int x, int y, int x2, int y2, int texX1, int texY1, int texX2, int texY2, std::string fileName, CColor3& color, int /*angle*/, bool /*tiling*/)
{
	int fl = g->tm.GetObjectNumber(fileName);
	
	PutNormalSprite(x, y, x2, y2, texX1, texY1, texX2, texY2, fl, color);
}

void CSpriteProcessor::PutNormalSprite(int x, int y, int _x2, int _y2, int texX1, int texY1, int texX2, int texY2, int fl, CColor3& color, int angle, bool /*tiling*/)
{
	CTexture* texture = g->tm.GetObjectPointer(fl);

	g->tm.BindTexture(fl);

	int width = texture->GetTextureRealWidth();
	int height = texture->GetTextureRealHeight();

	float x1 = GetOpenGLX(x);
	float y1 = GetOpenGLY(y);
	float x2 = GetOpenGLX(_x2);
	float y2 = GetOpenGLY(_y2);

	float tx1 = float(texX1) / float(width);
	float ty1 = float(texY1) / float(height)/* + dy*/;
	float tx2 = float(texX2) / float(width);
	float ty2 = float(texY2) / float(height)/* + dy*/;

	if (texture->IsYInvert())
	{
		ty1 = 1.0f - ty1;
		ty2 = 1.0f - ty2;
	}

	g->stp.SetColor(color.r, color.g, color.b);

	g->stp.PrepareForRender();

	if (0 == angle)
	{
		GLFUNC(glBegin)(GL_QUADS);

		GLFUNC(glTexCoord2f)(tx1, ty1);
		GLFUNC(glVertex2f)(x1, y1);

		GLFUNC(glTexCoord2f)(tx1, ty2);
		GLFUNC(glVertex2f)(x1, y2);

		GLFUNC(glTexCoord2f)(tx2, ty2);
		GLFUNC(glVertex2f)(x2, y2);

		GLFUNC(glTexCoord2f)(tx2, ty1);
		GLFUNC(glVertex2f)(x2, y1);

		GLFUNC(glEnd)();
	}
	else
	{

		////////////////////////////////
		// центры
		float cx = x1 + (x2-x1)/2;
		float cy = y2 + (y1-y2)/2;

		float rotateX1 = (x1-cx)*m_cosArray[angle] + (y1-cy)*m_sinArray[angle] + cx;
		float rotateY1 = -(x1-cx)*m_sinArray[angle] + (y1-cy)*m_cosArray[angle] + cy;

		float rotateX2 = (x1-cx)*m_cosArray[angle] + (y2-cy)*m_sinArray[angle] + cx;
		float rotateY2 = -(x1-cx)*m_sinArray[angle] + (y2-cy)*m_cosArray[angle] + cy;

		float rotateX3 = (x2-cx)*m_cosArray[angle] + (y2-cy)*m_sinArray[angle] + cx;
		float rotateY3 = -(x2-cx)*m_sinArray[angle] + (y2-cy)*m_cosArray[angle] + cy;
		
		float rotateX4 = (x2-cx)*m_cosArray[angle] + (y1-cy)*m_sinArray[angle] + cx;
		float rotateY4 = -(x2-cx)*m_sinArray[angle] + (y1-cy)*m_cosArray[angle] + cy;

		/////////////////////////////////

		GLFUNC(glBegin)(GL_QUADS);
		GLFUNC(glTexCoord2f)(tx1, ty1);
		GLFUNC(glVertex2f)(rotateX1, rotateY1);
		GLFUNC(glTexCoord2f)(tx1, ty2);
		GLFUNC(glVertex2f)(rotateX2, rotateY2);
		GLFUNC(glTexCoord2f)(tx2, ty2);
		GLFUNC(glVertex2f)(rotateX3, rotateY3);
		GLFUNC(glTexCoord2f)(tx2, ty1);
		GLFUNC(glVertex2f)(rotateX4, rotateY4);
		GLFUNC(glEnd)();
	}
	
	g->tm.RestoreDefaultSettings();
}

void CSpriteProcessor::PutSprite(int x, int y, int _x2, int _y2, int texX1, int texY1, int texX2, int texY2, int textureID, CColor3& color, CPolygons2DAccum* accum, int angle, int tiling, bool is_transparent, unsigned char alpha)
{
	CTexture* texture = g->tm.GetObjectPointer(textureID);

	int width = texture->GetTextureRealWidth();
	int height = texture->GetTextureRealHeight();

	// transparency
	if (!is_transparent)
	{
		accum->ChangeMaterialExt(texture->GetNumber(), textureID, BLEND_MODE_NO_BLEND, color.r, color.g, color.b, 255);
	}
	else
	{
		accum->ChangeMaterialExt(texture->GetNumber(), textureID, BLEND_MODE_SPRITE, color.r, color.g, color.b, alpha);
	}

	if ((tiling == 0) || (g->gi.IsNPOTSupport() && (tiling != 2)) || ((IsPowerOf2(texture->GetTextureWidth())) && (tiling != 2) && (IsPowerOf2(texture->GetTextureHeight()))) )
	{
		float x1 = GetOpenGLX(x);
		float y1 = GetOpenGLY(y);
		float x2 = GetOpenGLX(_x2);
		float y2 = GetOpenGLY(_y2);

		float tx1 = float(texX1) / float(width);
		float ty1 = float(texY1) / float(height);
		float tx2 = float(texX2) / float(width);
		float ty2 = float(texY2) / float(height);

		if (texture->IsYInvert())
		{
			ty1 = 1.0f - ty1;
			ty2 = 1.0f - ty2;
		}
	
		if (texture->IsXInvert())
		{
			tx1 = 1.0f - tx1;
			tx2 = 1.0f - tx2;
		}

		if (0 == angle)
		{
			accum->AddTextureCoords(tx1, ty1);
			accum->AddVertex(x1, y1);
			accum->AddTextureCoords(tx1, ty2);
			accum->AddVertex(x1, y2);
			accum->AddTextureCoords(tx2, ty2);
			accum->AddVertex(x2, y2);
			accum->AddTextureCoords(tx2, ty1);
			accum->AddVertex(x2, y1);
		}
		else
		{

			////////////////////////////////
			// центры
			float cx = x1 + (x2-x1)/2;
			float cy = y2 + (y1-y2)/2;

			float rotateX1 = (x1-cx)*m_cosArray[angle] + (y1-cy)*m_sinArray[angle] + cx;
			float rotateY1 = -(x1-cx)*m_sinArray[angle] + (y1-cy)*m_cosArray[angle] + cy;

			float rotateX2 = (x1-cx)*m_cosArray[angle] + (y2-cy)*m_sinArray[angle] + cx;
			float rotateY2 = -(x1-cx)*m_sinArray[angle] + (y2-cy)*m_cosArray[angle] + cy;

			float rotateX3 = (x2-cx)*m_cosArray[angle] + (y2-cy)*m_sinArray[angle] + cx;
			float rotateY3 = -(x2-cx)*m_sinArray[angle] + (y2-cy)*m_cosArray[angle] + cy;
			
			float rotateX4 = (x2-cx)*m_cosArray[angle] + (y1-cy)*m_sinArray[angle] + cx;
			float rotateY4 = -(x2-cx)*m_sinArray[angle] + (y1-cy)*m_cosArray[angle] + cy;

			/////////////////////////////////

			accum->AddTextureCoords(tx1, ty1);
			accum->AddVertex(rotateX1, rotateY1);
			accum->AddTextureCoords(tx1, ty2);
			accum->AddVertex(rotateX2, rotateY2);
			accum->AddTextureCoords(tx2, ty2);
			accum->AddVertex(rotateX3, rotateY3);
			accum->AddTextureCoords(tx2, ty1);
			accum->AddVertex(rotateX4, rotateY4);

		}
	}
	else if (tiling == 1)
	{
	// image tiling
		int texWidth = texture->GetTextureWidth();
		int texHeight = texture->GetTextureHeight();		

		int blockCount = 0;

		for (int currentX = x; _x2 > currentX; currentX += texWidth)
		{
			for (int currentY = y; _y2 > currentY; currentY += texHeight)
			{
				blockCount++;

				if (blockCount > 2000)
				{
					// разбиение на тайлы дает слишком много кусков
					assert(false);
				}

				// picture coords
				float x1 = GetOpenGLX(currentX);
				float y1 = GetOpenGLY(currentY);

				// x2
				float x2;
				if (_x2 - currentX > texWidth)
				{
					x2 = GetOpenGLX(currentX+texWidth);
				}
				else
				{
					x2 = GetOpenGLX(_x2);
				}
				
				// y2
				float y2;
				if (_y2 - currentY > texHeight)
				{
					y2 = GetOpenGLY(currentY+texHeight);
				}
				else
				{
					y2 = GetOpenGLY(_y2);
				}


				// texture coords
				float tx1 = float(0) / float(width);
				float ty1 = float(0) / float(height);

				// tx2
				float tx2;
				if (_x2 - currentX > texWidth)
				{
					tx2 = float(texWidth) / float(width);
				}
				else
				{
					tx2 = float(_x2-currentX) / float(width);
				}

				// ty2
				float ty2;
				if (_y2 - currentY > texHeight)
				{
					ty2 = float(texHeight) / float(height);
				}
				else
				{
					ty2 = float(_y2-currentY) / float(height);
				}


				if (texture->IsYInvert())
				{
					ty1 = 1.0f - ty1;
					ty2 = 1.0f - ty2;
				}
			
				if (texture->IsXInvert())
				{
					tx1 = 1.0f - tx1;
					tx2 = 1.0f - tx2;
				}

				

				if (0 == angle)
				{
					accum->AddTextureCoords(tx1, ty1);
					accum->AddVertex(x1, y1);
					accum->AddTextureCoords(tx1, ty2);
					accum->AddVertex(x1, y2);
					accum->AddTextureCoords(tx2, ty2);
					accum->AddVertex(x2, y2);
					accum->AddTextureCoords(tx2, ty1);
					accum->AddVertex(x2, y1);
				}
				else
				{
					////////////////////////////////
					// центры
					float cx = GetOpenGLX(x) + (GetOpenGLX(_x2) - GetOpenGLX(x))/2;
					float cy = GetOpenGLY(_y2) + (GetOpenGLY(y)-GetOpenGLY(_y2))/2;

					float rotateX1 = (x1-cx)*m_cosArray[angle] + (y1-cy)*m_sinArray[angle] + cx;
					float rotateY1 = -(x1-cx)*m_sinArray[angle] + (y1-cy)*m_cosArray[angle] + cy;

					float rotateX2 = (x1-cx)*m_cosArray[angle] + (y2-cy)*m_sinArray[angle] + cx;
					float rotateY2 = -(x1-cx)*m_sinArray[angle] + (y2-cy)*m_cosArray[angle] + cy;

					float rotateX3 = (x2-cx)*m_cosArray[angle] + (y2-cy)*m_sinArray[angle] + cx;
					float rotateY3 = -(x2-cx)*m_sinArray[angle] + (y2-cy)*m_cosArray[angle] + cy;
					
					float rotateX4 = (x2-cx)*m_cosArray[angle] + (y1-cy)*m_sinArray[angle] + cx;
					float rotateY4 = -(x2-cx)*m_sinArray[angle] + (y1-cy)*m_cosArray[angle] + cy;
					/////////////////////////////////
					accum->AddTextureCoords(tx1, ty1);
					accum->AddVertex(rotateX1, rotateY1);
					accum->AddTextureCoords(tx1, ty2);
					accum->AddVertex(rotateX2, rotateY2);
					accum->AddTextureCoords(tx2, ty2);
					accum->AddVertex(rotateX3, rotateY3);
					accum->AddTextureCoords(tx2, ty1);
					accum->AddVertex(rotateX4, rotateY4);
				}
			}
		}
	}
	else
	{
		// imageframe tiling
		int texWidth = texX2-texX1;
		int texHeight = texY2-texY1;		

		int blockCount = 0;
		
		for (int currentX = x; _x2 > currentX; currentX += texWidth)
		{
			for (int currentY = y; _y2 > currentY; currentY += texHeight)
			{
				blockCount++;

				if (blockCount > 2000)
				{
					// разбиение на тайлы дает слишком много кусков
					assert(false);
				}

				// picture coords
				float x1 = GetOpenGLX(currentX);
				float y1 = GetOpenGLY(currentY);

				// x2
				float x2;
				if (_x2 - currentX > texWidth)
				{
					x2 = GetOpenGLX(currentX+texWidth);
				}
				else
				{
					x2 = GetOpenGLX(_x2);
				}
				
				// y2
				float y2;
				if (_y2 - currentY > texHeight)
				{
					y2 = GetOpenGLY(currentY+texHeight);
				}
				else
				{
					y2 = GetOpenGLY(_y2);
				}


				// texture coords
				float tx1 = float(texX1) / float(width);
				float ty1 = float(texY1) / float(height);

				// tx2
				float tx2;
				if (_x2 - currentX > texWidth)
				{
					tx2 = float(texX2) / float(width);
				}
				else
				{
					tx2 = float(_x2-currentX+texX1) / float(width);
				}

				// ty2
				float ty2;
				if (_y2 - currentY > texHeight)
				{
					ty2 = float(texY2) / float(height);
				}
				else
				{
					ty2 = float(_y2-currentY+texY1) / float(height);
				}


				if (texture->IsYInvert())
				{
					ty1 = 1.0f - ty1;
					ty2 = 1.0f - ty2;
				}
			
				if (texture->IsXInvert())
				{
					tx1 = 1.0f - tx1;
					tx2 = 1.0f - tx2;
				}

				

				if (0 == angle)
				{
					accum->AddTextureCoords(tx1, ty1);
					accum->AddVertex(x1, y1);
					accum->AddTextureCoords(tx1, ty2);
					accum->AddVertex(x1, y2);
					accum->AddTextureCoords(tx2, ty2);
					accum->AddVertex(x2, y2);
					accum->AddTextureCoords(tx2, ty1);
					accum->AddVertex(x2, y1);
				}
				else
				{
					////////////////////////////////
					// центры
					float cx = GetOpenGLX(x) + (GetOpenGLX(_x2) - GetOpenGLX(x))/2;
					float cy = GetOpenGLY(_y2) + (GetOpenGLY(y)-GetOpenGLY(_y2))/2;

					float rotateX1 = (x1-cx)*m_cosArray[angle] + (y1-cy)*m_sinArray[angle] + cx;
					float rotateY1 = -(x1-cx)*m_sinArray[angle] + (y1-cy)*m_cosArray[angle] + cy;

					float rotateX2 = (x1-cx)*m_cosArray[angle] + (y2-cy)*m_sinArray[angle] + cx;
					float rotateY2 = -(x1-cx)*m_sinArray[angle] + (y2-cy)*m_cosArray[angle] + cy;

					float rotateX3 = (x2-cx)*m_cosArray[angle] + (y2-cy)*m_sinArray[angle] + cx;
					float rotateY3 = -(x2-cx)*m_sinArray[angle] + (y2-cy)*m_cosArray[angle] + cy;
					
					float rotateX4 = (x2-cx)*m_cosArray[angle] + (y1-cy)*m_sinArray[angle] + cx;
					float rotateY4 = -(x2-cx)*m_sinArray[angle] + (y1-cy)*m_cosArray[angle] + cy;
					/////////////////////////////////
					accum->AddTextureCoords(tx1, ty1);
					accum->AddVertex(rotateX1, rotateY1);
					accum->AddTextureCoords(tx1, ty2);
					accum->AddVertex(rotateX2, rotateY2);
					accum->AddTextureCoords(tx2, ty2);
					accum->AddVertex(rotateX3, rotateY3);
					accum->AddTextureCoords(tx2, ty1);
					accum->AddVertex(rotateX4, rotateY4);
				}
			}
		}
	}
}

void CSpriteProcessor::PutAlphaSprite(int x, int y, int _x2, int _y2, int texX1, int texY1, int texX2, int texY2, int textureID, unsigned char alpha, CColor3& color, CPolygons2DAccum* accum, int angle, bool tiling)
{
	CTexture* texture = g->tm.GetObjectPointer(textureID);

	int width = texture->GetTextureRealWidth();
	int height = texture->GetTextureRealHeight();

	if (!tiling)
	{
		float x1 = GetOpenGLX(x);
		float y1 = GetOpenGLY(y);
		float x2 = GetOpenGLX(_x2);
		float y2 = GetOpenGLY(_y2);

		float tx1 = float(texX1) / float(width);
		float ty1 = float(texY1/* + 1*/) / float(height);
		float tx2 = float(texX2) / float(width);
		float ty2 = float(texY2/* + 1*/) / float(height);

		if (texture->IsYInvert())
		{
			ty1 = 1.0f - ty1;
			ty2 = 1.0f - ty2;
		}

		if (texture->IsXInvert())
		{
			tx1 = 1.0f - tx1;
			tx2 = 1.0f - tx2;
		}

		accum->ChangeMaterialExt(texture->GetNumber(), textureID, BLEND_MODE_SPRITE, color.r, color.g, color.b, alpha);

		if (0 == angle)
		{
			accum->AddTextureCoords(tx1, ty1);
			accum->AddVertex(x1, y1);
			accum->AddTextureCoords(tx1, ty2);
			accum->AddVertex(x1, y2);
			accum->AddTextureCoords(tx2, ty2);
			accum->AddVertex(x2, y2);
			accum->AddTextureCoords(tx2, ty1);
			accum->AddVertex(x2, y1);
		}
		else
		{

			////////////////////////////////
			// центры
			float cx = x1 + (x2-x1)/2;
			float cy = y2 + (y1-y2)/2;

			float rotateX1 = (x1-cx)*m_cosArray[angle] + (y1-cy)*m_sinArray[angle] + cx;
			float rotateY1 = -(x1-cx)*m_sinArray[angle] + (y1-cy)*m_cosArray[angle] + cy;

			float rotateX2 = (x1-cx)*m_cosArray[angle] + (y2-cy)*m_sinArray[angle] + cx;
			float rotateY2 = -(x1-cx)*m_sinArray[angle] + (y2-cy)*m_cosArray[angle] + cy;

			float rotateX3 = (x2-cx)*m_cosArray[angle] + (y2-cy)*m_sinArray[angle] + cx;
			float rotateY3 = -(x2-cx)*m_sinArray[angle] + (y2-cy)*m_cosArray[angle] + cy;
			
			float rotateX4 = (x2-cx)*m_cosArray[angle] + (y1-cy)*m_sinArray[angle] + cx;
			float rotateY4 = -(x2-cx)*m_sinArray[angle] + (y1-cy)*m_cosArray[angle] + cy;

			/////////////////////////////////

			accum->AddTextureCoords(tx1, ty1);
			accum->AddVertex(rotateX1, rotateY1);
			accum->AddTextureCoords(tx1, ty2);
			accum->AddVertex(rotateX2, rotateY2);
			accum->AddTextureCoords(tx2, ty2);
			accum->AddVertex(rotateX3, rotateY3);
			accum->AddTextureCoords(tx2, ty1);
			accum->AddVertex(rotateX4, rotateY4);

		}
	}
	else
	{
		for (int currentX = x; _x2 > currentX; currentX += texture->GetTextureWidth())
		{
			for (int currentY = y; _y2 > currentY; currentY += texture->GetTextureHeight())
			{

				// picture coords
				float x1 = GetOpenGLX(currentX);
				float y1 = GetOpenGLY(currentY);

				// x2
				float x2;
				if (_x2 - currentX > texture->GetTextureWidth())
				{
					x2 = GetOpenGLX(currentX+texture->GetTextureWidth());
				}
				else
				{
					x2 = GetOpenGLX(_x2);
				}
				
				// y2
				float y2;
				if (_y2 - currentY > texture->GetTextureHeight())
				{
					y2 = GetOpenGLY(currentY+texture->GetTextureHeight());
				}
				else
				{
					y2 = GetOpenGLY(_y2);
				}


				// texture coords
				float tx1 = float(0) / float(width);
				float ty1 = float(0) / float(height);

				// tx2
				float tx2;
				if (_x2 - currentX > texture->GetTextureWidth())
				{
					tx2 = float(texture->GetTextureWidth()) / float(width);
				}
				else
				{
					tx2 = float(_x2-currentX) / float(width);
				}

				// ty2
				float ty2;
				if (_y2 - currentY > texture->GetTextureWidth())
				{
					ty2 = float(texture->GetTextureWidth()) / float(height);
				}
				else
				{
					ty2 = float(_y2-currentY) / float(height);
				}


				if (texture->IsYInvert())
				{
					ty1 = 1.0f - ty1;
					ty2 = 1.0f - ty2;
				}

				accum->ChangeMaterialExt(texture->GetNumber(), textureID, BLEND_MODE_SPRITE, color.r, color.g, color.b, alpha);

				if (0 == angle)
				{
					accum->AddTextureCoords(tx1, ty1);
					accum->AddVertex(x1, y1);
					accum->AddTextureCoords(tx1, ty2);
					accum->AddVertex(x1, y2);
					accum->AddTextureCoords(tx2, ty2);
					accum->AddVertex(x2, y2);
					accum->AddTextureCoords(tx2, ty1);
					accum->AddVertex(x2, y1);
				}
				else
				{
					////////////////////////////////
					// центры
					float cx = GetOpenGLX(x) + (GetOpenGLX(_x2) - GetOpenGLX(x))/2;
					float cy = GetOpenGLY(_y2) + (GetOpenGLY(y)-GetOpenGLY(_y2))/2;

					float rotateX1 = (x1-cx)*m_cosArray[angle] + (y1-cy)*m_sinArray[angle] + cx;
					float rotateY1 = -(x1-cx)*m_sinArray[angle] + (y1-cy)*m_cosArray[angle] + cy;

					float rotateX2 = (x1-cx)*m_cosArray[angle] + (y2-cy)*m_sinArray[angle] + cx;
					float rotateY2 = -(x1-cx)*m_sinArray[angle] + (y2-cy)*m_cosArray[angle] + cy;

					float rotateX3 = (x2-cx)*m_cosArray[angle] + (y2-cy)*m_sinArray[angle] + cx;
					float rotateY3 = -(x2-cx)*m_sinArray[angle] + (y2-cy)*m_cosArray[angle] + cy;
					
					float rotateX4 = (x2-cx)*m_cosArray[angle] + (y1-cy)*m_sinArray[angle] + cx;
					float rotateY4 = -(x2-cx)*m_sinArray[angle] + (y1-cy)*m_cosArray[angle] + cy;
					/////////////////////////////////
					accum->AddTextureCoords(tx1, ty1);
					accum->AddVertex(rotateX1, rotateY1);
					accum->AddTextureCoords(tx1, ty2);
					accum->AddVertex(rotateX2, rotateY2);
					accum->AddTextureCoords(tx2, ty2);
					accum->AddVertex(rotateX3, rotateY3);
					accum->AddTextureCoords(tx2, ty1);
					accum->AddVertex(rotateX4, rotateY4);
				}
			}
		}
	}
}


void CSpriteProcessor::PutAlphaSprite(int x, int y, int _x2, int _y2, int texX1, int texY1, int texX2, int texY2, int fl, BYTE alpha, CColor3& color, int angle, bool /*tiling*/)
{
	CTexture* texture = g->tm.GetObjectPointer(fl);
	
	g->tm.BindTexture(fl);

	int width = texture->GetTextureRealWidth();
	int height = texture->GetTextureRealHeight();

	float x1 = GetOpenGLX(x);
	float y1 = GetOpenGLY(y);
	float x2 = GetOpenGLX(_x2);
	float y2 = GetOpenGLY(_y2);

	float tx1 = float(texX1) / float(width);
	float ty1 = float(texY1/* + 1*/) / float(height);
	float tx2 = float(texX2) / float(width);
	float ty2 = float(texY2/* + 1*/) / float(height);

	if (texture->IsYInvert())
	{
		ty1 = 1.0f - ty1;
		ty2 = 1.0f - ty2;
	}

	if (texture->IsXInvert())
	{
		tx1 = 1.0f - tx1;
		tx2 = 1.0f - tx2;
	}

	g->stp.SetColor(color.r, color.g, color.b, alpha);

	g->stp.SetState(ST_BLEND, true);
	g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	g->stp.PrepareForRender();


	if (0 == angle)
	{
		GLFUNC(glBegin)(GL_QUADS);

		GLFUNC(glTexCoord2f)(tx1, ty1);
		GLFUNC(glVertex2f)(x1, y1);

		GLFUNC(glTexCoord2f)(tx1, ty2);
		GLFUNC(glVertex2f)(x1, y2);

		GLFUNC(glTexCoord2f)(tx2, ty2);
		GLFUNC(glVertex2f)(x2, y2);

		GLFUNC(glTexCoord2f)(tx2, ty1);
		GLFUNC(glVertex2f)(x2, y1);

		GLFUNC(glEnd)();
	}
	else
	{

		////////////////////////////////
		// центры
		float cx = x1 + (x2-x1)/2;
		float cy = y2 + (y1-y2)/2;

		float rotateX1 = (x1-cx)*m_cosArray[angle] + (y1-cy)*m_sinArray[angle] + cx;
		float rotateY1 = -(x1-cx)*m_sinArray[angle] + (y1-cy)*m_cosArray[angle] + cy;

		float rotateX2 = (x1-cx)*m_cosArray[angle] + (y2-cy)*m_sinArray[angle] + cx;
		float rotateY2 = -(x1-cx)*m_sinArray[angle] + (y2-cy)*m_cosArray[angle] + cy;

		float rotateX3 = (x2-cx)*m_cosArray[angle] + (y2-cy)*m_sinArray[angle] + cx;
		float rotateY3 = -(x2-cx)*m_sinArray[angle] + (y2-cy)*m_cosArray[angle] + cy;
		
		float rotateX4 = (x2-cx)*m_cosArray[angle] + (y1-cy)*m_sinArray[angle] + cx;
		float rotateY4 = -(x2-cx)*m_sinArray[angle] + (y1-cy)*m_cosArray[angle] + cy;

		/////////////////////////////////

		GLFUNC(glBegin)(GL_QUADS);
		GLFUNC(glTexCoord2f)(tx1, ty1);
		GLFUNC(glVertex2f)(rotateX1, rotateY1);
		GLFUNC(glTexCoord2f)(tx1, ty2);
		GLFUNC(glVertex2f)(rotateX2, rotateY2);
		GLFUNC(glTexCoord2f)(tx2, ty2);
		GLFUNC(glVertex2f)(rotateX3, rotateY3);
		GLFUNC(glTexCoord2f)(tx2, ty1);
		GLFUNC(glVertex2f)(rotateX4, rotateY4);
		GLFUNC(glEnd)();

	}
	

	g->stp.SetState(ST_BLEND, false);
}

void CSpriteProcessor::PutAddSprite(int x, int y, int _x2, int _y2, int texX1, int texY1, int texX2, int texY2, int fl, BYTE alpha, CColor3& color)
{
	CTexture* texture = g->tm.GetObjectPointer(fl);
	
	g->tm.BindTexture(fl);

	int width = texture->GetTextureRealWidth();
	int height = texture->GetTextureRealHeight();

	float x1 = GetOpenGLX(x);
	float y1 = GetOpenGLY(y);
	float x2 = GetOpenGLX(_x2);
	float y2 = GetOpenGLY(_y2);

	float tx1 = float(texX1) / float(width);
	float ty1 = float(texY1/* + 1*/) / float(height);
	float tx2 = float(texX2) / float(width);
	float ty2 = float(texY2/* + 1*/) / float(height);

	if (texture->IsYInvert())
	{
		ty1 = 1.0f - ty1;
		ty2 = 1.0f - ty2;
	}

	if (texture->IsXInvert())
	{
		tx1 = 1.0f - tx1;
		tx2 = 1.0f - tx2;
	}

	g->stp.SetColor((unsigned char)(color.r * alpha / 255), (unsigned char)(color.g * alpha / 255), (unsigned char)(color.b * alpha / 255), alpha);

	g->stp.SetState(ST_BLEND, true);
	g->stp.SetBlendFunc(GL_ONE, GL_ONE);
	g->stp.PrepareForRender();

	GLFUNC(glBegin)(GL_QUADS);
	GLFUNC(glTexCoord2f)(tx1, ty1);
	GLFUNC(glVertex2f)(x1, y1);
	GLFUNC(glTexCoord2f)(tx1, ty2);
	GLFUNC(glVertex2f)(x1, y2);
	GLFUNC(glTexCoord2f)(tx2, ty2);
	GLFUNC(glVertex2f)(x2, y2);
	GLFUNC(glTexCoord2f)(tx2, ty1);
	GLFUNC(glVertex2f)(x2, y1);
	GLFUNC(glEnd)();

	g->stp.SetState(ST_BLEND, false);
}

void CSpriteProcessor::PutAlphaSprite(int x, int y, int x2, int y2, int texX1, int texY1, int texX2, int texY2, std::string fileName, BYTE sprAlpha, CColor3& color, int /*angle*/, bool /*tiling*/)
{
	int fl = g->tm.GetObjectNumber(fileName);

	PutAlphaSprite(x, y, x2, y2, texX1, texY1, texX2, texY2, fl, sprAlpha, color);
}

void CSpriteProcessor::PutAlphaSprite(int x, int y, int texX1, int texY1, int texX2, int texY2, int fl, BYTE spr_alpha, CColor3& color, int /*angle*/, bool /*tiling*/)
{
	CTexture* texture = g->tm.GetObjectPointer(fl);
	g->tm.BindTexture(fl);

	int width = texture->GetTextureRealWidth();
	int height = texture->GetTextureRealHeight();

	float x1 = GetOpenGLX(x);
	float y1 = GetOpenGLY(y);
	float x2 = GetOpenGLX(x + (texX2 - texX1/* + 1*/));
	float y2 = GetOpenGLY(y + (texY2 - texY1/* + 1*/));

	float tx1 = float(texX1) / float(width);
	float ty1 = float(texY1/* + 1*/) / float(height);
	float tx2 = float(texX2) / float(width);
	float ty2 = float(texY2/* + 1*/) / float(height);

	if (texture->IsYInvert())
	{
		ty1 = 1.0f - ty1;
		ty2 = 1.0f - ty2;
	}

	if (texture->IsXInvert())
	{
		tx1 = 1.0f - tx1;
		tx2 = 1.0f - tx2;
	}

	g->stp.SetState(ST_BLEND, true);
	g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	g->stp.SetColor(color.r, color.g, color.b, spr_alpha);

	g->stp.PrepareForRender();

	GLFUNC(glBegin)(GL_QUADS);
	GLFUNC(glTexCoord2f)(tx1, ty1);
	GLFUNC(glVertex2f)(x1, y1);
	GLFUNC(glTexCoord2f)(tx1, ty2);
	GLFUNC(glVertex2f)(x1, y2);
	GLFUNC(glTexCoord2f)(tx2, ty2);
	GLFUNC(glVertex2f)(x2, y2);
	GLFUNC(glTexCoord2f)(tx2, ty1);
	GLFUNC(glVertex2f)(x2, y1);
	GLFUNC(glEnd)();

	g->stp.SetState(ST_BLEND, false);
}

void CSpriteProcessor::PutAlphaSprite(int x, int y, int texX1, int texY1, int texX2, int texY2, std::string fileName, BYTE sprAlpha, CColor3& color, int angle, bool /*tiling*/)
{
	int fl = g->tm.GetObjectNumber(fileName);

	PutAlphaSprite(x, y, texX1, texY1, texX2, texY2, fl, sprAlpha, color, angle);
}

void CSpriteProcessor::PutSplittedAlphaSprite(int x, int y, int texX1, int texY1, int texX2, int texY2, std::string fileName, BYTE sprAlpha, int blockSize)
{
	int fl = g->tm.GetObjectNumber(fileName);

	PutSplittedAlphaSprite(x, y, texX1, texY1, texX2, texY2, fl, sprAlpha, blockSize);
}

void CSpriteProcessor::PutSplittedAlphaSprite(int x, int y, int texX1, int texY1, int texX2, int texY2, int fl, BYTE sprAlpha, int blockSize)
{
	CTexture* texture = g->tm.GetObjectPointer(fl);
	/*if ((!texture->IsEnabled()) || (!texture->IsLoaded()))
	{
		return;
	}*/

	g->tm.BindTexture(fl);

	int width = texture->GetTextureRealWidth();
	int height = texture->GetTextureRealHeight();

	float dx = (float)blockSize / (float)width;
	float dy = (float)blockSize / (float)height;
	int xBlockCount = (int)(width / blockSize);
	int yBlockCount = (int)(height / blockSize);

	float x1 = GetOpenGLX(x);
	float y1 = GetOpenGLY(y);
	float x2 = GetOpenGLX(x + (texX2 - texX1/* + 1*/));
	float y2 = GetOpenGLY(y + (texY2 - texY1/* + 1*/));

	float tx1 = float(texX1) / float(width);
	float ty1 = float(texY1/* + 1*/) / float(height);
	float tx2 = float(texX2) / float(width);
	float ty2 = float(texY2/* + 1*/) / float(height);

	float tKoef = 1.0f;

	if (texture->IsYInvert())
	{
		ty1 = 1.0f - ty1;
		ty2 = 1.0f - ty2;
		tKoef *= -1.0f;
	}

	g->stp.SetState(ST_BLEND, true);
	g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	g->stp.SetColor(255, 255, 255, sprAlpha);

	g->stp.PrepareForRender();

	GLFUNC(glBegin)(GL_QUADS);

	for (int _x = 0; _x < xBlockCount; _x++)
	for (int _y = 0; _y < yBlockCount; _y++)
	{
		float _x1 = x1 + dx * _x * 2.0f;
		float _x2 = _x1 + dx * 2.0f;
		float _y1 = y1 - dy * _y * 2.0f;
		float _y2 = _y1 - dy * 2.0f;

		if (_x2 > x2)
		{
			_x2 = x2;
		}

		if (_y2 < y2)
		{
			_y2 = y2;
		}

		float _tx1 = tx1 + dx * _x;
		float _tx2 = _tx1 + dx;
		float _ty1 = (ty1 + dy * _y * tKoef);
		float _ty2 = (_ty1 + dy * tKoef);

		if (_tx2 > tx2)
		{
			_tx2 = tx2;
		}

		if (_ty2 < ty2)
		{
			_ty2 = ty2;
		}

		GLFUNC(glTexCoord2f)(_tx1, _ty1);
		GLFUNC(glVertex2f)(_x1, _y1);
		GLFUNC(glTexCoord2f)(_tx1, _ty2);
		GLFUNC(glVertex2f)(_x1, _y2);
		GLFUNC(glTexCoord2f)(_tx2, _ty2);
		GLFUNC(glVertex2f)(_x2, _y2);
		GLFUNC(glTexCoord2f)(_tx2, _ty1);
		GLFUNC(glVertex2f)(_x2, _y1);	
	}

	GLFUNC(glEnd)();
	
	g->stp.SetState(ST_BLEND, false);
}

CBaseBufferObject* CSpriteProcessor::MakeSplittedSpriteInVBO(int width, int height, int blockSize)
{
	int x = 0;
	int y = 0;

	int texX1 = 0;
	int texX2 = width;

	int texY1 = 0;
	int texY2 = height;
	
	float dx = (float)blockSize / (float)width;
	float dy = (float)blockSize / (float)height;
	int xBlockCount = (int)(width / blockSize);
	int yBlockCount = (int)(height / blockSize);

	float x1 = GetOpenGLX(x);
	float y1 = GetOpenGLY(y);
	float x2 = GetOpenGLX(x + (texX2 - texX1/* + 1*/));
	float y2 = GetOpenGLY(y + (texY2 - texY1/* + 1*/));

	float tx1 = float(texX1) / float(width);
	float ty1 = float(texY1/* + 1*/) / float(height);
	float tx2 = float(texX2) / float(width);
	float ty2 = float(texY2/* + 1*/) / float(height);

	float tKoef = 1.0f;

	/*if (texture->IsYInvert())
	{*/
		ty1 = 1.0f - ty1;
		ty2 = 1.0f - ty2;
		tKoef *= -1.0f;
	/*}*/

	float* vertices = MP_NEW_ARR(float, xBlockCount * yBlockCount * 8);
	float* textureCoords = MP_NEW_ARR(float, xBlockCount * yBlockCount * 8);
	
	int cnt = 0;;

	for (int _x = 0; _x < xBlockCount; _x++)
	for (int _y = 0; _y < yBlockCount; _y++)
	{
		float _x1 = x1 + dx * _x * 2.0f;
		float _x2 = _x1 + dx * 2.0f;
		float _y1 = y1 - dy * _y * 2.0f;
		float _y2 = _y1 - dy * 2.0f;

		if (_x2 > x2)
		{
			_x2 = x2;
		}

		if (_y2 < y2)
		{
			_y2 = y2;
		}

		float _tx1 = tx1 + dx * _x;
		float _tx2 = _tx1 + dx;
		float _ty1 = (ty1 + dy * _y * tKoef);
		float _ty2 = (_ty1 + dy * tKoef);

		if (_tx2 > tx2)
		{
			_tx2 = tx2;
		}

		if (_ty2 < ty2)
		{
			_ty2 = ty2;
		}

		vertices[cnt * 2] = _x1;
		vertices[cnt * 2 + 1] = _y1;
		textureCoords[cnt * 2] = _tx1;
		textureCoords[cnt * 2 + 1] = _ty1;
		cnt++;

		vertices[cnt * 2] = _x1;
		vertices[cnt * 2 + 1] = _y2;
		textureCoords[cnt * 2] = _tx1;
		textureCoords[cnt * 2 + 1] = _ty2;
		cnt++;

		vertices[cnt * 2] = _x2;
		vertices[cnt * 2 + 1] = _y2;
		textureCoords[cnt * 2] = _tx2;
		textureCoords[cnt * 2 + 1] = _ty2;
		cnt++;

		vertices[cnt * 2] = _x2;
		vertices[cnt * 2 + 1] = _y1;
		textureCoords[cnt * 2] = _tx2;
		textureCoords[cnt * 2 + 1] = _ty1;
		cnt++;		
	}

	CBaseBufferObject* vbo = MP_NEW(CBaseBufferObject);
	vbo->SetPrimitiveType(GL_QUADS);
	vbo->SetVertexDimension(2);
	vbo->SetVertexArray(vertices, cnt);
	vbo->SetTextureCoordsArray(0, textureCoords);
	//vbo->SetFaceArray(indices, (int)(cnt / 4));

	MP_DELETE_ARR(vertices);
	MP_DELETE_ARR(textureCoords);

	return vbo;

	//g->stp.SetState(ST_BLEND, false);
}

float CSpriteProcessor::GetOpenGLX(int x)const
{
	return (2.0f * float(x) / (float)g->stp.GetCurrentWidth() - 1.0f);
}

float CSpriteProcessor::GetOpenGLY(int y)const
{
	return (-2.0f * float(y) / (float)g->stp.GetCurrentHeight() + 1.0f);
}

void CSpriteProcessor::PutOneColorQuad(float x1, float y1, float x2, float y2, BYTE r, BYTE _g, BYTE b, BYTE a)
{
	g->stp.SetColor(r, _g, b, a);
	
	GLFUNC(glDisable)(GL_TEXTURE_2D);
		
	g->stp.PrepareForRender();

	GLFUNC(glBegin)(GL_QUADS);
	GLFUNC(glVertex2f)(x1, y1);
	GLFUNC(glVertex2f)(x2, y1);
	GLFUNC(glVertex2f)(x2, y2);
	GLFUNC(glVertex2f)(x1, y2);
	GLFUNC(glEnd)();

	GLFUNC(glEnable)(GL_TEXTURE_2D);
}

void CSpriteProcessor::PutOneColorQuad(int x, int y, int xx, int yy, BYTE r, BYTE _g, BYTE b, BYTE a, CPolygons2DAccum* accum)
{
	float x1 = GetOpenGLX(x);
	float y1 = GetOpenGLY(y);
	float x2 = GetOpenGLX(xx);
	float y2 = GetOpenGLY(yy);

	if (accum)
	{
		accum->ChangeMaterialExt(-1, -1, BLEND_MODE_SPRITE, r, _g, b, a);
		accum->AddTextureCoords(0, 0);
		accum->AddVertex(x1, y1);
		accum->AddTextureCoords(0, 0);
		accum->AddVertex(x2, y1);
		accum->AddTextureCoords(0, 0);
		accum->AddVertex(x2, y2);
		accum->AddTextureCoords(0, 0);
		accum->AddVertex(x1, y2);

		return;
	}

	g->stp.SetColor(r, _g, b, a);
	
	GLFUNC(glDisable)(GL_TEXTURE_2D);
		
	g->stp.PrepareForRender();

	GLFUNC(glBegin)(GL_QUADS);
	GLFUNC(glVertex2f)(x1, y1);
	GLFUNC(glVertex2f)(x2, y1);
	GLFUNC(glVertex2f)(x2, y2);
	GLFUNC(glVertex2f)(x1, y2);
	GLFUNC(glEnd)();

	GLFUNC(glEnable)(GL_TEXTURE_2D);
}

/*
void CSpriteProcessor::CreateOculusDistortionMesh(IOculusRiftSupport::DistortionMesh* mesh)
{
	if (!m_distortionMesh)
	{
		glDeleteLists(m_distortionMesh, 1);
	}
	m_distortionMesh = glGenLists(1);

	GLFUNC(glNewList)(m_distortionMesh, GL_COMPILE);
	
	for (int eye = 0; eye < 2; ++eye)
	{
		GLFUNC(glBegin)(GL_TRIANGLES);
		const IOculusRiftSupport::DistortionMesh& cMesh = mesh[eye];
		for (int i = 0; i < cMesh.IndexCount; ++i)
		{
			const IOculusRiftSupport::DistortionVertex& vertex =
				cMesh.pVertexData[cMesh.pIndexData[i]];			
			GLFUNC(glMultiTexCoord2f)(GL_TEXTURE0, vertex.TanEyeAnglesR.x, -vertex.TanEyeAnglesR.y);
			GLFUNC(glMultiTexCoord2f)(GL_TEXTURE1, vertex.TanEyeAnglesG.x, -vertex.TanEyeAnglesG.y);
			GLFUNC(glMultiTexCoord2f)(GL_TEXTURE2, vertex.TanEyeAnglesB.x, -vertex.TanEyeAnglesB.y);
			GLFUNC(glColor3f)(vertex.VignetteFactor, vertex.TimeWarpFactor, eye);
			GLFUNC(glVertex2f)(vertex.ScreenPosNDC.x, vertex.ScreenPosNDC.y);
		}
		GLFUNC(glEnd)();
	}	
	GLFUNC(glEndList)();
}

void CSpriteProcessor::PutOculusDistortionMesh(int texId)
{
	g->tm.BindTexture(texId);
	g->stp.PrepareForRender();	
	GLFUNC(glClearColor)(0, 0, 0, 0);
	GLFUNC(glClear)(GL_COLOR_BUFFER_BIT);
	GLFUNC(glCallList)(m_distortionMesh);
	g->tm.RestoreDefaultSettings();
}
*/

void CSpriteProcessor::CreateOculusDistortionMesh(IOculusRiftSupport::DistortionMesh* mesh, CShader* shader)
{
	if (!m_distortionMesh)
	{
		m_distortionMesh = new CBaseBufferObject();
	}

	IBaseBufferObject::VertexAttribDesc attribDesc[] = {
		{shader->GetAttribLocation("position"), 2, GL_FLOAT, offsetof(IOculusRiftSupport::DistortionVertex, ScreenPosNDC)},
        {shader->GetAttribLocation("timeWarpLerp"), 1, GL_FLOAT, offsetof(IOculusRiftSupport::DistortionVertex, TimeWarpFactor)},
		{shader->GetAttribLocation("inVignette"), 1, GL_FLOAT, offsetof(IOculusRiftSupport::DistortionVertex, VignetteFactor)},
		{shader->GetAttribLocation("eyeType"),	1, GL_FLOAT, offsetof(IOculusRiftSupport::DistortionVertex, EyeType)},
        {shader->GetAttribLocation("tanEyeAnglesR"), 2, GL_FLOAT, offsetof(IOculusRiftSupport::DistortionVertex, TanEyeAnglesR)},
		{shader->GetAttribLocation("tanEyeAnglesG"), 2, GL_FLOAT, offsetof(IOculusRiftSupport::DistortionVertex, TanEyeAnglesG)},
		{shader->GetAttribLocation("tanEyeAnglesB"), 2, GL_FLOAT, offsetof(IOculusRiftSupport::DistortionVertex, TanEyeAnglesB)}
	};

	m_distortionMesh->SetPrimitiveType(GL_TRIANGLES);
	m_distortionMesh->SetVertexDescSize(sizeof(IOculusRiftSupport::DistortionVertex));
	m_distortionMesh->SetAttribDesc(attribDesc, sizeof(attribDesc) / sizeof(attribDesc[0]));
	m_distortionMesh->SetVertexArray(mesh->pVertexData, mesh->VertexCount);
	m_distortionMesh->SetFaceArray(mesh->pIndexData, mesh->IndexCount / 3);
}

void CSpriteProcessor::PutOculusDistortionMesh(int texId)
{
	g->tm.BindTexture(texId);
	GLFUNC(glClearColor)(0, 0, 0, 0);
	GLFUNC(glClear)(GL_COLOR_BUFFER_BIT);
	//glPolygonMode(GL_LINES, GL_FRONT_AND_BACK);
	m_distortionMesh->Render();
}

CSpriteProcessor::~CSpriteProcessor()
{
	if (m_distortionMesh)
	{
		delete m_distortionMesh;
	}
}