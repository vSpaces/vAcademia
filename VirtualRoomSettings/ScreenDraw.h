#pragma once
#ifndef SCREEN_DRAW
#define SCREEN_DRAW

#include <atlbase.h>
#include <atlapp.h>
#include "../common/ViskoeThread.h"
#include "resource.h"       // main symbols
#include <atlframe.h>

#include <GdiPlus.h>
using namespace Gdiplus;

#define IDT_TIMER1		1
#define TIMEOUT			100


class CScreenDrawWindow :
	public CFrameWindowImpl<CScreenDrawWindow>,
	public CUpdateUI<CScreenDrawWindow>,
	public CMessageFilter,
	public ViskoeThread::CThreadImpl<CScreenDrawWindow>
{
	MONITORINFOEX monitorInfo;

public:
	CScreenDrawWindow();
public:
	virtual ~CScreenDrawWindow();
	void	ReleaseWindow();
	void	CreateOnMonitor( MONITORINFOEX info);
	void	DrawFrame( LPRECT alpRect, bool leftEdge, bool topEdge, bool rightEdge, bool bottomEdge);

public:
	DWORD Run();

public:

	BEGIN_UPDATE_UI_MAP(CScreenDrawWindow)

	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CScreenDrawWindow)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		CHAIN_MSG_MAP(CUpdateUI<CScreenDrawWindow>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CScreenDrawWindow>)
	END_MSG_MAP()


public:
	void	ShowFullScreen();
	void	HideWindow();
	void	CaptureScreen();
	void	ClearResources();
	void	SetImageToDraw(Bitmap* ahBmp);
	void	SetImagesToDraw(std::vector<Bitmap*> &aBmps);
	void	SetImgPosition(int aX, int aY);
	void	SetImgPosition(int aPositionType);
	void	SetTimeToShow(int aTime);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	static HBITMAP CreateDIBSection( unsigned int auWidth, unsigned int auHeight, unsigned int auBPP, void** lppBits, BITMAPINFO &bmpInfo, bool abReverseHMode);
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
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual void OnFinalMessage(HWND /*hWnd*/);

	

private:
	HDC		m_hScreenDC;
	HDC		hMemDC;
	HBITMAP	hMemScreenBitmap;
	HBITMAP	hMemBackFrameBitmap;
	RECT	rcRect;
	byte	drawFlags;
	bool	isHidden;
	std::vector<Bitmap*> bitmapToDrawArray;
	int posX, posY;
	int timeToShow;
	int startTimeToShow;
	int currentShowIndex;
	int currentPositionType;
};

#endif