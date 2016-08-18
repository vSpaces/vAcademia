// nrmCommon.cpp: implementation of the nrmCommon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Common.h"
#include <locale>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


char* WC2MB(const wchar_t* apwc)
{
	int iSLen=wcslen(apwc);
	char* pch = MP_NEW_ARR( char, iSLen*MB_CUR_MAX+1);
	size_t convertedCnt = 0;
	wcstombs_s(&convertedCnt, pch, iSLen*MB_CUR_MAX+1, apwc, iSLen);
	pch[iSLen]=0;
	return pch;
}

wchar_t* MB2WC(const char* apch)
{
	int iSLen=strlen(apch);
	wchar_t* wsAttrValue = MP_NEW_ARR( wchar_t, iSLen+1);
	size_t convertedCnt = 0;
	mbstowcs_s(&convertedCnt, wsAttrValue, iSLen+1, apch, iSLen);
	wsAttrValue[iSLen]=0;
	return wsAttrValue;
}
