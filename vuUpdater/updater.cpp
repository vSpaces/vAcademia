// updater.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "updater.h"
#include "updaterDlg.h"
#include "mdump.h"

/*#ifdef _DEBUG
#define new DEBUG_NEW
#endif*/

int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPSTR     lpCmdLine,
					 int       nCmdShow)
{
	
	// Initialize global strings
	//LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	//LoadString(hInstance, IDC_OMSPLAYER, szWindowClass, MAX_LOADSTRING);
	/*TCHAR title[30] = _T("VSpaces virtual worlds player"); 
	wcscpy((wchar_t*)&szWindowClass, (wchar_t*)title);
	MyRegisterClass(hInstance);*/


	CUpdaterDlg dlg;
	dlg.ParseCmdLine(lpCmdLine);
	//ParseINI(hInstance);
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	//MessageBox( NULL, "Конец", "Конец", MB_OK | MB_ICONINFORMATION);
	return 0;
}
