#include "StdAfx.h"
#include "ILogWriter.h"
#include "InfoManager.h"
#include "SimpleThreadStarter.h"
//#include "DataBuffer2.h"
//#include "UrlParser.h"
#include "atltime.h"
#include <string>
#include "../rm/rm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "DataBuffer.h"
#include "cscl3dws.h"

struct stTrace
{
public:
	MP_STRING text;
	LOG_TYPE	logType;

	stTrace()
		:MP_STRING_INIT(text)
	{
		
	}
};

CInfoManager::CInfoManager( omsContext* aContext) :
	MP_VECTOR_INIT(mTraces)
{
	
	pContext = aContext;
	if (pContext != NULL)
		pContext->mpInfoMan = this;
	wndMain = MP_NEW( CMainFrame);
	wndMain->SetInfoManager(this);
	//pClientSession = NULL;
	//pCallbacks = NULL;
	InitializeCriticalSection(&mutex);
	InitializeCriticalSection(&csUpdateLogs);
	miTestMode = 0;
	mbTestStarting = false;
	m_testLogFile = NULL;
}

CInfoManager::~CInfoManager()
{
	//Disconnect();
	DeleteCriticalSection(&mutex);
	DeleteCriticalSection(&csUpdateLogs);
}

void CInfoManager::CreateWnd( unsigned long am_hParentWnd)
{
	m_hParentWnd = (HWND)am_hParentWnd;
	if(!wndMain->IsWindow())
	{
		RECT rc;
		rc.left = 0; rc.right = 640; rc.top = 50; rc.bottom = 530;
		::GetWindowRect( (HWND)m_hParentWnd, &rc);
		rc.top += 50;
		rc.right = rc.left + 720; rc.bottom = rc.top + 540;
		wndMain->SetContext( pContext);
		if(wndMain->CreateEx( (HWND)m_hParentWnd, &rc) == NULL)
		{
			ATLTRACE(_T("[INFOMANAGER] Main window creation failed!\n"));
		}
	}
}

void CInfoManager::ShowWindow( bool bShow)
{
	if( bShow)
		wndMain->ShowWindow( SW_SHOWDEFAULT);
	else
		wndMain->ShowWindow( SW_HIDE);
}

void CInfoManager::Trace(const std::string& text, LOG_TYPE logType)
{
/*	stTrace* pNewTrace = MP_NEW(stTrace);
	pNewTrace->logType = logType;
	pNewTrace->text = text;
	EnterCriticalSection(&csUpdateLogs);
	mTraces.push_back(pNewTrace);
	LeaveCriticalSection(&csUpdateLogs);*/
}

void CInfoManager::Update()
{
	if (wndMain->IsWindow() && wndMain->IsWindowVisible())
		wndMain->Update();
}

void CInfoManager::TraceTestResult(char* resStr)
{
	wndMain->TraceTestResult(resStr);
	if(mbTestStarting)
	{	
		if( !m_testLogFile || m_testLogFile == INVALID_HANDLE_VALUE)
		{
			CComString sFullPath = pContext->mp3DWS->GetApplicationRoot();
			sFullPath += "\\testResult.txt";
			m_testLogFile = ::CreateFile( sFullPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		}
		DWORD dwWriteSize = 0;
		::WriteFile( m_testLogFile, resStr, strlen(resStr), &dwWriteSize, NULL);			
	}
}

void CInfoManager::Destroy()
{
	if( wndMain->IsWindow())
		wndMain->DestroyWindow();
	else
		MP_DELETE( wndMain);
	wndMain = NULL;
	MP_DELETE_THIS;
}

void CInfoManager::ShowObject(const rmml::mlString& aTarget, bool aShowWindow)
{
	if (wndMain->IsWindow())
	{
		if (aShowWindow)
			ShowWindow(true);
		if (wndMain->IsWindowVisible())
		{
			wndMain->ShowObject(aTarget);
		}
	}
}

void CInfoManager::TraceSessionPacketInfo(std::string& asSessionName, BYTE packetType, DWORD size, bool isOut)
{
	wndMain->TraceSessionPacketInfo(asSessionName, packetType, size, isOut);
}

void CInfoManager::SetAutoTestMode( int aTestMode)
{
	miTestMode = aTestMode;
	pContext->mpSM->SetTestMode((rmml::mlETestModes)miTestMode);
}

bool CInfoManager::IsTestMode()
{
	return miTestMode > 0 ? true : false;
}

void CInfoManager::StartAutoTesting()
{
	if(mbTestStarting)
		return;	
	if( !m_testLogFile || m_testLogFile == INVALID_HANDLE_VALUE)
	{
		CComString sFullPath = pContext->mp3DWS->GetApplicationRoot();
		sFullPath += "\\testResult.txt";
		m_testLogFile = ::CreateFile( sFullPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	mbTestStarting = true;
	wndMain->StartAutoTesting();
}

void CInfoManager::TestEnded( int aiCompleteTest, int aiFailTest)
{
	if(miTestMode == 0)
		return;	
	if( !m_testLogFile || m_testLogFile == INVALID_HANDLE_VALUE)
	{
		CComString sFullPath = pContext->mp3DWS->GetApplicationRoot();
		sFullPath += "\\testResult.txt";
		m_testLogFile = ::CreateFile( sFullPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	DWORD dwWriteSize = 0;
	::WriteFile( m_testLogFile, &aiCompleteTest, 4, &dwWriteSize, NULL);
	dwWriteSize = 0;
	::WriteFile( m_testLogFile, &aiFailTest, 4, &dwWriteSize, NULL);	
	::CloseHandle( m_testLogFile);	
	pContext->mpRM->OnExit();
	CSimpleThreadStarter testThreadStarter(__FUNCTION__);	
	testThreadStarter.SetRoutine(TestEndedThread_);
	testThreadStarter.SetParameter(this);
	HANDLE hTestEndedThread = testThreadStarter.Start();

	mbTestStarting = false;
}

void CInfoManager::Ended()
{	
	//::SendMessage( m_hParentWnd, WM_DESTROY, 0, 0);	
	pContext->mpApp->exit();
	//Sleep(5000);	
}

DWORD WINAPI CInfoManager::TestEndedThread_(LPVOID param)
{
	Sleep(15000);

	CInfoManager* apInfoManager = (CInfoManager*)param;
	apInfoManager->Ended();
	
	return 0;
}
