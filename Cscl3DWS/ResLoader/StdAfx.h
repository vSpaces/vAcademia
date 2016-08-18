// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__83D5821D_B961_4246_B416_4C04B0095824__INCLUDED_)
#define AFX_STDAFX_H__83D5821D_B961_4246_B416_4C04B0095824__INCLUDED_

#include "std_define.h"
#include <windows.h>
#include <atlbase.h>

#define RESLOADER_BUILD
#include "..\..\common\memprofiling.h"
#undef RESLOADER_BUILD

#include "rtl.h"

#include "..\oms\oms_context.h"
#include "res.h"
#include "crmResourceFile.h"
#include "crmMemResourceFile.h"
#include "crmResLibrary.h"
#include "ComMan.h"
#include "../../Common/Common.h"

#define		MIN_DOWNLOAD_THREAD_COUNT	6
#define		MAX_DOWNLOAD_THREAD_COUNT	16
#define		DOWNLOAD_SCHEME_COUNT		2
#define		ZONES_COUNT					11

#define		RESLOADER_BUILD_H			1

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#endif

#endif 
