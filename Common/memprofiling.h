#pragma once

#define MP_DLL_ID			255

#include "memprofiling_enable.h"
#include "memprofiling_dllid.h"

#ifdef MP_DLL_ID_INTERNAL
	#pragma warning( error : MP_DLL_ID_INTERNAL is already defined)
#else
	#define MP_DLL_ID_INTERNAL MP_DLL_ID
#endif

#include "../nmemoryprofiler/MemoryProfilerCreate.h"