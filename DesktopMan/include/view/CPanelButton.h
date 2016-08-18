// CPanelButton.h : Declaration of the CCPanelButton

#pragma once

#include "../../resource.h"       // main symbols
#include <atlhost.h>


// CCPanelButton

class CCPanelButton : 
	public CAxDialogImpl<CCPanelButton>
{
public:
	CCPanelButton()
	{
	}

	~CCPanelButton()
	{
	}

	enum { IDD = IDD_BUTTONPANEL };

BEGIN_MSG_MAP(CCPanelButton)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
	COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
	CHAIN_MSG_MAP(CAxDialogImpl<CCPanelButton>)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CAxDialogImpl<CCPanelButton>::OnInitDialog(uMsg, wParam, lParam, bHandled);
		return 1;  // Let the system set the focus
	}

	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		CWindow wnd = GetParent();
		if (wnd)
			wnd.ShowWindow(SW_HIDE);
		//ShowWindow(SW_HIDE);
		return 0;
	}

	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		CWindow wnd = GetParent();
		if (wnd)
			wnd.ShowWindow(SW_HIDE);
		return 0;
	}
};


