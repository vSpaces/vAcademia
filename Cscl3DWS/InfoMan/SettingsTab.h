#pragma once

#include "Dialog/DialogLayout.h"
#include ".\basedialogtab.h"
#include "ILogWriter.h"
#include <map>
//#include <afxrich.h>

class CSettingsTab :
	public CBaseDialogTab,
	public CDialogImpl<CSettingsTab>,
	public CUpdateUI<CSettingsTab>,
	public CDialogLayout<CSettingsTab>
{
public:
	CSettingsTab();
	virtual ~CSettingsTab();

	enum { IDD = IDD_SETTINGS};

	BEGIN_LAYOUT_MAP()
		LAYOUT_CONTROL(IDC_SHOW_HIDDEN_3DO, LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_LEFT | LAYOUT_ANCHOR_TOP | LAYOUT_ANCHOR_BOTTOM)
	END_LAYOUT_MAP()

public:
	BEGIN_UPDATE_UI_MAP(CSettingsTab)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CSettingsTab)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_SHOW_HIDDEN_3DO, BN_CLICKED, OnBnClickedShowHidden3DO)
		CHAIN_MSG_MAP(CDialogLayout<CSettingsTab>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

protected:
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);


protected:
	CButton	m_showHidden3DO;

	oms::omsContext *mpOmsContext;

public:
	LRESULT OnBnClickedShowHidden3DO(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void SetContext( oms::omsContext *apContext)
	{
		mpOmsContext = apContext;
	}

};