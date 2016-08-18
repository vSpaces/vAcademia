#pragma once

#ifndef _MEMORY_PPOFILER

#define _MEMORY_PPOFILER

#include "IMemoryProfiler.h"

static IMemoryProfiler* GetMemoryProfilerInstance()
{
	HMODULE hDll = ::LoadLibraryW(L"nmemoryprofiler.dll");
	if (hDll == NULL)
	{
		return NULL;
	}
	IMemoryProfiler* (*lpfnCreateMemoryProfiler)();
	(FARPROC&)lpfnCreateMemoryProfiler = GetProcAddress(hDll, "CreateMemoryProfiler");
	if (lpfnCreateMemoryProfiler == NULL)
	{		
		return NULL;
	}
	return lpfnCreateMemoryProfiler();
}

static IMemoryProfiler* gMP = GetMemoryProfilerInstance();

#endif