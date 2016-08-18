
#include "StdAfx.h"
#include "Thread.h"
#include "WinBase.h"
#include "SimpleThreadStarter.h"

CThread::CThread(void):
	m_isAffinitySelfManaged(false),
	m_state(STATE_NOTEXECUTED),
	m_stackSize(64000),
	m_handle(NULL)
{
}

void CThread::SetStackSize(unsigned int stackSize)
{
	m_stackSize = stackSize;
}

unsigned int CThread::GetStackSize()const
{
	return m_stackSize;
}

void CThread::SetThreadProcedure(LPTHREAD_START_ROUTINE threadProc)
{
	m_threadProc = threadProc;
}

void CThread::SetParameter(LPVOID lpParam)
{
	m_lpParam = lpParam;
}

void CThread::SetAffinity(unsigned int mask)
{
	SetThreadAffinityMask(m_handle, mask);
}

BOOL CThread::Start()
{
	if (m_handle != NULL)
	{
		CloseHandle(m_handle);
	}

	CSimpleThreadStarter threadStarter(__FUNCTION__);
	threadStarter.SetStackSize(m_stackSize);
	threadStarter.SetRoutine(m_threadProc);
	threadStarter.SetParameter(m_lpParam);
	if (m_isAffinitySelfManaged)
	{
		threadStarter.SetThreadType(THREAD_TYPE_SELF_MANAGED);
	}
	m_handle = threadStarter.Start();
	m_threadId = threadStarter.GetThreadID();
	
	if (m_handle != NULL)
	{
		m_state = STATE_EXECUTING;
	}
	else
	{
		m_errorCode = GetLastError();
	}

	return (m_handle != NULL);
}

void CThread::SelfManageAffinity()
{
	m_isAffinitySelfManaged = true;
}

/*BOOL CThread::Pause()
{
	DWORD res = SuspendThread(m_handle);

	if (res != 0xFFFFFFFF)
	{
		m_state = STATE_STOPPING;
	}
	else
	{
		m_errorCode = GetLastError();
	}

	return (res != 0xFFFFFFFF);
}

BOOL CThread::Play()
{
	if (m_state == STATE_NOTEXECUTED)
	{
		if (m_handle != NULL)
		{
			CloseHandle(m_handle);
		}

		m_handle = CreateThread(NULL, m_stackSize, m_threadProc, m_lpParam, 0, &m_threadId);

		if (m_handle != NULL)
		{
			m_state = STATE_EXECUTING;
		}
		else
		{
			m_errorCode = GetLastError();
		}

		return (m_handle != NULL);
	}
	else
	{
		DWORD res = ResumeThread(m_handle);

		if (res == 0xFFFFFFFF)
		{
			m_errorCode = GetLastError();
		}

		return (res != 0xFFFFFFFF);
	}
}*/

int CThread::GetPriority()
{
	return GetThreadPriority(m_handle);
}

BOOL CThread::SetPriority(int priority)
{
	BOOL res = SetThreadPriority(m_handle, priority); 

	if (!res)
	{
		m_errorCode = GetLastError();
	}

	return res;
}

int CThread::GetCurrentState()
{
	if (IsThreadTerminated())
	{
		m_state = STATE_NOTEXECUTED;
	}

	return m_state;
}

BOOL CThread::IsThreadTerminated()
{
	DWORD exitCode = 0;

	BOOL res = GetExitCodeThread(m_handle, &exitCode);

	if (!res)
	{
		m_errorCode = GetLastError();
	}

	return (exitCode != STILL_ACTIVE); 
}

DWORD CThread::GetExitCode()
{
	DWORD exitCode = 0;

	BOOL res = GetExitCodeThread(m_handle, &exitCode);

	if (!res)
	{
		m_errorCode = GetLastError();
	}

	return exitCode;
}

BOOL CThread::TerminateStop()
{
	//09.04.2014 лишь при закрытии приложения - иначе риск повисаний
	BOOL res = TerminateThread(m_handle, 0);

	if (!res)
	{
		m_errorCode = GetLastError();
	}

	return res;
}

DWORD CThread::GetErrorCode()const
{
	return m_errorCode;
}

HANDLE CThread::GetHandle()
{
	return m_handle;
}

CThread::~CThread(void)
{
	if (m_handle != NULL)
	{
		CloseHandle(m_handle);
	}
}