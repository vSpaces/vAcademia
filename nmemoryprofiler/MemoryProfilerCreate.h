#pragma once

#include "IBlockMemoryController.h"
#include <windows.h>
#include "MemoryProfilerCreateImpl.h"

/*static IBlockMemoryController* GetBlockMemoryControllerInstance()
{
	HMODULE hDll = ::LoadLibrary("nmemoryprofiler.dll");
	if (hDll == NULL)
	{
		return NULL;
	}
	IBlockMemoryController* (*lpfnCreateBlockMemoryController)();
	(FARPROC&)lpfnCreateBlockMemoryController = GetProcAddress(hDll, "CreateBlockMemoryController");
	if (lpfnCreateBlockMemoryController == NULL)
	{		
		return NULL;
	}
	return lpfnCreateBlockMemoryController();
}*/

//static IBlockMemoryController* gBMC = GetBlockMemoryControllerInstance();

#include "MapMP.h"
#include "ListMP.h"
#include "StackMP.h"
#include "VectorMP.h"
#include "StringMP.h"
#include "WideStringMP.h"

#include "MemoryProfilerFuncs.h"
#include "MemoryProfilerDefines.h"
