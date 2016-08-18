#pragma once
#include "infoman.h"
#include "MainFrm.h"
/*#include "imessagehandler.h"
#include "iclientsession.h"
#include "ObjectStateQueue.h"*/
#include <sstream>

using namespace oms;
using namespace info;

struct stTrace;

class CInfoManager :
	public IInfoManager,
	public IExternalLogWriter
{
public:
	CInfoManager( omsContext* aContext);
	virtual ~CInfoManager();
	
	void CreateWnd( unsigned long m_hParentWnd);
	void ShowWindow( bool bShow);
	void Destroy();
	void Trace(const std::string& text, LOG_TYPE logType);
	void Update();
	void TraceTestResult(char* resStr);
	void ShowObject(const rmml::mlString& aTarget, bool aShowWindow);
	void TraceSessionPacketInfo(std::string& asSessionName, BYTE packetType, DWORD size, bool isOut);
	void SetAutoTestMode( int aTestMode);
	bool IsTestMode();
	void StartAutoTesting();
	void TestEnded( int aiCompleteTest, int aiFailTest);
	static DWORD WINAPI TestEndedThread_(LPVOID param);

protected:
	omsContext*			pContext;
	CMainFrame *wndMain;
	//cs::iclientsession* pClientSession;
	MP_VECTOR<stTrace*> mTraces; // очередь трейсов
	CRITICAL_SECTION csUpdateLogs;
	int miTestMode;
	bool mbTestStarting;
	HWND m_hParentWnd;
	HANDLE m_testLogFile;


private:
	CRITICAL_SECTION mutex; 
	void Ended();
};
