// OMSPlayer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MainWindow.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	SetCommandLine(lpCmdLine);
	SetResourceInstance(hInstance);

	DoInitializationAndLoading(0);

	DoMainLoop();

	DoQuit();

	return 0;
}