 // stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

//#define DESKTOP_DETECT_MEMORY_LEAKS

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Change these values to use different versions
#define WINVER			0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE		0x0500
#define _RICHEDIT_VER	0x0100

#include "std_define.h"

#define DESKTOPMAN_BUILD
#include "..\common\memprofiling.h"
#undef DESKTOPMAN_BUILD

#include <atlbase.h>
#include <atlapp.h>
#include <shellapi.h>
#include <atlwin.h>
#include <atlmisc.h>
#include <TCHAR.h>
#include <atlctrls.h>
#include <atlctrlw.h>
#include <atlframe.h>
#include <atldlgs.h>
#include <atlddx.h>
#include "resource.h"
#include <atlscrl.h>
#include "..\rtl\rtl.h"

// TODO: reference additional headers your program requires here
#include "include/Errors.h"
#include "include/IDesktopMan.h"
#include "../Common/ViskoeThread.h"
#include "EventSource.h"


using namespace desktop;

#define SAFE_DELETE(x) { if(x!=NULL) MP_DELETE( x); x = NULL;}
#define SAFE_RELEASE(x) { if(NULL != x) x->Release(); x = NULL;}
#define SAFE_DELETE_ARRAY(x) { if(x!=NULL) MP_DELETE_ARR( x); x=NULL; }

extern CAppModule _Module;