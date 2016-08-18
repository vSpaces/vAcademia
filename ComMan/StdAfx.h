// stdafx.h

#pragma once

#define COMMAN_EXPORTS
#include "std_define.h"
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

/*#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x400
#endif*/

// Windows Header Files:
#include <windows.h>
#include <windef.h>
#include <atlbase.h>

#define COMMAN_BUILD
#include "..\common\memprofiling.h"
#undef COMMAN_BUILD

//#define VC_SETTINGS
#define COMMAN

#include "rtl.h"
#include "Common.h"
#include "globals.h"
#include "common_globals.h"
#include "time.h"
#include "assert.h"
#include <shellapi.h>
#include <mmsystem.h>
#include <WINDOWSX.H>

#include <crtdbg.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "StringFunctions.h"

#define MAX_PATH2          3120

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#endif

void correctPath( CWComString& aPath);
void correctRemotePath( CWComString& aPath);

#define LOGSESSSIONINFO 1
// TODO: reference additional headers your program requires here