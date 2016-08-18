// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

/*#if defined(DEBUG) || defined(_DEBUG)
#define _HAS_ITERATOR_DEBUGGING 0
//#define _ITERATOR_DEBUG_LEVEL 0
#endif*/

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include "std_define.h"
#undef LOGWRITER_STATIC_LIB
// Windows Header Files:
#include <windows.h>
#include <atlbase.h>

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#define LOGWRITER_BUILD
#include "..\common\memprofiling.h"
#undef LOGWRITER_BUILD

// TODO: reference additional headers your program requires here
#define LOGWRITER_EXPORTS