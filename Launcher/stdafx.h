// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
//#include "..\..\common\std_define.h"
#define _WIN32_WINNT 0x0500
#define _SECURE_SCL	0

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
//#include <winuser.h>

#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlctrls.h>
#include <atlctrlw.h>
#include <atlframe.h>

#include <gdiplus.h>
#include <xstring>

using namespace Gdiplus;

#include "resource.h"
#include "launchererrors.h"

#include "..\..\rtl\rtl.h"

#define LAUNCHER
#include "..\..\common\memprofiling.h"

typedef INT	SOCKERR; 