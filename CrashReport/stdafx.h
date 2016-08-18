// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "std_define.h"

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif 
#define _ATL_APARTMENT_THREADED

/*#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlctrls.h>
#include <atlctrlw.h>
#include <atlframe.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include "rtl.h"*/

#include "VacademiaReport.h"
#include "../VacademiaReportLib/resource.h"

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include "resource.h"


// TODO: reference additional headers your program requires here
#define MEMORY_PROFILING	0
#define NO_PROFILING 1
#include "..\common\memprofiling.h"
