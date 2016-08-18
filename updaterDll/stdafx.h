// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

/*#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x400
#endif*/

#define VC_SETTINGS

#include "res_consts.h"

#ifndef countof
#define countof(x)	(sizeof(x)/sizeof(x[0]))
#endif

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0502

#define MDUMP_STATIC_LIB 1
#define RTL_STATIC_LIB 1
#define LOGWRITER_STATIC_LIB 1

// Windows Header Files:
#include "std_define.h"

#include <windows.h>
#include <atlbase.h>

/*#if( defined(DEBUG) || defined(_DEBUG))
	#define UPDATERDLL_BUILD
	#include "..\common\memprofiling.h"
	#undef UPDATERDLL_BUILD
#else*/
	#include "../nmemoryprofiler/MemoryProfilerDefines.h"
//#endif

//#include "ComString.h"
#include "rtl.h"

#define MAX_PATH2          3120

extern WCHAR lpcIniFullPathName[MAX_PATH*2+2];
extern WCHAR glpcRootPath[MAX_PATH*2+2];
extern WCHAR glpcRepositoryPath[MAX_PATH*2+2];
extern WCHAR glpcUserPath[MAX_PATH*2+2];
extern TCHAR glpcUIPath[MAX_PATH*2+2];
extern TCHAR glpcDownloadManager[MAX_PATH*2+2];
extern TCHAR glpcComponentsPath[MAX_PATH*2+2];
extern CComString m_NewNersion;
extern CComString ucCurentVersion;
extern CComString m_Lang;
extern CComString m_LanguageIni;
extern CWComString m_ClientPath;
extern BOOL m_IsRequiredUpdate;
extern BOOL m_IsNeedDownloadSetupOnly;
extern BOOL m_IsBrokenClient;

#define	LOGIN_SERVER_STATUS_BIT		0x1
#define	RES_SERVER_STATUS_BIT		0x2

inline BOOL BSTR2TSTR(LPTSTR pDst, BSTR pSrc, int nSize)
{
	if(pDst == NULL || pSrc == NULL)
		return FALSE;
#ifndef _UNICODE
	int nLength = SysStringLen(pSrc);
	if(nLength >= nSize)
		nLength = nSize - 1;
	nLength = WideCharToMultiByte(CP_ACP, 0, pSrc, nLength, pDst, nLength, NULL, NULL);
	pDst[nLength] = 0;
	return TRUE;
#else
	lstrcpyn(pDst,pSrc, nSize);
	pDst[nSize] = 0;
	return TRUE;
#endif
}

inline BOOL TSTR2BSTR(BSTR pDst, TCHAR* pSrc, int nSize)
{
	if(pDst == NULL || pSrc == NULL)
		return FALSE;
#ifndef _UNICODE
	int nLength = lstrlen(pSrc);
	if(nLength >= nSize)
		nLength = nSize - 1;
	nLength = MultiByteToWideChar(CP_ACP, 0, pSrc, nLength, pDst, nLength);
	pDst[nLength] = 0;
	return TRUE;
#else
	lstrcpyn(pDst,pSrc, nSize);
	pDst[nSize] = 0;
	return TRUE;
#endif
}

//HMODULE loadDLL( LPCSTR alpcRelPath, LPCSTR alpcName);