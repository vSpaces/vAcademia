
#pragma once

#include "BaseSprite.h"
#include "Color3.h"

class CLine : public CBaseSprite
{
public:
	CLine();
	~CLine();

	void SetLineWidth(const unsigned char lineWidth);
	unsigned char GetLineWidth()const;

	void SetColor(unsigned char r, unsigned char g, unsigned char b);

	void SetAlpha(const unsigned char alpha);
	unsigned char GetAlpha();

	void SetSecondaryCoords(int x, int y);

	bool IsNeedToUpdate();

	void Draw(CPolygons2DAccum* const accum);
	void Draw(const int x, const int y, CPolygons2DAccum* const accum);

private:
	unsigned char m_lineWidth;
	unsigned char m_alpha;

	int m_x2, m_y2;

	CColor3 m_color;
};