// VoipMan.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "shlobj.h"
#include "../../Logger/Logger.h"
#include "../../LogWriter/LogWriter.h"

#include "include\VoipManagerPipeEVO.h"
#include "include\ConaitoSDK\VoipManagerConaitoServer.h"
#include <shellapi.h>
//#include "include\ConaitoSDK\VoipManagerConaitoCapture.h"

#define MAX_LOADSTRING 100
// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

//oms::omsContext *pOMSContext = NULL;
std::auto_ptr<oms::omsContext>	pOMSContext;
CVoipManagerPipeEVO	*m_voipManagerPipe;
//CVoipManagerConaito *pVoipMan = NULL;

HWND hWndMainServer;
HWND hWndEdit;
HICON hTrayIconSmall = 0;
CVoipManagerConaitoServer *pVoipMan = NULL;
bool m_bInstalled = false;
unsigned int WM_TRAYICON;
//typedef CEditT<ATL::CWindow>   CEdit;


CEvoSettings				*settings2 = NULL;

VOID CALLBACK TimerProc( HWND hwnd,
						UINT uMsg,
						UINT_PTR idEvent,
						DWORD dwTime
						)
{
	LPVOID clientInstance = pVoipMan->GetClientInstance();
#ifdef USE_CONAITO_SDK_LIB
	int gainLevel = EVO_GetVoiceGainLevel( clientInstance);
#endif
#ifdef USE_MUMBLE_SDK
	int gainLevel = MUMBLE_GetVoiceGainLevel( clientInstance);
#endif
	int gainLevel2 = gainLevel;
	int maxIncrement = 0;
	int maxDecrement = 0;
	int maxGain = 0;
#ifdef USE_CONAITO_SDK_LIB
	EVO_GetAGCSettings( clientInstance, &gainLevel2, &maxIncrement, &maxDecrement, &maxGain);
#endif
#ifdef USE_MUMBLE_SDK
	MUMBLE_GetAGCSettings( clientInstance, &gainLevel2, &maxIncrement, &maxDecrement, &maxGain);
#endif
	int channelID = 0;
	/*Channel channel;
	EVO_GetChannel( clientInstance, channelID, &channel);*/
	/*
	TTCHAR szChannelPath[EVO_STRLEN];
	if( !EVO_GetChannelPath( clientInstance, wParam, szChannelPath))
	szChannelPath[0] = 0;
	*/
	bool b = settings2->GetUseDS()!=0;
	EVO_WRAPPER_SoundSystemType soundSystem = b ? SOUNDSYSTEM_DSOUND:SOUNDSYSTEM_WINMM;
	int inputDeviceID = pVoipMan->GetCurrentInputDeviceID( soundSystem);
	int outputDeviceID = pVoipMan->GetCurrentOutputDeviceID( soundSystem);
	int voiceActivate = pVoipMan->GetVoiceActivated();//EVO_GetVoiceActivated( clientInstance);
#ifdef USE_CONAITO_SDK_LIB
	int inputLevel = EVO_GetCurrentInputLevel( clientInstance);
	bool bDenoising = EVO_IsDenoising( clientInstance);
	int masterVolume = EVO_GetMasterVolume( clientInstance);
	int voiceActivateLevel = EVO_GetVoiceActivationLevel( clientInstance);
	int encoderComplexity = EVO_GetEncoderComplexity( clientInstance); // from 1 to 10
	bool isForward =  EVO_IsForwardingToAll( clientInstance);
#endif
#ifdef USE_MUMBLE_SDK
	int inputLevel = (int)MUMBLE_GetCurrentInputLevel( clientInstance);
	bool bDenoising = MUMBLE_IsDenoising( clientInstance)!=0;
	int masterVolume = MUMBLE_GetMasterVolume( clientInstance);
	int voiceActivateLevel = MUMBLE_GetVoiceActivationLevel( clientInstance);
	int encoderComplexity = MUMBLE_GetEncoderComplexity( clientInstance); // from 1 to 10
	bool isForward =  MUMBLE_IsForwardingToAll( clientInstance)!=0;
#endif
	
	int isLog = pVoipMan->GetLogMode(); 

	CComString tmp;
	CComString str;
	str += "gainLevel: "; tmp.Format("%i",gainLevel); str += tmp;
	str += "\r\n AGCSettings: "; 
	str += "\r\n\t maxIncrement: "; tmp.Format("%i",maxIncrement); str += tmp;
	str += "\r\n\t maxDecrement: "; tmp.Format("%i",maxDecrement); str += tmp;
	str += "\r\n\t maxGain: "; tmp.Format("%i",maxGain); str += tmp;
	str += "\r\n channelPath: ";  tmp = pVoipMan->GetChannelPath(); str += tmp;
	str += "\r\n inputLevel: ";  tmp.Format("%i",inputLevel); str += tmp;
	str += "\r\n bDenoising: ";  tmp.Format("%i",bDenoising); str += tmp;
	str += "\r\n soundSystem is DSOUND: "; tmp.Format("%i", b); str += tmp;
	str += "\r\n inputDeviceID: ";  tmp.Format("%i",inputDeviceID); str += tmp;
	str += "\r\n masterVolume: ";  tmp.Format("%i",masterVolume); str += tmp;
	str += "\r\n outputDeviceID: ";  tmp.Format("%i",outputDeviceID); str += tmp;
	str += "\r\n voiceActivate: ";  tmp.Format("%i",voiceActivate); str += tmp;
	str += "\r\n voiceActivateLevel: ";  tmp.Format("%i",voiceActivateLevel); str += tmp;
	str += "\r\n encoderComplexity: ";  tmp.Format("%i",encoderComplexity); str += tmp;
	str += "\r\n isForward: ";  tmp.Format("%i",isForward); str += tmp;
	str += "\r\n isLog: ";  tmp.Format("%i",isLog); str += tmp;
	::SetWindowText( hWndEdit, str.GetBuffer());
}


std::wstring GetApplicationDataDirectory()
{
	wchar_t tmp[MAX_PATH];
	SHGetSpecialFolderPathW(NULL, tmp, CSIDL_APPDATA, true);
	return tmp;
}

#include "CrashHandler.h"

CCrashHandler ch;

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	ch.SetProcessExceptionHandlers();
	ch.SetThreadExceptionHandlers();
	pOMSContext = std::auto_ptr<oms::omsContext>(new oms::omsContext);
	MP_NEW_V( settings2, CEvoSettings, pOMSContext.get());
	oms::omsContext *pContext = pOMSContext.get();	
	CLogWriter *m_pLogWriter = MP_NEW( CLogWriter);
//	CLogWriter *m_pLogWriter = NULL;

	std::string sNameDir = "VacademiaEVoip";	
	if (m_pLogWriter)
		m_pLogWriter->SetApplicationName( sNameDir);
	pContext->mpLogWriter = m_pLogWriter;
	if (pContext->mpLogWriter)
		pContext->mpLogWriter->SetApplicationDataDirectory( GetApplicationDataDirectory().c_str());

	if( pContext->mpLogWriter != NULL)
			pContext->mpLogWriter->WriteLn("[VOIPSERVER] start");
	if( lpCmdLine == NULL)
	{
		if( pContext->mpLogWriter != NULL)
			pContext->mpLogWriter->WriteLn("[VOIPSERVER] no parameters");
		return 1;
	}
	CComString sCmdLine = lpCmdLine;
//	sCmdLine = "-piperead \\.\pipe\voipmanpipefromEVOIP -pipewrite \\.\pipe\voipmanpipefromEVOIP";
	if( sCmdLine.GetLength() == 0)
	{
		if( pContext->mpLogWriter != NULL)
			pContext->mpLogWriter->WriteLn("[VOIPSERVER] no parameters");
		return 1;
	}
	sCmdLine.MakeLower();
	int posR = sCmdLine.Find("-piperead");	
	int posW = sCmdLine.Find("-pipewrite");
	if( posR == -1 || posW == -1)
	{
		if( pContext->mpLogWriter != NULL)
			pContext->mpLogWriter->WriteLn("[VOIPSERVER] parameters invalid");
		return 1;
	}
	//MessageBox( NULL, sCmdLine.GetBuffer(), "111", MB_OK | MB_ICONINFORMATION);	
	CComString sReadPipe = sCmdLine.Right( sCmdLine.GetLength() -  posR - 9);
	int pos = sReadPipe.Find("-");
	if( pos > -1)
		sReadPipe = sReadPipe.Left( pos);
	sReadPipe.Replace("\"", "");
	sReadPipe.Replace("/", "\\");
	sReadPipe.TrimLeft(); sReadPipe.TrimRight();
	CComString sWritePipe = sCmdLine.Right( sCmdLine.GetLength() -  posW - 10);
	pos = sWritePipe.Find("-");
	if( pos > -1)
		sWritePipe = sWritePipe.Left( pos);
	sWritePipe.Replace("\"", "");
	sWritePipe.Replace("/", "\\");
	sWritePipe.TrimLeft(); sWritePipe.TrimRight();
	//CComString sPipeReadFromEVOIP = "\\\\.\\pipe\\voipmanpipefromEVOIP";
	MSG msg;
	HACCEL hAccelTable;	

	//CTrayController		trayController;
	//trayController.Start();
	
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_VOIPMANSERVER);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_VOIPMANSERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	//nCmdShow = SW_SHOWNORMAL;
	nCmdShow = SW_HIDE;

	if (!InitInstance (hInstance, nCmdShow)) 
	{
		if( pContext->mpLogWriter != NULL)
			pContext->mpLogWriter->WriteLn("[VOIPSERVER] InitInstance failed");
		return FALSE;
	}
	
	
	//int nIDEvent = SetTimer( hWndMainServer, NULL, 1000, TimerProc);
	int ff = 0;

	CreateVoipManager( pContext);
	MP_NEW_V4( m_voipManagerPipe, CVoipManagerPipeEVO, pContext, pVoipMan, sWritePipe.GetBuffer(), sReadPipe.GetBuffer());
	m_voipManagerPipe->SetHWND( hWndMainServer);
	pVoipMan->Initialize( true);
	m_voipManagerPipe->Run( true);
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}		
	}

	if( pContext->mpLogWriter != NULL)
		pContext->mpLogWriter->WriteLn("[VOIPSERVER] destroyed window");

	// Main message loop:
	
	//KillTimer( hWndMainServer, nIDEvent);
	MP_DELETE( settings2);
	MP_DELETE( m_voipManagerPipe);	
	DestroyVoipManager( pContext);
	pContext->mpLogWriter = NULL;
	if (m_pLogWriter)
		MP_DELETE( m_pLogWriter);

	/*if (m_bInstalled)	
	{
		NOTIFYICONDATA nid;
		nid.uFlags = 0;
		Shell_NotifyIcon(NIM_DELETE, &nid);
	}*/
	return (int) msg.wParam;
}



ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_VOIPMANSERVER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_VOIPMANSERVER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hWndMainServer;

	hInst = hInstance; // Store instance handle in our global variable

	hWndMainServer = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION,
		200, 100, 370, 370, NULL, NULL, hInstance, NULL);

	if (!hWndMainServer)
	{
		return FALSE;
	}

	hWndEdit = CreateWindow("EDIT", szTitle, WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | WS_VSCROLL 
		| WS_HSCROLL, 10, 10, 350, 325, hWndMainServer, NULL, hInstance, NULL);

	//::SetWindowText( hWndEdit, "dddddd");

	ShowWindow(hWndMainServer, nCmdShow);
	UpdateWindow(hWndMainServer);

	/*if( hTrayIconSmall == 0)
	{
		// Load a small icon to use for the system tray
		hTrayIconSmall = (HICON)::LoadImage( hInstance,
			MAKEINTRESOURCE(IDI_VOIPMANSERVER), 
			IMAGE_ICON,
			::GetSystemMetrics(SM_CXSMICON),
			::GetSystemMetrics(SM_CYSMICON),
			LR_DEFAULTCOLOR);
	}	
	WM_TRAYICON = ::RegisterWindowMessage(_T("WM_TRAYICON"));	
	NOTIFYICONDATA nid;
	nid.hWnd = hWndMainServer;
	nid.uID = IDR_POPUPICON;
	nid.hIcon = hTrayIconSmall;
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	nid.uCallbackMessage = WM_TRAYICON;
	_tcscpy(nid.szTip, "VoipTrayIconWindow");
	m_bInstalled = Shell_NotifyIcon(NIM_ADD, &nid) ? true : false;
	//InstallIcon(_T("WTLTrayIconWindow"), hTrayIconSmall, IDR_POPUP);*/

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	/*if( message == WM_TRAYICON)
	{
		switch(lParam)
		{
			//case WM_LBUTTONDBLCLK:		
			case WM_LBUTTONUP:
				{
					ShowWindow( hWndMainServer, SW_SHOWNORMAL);
					break;
				}							
		}
		return 0;
	}*/
	switch (message) 
	{
	/*case WM_CLOSE:
	//case WS_MINIMIZE:
		{
			//ShowWindow(  hWndMainServer, SW_HIDE);
			int ff = 0;
		}
		
		break;*/
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 		
		// Parse the menu selections:		
		switch (wmId)
		{		
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_TIMER:
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
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

namespace voip
{
	VOIPMAN_API omsresult CreateVoipManager( oms::omsContext* aContext)
	{
		MP_NEW_V( pVoipMan, CVoipManagerConaitoServer, aContext);	
		return OMS_OK;
	}

	VOIPMAN_API void DestroyVoipManager( oms::omsContext* aContext)
	{		
		if(pVoipMan != NULL)
		{
			pVoipMan->Release();
			pVoipMan = NULL;
		}
	}
}