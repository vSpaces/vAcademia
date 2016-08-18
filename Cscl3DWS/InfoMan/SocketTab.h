// CSocketTab.h : Declaration of the CSocketTab

#pragma once

#include "Dialog/DialogLayout.h"
#include ".\basedialogtab.h"
#include "resource.h"       // main symbols
#include "ILogWriter.h"

#define SESSION_MAP_SIZE 256
#define SESSION_TYPE_SIZE 2

struct SessionTypeMap
{
	MP_STRING_DC typeMap[2][256];

	SessionTypeMap();
};

typedef MP_MAP<std::string, SessionTypeMap*> SessionsTypeListMap;

// CSocketTab

class CSocketTab :
	public CBaseDialogTab,
	public CDialogImpl<CSocketTab>,
	public CUpdateUI<CSocketTab>,
	public CDialogLayout<CSocketTab>
{
public:
	CSocketTab();
	~CSocketTab()
	{
	}

	enum { IDD = IDD_NET_SESSIONS };

	BEGIN_LAYOUT_MAP()
		LAYOUT_CONTROL(IDC_SOCKET_SESSION_VIEW, LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_LEFT | LAYOUT_ANCHOR_TOP | LAYOUT_ANCHOR_BOTTOM)
		LAYOUT_CONTROL(IDC_BUTTON_UPDATE_SESSIONS, LAYOUT_ANCHOR_BOTTOM )
	END_LAYOUT_MAP()

	BEGIN_UPDATE_UI_MAP(CSocketTab)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CSocketTab)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_BUTTON_UPDATE_SESSIONS, BN_CLICKED, OnBnClickedStartWriteLog)
		CHAIN_MSG_MAP(CDialogLayout<CSocketTab>)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	oms::omsContext *mpOmsContext;
	CTreeViewCtrl m_testTree;
	CEdit m_resultsEdit;
	CButton m_startStopWriteLogBtn;
	char* currentSession;
	bool m_building;
	bool m_writeLog;
public:
	void SetContext( oms::omsContext *apContext);
	void BuildTree();
	HTREEITEM AddItem(char* data, char* txt, HTREEITEM hRoot);
	HTREEITEM AddRoot(const char* txt);
	LRESULT OnBnClickedStartWriteLog(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void Update();
	void TraceSessionPacketInfo(std::string& asSessionName, BYTE packetType, DWORD size, bool isOut);

	SessionTypeMap syncServerTypeMap;
	SessionTypeMap serviceServerTypeMap;

	SessionTypeMap resTypeMap;
	SessionTypeMap oResXMLTypeMap;
	SessionTypeMap oResUploadSocketTypeMap;
	SessionTypeMap serviceWorldServerTypeMap;
	SessionTypeMap voipRecordServerTypeMap;
	SessionTypeMap VoipPlayServerTypeMap;
	SessionTypeMap PreviewServerTypeMap;
	SessionTypeMap UDPServerTypeMap;
	SessionTypeMap sharingServerTypeMap;

	SessionsTypeListMap typeListMap;

	CRITICAL_SECTION csTypeListMap;
};


