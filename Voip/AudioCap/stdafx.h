// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
//#define MEMORY_PROFILING	1
// Windows Header Files:
#include <windows.h>

#include <wtypes.h>
#include <atldef.h>
#include <atlcomcli.h>
#include "ComString.h"

//#include "..\..\nmemoryprofiler\MemoryProfilerCreate.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)  if(x) {x->Release(); x=0;}
#endif

// TODO: reference additional headers your program requires here
