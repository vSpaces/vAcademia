// SimpleDialog.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "classes/BasePopupDialog.h"
#include "CrashHandler.h"

CAppModule _AppModule;

int Run(LPSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_AppModule.AddMessageLoop(&theLoop);

	CBasePopupDialog dlgMain;
	//dlgMain.Create(NULL);

	/*if(dlgMain.Create(NULL) == NULL)
	{
	ATLTRACE(_T("Main dialog creation failed!\n"));
	int error = GetLastError();
	return 0;
	}*/

	nCmdShow = SW_SHOW;

	//dlgMain.ShowWindow( nCmdShow );
	dlgMain.DoModal( );

	int nRet = theLoop.Run();

	_AppModule.RemoveMessageLoop();
	return nRet;
}

CCrashHandler ch;

int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPSTR     lpCmdLine,
					 int       nCmdShow)
{
	ch.SetProcessExceptionHandlers();
	ch.SetThreadExceptionHandlers();
	HRESULT hRes = ::CoInitialize(NULL);
	// If you are running on NT 4.0 or higher you can use the following call instead to 
	// make the EXE free threaded. This means that calls come in on a random RPC thread.
	//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _AppModule.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpCmdLine, nCmdShow);

	_AppModule.Term();
	::CoUninitialize();

	return nRet;
}