// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "std_define.h"

/*#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x400
#endif*/

#define INFOMAN_BUILD
#include "..\..\common\memprofiling.h"
#undef INFOMAN_BUILD

#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>
#include <crtdbg.h>
#include "rtl.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#endif

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlmisc.h>

// TODO: reference additional headers your program requires here
