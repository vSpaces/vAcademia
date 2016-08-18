
#pragma once

#include "CommonEngineHeader.h"

#include <string>
#include "Font.h"
#include "BaseSprite.h"

class CTextSprite : public CBaseSprite
{
public:
	CTextSprite();
	~CTextSprite();

	virtual void SetText(const std::wstring& text);
	std::wstring GetText()const;

	void SetFont(const std::string& fontName);
	std::string GetFontName()const;

	void SetFontSize(const unsigned char size);
	unsigned char GetFontSize()const;

	void SetColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a);
	void GetColor(unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a);

	void SetItalicStyle(const bool isItalic);
	bool IsItalicEnabled()const;

	void SetBoldStyle(const bool isBold);
	bool IsBoldEnabled()const;

	void SetUnderlineStyle(const bool isUnderline);
	bool IsUnderlineEnabled()const;

	void SetAntialiasing(const bool isEnabled);
	bool IsAntialiasingEnabled()const;

	virtual int GetRealWidth();
	int GetRealWidth(std::wstring& str);
	virtual int GetRealHeight();
	int GetAlphabetHeight();

	void SetVerticalAlign(const unsigned char align);
	unsigned char GetVerticalAlign()const;
	
	void SetSelectionCoords(const int from, const int to); 
	void SetBkgColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a);

	virtual void Draw(CPolygons2DAccum* const accum);
	virtual void Draw(const int x, const int y, CPolygons2DAccum* const accum);

	void ClearSelection();
	void SetSelection(const int x, const int y, const int w, const int h);
	void SetSelectionXY(const int x, const int y);
	void SetSelectionWH(const int w, const int h);
	int GetSelectionX()const;
	int GetSelectionY()const;
	int GetSelectionWidth()const;
	int GetSelectionHeight()const;

	unsigned char GetFontID()const;

	void UpdateIfNeeded();

protected:
	void CleanUp();

	CFont* m_font;

	MP_STRING m_fontName;
	MP_WSTRING m_text;
	MP_WSTRING m_dotsText;
	
	int m_cursorX;
	int m_cursorY;
	unsigned int m_cursorPos;
	int m_selectionFrom;
	int m_selectionTo;
	int m_selectionX;
	int m_selectionY;
	int m_selectionWidth;
	int m_selectionHeight;

	unsigned short m_lineHeight;
	
	int m_lastWidth;
	int m_lastHeight;

	unsigned char m_r, m_g, m_b, m_a;
	unsigned char m_bkg_r, m_bkg_g, m_bkg_b, m_bkg_a;
	unsigned char m_size;
	unsigned char m_vAlign;
	unsigned char m_cursorHeight;
	
	bool m_isBold;
	bool m_isItalic;
	bool m_isUnderline;
	bool m_isAntialiasing;

	bool m_isNeedToUpdate;
	bool m_isNeedToUpdateWidth;
};