
#pragma once

#include "IDrawingTarget.h"

#define SYNCH_MOVE_NONE					1
#define SYNCH_MOVE_WHEN_LDOWN			2
#define SYNCH_MOVE_WHEN_RDOWN			4
#define SYNCH_MOVE_WHEN_DOWN			6
#define SYNCH_KEYS						8
#define SYNCH_MOVE_WHEN_DOWN_AND_KEYS	14

class ITool  
{
public:
	virtual void SetDrawingTarget(IDrawingTarget* drawingTarget) = 0;
	virtual void SetParameter(int paramID, void* param) = 0;

	virtual void OnMouseMove(int x, int y) = 0;
	virtual void OnLMouseDown(int x, int y) = 0;
	virtual void OnLMouseUp(int x, int y) = 0;
	virtual void OnRMouseDown(int x, int y) = 0;
	virtual void OnRMouseUp(int x, int y) = 0;
	virtual void OnChar(int keycode, int scancode) = 0;

	virtual void OnSelected() = 0;
	virtual void OnDeselected() = 0;
	virtual void OnEndRestoreState() = 0;

	virtual int GetSynchRequiredType() = 0;

	virtual void RedoAction(void* data) = 0;
	virtual void Destroy() = 0;
};