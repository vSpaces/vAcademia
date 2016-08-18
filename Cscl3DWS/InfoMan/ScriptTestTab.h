// ScriptTestTab.h : Declaration of the CScriptTestTab

#pragma once

#include "Dialog/DialogLayout.h"
#include ".\basedialogtab.h"
#include "resource.h"       // main symbols
#include "ILogWriter.h"

// CScriptTestTab
#define UM_CHECKSTATECHANGE (WM_USER + 100)

class CScriptTestTab : 
	public CBaseDialogTab,
	public CDialogImpl<CScriptTestTab>,
	public CUpdateUI<CScriptTestTab>,
	public CDialogLayout<CScriptTestTab>
{
public:
	CScriptTestTab()
	{
	}

	virtual ~CScriptTestTab();

	enum { IDD = IDD_SCRIPTTESTTAB };

	BEGIN_LAYOUT_MAP()
		LAYOUT_CONTROL(IDC_TEST_RESULT, LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_LEFT | LAYOUT_ANCHOR_TOP | LAYOUT_ANCHOR_BOTTOM)
		LAYOUT_CONTROL(IDC_BUTTON_START, LAYOUT_ANCHOR_BOTTOM )
		LAYOUT_CONTROL(IDC_BUTTON_STOP, LAYOUT_ANCHOR_BOTTOM )
		LAYOUT_CONTROL(IDC_TREE_TEST, LAYOUT_ANCHOR_LEFT | LAYOUT_ANCHOR_VERTICAL )
		LAYOUT_CONTROL(IDC_BUTTON_SELECT, LAYOUT_ANCHOR_BOTTOM )
		LAYOUT_CONTROL(IDC_BUTTON_DESELECT, LAYOUT_ANCHOR_BOTTOM )
		LAYOUT_CONTROL(IDC_BUTTON_GETLIST, LAYOUT_ANCHOR_BOTTOM )
	END_LAYOUT_MAP()

	BEGIN_UPDATE_UI_MAP(CScriptTestTab)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CScriptTestTab)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		MESSAGE_HANDLER(UM_CHECKSTATECHANGE, OnCheckBoxChanged)
		COMMAND_HANDLER(IDC_BUTTON_START, BN_CLICKED, OnBnClickedButtonStart)
		COMMAND_HANDLER(IDC_BUTTON_STOP, BN_CLICKED, OnBnClickedButtonStop)
		COMMAND_HANDLER(IDC_BUTTON_SELECT, BN_CLICKED, OnBnClickedButtonSelect)
		COMMAND_HANDLER(IDC_BUTTON_DESELECT, BN_CLICKED, OnBnClickedButtonDeselect)
		COMMAND_HANDLER(IDC_BUTTON_GETLIST, BN_CLICKED, OnBnClickedButtonGetList)
		CHAIN_MSG_MAP(CDialogLayout<CScriptTestTab>)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	CButton m_startButton;
	CButton m_stopButton;
	CEdit m_resultsEdit;
	CTreeViewCtrl m_testTree;

	oms::omsContext *mpOmsContext;

public:
	void SetContext( oms::omsContext *apContext);
	void Trace(char* resStr);
	void SelectAllItems();
	void DeSelectAllItems();

	LRESULT OnBnClickedButtonStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	void BuildTestTree();
	wchar_t* GetModuleNames();
	wchar_t* GetTestsByModule(const char* moduleName);
	HTREEITEM AddRoot(const char* txt);
	HTREEITEM AddItem(char* data, char* txt, HTREEITEM hRoot);
	LRESULT OnBnClickedButtonStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCheckBoxChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnBnClickedButtonSelect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonDeselect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	void StartAutoTesting();
	static DWORD WINAPI startAutoTestingThread_(LPVOID param);
	LRESULT OnBnClickedButtonGetList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};


