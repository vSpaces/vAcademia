
#include "StdAfx.h"
#include "Font.h"
#include "FTGLTextureFont.h"
#include "FTGLPolygonFont.h"
#include "FTFont.h"	
#include "HelperFunctions.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"

CFont::CFont():
	m_font(NULL),
	m_isBold(false),
	m_isItalic(false),
	m_textScale(1.0f),
	m_alphabetHeight(0),
	m_isUnderline(false),
	m_isAntialised(true),
	MP_STRING_INIT(m_family)
{

}

void CFont::SaveFontTexture(std::wstring texturePath)
{
	if (m_font)
	{
		int textureID = ((FTGLTextureFont*)m_font)->GetFontTextureID();
		if (textureID != -1)
		{
			GLFUNC(glBindTexture)(GL_TEXTURE_2D, textureID);
			GLint width, height;
			GLFUNC(glGetTexLevelParameteriv)(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
			GLFUNC(glGetTexLevelParameteriv)(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
			GLFUNC(glBindTexture)(GL_TEXTURE_2D, 0);
			g->tm.RefreshTextureOptimization();
			g->tm.SaveTexture(textureID, width, height, true, texturePath);
		}
	}
}

void CFont::LoadFromFile(std::string fileName, std::string family, int fontSize)
{
	if (m_font)
	{
		FreeResources();
	}

	if (!m_isAntialised)
	{
		// не смог добиться от класса FTGLTextureGDIFont нормального бленда текстуры шрифта
		// почему то она иногда становится белой.
		MP_NEW_P2(_font, FTGLTextureFont, fileName.c_str(), false);	
		m_font = (void*)_font;		
	}
	else 
	{
		MP_NEW_P2(_font, FTGLTextureFont, fileName.c_str(), true);	
		m_font = (void*)_font;
	}

	((FTFont*)m_font)->FaceSize(fontSize);   

	if( m_fontSize != fontSize || m_family != family)
	{
		m_fontSize = fontSize;
		m_family = family;
		m_alphabetHeight = 0;
	}
}

std::string CFont::GetFamily()const
{
	return m_family;
}

void CFont::SetUnderliness(bool isUnderline)
{
	if( m_isUnderline != isUnderline)
	{
		m_isUnderline = isUnderline;
		m_alphabetHeight = 0;
	}
}

bool CFont::IsUnderline()const
{
	return m_isUnderline;
}

bool CFont::IsBold()const
{
	return m_isBold;
}

bool CFont::IsItalic()const
{
	return m_isItalic;
}

int CFont::GetFontSize()const
{
	return m_fontSize;
}

void CFont::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	m_colorR = r;
	m_colorG = g;
	m_colorB = b;
	m_colorA = a;
}

void CFont::GetColor(unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a)
{
	*r = m_colorR;
	*g = m_colorG;
	*b = m_colorB;
	*a = m_colorA;
}


void CFont::PutCursor(int x, int y, std::wstring text, int align)
{
	g->stp.PushMatrix();
	g->stp.SetColor(150, 150, 150);

	GLFUNC(glTranslatef)(g->sp.GetOpenGLX(x), g->sp.GetOpenGLY(y), 0);
	GLFUNC(glScalef)(2.0f / (float)g->stp.GetCurrentWidth(), 2.0f / (float)g->stp.GetCurrentHeight(), 1);

	GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	//g->stp.SetState(ST_ZTEST, false);
	if (!m_isAntialised)
	{
		//glDisable(GL_TEXTURE_2D);
	}

	float x1, x2, y1, y2, z1, z2;
	if (text.size() > 0)
	{
		((FTFont*)m_font)->BBox(text.c_str(), x1, y1, z1, x2, y2, z2);
	}
	else
	{
		((FTFont*)m_font)->BBox("1", x1, y1, z1, x2, y2, z2);
		x2 = 0;
	}	
	GLFUNC(glTranslatef)(x2, 0, 0);

	if (align == FONT_ALIGN_RIGHT)
	{
		float dx = x2 - x1;
		GLFUNC(glTranslatef)(-dx, 0, 0);
	}
	else if (align == FONT_ALIGN_CENTER)
	{
		float dx = (x2 - x1) / 2.0f;
		GLFUNC(glTranslatef)(-dx, 0, 0);
	}
	
	x1 = 0; y1 = 0; y2 = ((FTFont*)m_font)->LineHeight() / 2; 
	x2 = y2 * 0.2f;

	g->stp.PrepareForRender();

	GLFUNC(glDisable)(GL_TEXTURE_2D);
	GLFUNC(glBegin)(GL_QUADS);
		GLFUNC(glVertex2f)(x1,y1);
		GLFUNC(glVertex2f)(x2,y1);
		GLFUNC(glVertex2f)(x2,y2);
		GLFUNC(glVertex2f)(x1,y2);
	GLFUNC(glEnd)();
	GLFUNC(glEnable)(GL_TEXTURE_2D);

	if (!m_isAntialised)
	{
		//glEnable(GL_TEXTURE_2D);
	}
	g->stp.SetColor(255, 255, 255);
	//g->stp.SetState(ST_ZTEST, true);
	GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	GLFUNC(glPopMatrix)();
}

int CFont::GetStringWidth(std::wstring text)
{
	assert(m_font);
	if (!m_font)
	{
		return 0;
	}

	float x1, x2, y1, y2, z1, z2;
	((FTFont*)m_font)->BBox(text.c_str(), x1, y1, z1, x2, y2, z2);
	return (int)(fabsf(x2)) + (((int)x2 != x2) ? 1 : 0);
	//added lineWidth, + 1 nikitin
}

int CFont::GetStringHeight(std::wstring text)
{
	assert(m_font);
	if (!m_font)
	{
		return 0;
	}

	float x1, x2, y1, y2, z1, z2;
	((FTFont*)m_font)->BBox(text.c_str(), x1, y1, z1, x2, y2, z2);
	if (!m_isUnderline)
	{
		int _y1 = (int)y1;
		if (_y1 > y1) 
		{
			_y1--;
		}
		int _y2 = (int)y2;
		if (_y2 < y2) 
		{
			_y2++;
		}

		return (int)(fabsf((float)(_y2 - _y1 + 1)));
	}
	else
	{
		return (int)(fabsf((float)(y2 - y1 + 1)) + ((FTFont*)m_font)->LineHeight() * 0.2f);
	}
}

int CFont::GetAlphabetHeight()
{
	if (m_alphabetHeight == 0)
	{
//		int h = ((FTFont*)m_font)->Ascender() - ((FTFont*)m_font)->Descender();
		m_alphabetHeight = (unsigned short)GetStringHeight(L"Aabcdefghijklmnopqrstuvwxyz");
	}
	
	return m_alphabetHeight;
}

void CFont::SetTextScale(float scale)
{
	m_textScale = scale;
}

float CFont::GetTextScale()const
{
	return m_textScale;
}

void CFont::OutTextXY(int x, int y, std::wstring text, int align, int valign, CPolygons2DAccum* accum)
{
	assert(m_font);
	if (!m_font)
	{
		return;
	}

	// нормальная поддержка underline
	if (m_isUnderline)
	{
		++y;
	}

	g->stp.PushMatrix();
	g->stp.SetColor(m_colorR, m_colorG, m_colorB, m_colorA);

	float x1, x2, y1, y2, z1, z2;
	float ny1, ny2;
	((FTFont*)m_font)->GetSmallAlphabetY(y1, y2);	
	((FTFont*)m_font)->BBox(text.c_str(), x1, ny1, z1, x2, ny2, z2);

	int _y1 = (int)y1;
	if (_y1 > y1) 
	{
		_y1--;
	}
	int _y2 = (int)y2;
	if (_y2 < y2) 
	{
		_y2++;
	}

	int dy = (int)(fabsf((float)(_y2 - _y1 + 1)));

	int addY = 0;
	if (FONT_VALIGN_MIDDLE == valign)
	{
		addY = (dy / 2);
	}
	else if (FONT_VALIGN_TOP == valign)
	{
		addY = dy;
	}

	addY = (int)(addY * m_textScale);

	if (accum)
	{
		accum->ChangeOrientParams(g->sp.GetOpenGLX(x), g->sp.GetOpenGLY(y + addY),
			2.0f / (float)g->stp.GetCurrentWidth(), 2.0f / (float)g->stp.GetCurrentHeight());
	}

	GLFUNC(glTranslatef)(g->sp.GetOpenGLX(x), g->sp.GetOpenGLY(y + addY), 0);
	GLFUNC(glScalef)(2.0f / (float)g->stp.GetCurrentWidth() * m_textScale, 2.0f / (float)g->stp.GetCurrentHeight() * m_textScale, 1);

	g->stp.SetState(ST_BLEND, true);
	g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//g->stp.SetState(ST_ZTEST, false);
	GLFUNC(glEnable)(GL_TEXTURE_2D);
	if (!m_isAntialised)
	{
		//glDisable(GL_TEXTURE_2D);
	}

	if (align == FONT_ALIGN_RIGHT)
	{
		float dx = x2 - x1;
		GLFUNC(glTranslatef)(-dx, 0, 0);
	}
	else if (align == FONT_ALIGN_CENTER)
	{
		float dx = (x2 - x1) / 2;
		GLFUNC(glTranslatef)(-dx, 0, 0);
	}

	GLFUNC(glTranslatef)(0, 0, 0);	

	if (accum)
	{
		accum->SetColor(m_colorR, m_colorG, m_colorB, m_colorA);
	}
	else
	{
		g->stp.PrepareForRender();
	}

	if (m_isUnderline)
	{
		y1 = -1.0f * ((FTFont*)m_font)->LineHeight() * -0.07f;
		y2 = -1.0f * ((FTFont*)m_font)->LineHeight() * 0.0f;

		if (!accum)
		{
			GLFUNC(glDisable)(GL_TEXTURE_2D);
			GLFUNC(glBegin)(GL_QUADS);
				GLFUNC(glVertex2f)(x1,y1);
				GLFUNC(glVertex2f)(x2,y1);
				GLFUNC(glVertex2f)(x2,y2);
				GLFUNC(glVertex2f)(x1,y2);
			GLFUNC(glEnd)();
			GLFUNC(glEnable)(GL_TEXTURE_2D);		
		}
		else
		{		
			accum->ChangeMaterial(-1, BLEND_MODE_NO_BLEND);
			accum->AddTextureCoords(0, 0);
			accum->AddVertex(x1, y1);
			accum->AddTextureCoords(0, 0);
			accum->AddVertex(x2, y1);
			accum->AddTextureCoords(0, 0);
			accum->AddVertex(x2, y2);
			accum->AddTextureCoords(0, 0);
			accum->AddVertex(x1, y2);
		}
	}

	

	// text
    ((FTFont*)m_font)->Render(text.c_str(), (IPolygonsAccum*)accum);

	g->stp.SetColor(255, 255, 255);
	if (!m_isAntialised)
	{
		//glEnable(GL_TEXTURE_2D);
	}
	//g->stp.SetState(ST_ZTEST, true);

	g->stp.SetState(ST_BLEND, false);
	g->stp.PrepareForRender();

	g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	g->stp.PopMatrix();

	if (accum)
	{
		accum->ChangeOrientParams(0.0f, 0.0f, 1.0f, 1.0f);
	}

	g->tm.RefreshTextureOptimization();
}

void CFont::OutTextXInverseY(int x, int y, std::wstring text, int align, int valign)
{
	OutTextXY(x, -y, text, align, valign);
}

void CFont::SetStyle(bool isBold, bool isItalic)
{
	if( m_isBold != isBold || m_isItalic != isItalic)
	{
		m_isBold = isBold;
		m_isItalic = isItalic;
		m_alphabetHeight = 0;
	}
}

void CFont::SetAntialiasing(bool isAntialiasing)
{
	if( m_isAntialised != isAntialiasing)
	{
		m_isAntialised = isAntialiasing;
		m_alphabetHeight = 0;
	}
}

bool CFont::IsAntialiased()const
{
	return m_isAntialised;
}

float CFont::GetLineHeight()
{
	assert(m_font);
	if (!m_font)
	{
		return 0.0f;
	}

	return ((FTFont*)m_font)->LineHeight();
}

void CFont::FreeResources()
{
	if (m_font)
	{
		FTGLTextureFont* fnt = (FTGLTextureFont*)m_font;
		MP_DELETE(fnt);
		m_font = NULL;
	}
}

CFont::~CFont()
{
    FreeResources();
}