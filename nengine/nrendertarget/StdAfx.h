// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__6A7E02F8_4413_4D0B_87E6_325E5CFA853A__INCLUDED_)
#define AFX_STDAFX_H__6A7E02F8_4413_4D0B_87E6_325E5CFA853A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include "std_define.h"

#include <windows.h>
#include <glew.h>

#pragma warning(push)
#pragma warning(disable : 4100)
#include <ode.h>
#pragma warning(pop)

#include <gl\gl.h>
#include <gl\glu.h>


#pragma  warning (disable: 4786)
#pragma  warning (disable: 4702)
#pragma  warning (disable: 4505)

#include "GlobalSingletonStorage.h"

#define NRENDERTARGET_BUILD
#include "..\..\common\memprofiling.h"
#undef NRENDERTARGET_BUILD

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__6A7E02F8_4413_4D0B_87E6_325E5CFA853A__INCLUDED_)
