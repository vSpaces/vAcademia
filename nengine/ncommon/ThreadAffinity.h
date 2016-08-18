
#pragma once

#include <windows.h>
#include <winbase.h>
#include <assert.h>

static unsigned int g_coreCount = 0;

static unsigned int g_mainThreadID = 0;

static void InitSecondaryThreadAffinityRoutine()
{
	_SYSTEM_INFO nfo;
	GetSystemInfo(&nfo);
	g_coreCount = nfo.dwNumberOfProcessors;	

	g_mainThreadID = GetCurrentThreadId();
}

__forceinline static void SetSecondaryThreadAffinity(HANDLE hndl)
{
	if (0 == g_coreCount)
	{
		InitSecondaryThreadAffinityRoutine();
	}

	if (g_coreCount > 1)
	{
		int threadMask = 1;
		for (unsigned int k = 0; k < g_coreCount; k++)
		{
			threadMask *= 2;
		}

		threadMask -= 2;

		SetThreadAffinityMask(hndl, threadMask);
	}	
}

static void CheckMainThread()
{
	assert(GetCurrentThreadId() == g_mainThreadID);
}

static void CheckNonMainThread()
{
	assert(GetCurrentThreadId() != g_mainThreadID);
}