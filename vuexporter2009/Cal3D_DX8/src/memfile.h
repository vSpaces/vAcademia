// memfile.h: interface for the CalMemFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMFILE_H__B6A75286_1872_4692_A800_43BDC2097A93__INCLUDED_)
#define AFX_MEMFILE_H__B6A75286_1872_4692_A800_43BDC2097A93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "global.h"

class CalMemFile  
{
public:
	bool eof();
	void init(void* apData, DWORD adwSize);
	DWORD read(void* lpBuf, DWORD adwSize);
	CalMemFile();
	virtual ~CalMemFile();

private:
	DWORD	dwLocation;
	void*	lpData;
	DWORD	dwSize;
};

#endif // !defined(AFX_MEMFILE_H__B6A75286_1872_4692_A800_43BDC2097A93__INCLUDED_)
