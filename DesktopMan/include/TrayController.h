#pragma once

#include "trayiconimpl.h"
#include "CoolContextMenu.h"
#include "WTLTrayIconWindowview.h"

class CDesktopManager;

class CTrayController :
	public CFrameWindowImpl<CTrayController>,
	public CMessageFilter,
	public CTrayIconImpl<CTrayController>,
	public ViskoeThread::CThreadImpl<CTrayController>,
	public CCoolContextMenu<CTrayController>
{

public:
	CTrayController(CDesktopManager* apManager);
	virtual ~CTrayController();

public:
	void	ShowTrayIcon();
	void	HideTrayIcon();
	
	// реализация CThreadImpl<nrmPreviewRecorder>
public:
	DWORD Run();

public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	BEGIN_MSG_MAP(CCaptureWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		COMMAND_ID_HANDLER(ID_DEFINE_REGION, OnDefineRegion)
		COMMAND_ID_HANDLER(ID_COPY_SELECTION, OnRecapSelection)
		CHAIN_MSG_MAP(CFrameWindowImpl<CTrayController>)
		CHAIN_MSG_MAP(CTrayIconImpl<CTrayController>)
		CHAIN_MSG_MAP(CCoolContextMenu<CTrayController>)
	END_MSG_MAP()

public:
	// Called prior to displaying the menu
	void PrepareMenu(HMENU hMenu);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void	AssociateImage(CMenuItemInfo& mii, MenuItemData * pMI);

protected:
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDefineRegion(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnRecapSelection(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
	CDesktopManager*	pManager;
	bool				iconNeedBeShown;
	bool				iconIsShown;
	HICON				hTrayIconSmall;
};