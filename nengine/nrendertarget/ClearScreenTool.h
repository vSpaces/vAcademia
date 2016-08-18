#pragma once

#include "ITool.h"
#include "Color4.h"

class CClearScreenTool : public ITool
{
public:
	CClearScreenTool();
	~CClearScreenTool();

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
	IDrawingTarget* m_drawingTarget;

	CColor4 m_color;
};