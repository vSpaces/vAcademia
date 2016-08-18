// D:\Users\Pasa\Programming\vu\Release_FG_To_Head\DesktopMan\src\WndSelectorDialog.cpp : Implementation of CWndSelectorDialog

#include "stdafx.h"
#include "../../include/selectors/WndSelectorDialog.h"
#include "../../include/selectors/WndSelector.h"
#include "..\..\include\selectors\wndselectordialog.h"
//#include "..\include\view\DrawWindow.h"
#define SETWTEXT(x,y) ::SetWindowTextW(x.m_hWnd,y);
#define SETWTEXT(x,y) ::SetWindowTextW(x.m_hWnd,y);
BOOL CALLBACK MonitorEnumProcWnd( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);

// CWndSelectorDialog
CWndSelectorDialog::CWndSelectorDialog(CWndSelector *wndSel, oms::omsContext* apContext, bool NP):
	MP_STRING_INIT(staticCaptionEngText),
	MP_STRING_INIT( staticHideClientEngText),
	MP_STRING_INIT( staticTitleEngText),
	MP_STRING_INIT( staticSizeEngText),
	MP_STRING_INIT( windowCaptionText),
	MP_STRING_INIT( staticTitleEngText_Or),
	MP_STRING_INIT( btn_CaptionText),
	MP_STRING_INIT( btnCaptionCancel),
	MP_STRING_INIT( staticCaptionSelectApp),
	m_bHide(false)
{
	notepad = NP;
	wndSelector = wndSel;
	hBitmapFinderToolFilled = NULL;
	hBitmapFinderToolEmpty = NULL;

	context = apContext;
	wchar_t wcsLanguage[100] = L"";
	context->mpApp->GetLanguage((wchar_t*)wcsLanguage, 99);
	if (!wcscmp(wcsLanguage, L"eng"))
		m_englishVersion = true;
	else
		m_englishVersion = false;

	//english texts
	staticCaptionEngText = "Press an icon with a sight, clamp the left button of the mouse, then drag a sight on a window which want to choose.";
	staticHideClientEngText = "Hide vAcademia";
	staticTitleEngText = "Title: ";
	staticSizeEngText = "Size: ";
	windowCaptionText = "Choose application";
	staticTitleEngText_Or = "... or just press the button and select any running application you like to share";
	btn_CaptionText = "Select";
	btnCaptionCancel = "Cancel";
	staticCaptionSelectApp = "Window to share";
}


CWndSelectorDialog::~CWndSelectorDialog()
{
	ClearResources();
}

void CWndSelectorDialog::ClearResources()
{
	if (hBitmapFinderToolFilled)
		DeleteObject(hBitmapFinderToolFilled);
	if (hBitmapFinderToolEmpty)
		DeleteObject(hBitmapFinderToolEmpty);
}

LRESULT CWndSelectorDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetBitmap( IDB_BACKTEXTURE);

//	CBitmap	backBitmap;
//	backBitmap.LoadBitmap( MAKEINTRESOURCE( IDB_BACKTEXTURE));

	// Colorize dialog elements
	buttonSet.SubclassWindow( GetDlgItem( IDOK));
	buttonCancel.SubclassWindow( GetDlgItem( IDCANCEL));
	


	staticCaptioSelectAplication.SubclassWindow( GetDlgItem(IDC_STATIC_SELECT_APLICATION));

	//staticCaptioSelectAplication.SetFont();
	staticCaptionSlct.SubclassWindow( GetDlgItem(IDC_STATIC_CAPTION_SELCT));
	staticCaption.SubclassWindow( GetDlgItem(IDC_STATIC_CAPTION));
	staticTitle.SubclassWindow( GetDlgItem(IDC_STATIC_TITLE));
	staticSize.SubclassWindow( GetDlgItem(IDC_STATIC_SIZE));
	//staticBack.SubclassWindow( GetDlgItem(IDC_STATIC_BACK));
	editTitle.SubclassWindow( GetDlgItem(IDC_NAME_EDIT));
	editTitle.SetDisabledColors( RGB(0,0,0), RGB(200,225,255));
	editSize.SubclassWindow( GetDlgItem(IDC_STATIC_SIZEVALUE));
	checkHideMainWnd.SubclassWindow( GetDlgItem(IDC_HIDE_CHECK));
	staticHideClient.SubclassWindow( GetDlgItem(IDC_STATIC_HIDECLIENT));
	buttonSelectFromList.SubclassWindow( GetDlgItem( IDC_BTN_SELECT_FROM_LIST));
	
	//checkHideMainWnd.SetMyTextColor( RGB(255,255,255), RGB(255,255,255));

	if (m_englishVersion)
	{

		SETWTEXT(staticCaption, L"Press an icon with an aim, clamp the left button of the mouse, then drag a sight on a window which want to choose.");
		SETWTEXT(staticHideClient, L"Hide vAcademia");
		SETWTEXT(staticTitle, L"Title: ");
		SETWTEXT(staticSize, L"Size: ");
		SETWTEXT(staticCaptionSlct, L"... or just press the button and select any running application you like to share");
		SETWTEXT(buttonSelectFromList, L"Select");
		SETWTEXT(buttonCancel, L"Cancel");
		SETWTEXT(staticCaptioSelectAplication, L"Window to share");
		::SetWindowText(m_hWnd, "Choose application");
		//staticCaption.SetWindowText(staticCaptionEngText.c_str());
		//staticHideClient.SetWindowText(staticHideClientEngText.c_str());
		//staticTitle.SetWindowText(staticTitleEngText.c_str());
		//staticSize.SetWindowText(staticSizeEngText.c_str());
		//staticCaptionSlct.SetWindowText(staticTitleEngText_Or.c_str());
		//buttonSelectFromList.SetWindowText(btn_CaptionText.c_str());
		//buttonCancel.SetWindowText(btnCaptionCancel.c_str());
		//staticCaptioSelectAplication.SetWindowText(staticCaptionSelectApp.c_str());
	}
	else
	{
		SETWTEXT(staticCaption, L"Нажмите на иконку с прицелом, зажав левую кнопку мыши, затем перетащите прицел на окно, которое хотите выбрать.");
		SETWTEXT(staticHideClient, L"Спрятать окно vAcademia");
		SETWTEXT(staticTitle, L"Название: ");
		SETWTEXT(staticSize, L"Размер: ");
		SETWTEXT(staticCaptionSlct, L"... или нажмите на кнопку для выбора окна из списка запущенных приложений");
		SETWTEXT(buttonSelectFromList, L"Выбрать");
		SETWTEXT(buttonCancel, L"Отмена");
		SETWTEXT(staticCaptioSelectAplication, L"Выбранное приложение");
		::SetWindowText(m_hWnd, "Выбор приложения");







	}
	
	HCURSOR hCursor = LoadCursor( NULL, IDC_HAND);
	SetClassLong(GetDlgItem(IDC_STATIC_FINDER_TOOL),    // window handle 
		GCL_HCURSOR,      // change cursor 
		(LONG) hCursor);   // new cursor 

	//CAxDialogImpl<CWndSelectorDialog>::OnInitDialog(uMsg, wParam, lParam, bHandled);
	buttonSet.EnableWindow( FALSE);
	m_selWindow = NULL;
	m_WndListAplication = NULL;
	if (notepad != false)
	{
		wndSelector->CaptureNotepadIfNeeded();
		//Invoke<&ISelectorWindowEvents::OnSelectorWindowOK>();
	}
	return 1;  // Let the system set the focus
}

void CWndSelectorDialog::StartSelection()
{
	ATLASSERT( !IsRunning());
	if(IsRunning())
	{
		if (notepad != TRUE)
		{ShowWindow(SW_SHOW);};
	}
	else
	{
		Run();	
	}
}

void CWndSelectorDialog::SetFinderToolImage (bool bSet)
{
	HBITMAP hBitmapToSet = NULL;
	if (bSet)
	{
		hBitmapToSet = hBitmapFinderToolFilled;		
	}
	else
	{
		hBitmapToSet = hBitmapFinderToolEmpty;
	}
	/*this->SendDlgItemMessage( 
		(int)IDC_STATIC_FINDER_TOOL, // identifier of control 
		(UINT)STM_SETICON, // message to send 
		(WPARAM)hIconToSet, // first message parameter 
		(LPARAM)0L // second message parameter 
		);*/

	this->SendDlgItemMessage( 
		(int)IDC_STATIC_FINDER_TOOL, // identifier of control 
		(UINT)STM_SETIMAGE, // message to send 
		(WPARAM)IMAGE_BITMAP, // first message parameter 
		(LPARAM)hBitmapToSet // second message parameter 
		);
}


LRESULT CWndSelectorDialog::OnStnClickedStaticFinderTool(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SetFinderToolImage(false);
	m_hPrevCursor =  SetCursor(m_hCursor);
	wndSelector->FromWindow(this->m_hWnd);
	wndSelector->StartSelection();
	return 0;
}



LRESULT CWndSelectorDialog::OnLButtonUp(UINT uMsg, WPARAM wParam, 
										LPARAM lParam, BOOL& bHandled)
{
	wndSelector->StopSelection();
	SetFinderToolImage (true);
	if(m_selWindow != NULL)
		buttonSet.EnableWindow( TRUE);
	
	return 0;
}

void CWndSelectorDialog::OnProcessListCancelled()
{
	EnableWindow( TRUE);
}

void CWndSelectorDialog::OnProcessWindowSelected(HWND ahWnd)
{

//HWND bhWnd;
//bhWnd = FindWindow(NULL, "Безымянный - Блокнот");

	if( ::IsWindow( ahWnd))
	{
		m_selWindow = ahWnd;
		SetSizeEdit(ahWnd);
		SetNameEdit(ahWnd);
		wndSelector->Set_g_hwndFoundWindow(m_selWindow);
		buttonSet.EnableWindow( TRUE);
	}
	EnableWindow( TRUE);
	
}

LRESULT CWndSelectorDialog::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{

	if(wndSelector && wndSelector->IsSelecting())
	{
		HWND fWindow = wndSelector->DoMouseMove();
		CWindow myWindow = fWindow;
		m_selWindow = fWindow;
		SetSizeEdit(fWindow);
		SetNameEdit(fWindow);
	}

	return 0;
}

bool CWndSelectorDialog::GetWindowSize(RECT &rec, int &width, int &height)
{
	width = rec.right - rec.left;
	height = rec.bottom - rec.top;
	return (width > 0 && height > 0);
}

void CWndSelectorDialog::SetSizeEdit(HWND hwnd)
{
	if (hwnd)
	{   ///IsWindow()
		CWindow myWindow = hwnd;
		RECT rec;
		myWindow.GetWindowRect(&rec);
		int width, height;
		if (GetWindowSize(rec, width, height))
		{
			CString str;
			str.Format("%d x %d", width, height);
			m_sizeStaticStr = str; 
			editSize.SetWindowText(str);
		}
		DoDataExchange(FALSE);
		
	}
}

HWND CWndSelectorDialog::FindParentWindow(HWND hChild)
{
	HWND hParent = NULL;
	HWND hResult = NULL;
	hParent = ::GetParent(hChild);
	if (hParent != NULL)
		hResult = FindParentWindow(hParent);
	if (hResult == NULL)
		return hChild;
	else 
		return hResult;
}

void CWndSelectorDialog::SetNameEdit(HWND hwnd)
{
	if (hwnd)
	{
		CComString text;
		CWindow myWindow = hwnd;

		int len = myWindow.GetWindowTextLength();
		if(myWindow.GetWindowText(text.GetBufferSetLength(len+1), len+1))
		{
			m_nameEditStr = text.GetBuffer();
			ATLTRACE("Window text = %s\n", text.GetBuffer());
			DoDataExchange(FALSE);
		}
		else
		{
			HWND parentHwnd = FindParentWindow(hwnd);
			if (parentHwnd != hwnd)
				SetNameEdit(parentHwnd);
			else
				m_nameEditStr = "";
		}
	}
}

DWORD CWndSelectorDialog::Run()
{
	// Создаем главное окно 
	//if (this->Create(NULL, NULL) == NULL)
	if (this->Create((HWND)(context->mpWindow->GetMainWindowHandle()), NULL) == NULL)
	{
		ATLTRACE(_T("WndSelector dilog window creation failed!\n"));
		return 0;
	}
	
	//hIconFinderToolFilled =  AtlLoadIconImage(MAKEINTRESOURCE(IDI_FINDER_FILLED));
	//hIconFinderToolEmpty = AtlLoadIconImage(MAKEINTRESOURCE(IDI_FINDER_EMPTY));

	hBitmapFinderToolFilled = AtlLoadBitmap(MAKEINTRESOURCE(IDB_BITMAP_FILLER));
	hBitmapFinderToolEmpty = AtlLoadBitmap(MAKEINTRESOURCE(IDB_BITMAP_FILLEREMPTY));

	m_hCursor = AtlLoadCursor(IDC_CURSOR_SEARCH_WINDOW);
if (notepad != TRUE)
{ShowWindow(SW_SHOW);}

	return 1;
}

void CWndSelectorDialog::ReleaseWindow()
{
	ClearResources();
	if( IsWindow())
	{
		DestroyWindow();
	}
}
LRESULT CWndSelectorDialog::OnBnClickedHideCheck(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (m_bHide)
	{
		ShowMainWindow();
	}
	else
	{
		HideMainWindow();
	}
	DoDataExchange(TRUE);
	return 0;
}

bool CWndSelectorDialog::ShowMainWindow()
{
	HWND hMainWnd = wndSelector->GetMainWindowHandle();
	if (hMainWnd)
	{
		CWindow myWindow = hMainWnd;
		if (!myWindow.IsWindowVisible())
		{
			myWindow.SetWindowPos(HWND_TOP, &m_mainWndRec, SWP_SHOWWINDOW);
		}
		return true;
	}
	return false;
}

bool CWndSelectorDialog::HideMainWindow()
{
	if( !wndSelector)
		return false;
	HWND hMainWnd = wndSelector->GetMainWindowHandle();
	if (hMainWnd)
	{
		CWindow myWindow = hMainWnd;
		myWindow.GetWindowRect(&m_mainWndRec);
		myWindow.SetWindowPos(HWND_TOP, 0, 0, 0, 0, SWP_HIDEWINDOW);
		return true;
	}
	return false;
}
LRESULT CWndSelectorDialog::OnStnClickedStaticHideclient(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here

	return 0;
}
LRESULT CWndSelectorDialog::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
/*	buttonSet.DestroyWindow();
	buttonCancel.DestroyWindow();
	staticCaption.DestroyWindow();
	staticTitle.DestroyWindow();
	staticSize.DestroyWindow();
	staticHideClient.DestroyWindow();
	editSize.DestroyWindow();
	editTitle.DestroyWindow();
	checkHideMainWnd.DestroyWindow();
*/
	bHandled = FALSE;
	return 0;
}

void CWndSelectorDialog::OnFinalMessage(HWND /*hWnd*/)
{
	MP_DELETE_THIS;
}

LRESULT CWndSelectorDialog::OnBnClickedBtnSelectFromList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(m_WndListAplication == NULL)
	{
		MP_NEW_V2( m_WndListAplication, CProcessListDlg, m_englishVersion, this);
		m_WndListAplication->Create(m_hWnd, NULL);
	}
	m_WndListAplication->CompileList();
	RECT rcMine;
	GetWindowRect( &rcMine);

	RECT rcList;
	m_WndListAplication->GetWindowRect( &rcList);

	bool	abPlaceOnRight = true;
	CWndSelector::vecMonitorsInfo monitorsInfo;
	EnumDisplayMonitors( 0, NULL, MonitorEnumProcWnd, (LPARAM)&monitorsInfo);

	CWndSelector::vecMonitorsInfo::iterator	it = monitorsInfo.begin();
	for (; it != monitorsInfo.end(); it++)
	{
		if (rcMine.left >= (*it).rcMonitor.left && rcMine.left <= (*it).rcMonitor.right
			&& rcMine.top >= (*it).rcMonitor.top && rcMine.top <= (*it).rcMonitor.bottom)
		{
			int listWidth = rcList.right - rcList.left;

			if( rcMine.right + listWidth > (*it).rcMonitor.right)
			{
				abPlaceOnRight = false;
			}
			break;
		}
	}

	int mineWidth = rcMine.right - rcMine.left;
	if( abPlaceOnRight)
		m_WndListAplication->SetWindowPos( 0, rcMine.left + mineWidth, rcMine.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	else
		m_WndListAplication->SetWindowPos( 0, rcMine.left - (rcList.right-rcList.left), rcMine.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
if (notepad != TRUE)
{m_WndListAplication->ShowWindow(SW_SHOW);}
	EnableWindow(FALSE);

    /*wndSelector->Set_g_hwndFoundWindow(m_WndListAplication->m_MainWndProc);
	FillWNDInfo(m_WndListAplication->m_MainWndProc); 

	SAFE_DELETE (m_WndListAplication);*/
	return 0;}