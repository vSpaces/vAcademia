// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include "..\common\std_define.h"
#undef RTL_STATIC_LIB

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#include <windows.h>
#include <atlbase.h>

// TODO: reference additional headers your program requires here
#include "rtl.h"

#define RTL_BUILD
#include "..\common\memprofiling.h"
#undef RTL_BUILD
