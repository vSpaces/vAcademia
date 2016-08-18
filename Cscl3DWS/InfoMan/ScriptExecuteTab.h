#pragma once

#include "Dialog/DialogLayout.h"
#include ".\basedialogtab.h"
#include "ILogWriter.h"
#include <map>
//#include <afxrich.h>

class CScriptExecuteTab :
	public CBaseDialogTab,
	public CDialogImpl<CScriptExecuteTab>,
	public CUpdateUI<CScriptExecuteTab>,
	public CDialogLayout<CScriptExecuteTab>
{
public:
	CScriptExecuteTab();
	virtual ~CScriptExecuteTab();

	enum { IDD = IDD_SCRIPT_EXECUTE};

	BEGIN_LAYOUT_MAP()
		LAYOUT_CONTROL(IDC_EDIT_SCRIPT, LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_LEFT | LAYOUT_ANCHOR_TOP | LAYOUT_ANCHOR_BOTTOM)
		LAYOUT_CONTROL(IDC_BUTTON_EXECUTE, LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_BOTTOM )
	END_LAYOUT_MAP()

public:
	BEGIN_UPDATE_UI_MAP(CScriptExecuteTab)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CScriptExecuteTab)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_BUTTON_EXECUTE, BN_CLICKED, OnBnClickedButtonExecute)
		CHAIN_MSG_MAP(CDialogLayout<CScriptExecuteTab>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

protected:
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

protected:
	CEdit	m_EditScript;
	CButton	m_ButtonExecute;

	oms::omsContext *mpOmsContext;

public:
	LRESULT OnBnClickedButtonExecute(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void SetContext( oms::omsContext *apContext)
	{
		mpOmsContext = apContext;
	}

};