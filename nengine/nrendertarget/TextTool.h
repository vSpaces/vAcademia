
#pragma once

#include "CommonRenderTargetHeader.h"

#include "Font.h"
#include "ITool.h"
#include "Color4.h"

#define WORK_MODE_FREE		0
#define WORK_MODE_START		1
#define WORK_MODE_TYPING	2
#define WORK_MODE_PASTE		3

class CTextTool : public ITool
{
public:
	CTextTool();
	~CTextTool();

	void SetDrawingTarget(IDrawingTarget* drawingTarget);
	void SetParameter(int paramID, void* param);

	void OnMouseMove(int x, int y);
	void OnLMouseDown(int x, int y);
	void OnLMouseUp(int x, int y);
	void OnRMouseDown(int x, int y);
	void OnRMouseUp(int x, int y);
	void OnChar(int keycode, int scancode);

	void OnSelected();
	void OnDeselected();
	void OnEndRestoreState();

	void RedoAction(void* data);
	void PasteText(wchar_t* result, bool isPasteOnIconClick = false);

	int GetSynchRequiredType();

	void Destroy();

private:
	void Draw(int x, int y);
	void FastDraw(int x, int y);
	void UpdateFont();
	void MiddleMoving();

	void PushWord(bool isDrawAnything = false);

	IDrawingTarget* m_drawingTarget;

	MP_WSTRING m_text;
	MP_WSTRING m_bufferText;

	CFont* m_font;
	
	MP_STRING m_fontName;
	CColor4 m_color;
	int m_workMode;
	int m_fontSize;
	bool m_isItalic;
	bool m_isUnderline;
	bool m_isAntialiasing;
	bool m_isBold;
	unsigned char m_textOpacity;

	bool m_isFontChanged;
	bool m_isNeedToCorrectCursor;

	bool m_isWorking;
	int m_x, m_y;
	int m_lastTextHeight;
	int m_pasteX, m_pasteY;
	int m_bufferPasteX;
	int m_bufferPasteY;
};