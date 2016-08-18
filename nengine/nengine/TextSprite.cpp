
#include "StdAfx.h"
#include "TextSprite.h"
#include "HelperFunctions.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"

#define SAFELY_UPDATE_PARAM(x) if (m_##x != x) { m_isNeedToUpdate = true; m_lastWidth = 0; if (GetScene2D()) { GetScene2D()->OnParamsChanged(); } m_##x = x; }

CTextSprite::CTextSprite():
	m_r(255), 
	m_g(255), 
	m_b(255), 
	m_a(255),
	m_bkg_r(0),
	m_bkg_g(0),
	m_bkg_b(0),
	m_bkg_a(0xAA),
	m_size(12),
	m_font(NULL),
	m_cursorX(0),
	m_cursorY(0),
	m_cursorPos(0),
	m_lastWidth(0),
	m_selectionX(0),
	m_selectionY(0),
	m_lastHeight(0),
	m_isBold(false),
	m_lineHeight(0xFFFF),
	m_selectionTo(0),	
	m_cursorHeight(0),
	m_isItalic(false),
	m_selectionFrom(0),
	m_selectionWidth(0),
	m_isUnderline(false),	
	m_selectionHeight(0),
	m_isNeedToUpdate(true),
	m_isAntialiasing(false),
	m_isNeedToUpdateWidth(true),
	m_vAlign(FONT_VALIGN_BOTTOM),
	CBaseSprite(SPRITE_TYPE_TEXT),
	MP_STRING_INIT(m_fontName),
	MP_WSTRING_INIT(m_text),
	MP_WSTRING_INIT(m_dotsText)
{
	m_fontName = "Times New Roman";
	SetLoadedStatus(true);
}

void CTextSprite::SetText(const std::wstring& text)
{
	if (m_text != text)
	{
		if ((GetScene2D()) && (m_isVisible))
		{
			GetScene2D()->OnParamsChanged();
		}

		m_isNeedToUpdateWidth = true;		

		m_text = text;
	}    
}

std::wstring CTextSprite::GetText()const
{
	return m_text;
}

void CTextSprite::SetFont(const std::string& fontName)
{
	SAFELY_UPDATE_PARAM(fontName);
}

std::string CTextSprite::GetFontName()const
{
	return m_fontName;
}

void CTextSprite::SetFontSize(const unsigned char size)
{
	SAFELY_UPDATE_PARAM(size);	
}

unsigned char CTextSprite::GetFontSize()const
{
	return m_size;
}

void CTextSprite::SetColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
{
	if ((m_r != r) || (m_g != g) || (m_b != b) || (m_a != a))
	{
		if ((GetScene2D()) && (m_isVisible))
		{
			GetScene2D()->OnParamsChanged();
		}
	}

	m_r = r;
	m_g = g;
	m_b = b;
	m_a = a;
}

void CTextSprite::SetBkgColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
{
	m_bkg_a = a;
	m_bkg_r = r;
	m_bkg_g = g;
	m_bkg_b = b;
}

void CTextSprite::GetColor(unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a)
{
	r = m_r;
	g = m_g;
	b = m_b;
	a = m_a;
}

void CTextSprite::SetItalicStyle(const bool isItalic)
{
	SAFELY_UPDATE_PARAM(isItalic);
}

bool CTextSprite::IsItalicEnabled()const
{
	return m_isItalic;
}

void CTextSprite::SetBoldStyle(const bool isBold)
{
	SAFELY_UPDATE_PARAM(isBold);
}

bool CTextSprite::IsBoldEnabled()const
{
	return m_isBold;
}

void CTextSprite::SetUnderlineStyle(const bool isUnderline)
{
	SAFELY_UPDATE_PARAM(isUnderline);
}

bool CTextSprite::IsUnderlineEnabled()const
{
	return m_isUnderline;
}

void CTextSprite::SetAntialiasing(const bool isAntialiasing)
{
	SAFELY_UPDATE_PARAM(isAntialiasing);
	m_isAntialiasing = false;
}

bool CTextSprite::IsAntialiasingEnabled()const
{
	return m_isAntialiasing;
}

void CTextSprite::SetSelectionCoords(const int from, const int to)
{
	if ((this->m_selectionFrom != from) ||
		(this->m_selectionTo != to))
	{
		if ((GetScene2D()) && (m_isVisible))
		{
			GetScene2D()->OnParamsChanged();
		}

		CleanUp();
	}

	this->m_selectionFrom = from;
	this->m_selectionTo = to;
}

void CTextSprite::Draw(CPolygons2DAccum* const accum)
{
	Draw(m_x, m_y, accum);
}

void CTextSprite::UpdateIfNeeded()
{
	if (m_isNeedToUpdate)
	{
		if (m_font)
		{
			g->fm.DeleteFont(m_font);
		}
	
		m_font = g->fm.GetFont(m_fontName, m_size, m_isBold, m_isItalic, m_isUnderline, m_isAntialiasing);		

		m_isNeedToUpdate = false;
	}

	if (m_font)
	{
		m_font->SetUnderliness(m_isUnderline);
	}
}

unsigned char CTextSprite::GetFontID()const
{
	return (unsigned char)m_font;
}

void CTextSprite::Draw(const int x, const int y, CPolygons2DAccum* const accum)
{
	stat_2dVisibleTextCount++;

	UpdateIfNeeded();	

	if (this->m_selectionFrom < this->m_selectionTo)
	{
		if (m_font)
		{
			if (this->m_selectionWidth > 0 && this->m_selectionHeight > 0)
			{
				m_font->SetColor(m_r, m_g, m_b, m_a);
				g->sp.PutOneColorQuad(x + m_selectionX, y, x + m_selectionX + m_selectionWidth, y + m_selectionHeight, m_bkg_r, m_bkg_g, m_bkg_b, m_bkg_a, accum);
			}

			std::wstring before = L"";
			std::wstring after = L"";
			std::wstring now = L"";
			
			before.assign(m_text, 0, m_selectionFrom);
			now.assign(m_text, m_selectionFrom, m_selectionTo - m_selectionFrom);
			int dif = m_text.size() - m_selectionTo;
			if (dif > 0)
				after.assign(m_text, m_selectionTo, m_text.size() - m_selectionTo);

			if (!before.empty())
			{
				m_font->SetColor(m_r, m_g, m_b, m_a);
				g->stp.SetColor(m_r, m_g, m_b, m_a);			
				m_font->OutTextXY(x, y, before.c_str(), 0, m_vAlign, accum);
			}
			if (!now.empty())
			{
				m_font->SetColor(0xFF - m_r, 0xFF - m_g, 0xFF - m_b, m_a);
				g->stp.SetColor(0xFF - m_r, 0xFF - m_g, 0xFF - m_b, m_a);				
				m_font->OutTextXY(x + m_selectionX, y, now.c_str(), 0, m_vAlign, accum);
				m_font->SetColor(m_r, m_g, m_b, m_a);
			}
			if (!after.empty())
			{
				m_font->SetColor(m_r, m_g, m_b, m_a);
				g->stp.SetColor(m_r, m_g, m_b, m_a);		
				m_font->OutTextXY(x + m_selectionX + m_selectionWidth, y, after.c_str(), 0, m_vAlign, accum);
			}
		}
	}
	else
	{
		if (m_font)
		{
			m_font->SetColor(m_r, m_g, m_b, m_a);		
			m_font->OutTextXY(x, y, m_text.c_str(), 0, m_vAlign, accum);
		}
	}
}

int CTextSprite::GetRealWidth()
{
	if (m_text.empty())
	{
		return 0;
	}

	if (m_isNeedToUpdate)
	{
		m_isNeedToUpdateWidth = true;
	}

	UpdateIfNeeded();

	if (!m_font)
	{
		return 0;
	}

	if ((!m_isNeedToUpdateWidth) && (m_lastWidth != 0))
	{
		return m_lastWidth;
	}

	int width = m_font->GetStringWidth(m_text.c_str());
	
	if ((width > m_width) && (m_width != 0))
	{
		m_lastWidth = m_width;
		return m_width;
	}
	else
	{
		m_lastWidth = width;
		return width;
	}
}

int CTextSprite::GetRealWidth(std::wstring& str)
{
	if (str.empty())
	{
		return 0;
	}

	UpdateIfNeeded();

	if (!m_font)
	{
		return 0;
	}	

	return m_font->GetStringWidth(str.c_str());
}

int CTextSprite::GetRealHeight()
{
	if (m_text.empty())
	{
		return 0;
	}

	bool isNeedToUpdateHeight = m_isNeedToUpdate;

	UpdateIfNeeded();

	if (!m_font)
	{
		return 0;
	}

	if ((!isNeedToUpdateHeight) && (m_lastHeight != 0))
	{
		return m_lastHeight;
	}

	int height = m_font->GetStringHeight(m_text.c_str());

	if ((height > m_height) && (m_height != 0))
	{
		m_lastHeight = height;
		return m_height;
	}
	else
	{
		m_lastHeight = height;
		return height;
	}
}

int CTextSprite::GetAlphabetHeight()
{
	if (m_text.empty())
	{
		return 0;
	}

	UpdateIfNeeded();

	if (!m_font)
	{
		return 0;
	}

	int height = m_font->GetStringHeight(m_text.c_str());
	int alphabetHeight = m_font->GetAlphabetHeight();
	if (alphabetHeight > height)
	{
		height = alphabetHeight;
	}

	if ((height > m_height) && (m_height != 0))
	{
		return m_height;
	}
	else
	{
		return height;
	}
}

void CTextSprite::SetVerticalAlign(const unsigned char align)
{
	m_vAlign = align;
}

unsigned char CTextSprite::GetVerticalAlign()const
{
	return m_vAlign;
}

void CTextSprite::ClearSelection()
{
	this->m_selectionX = 0;
	this->m_selectionY = 0;
	this->m_selectionHeight = 0;
	this->m_selectionWidth = 0;
	this->m_selectionFrom = 0;
	this->m_selectionTo = 0;

	CleanUp();
}

void CTextSprite::SetSelection(const int x, const int y, const int w, const int h)
{
	this->m_selectionX = x;
	this->m_selectionY = y;
	this->m_selectionHeight = h;
	this->m_selectionWidth = w;

	CleanUp();
}

void CTextSprite::SetSelectionXY(const int x, const int y)
{
	this->m_selectionX = x;
	this->m_selectionY = y;

	CleanUp();
}

void CTextSprite::SetSelectionWH(const int w, const int h)
{
	this->m_selectionHeight = h;
	this->m_selectionWidth = w;

	CleanUp();
}

int CTextSprite::GetSelectionX()const 
{
	return this->m_selectionX;
}
int CTextSprite::GetSelectionY()const 
{
	return this->m_selectionY;
}
int CTextSprite::GetSelectionWidth()const 
{
	return this->m_selectionWidth;
}
int CTextSprite::GetSelectionHeight()const 
{
	return this->m_selectionHeight;
}

void CTextSprite::CleanUp()
{
	m_isNeedToUpdateWidth = true;	
}

CTextSprite::~CTextSprite()
{
	if (m_font)
	{
		g->fm.DeleteFont(m_font);
		m_font = NULL;
	}

	CleanUp();
}