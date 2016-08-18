// RemoteDesktopWindow.cpp : Implementation of CRemoteDesktopWindow

#include "stdafx.h"
#include "../../include/view/RemoteDesktopWindow.h"

CRemoteDesktopWindow::CRemoteDesktopWindow(LPCSTR alpTarget, SHARING_CONTEXT* apContext)
{
	hBitmap = 0;
	context = apContext;
	mImageView.SetContext(apContext);
	mImageView.SetSessionName(alpTarget);
	mImageView.InitMutex();
	isClosed = false;
	frameWidth = 0;
	frameHeight = 0;
	firstShow = true;
}

CRemoteDesktopWindow::~CRemoteDesktopWindow()
{
	isClosed = true;
	mImageView.Lock();
	if( IsWindow())
	{
		DestroyWindow();
		m_hWnd = 0;
	}
	if( hBitmap != 0)
	{
		DeleteObject( hBitmap);
		hBitmap = 0;
	}
	mImageView.UnLock();
	mImageView.DeleteMutex();
}

void CRemoteDesktopWindow::UpdateViewPosition()
{
	RECT	rcView;
	GetClientRect(&rcView);
//	if (rcView.bottom - rcView.top >= frameHeight+80)
		rcView.bottom = frameHeight;
// 	else
//		rcView.bottom = rcView.bottom-80;
//	if (rcView.right-rcView.left >= frameWidth)
		rcView.right = frameWidth;
	mImageView.MoveWindow( &rcView, TRUE);

//	GetClientRect(&rcView);
//	rcView.top = rcView.bottom-80;
//	mPanel.MoveWindow(&rcView, TRUE);

// 	CWindow wndView = GetDlgItem( IDC_VIEW_AREA);
// 	RECT rcView;
// 	wndView.GetWindowRect( &rcView);
// 	ScreenToClient( &rcView);
// 	mImageView.MoveWindow( &rcView, TRUE);
}
LRESULT CRemoteDesktopWindow::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	return 0;
	UpdateViewPosition();
	bHandled = FALSE;
	Invalidate(FALSE);
	mPanel.Invalidate();
	return 0;
}

LRESULT CRemoteDesktopWindow::OnSizing(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	return 0;
	RECT* lpRect = (RECT*)lParam;
	if( lpRect)
	{
		if( ((lpRect->right - lpRect->left)) % 2 == 1)	lpRect->right--;
		if( ((lpRect->bottom - lpRect->top)) % 2 == 1)	lpRect->bottom--;
	}
	return FALSE;
}
LRESULT CRemoteDesktopWindow::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	bHandled = FALSE;
	return 0;

}

LRESULT CRemoteDesktopWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( mImageView.Create( m_hWnd, NULL, NULL, WS_CHILD | WS_BORDER, 0))
	{
		mImageView.SetBitmap( hBitmap);
		mImageView.ShowWindow(SW_SHOW);
	}
	mPanel.Create(m_hWnd,NULL);
//	mPanel.ShowWindow(SW_SHOW);

 	UpdateViewPosition();

	return 1;  // Let the system set the focus
}
LRESULT CRemoteDesktopWindow::OnClose (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ShowWindow( SW_HIDE);
	return 0;
} 

void CRemoteDesktopWindow::Show()
{
	if( hBitmap != 0)
	{
		DeleteObject( hBitmap);
		hBitmap = 0;
	}
	HDC hScreenDC = ::GetDC( 0);
	if( hScreenDC)
	{
		hBitmap = CreateCompatibleBitmap( hScreenDC
				, 50
				, 50);
		::ReleaseDC(0, hScreenDC);
	}
	RECT rect = {0,0,500,500};
	
	if( !IsWindow())
	{
		if( Create( GetActiveWindow(),rect, 0))
		{
			ShowWindow( SW_SHOWNORMAL);
			CenterWindow( 0);
		}
	}
	else
	{
		mImageView.SetBitmap( hBitmap);
		mImageView.ShowWindow(SW_SHOW);
	}
	SetWindowPos( HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}
void CRemoteDesktopWindow::SetWindowSize(unsigned int auWidth, unsigned int auHeight)
{
	RECT	rc;
	GetClientRect(&rc);
	int w = auWidth;
	int h = auHeight;
	if ((w != rc.right-rc.left || h != rc.bottom - rc.top) && firstShow)
	{
		frameWidth = auWidth;
		frameHeight = auHeight;
		ResizeClient(w,h);
		UpdateViewPosition();
	}
	firstShow = false;
}

void CRemoteDesktopWindow::Update(unsigned int auWidth, unsigned int auHeight
								  , void* lpData, unsigned int auSize, bool abReverseHMode)
{
	SetWindowSize(auWidth, auHeight);
	mImageView.Lock();
	if (isClosed)
		return;
	if( hBitmap != 0)
	{
		DeleteObject( hBitmap);
		hBitmap = 0;
	}
	HDC screenDC = ::GetDC(0);
	BITMAPINFO info;
	void* pBits = 0;
	ZeroMemory(&info,sizeof(BITMAPINFO));
	info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth = auWidth;
	info.bmiHeader.biHeight = !abReverseHMode ? auHeight : -((int)auHeight);
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = 24;
	info.bmiHeader.biCompression = BI_RGB;

	hBitmap = ::CreateDIBSection(screenDC, &info
		, DIB_RGB_COLORS,(void **)&pBits, NULL, 0);

	
	::SetDIBits(screenDC, hBitmap, 0, auHeight, lpData, &info, DIB_RGB_COLORS);
	::ReleaseDC( 0, screenDC);


	mImageView.SetBitmap( hBitmap);
	mImageView.ShowWindow(SW_SHOW);
	
	mImageView.UnLock();
}
