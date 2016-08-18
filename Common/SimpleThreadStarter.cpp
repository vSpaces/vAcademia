
#include "StdAfx.h"
#include <Assert.h>
#include "SimpleThreadStarter.h"
#include "CrashHandlerInThread.h"

#define DEFAULT_STACK_SIZE			65535

DWORD WINAPI STSThreadProc(LPVOID lpParam)
{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();

	SThreadStartInfo* info = (SThreadStartInfo*)lpParam;
	LPTHREAD_START_ROUTINE routine = info->routine;
	void* param = info->param;
	
	DWORD result = routine(param);

	delete info;

	return result;
}

CSimpleThreadStarter::CSimpleThreadStarter(
#ifdef _DEBUG
	char* func
#else
	char*
#endif
	):
	m_stackSize(DEFAULT_STACK_SIZE),
	m_isSecondaryThread(true),
	m_threadID(0xFFFFFFFF),
	m_routine(NULL),	
	m_handle(NULL),
	m_param(NULL)	
{
#ifdef _DEBUG
	m_functionName = func;
#endif
}

void CSimpleThreadStarter::SetThreadType(int type)
{
	if (THREAD_TYPE_SELF_MANAGED == type)
	{
		m_isSecondaryThread = false;
	}
	else if (THREAD_TYPE_SECONDARY == type)
	{
		m_isSecondaryThread = true;
	}
	else
	{
		assert(false);
	}
}

void CSimpleThreadStarter::UpdateThreadAffinityIfNeeded()
{
	if (!m_handle)
	{
		return;
	}

	if (!m_isSecondaryThread)
	{
		return;
	}

	static int coreCount = -1;
	if (coreCount == -1)
	{
		_SYSTEM_INFO nfo;
		GetSystemInfo(&nfo);
		coreCount = nfo.dwNumberOfProcessors;	
	}

	if (coreCount > 1)
	{
		int threadMask = 1;
		for (int k = 0; k < coreCount; k++)
		{
			threadMask *= 2;
		}

		threadMask -= 2;

		SetThreadAffinityMask(m_handle, threadMask);
	}	
}

void CSimpleThreadStarter::SetStackSize(unsigned int stackSize)
{
	if (stackSize > 0)
	{
		m_stackSize = stackSize;
	}
	else
	{
		m_stackSize = DEFAULT_STACK_SIZE;
	}
}

unsigned int CSimpleThreadStarter::GetStackSize()
{
	return m_stackSize;
}

void CSimpleThreadStarter::SetRoutine(LPTHREAD_START_ROUTINE routine)
{
	m_routine = routine;
}

LPTHREAD_START_ROUTINE CSimpleThreadStarter::GetRoutine()
{
	return m_routine;
}

void CSimpleThreadStarter::SetParameter(void* param)
{
	m_param = param;
}

void* CSimpleThreadStarter::GetParameter()
{
	return m_param;
}

HANDLE CSimpleThreadStarter::Start(bool isSuspended)
{
	SThreadStartInfo* info = new SThreadStartInfo(m_routine, m_param, m_functionName);
	m_handle = ::CreateThread(NULL, m_stackSize, STSThreadProc, (void*)info, isSuspended ? CREATE_SUSPENDED : 0, (LPDWORD)&m_threadID);
	UpdateThreadAffinityIfNeeded();

	return m_handle;
}

HANDLE CSimpleThreadStarter::GetThreadHandle()
{
	return m_handle;
}

unsigned int CSimpleThreadStarter::GetThreadID()
{
	return m_threadID;	
}

CSimpleThreadStarter::~CSimpleThreadStarter()
{
}
