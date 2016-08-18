#pragma once

#include "Dialog/DialogLayout.h"
#include "resource.h"

class CBaseDialogTab
{
public:
	CBaseDialogTab(){};
	virtual ~CBaseDialogTab(){};

	//enum { IDD = IDD_BASEDIALOG};
	//enum { IDD = IDD_OBJECTVIEW};

public:
	virtual LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){ return TRUE;};
	virtual LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){ 
		return TRUE;
	};
	virtual LRESULT OnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){ return TRUE;};
	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){ return TRUE;};
	virtual LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){ return TRUE;};
	virtual LRESULT OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){ return TRUE;};

};