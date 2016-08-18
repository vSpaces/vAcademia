
#include "StdAfx.h"
#include "Paper.h"
#include "GlobalSingletonStorage.h"

CPaper::CPaper():
	m_bottomMargin(0),
	m_rightMargin(0),
	m_leftMargin(0),
	m_topMargin(0),
	m_isTiling(false)
{
	SetType(SPRITE_TYPE_PAPER);
}

void CPaper::SetMargins(int top, int left, int bottom, int right)
{
	m_bottomMargin = bottom;
	m_rightMargin = right;
	m_leftMargin = left;
	m_topMargin = top;
}

void CPaper::SetTiling(bool isTiling)
{
	m_isTiling = isTiling;
}

void CPaper::Draw(CPolygons2DAccum* const accum)
{
	Draw(m_x, m_y, accum);
}

void CPaper::Draw(const int x, const int y, CPolygons2DAccum* const accum)
{
	int textureID = GetTextureID(0);
	if (-1 == textureID)
	{
		return;
	}

	CTexture* texture = g->tm.GetObjectPointer(textureID);
	texture->SetReallyUsedStatus(true);
	bool isAlpha = (texture->IsTransparent() || (m_alpha != 255));

	OnBeforeDraw(accum);

	bool isAllMargineZero = m_leftMargin == 0 && m_rightMargin == 0 && m_topMargin == 0 && m_bottomMargin == 0;
	
	if (accum)
	{
		int tile = m_isTiling ? 2 : 0;
		if (!isAllMargineZero)
		{
			// уголки			
			g->sp.PutSprite(x, y, x + m_leftMargin, y + m_topMargin, m_tx1, m_ty1, m_tx1 + m_leftMargin, m_ty1 + m_topMargin, textureID, m_color, accum, 0, tile, isAlpha, (!isAlpha) ? 255 : m_alpha);
			g->sp.PutSprite(x, y + m_height - m_bottomMargin, x + m_leftMargin, y + m_height, m_tx1, m_ty2 - m_bottomMargin, m_tx1 + m_leftMargin, m_ty2, textureID, m_color, accum, 0, tile, isAlpha, (!isAlpha) ? 255 : m_alpha);
			g->sp.PutSprite(x + m_width - m_rightMargin, y, x + m_width, y + m_topMargin, m_tx2 - m_rightMargin, m_ty1, m_tx2, m_ty1 + m_topMargin, textureID, m_color, accum, 0, tile, isAlpha, (!isAlpha) ? 255 : m_alpha);
			g->sp.PutSprite(x + m_width - m_rightMargin, y + m_height - m_bottomMargin, x + m_width, y + m_height, m_tx2 - m_rightMargin, m_ty2 - m_bottomMargin, m_tx2, m_ty2, textureID, m_color, accum, 0, tile, isAlpha, (!isAlpha) ? 255 : m_alpha);

			// прямоугольники
			//up
			g->sp.PutSprite(x + m_leftMargin, y, x + m_width - m_rightMargin, y + m_topMargin, m_tx1 + m_leftMargin, m_ty1, m_tx2 - m_rightMargin, m_ty1 + m_topMargin, textureID, m_color, accum, 0, tile, isAlpha, (!isAlpha) ? 255 : m_alpha);
			//down
			g->sp.PutSprite(x + m_leftMargin, y + m_height - m_bottomMargin, x + m_width - m_rightMargin, y + m_height, m_tx1 + m_leftMargin, m_ty2 - m_bottomMargin, m_tx2 - m_rightMargin, m_ty2, textureID, m_color, accum, 0, tile, isAlpha, (!isAlpha) ? 255 : m_alpha);
			// left
			g->sp.PutSprite(x, y + m_topMargin, x + m_leftMargin, y + m_height - m_bottomMargin, m_tx1, m_ty1 + m_topMargin, m_tx1 + m_leftMargin, m_ty2 - m_bottomMargin, textureID, m_color, accum, 0, tile, isAlpha, (!isAlpha) ? 255 : m_alpha);
			//right
			g->sp.PutSprite(x + m_width - m_rightMargin, y + m_topMargin, x + m_width, y + m_height - m_bottomMargin, m_tx2 - m_rightMargin, m_ty1 + m_topMargin, m_tx2, m_ty2 - m_bottomMargin, textureID, m_color, accum, 0, tile, isAlpha, (!isAlpha) ? 255 : m_alpha);

			// большая часть
			g->sp.PutSprite(x + m_leftMargin, y + m_topMargin, x + m_width - m_rightMargin, y + m_height - m_bottomMargin, m_tx1 + m_leftMargin + 1, m_ty1 + m_topMargin + 1, m_tx2 - m_rightMargin - 1, m_ty2 - m_bottomMargin - 1, textureID, m_color, accum, 0, tile, isAlpha, (!isAlpha) ? 255 : m_alpha);
		}
		else
		{
			// большая часть
			g->sp.PutSprite(x + m_leftMargin, y + m_topMargin, x + m_width - m_rightMargin, y + m_height - m_bottomMargin, m_tx1 + m_leftMargin, m_ty1 + m_topMargin, m_tx2 - m_rightMargin, m_ty2 - m_bottomMargin, textureID, m_color, accum, 0, tile, isAlpha, (!isAlpha) ? 255 : m_alpha);
		}

	}
	else if (!isAlpha)
	{
		if (!isAllMargineZero)
		{
			// уголки
			g->sp.PutNormalSprite(x, y, x + m_leftMargin, y + m_topMargin, m_tx1, m_ty1, m_tx1 + m_leftMargin, m_ty1 + m_topMargin, textureID, m_color);
			g->sp.PutNormalSprite(x, y + m_height - m_bottomMargin, x + m_leftMargin, y + m_height, m_tx1, m_ty2 - m_bottomMargin, m_tx1 + m_leftMargin, m_ty2, textureID, m_color);
			g->sp.PutNormalSprite(x + m_width - m_rightMargin, y, x + m_width, y + m_topMargin, m_tx2 - m_rightMargin, m_ty1, m_tx2, m_ty1 + m_topMargin, textureID, m_color);
			g->sp.PutNormalSprite(x + m_width - m_rightMargin, y + m_height - m_topMargin, x + m_width, y + m_height, m_tx2 - m_rightMargin, m_ty2 - m_bottomMargin, m_tx2, m_ty2, textureID, m_color);

			// прямоугольники
			g->sp.PutNormalSprite(x + m_leftMargin, y, x + m_width - m_rightMargin, y + m_topMargin, m_tx1 + m_leftMargin, m_ty1, m_tx2 - m_rightMargin, m_ty1 + m_topMargin, textureID, m_color);
			g->sp.PutNormalSprite(x + m_leftMargin, y + m_height - m_bottomMargin, x + m_width - m_rightMargin, y + m_height, m_tx1 + m_leftMargin, m_ty2 - m_bottomMargin, m_tx2 - m_rightMargin, m_ty2, textureID,m_color);
			g->sp.PutNormalSprite(x, y + m_topMargin, x + m_leftMargin, y + m_height - m_bottomMargin, m_tx1, m_ty1 + m_topMargin, m_tx1 + m_leftMargin, m_ty2 - m_bottomMargin, textureID, m_color);
			g->sp.PutNormalSprite(x + m_width - m_rightMargin, y + m_topMargin, x + m_width, y + m_height - m_bottomMargin, m_tx2 - m_rightMargin, m_ty1 + m_topMargin, m_tx2, m_ty2 - m_bottomMargin, textureID, m_color);

			// большая часть
			g->sp.PutNormalSprite(x + m_leftMargin, y + m_topMargin, x + m_width - m_rightMargin, y + m_height - m_bottomMargin, m_tx1 + m_leftMargin + 1, m_ty1 + m_topMargin + 1, m_tx2 - m_rightMargin - 1, m_ty2 - m_bottomMargin - 1, textureID, m_color);
		}
		else
		{
			// большая часть
			g->sp.PutNormalSprite(x + m_leftMargin, y + m_topMargin, x + m_width - m_rightMargin, y + m_height - m_bottomMargin, m_tx1 + m_leftMargin, m_ty1 + m_topMargin, m_tx2 - m_rightMargin, m_ty2 - m_bottomMargin, textureID, m_color);
		}

	}
	else
	{
		if (!isAllMargineZero)
		{
			// уголки
			g->sp.PutAlphaSprite(x, y, x + m_leftMargin, y + m_topMargin, m_tx1, m_ty1, m_tx1 + m_leftMargin, m_ty1 + m_topMargin, textureID, m_alpha, m_color);
			g->sp.PutAlphaSprite(x, y + m_height - m_bottomMargin, x + m_leftMargin, y + m_height, m_tx1, m_ty2 - m_bottomMargin, m_tx1 + m_leftMargin, m_ty2, textureID, m_alpha, m_color);
			g->sp.PutAlphaSprite(x + m_width - m_rightMargin, y, x + m_width, y + m_topMargin, m_tx2 - m_rightMargin, m_ty1, m_tx2, m_ty1 + m_topMargin, textureID, m_alpha, m_color);
			g->sp.PutAlphaSprite(x + m_width - m_rightMargin, y + m_height - m_bottomMargin, x + m_width, y + m_height, m_tx2 - m_rightMargin, m_ty2 - m_bottomMargin, m_tx2, m_ty2, textureID, m_alpha, m_color);

			// прямоугольники
			g->sp.PutAlphaSprite(x + m_leftMargin, y, x + m_width - m_rightMargin, y + m_topMargin, m_tx1 + m_leftMargin, m_ty1, m_tx2 - m_rightMargin, m_ty1 + m_topMargin, textureID, m_alpha, m_color);
			g->sp.PutAlphaSprite(x + m_leftMargin, y + m_height - m_bottomMargin, x + m_width - m_rightMargin, y + m_height, m_tx1 + m_leftMargin, m_ty2 - m_bottomMargin, m_tx2 - m_rightMargin, m_ty2, textureID, m_alpha, m_color);
			g->sp.PutAlphaSprite(x, y + m_topMargin, x + m_leftMargin, y + m_height - m_bottomMargin, m_tx1, m_ty1 + m_topMargin, m_tx1 + m_leftMargin, m_ty2 - m_bottomMargin, textureID, m_alpha, m_color);
			g->sp.PutAlphaSprite(x + m_width - m_rightMargin, y + m_topMargin, x + m_width, y + m_height - m_bottomMargin, m_tx2 - m_rightMargin, m_ty1 + m_topMargin, m_tx2, m_ty2 - m_bottomMargin, textureID, m_alpha, m_color);

			// большая часть
			g->sp.PutAlphaSprite(x + m_leftMargin, y + m_topMargin, x + m_width - m_rightMargin, y + m_height - m_bottomMargin, m_tx1 + m_leftMargin + 1, m_ty1 + m_topMargin + 1, m_tx2 - m_rightMargin - 1, m_ty2 - m_bottomMargin - 1, textureID, m_alpha, m_color);
		}
		else
		{
			// большая часть
			g->sp.PutAlphaSprite(x + m_leftMargin, y + m_topMargin, x + m_width - m_rightMargin, y + m_height - m_bottomMargin, m_tx1 + m_leftMargin, m_ty1 + m_topMargin, m_tx2 - m_rightMargin, m_ty2 - m_bottomMargin, textureID, m_alpha, m_color);
		}
	}
	
	OnAfterDraw(accum);
}

bool CPaper::IsPixelTransparent(const int tx, const int ty)const
{
	if (!IsLoaded())
	{
		return false;
	}

	if ((tx >= 0) && (tx < m_width) && (ty >= 0) && (ty < m_height) && (m_width != 0) && (m_height != 0))
	{
		int absX = GetX() + tx;
		int absY = GetY() + ty;
		int maskX, maskY, maskWidth, maskHeight;
		GetScissors(maskX, maskY, maskWidth, maskHeight); 
		if (maskWidth != -1)
		if ((absX < maskX) || (absX > maskX + maskWidth) ||
			(absY < maskY) || (absY > maskY + maskHeight))
		{
			return true;
		}

		unsigned int textureID = GetTextureID(0);
		if (textureID == -1)
		{
			return false;
		}

		CTexture* texture = g->tm.GetObjectPointer(textureID);

		int x = (int)((float)(tx - m_leftMargin) / (float)(m_width - m_leftMargin - m_rightMargin) * (float)(m_tx2 - m_tx1 - m_leftMargin - m_rightMargin)) + m_tx1 + m_leftMargin;
		int y = (int)((float)(ty - m_topMargin) / (float)(m_height - m_topMargin - m_bottomMargin) * (float)(m_ty2 - m_ty1 - m_topMargin - m_bottomMargin)) + m_ty1 + m_topMargin;

		if (tx < m_leftMargin)
		{
			x = tx;
		}

		if (ty < m_topMargin)
		{
			y = ty;
		}

		if (m_width - tx < m_rightMargin)
		{
			x = m_tx2 - (m_width - tx);
		}

		if (m_height - ty < m_bottomMargin)
		{
			y = m_ty2 - (m_height - ty);
		}
		
		return texture->IsPixelTransparent(x, y);		
	}

	return true;
}

CPaper::~CPaper()
{
}