
#include "StdAfx.h"
#include "Line.h"

CLine::CLine():
	CBaseSprite(SPRITE_TYPE_LINE),
	m_lineWidth(1),
	m_alpha(255),
	m_x2(0),
	m_y2(0)
{
	SetLoadedStatus(true);
}

void CLine::SetAlpha(const unsigned char alpha)
{
	m_alpha = alpha;
}

unsigned char CLine::GetAlpha()
{
	return m_alpha;
}

void CLine::SetLineWidth(const unsigned char lineWidth)
{
	m_lineWidth = lineWidth;
}

unsigned char CLine::GetLineWidth()const
{
	return m_lineWidth;
}

void CLine::SetColor(unsigned char r, unsigned char g, unsigned char b)
{
	m_color.r = r;
	m_color.g = g;
	m_color.b = b;
}

bool CLine::IsNeedToUpdate()
{
	return false;
}

void CLine::Draw(CPolygons2DAccum* const accum)
{
	Draw(0, 0, accum);
}

void CLine::Draw(const int /*x*/, const int /*y*/, CPolygons2DAccum* const accum)
{
	int x1 = GetX();
	int y1 = GetY();
	int x2 = m_x2;
	int y2 = m_y2;

	float _x1 = g->sp.GetOpenGLX(x1);
	float _x2 = g->sp.GetOpenGLX(x2);
	float _y1 = g->sp.GetOpenGLY(y1);
	float _y2 = g->sp.GetOpenGLY(y2);
	
	if (accum)	
	{
		accum->SetLineWidth(m_lineWidth);
		accum->ChangeMaterialExt(-1, -1, BLEND_MODE_LINE, m_color.r, m_color.g, m_color.b, m_alpha);
		accum->AddTextureCoords(0, 0);
		accum->AddVertex(_x1, _y1);
		accum->AddTextureCoords(0, 0);
		accum->AddVertex(_x2, _y2);		
		accum->SetLineWidth(1);
	}
	else
	{
		g->stp.SetColor(m_color.r, m_color.g, m_color.b);
		g->stp.PrepareForRender();

		GLFUNC(glDisable)(GL_TEXTURE_2D);

		glLineWidth((GLfloat)m_lineWidth);
		glBegin(GL_LINES);		
		glVertex2f(_x1, _y1);
		glVertex2f(_x2, _y2);		
		glEnd();
		glLineWidth(1);

		GLFUNC(glEnable)(GL_TEXTURE_2D);
	}
}

void CLine::SetSecondaryCoords(int x, int y)
{
	m_x2 = x;
	m_y2 = y;
}

CLine::~CLine()
{
}