#include "StdAfx.h"
#include "screenDraw.h"
#include <mmsystem.h>
#include "showImageCodes.h"

#define     SelectBitmap(hdc, hbm)  ((HBITMAP)SelectObject((hdc), (HGDIOBJ)(HBITMAP)(hbm)))
#define     SelectPen(hdc, hpen)    ((HPEN)SelectObject((hdc), (HGDIOBJ)(HPEN)(hpen)))

CScreenDrawWindow::CScreenDrawWindow( )
{
	isHidden = true;
	monitorInfo.cbSize = 0;

	hMemScreenBitmap = 0;
	hMemBackFrameBitmap = 0;
	m_hScreenDC = NULL;
	posX = 0;
	posY = 0;
	timeToShow = 0;
	startTimeToShow = 0;
	currentShowIndex = 0;
	currentPositionType = 0;
}

CScreenDrawWindow::~CScreenDrawWindow()
{
	
}

void CScreenDrawWindow::ClearResources()
{
	DestroyBackBuffers();
	DestroyMonitorDC();
	
	for (unsigned int i=0; i < bitmapToDrawArray.size(); i++){
		delete bitmapToDrawArray[i];
	}
	bitmapToDrawArray.clear();
}

void CScreenDrawWindow::ReleaseWindow()
{
	ClearResources();
	if( IsWindow())
	{
		DestroyWindow();
	}
}

void CScreenDrawWindow::OnFinalMessage(HWND /*hWnd*/)
{
	ClearResources();
	//delete this;
	// override to do something, if needed
}

LRESULT CScreenDrawWindow::OnDestroy(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	// ??
	bHandled = FALSE;
	return 0;
}

HDC		CScreenDrawWindow::GetMonitorDC()
{
	return ::GetDC( 0);
}

void	CScreenDrawWindow::CreateOnMonitor( MONITORINFOEX info)
{
	ATLASSERT( !IsRunning());

	if( !IsRunning())
	{
		monitorInfo = info;
		CreateBackBuffersIfNeeded();
		Run();
	}
}

void	CScreenDrawWindow::ShowFullScreen()
{
	isHidden = false;
	if( !IsWindow())
		return;
	//CaptureScreen();
	RECT rcWindow = monitorInfo.rcMonitor;
	
	SetWindowPos( HWND_TOPMOST, &rcWindow, SWP_SHOWWINDOW);
	InvalidateRect (NULL, FALSE);
}

void	CScreenDrawWindow::HideWindow()
{
	if( !isHidden)
	{
		isHidden = true;
		ShowWindow( SW_HIDE);
		DestroyBackBuffers();
	}
}

void	CScreenDrawWindow::CaptureScreen()
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

LRESULT CScreenDrawWindow::OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 1;
}

LRESULT CScreenDrawWindow::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (currentShowIndex >= bitmapToDrawArray.size())
		return 0;
	PAINTSTRUCT ps;
	HDC hDC  = BeginPaint(&ps);

	HBITMAP oldMemBitmap = SelectBitmap( hMemDC, hMemBackFrameBitmap);

/*	// Скопируем фон на BackBuffer
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
*/	

	Graphics* graphic = Graphics::FromHDC(hMemDC);
	Color color1;
	graphic->Clear( color1.Transparent);
	Image* img = (Image*)bitmapToDrawArray[currentShowIndex];

	if (currentPositionType == CENTER_POSITION)
	{
		int x = (monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left)/2;
		x = x - img->GetWidth()/2;
		int y = (monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top)/2;
		y = y - img->GetHeight()/2;
		graphic->DrawImage(img, x, y, img->GetWidth(), img->GetHeight());
	}
	else
		graphic->DrawImage(img, posX, posY, img->GetWidth(), img->GetHeight());

	POINT pnt;
	pnt.x = monitorInfo.rcMonitor.left;
	pnt.y = monitorInfo.rcMonitor.top;
	POINT pntSrc;
	pntSrc.x=pntSrc.y=0;
	SIZE szWin;
	szWin.cx = monitorInfo.rcMonitor.right-monitorInfo.rcMonitor.left;
	szWin.cy = monitorInfo.rcMonitor.bottom-monitorInfo.rcMonitor.top;

	BLENDFUNCTION stBlend;
	stBlend.AlphaFormat = AC_SRC_ALPHA;
	stBlend.BlendOp = AC_SRC_OVER;
	stBlend.BlendFlags = 0;
	stBlend.SourceConstantAlpha = 255;
	#define ULW_ALPHA               0x00000002
	::UpdateLayeredWindow(m_hWnd, hDC, &pnt, &szWin, hMemDC, &pntSrc, 0, &stBlend, ULW_ALPHA);
/*
	::SetStretchBltMode(hDC, HALFTONE);

	::BitBlt( hDC , 0, 0
		, monitorInfo.rcMonitor.right-monitorInfo.rcMonitor.left
		, monitorInfo.rcMonitor.bottom-monitorInfo.rcMonitor.top
		, hMemDC
		, 0
		, 0
	//	, bm.bmWidth
	//	, bm.bmHeight
		, SRCCOPY);
*/
	SelectBitmap( hMemDC, oldMemBitmap);
	EndPaint( &ps);

	return 0;
}

LRESULT CScreenDrawWindow::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (timeGetTime() - startTimeToShow > timeToShow)
	{
		currentShowIndex++;
		startTimeToShow = timeGetTime();
		InvalidateRect (NULL, FALSE);
	//	RedrawWindow ( NULL, NULL, 
	//		RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
	//	UpdateWindow ();

		if ((unsigned int)currentShowIndex >= bitmapToDrawArray.size()) {
			HideWindow();
			DestroyWindow();
		}
	}
	return 0;
}

LRESULT CScreenDrawWindow::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	int xPos = GET_X_LPARAM(lParam); 
	int yPos = GET_Y_LPARAM(lParam);

	SetCapture();
	return 0;
}

LRESULT CScreenDrawWindow::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	int xPos = GET_X_LPARAM(lParam); 
	int yPos = GET_Y_LPARAM(lParam);

	return 0;
}

LRESULT CScreenDrawWindow::OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT CScreenDrawWindow::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	int xPos = GET_X_LPARAM(lParam); 
	int yPos = GET_Y_LPARAM(lParam);

	ReleaseCapture();
	return 0;
}

LRESULT CScreenDrawWindow::OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if( (int) wParam == VK_ESCAPE)
	{
		ShowWindow(SW_HIDE);
	}	
	return 0;
}

void CScreenDrawWindow::CreateBackBuffersIfNeeded()
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

void	CScreenDrawWindow::DestroyMonitorDC()
{
	if (m_hScreenDC)
		::ReleaseDC(0, m_hScreenDC);
	
	m_hScreenDC = 0;
}

void CScreenDrawWindow::DestroyBackBuffers()
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

DWORD CScreenDrawWindow::Run()
{
	// Создаем главное окно 
	//CMessageLoop theLoop;
	//_Module.AddMessageLoop(&theLoop);
#define WS_EX_LAYERED           0x00080000
	if( CreateEx( NULL, NULL, WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_LAYERED) == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	this->SetTimer(IDT_TIMER1, (UINT)(TIMEOUT), NULL);
	startTimeToShow = timeGetTime();

	if( isHidden)
		ShowWindow( SW_HIDE);
	else
		ShowFullScreen();

	return 0;
}

/************************************************************************/
/* Protected
/************************************************************************/
BOOL CScreenDrawWindow::PreTranslateMessage(MSG* pMsg)
{
	return CFrameWindowImpl<CScreenDrawWindow>::PreTranslateMessage(pMsg);
		//return TRUE;

	//return m_view.PreTranslateMessage(pMsg);
}

HBITMAP CScreenDrawWindow::CreateDIBSection( unsigned int auWidth, unsigned int auHeight, unsigned int auBPP, void** lppBits, BITMAPINFO &bmpInfo, bool abReverseHMode)
{
	HDC hScreenDC = ::GetDC( 0);
	ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = auWidth;
	bmpInfo.bmiHeader.biHeight = abReverseHMode ? auHeight : -((int)auHeight);
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = auBPP;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	HBITMAP hbm = ::CreateDIBSection(hScreenDC, &bmpInfo
		, DIB_RGB_COLORS,(void **)lppBits, NULL, 0);

	::ReleaseDC(0, hScreenDC);
	return hbm;
}

void CScreenDrawWindow::SetImageToDraw(Bitmap* ahBmp)
{
	bitmapToDrawArray.push_back(ahBmp);
}

void CScreenDrawWindow::SetImagesToDraw(std::vector<Bitmap*> &aBmps)
{
	bitmapToDrawArray = aBmps;
}

void CScreenDrawWindow::SetImgPosition(int aX, int aY)
{
	posX = aX;
	posY = aY;
}

void CScreenDrawWindow::SetImgPosition(int aPositionType)
{
	currentPositionType = aPositionType;
}

void CScreenDrawWindow::SetTimeToShow(int aTime)
{
	timeToShow = aTime;
}