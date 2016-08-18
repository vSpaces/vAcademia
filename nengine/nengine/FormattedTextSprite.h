
#pragma once

#include "CommonEngineHeader.h"

#include <vector>
#include "TextSprite.h"

#define ALIGN_LEFT		0
#define ALIGN_CENTER	1
#define ALIGN_RIGHT		2

typedef struct _SPart
{
	MP_WSTRING text;
	bool isBold;
	bool isItalic;
	bool isUnderline;
	bool isShadowEnabled;
	short r, _g, b;
	int x, y;
	CTextSprite* textSprite;
	int height;
	int lineID;
	int lineWidth;
	short shadowR, shadowG, shadowB;
	short shadowOffsetX, shadowOffsetY;

	_SPart();
	~_SPart();	
} SPart;

class CFormattedTextSprite : public CTextSprite
{
public:
	CFormattedTextSprite();
	~CFormattedTextSprite();

	void PermitTags(const bool isEnabled);
	bool IsTagsPermitted()const;

	int GetRealWidth();
	int GetRealHeight();

	int GetCursorX()const;
	int GetCursorY()const;
	int GetCursorPos()const;
	int GetCursorHeight();

	void GetVisibleBounds(int& begin, int& end);

	void SetCursorPos(const unsigned int pos);
	void SetSelection(int from, int to);
	void IncreaseCursor(unsigned int delta = 1);
	void DecreaseCursor(unsigned int delta = 1);

	void SetLineHeight(const short height);
	void SetDotsText(const std::wstring& text);
	
	void SetText(const std::wstring& text);
	void AddText(const std::wstring& text);

	void SetBkgColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a);

	void Draw(CPolygons2DAccum* const accum);
	void Draw(const int x, const int y, CPolygons2DAccum* const accum);

	void UpdateIfNeeded();

	void SetHorizontalAlign(const int align);
	int GetHorizontalAlign()const;

	// возвращает true, если при форматировании текст не поместился в отведенное под него поле и был обрезан
	bool GetSliced()const 
	{ 
		return m_sliced; 
	}

	bool AreWordBreaksExists();

	bool IsPixelTransparent(const int tx, const int ty)const;

	void PrintDebugInfo();

	int GetSymbolIDByXY(int x, int y);

	void SetFontSize(const unsigned char size);
	void SetColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a);
	void SetItalicStyle(const bool isItalic);
	void SetBoldStyle(const bool isBold);
	void SetUnderlineStyle(const bool isUnderline);
	void SetAntialiasing(const bool isEnabled);
	void SetFont(const std::string& fontName);

private:
	void SetPartSettings(SPart* const part, const unsigned char style, const short r, const short g, const short b, 
		const int x, const int y, const short shadowR, const short shadowG, const short shadowB,
		const short shadowOffsetX, const short shadowOffsetY, const int lineID, const char fontSize);
	void AddText(const int currentY, const int currentLineID, const std::wstring& text);

	void UpdateLineHeight(const int lineID);

	static unsigned char* GetMemoryBuffer(const unsigned int bufferID, const unsigned int size, int defaultValue);

	MP_VECTOR<SPart *> m_parts;
	
	MP_WSTRING m_text;

	bool m_sliced;
	int m_realWidth;
	int m_lastWidth;
	int m_lastHeight;
	int m_horizontalAlign;
	bool m_isTagsEnabled;
	bool m_isTagsAllowed;
	bool m_wordBreaks;
};
