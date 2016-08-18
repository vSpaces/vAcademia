// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define VASE_EXPORTS

//#define DESKTOP_DETECT_MEMORY_LEAKS
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include "std_define.h"

// Windows Header Files:
#include <windows.h>

#include "vase.h"

#define VASENCODER_BUILD
#include "../common/memprofiling.h"
#include "rtl.h"

#define SAFE_DELETE(x) { if(x!=NULL) MP_DELETE( x); x = NULL;}
//#define SAFE_FREE(x) { if(x!=NULL) free(x); x = NULL;}
#define SAFE_DELETE_ARRAY(x) { if(x!=NULL) MP_DELETE_ARR( x); x = NULL;}
