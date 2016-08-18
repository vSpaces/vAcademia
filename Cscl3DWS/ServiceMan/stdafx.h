// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "std_define.h"
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

/*#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x400
#endif*/

// Windows Header Files:
#include <windows.h>
#include <atlbase.h>
#include "rtl.h"

#include <crtdbg.h>

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#endif

#define SERVICEMAN_BUILD
#include "..\..\common\memprofiling.h"
#undef SERVICEMAN_BUILD

HWND hWndMain;

// TODO: reference additional headers your program requires here
