#include "StdAfx.h"
#include "..\include\capturewindow.h"
#include "..\include\selectors\DesktopRectSelector.h"
#include "../include/MemLeaks.h"

#define     SelectBitmap(hdc, hbm)  ((HBITMAP)SelectObject((hdc), (HGDIOBJ)(HBITMAP)(hbm)))
#define     SelectPen(hdc, hpen)    ((HPEN)SelectObject((hdc), (HGDIOBJ)(HPEN)(hpen)))
/*
BOOL SaveToFile(LPCSTR fn, HBITMAP hBitmap)
{
	HDC					hdc = NULL;
	LPVOID				pBuf = NULL;
	BITMAPINFO			bmpInfo;
	BITMAPFILEHEADER	bmpFileHeader; 

	FILE* fp = fopen(fn, "w");
	if( !fp)	return FALSE;

	do{ 
		hdc=GetDC(NULL);
		ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
		bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		GetDIBits(hdc,hBitmap,0,0,NULL,&bmpInfo,DIB_RGB_COLORS); 
		if(bmpInfo.bmiHeader.biSizeImage<=0)
			bmpInfo.bmiHeader.biSizeImage=bmpInfo.bmiHeader.biWidth*abs(bmpInfo.bmiHeader.biHeight)*(bmpInfo.bmiHeader.biBitCount+7)/8;

		if((pBuf = malloc(bmpInfo.bmiHeader.biSizeImage))==NULL)
		{
			MessageBox( NULL, "Unable to Allocate Bitmap Memory", "Error", MB_OK|MB_ICONERROR);
			break;
		}            
		bmpInfo.bmiHeader.biCompression=BI_RGB;
		GetDIBits(hdc,hBitmap,0,bmpInfo.bmiHeader.biHeight,pBuf, &bmpInfo, DIB_RGB_COLORS);       

		bmpFileHeader.bfReserved1=0;
		bmpFileHeader.bfReserved2=0;
		bmpFileHeader.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+bmpInfo.bmiHeader.biSizeImage;
		bmpFileHeader.bfType='MB';
		bmpFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER); 

		UINT readed=0;
		fwrite((LPSTR)&bmpFileHeader,sizeof(BITMAPFILEHEADER), 1, fp);
		fwrite((LPSTR)&bmpInfo.bmiHeader,sizeof(BITMAPINFOHEADER), 1, fp);
		fwrite((LPSTR)pBuf,bmpInfo.bmiHeader.biSizeImage, 1, fp);

	}while(false); 
	if(hdc)     ReleaseDC(NULL,hdc); 
	if(pBuf)    free(pBuf); 
	fclose(fp);
	DeleteObject(hBitmap);
	return TRUE;
}
*/
// bool abWindow - создавать ли окно

CCaptureWindow::CCaptureWindow( CDesktopRectSelector* apManager)
{
	ATLASSERT( apManager);
	pManager = apManager;
	monitorInfo.cbSize = 0;
	drawFrame = 0;
	isHidden = true;

	POINT penWidth={1,1};
	LOGPEN pen;
	pen.lopnStyle = PS_DOT;
	pen.lopnWidth = penWidth;
	pen.lopnColor = RGB(0,0,0);
	hDottedPen = CreatePenIndirect( &pen);

	hMemScreenBitmap = 0;
	hMemBackFrameBitmap = 0;
	m_hScreenDC = NULL;
}

CCaptureWindow::~CCaptureWindow()
{
	ClearResources();
}

void CCaptureWindow::ClearResources()
{
	if( hDottedPen)
	{
		DeleteObject( hDottedPen);
		hDottedPen = 0;
	}
	DestroyBackBuffers();
	DestroyMonitorDC();
	
}

void CCaptureWindow::ReleaseWindow()
{
	ClearResources();
	if( IsWindow())
	{
		DestroyWindow();
	}
}

void CCaptureWindow::OnFinalMessage(HWND /*hWnd*/)
{
	MP_DELETE_THIS;
	// override to do something, if needed
}

LRESULT CCaptureWindow::OnDestroy(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	// ??
	bHandled = FALSE;
	return 0;
}

HDC		CCaptureWindow::GetMonitorDC()
{
	//return CreateDC( NULL, monitorInfo.szDevice, NULL, NULL);
	return ::GetDC( 0);
}

void	CCaptureWindow::CreateOnMonitor( MONITORINFOEX info)
{
	ATLASSERT( !IsRunning());

	if( !IsRunning())
	{
		monitorInfo = info;
		CreateBackBuffersIfNeeded();
		Run();
	}
}

void	CCaptureWindow::ShowFullScreen()
{
	drawFrame = 0;
	isHidden = false;
	if( !IsWindow())
		return;

	CreateBackBuffersIfNeeded();
	CaptureScreen();
	RECT rcWindow = monitorInfo.rcMonitor;
	SetWindowPos( HWND_TOPMOST, &rcWindow, SWP_SHOWWINDOW);
}

void	CCaptureWindow::HideWindow()
{
	if( !isHidden)
	{
		isHidden = true;
		ShowWindow( SW_HIDE);
		DestroyBackBuffers();
	}
}

void	CCaptureWindow::CaptureScreen()
{
	if( m_hScreenDC)
	{
		HBITMAP oldBitmap = SelectBitmap( hMemDC, hMemScreenBitmap);
		::BitBlt( hMemDC, 0, 0
				, monitorInfo.rcMonitor.right-monitorInfo.rcMonitor.left
				, monitorInfo.rcMonitor.bottom-monitorInfo.rcMonitor.top
				, m_hScreenDC
				, monitorInfo.rcMonitor.left
				, monitorInfo.rcMonitor.top
				, SRCCOPY);

		SelectBitmap( hMemDC, oldBitmap);
	}
}

void	CCaptureWindow::DrawFrame( LPRECT alpRect, bool leftEdge, bool topEdge, bool rightEdge, bool bottomEdge)
{
	if( alpRect)
	{
		drawFlags = 0;
		if( leftEdge) drawFlags += 1;
		if( topEdge) drawFlags += 2;
		if( rightEdge) drawFlags += 4;
		if( bottomEdge) drawFlags += 8;
		rcRect = *alpRect;
		drawFrame = true;
		//InvalidateRect( &rcRect);
		InvalidateRect( NULL, FALSE);
	}
	else
	{
		drawFrame = true;
		InvalidateRect( NULL, FALSE);
	}
}

void	CCaptureWindow::CopyImagePart( HDC hDC, LPRECT lpSrcRect, LPRECT lpDestRect)
{
//	if( hMemBackFrameBitmap == 0)
//		return;

	::BitBlt( hDC, lpDestRect->left, lpDestRect->top
		, lpDestRect->right-lpDestRect->left
		, lpDestRect->bottom-lpDestRect->top
		, m_hScreenDC
		, lpSrcRect->left+monitorInfo.rcMonitor.left
		, lpSrcRect->top+monitorInfo.rcMonitor.top
		, SRCCOPY);

/* //обходимся без hMemScreenBitmap
	HBITMAP oldMemBitmap = SelectBitmap( hMemDC, hMemScreenBitmap);
	::BitBlt( hDC //dest
		, lpDestRect->left, lpDestRect->top
		, lpDestRect->right-lpDestRect->left
		, lpDestRect->bottom-lpDestRect->top
		, hMemDC
		, lpSrcRect->left
		, lpSrcRect->top
		, SRCCOPY);
	SelectBitmap( hMemDC, oldMemBitmap);
	*/
}

LRESULT CCaptureWindow::OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 1;
}

LRESULT CCaptureWindow::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	PAINTSTRUCT ps;
	HDC hDC  = BeginPaint(&ps);

	HBITMAP oldMemBitmap = SelectBitmap( hMemDC, hMemBackFrameBitmap);

	// Скопируем фон на BackBuffer
	HDC hTempDC  = CreateCompatibleDC( hDC);
	HBITMAP oldTempBitmap = SelectBitmap( hTempDC, hMemScreenBitmap);
	::BitBlt( hMemDC //dest
		, 0, 0
		, monitorInfo.rcMonitor.right-monitorInfo.rcMonitor.left
		, monitorInfo.rcMonitor.bottom-monitorInfo.rcMonitor.top
		, hTempDC
		, 0
		, 0
		, SRCCOPY);
	SelectBitmap( hTempDC, oldTempBitmap);
	DeleteDC( hTempDC);

	if( drawFrame)
	{
		HPEN hOldPen = SelectPen( hMemDC, hDottedPen);
		if( drawFlags & 1)
		{
			::MoveToEx( hMemDC, rcRect.left, rcRect.top, NULL);
			::LineTo( hMemDC, rcRect.left, rcRect.bottom);
		}
		if( drawFlags & 2)
		{
			::MoveToEx( hMemDC, rcRect.left, rcRect.top, NULL);
			::LineTo( hMemDC, rcRect.right, rcRect.top);
		}
		if( drawFlags & 4)
		{
			::MoveToEx( hMemDC, rcRect.right, rcRect.top, NULL);
			::LineTo( hMemDC, rcRect.right, rcRect.bottom);
		}
		if( drawFlags & 8)
		{
			::MoveToEx( hMemDC, rcRect.left, rcRect.bottom, NULL);
			::LineTo( hMemDC, rcRect.right, rcRect.bottom);
		}
		SelectPen( hMemDC, hOldPen);
	}

	::BitBlt( hDC /*dest*/, 0, 0
		, monitorInfo.rcMonitor.right-monitorInfo.rcMonitor.left
		, monitorInfo.rcMonitor.bottom-monitorInfo.rcMonitor.top
		, hMemDC
		, 0
		, 0
		, SRCCOPY);

	SelectBitmap( hMemDC, oldMemBitmap);
	EndPaint( &ps);
	return 0;
}

LRESULT CCaptureWindow::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	int xPos = GET_X_LPARAM(lParam); 
	int yPos = GET_Y_LPARAM(lParam);
	pManager->StartSelection( xPos, yPos, this);
	SetCapture();
	return 0;
}

LRESULT CCaptureWindow::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	int xPos = GET_X_LPARAM(lParam); 
	int yPos = GET_Y_LPARAM(lParam);
	pManager->HandleMouseMove( xPos, yPos, this);
	return 0;
}

LRESULT CCaptureWindow::OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetCursor(hCursor); 
	return 0;
}

LRESULT CCaptureWindow::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	int xPos = GET_X_LPARAM(lParam); 
	int yPos = GET_Y_LPARAM(lParam);
	pManager->StopSelection( xPos, yPos, this);
	ReleaseCapture();
	return 0;
}

LRESULT CCaptureWindow::OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if( (int) wParam == VK_ESCAPE)
	{
		pManager->CancelSourceSelection();
		pManager->EscSourceSelection();
	}	
	return 0;
}

void CCaptureWindow::CreateBackBuffersIfNeeded()
{
	if (hMemScreenBitmap && hMemBackFrameBitmap && m_hScreenDC) 
		return;

	DestroyBackBuffers();
	DestroyMonitorDC();

	BITMAPINFO bmpInfo;
	void *pMemScreenBits, *pMemBackFrameBits;
	//HDC monitorDC = ::GetDC( 0);//GetMonitorDC();
	m_hScreenDC = GetMonitorDC();
	if( m_hScreenDC)
	{
		hMemDC = CreateCompatibleDC( m_hScreenDC);
		if( hMemDC)
		{
			hMemScreenBitmap = CreateDIBSection(monitorInfo.rcMonitor.right-monitorInfo.rcMonitor.left
				, monitorInfo.rcMonitor.bottom-monitorInfo.rcMonitor.top
				, 32, &pMemScreenBits, bmpInfo, true);
			//int ii = GetLastError();
			//SaveToFile("c:\\hMemScreenBitmap.bmp", hMemScreenBitmap);
			hMemBackFrameBitmap = CreateDIBSection( monitorInfo.rcMonitor.right-monitorInfo.rcMonitor.left
				, monitorInfo.rcMonitor.bottom-monitorInfo.rcMonitor.top
				, 32, &pMemBackFrameBits, bmpInfo, true);
			//SaveToFile("c:\\hMemBackFrameBitmap.bmp", hMemBackFrameBitmap);
		}
	}
}

void	CCaptureWindow::DestroyMonitorDC()
{
	if (m_hScreenDC)
		::ReleaseDC(0, m_hScreenDC);
	
	m_hScreenDC = 0;
}

void CCaptureWindow::DestroyBackBuffers()
{
	if( hMemScreenBitmap)
		DeleteObject( hMemScreenBitmap);
	if( hMemBackFrameBitmap)
		DeleteObject( hMemBackFrameBitmap);	

	hMemBackFrameBitmap = 0;
	hMemScreenBitmap = 0;

	if (hMemDC)
	{
		DeleteDC(hMemDC);
		hMemDC = NULL;
	}
}

DWORD CCaptureWindow::Run()
{
	// Создаем главное окно 
	//CMessageLoop theLoop;
	//_Module.AddMessageLoop(&theLoop);

	if( CreateEx( NULL, NULL, WS_POPUP, WS_EX_TOOLWINDOW) == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	hCursor = LoadCursor(NULL, IDC_CROSS); 
	SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)hCursor);   // new cursor 

	if( isHidden)
		ShowWindow( SW_HIDE);
	else
		ShowFullScreen();

	//int nRet = theLoop.Run();

	/*int nRet = 0;
	while( true)
	{
		Sleep(10);
	}
	return 0;*/

	//_Module.RemoveMessageLoop();

	//m_hWnd = 0;
	//ReleaseWindow();

	//return nRet;

	return 0;
}

/************************************************************************/
/* Protected
/************************************************************************/
BOOL CCaptureWindow::PreTranslateMessage(MSG* pMsg)
{
	return CFrameWindowImpl<CCaptureWindow>::PreTranslateMessage(pMsg);
		//return TRUE;

	//return m_view.PreTranslateMessage(pMsg);
}

/*BOOL CCaptureWindow::OnIdle()
{
	UIUpdateToolBar();
	return FALSE;
}
*/