#pragma once
#include "../VirtualRoomSettings/SingletonTemplate.h"
#include "EventSource.h"


struct MouseEvent
{
	WPARAM wParam;
	LPARAM lParam;
};

struct IHookMouseEvents
{
	virtual void OnGlobalMouseEvent(MouseEvent aMouseEvent) = 0;
};

class CHookMouse: public Singleton<CHookMouse>, 
				  public CEventSource<IHookMouseEvents>
{
protected:
	CHookMouse(void);
	friend class Singleton<CHookMouse>;

public:
	void NotifyLister();
	bool StartHookMouse();
	bool StopHookMouse();
	HHOOK hhMouse, hhKeyboard;
	static LRESULT CALLBACK HookMouse(int, WPARAM, LPARAM);

	LPARAM GetLastMouseLParam();
	void SetLastMouseLParam(LPARAM lParam);

	bool GetBtnPressed();
	void SetBtnPressed(bool anIsPressed);

private:
	LPARAM m_lastMouseMoveLParam;
	bool m_btnPressed;
};
