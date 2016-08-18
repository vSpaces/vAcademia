
#pragma once

#include "Sprite.h"

class CPaper : public CSprite
{
public:
	CPaper();
	~CPaper();

	void SetMargins(int top, int left, int bottom, int right);
	void Draw(CPolygons2DAccum* const accum);
	void Draw(const int x, const int y, CPolygons2DAccum* const accum);
	void SetTiling(bool isTiling);

	bool IsPixelTransparent(const int tx, const int ty)const;

private:
	int m_topMargin, m_bottomMargin;
	int m_leftMargin, m_rightMargin;
	int m_isTiling;
};