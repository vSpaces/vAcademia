
#pragma once

class IMouseController;

class IMouseListener
{
public:
	virtual void OnLMouseDown(float x, float y) = 0;
	virtual void OnLMouseUp(float x, float y) = 0;

	virtual void OnRMouseDown(float x, float y) = 0;
	virtual void OnRMouseUp(float x, float y) = 0;

	virtual void OnMouseMove(float x, float y) = 0;
	virtual void OnChar(int key, int scancode) = 0;
	virtual void OnKeyUp(int key) = 0;

	virtual void OnLMouseDblClick(float x, float y) = 0;
	virtual void OnRMouseDblClick(float x, float y) = 0;

	virtual bool IsNeedMouseEvents() = 0;

	virtual void SetMouseController(IMouseController* ctrl) = 0;
};