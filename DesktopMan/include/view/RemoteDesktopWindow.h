// RemoteDesktopWindow.h : Declaration of the CRemoteDesktopWindow

#pragma once

// #include "resource.h"       // main symbols
// #include <atlhost.h>
#include "RemoteWindow.h"
#include "../SharingContext.h"
#include "CPanelButton.h"
// CRemoteDesktopWindow

class CRemoteDesktopWindow : 
	public CFrameWindowImpl<CRemoteDesktopWindow>
{
public:
	CRemoteDesktopWindow(LPCSTR alpTarget, SHARING_CONTEXT* apContext);

	~CRemoteDesktopWindow();

//	enum { IDD = IDD_REMOTEDESKTOPWINDOW };

BEGIN_MSG_MAP(CRemoteDesktopWindow)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
	MESSAGE_HANDLER (WM_CLOSE, OnClose)
	MESSAGE_HANDLER(WM_SIZE, OnSize)
	MESSAGE_HANDLER(WM_SIZING, OnSizing)
	MSG_WM_KEYUP(OnKeyUp)
	MSG_WM_KEYDOWN(OnKeyDown)
// 	MSG_WM_SYSKEYUP(OnKeyUp)
// 	MSG_WM_SYSKEYDOWN(OnKeyDown)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSizing(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
	//	mImageView.OnKeyUp(nChar, nRepCnt, nFlags);
	}

	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
	//	mImageView.OnKeyDown(nChar, nRepCnt, nFlags);
	}

	void UpdateViewPosition();
	bool isClosed;
	CRemoteWindowCtrl mImageView;
	CCPanelButton mPanel;
	CButton m_button;
	HBITMAP	hBitmap;
	SHARING_CONTEXT* context;
	unsigned int frameWidth;
	unsigned int frameHeight;
	bool firstShow;

	void Show();
	void Update(unsigned int auWidth, unsigned int auHeight
		, void* lpData, unsigned int auSize, bool abReverseHMode);
	void SetWindowSize(unsigned int auWidth, unsigned int auHeight);
};


