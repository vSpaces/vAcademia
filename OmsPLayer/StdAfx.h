// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "std_define.h"
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>
#include "wtypes.h"
// C RunTime Header Files
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <xstring>
#include "rtl.h"

#define VC_SETTINGS

#define OMSPLAYER_BUILD
#include "..\common\memprofiling.h"
#undef OMSPLAYER_BUILD

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
