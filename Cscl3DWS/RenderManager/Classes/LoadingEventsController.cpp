
#include "StdAfx.h"
#include "LoadingEventsController.h"

#define KEY_ID_LOGIN_SUCCESS			0
#define KEY_ID_WINDOW_RESIZED			1
#define KEY_ID_NEW_3D_FRAME				2

CLoadingEventsController::CLoadingEventsController()
{
	m_windowHandle = FindWindowA(NULL, "Loading and performance checker");
}

void CLoadingEventsController::OnLoginSuccess()
{
	SendKeyMessage(KEY_ID_LOGIN_SUCCESS);
}

void CLoadingEventsController::OnWindowResized()
{
	SendKeyMessage(KEY_ID_WINDOW_RESIZED);
}

void CLoadingEventsController::OnNew3DFrame()
{
	SendKeyMessage(KEY_ID_NEW_3D_FRAME);
}

void CLoadingEventsController::SendKeyMessage(int key)
{
	if (m_windowHandle)
	{
		SendMessage(m_windowHandle, WM_KEYDOWN, key, 0);
	}
}

CLoadingEventsController::~CLoadingEventsController()
{

}