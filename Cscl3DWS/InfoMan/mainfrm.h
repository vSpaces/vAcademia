// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
//#include "atlapp.h"
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlmisc.h>

#include "resource.h"
#include "aboutdlg.h"
#include "TabViewWnd.h"

#include "ObjectTab.h"
#include "LogTab.h"
#include "ScriptExecuteTab.h"
#include "SettingsTab.h"
#include "ScriptTestTab.h"
#include "SharingTab.h"
#include "SocketTab.h"
#include "RecordTab.h"
#include "ResourceLoadTab.h"
#include "view.h"

class CMainFrame : public CFrameWindowImpl<CMainFrame>, public CUpdateUI<CMainFrame>
{
public:

	CMainFrame();
	~CMainFrame()
	{
		m_hWnd = NULL;
	};

	void SetContext( oms::omsContext *apContext);
	void SetInfoManager(IExternalLogWriter* apInfoManager);
	void Trace(const std::string& text, LOG_TYPE logType);
	void TraceTestResult(char* resStr);
	inline void ShowObject(const rmml::mlString& aTarget) { mp_viewObject->ShowObject(aTarget);}
	void TraceSessionPacketInfo(std::string& asSessionName, BYTE packetType, DWORD size, bool isOut);
	virtual void OnFinalMessage(HWND hWnnd);
	void StartAutoTesting();

	enum { IDD = IDR_MAINFRAME };
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

private:
	CObjectTab *mp_viewObject;
	CLogTab *mp_viewLog;
	CScriptExecuteTab* mp_viewScriptExecute;
	CSettingsTab* mp_viewSettings;
	CView m_view;
	CTabViewWnd	m_TabViewWnd;
	CScriptTestTab* mp_viewScriptTest;
	CSharingTab* mp_viewSharingState;
	CRecordTab* mp_viewRecordState;
	CSocketTab* mp_viewSocketState;
	CResourceLoadTab* mp_viewResourceLoad;

	CCommandBarCtrl m_CmdBar;
	oms::omsContext *pContext;
	IExternalLogWriter* mpInfoManager;

	DWORD updateTime;

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_EDIT_PASTE, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
	END_UPDATE_UI_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();
	virtual void Update();

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClose( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};