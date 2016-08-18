#include "stdafx.h"
#include "CopyDataPoster.h"
#include "CrashHandlerInThread.h"
#include "../nengine/ncommon/ThreadAffinity.h"

void Execute(ThreadData* threadInfo)
{
	SendMessage(threadInfo->hwnd, WM_COPYDATA, threadInfo->msgCode, threadInfo->message);
	COPYDATASTRUCT* message = (COPYDATASTRUCT*)(threadInfo->message);
	unsigned char* data = (unsigned char *)message->lpData;
	MP_DELETE_ARR(data);
	MP_DELETE(message);
	MP_DELETE(threadInfo);
}

DWORD WINAPI SendThreadProc(LPVOID lpParam)
{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();

	SetSecondaryThreadAffinity( GetCurrentThread());

	bool isWorking = true;
	while (isWorking)
	{
		ThreadData* threadInfo = CCopyDataPoster::GetInstance()->GetThreadData();
		if (threadInfo)
		{
			Execute(threadInfo);
		}
		else
		{

			Sleep(5);
		}
	}

	return 0;
}

CCopyDataPoster::CCopyDataPoster():
	MP_VECTOR_INIT( m_threads_data)
{
	InitializeCriticalSection(&m_cs);
	m_thread.SetStackSize(32 * 1024);
	m_threads_data.reserve(20);
}

CCopyDataPoster* CCopyDataPoster::GetInstance()
{
	static CCopyDataPoster* obj = NULL;

	if (!obj)
	{
		obj = new CCopyDataPoster();
		obj->StartThread();
	}

	return obj;
}

void CCopyDataPoster::StartThread()
{
	m_thread.SetThreadProcedure(SendThreadProc);
	m_thread.Start();
}

void CCopyDataPoster::AddData(ThreadData* data)
{
	EnterCriticalSection(&m_cs);
	m_threads_data.push_back(data);
	LeaveCriticalSection(&m_cs);
}

ThreadData* CCopyDataPoster::GetThreadData()
{
	EnterCriticalSection(&m_cs);

	if (m_threads_data.size() > 0)
	{
		ThreadData* res = m_threads_data[0];
		m_threads_data.erase(m_threads_data.begin());
		LeaveCriticalSection(&m_cs);
		return res;
	}
	LeaveCriticalSection(&m_cs);

	return NULL;
}

CCopyDataPoster::~CCopyDataPoster()
{
	DeleteCriticalSection(&m_cs);
}