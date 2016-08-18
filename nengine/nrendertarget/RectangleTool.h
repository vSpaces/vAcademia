
#pragma once

#include "ITool.h"
#include "Color4.h"

class CRectangleTool : public ITool
{
public:
	CRectangleTool();
	~CRectangleTool();

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

	int GetSynchRequiredType();

	void Destroy();

private:
	void Draw();
	void MovingMiddle(int x, int y);

	IDrawingTarget* m_drawingTarget;

	CColor4 m_color, m_backColor;	
	unsigned int m_strokeWidth;
	unsigned char m_opacity;
	unsigned char m_mode;

	bool m_isWorking;
	bool m_isMoveMode;

	int m_lastMouseX, m_lastMouseY;
	int m_startX, m_startY;
	int m_endX, m_endY;
};