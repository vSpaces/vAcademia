// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include "std_define.h"
// Windows Header Files:
#include <windows.h>

#define LOGGER_BUILD
#include "..\common\memprofiling.h"
#undef LOGGER_BUILD

#include <atlbase.h>
#include "rtl.h"

#define LOGGER_EXPORTS

// TODO: reference additional headers your program requires here

//#include <Shlwapi.h >