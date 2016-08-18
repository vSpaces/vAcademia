// VirtualRoomSettings.cpp : Implementation of WinMain

#include "stdafx.h"
#include "resource.h"
#include "VirtualRoomSettings.h"
#include "VirtualRoomSettingsMain.h"

CAppModule _AppModule;

//
int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPSTR     lpCmdLine,
					 int       nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
	// If you are running on NT 4.0 or higher you can use the following call instead to 
	// make the EXE free threaded. This means that calls come in on a random RPC thread.
	//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _AppModule.Init(NULL, hInstance);

	CMessageLoop theLoop;
	_AppModule.AddMessageLoop(&theLoop);
	CVirtualRoomSettingsMain dlgMain;
	dlgMain.DoModal( );

	int nRet = theLoop.Run();

	_AppModule.RemoveMessageLoop();

	_AppModule.Term();
	::CoUninitialize();

	return 0;
}

