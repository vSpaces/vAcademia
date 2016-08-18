#include "StdAfx.h"
#include "mainfrm.h"

CMainFrame::CMainFrame()
{
	pContext = NULL;
	updateTime = GetTickCount();
	mp_viewObject = NULL;
	mp_viewLog = NULL;
	mp_viewScriptExecute = NULL;
	mp_viewSettings = NULL;
	mp_viewScriptTest = NULL;
	mp_viewSharingState = NULL;
	mp_viewSocketState = NULL;
	mp_viewResourceLoad = NULL;
	mpInfoManager = NULL;
}

BOOL CMainFrame::PreTranslateMessage( MSG* pMsg )
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	return m_TabViewWnd.PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	return FALSE;
}

void CMainFrame::Update()
{
	if ( (GetTickCount() - updateTime) > 1000)
	{
		updateTime = GetTickCount();
		mp_viewSocketState->Update();
		mp_viewRecordState->Update();
	}
}

LRESULT CMainFrame::OnCreate( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
{
	CDialogTemplate::SetResourceInstance( (HINSTANCE)_Module.m_hInstResource);

	m_hWndClient = m_TabViewWnd.Create( m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE, WS_EX_STATICEDGE );

	mp_viewObject = MP_NEW( CObjectTab);
	mp_viewObject->SetContext( pContext);
	HWND hWndTemp = mp_viewObject->Create( m_TabViewWnd);
	m_TabViewWnd.AddTab( "Objects", hWndTemp);

	mp_viewLog = MP_NEW( CLogTab);
	mp_viewLog->SetContext( pContext);
	mp_viewLog->SetInfoManager( mpInfoManager);
	HWND hWndLog = mp_viewLog->Create(m_TabViewWnd);
	m_TabViewWnd.AddTab( "Log", hWndLog, FALSE);

	mp_viewScriptExecute = MP_NEW( CScriptExecuteTab);
	mp_viewScriptExecute->SetContext( pContext);
	HWND hWndScriptExecute = mp_viewScriptExecute->Create(m_TabViewWnd);
	m_TabViewWnd.AddTab( "Script Execute", hWndScriptExecute, FALSE);

	mp_viewScriptTest = MP_NEW( CScriptTestTab);
	mp_viewScriptTest->SetContext( pContext);
	HWND hWndScriptTest = mp_viewScriptTest->Create(m_TabViewWnd);
	m_TabViewWnd.AddTab( "Script Test", hWndScriptTest, FALSE);

	mp_viewSharingState = MP_NEW( CSharingTab);
	mp_viewSharingState->SetContext( pContext);
	HWND hWndViewSharingState = mp_viewSharingState->Create(m_TabViewWnd);
	m_TabViewWnd.AddTab( "Sharing view", hWndViewSharingState, FALSE);

	mp_viewSocketState = MP_NEW( CSocketTab);
	mp_viewSocketState->SetContext( pContext);
	HWND hWndViewSocketSessionsState = mp_viewSocketState->Create(m_TabViewWnd);
	m_TabViewWnd.AddTab( "Socket view", hWndViewSocketSessionsState, FALSE);

	mp_viewRecordState = MP_NEW( CRecordTab);
	mp_viewRecordState->SetContext( pContext);
	HWND hWndViewRecordSessionsState = mp_viewRecordState->Create(m_TabViewWnd);
	m_TabViewWnd.AddTab( "Record view", hWndViewRecordSessionsState, FALSE);

	mp_viewResourceLoad = MP_NEW( CResourceLoadTab);
	mp_viewResourceLoad->SetContext( pContext);
	HWND hWndViewResourceSessionsState = mp_viewResourceLoad->Create(m_TabViewWnd);
	m_TabViewWnd.AddTab( "Resource load", hWndViewResourceSessionsState, FALSE);

	UIEnable(ID_EDIT_PASTE, FALSE);

	return 0;
}

LRESULT CMainFrame::OnClose( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
{
	ShowWindow( SW_HIDE);
	return 0;
}

void CMainFrame::OnFinalMessage(HWND hWnnd)
{
	MP_DELETE_THIS;
}

void CMainFrame::SetContext( oms::omsContext *apContext )
{
	pContext = apContext;
}

void CMainFrame::SetInfoManager( IExternalLogWriter* apInfoManager )
{
	mpInfoManager = apInfoManager;
}

void CMainFrame::Trace( const std::string& text, LOG_TYPE logType )
{
//	mp_viewLog->Trace( text, logType);
}

void CMainFrame::TraceTestResult(char* resStr)
{
	if( mp_viewSocketState)
		mp_viewScriptTest->Trace(resStr);
}

void CMainFrame::TraceSessionPacketInfo(std::string& asSessionName, BYTE packetType, DWORD size, bool isOut)
{
	if( mp_viewSocketState)
		mp_viewSocketState->TraceSessionPacketInfo(asSessionName, packetType, size, isOut);
}

void CMainFrame::StartAutoTesting()
{
	m_TabViewWnd.SetActiveTab( 3);
	BOOL bHandled = true;
	if( mp_viewSocketState)
		mp_viewScriptTest->StartAutoTesting();
	//mp_viewScriptTest->OnBnClickedButtonSelect(0, 0, 0, bHandled);
	//mp_viewScriptTest->OnBnClickedButtonStart( 0, 0, 0, bHandled);
}
