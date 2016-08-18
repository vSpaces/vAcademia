
#pragma once

class IMouseListener;

class IMouseController
{
public:
	virtual void OnLMouseDown(const int x, const int y) = 0;
	virtual void OnLMouseUp(const int x, const int y) = 0;

	virtual void OnRMouseDown(const int x, const int y) = 0;
	virtual void OnRMouseUp(const int x, const int y) = 0;

	virtual void OnMouseMove(const int x, const int y) = 0;

	virtual void OnKeyDown(const int key, const int scancode, const bool isAlt, const bool isCtrl, const bool isShift) = 0;
    virtual void OnKeyUp(const int key, const bool isAlt, const bool isCtrl, const bool isShift) = 0;

	virtual void OnLMouseDblClick(const int x, const int y) = 0;
	virtual void OnRMouseDblClick(const int x, const int y) = 0;

	virtual void OnMouseWheel(const int delta) = 0;

	virtual bool IsKeyboardInputNeeded() = 0;

	virtual void RemoveMouseListenerFromLastUsed(IMouseListener* listener) = 0;
};