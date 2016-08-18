#pragma once
#include "Resource.h"
#define MAX_LOAD_STRING 100

class CMainWindow
{
public:
	CMainWindow(const HINSTANCE &hInstance, const int &nCmdShow);
	virtual ~CMainWindow();
	void SetTitle(const wstring &text);
	void SetAttributes(const int &nIndex, const LONG_PTR &dwNewLong);
	BOOL SetPos(const HWND &hWndInsertAfter, const int &X,
		const int &Y, const int &cx, const int &cy, const UINT &uFlags);
	void Show(_In_ int nCmdShow)const;
	HWND GetHandle()const;
	HINSTANCE GetHinst()const;
	BOOL IsCorrectInstance()const;
	void DoMessageLoop();
	virtual LRESULT Processor(const HWND &hWnd, const UINT &message, const WPARAM &wParam, const  LPARAM &lParam);
private:
	ATOM MyRegisterClass();
	BOOL InitInstance();

	MSG m_msg;
	HACCEL m_hAccelTable;
	BOOL m_isCorrectInstance;
	HINSTANCE m_hInst;
	int m_nCmdShow;
	HWND m_mainWindow;
	TCHAR m_szTitle[MAX_LOAD_STRING];					// The title bar text
	TCHAR m_szWindowClass[MAX_LOAD_STRING];			// the main window class name
	HBRUSH hBrush;
};