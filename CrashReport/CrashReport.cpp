// CrashReport.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "CrashReport.h"
#include "io.h"
#include "../VacademiareportLib/func.h"
#include "ReportMan.h"
#include "DialogError.h"
#include "WndError.h"
#include "DumpParamsGetter.h"
#include <Comman.h>
#include <ServiceMan.h>
#include <filesys.h>
#include "CommandLineParams.h"

using namespace service;
using namespace cm;

//////////////////////////////////////////////////////////////////////////

#define MAX_LOADSTRING 100

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

//////////////////////////////////////////////////////////////////////////

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
BOOL				SendSignal2Launcher( int processId, unsigned int msgCode, unsigned int message);

HWND hMainWnd;
CDumpParamsGetter dumpParamsGetter;


HMODULE hComModule = NULL;
oms::omsresult g_res = 0;
oms::omsContext* context = NULL;
#define	COMMAN_MANAGER_DLL	L"ComMan.dll"
IServiceWorldManager *pServiceWorldManager = NULL;

HMODULE LoadDLL( LPCWSTR alpcRelPath, LPCWSTR alpcName)
{
	CWComString sModuleDirectory;
	GetModuleFileNameW( NULL, sModuleDirectory.GetBufferSetLength(MAX_PATH), MAX_PATH);

	CWComString sCurrentDirectory;
	GetCurrentDirectoryW( MAX_PATH, sCurrentDirectory.GetBufferSetLength(MAX_PATH));

	SECFileSystem fs;
	SetCurrentDirectoryW( fs.GetPath(sModuleDirectory) + alpcRelPath);
	HMODULE handle = ::LoadLibraryW( alpcName);
	SetCurrentDirectoryW( sCurrentDirectory);
	return handle;
}

BOOL ServiceManInit()
{
	context = new oms::omsContext();
	WriteLog__( "ServiceManInit\n");
	if ( !hComModule)
		hComModule = ::LoadDLL( L"", COMMAN_MANAGER_DLL);

	if ( hComModule)
	{
		omsresult (*lpCreateComManager)( oms::omsContext* context);
		(FARPROC&)lpCreateComManager = (FARPROC) GetProcAddress( hComModule, "CreateComManager");
		if ( lpCreateComManager)
			lpCreateComManager( context);
		if ( !context->mpComMan)
		{
			WriteLog__( "ServiceManInit Load ComMan is failed\n");
			FreeLibrary( hComModule);
			hComModule = NULL;
			return FALSE;
		}
	}
	else
	{
		WriteLog__( "ServiceManInit Init ComMan is failed\n");
		return FALSE;
	}

	WriteLog__( "ServiceManInit Init ComMan is succ\n");

	WCHAR lpcIniFullPathName[ 4 * MAX_PATH];

	lpcIniFullPathName[0] = L'\0';

	// Getting execute full name
	WCHAR lpcStrBuffer[MAX_PATH*2+2];	lpcStrBuffer[0] = 0;
	::GetModuleFileNameW( NULL, lpcStrBuffer, MAX_PATH);

	WriteLog__( "ServiceManInit lpcStrBuffer = '");
	WriteLog__( lpcStrBuffer);
	WriteLog__( "'\n");
	if( lpcStrBuffer[0] == 0)
	{
		WriteLog__( "ServiceManInit error lpcStrBuffer[0] == 0");
		return FALSE;
	}

	SECFileSystem fs;
	CWComString sExePath = fs.GetPath( lpcStrBuffer, TRUE);
	CWComString sExecutableFileName = fs.GetFileName( lpcStrBuffer);
	CWComString sIniFilePath;

	sIniFilePath = fs.AppendWildcard( sExePath, L"player.ini");

	wcscpy(lpcIniFullPathName,sIniFilePath.GetBuffer());

	WriteLog__( "ServiceManInit lpcIniFullPathName = '");
	WriteLog__( lpcIniFullPathName);
	WriteLog__( "'\n");

	g_res = context->mpComMan->PutPathIni( lpcIniFullPathName);		

	g_res = CreateServiceWorldManager( context);
	CComString s;
	s.Format( "ServiceManInit:: g_res = '%d'\n", g_res);	
	WriteLog__( s.GetBuffer());	
	pServiceWorldManager = context->mpServiceWorldMan;
	if ( pServiceWorldManager == NULL)
		return FALSE;

	//pServiceWorldManager->Connect();
	s.Format( "ServiceManInit::pServiceWorldManager->Connect\n");
	WriteLog__( s.GetBuffer());	
	return TRUE;
}

void ServiceManDestroy()
{
	if( !context)
		return;

	DestroyServiceManager( context);	

	if( !hComModule)
		return;

	void (*lpDestroyComManager)( oms::omsContext* context);
	(FARPROC&)lpDestroyComManager = (FARPROC)GetProcAddress( hComModule, "DestroyComManager");
	if( lpDestroyComManager)
		lpDestroyComManager( context);
	FreeLibrary( hComModule);
	delete context;
}

#include "ProcessFinder.h"

void CloseProcesses( LPCSTR aExeName)
{
	std::vector<unsigned int> processIDs;
	CProcessWindowFinder processWindowFinder;
	processWindowFinder.GetPlayerProcesses( aExeName, processIDs);
	for(int i=0;i<processIDs.size();i++)
	{		
		CComString s = "application: "; s += aExeName;
		s += " is run at pid = ";
		char ff[10];
		_itoa(processIDs[i], ff, 10);
		s += ff;
		WriteLog__( s.GetBuffer());
	}
}

BOOL RunCrash( HINSTANCE hInstance, LPTSTR lpCmdLine)
{
	MSG msg;
	HACCEL hAccelTable;
	BOOL bPlayerCrash = TRUE;	

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_CRASHREPORT);

	if ( __argc < 2)
	{
		WriteLog__( "Arguments is less 2");
		CloseLog__();
		return FALSE;
	}

	CCommandLineParams gcmParams;
	gcmParams.SetCommandLine( lpCmdLine);

	CComString szProjectName = "player.exe";
	CComString szCrashType = APP_ERROR;
	BOOL bAppNotRestarted = FALSE;
	CComString szUserName = "";
	CComString szPassword = "";
	CComString szIsEncodingPassword = "1";
	CWComString szRoot = L"";
	BOOL bSendCrashShow = FALSE;

	if ( gcmParams.AppNameIsSet())
	{
		szProjectName = gcmParams.GetAppName().c_str();
	}

	if ( gcmParams.CrashTypeIsSet())
	{
		szCrashType = gcmParams.GetCrashType().c_str();
	}

	if ( szCrashType.CompareNoCase( APP_HANG) != 0 && szCrashType.CompareNoCase( APP_ERROR) != 0
		&& szCrashType.CompareNoCase( ASSERT_ERROR) != 0 && szCrashType.CompareNoCase( APP_LAUNCHER_ERROR)
		&& szCrashType.CompareNoCase( APP_PREV_CRASH) != 0)
	{
		szCrashType = APP_ERROR;
	}

	if ( szCrashType.CompareNoCase( APP_PREV_CRASH) == 0
		|| szCrashType.CompareNoCase( APP_LAUNCHER_ERROR) == 0)
	{
		bPlayerCrash = FALSE;
	}

	if ( gcmParams.AllowAppRestartIsSet())
	{
		bAppNotRestarted = gcmParams.GetAllowAppRestart() == 0 ? TRUE : FALSE;
	}

	if ( gcmParams.UserNameIsSet())
	{
		szUserName = gcmParams.GetUserName().c_str();
	}

	if ( gcmParams.PasswordIsSet())
	{
		szPassword = gcmParams.GetPassword().c_str();
	}

	if ( gcmParams.PasswordEncodingModeIsSet())
	{
		szIsEncodingPassword.Format( "%d", gcmParams.GetPasswordEncodingMode());
	}

	if ( gcmParams.RootIsSet())
	{
		szRoot = gcmParams.GetRoot().c_str();
	}

	if ( gcmParams.SendCrashShowIsSet())
	{
		bSendCrashShow = gcmParams.GetSendCrashShow() == 0 ? FALSE : TRUE;
	}

/*
	над кодом, который ниже, не было рефакторинга
*/

	CReportMan *reportMan = new CReportMan( FALSE);	
	
	reportMan->init( szProjectName.GetBuffer(), szCrashType.GetBuffer(), szRoot.GetBuffer());

	ServiceManInit();
	reportMan->setServiceWorldManager( pServiceWorldManager);
	//reportMan->setProtocol( CReportMan::SOCKET_QUERY);

	TCHAR *szScratch = new TCHAR[ 4 * MAX_PATH];
	//sprintf( szScratch, "Something bad happened in your program, would you like to send a diagnostic file '%s'?", lpFileName);

	BOOL bNoError = szCrashType.CompareNoCase( APP_ERROR) != 0 && szCrashType.CompareNoCase( APP_HANG) != 0;
	//CDialogError dlgError;
	CWndError wndError;
	//dlgError.SetAppLanguage( reportMan->getLanguage());
	wndError.SetAppLanguage( reportMan->getLanguage());	
	wndError.SetAppRestartCheckVisible( !bAppNotRestarted);
	
	// ask the user if they want to save a dump file
	//if ( bNoError || ::MessageBox( NULL, szScratch, szProjectName, MB_YESNO ) == IDYES)
	if ( bPlayerCrash)//bNoError || dlgError.DoModal( hMainWnd) == IDOK)
	{	
		WriteLog__( "SetProgress \n");		
		wndError.Create( hMainWnd, reportMan);		
		wndError.UpdateWindow();
		wndError.SetProgress( 5);
		if ( bSendCrashShow)
		{
			wndError.ShowWindow( SW_SHOW);
		}
		
		WriteLog__( "sendAsynch \n");

		reportMan->sendAsynch( &wndError);
		DWORD	dwFrameTime = 0;
		BOOL	bExit = FALSE;
		while (true) 
		{
			while ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if ( !TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);					
				}
				if ( (!wndError.IsWindowVisible() && reportMan->isEnding())
						|| ( reportMan->isEnding() && reportMan->getStatusCode() != 200)
						|| ( msg.message == WM_QUIT || msg.message == WM_CLOSE) && (msg.hwnd == hMainWnd || msg.hwnd == 0))
				{
					bExit = true;
					break;
				}			
			}

			Sleep( 50);

			if( bExit)	
			{
				break;
			}
		}

		CloseLog__();
		OpenLog__(NULL, FALSE, TRUE);

		if ( reportMan->getStatusCode() == 200)		
		{
			TCHAR s[MAX_PATH];
			sprintf( s, "Transfer of file is successful!!! code = %d\n", reportMan->getStatusCode());
			WriteLog__( s);

			if ( reportMan->getLanguage() == ENG)
			{
				sprintf( szScratch, "Error data were successfully sent to our server.");
				if ( !bNoError && wndError.IsAppRestart())
					lstrcat(szScratch, 	". \"Virtual academia\" is about to start.");
				//::MessageBox( NULL, szScratch, "Virtual academia", MB_OK );
			}
			else
			{
				sprintf( szScratch, "Данные об ошибке успешно отправлены на сервер");
				if ( !bNoError && wndError.IsAppRestart())
					lstrcat(szScratch, 	". Сейчас приложение 'Виртуальная академия' запуститься!");
				//::MessageBox( NULL, szScratch, "Виртуальная академия", MB_OK );
			}
		}
		else
		{
			//wndError.SetProgress( 0);
			TCHAR s[MAX_PATH];
			if ( reportMan->getLanguage() == ENG)
			{
				sprintf( s, "An error has occurred. Error code = %d", reportMan->getStatusCode());
				//::MessageBox( NULL, s, "Virtual academia", MB_OK );
			}
			else
			{
				sprintf( s, "Произошла ошибка, код ошибки = %d", reportMan->getStatusCode());
				//::MessageBox( NULL, s, "Виртуальная академия", MB_OK );
			}
			//sprintf( s, "Произошла ошибка креш-дамп не отправлен. Код ошибки = %d\n", reportMan->getStatusCode());
			WriteLog__( s);
			WriteLog__( "\n");
		}
		WriteLog__( "ending \n");
	}
	else
	{
		reportMan->runAsynch( pServiceWorldManager);
		while( !reportMan->isEnding())
		{
			Sleep( 500);
		}			
		WriteLog__( "ending \n");
	}

	ServiceManDestroy();

	std::string s = "Restard need is (";
	s += !bNoError == true ? "bNoError == false " : "bNoError == true";
	s += bPlayerCrash == true ? "bPlayerCrash == true " : "bPlayerCrash == false";
	s += wndError.IsAppRestart() == true ? "wndError.IsAppRestart() == true " : "wndError.IsAppRestart() == false";
	s += "\n";
	WriteLog__( (TCHAR*)s.c_str());

	if ( !bNoError && bPlayerCrash && wndError.IsAppRestart())
	{
		if( szCrashType == APP_HANG || szCrashType == APP_UNKNOWN)
		{
			CloseProcesses( "vacademia.exe");
			CloseProcesses( "player.exe");			
		}
		WCHAR sAppName[ 2 * MAX_PATH + 1] = {L'\0'};
		//ZeroMemory(sAppName, sizeof(sAppName));
		if (GetModuleFileNameW(0, sAppName, _countof(sAppName)-2) <= 0)
			lstrcpyW(sAppName, L"Unknown");
		WCHAR *lastperiod = lstrrchrW( sAppName, L'\\');
		if (lastperiod)
		{
			lastperiod[0] = 0;
		}

		wcscat( sAppName, L"\\vacademia.exe");		

		WCHAR szCommandLine[ 8 * MAX_PATH];
		lstrcpyW(szCommandLine, sAppName);
		USES_CONVERSION;
		if ( szUserName.GetLength() > 2 && szPassword.GetLength() > 2)
		{
			lstrcatW(szCommandLine, L" -user ");
			lstrcatW(szCommandLine, A2W(szUserName.GetBuffer()));
			lstrcatW(szCommandLine, L" -password \"");
			lstrcatW(szCommandLine, A2W(szPassword.GetBuffer()));	
			lstrcatW(szCommandLine, L"\"");

			if ( szIsEncodingPassword.GetLength() > 0)
			{
				lstrcatW(szCommandLine, L" -encoding ");
				lstrcatW(szCommandLine, A2W(szIsEncodingPassword.GetBuffer()));
			}			
		}
		if ( szRoot.GetLength() > 5)
		{
			lstrcatW(szCommandLine, L" ");
			lstrcatW(szCommandLine, szRoot.GetBuffer());	
		}
		WriteLog__( szCommandLine);	

		STARTUPINFOW si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		si.dwFlags = STARTF_FORCEONFEEDBACK;
		si.wShowWindow = SW_SHOW;

		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));
		HANDLE	hCrashReport;
		if (CreateProcessW(
			NULL,					// name of executable module
			szCommandLine,			// command line string
			NULL,					// process attributes
			NULL,					// thread attributes
			FALSE,					// handle inheritance option
			0,						// creation flags CREATE_NO_WINDOW | NORMAL_PRIORITY_CLASS
			NULL,					// new environment block
			NULL,					// current directory name
			&si,					// startup information
			&pi))					// process information
		{

		}
		else
		{
			if ( reportMan->getLanguage() == ENG)
				sprintf( szScratch, "An error has occurred: application '%s' can’t start up. Please restart the application. Sorry for inconvenience. ", sAppName);
			else
				sprintf( szScratch, "Произошла ошибка: приложение '%s' не запустилось. Попробуйте сами запустить. Простите за неудобства.", sAppName);
			::MessageBox( NULL, szScratch, (reportMan->getLanguage() == ENG ? "Virtual Academia" : "Виртуальная академия"), MB_OK | MB_TOPMOST);
		}
	}
	else
	{
		#define APP_BY_VREPORT				1024
		SendSignal2Launcher( 0, APP_BY_VREPORT, 0);	
	}
	

	delete reportMan;
	delete szScratch;
	return TRUE;
}

BOOL bSendSignalNeed = TRUE;
HWND hMyVacademiaWnd = 0;

BOOL CALLBACK EnumLWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD dwWinProcID = 0;
	GetWindowThreadProcessId(hwnd, &dwWinProcID);
	if(dwWinProcID == (DWORD)lParam)
	{
		TCHAR   className[MAX_PATH+1];
		if( GetClassName( hwnd, className, MAX_PATH) > 0)
		{
			if( _tcscmp( className, "LAUNCHER") == 0)
			{
				hMyVacademiaWnd = hwnd;
				return FALSE; // нашли нужное окно. Можно не продолжать поиск
			}
		}
	}
	return TRUE;
}

BOOL SendSignal2Launcher( int processId, unsigned int msgCode, unsigned int message)
{
	unsigned int msgCode_ = (msgCode << 16) | message;
	if ( hMyVacademiaWnd == 0 && processId > 0)
	{
		CProcessWindowFinder processWindowFinder;
		EnumWindows(EnumLWindowsProc, (LPARAM)processId);
		//hMyVacademiaWnd = processWindowFinder.GetPlayerWindow( processId);
		if ( hMyVacademiaWnd != 0)
			return PostMessage( hMyVacademiaWnd, WM_USER, GetCurrentProcessId(), msgCode);
	}
	else if ( hMyVacademiaWnd != 0)
	{
		return PostMessage( hMyVacademiaWnd, WM_USER, GetCurrentProcessId(), msgCode);
	}
	else if ( processId <= 0)
	{
		BOOL res = FALSE;
		CProcessWindowFinder processWindowFinder;

		std::vector<unsigned int> processIDs;
		processWindowFinder.GetPlayerProcesses( "vacademia.exe", processIDs);
		std::vector<unsigned int>::iterator it = processIDs.begin(), end = processIDs.end();
		for( ; it != end; it++)
		{
			unsigned int procID = *it;
			//hMyVacademiaWnd = processWindowFinder.GetPlayerWindow( procID);
			EnumWindows(EnumLWindowsProc, (LPARAM)procID);
			if ( hMyVacademiaWnd != 0)
			{
				res = PostMessage( hMyVacademiaWnd, WM_USER, GetCurrentProcessId(), msgCode_);
			}
			else 
			{
				res = FALSE;
			}
		}
		return res;
	}

	return FALSE;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	//ch.SetProcessExceptionHandlers();
	//ch.SetThreadExceptionHandlers();

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CRASHREPORT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if ( !InitInstance(hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	try
	{
		OpenLog__( NULL, TRUE, FALSE);
		RunCrash( hInstance, lpCmdLine);
		CloseLog__();		
	}	
	catch(...)
	{
		CloseLog__();
	}	

	PostQuitMessage( WM_QUIT);

	return 0;//(int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_CRASHREPORT);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_CRASHREPORT;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
   
   if (!hWnd)
   {
      return FALSE;
   }

  // ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   hMainWnd = hWnd;

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}