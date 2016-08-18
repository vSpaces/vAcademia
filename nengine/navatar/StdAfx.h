// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__53B0E23C_F41F_4DF9_9746_5CAC98D65884__INCLUDED_)
#define AFX_STDAFX_H__53B0E23C_F41F_4DF9_9746_5CAC98D65884__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include "std_define.h"

#include <windows.h>
#include <atlbase.h>

#include "..\..\rtl\rtl.h"

#pragma  warning (disable: 4702)

#define NAVATAR_BUILD
#include "../../../common/memprofiling.h"
#undef NAVATAR_BUILD

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__53B0E23C_F41F_4DF9_9746_5CAC98D65884__INCLUDED_)
