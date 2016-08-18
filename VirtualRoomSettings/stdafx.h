// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

#define WINVER		0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#define VCSETTINGS_BUILD
#include "..\common\memprofiling.h"
#undef VCSETTINGS_BUILD

#define VC_SETTINGS

#include <atlbase.h>
#include <atlcoll.h>
#include <atlstr.h>
#include <atltypes.h>

#include <atlapp.h>

extern CAppModule _AppModule;

#include <atlwin.h>
#include <atlctl.h>
#include <atluser.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>

#include "rtl.h"

