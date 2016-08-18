#include "StdAfx.h"
#include "../../include/DesktopMan.h"
#include "../../include/selectors/DesktopRectSelector.h"
#include "../../include/selectors/DesktopFrame.h"
#include "../../include/IDesktopMan.h"
#include "../../include/CaptureWindow.h"
#include "../../include/MemLeaks.h"
#include "../../include/selectors/SafeRect.h"
#include <WindowsX.h>
#include "../../WebCamGrabber/errorCodes.h"

CDesktopRectSelector::CDesktopRectSelector(oms::omsContext* apContext):
MP_VECTOR_INIT(captureWindows),
MP_VECTOR_INIT(captureBuffers)
{
	context = apContext;
	currentFrame = NULL;
	selectionOnProgress = false;
	lButtonDownCount = 0;
	screenWidth = 0;
	screenHeight = 0;
	m_hScreenDC = NULL;
	m_hBackHDC = NULL;
	m_isInteractiveBoardCapture = false;
	m_interactiveBoardMonitorID = -1;
	m_needUpdateInteractiveBoard = false;
}

CDesktopRectSelector::~CDesktopRectSelector()
{
	ClearCapturedImage();
	DeleteDCes();
	DestroyCaptureWindows();
}

int CDesktopRectSelector::GetType()
{
	if (m_isInteractiveBoardCapture)
		return INTERACTIVEBOARD_SELECTOR;
	else
		return RECT_SELECTOR;
}

void CDesktopRectSelector::DestroyCaptureWindows()
{
	vecCaptureWindows::iterator	it = captureWindows.begin();
	for (; it != captureWindows.end(); it++)
	{
		CCaptureWindow* captureWindow = *it;
		captureWindow->ReleaseWindow();
	}
	captureWindows.clear();
}

/************************************************************************/
/* Public
/************************************************************************/
// начинает выборку области
bool CDesktopRectSelector::StartSourceSelection()
{
	lButtonDownCount = 0;
	CreateCaptureWindows();
	if (m_isInteractiveBoardCapture)
	{
		CCaptureWindow* captureWindow = GetVirtualBoardMonitor();
		if (!captureWindow)
		{
			InvokeEx<int, &IDesktopSelectorEvents::OnSelectionCompleted>(INTERACTIVE_BOARD_NOT_FOUND);
			return false;
		}
		MONITORINFOEX monitorInfo = captureWindow->GetMonitorInfo();
		selectionRect = monitorInfo.rcMonitor;
		return true;
	}

	vecCaptureWindows::iterator	it = captureWindows.begin();
	for (; it != captureWindows.end(); it++)
	{
		CCaptureWindow* captureWindow = *it;
		captureWindow->ShowFullScreen();
	}
	return true;
}

// заканчивает выборку области
void CDesktopRectSelector::CancelSourceSelection()
{
	lButtonDownCount = 0;
	selectionOnProgress = false;
	HideCaptureWindows();
}

void CDesktopRectSelector::EscSourceSelection()
{
	Invoke<&IDesktopSelectorEvents::OnSelectionEscape>();
}

// возвращает последний полученный кадр
IDesktopFrame* CDesktopRectSelector::GetCurrentFrame()
{
	return currentFrame;
}

// делает снимок нового кадра с текущими параметрами
IDesktopFrame* CDesktopRectSelector::RecapFrame()
{
	if( lButtonDownCount == 0 && !m_isInteractiveBoardCapture)
	{
		ATLASSERT( FALSE);
		return NULL;
	}

	if (m_needUpdateInteractiveBoard)
	{
		CCaptureWindow* captureWindow = GetVirtualBoardMonitor();
		if (captureWindow)
		{
			MONITORINFOEX monitorInfo = captureWindow->GetMonitorInfo();
			selectionRect = monitorInfo.rcMonitor;
		}
		m_needUpdateInteractiveBoard = false;
	}
	CreateCaptureWindows();
	vecCaptureWindows::iterator	it = captureWindows.begin();
	for (; it != captureWindows.end(); it++)
	{
		CCaptureWindow* captureWindow = *it;
		//можем сразу копировать с экрана без промежуточного буфера
		//captureWindow->CaptureScreen();
		//HideCaptureWindows();
	}
	CreateCapturedImage();

	return currentFrame;
}

// возвращает текущую область захвата
RECT CDesktopRectSelector::GetCurrentSelectingRect()
{
	return this->selectionRect;
}

//возвращает true если захват возможен
bool CDesktopRectSelector::IsCaptureAvailable()
{
	return true;
}
bool CDesktopRectSelector::CanShowCaptureRect()
{
	return !m_isInteractiveBoardCapture;
}
void CDesktopRectSelector::SetShowCursorState(int state)
{
	SetCursorState(state);
}

/************************************************************************/
/* Private
/************************************************************************/
BOOL CALLBACK MonitorEnumProc( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	ATLASSERT( dwData);
	CDesktopRectSelector::vecMonitorsInfo* monitors = (CDesktopRectSelector::vecMonitorsInfo*)dwData;
	MONITORINFOEX info;
	info.cbSize = sizeof( info);
	if( GetMonitorInfo( hMonitor, &info))
		monitors->push_back( info);
	return TRUE;
}

void	CDesktopRectSelector::CreateCaptureWindows()
{
	vecMonitorsInfo monitorsInfo;
	EnumDisplayMonitors( 0, NULL, MonitorEnumProc, (LPARAM)&monitorsInfo);
	vecMonitorsInfo::iterator	it = monitorsInfo.begin();
	float scaleX = 1, scaleY = 1;
	float minScaleX = 1, minScaleY = 1;
	float maxScaleX = 1, maxScaleY = 1;
	for (; it != monitorsInfo.end(); it++)
	{
		FindScale((*it), scaleX, scaleY);
		if (scaleX < minScaleX)
			minScaleX = scaleX;
		if (scaleY < minScaleY)
			minScaleY = scaleY;
		if (scaleX > maxScaleX)
			maxScaleX = scaleX;
		if (scaleY > maxScaleY)
			maxScaleY = scaleY;
	}
	it = monitorsInfo.begin();
	for (; it != monitorsInfo.end(); it++)
	{
		CreateMonitorCaptureWindow( (*it));
	}
	ScaleSelectionRect(maxScaleX, maxScaleY);
	ScaleSelectionRect(minScaleX, minScaleY);
}

void	CDesktopRectSelector::FindScale( MONITORINFOEX& info, float& dx, float& dy)
{
	vecCaptureWindows::iterator	it = captureWindows.begin();
	for (; it != captureWindows.end(); it++)
	{
		CCaptureWindow* captureWindow = *it;
		MONITORINFOEX capturedInfo = captureWindow->GetMonitorInfo();
		if( capturedInfo.szDevice != NULL && info.szDevice != NULL 
			&& strcmp(capturedInfo.szDevice, info.szDevice) == 0)
		{
			if( info.rcWork.left == capturedInfo.rcWork.left
				&& info.rcWork.right == capturedInfo.rcWork.right
				&& info.rcWork.top == capturedInfo.rcWork.top
				&& info.rcWork.bottom == capturedInfo.rcWork.bottom)
			{
				return;
			}

			if (capturedInfo.rcMonitor.right != 0)
				dx = (float)info.rcMonitor.right / capturedInfo.rcMonitor.right;
			if (capturedInfo.rcMonitor.bottom != 0)
				dy = (float)info.rcMonitor.bottom / capturedInfo.rcMonitor.bottom;

		}			
	}
}
void	CDesktopRectSelector::CreateMonitorCaptureWindow( MONITORINFOEX& info)
{
	RECT rcWindow;

	vecCaptureWindows::iterator	it = captureWindows.begin();
	for (; it != captureWindows.end();)
	{
		CCaptureWindow* captureWindow = *it;
		MONITORINFOEX capturedInfo = captureWindow->GetMonitorInfo();
		if( capturedInfo.szDevice != NULL && info.szDevice != NULL 
			&& strcmp(capturedInfo.szDevice, info.szDevice) == 0)
		{
			if( info.rcWork.left == capturedInfo.rcWork.left
				&& info.rcWork.right == capturedInfo.rcWork.right
				&& info.rcWork.top == capturedInfo.rcWork.top
				&& info.rcWork.bottom == capturedInfo.rcWork.bottom)
			{
				return;
			}

			captureWindow->ReleaseWindow();
			it = captureWindows.erase( it);
		}
		else
			it++;
	}

	MP_NEW_P( pCaptureWindow, CCaptureWindow, this);
	pCaptureWindow->CreateOnMonitor( info);
	rcWindow.bottom = info.rcMonitor.bottom;
	rcWindow.top = info.rcMonitor.top;
	rcWindow.left = info.rcMonitor.left;
	rcWindow.right = info.rcMonitor.right;
	pCaptureWindow->SetWindowPos( HWND_TOPMOST, &rcWindow, SWP_HIDEWINDOW);
	captureWindows.push_back( pCaptureWindow);
	
	if (screenHeight < info.rcMonitor.bottom)
		screenHeight = info.rcMonitor.bottom;
	if (screenWidth < info.rcMonitor.right)
		screenWidth = info.rcMonitor.right;
}

void	CDesktopRectSelector::HideCaptureWindows()
{
	vecCaptureWindows::iterator	it = captureWindows.begin();
	for (; it != captureWindows.end(); it++)
	{
		CCaptureWindow* captureWindow = *it;
		captureWindow->HideWindow();
	}
}

POINT GetAbsXY( int aX, int aY, CCaptureWindow* apCaptureWindow)
{
	POINT pnt={aX,aY};
	RECT rcWindow;
	if (apCaptureWindow->GetWindowRect( &rcWindow))
	{
		pnt.x += rcWindow.left;
		pnt.y += rcWindow.top;
	}
	return pnt;
}

void	CDesktopRectSelector::ScaleSelectionRect(float dx, float dy)
{
	selectionRect.left = (long)(selectionRect.left*dx);
	selectionRect.right = (long)(selectionRect.right*dx);
	selectionRect.top = (long)(selectionRect.top*dy);
	selectionRect.bottom = (long)(selectionRect.bottom*dy);
}


void	CDesktopRectSelector::StartSelection( int aX, int aY, CCaptureWindow* apCaptureWindow)
{
	if( lButtonDownCount != 1)
	{
		selectionOnProgress = true;
		startPoint = GetAbsXY( aX, aY, apCaptureWindow);
		currentPoint = startPoint;
	}
}

void	CDesktopRectSelector::UpdateSelectionRect( int aX, int aY, CCaptureWindow* apCaptureWindow)
{
	currentPoint = GetAbsXY( aX, aY, apCaptureWindow);

	selectionRect.left = min( startPoint.x, currentPoint.x);
	selectionRect.right = max( startPoint.x, currentPoint.x);
	selectionRect.top = min( startPoint.y, currentPoint.y);
	selectionRect.bottom = max( startPoint.y, currentPoint.y);
}

void	CDesktopRectSelector::ClearCapturedImage()
{
	if( currentFrame != NULL)
	{
		MP_DELETE( currentFrame);
		currentFrame = NULL;
	}
}

void CDesktopRectSelector::DeleteDCes()
{
	if (m_hScreenDC)
		ReleaseDC( 0, m_hScreenDC);
	if (m_hBackHDC)
		DeleteDC( m_hBackHDC);
	m_hScreenDC = 0;
	m_hBackHDC = 0;
}

void	CDesktopRectSelector::CreateDCesIfNeeded()
{
	if (m_hScreenDC != NULL)
		return;
	m_hScreenDC = GetDC(0);
	if (m_hScreenDC)
		m_hBackHDC = CreateCompatibleDC( m_hScreenDC);
}

void	CDesktopRectSelector::CreateCapturedImage()
{
	void *imgBits = NULL;
	BITMAPINFO bmpInfo;
	ClearCapturedImage();

	if( selectionRect.right-selectionRect.left == 0 
		|| selectionRect.bottom-selectionRect.top == 0)
		return;

	CSafeRect safeRect(screenWidth*16, screenHeight*16);
	safeRect.Init(selectionRect.left, selectionRect.top, selectionRect.right, selectionRect.bottom);
	selectionRect.left = safeRect.GetX();
	selectionRect.top = safeRect.GetY();
	selectionRect.right = safeRect.GetX2()+1;
	selectionRect.bottom = safeRect.GetY2()+1;

	CreateDCesIfNeeded();

	if( m_hScreenDC)
	{
		if( m_hBackHDC)
		{
			HBITMAP hCapturedImage = CreateDIBSection(selectionRect.right-selectionRect.left
				, selectionRect.bottom-selectionRect.top
				, 32, &imgBits, bmpInfo, true);
			bmpInfo.bmiHeader.biSizeImage = bmpInfo.bmiHeader.biWidth*abs(bmpInfo.bmiHeader.biHeight)*bmpInfo.bmiHeader.biBitCount/8;
			if( hCapturedImage)
			{
				HBITMAP oldBitmap = SelectBitmap( m_hBackHDC, hCapturedImage);
				vecCaptureWindows::iterator	it = captureWindows.begin();
				for (; it != captureWindows.end(); it++)
				{
					RECT rcWindow, rcDestRect;

					CCaptureWindow* captureWindow = *it;
					captureWindow->GetWindowRect( &rcWindow);

					if( IntersectRect( &rcDestRect, &rcWindow, &selectionRect))
					{
						RECT wndRect = rcDestRect;
						RECT selectRect = rcDestRect;

						OffsetRect( &wndRect, -rcWindow.left, -rcWindow.top);
						OffsetRect( &selectRect, -selectionRect.left, -selectionRect.top);

						captureWindow->CopyImagePart( m_hBackHDC, &wndRect/*src*/, &selectRect/*dest*/);
					}
				}
				DrawCursorIfNeeded( m_hBackHDC, selectionRect);
				SelectBitmap( m_hBackHDC, oldBitmap);

				SAFE_DELETE( currentFrame);
				MP_NEW_V2( currentFrame, CDesktopFrame, imgBits, bmpInfo);

				DeleteObject(hCapturedImage);
			}
		}
	}
}

void	CDesktopRectSelector::HandleMouseMove( int aX, int aY, CCaptureWindow* apCaptureWindow)
{
	if( selectionOnProgress)
	{
		UpdateSelectionRect( aX, aY, apCaptureWindow);
		vecCaptureWindows::iterator	it = captureWindows.begin();
		for (; it != captureWindows.end(); it++)
		{
			RECT rcWindow, rcDestRect;

			CCaptureWindow* captureWindow = *it;
			captureWindow->GetWindowRect( &rcWindow);

			if( IntersectRect( &rcDestRect, &rcWindow, &selectionRect))
			{
				rcDestRect.left -= rcWindow.left;	rcDestRect.right -= rcWindow.left;
				rcDestRect.top -= rcWindow.top;		rcDestRect.bottom -= rcWindow.top;

				captureWindow->DrawFrame( &rcDestRect
					, selectionRect.left > rcWindow.left		// left line 
					, selectionRect.top > rcWindow.top			// top line 
					, selectionRect.right < rcWindow.right		// right line 
					, selectionRect.bottom < rcWindow.bottom);	// bottom line 
			}
		}
	}
}

void	CDesktopRectSelector::StopSelection( int aX, int aY, CCaptureWindow* apCaptureWindow)
{
	lButtonDownCount++;
	if( lButtonDownCount == 2)
	{
		selectionOnProgress = false;
		UpdateSelectionRect( aX, aY, apCaptureWindow);
		HideCaptureWindows();
		CreateCapturedImage();
		int resultCode = 0;
		if( this->selectionRect.right<=this->selectionRect.left || this->selectionRect.bottom<=this->selectionRect.top)
			resultCode = WEBCAMERA_CANNOTCAPTUREFRAMERECT;
		InvokeEx<int, &IDesktopSelectorEvents::OnSelectionCompleted>(resultCode);
	}
}

void	CDesktopRectSelector::EmulateSelection( int aLeft, int aRight, int aTop, int aBottom)
{
	selectionRect.left = aLeft;
	selectionRect.right = aRight;
	selectionRect.top = aTop;
	selectionRect.bottom = aBottom;
	int resultCode = 0;	
	lButtonDownCount = 2;
}

HWND CDesktopRectSelector::GetCapturedWindow()
{
	return NULL;
}

void CDesktopRectSelector::SetInteractiveBoardCapture(int aMonitorID)
{
	m_isInteractiveBoardCapture = true;
	m_interactiveBoardMonitorID = aMonitorID;
}

void CDesktopRectSelector::UpdateCurrentSelectingRect()
{
	m_needUpdateInteractiveBoard = true;
}

CCaptureWindow*	CDesktopRectSelector::GetVirtualBoardMonitor() 
{
	bool successFindMonitor = false;
	if (context->mpRM)
	{
		if (m_interactiveBoardMonitorID >= (int)captureWindows.size() || m_interactiveBoardMonitorID == -1)
			successFindMonitor = false;
		else
			successFindMonitor = true;
	}
	if (successFindMonitor)
		return captureWindows[m_interactiveBoardMonitorID];
	else {
		vecCaptureWindows::iterator	it = captureWindows.begin();
		for (; it != captureWindows.end(); it++)
		{
			CCaptureWindow* captureWindow = *it;
			MONITORINFOEX capturedInfo = captureWindow->GetMonitorInfo();
			if (capturedInfo.dwFlags == MONITORINFOF_PRIMARY)
				return captureWindow;
		}
		return NULL;
	}
	return NULL;
}