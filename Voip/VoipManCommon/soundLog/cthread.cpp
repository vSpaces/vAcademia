/*
 * thread.cxx
 */

#include "stdafx.h"
#include "cthread.h"

#ifndef _WIN32_WCE
#ifdef _MSC_VER
#pragma comment(lib, "mpr.lib")
#endif
#endif

#if defined(_WIN32_DCOM) 
  #include <objbase.h>
#ifdef _MSC_VER
  #pragma comment(lib, _OLE_LIB)
#endif
#endif

#include "CrashHandlerInThread.h"
//////////////////////////////////////////////////////////////////////////

#if defined(_DEBUG) && defined(_MSC_VER)

typedef struct tagTHREADNAME_INFO
{
	DWORD dwType ;                       // must be 0x1000
	LPCSTR szName ;                      // pointer to name (in user addr space)
	DWORD dwThreadID ;                   // thread ID (-1=caller thread, but seems to set more than one thread's name)
	DWORD dwFlags ;                      // reserved for future use, must be zero
} THREADNAME_INFO ;

void SetWinDebugThreadName (THREADNAME_INFO * info)
{
	__try
	{
		RaiseException (0x406D1388, 0, sizeof(THREADNAME_INFO)/sizeof(DWORD), (DWORD *) info) ;
	}                              // if not running under debugger exception comes back
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{                              // just keep on truckin'
	}
}

#endif // defined(_DEBUG) && defined(_MSC_VER)

///////////////////////////////////////////////////////////////////////////////
// CThread

void CThread::Sleep(const long delay)
{ ::Sleep(delay); }

inline CThread::CThread()
{ }

inline DWORD CThread::GetThreadId() const
{ return threadId; }

inline DWORD CThread::GetCurrentThreadId()
{ return ::GetCurrentThreadId(); }

void CThread::SetThreadName(CComString & name)
{
	if ( name.IsEmpty())
		threadName.Format( "%s:%08x", "CThread", (INT)this);		
	else
		threadName.Format( "%s", name.GetString(), (INT)this);		

#if defined(_DEBUG) && defined(_MSC_VER)
	if (threadId) 
	{       // make thread name known to debugger
		THREADNAME_INFO Info = { 0x1000, (const char *) threadName, threadId, 0 } ;
		SetWinDebugThreadName (&Info) ;
	}
#endif // defined(_DEBUG) && defined(_MSC_VER)
}

CComString CThread::GetThreadName() const
{
	return threadName; 
}

///////////////////////////////////////////////////////////////////////////////
// Threads

DWORD WINAPI CThread::MainFunction(LPVOID threadPtr)
{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();
	assert( threadPtr);
	CThread * thread = (CThread *)threadPtr;
	thread->SetThreadName(thread->GetThreadName());

#if defined(_WIN32_DCOM)
	::CoInitializeEx(NULL, COINIT_MULTITHREADED);
#endif

	thread->Main();

#if defined(_WIN32_DCOM)
	::CoUninitialize();
#endif

	return 0;
}


void CThread::Win32AttachThreadInput()
{
}


CThread::CThread(PINDEX stackSize,
                 AutoDeleteFlag deletion,
                 Priority priorityLevel,
                 const CComString & name)
  : threadName(name)
{
  //PAssert(stackSize > 0, PInvalidParameter);
	assert( stackSize >= 0);
	originalStackSize = stackSize;

	autoDelete = deletion == AutoDeleteThread;

	CSimpleThreadStarter threadStarter(__FUNCTION__);
	threadStarter.SetStackSize(stackSize);
	threadStarter.SetRoutine(MainFunction);
	threadStarter.SetParameter(this);
	threadHandle = threadStarter.Start(true);
	threadId = threadStarter.GetThreadID();

	assert(threadHandle != NULL);

	SetPriority(priorityLevel);

	traceBlockIndentLevel = 0;  
}


CThread::~CThread()
{
	if (originalStackSize <= 0)
		return;

	if (!IsTerminated())
		Terminate();

	if (threadHandle != NULL)
		CloseHandle(threadHandle);
}


void CThread::Restart()
{
	assert(IsTerminated());

	CSimpleThreadStarter threadStarter(__FUNCTION__);
	threadStarter.SetStackSize(originalStackSize);
	threadStarter.SetRoutine(MainFunction);
	threadStarter.SetParameter(this);
	threadHandle = threadStarter.Start(true);
	threadId = threadStarter.GetThreadID();

	assert(threadHandle != NULL);
}


void CThread::Terminate()
{
	assert( originalStackSize > 0);   
	//09.04.2014 лишь при закрытии приложения - иначе риск повисаний
    TerminateThread(threadHandle, 1);
}


BOOL CThread::IsTerminated() const
{
	return WaitForTermination(0);
}


void CThread::WaitForTermination() const
{
	WaitForTermination(0x0ffffff);
}


BOOL CThread::WaitForTermination(const long maxWait) const
{
	if (threadHandle == NULL) 
	{    
		return TRUE;
	}

	DWORD result;
	PINDEX retries = 10;
	while ((result = WaitForSingleObject(threadHandle, maxWait)) != WAIT_TIMEOUT) 
	{
		if (result == WAIT_OBJECT_0)
		return TRUE;

		if (::GetLastError() != ERROR_INVALID_HANDLE) 
		{
			assert(FALSE);
			return TRUE;
		}

		if ( retries == 0)
			return TRUE;

		retries--;
	}

	return FALSE;
}


void CThread::Suspend(BOOL susp)
{
	assert(!IsTerminated());
	if (susp)
		SuspendThread(threadHandle);
	else
		Resume();
}


void CThread::Resume()
{
	assert(!IsTerminated());
	ResumeThread(threadHandle);
}


BOOL CThread::IsSuspended() const
{
	SuspendThread(threadHandle);
	return ResumeThread(threadHandle) > 1;
}


void CThread::SetAutoDelete(AutoDeleteFlag deletion)
{
}

#if !defined(_WIN32_WCE) || (_WIN32_WCE < 300)
#define PTHREAD_PRIORITY_LOWEST THREAD_PRIORITY_LOWEST
#define PTHREAD_PRIORITY_BELOW_NORMAL THREAD_PRIORITY_BELOW_NORMAL
#define PTHREAD_PRIORITY_NORMAL THREAD_PRIORITY_NORMAL
#define PTHREAD_PRIORITY_ABOVE_NORMAL THREAD_PRIORITY_ABOVE_NORMAL
#define PTHREAD_PRIORITY_HIGHEST THREAD_PRIORITY_HIGHEST
#else
#define PTHREAD_PRIORITY_LOWEST 243
#define PTHREAD_PRIORITY_BELOW_NORMAL 245
#define PTHREAD_PRIORITY_NORMAL 247
#define PTHREAD_PRIORITY_ABOVE_NORMAL 249
#define PTHREAD_PRIORITY_HIGHEST 251
#endif

void CThread::SetPriority( Priority priorityLevel)
{
	assert(!IsTerminated());

	static int const priorities[ NumPriorities] = 
	{
		PTHREAD_PRIORITY_LOWEST,
		PTHREAD_PRIORITY_BELOW_NORMAL,
		PTHREAD_PRIORITY_NORMAL,
		PTHREAD_PRIORITY_ABOVE_NORMAL,
		PTHREAD_PRIORITY_HIGHEST
	};
	SetThreadPriority(threadHandle, priorities[priorityLevel]);
}


CThread::Priority CThread::GetPriority() const
{
	assert(!IsTerminated());

	switch (GetThreadPriority(threadHandle)) 
	{
		case PTHREAD_PRIORITY_LOWEST :
			return LowestPriority;
		case PTHREAD_PRIORITY_BELOW_NORMAL :
			return LowPriority;
		case PTHREAD_PRIORITY_NORMAL :
			return NormalPriority;
		case PTHREAD_PRIORITY_ABOVE_NORMAL :
			return HighPriority;
		case PTHREAD_PRIORITY_HIGHEST :
			return HighestPriority;
	}
	assert( false);
	return LowestPriority;
}


void CThread::InitialiseProcessThread()
{
	originalStackSize = 0;
	autoDelete = FALSE;
	threadHandle = GetCurrentThread();
	threadId = GetCurrentThreadId();	
	traceBlockIndentLevel = 0;
}

///////////////////////////////////////////////////////////////////////////////
// End Of File ///////////////////////////////////////////////////////////////
