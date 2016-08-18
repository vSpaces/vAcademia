// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__C9F56FBE_38B7_45D1_9AA1_0EE6B8CBA393__INCLUDED_)
#define AFX_STDAFX_H__C9F56FBE_38B7_45D1_9AA1_0EE6B8CBA393__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning (disable:4786)
#pragma warning (disable:4702)
#pragma warning (disable:4201)

/*#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x400
#endif*/

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define MAP_MANAGER 1

#define USE_3D_ANIMATION				1
#define USE_PHYSICS						1
#define USE_GEOMETRY_OPTIMIZATION		1
#include "std_define.h"

#define RENDERMANAGER_BUILD
#include "..\..\common\memprofiling.h"
#undef RENDERMANAGER_BUILD

#include <windows.h>
#include <atlbase.h>
#include "rtl.h"

// TODO: reference additional headers your program requires here
class CNatura3DModule;
extern	CNatura3DModule _Module;
#define RM_EXPORTS
#include "nrm.h"

#include "..\oms\oms_context.h"
#include "rmext.h"
#include "res.h"

#include "GlobalSingletonStorage.h"
#include "RMContext.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#include "rmext.h"
#include "rm.h"
#include "sm.h"
#include "res.h"
#ifndef csclRM_API
#include "nrm.h"
#endif

// User data constants
#define MLMEDIA_FROM_VISIBLEOBJECT	1

// Natura3D overrides
#include "n3d3DObject.h"
#include "n3dGroup.h"

// render manager classes
//#include "crmResourceFile.h"
//#include "crmMemResourceFile.h"

#include "../../Common/Common.h"
#include "../../Common/ViskoeThread.h"
void	rm_trace(const char* tx,...);

#endif // !defined(AFX_STDAFX_H__C9F56FBE_38B7_45D1_9AA1_0EE6B8CBA393__INCLUDED_)
