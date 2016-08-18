
#pragma once

#include "CommonEngineHeader.h"

#include "CommonObject.h"
#include "Polygons2DAccum.h"

#define STYLE_NORMAL					1
#define STYLE_BOLD						2
#define STYLE_ITALIC					4
#define STYLE_UNDERLINE					8
#define STYLE_BOLD_ITALIC				16
#define STYLE_BOLD_UNDERLINE			32
#define STYLE_ITALIC_UNDERLINE			64
#define STYLE_BOLD_ITALIC_UNDERLINE		128

#define FONT_ALIGN_LEFT		0
#define FONT_ALIGN_CENTER	1
#define FONT_ALIGN_RIGHT	2

#define FONT_VALIGN_BOTTOM	0
#define FONT_VALIGN_MIDDLE	1
#define FONT_VALIGN_TOP		2

class CFont : public CCommonObject
{
friend class CFontManager;

public:
	CFont();
	~CFont();

	void LoadFromFile(std::string fileName, std::string family, int fontSize);

	void SaveFontTexture(std::wstring texturePath);

	void SetUnderliness(bool isUnderline);
	bool isUnderlined()const;

	void SetAntialiasing(bool isAntialiasing);
	bool IsAntialiased()const;

	void SetTextScale(float scale);
	float GetTextScale()const;

	bool IsUnderline()const;
	bool IsBold()const;
	bool IsItalic()const;

	int GetFontSize()const;

	std::string GetFamily()const;

	void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	void GetColor(unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);

	float GetLineHeight();

	void OutTextXY(int  x, int y, std::wstring text, int align = FONT_ALIGN_LEFT, int valign = FONT_VALIGN_BOTTOM, CPolygons2DAccum* accum = NULL);
	void OutTextXInverseY(int x, int y, std::wstring text, int align = FONT_ALIGN_LEFT, int valign = FONT_VALIGN_BOTTOM);

	void PutCursor(int x, int y, std::wstring text, int align = FONT_ALIGN_LEFT);

	int GetStringWidth(std::wstring text);
	int GetStringHeight(std::wstring text);
	int GetAlphabetHeight();

	void FreeResources();

protected:
	void SetStyle(bool isBold, bool isItalic);

private:
	void* m_font;
	int m_fontSize;

	bool m_isBold;
	bool m_isItalic;
	bool m_isUnderline;
	bool m_isAntialised;

	float m_textScale;

	MP_STRING m_family;

	unsigned char m_colorR, m_colorG, m_colorB, m_colorA;

	unsigned short m_alphabetHeight; 
};
