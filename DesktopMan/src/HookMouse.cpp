#include "StdAfx.h"
#include "..\include\hookmouse.h"

#define MIN_DELTA 3

CHookMouse::CHookMouse(void)
{
	hhMouse = NULL;
	m_btnPressed = false;
}

bool CHookMouse::StartHookMouse()
{
	HMODULE hSelf;

	GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (wchar_t *) &HookMouse, &hSelf);

	hhMouse = SetWindowsHookEx(WH_MOUSE_LL, HookMouse, hSelf, 0);

	if (hhMouse != NULL)
		return true;
	else
		return false;
}

bool CHookMouse::StopHookMouse()
{
	if (hhMouse != NULL)
		UnhookWindowsHookEx(hhMouse);
	return true;
}

LRESULT CALLBACK CHookMouse::HookMouse(int nCode, WPARAM wParam, LPARAM lParam) {
	bool isHandledEvent = false;
	CHookMouse* hookClass = CHookMouse::Instance();
	if (nCode >= 0) {
		switch (wParam) {
			case WM_LBUTTONDOWN:
				hookClass->SetBtnPressed(true);
				isHandledEvent = true;
				break;
			case WM_LBUTTONUP:
				hookClass->SetBtnPressed(false);
				isHandledEvent = true;
				break;
			case WM_RBUTTONDOWN:
				hookClass->SetBtnPressed(true);
				isHandledEvent = true;
				break;
			case WM_RBUTTONUP:
				hookClass->SetBtnPressed(false);
				isHandledEvent = true;
				break;
			case WM_MOUSEMOVE:
				if (hookClass->GetBtnPressed())
					isHandledEvent = true;
				break;
			default:
				break;
		}
	}

	if (isHandledEvent)
	{
		MSLLHOOKSTRUCT* mouseHook = (MSLLHOOKSTRUCT*)lParam;
		if (mouseHook != NULL)
		{
			//дельта для WM_MOUSEMOVE
			if (abs(GET_X_LPARAM(hookClass->GetLastMouseLParam()) - mouseHook->pt.x) > MIN_DELTA 
				|| abs(GET_Y_LPARAM(hookClass->GetLastMouseLParam()) - mouseHook->pt.y) > MIN_DELTA
				|| wParam != WM_MOUSEMOVE)
			{
				LPARAM lMouseParam;
				lMouseParam = (mouseHook->pt.y)*65536 + mouseHook->pt.x;

				MouseEvent mEvent;
				mEvent.wParam = wParam;
				mEvent.lParam = lMouseParam;
				hookClass->SetLastMouseLParam(lMouseParam);

				hookClass->InvokeEx<MouseEvent, &IHookMouseEvents::OnGlobalMouseEvent>(mEvent);
			}
		}
	}
	hookClass->FreeInst();
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

bool CHookMouse::GetBtnPressed()
{
	return m_btnPressed;
}

void CHookMouse::SetBtnPressed(bool anIsPressed)
{
	m_btnPressed = anIsPressed;
}

LPARAM CHookMouse::GetLastMouseLParam()
{
	return m_lastMouseMoveLParam;
}

void CHookMouse::SetLastMouseLParam(LPARAM lParam)
{
	m_lastMouseMoveLParam = lParam;
}