#include "StdAfx.h"
#include "../../include/selectors/WndSelector.h"
#include "../../include/selectors/DesktopFrame.h"
#include "../../include/capturewindow.h"
#include "../../include/selectors/SafeRect.h"
#include <Wingdi.h>
#include <gdiplus.h>
#include <mmsystem.h>
#include "../../WebCamGrabber/errorCodes.h"
#include "../../include/selectors/WndSelectorDialog.h"
#include "../../../nengine/ncommon/ThreadAffinity.h"

//using namespace Gdiplus;
#define     SelectBitmap(hdc, hbm)  ((HBITMAP)SelectObject((hdc), (HGDIOBJ)(HBITMAP)(hbm)))

CWndSelector::CWndSelector(oms::omsContext* apContext, bool NPad)
{
/*	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
*/
	NP=NPad;
	context = apContext;
	bStartSearchWindow = FALSE;
	bCaptureAvailable = TRUE;
	hwndFoundWindow = NULL;
	g_hwndFoundWindow = NULL;
	currentFrame = NULL;

	screenHeight = 0;
	screenWidth = 0;

	MP_NEW_V3( wndSelectorDlg, CWndSelectorDialog, this, context, NP);
	wndSelectorDlg->Subscribe(this);

	MP_NEW_V2( m_notepadRunner, CCaptureNotepadRunning, this, true);

	m_hScreenDC = NULL;
	m_hBackHDC = NULL;

	m_lastCaptureTime = 0;
	m_updateWindowTime = 2000;
	m_blackScreenCount = 0;
}

bool CWndSelector::Release()
{
	if (m_notepadRunner->IsRunning())
	{
		return false;
	}
	else
	{
		MP_DELETE_THIS;
		return true;
	}
}

int CWndSelector::CaptureNotepadIfNeeded()
{
	m_notepadRunner->Start();

	return true;
}

bool CWndSelector::OpenNotepad()
{
	bool ON;
	WCHAR np_name[300];
	WCHAR windir[255];
	STARTUPINFOW cif;
	ZeroMemory(&cif,sizeof(STARTUPINFO));
	GetWindowsDirectoryW(windir, 250);
	wcscpy_s(np_name, 300, windir);
	wcscat_s(np_name, 300, L"\\notepad.exe");
	ON = CreateProcessW(np_name,NULL,NULL,NULL,FALSE,NULL,NULL,NULL,&cif,&pinf) != FALSE;
	HWND hWndDesktop = GetDesktopWindow();
	return ON;
}

bool CWndSelector::CaptureNotepad()
{
	bool Find = FALSE;
	DWORD WFI = WaitForInputIdle(pinf.hProcess, 10000);
	if (WFI == 0)
	{
		hWnd = ::GetTopWindow(0);
		HWND WindowHandle = NULL;
		while(hWnd)
		{
			DWORD pid;
			DWORD dwTheardId = ::GetWindowThreadProcessId(hWnd, &pid);
			if(pid == pinf.dwProcessId) // your process id;
			{
				//hWnd
				CWindow selectedWindow = hWnd;
				RECT selectedWndRect;
				selectedWindow.GetWindowRect(&selectedWndRect);
				selectedWindow.SetWindowPos(HWND_BOTTOM, &selectedWndRect, SWP_NOACTIVATE);
				wndSelectorDlg->OnProcessWindowSelected(hWnd);
				Find = TRUE;
				return TRUE;
			}
			
			hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT);
		}
		if(Find != TRUE) return FALSE;
	}
	else
	{
		return FALSE;
	}
	return FALSE;
}

void CWndSelector::ReturnError(int anErrorCode)
{
	if( wndSelectorDlg)
	{
		wndSelectorDlg->ShowWindow( SW_HIDE);
		int resultCode = anErrorCode;
		if( !g_hwndFoundWindow && resultCode==0)
			resultCode = USER_NOTCHOICEWND;
		InvokeEx<int, &IDesktopSelectorEvents::OnSelectionCompleted>(resultCode);
	}
}

CWndSelector::~CWndSelector(void)
{
	if (m_notepadRunner->IsRunning())
		m_notepadRunner->SignalStop();
	SAFE_DELETE(m_notepadRunner);
	ClearCapturedImage();
	DeleteDCes();
	//DeleteHBitmap();
	if(wndSelectorDlg != NULL)
	{
		wndSelectorDlg->ReleaseWindow();
		wndSelectorDlg = NULL;
	}
}

int CWndSelector::GetType()
{
	return WND_SELECTOR;
}

void CWndSelector::FromWindow(HWND hWindow)
{
	hwndDialog = hWindow;
}

//////////////////////////////////////////////////////////////////////////

// начинает выборку области
bool CWndSelector::StartSourceSelection()
{
	DefineScreenSize();
	wndSelectorDlg->StartSelection();
	
	return true;
}

// заканчивает выборку области
void CWndSelector::CancelSourceSelection()
{
	bStartSearchWindow = false;
	wndSelectorDlg->ShowWindow(SW_HIDE);

	//DeleteHBitmap();
}

// возвращает последний полученный кадр
IDesktopFrame* CWndSelector::GetCurrentFrame()
{
	return currentFrame;
}

void CWndSelector::CreateDCesIfNeeded()
{
	if (m_hScreenDC != NULL || m_hBackHDC != NULL)
		return;
	m_hScreenDC = GetDC( 0);
	if( m_hScreenDC)
	{
		m_hBackHDC = CreateCompatibleDC( m_hScreenDC);
	}
}

void CWndSelector::DeleteDCes()
{
	if (m_hScreenDC)
		ReleaseDC( 0, m_hScreenDC);
	if (m_hBackHDC)
		DeleteDC( m_hBackHDC);
	m_hScreenDC = 0;
	m_hBackHDC = 0;
}

// делает снимок нового кадра с текущими параметрами
IDesktopFrame* CWndSelector::RecapFrame()
{
	ClearCapturedImage();
	//ATLTRACE("HANDLE  %d\n", g_hwndFoundWindow);
	if (IsWindowVisible(g_hwndFoundWindow))
	{
		RECT rec;
		BOOL ret = GetWindowRect(g_hwndFoundWindow, &rec);
		int width, height;

		// g_hwndFoundWindow - дочернее окно и при сворачивании его изображение не надо обновлять 
		HWND hParent = GetParent(g_hwndFoundWindow);
		if (hParent != NULL && rec.bottom < 0 && rec.left < 0 && rec.right < 0 && rec.top < 0)
			return NULL;
		
		CSafeRect safeRect(screenWidth*16, screenHeight*16);
		safeRect.Init(rec.left, rec.top, rec.right, rec.bottom);
		rec.left = safeRect.GetX();
		rec.top = safeRect.GetY();
		rec.right = safeRect.GetX2()+1;
		rec.bottom = safeRect.GetY2()+1;

		GetWindowSize(rec, width, height);
		CreateDCesIfNeeded();
		
		if( m_hScreenDC)
		{
			if( m_hBackHDC)
			{
				void *pBits = NULL;
				BITMAPINFO info;
				HBITMAP hCapturedImage = CreateDIBSection(width, height, 32, &pBits, info, true);
				info.bmiHeader.biSizeImage = info.bmiHeader.biWidth*abs(info.bmiHeader.biHeight)*info.bmiHeader.biBitCount/8;
				if( hCapturedImage)
				{
					HBITMAP oldBitmap = SelectBitmap( m_hBackHDC, hCapturedImage);
					MakeWhiteBitmap(pBits, width, height);
/*
					PISWINREDIRECTEDFOPRINT pIsWindowRedirectedForPrint = NULL;
					BOOL canCapture = true;
					if (GetModuleHandle(TEXT("User32.dll")))
					{
						pIsWindowRedirectedForPrint = (PISWINREDIRECTEDFOPRINT) GetProcAddress(
							GetModuleHandle(TEXT("User32.dll")), 
							"IsWindowRedirectedForPrint");
					}
					
					if(NULL != pIsWindowRedirectedForPrint)
					{
						canCapture = pIsWindowRedirectedForPrint();
					}
					if (!canCapture)
						return NULL;
*/
					ret = PrintWindow(g_hwndFoundWindow, m_hBackHDC, 0);

					if (timeGetTime() - m_lastCaptureTime > m_updateWindowTime ) {
						RefreshWindow(g_hwndFoundWindow);
						m_lastCaptureTime = timeGetTime();
					}

					DrawCursorIfNeeded( m_hBackHDC, rec);
					SelectBitmap( m_hBackHDC, oldBitmap);
					
					if (IsCapturedImageBlack((unsigned char*)pBits, width, height))
						m_blackScreenCount++;
					else
						m_blackScreenCount = 0;
					//если 3 подряд кадра скапчены черными то показываем его , иначе думает что PrintWindow неверно скапчил
					if (!ret || (m_blackScreenCount > 0 && m_blackScreenCount <= 2))
						return NULL;

					MP_NEW_V2( currentFrame, CDesktopFrame, pBits, info);
					DeleteObject(hCapturedImage);
				//	SaveBitmapToFile(&hCapturedImage);
				}
			}
		}
		return currentFrame;
	}
	bCaptureAvailable = FALSE;
	return NULL;
}

bool CWndSelector::IsCapturedImageBlack(unsigned char* imgBits, int width, int height)
{
	for (int i=0; i<width*height; i++)
	{
		if (imgBits[i] != 0)
			return false;
	}
	return true;
}

void CWndSelector::Set_g_hwndFoundWindow(HWND wnd)
{
	g_hwndFoundWindow = wnd;
	return ;
}
RECT CWndSelector::GetCurrentSelectingRect()
{
	if(g_hwndFoundWindow && IsWindowVisible(g_hwndFoundWindow))
	{
		RECT rec;
		GetWindowRect(g_hwndFoundWindow, &rec);
		return rec;
	}
	RECT rec = {0, 0, 0, 0};
	return rec;	
}

bool CWndSelector::IsCaptureAvailable()
{
	return bCaptureAvailable;
}

//////////////////////////////////////////////////////////////////////////

void CWndSelector::ClearCapturedImage()
{
	SAFE_RELEASE( currentFrame);
}
/*
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes
	ImageCodecInfo* pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure
	void* mem = MP_NEW_ARR(BYTE, size);
	pImageCodecInfo = (ImageCodecInfo*)mem;
	if(pImageCodecInfo == NULL)
		return -1;  // Failure
	GetImageEncoders(num, size, pImageCodecInfo);
	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			SAFE_DELETE_ARRAY(pImageCodecInfo);
			return j;  // Success
		}    
	}
	SAFE_DELETE_ARRAY(pImageCodecInfo);
	return -1;  // Failure
}
*/
bool CWndSelector::SaveBitmapToFile(HBITMAP *pHBmp)
{
/*	HPALETTE pal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);
	Bitmap bmp(*pHBmp, pal);
	CLSID   encoderClsid;
	GetEncoderClsid(L"image/bmp", &encoderClsid);
	Status stat = bmp.Save(L"C:\\a.bmp", &encoderClsid, NULL);
	*/
	return true;
}

bool CWndSelector::GetWindowSize(RECT &rec, int &width, int &height)
{
	width = rec.right - rec.left;
	height = rec.bottom - rec.top;
	return (width > 0 && height > 0);
}

bool CWndSelector::StartSelection()
{
	// Set the global "g_bStartSearchWindow" flag to TRUE.
	bStartSearchWindow = TRUE;
	// Very important : capture all mouse activities from now onwards and
	// direct all mouse messages to the "Search Window" dialog box procedure.
	SetCapture (hwndDialog);
	return true;
}

HWND CWndSelector::StopSelection()
{
	// If there was a found window, refresh it so that its highlighting is erased. 
	if (g_hwndFoundWindow)
	{
		RefreshWindow(g_hwndFoundWindow, true);
	}
	// Very important : must release the mouse capture.
	ReleaseCapture();

	// Set the global search window flag to FALSE.
	bStartSearchWindow = FALSE;

	/*RECT rec;
	GetWindowRect(g_hwndFoundWindow, &rec);
	int width, height;
	GetWindowSize(rec, width, height);
	CreateHBitmap(width, height);*/

	return g_hwndFoundWindow;
}

/*void CWndSelector::CreateHBitmap(int &width, int &height)
{
	DeleteHBitmap();

	HDC hWindowDC = GetDC(0);//g_hwndFoundWindow);
	hMemDC = CreateCompatibleDC(hWindowDC);
	hCapturedImage = CreateCompatibleBitmap(hWindowDC, width, height);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hCapturedImage);
	//ReleaseDC(g_hwndFoundWindow, hWindowDC);
	ReleaseDC(0, hWindowDC);
}

void CWndSelector::DeleteHBitmap()
{
	if( hCapturedImage != 0)
	{
		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
		DeleteObject(hCapturedImage);
		hCapturedImage = 0;
		hMemDC = 0;
	}
}*/




void CWndSelector::RefreshWindow (HWND hwndWindowToBeRefreshed, bool aTotal)
{
	if (aTotal)
	{
		InvalidateRect (hwndWindowToBeRefreshed, NULL, TRUE);
		RedrawWindow (hwndWindowToBeRefreshed, NULL, NULL, 
			RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
		UpdateWindow (hwndWindowToBeRefreshed);
	}
	else
	{
		RedrawWindow (hwndWindowToBeRefreshed, NULL, NULL, 
			RDW_NOERASE | RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
	}
}

HWND CWndSelector::DoMouseMove()
{
	if (bStartSearchWindow)
	{
		POINT		screenpoint;
		HWND		hwndFoundWindow = NULL;

		GetCursorPos (&screenpoint);  

		// Determine the window that lies underneath the mouse cursor.
		hwndFoundWindow = WindowFromPoint (screenpoint);

		// Check first for validity.
		if (CheckWindowValidity (hwndFoundWindow))
		{
			// We have just found a new window.

			//проверяем - а не фоновое ли это окно(рабочий стол) - при капчинге попртятся все окна
			LONG styleWin = ::GetWindowLong( hwndFoundWindow,GWL_STYLE);
			if ( !((styleWin & WS_CHILDWINDOW) &&
				(styleWin & WS_VISIBLE) &&
				(styleWin & LVS_NOSCROLL) &&
				(styleWin & LVS_EDITLABELS) &&
				(styleWin & LVS_SHAREIMAGELISTS)) )
			{
				// If there was a previously found window, we must instruct it to refresh itself. 
				// This is done to remove any highlighting effects drawn by us.
				if (g_hwndFoundWindow)
				{
					RefreshWindow (g_hwndFoundWindow, true);
				}

				// Indicate that this found window is now the current global found window.
				g_hwndFoundWindow = hwndFoundWindow;
				HighlightFoundWindow (g_hwndFoundWindow);
			}
		}
	}
	return g_hwndFoundWindow;
}

// Synopsis :
// 1. This function checks a hwnd to see if it is actually the "Search Window" Dialog's or Main Window's
// own window or one of their children. If so a FALSE will be returned so that these windows will not
// be selected. 
// 2. Also, this routine checks to see if the hwnd to be checked is already a currently found window.
// If so, a FALSE will also be returned to avoid repetitions.
bool CWndSelector::CheckWindowValidity (HWND hwndToCheck)
{
	// The window must not be NULL.
	if (hwndToCheck == NULL)
	{
		return false;	
	}

	// It must also be a valid window as far as the OS is concerned.
	if (IsWindow(hwndToCheck) == FALSE)
	{
		return false;
	}

	// Ensure that the window is not the current one which has already been found.
	if (hwndToCheck == g_hwndFoundWindow)
	{
		return false;
	}

	// It also must not be the "Search Window" dialog box itself.
	if (hwndToCheck == hwndDialog)
	{
		return false;
	}

	if (GetMainWindowHandle() == hwndToCheck)
	{
		return false;
	}
	

	return true;
}

void CWndSelector::HighlightFoundWindow (HWND hwndFoundWindow)
{
	RECT rect;   
	GetWindowRect(hwndFoundWindow, &rect);
	HDC hWindowDC = GetWindowDC (hwndFoundWindow);
	if (hWindowDC)
	{
		HGDIOBJ oldPen = SelectObject(hWindowDC, GetStockObject(DC_PEN));
		HGDIOBJ oldBrush = SelectObject(hWindowDC, GetStockObject(HOLLOW_BRUSH));
		COLORREF oldColor = SetDCPenColor(hWindowDC, RGB(0xff,00,00));

		Rectangle(hWindowDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top);
		SelectObject(hWindowDC, oldPen);
		SelectObject(hWindowDC, oldBrush);
		SetDCPenColor(hWindowDC, oldColor);
		ReleaseDC(hwndFoundWindow, hWindowDC);
	}
}

void CWndSelector::OnSelectorWindowOK()
{
	ReturnError(0);
}

void CWndSelector::OnSelectorWindowCancel()
{
	rtl_TerminateOtherProcess(pinf.hProcess,NO_ERROR);
	if( wndSelectorDlg)
	{
		wndSelectorDlg->ShowWindow( SW_HIDE);
		Invoke<&IDesktopSelectorEvents::OnSelectionEscape>();
	}
}

HWND CWndSelector::GetCapturedWindow()
{
	return g_hwndFoundWindow;
}

bool CWndSelector::CanShowCaptureRect()
{
	return true;
}
void CWndSelector::SetShowCursorState(int state)
{
	SetCursorState(state);
}

BOOL CALLBACK MonitorEnumProcWnd( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	ATLASSERT( dwData);
	CWndSelector::vecMonitorsInfo* monitors = (CWndSelector::vecMonitorsInfo*)dwData;
	MONITORINFOEX info;
	info.cbSize = sizeof( info);
	if( GetMonitorInfo( hMonitor, &info))
		monitors->push_back( info);
	return TRUE;
}

void	CWndSelector::DefineScreenSize()
{
	vecMonitorsInfo monitorsInfo;
	EnumDisplayMonitors( 0, NULL, MonitorEnumProcWnd, (LPARAM)&monitorsInfo);
	vecMonitorsInfo::iterator	it = monitorsInfo.begin();

	for (; it != monitorsInfo.end(); it++)
	{
		if (screenHeight < (*it).rcMonitor.bottom)
			screenHeight = (*it).rcMonitor.bottom;
		if (screenWidth < (*it).rcMonitor.right)
			screenWidth = (*it).rcMonitor.right;
	}
}



//////////////////////////////////////////////////////////////////////////
//class CCaptureNotepadRunning
//////////////////////////////////////////////////////////////////////////

CCaptureNotepadRunning::CCaptureNotepadRunning(CWndSelector* selector, bool notifyAboutComplete)
{
	m_notepadSelector = selector;
	m_notifyAboutComplete = notifyAboutComplete;
}

CCaptureNotepadRunning::~CCaptureNotepadRunning(void)
{
	rtl_TerminateOtherProcess(m_notepadSelector->pinf.hProcess,NO_ERROR);
//	Stop();
}

DWORD CCaptureNotepadRunning::Run()
{
	HANDLE hndl = GetCurrentThread();
	SetSecondaryThreadAffinity( hndl);
	
	int ErrorCode = 0;
	if (m_notepadSelector->OpenNotepad()!=true)
	{
		ErrorCode = WND_NOTCREATE_NOTEPAD;
	}
	else
	{
		if (m_notepadSelector->CaptureNotepad()!=true)
		{
			ErrorCode = WND_NOTCAPTURE_NOTEPAD;
		}
		else
		{

		}
	};
	m_notepadSelector->ReturnError(ErrorCode);
	return true;
}
