
#pragma once

#include "CommonRenderManagerHeader.h"

#include <IMouseListener.h>
#include "IMouseController.h"

class ISimpleEventsListener
{
public:
	virtual void OnLMouseDown(const int x, const int y) = 0;
	virtual void OnLMouseUp(const int x, const int y) = 0;
	virtual void OnMouseMove(const int x, const int y) = 0;
	virtual void OnKeyDown(const unsigned char keycode, const unsigned char scancode, const bool isAlt, const bool isCtrl, const bool isShift) = 0;
	virtual void OnKeyUp(const unsigned char keycode, const bool isAlt, const bool isCtrl, const bool isShift) = 0;
	virtual void OnMouseWheel(const int delta) = 0;
	virtual bool IsActive() = 0;
};

class CMouseController : public IMouseController
{
public:
	CMouseController();
	~CMouseController();

	void AddMouseListener(ISimpleEventsListener* mouseListener);
	void DeleteMouseListener(ISimpleEventsListener* mouseListener);

	void OnLMouseDown(const int x, const int y);
	void OnLMouseUp(const int x, const int y);

	void OnRMouseDown(const int x, const int y);
	void OnRMouseUp(const int x, const int y);

	void OnMouseMove(const int x, const int y);

	void OnKeyDown(const int key, const int scancode, const bool isAlt, const bool isCtrl, const bool isShift);
	void OnKeyUp(const int key, const bool isAlt, const bool isCtrl, const bool isShift);

	void OnLMouseDblClick(const int x, const int y);
	void OnRMouseDblClick(const int x, const int y);

	void OnMouseWheel(const int delta);

	void UpdateMouseEvent(IMouseListener* mouseListener, float x, float y, bool painting);

	bool IsKeyboardInputNeeded();

	void HandleMouse(bool isHandled);

	void OnNewFrame();
	void UpdateMouseInfoIfNeeded();

	void EnableMouseOptimization();
	void DisableMouseOptimization();

	void RemoveMouseListenerFromLastUsed(IMouseListener* mouseListener);

	void DisableKeyboardInput();

private:
	void UpdateMouseEventForPaint(IMouseListener* mouseListener, float x, float y);
	void UpdateMouseEventForSharing(IMouseListener* mouseListener, float x, float y);
	void NeedUnderMouseCheck();

	void SetLastMouseListener(IMouseListener* mouseListener);
	void SetLastLastMouseListener(IMouseListener* mouseListener);

	bool m_isLMouseDown;
	bool m_isRMouseDown;
	bool m_isMouseMove;

	bool m_isMouseHandled;

	bool m_isLMouseDownForLastListener;
	bool m_isRMouseDownForLastListener;

	float m_lastX, m_lastY;

	bool m_isOptimizationExists;
	int m_lastCheckDeltaTime;

	bool m_isMouseHandledAlready;

	MP_VECTOR<ISimpleEventsListener *> m_mouseListeners;

	IMouseListener* m_lastMouseListener;
	IMouseListener* m_lastLastMouseListener;
};
