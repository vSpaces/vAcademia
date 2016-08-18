// RecordTab.h : Declaration of the CRecordTab

#pragma once

#include "Dialog/DialogLayout.h"
#include ".\basedialogtab.h"
#include "resource.h"       // main symbols
#include "ILogWriter.h"
#include "..\SyncMan\SyncMan.h"
#include "Mutex.h"

// CRecordTab

typedef MP_VECTOR<syncRecordExtendedInfo> syncRecordExtendedInfoVec;
typedef MP_VECTOR<syncRecordExtendedInfo>::iterator syncRecordExtendedInfoVecIter;

class CRecordTab : 
	public CBaseDialogTab,
	public CDialogImpl<CRecordTab>,
	public CUpdateUI<CRecordTab>,
	public CDialogLayout<CRecordTab>,
	public sync::syncIRecordManagerCallback2
{
public:
	CRecordTab();

	~CRecordTab()
	{
	}

	enum { IDD = IDD_RECORDTAB };

BEGIN_LAYOUT_MAP()
	LAYOUT_CONTROL(IDC_LIST_RECORD, LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_LEFT | LAYOUT_ANCHOR_TOP | LAYOUT_ANCHOR_BOTTOM)
	//LAYOUT_CONTROL(IDC_LIST_RECORD2, LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_LEFT | LAYOUT_ANCHOR_TOP | LAYOUT_ANCHOR_BOTTOM)
	LAYOUT_CONTROL(IDC_RECORD_VIEW, LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_LEFT | LAYOUT_ANCHOR_TOP | LAYOUT_ANCHOR_BOTTOM)
	LAYOUT_CONTROL(IDC_BUTTON_RL, LAYOUT_ANCHOR_BOTTOM )
	LAYOUT_CONTROL(IDC_BUTTON_RI, LAYOUT_ANCHOR_BOTTOM )
	LAYOUT_CONTROL(IDC_BUTTON_EX, LAYOUT_ANCHOR_BOTTOM )
	LAYOUT_CONTROL(IDC_BUTTON_IM, LAYOUT_ANCHOR_BOTTOM )
END_LAYOUT_MAP()

BEGIN_UPDATE_UI_MAP(CRecordTab)
END_UPDATE_UI_MAP()

BEGIN_MSG_MAP(CRecordTab)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
	COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
	COMMAND_HANDLER(IDC_BUTTON_RL, BN_CLICKED, OnBnClickedButtonUpdateTree)
	COMMAND_HANDLER(IDC_BUTTON_RI, BN_CLICKED, OnBnClickedButtonUpdateInfo)
	COMMAND_HANDLER(IDC_BUTTON_EX, BN_CLICKED, OnBnClickedButtonExport)
	COMMAND_HANDLER(IDC_BUTTON_IM, BN_CLICKED, OnBnClickedButtonImport)
	//COMMAND_HANDLER(IDC_LIST_RECORD, LBN_SELCHANGE, OnLbnSelchangeListRecord)
	COMMAND_HANDLER(IDC_LIST_RECORD, LBN_DBLCLK, OnLbnDblclkListRecord)
	//NOTIFY_HANDLER(IDC_LIST_RECORD2, NM_DBLCLK, OnNMDblclkListRecord2)
	CHAIN_MSG_MAP(CDialogLayout<CRecordTab>)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		//EndDialog(wID);
		return 0;
	}

	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
	//	EndDialog(wID);
		return 0;
	}
private:
	oms::omsContext *mpOmsContext;
	CListBox m_ListBox;
	//CListViewCtrl m_ListCtrl;
	CEdit m_resultsEdit;
	//char* currentSession;
	bool m_building;
	int beginRecordRequest;
	CMutex mutex;
	syncRecordExtendedInfoVec v_syncRecordExtendedInfo;
public:
	void SetContext( oms::omsContext *apContext);
	void GetRecords();
	void Update();
	//AddItem(char* data, char* txt, HTREEITEM hRoot);
	//HTREEITEM AddRoot(const char* txt);
	//LRESULT OnTvnSelchangedTreeRecordSettings(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonUpdateTree(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonUpdateInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonExport(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonImport(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

public: // реализация интерфейса sync::syncIRecordManagerCallback
	void onReceivedRecordsExtendedInfo( unsigned int totalCount, unsigned int auRecordCount, syncRecordExtendedInfo** appRecordsInfo, unsigned int auID, int aiErrorCode);
	// с сервера получена полная информация о записи
	//void onReceivedFullRecordInfo(unsigned int aiRecordObjectID,  syncRecordInfo* appRecordInfo, const wchar_t* aOutsideUrl, int aiErrorCode);
	LRESULT OnLbnSelchangeListRecord(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnLbnDblclkListRecord(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnNMDblclkListRecord2(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
};


