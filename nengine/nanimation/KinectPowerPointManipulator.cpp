
#include "StdAfx.h"
#include "Tlhelp32.h"
#include "PlatformDependent.h"
#include "KinectPowerPointManipulator.h"

CKinectPowerPointManipulator::CKinectPowerPointManipulator():
	m_powerPointProcess(NULL)
{
}

void CKinectPowerPointManipulator::Prepare()
{
	FindPowerPointIfNeeded();

	if (m_powerPointWindow == NULL)
	{
		STARTUPINFOW si;
		ZeroMemory(&si,sizeof(si));
		si.cb = sizeof(si);
		PROCESS_INFORMATION pi;
		ZeroMemory(&pi,sizeof(pi));

		std::wstring path = GetApplicationRootDirectory();
		path += L"vacademia_ppt_support.exe";
	
		BOOL res = CreateProcessW(NULL, (LPWSTR)path.c_str(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL,
			NULL, &si, &pi);		
		if (res)
		{						
			CloseHandle(pi.hThread);
		}

		m_powerPointProcess = pi.hProcess;
	}
}

void CKinectPowerPointManipulator::EndWork()
{
	if (m_powerPointProcess != NULL)
	{
		rtl_TerminateOtherProcess(m_powerPointProcess, 0);		
	}	

	m_powerPointWindow = NULL;
}

void CKinectPowerPointManipulator::OnGestureDetected(int gestureID)
{
	if (gestureID == GESTURE_SWIPE_LEFT)
	{
		NextSlide();
	}
	else if (gestureID == GESTURE_SWIPE_RIGHT)
	{
		PrevSlide();
	}
}

HWND CKinectPowerPointManipulator::GetControlAppWindow()
{
	FindPowerPointIfNeeded();

	return m_powerPointWindow;
}

void CKinectPowerPointManipulator::NextSlide()
{
	FindPowerPointIfNeeded();

	if (m_powerPointWindow != NULL)
	{
		SendMessage(m_powerPointWindow, WM_KEYDOWN, 37, 0);
	}
}

void CKinectPowerPointManipulator::PrevSlide()
{
	FindPowerPointIfNeeded();

	if (m_powerPointWindow != NULL)
	{
		SendMessage(m_powerPointWindow, WM_KEYDOWN, 39, 0);
	}
}

void CKinectPowerPointManipulator::FindPowerPointIfNeeded()
{
	CheckPowerPointIfNeeded();

	m_powerPointWindow = FindWindow(NULL, "vAcademia PPT Support Application");
}

void CKinectPowerPointManipulator::CheckPowerPointIfNeeded()
{
}

CKinectPowerPointManipulator::~CKinectPowerPointManipulator()
{
}