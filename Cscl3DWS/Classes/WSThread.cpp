// WSThread.cpp: implementation of the CWSThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WSThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "CrashHandlerInThread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DWORD	WINAPI ThreadProc(LPVOID pParam)
{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();
	return ((CWSThread*)pParam)->Thread();
}

CWSThread::CWSThread()
{
	hThread = 0;
}

CWSThread::~CWSThread()
{
	if ( hThread != 0)
	{
		CloseHandle( hThread);
		hThread = 0;
	}
}

void CWSThread::Run()
{
	DWORD  idDebugEventThread;

	if( hThread == 0)
	{
		CSimpleThreadStarter threadStarter(__FUNCTION__);		
		threadStarter.SetRoutine(ThreadProc);
		threadStarter.SetParameter(this);
		hThread = threadStarter.Start();
		idDebugEventThread = threadStarter.GetThreadID();
	}
}

void CWSThread::Stop()
{
}

void CWSThread::Pause()
{
}

void CWSThread::SetPriority()
{
}

UINT CWSThread::Thread()
{
	return 0;
}
