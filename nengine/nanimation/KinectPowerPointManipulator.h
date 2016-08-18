
#pragma once

#include "IGestureHandler.h"

class CKinectPowerPointManipulator : public IGestureHandler
{
public:
	CKinectPowerPointManipulator();
	~CKinectPowerPointManipulator();

	void OnGestureDetected(int gestureID);

	HWND GetControlAppWindow();

	void Prepare();
	void EndWork();

private:
	void NextSlide();
	void PrevSlide();

	void FindPowerPointIfNeeded();

	void CheckPowerPointIfNeeded();

	HWND m_powerPointWindow;
	HANDLE m_powerPointProcess;
};