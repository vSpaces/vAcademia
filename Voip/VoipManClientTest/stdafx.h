// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WS3DMAN_EXPORTS
#define _SECURE_SCL 0
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <atlbase.h>
#include <commdlg.h>
#include <atlcom.h>
#include <atlctl.h>
#include <shellapi.h>

#define CSCL_BUILD
#include "..\common\memprofiling.h"
#undef CSCL_BUILD

#include "rtl.h"
#include "oms_context.h"
#include "rmml.h"

// TODO: reference additional headers your program requires here
