// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

//#define UNICODE

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include "std_define.h"


#define PATHREFINER_BUILD
#include "..\common\memprofiling.h"
#undef PATHREFINER_BUILD

#include <windows.h>
#include <atlbase.h>
#include "rtl.h"

#define PATHREFINE_EXPORTS

// TODO: reference additional headers your program requires here

#include <Shlwapi.h >