// SharingTab.h : Declaration of the CSharingTab

#pragma once

#include "Dialog/DialogLayout.h"
#include ".\basedialogtab.h"
#include "resource.h"       // main symbols
#include "ILogWriter.h"


// CSharingTab

class CSharingTab : 
	public CBaseDialogTab,
	public CDialogImpl<CSharingTab>,
	public CUpdateUI<CSharingTab>,
	public CDialogLayout<CSharingTab>
{
public:
	CSharingTab()
	{
	}

	~CSharingTab()
	{
	}

	enum { IDD = IDD_SHARINGTAB };

BEGIN_LAYOUT_MAP()
	LAYOUT_CONTROL(IDC_TREE_SHARING_SETTINGS, LAYOUT_ANCHOR_LEFT | LAYOUT_ANCHOR_TOP | LAYOUT_ANCHOR_BOTTOM )
	LAYOUT_CONTROL(IDC_SESSION_VIEW, LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_LEFT | LAYOUT_ANCHOR_TOP | LAYOUT_ANCHOR_BOTTOM)
	LAYOUT_CONTROL(IDC_BUTTON1, LAYOUT_ANCHOR_BOTTOM )
	LAYOUT_CONTROL(IDC_BUTTON2, LAYOUT_ANCHOR_BOTTOM )
	LAYOUT_CONTROL(IDC_BUTTON3, LAYOUT_ANCHOR_BOTTOM)
END_LAYOUT_MAP()

BEGIN_UPDATE_UI_MAP(CSharingTab)
END_UPDATE_UI_MAP()

BEGIN_MSG_MAP(CSharingTab)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
	COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
	NOTIFY_HANDLER(IDC_TREE_SHARING_SETTINGS, TVN_SELCHANGED, OnTvnSelchangedTreeSharingSettings)
	COMMAND_HANDLER(IDC_BUTTON1, BN_CLICKED, OnBnClickedButtonUpdateTree)
	COMMAND_HANDLER(IDC_BUTTON2, BN_CLICKED, OnBnClickedButtonUpdateInfo)
	COMMAND_HANDLER(IDC_BUTTON3, BN_CLICKED, OnBnClickedButtonShow)
	CHAIN_MSG_MAP(CDialogLayout<CSharingTab>)
END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		return 0;
	}
private:
	oms::omsContext *mpOmsContext;
	CTreeViewCtrl m_testTree;
	CEdit m_resultsEdit;
	char* currentSession;
	bool m_building;
public:
	void SetContext( oms::omsContext *apContext);
	void BuildTree();
	HTREEITEM AddItem(char* data, char* txt, HTREEITEM hRoot);
	HTREEITEM AddRoot(const char* txt);
	LRESULT OnTvnSelchangedTreeSharingSettings(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonUpdateTree(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonUpdateInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonShow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};


