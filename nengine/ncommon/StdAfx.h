// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__BD17E9FF_C60E_4F19_9017_678399F78293__INCLUDED_)
#define AFX_STDAFX_H__BD17E9FF_C60E_4F19_9017_678399F78293__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include "std_define.h"

#define GLEW_STATIC

#include <windows.h>
#include <atlbase.h>
#include <glew.h>
#include <gl\gl.h>
#include <gl\glu.h>

#include "..\..\rtl\rtl.h"

#pragma  warning (disable: 4786)
#pragma  warning (disable: 4702)
#pragma  warning (disable: 4505)

#define NCOMMON_BUILD
#include "..\..\common\memprofiling.h"
#undef NCOMMON_BUILD

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BD17E9FF_C60E_4F19_9017_678399F78293__INCLUDED_)
