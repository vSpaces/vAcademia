#include "StdAfx.h"
#include <windowsx.h>
#include "..\include\TrayController.h"
#include "..\include\DesktopMan.h"

CTrayController::CTrayController(CDesktopManager* apManager)
{
	pManager = apManager;
	iconNeedBeShown = false;
	iconIsShown = false;
	hTrayIconSmall = 0;
	//Start();
}

CTrayController::~CTrayController()
{
	if( hTrayIconSmall != 0)
		DestroyIcon( hTrayIconSmall);
	m_ImageList.Destroy();
	if (IsWindow())
		DestroyWindow();
	m_hWnd = NULL;
}

void	CTrayController::ShowTrayIcon()
{
	if( iconIsShown)
		return;
	if( !IsWindow())
	{
		iconNeedBeShown = true;
		return;
	}
	if( hTrayIconSmall == 0)
	{
		// Load a small icon to use for the system tray
		hTrayIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(),
			MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON,
			::GetSystemMetrics(SM_CXSMICON),
			::GetSystemMetrics(SM_CYSMICON),
			LR_DEFAULTCOLOR);
	}
	// Install the tray icon
	InstallIcon(_T("WTLTrayIconWindow"), hTrayIconSmall, IDR_POPUP);
	iconNeedBeShown = false;
	iconIsShown = true;
}

void	CTrayController::HideTrayIcon()
{
	if( !iconIsShown)
		return;
	if( !IsWindow())
		return;

	// Install the tray icon
	RemoveIcon();
	iconNeedBeShown = false;
	iconIsShown = false;
}

DWORD CTrayController::Run()
{
	// Создаем главное окно 
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);
	if( CreateEx( NULL, NULL, WS_POPUPWINDOW) == NULL)
	{
		ATLTRACE(_T("CTrayController creation failed!\n"));
		ATLASSERT( FALSE);
		return 0;
	}

	ShowWindow( SW_HIDE);
	int nRet = theLoop.Run();
	_Module.RemoveMessageLoop();
	return nRet;
}

// Called prior to displaying the menu
void CTrayController::PrepareMenu(HMENU hMenu)
{
	CMenuHandle menu(hMenu);
	menu.SetMenuDefaultItem();
}

/*BOOL CTrayController::PreTranslateMessage(MSG* pMsg)
{

}*/

void CTrayController::AssociateImage(CMenuItemInfo& mii, MenuItemData * pMI)
{
	switch (mii.wID)
	{
	case ID_DEFINE_REGION:
		pMI->iImage = 0;
		break;
	case ID_COPY_SELECTION:
		pMI->iImage = 1;
		break;
	default:
		pMI->iImage = -1;
		break;
	}
}

LRESULT CTrayController::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	//pLoop->AddIdleHandler(this);

	CCoolContextMenu<CTrayController>::GetSystemSettings();
	COLORREF crMask = CLR_DEFAULT;
	HIMAGELIST hImageList = ImageList_LoadImage(ATL::_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDR_POPUP_IMAGES), 16, 1, crMask, IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
	ATLASSERT(hImageList != NULL);
	m_ImageList.Attach(hImageList);

	if( iconNeedBeShown)
		ShowTrayIcon();

	return 0;
}

LRESULT CTrayController::OnDefineRegion(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	pManager->StartDefineRegion();
	return 0;
}

LRESULT CTrayController::OnRecapSelection(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	pManager->RecapSelection();
	return 0;
}


/************************************************************************/
/* Protected
/************************************************************************/
BOOL CTrayController::PreTranslateMessage(MSG* pMsg)
{
	return CFrameWindowImpl<CTrayController>::PreTranslateMessage(pMsg);}