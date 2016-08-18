// nmemoryprofiler.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "MemoryProfiler.h"
#include "BlockMemoryController.h"
#include "nmemoryprofiler.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

/*****************************************************
плагин возвращает ссылку на интерфейс
через который можно  заинициализировать memory_profiler
******************************************************/

#pragma warning(push)
#pragma warning(disable : 4273)

IMemoryProfiler* CreateMemoryProfiler()
{
	CMemoryProfiler*	pMemoryProfilerApp = CMemoryProfiler::GetInstance();
	return pMemoryProfilerApp;
}

IBlockMemoryController* CreateBlockMemoryController()
{
	CBlockMemoryController*	blockMemoryController = CBlockMemoryController::GetInstance();
	return blockMemoryController;
}

#pragma warning(pop)