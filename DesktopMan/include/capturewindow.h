#pragma once

#include "selectors\selectorbase.h"

class CDesktopRectSelector;

class CCaptureWindow :
	public CFrameWindowImpl<CCaptureWindow>,
	public CUpdateUI<CCaptureWindow>,
	public CMessageFilter,
	public ViskoeThread::CThreadImpl<CCaptureWindow>,
	public CSelectorBase
{
	MONITORINFOEX monitorInfo;

public:
	CCaptureWindow(CDesktopRectSelector* apManager);
	virtual ~CCaptureWindow();
	void	ReleaseWindow();
	void	CreateOnMonitor( MONITORINFOEX info);
	void	DrawFrame( LPRECT alpRect, bool leftEdge, bool topEdge, bool rightEdge, bool bottomEdge);
	void	CopyImagePart( HDC hDC, LPRECT lpSrcRect, LPRECT lpDestRect);
	MONITORINFOEX	GetMonitorInfo(){ return monitorInfo;}
	
	// реализация CThreadImpl<nrmPreviewRecorder>
public:
	DWORD Run();

public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)
	BEGIN_UPDATE_UI_MAP(CCaptureWindow)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CCaptureWindow)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		CHAIN_MSG_MAP(CUpdateUI<CCaptureWindow>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CCaptureWindow>)
	END_MSG_MAP()

public:
	// Called prior to displaying the menu
	//void PrepareMenu(HMENU hMenu);
	void	ShowFullScreen();
	void	HideWindow();
	void	CaptureScreen();
	void	ClearResources();

	//CWTLTrayIconWindowView m_view;
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	HDC		GetMonitorDC();
	void	CreateBackBuffersIfNeeded();
	void	DestroyBackBuffers();
	void	DestroyMonitorDC();
	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual void OnFinalMessage(HWND /*hWnd*/);

private:
	HDC		m_hScreenDC;
	HDC		hMemDC;
	HBITMAP	hMemScreenBitmap;
	HBITMAP	hMemBackFrameBitmap;
	HCURSOR	hCursor;
	HPEN	hDottedPen;
	CDesktopRectSelector*	pManager;
	bool	isHidden;
	//! Drawing params
	bool	drawFrame;
	RECT	rcRect;
	byte	drawFlags;
};