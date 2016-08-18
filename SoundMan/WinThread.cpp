#include "stdafx.h"
#include "WinThread.h"
#include "ThreadAffinity.h"

#include "CrashHandlerInThread.h"

// Threads
DWORD WINAPI main_function(void * threadPtr)
{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();

	ATLASSERT( threadPtr);
	CWinThread * thread = (CWinThread *)threadPtr;
	
	CoInitialize(NULL);

	thread->main();

	CoUninitialize();
	
	return 0;
}

CWinThread::CWinThread()
{
	m_dwThreadId = -1;
	m_bTerminating = FALSE;
	m_bTerminated = FALSE;
	m_handle = NULL;
	m_dwStackSize = 0;
	m_pObject = NULL;
}

CWinThread::~CWinThread()
{	
	if ( !is_terminated())
	{
		terminating();
		Sleep( 1);
		terminated();
	}
}

//////////////////////////////////////////////////////////////////////////

BOOL CWinThread::create( LPTHREAD_START_ROUTINE lpStartAddress /*= NULL*/)
{
	if ( lpStartAddress == NULL)
		lpStartAddress = main_function;
	m_lpStartAddress = lpStartAddress;

	CSimpleThreadStarter threadStarter(__FUNCTION__);
	threadStarter.SetStackSize(m_dwStackSize);
	threadStarter.SetRoutine(m_lpStartAddress);
	threadStarter.SetParameter((PVOID) this);
	m_handle = threadStarter.Start();	
	m_dwThreadId = threadStarter.GetThreadID();	 

	ATLASSERT( m_handle != NULL);
	return m_handle != NULL;	
}

BOOL CWinThread::create( LPTHREAD_START_ROUTINE lpStartAddress, PVOID pObject)
{
	ATLASSERT( pObject);
	if ( !pObject)
		return FALSE;

	CSimpleThreadStarter threadStarter(__FUNCTION__);
	threadStarter.SetRoutine(lpStartAddress);
	threadStarter.SetParameter((PVOID) pObject);
	m_handle = threadStarter.Start();	
	m_dwThreadId = threadStarter.GetThreadID();	

	if ( m_handle != NULL)
	{
		m_pObject = pObject;
		return TRUE;
	}
	return FALSE;
}

BOOL CWinThread::resume()
{
	ATLASSERT( m_handle);
	if ( m_handle != NULL)
		return ResumeThread( m_handle) != -1;
	return FALSE;
}

BOOL CWinThread::suspend()
{
	ATLASSERT( m_handle);
	if ( m_handle != NULL)
		return SuspendThread( m_handle) != -1;
	return FALSE;
}

void CWinThread::terminated()
{
	if ( m_handle == NULL)
		return;
	if ( WaitForSingleObject( m_handle, 20000) == WAIT_TIMEOUT)
	{
		TerminateThread( m_handle, 1);
	}
	CloseHandle(m_handle);
	m_handle = NULL;
}

BOOL CWinThread::restart()
{	
	if ( m_lpStartAddress == NULL)
		return FALSE;

	CSimpleThreadStarter threadStarter(__FUNCTION__);
	threadStarter.SetRoutine(m_lpStartAddress);
	threadStarter.SetParameter((PVOID) this);
	m_handle = threadStarter.Start();	
	m_dwThreadId = threadStarter.GetThreadID();	

	return m_handle != NULL;	
}

BOOL CWinThread::is_terminated() const
{
	return wait_for_termination( 0);
}

BOOL CWinThread::wait_for_termination( const long maxWait) const
{
	if ( m_handle == NULL) 	
		return TRUE;	

	DWORD result;
	int retries = 10;
	while (( result = WaitForSingleObject( m_handle, maxWait)) != WAIT_TIMEOUT) 
	{
		if ( result == WAIT_OBJECT_0)
			return TRUE;

		if (::GetLastError() != ERROR_INVALID_HANDLE) 
		{
			ATLASSERT( FALSE);
			return TRUE;
		}

		if ( retries == 0)
			return TRUE;

		retries--;
	}

	return FALSE;
}

BOOL CWinThread::is_suspended() const
{	
	if ( m_handle == NULL)
		return FALSE;
	SuspendThread( m_handle);
	return ResumeThread( m_handle) > 1;
}

#if !defined(_WIN32_WCE) || (_WIN32_WCE < 300)
#define CTHREAD_PRIORITY_LOWEST THREAD_PRIORITY_LOWEST
#define CTHREAD_PRIORITY_BELOW_NORMAL THREAD_PRIORITY_BELOW_NORMAL
#define CTHREAD_PRIORITY_NORMAL THREAD_PRIORITY_NORMAL
#define CTHREAD_PRIORITY_ABOVE_NORMAL THREAD_PRIORITY_ABOVE_NORMAL
#define CTHREAD_PRIORITY_HIGHEST THREAD_PRIORITY_HIGHEST
#else
#define CTHREAD_PRIORITY_LOWEST 243
#define CTHREAD_PRIORITY_BELOW_NORMAL 245
#define CTHREAD_PRIORITY_NORMAL 247
#define CTHREAD_PRIORITY_ABOVE_NORMAL 249
#define CTHREAD_PRIORITY_HIGHEST 251
#endif

void CWinThread::set_priority( Priority priorityLevel)
{
	ATLASSERT(!is_terminated());

	static int const priorities[ NumPriorities] = 
	{
		CTHREAD_PRIORITY_LOWEST,
		CTHREAD_PRIORITY_BELOW_NORMAL,
		CTHREAD_PRIORITY_NORMAL,
		CTHREAD_PRIORITY_ABOVE_NORMAL,
		CTHREAD_PRIORITY_HIGHEST
	};
	SetThreadPriority( m_handle, priorities[ priorityLevel]);
}


CWinThread::Priority CWinThread::get_priority() const
{
	ATLASSERT( !is_terminated());

	switch ( GetThreadPriority( m_handle)) 
	{
		case CTHREAD_PRIORITY_LOWEST :
			return LowestPriority;
		case CTHREAD_PRIORITY_BELOW_NORMAL :
			return LowPriority;
		case CTHREAD_PRIORITY_NORMAL :
			return NormalPriority;
		case CTHREAD_PRIORITY_ABOVE_NORMAL :
			return HighPriority;
		case CTHREAD_PRIORITY_HIGHEST :
			return HighestPriority;
	}
	ATLASSERT( false);
	return LowestPriority;
}

//////////////////////////////////////////////////////////////////////////

void CWinThread::terminating()
{
	m_bTerminating = TRUE;
}


//////////////////////////////////////////////////////////////////////////
