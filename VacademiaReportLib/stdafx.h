// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "std_define.h"
/*#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x400
#endif*/

#include "targetver.h"

#include "VacademiaReport.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "resource.h"

// TODO: reference additional headers your program requires here

#define VACADEMIA_REPORT_LIB_BUILD
#include "..\common\memprofiling.h"
#undef VACADEMIA_REPORT_LIB_BUILD

#define VOIPAUDIOCAPTURE_EXPORTS

#define SAFE_DELETE(p)       { if (p) delete p; }
#define SAFE_DELETE_ARRAY(p) { if (p) delete[] p; }
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }