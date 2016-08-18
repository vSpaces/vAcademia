// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#include "res_consts.h"

//#include "ComString.h"

#ifndef countof
#define countof(x)	(sizeof(x)/sizeof(x[0]))
#endif

#define WIN32_LEAN_AND_MEAN

#include <atlbase.h>
#include <atlapp.h>
#include "ComString.h"

//extern CAppModule _Module;

#include <atlwin.h>

#define MAX_PATH2          3120

/*#define _SECURE_SCL 0
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <atlbase.h>*/
#include "../../common/comstring.h"

#include <crtdbg.h>

/*#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#endif*/

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlmisc.h>
#include <windows.h>

extern TCHAR lpcIniFullPathName[MAX_PATH*2+2];
extern TCHAR glpcRootPath[MAX_PATH*2+2];
extern TCHAR glpcRepositoryPath[MAX_PATH*2+2];
extern TCHAR glpcUserPath[MAX_PATH*2+2];
extern TCHAR glpcUIPath[MAX_PATH*2+2];
extern TCHAR glpcDownloadManager[MAX_PATH*2+2];
extern TCHAR glpcComponentsPath[MAX_PATH*2+2];
extern CComString m_NewNersion;
extern CComString ucCurentVersion;

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

HMODULE loadDLL( LPCSTR alpcRelPath, LPCSTR alpcName);




