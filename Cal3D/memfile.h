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
	CalMemFile();
	virtual ~CalMemFile();

public:
	void init(void* apData, DWORD adwSize);

	template <class T>
	__forceinline bool readType(T& apValue)
	{
		if( dwLocation + sizeof(T) >= dwSize)
			return false;
		apValue = *(T*)((DWORD) lpData + dwLocation);
		dwLocation += sizeof(T);
		return true;
	}

	__forceinline DWORD read(void *lpBuf, DWORD adwSize)
	{
		DWORD readed = adwSize;

		if( dwLocation + adwSize >= dwSize)
			readed = dwSize - dwLocation;
		memcpy( lpBuf, (void*)((DWORD) lpData + dwLocation), readed);

		dwLocation += readed;

		return readed;
	}

	__forceinline bool eof()
	{
		return (dwLocation > dwSize);
	}

private:
	DWORD	dwLocation;
	void*	lpData;
	DWORD	dwSize;
};

#endif // !defined(AFX_MEMFILE_H__B6A75286_1872_4692_A800_43BDC2097A93__INCLUDED_)
