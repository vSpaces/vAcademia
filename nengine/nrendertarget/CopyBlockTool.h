
#pragma once

#include "ITool.h"
#include "Color4.h"

#define WORK_MODE_FREE		0
#define WORK_MODE_SELECTING	1
#define WORK_MODE_MOVING	2

class CCopyBlockTool : public ITool
{
public:
	CCopyBlockTool();
	~CCopyBlockTool();

	void SetDrawingTarget(IDrawingTarget* drawingTarget);
	void SetParameter(int paramID, void* param);

	void OnMouseMove(int x, int y);
	void OnLMouseDown(int x, int y);
	void OnLMouseUp(int x, int y);
	void OnRMouseDown(int x, int y);
	void OnRMouseUp(int x, int y);
	void OnChar(int keycode, int scancode);

	void OnStartMoving();
	void OnEndMoving();

	void OnSelected();
	void OnDeselected();
	void OnEndRestoreState();

	void RedoAction(void* data);

	int GetSynchRequiredType();

	void Destroy();

private:
	void DrawMoving();
	void DrawSelecting();

	IDrawingTarget* m_drawingTarget;

	CColor4 m_color;

	unsigned int m_blockX, m_blockY, m_blockX2, m_blockY2;	
	int m_blockWidth, m_blockHeight;
	unsigned char m_workMode;
	int m_tempTextureID;
	int m_x, m_y;
};