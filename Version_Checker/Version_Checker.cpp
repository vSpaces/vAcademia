// Version Checker.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MainWindow.h"
#include "UpdateDialog.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CUpdateDialog UpdateDialog(hInstance, nCmdShow);
	UpdateDialog.Initialize();
	return 0;
}