// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _SECURE_SCL	0
// Windows Header Files:
#include <windows.h>

#include <stdlib.h>

inline wchar_t* MB2WC(const char* apch){
	int iSLen = (int)strlen(apch);
	wchar_t* wsAttrValue=new wchar_t[iSLen+1];
	mbstowcs(wsAttrValue, apch, iSLen);
	wsAttrValue[iSLen]=0;
	return wsAttrValue;
}

class cLPWCSTR{
	wchar_t* mpwcStr;
public:
	cLPWCSTR(const char* apszStr){
		if(apszStr==NULL){ mpwcStr=NULL; return; }
		mpwcStr=MB2WC(apszStr);
	}
	cLPWCSTR(char* apszStr){
		if(apszStr==NULL){ mpwcStr=NULL; return; }
		mpwcStr=MB2WC(apszStr);
	}
	operator const wchar_t*(){ return mpwcStr; }
	~cLPWCSTR(){ if(mpwcStr) delete mpwcStr; }
};

inline char* WC2MB(const wchar_t* apwc){
	int iSLen=wcslen(apwc);
	char* pch=new char[iSLen*MB_CUR_MAX+1];
	wcstombs(pch, apwc, iSLen);
	pch[iSLen]=0;
	return pch;
}

class cLPCSTR{
	char* mpchStr;
public:
	cLPCSTR(const wchar_t* apwcStr){
		if(apwcStr==NULL){ mpchStr=NULL; return; }
		mpchStr=WC2MB(apwcStr);
	}
	cLPCSTR(wchar_t* apwcStr){
		if(apwcStr==NULL){ mpchStr=NULL; return; }
		mpchStr=WC2MB(apwcStr);
	}
	operator char*(){ return mpchStr; }
	~cLPCSTR(){ if(mpchStr) delete mpchStr; }
};

// TODO: reference additional headers your program requires here
