#if !defined(AFX_STDAFX_H__AB18C0C9_F97A_4303_9BB2_8E0AF724BD15__INCLUDED_)
#define AFX_STDAFX_H__AB18C0C9_F97A_4303_9BB2_8E0AF724BD15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WS3DMAN_EXPORTS
#include "std_define.h"
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>
#include <atlbase.h>
#include <commdlg.h>
#include <atlcom.h>
#include <atlctl.h>
#include <shellapi.h>
#include <mmsystem.h>
#include <WINDOWSX.H>

#define CSCL_BUILD
#include "..\common\memprofiling.h"
#undef CSCL_BUILD

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#endif

#include "rtl.h"
#include "oms\oms_context.h"
#include "rm\rmext.h"
#include "rm\rm.h"
#include "sm\sm.h"
#include "res\res.h"

#endif