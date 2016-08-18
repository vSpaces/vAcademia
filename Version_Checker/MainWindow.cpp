#include "stdafx.h"
#include "MainWindow.h"
#include <map>

LRESULT CALLBACK WndProc(const HWND hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam);

map<HWND, CMainWindow*> g_windows;

CMainWindow:: CMainWindow(const HINSTANCE &hInstance, const int &nCmdShow)
{
	hBrush = NULL;
	m_hInst = hInstance;
	m_nCmdShow = nCmdShow;
	LoadString(hInstance, IDS_APP_TITLE, m_szTitle, MAX_LOAD_STRING);
	LoadString(hInstance, IDC_VERSIONCHECKER, m_szWindowClass, MAX_LOAD_STRING);
	MyRegisterClass();
	m_isCorrectInstance = InitInstance();

	m_hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VERSIONCHECKER));	
}
CMainWindow:: ~CMainWindow() 
{
	DeleteObject(hBrush);
}

void CMainWindow:: DoMessageLoop()
{
	while (GetMessage(&m_msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(m_msg.hwnd, m_hAccelTable, &m_msg))
		{
			TranslateMessage(&m_msg);
			DispatchMessage(&m_msg);
		}
	}
}

void CMainWindow:: Show(_In_ int nCmdShow)const
{
	ShowWindow(m_mainWindow, nCmdShow);
	UpdateWindow(m_mainWindow);
}

BOOL CMainWindow:: SetPos(const HWND &hWndInsertAfter, const int &X,
						  const int &Y, const int &cx, const int &cy, const UINT &uFlags)
{
	return SetWindowPos(m_mainWindow, NULL, X, Y, cx, cy, uFlags);
}
void CMainWindow:: SetAttributes(const int &nIndex, const LONG_PTR &dwNewLong)
{
	SetWindowLongPtr(m_mainWindow, nIndex, dwNewLong);
}
void CMainWindow:: SetTitle(const wstring &text)
{
	SetWindowTextW(m_mainWindow, text.c_str());
}

BOOL CMainWindow:: IsCorrectInstance()const
{
	return m_isCorrectInstance;
}
HWND CMainWindow:: GetHandle()const
{
	return m_mainWindow;
}

ATOM CMainWindow:: MyRegisterClass()
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= m_hInst;
	wcex.hIcon			= LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_VERSIONCHECKER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	LOGBRUSH logBrush1;
	logBrush1.lbColor =  RGB(16, 85, 188);
	logBrush1.lbStyle = BS_SOLID;
	logBrush1.lbHatch = NULL;
	HBRUSH hBrush = CreateBrushIndirect( &logBrush1);
	wcex.hbrBackground	= hBrush;
	//wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);	
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= m_szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


BOOL CMainWindow:: InitInstance()
{
	/*m_mainWindow = CreateWindowEx(NULL, m_szWindowClass, m_szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, 0, 475, 290, NULL, NULL, m_hInst, NULL);*/
	m_mainWindow = CreateWindowEx(NULL, m_szWindowClass, m_szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, 0, 423, 204, NULL, NULL, m_hInst, NULL);

	if (!m_mainWindow)
	{
		return FALSE;
	}
	g_windows[m_mainWindow] = this;
	return TRUE;
}

LRESULT CALLBACK WndProc(const HWND hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam)
{
	map<HWND, CMainWindow*>::iterator it = g_windows.find(hWnd);
	if(it == g_windows.end())
	{
		switch (message)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}else
	{
		return (*it).second->Processor(hWnd, message, wParam, lParam);
	}
	return 0;
}

HINSTANCE CMainWindow:: GetHinst()const
{
	return m_hInst;
}

LRESULT CMainWindow:: Processor(const HWND &hWnd, const UINT &message, const WPARAM &wParam, const LPARAM &lParam)
{
	return 0;
}