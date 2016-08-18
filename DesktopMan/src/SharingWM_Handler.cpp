#include "StdAfx.h"
#include "..\include\sharingwm_handler.h"
#include "..\protocol\RemoteDesktopIn.h"

//#define LOG_MESSAGES

SharingWM_Handler::SharingWM_Handler(void)
{
	m_LButtonDown = false;
	m_RButtonDown = false;
	m_ctrlPressed = false;

	hLastActiveWindow = NULL;

	m_context = NULL;
	m_selector = NULL;
	m_fullControlMode = 0;
}

SharingWM_Handler::~SharingWM_Handler(void)
{
}

void SharingWM_Handler::SetContext(SHARING_CONTEXT* apContext)
{
	m_context = apContext;
}

void SharingWM_Handler::SetSharingSelector(IDesktopSelector* aSelector)
{
	m_selector = aSelector;
}

void SharingWM_Handler::SetRemoteControlMode(int aFullControl)
{
	m_fullControlMode = aFullControl;
}

HWND SharingWM_Handler::FindParentWindow(HWND hChild)
{
	HWND hParent = NULL;
	HWND hResult = NULL;
	hParent = GetParent(hChild);
	if (hParent != NULL)
		hResult = FindParentWindow(hParent);
	if (hResult == NULL)
		return hChild;
	else 
		return hResult;
}

bool SharingWM_Handler::NotifyChildWindow(HWND hChildWindow, LPARAM Param, int currentMessage, RECT mainRect)
{
	bool messageSended = false;
	HWND hCurrentWindow = GetTopWindow(hChildWindow);
	if (hCurrentWindow == NULL)
		return messageSended;
	RECT childWindowRect;
	do 
	{
		if (!IsWindowVisible(hCurrentWindow))
			continue;
		GetWindowRect(hCurrentWindow, &childWindowRect);
		int x = GET_X_LPARAM(Param)+mainRect.left;
		int y = GET_Y_LPARAM(Param)+mainRect.top;
		if (childWindowRect.left <= x && childWindowRect.right >= x && childWindowRect.top <= y && childWindowRect.bottom >= y)
		{	
			bool res = NotifyChildWindow(hCurrentWindow, Param, currentMessage, mainRect);
			if (!res)
			{
				hLastActiveWindow = hCurrentWindow;
				LPARAM newLp = (GET_Y_LPARAM(Param) - (childWindowRect.top - mainRect.top))*65536 + (GET_X_LPARAM(Param) - (childWindowRect.left - mainRect.left));
				if (currentMessage == WM_LBUTTONDOWN || currentMessage == WM_LBUTTONDBLCLK)
				{
					//	ReleaseCapture();
					//	SetCapture(hCurrentWindow);

					LPARAM testParam = currentMessage * 65536 + HTCLIENT;
					PostMessage(hCurrentWindow, WM_MOUSEACTIVATE, (WPARAM)FindParentWindow(hCurrentWindow), testParam);
					PostMessage(hCurrentWindow, WM_SETFOCUS, NULL, NULL);
					PostMessage(hCurrentWindow, WM_SETCURSOR, (WPARAM)hCurrentWindow, testParam);
					PostMessage(hCurrentWindow, currentMessage, MK_LBUTTON, newLp);
					m_LButtonDown = true;
				}
				else if (currentMessage == WM_RBUTTONDOWN )
				{
					PostMessage(hCurrentWindow, currentMessage, MK_RBUTTON, newLp);
					m_RButtonDown = true;
				}
				else if (currentMessage == WM_MOUSEMOVE)
				{
					if (m_LButtonDown)
						PostMessage(hCurrentWindow, currentMessage, MK_LBUTTON, newLp);
					else if (m_RButtonDown)
						PostMessage(hCurrentWindow, currentMessage, MK_RBUTTON, newLp);
					else
					{
						LPARAM testParam = WM_MOUSEMOVE * 65536 + HTCLIENT;
						PostMessage(hCurrentWindow, WM_SETCURSOR, (WPARAM)hCurrentWindow, testParam);
						PostMessage(hCurrentWindow, currentMessage, 0, newLp);
					}
				}
				else
				{
					if (currentMessage == WM_LBUTTONUP)
						m_LButtonDown = false;
					if (currentMessage == WM_RBUTTONUP)
						m_RButtonDown = false;
					PostMessage(hCurrentWindow, currentMessage, 0, newLp);
				}
				messageSended = true;
			}
			else
				messageSended = true;
			break;
		}
	} while ((hCurrentWindow=GetNextWindow(hCurrentWindow, GW_HWNDNEXT)) != NULL);

	return messageSended;
}

void SharingWM_Handler::OnRemoteCommandReceived( BYTE* aData, int aDataSize)
{
	CRemoteDesktopIn packetIn(aData, aDataSize);
	if (!packetIn.Analyse())
	{
		ATLASSERT( FALSE);
		return;
	}
	if (!m_selector)
		return;
	RECT selectorRect = m_selector->GetCurrentSelectingRect();
	HWND hWindow = m_selector->GetCapturedWindow();
	int currentMessage = -1;
	RECT mainRect;
	LPARAM lParam;
	lParam = (packetIn.GetSecondParam())*65536 + (packetIn.GetFirstParam());
	if (hWindow == NULL)
	{
		POINT point;
		point.x = packetIn.GetFirstParam()+selectorRect.left;
		point.y = packetIn.GetSecondParam()+selectorRect.top;
		hWindow = WindowFromPoint(point);
		if (hWindow)
			GetWindowRect(hWindow, &mainRect);

		lParam = (packetIn.GetSecondParam()+selectorRect.top - mainRect.top)*65536 + selectorRect.left + packetIn.GetFirstParam() - mainRect.left;

	}

	if (hWindow != NULL && m_fullControlMode == 0)
	{
		GetWindowRect(hWindow, &mainRect);
		unsigned int scanCode = packetIn.GetSecondParam();

		if (packetIn.GetType() == 1)
		{
#ifdef LOG_MESSAGES
			//не удаетс€ генерить глобально нажати€ клавиш ctrl-c и тп - обрабатывает их система и текущее актиное окно
			//а также их ловит академи€ и отправл€ет по новой
				/*		char buf[100];
				itoa(scanCode,buf,10);

				if (IsSystemKey(scanCode))
				{
					if (packetIn.GetCode() == 0) {
						
						if (m_context->gcontext->mpLogWriter)
						{
							string str = "[SharingMAN] IsSystemKey UP = ";
							str += buf;
							m_context->gcontext->mpLogWriter->WriteLnLPCSTR(str.c_str());
						}

						if (scanCode == VK_CONTROL)
							m_ctrlPressed = false;
						keybd_event(scanCode, GetHardwareScanCode(scanCode), KEYEVENTF_KEYUP, 0);
					}
					if (packetIn.GetCode() == 1) {
						if (m_context->gcontext->mpLogWriter)
						{
							string str = "[SharingMAN] IsSystemKey DOWN = ";
							str += buf;
							m_context->gcontext->mpLogWriter->WriteLnLPCSTR(str.c_str());
						}

						if (scanCode == VK_CONTROL)
							m_ctrlPressed = true;
						keybd_event(scanCode, GetHardwareScanCode(scanCode), 0, 0);
					}
					return;
				}*/
#endif			
			if (packetIn.GetCode() == 0)
			{
#ifdef LOG_MESSAGES
				if (m_context->gcontext->mpLogWriter)
				{
					string str = "[SharingMAN] NON SystemKey UP = ";
					str += buf;
					m_context->gcontext->mpLogWriter->WriteLnLPCSTR(str.c_str());
				} 
#endif
				currentMessage = WM_KEYUP;
				lParam = 0x30000001;
				PostMessage(hLastActiveWindow, currentMessage, 0, lParam);
			}
			if (packetIn.GetCode() == 1)
			{
#ifdef LOG_MESSAGES
				if (m_context->gcontext->mpLogWriter)
				{
					string str = "[SharingMAN] NON SystemKey DOWN = ";
					str += buf;
					m_context->gcontext->mpLogWriter->WriteLnLPCSTR(str.c_str());
				} 
#endif
				currentMessage = WM_KEYDOWN;
				lParam = 0x00000001;
				PostMessage(hLastActiveWindow, currentMessage, packetIn.GetSecondParam(), lParam);
			}
		}
		else
		{
			if (packetIn.GetCode() == 0)
				currentMessage = WM_LBUTTONDOWN;
			else if (packetIn.GetCode() == 1)
				currentMessage = WM_LBUTTONUP;
			else if (packetIn.GetCode() == 2)
				currentMessage = WM_LBUTTONDBLCLK;
			else if (packetIn.GetCode() == 3)
				currentMessage = WM_RBUTTONDOWN;
			else if (packetIn.GetCode() == 4)
				currentMessage = WM_RBUTTONUP;
			else if (packetIn.GetCode() == 5)
				currentMessage = WM_RBUTTONDBLCLK;
			else if (packetIn.GetCode() == 6)
				currentMessage = WM_MOUSEMOVE;
#ifdef LOG_MESSAGES
			char buf[100];
			itoa(currentMessage,buf,10);
			if (m_context->gcontext->mpLogWriter)
			{
				string str = "[SharingMAN] mouse = ";
				str += buf;
				m_context->gcontext->mpLogWriter->WriteLnLPCSTR(str.c_str());
			}
#endif
			if (currentMessage != -1)
			{
				//	AttachThreadInput(GetCurrentThreadId(), GetWindowThreadProcessId(hWindow, NULL), TRUE);
				//	AttachThreadInput(GetCurrentThreadId(), GetWindowThreadProcessId(hWindow, NULL), FALSE);
				if (!NotifyChildWindow(hWindow, lParam, currentMessage, mainRect))
				{
					RECT rcClient, rcWind;
					POINT ptDiff;
					GetClientRect(hWindow, &rcClient);
					GetWindowRect(hWindow, &rcWind);

					ptDiff.x = rcClient.left;
					ptDiff.y = rcClient.top;
					ClientToScreen(hWindow, &ptDiff);
					ptDiff.x = ptDiff.x -  rcWind.left;
					ptDiff.y = ptDiff.y - rcWind.top ;

					hLastActiveWindow = hWindow;
					LPARAM newLp = packetIn.GetSecondParam()*65536 + packetIn.GetFirstParam();
					newLp = (packetIn.GetSecondParam()+selectorRect.top - mainRect.top - ptDiff.y)*65536 + selectorRect.left + packetIn.GetFirstParam() - mainRect.left - ptDiff.x;
					if (currentMessage == WM_LBUTTONDOWN || currentMessage == WM_LBUTTONDBLCLK)
					{
						LPARAM testParam = currentMessage * 65536 + HTCLIENT;
						PostMessage(hWindow, WM_MOUSEACTIVATE, (WPARAM)FindParentWindow(hWindow), testParam);
						PostMessage(hWindow, WM_SETFOCUS, NULL, NULL);
						PostMessage(hWindow, WM_SETCURSOR, (WPARAM)hWindow, testParam);

						PostMessage(hWindow, currentMessage, MK_LBUTTON, newLp);
						m_LButtonDown = true;
					}
					else if (currentMessage == WM_RBUTTONDOWN)
					{
						PostMessage(hWindow, currentMessage, MK_RBUTTON, newLp);
						m_RButtonDown = true;
					}
					else if (currentMessage == WM_MOUSEMOVE)
					{
						if (m_LButtonDown)
							PostMessage(hWindow, currentMessage, MK_LBUTTON, newLp);
						else if (m_RButtonDown)
							PostMessage(hWindow, currentMessage, MK_RBUTTON, newLp);
						else
						{
							LPARAM testParam = WM_MOUSEMOVE * 65536 + HTCLIENT;
							PostMessage(hWindow, WM_SETCURSOR, (WPARAM)hWindow, testParam);
							PostMessage(hWindow, currentMessage, 0, newLp);
						}
					}
					else
					{
						if (currentMessage == WM_LBUTTONUP)
							m_LButtonDown = false;
						if (currentMessage == WM_RBUTTONUP)
							m_RButtonDown = false;
						PostMessage(hWindow, currentMessage, 0, newLp);
					}
				}
			}
		}
	}
	if (m_fullControlMode == 1)
	{
		if (packetIn.GetType() == 1)
		{
			if (packetIn.GetCode() == 0)
				keybd_event(packetIn.GetSecondParam(), 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
			if (packetIn.GetCode() == 1)
				keybd_event(packetIn.GetSecondParam(), 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
		}
		else if (packetIn.GetType() == 0)
		{
			SetCursorPos(packetIn.GetFirstParam()+selectorRect.left, packetIn.GetSecondParam()+selectorRect.top);
			if (packetIn.GetCode() == 0)
				mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
			else if (packetIn.GetCode() == 1)
				mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
			else if (packetIn.GetCode() == 2)
			{
				mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
				mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
				mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
				mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
			}
			else if (packetIn.GetCode() == 3)
				mouse_event(MOUSEEVENTF_RIGHTDOWN,0,0,0,0);
			else if (packetIn.GetCode() == 4)
				mouse_event(MOUSEEVENTF_RIGHTUP,0,0,0,0);
			else if (packetIn.GetCode() == 5)
			{
				mouse_event(MOUSEEVENTF_RIGHTDOWN,0,0,0,0);
				mouse_event(MOUSEEVENTF_RIGHTUP,0,0,0,0);
				mouse_event(MOUSEEVENTF_RIGHTDOWN,0,0,0,0);
				mouse_event(MOUSEEVENTF_RIGHTUP,0,0,0,0);
			}
		}
	}
}

bool SharingWM_Handler::IsSystemKey(unsigned int aScanCode)
{
	bool result = false;
	switch (aScanCode)
	{
	case VK_SHIFT:
		result = true;
		break;
	case VK_CONTROL:
		result = true;
		break;
	case VK_CAPITAL:
		result = true;
		break;
	case 0x43: //c
		if (m_ctrlPressed) //ctrl+c
			result = true;
		break;
	case 0x56: //v
		if (m_ctrlPressed) //ctrl+c
			result = true;
		break;
	case 0x58: //x
		if (m_ctrlPressed) //ctrl+x
			result = true;
		break;
	}

	return result;
}

unsigned char SharingWM_Handler::GetHardwareScanCode(unsigned int aScanCode)
{
	unsigned char hardScanCode = 0;
	switch (aScanCode)
	{
	case VK_SHIFT:
		hardScanCode = 0xaa;
		break;
	case VK_CONTROL:
		hardScanCode = 0x9d;
		break;
	case VK_CAPITAL:
		hardScanCode = 0xba;
		break;
	case 0x43: //c
		hardScanCode = 0xae;
		break;
	case 0x56: //v
		hardScanCode = 0xaf;
		break;
	case 0x58: //x
		hardScanCode = 0xad;
		break;
	}
	return 0;
}