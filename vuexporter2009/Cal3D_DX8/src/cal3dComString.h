// Cal3DComString.h : header file
//
// Cal3DComString Header
//
// Written by Paul E. Bible <pbible@littlefishsoftware.com>
// Copyright (c) 2000. All Rights Reserved.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. If the source code in this file is used in 
// any  commercial application then a statement along the lines of 
// "Portions copyright (c) Paul E. Bible, 2000" must be included in
// the startup banner, "About" box -OR- printed documentation. An email 
// letting me know that you are using it would be nice as well. That's 
// not much to ask considering the amount of work that went into this.
// If even this small restriction is a problem send me an email.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// Expect bugs!
// 
// Please use and enjoy, and let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into 
// this file. 
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(CAL3D_COMSTRING_INCLUDED)
#define CAL3D_COMSTRING_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "windows.h"
#include "platform.h"

class CAL3D_API Cal3DComString
{
public: 
	void ReleaseBuffer(int count=-1);
	LPTSTR GetBuffer(int count=0)	const;
	LPTSTR GetBufferSetLength(int count);
	Cal3DComString();
	Cal3DComString(const Cal3DComString& szString);
	Cal3DComString(Cal3DComString& szString);
	Cal3DComString(LPCTSTR pszString);
	Cal3DComString(BSTR bstrString);
	Cal3DComString(TCHAR ch, int nRepeat);
	virtual ~Cal3DComString();
	
	// Assignment Operations
	const Cal3DComString& operator=(Cal3DComString& strSrc);
	const Cal3DComString& operator=(LPCTSTR lpsz);
	const Cal3DComString& operator=(BSTR bstrStr);
	operator LPCTSTR() const	{ return m_pszString; }
	TCHAR*	GetString() const		{ return m_pszString; }
	BSTR	AllocSysString();
	
	// Concatenation
	const Cal3DComString& operator+=(Cal3DComString& strSrc);
	const Cal3DComString& operator+=(LPCTSTR lpsz);
	const Cal3DComString& operator+=(BSTR bstrStr);
	const Cal3DComString& operator+=(TCHAR ch);
	friend Cal3DComString CAL3D_API operator+(Cal3DComString& strSrc1, Cal3DComString& strSrc2);
	friend Cal3DComString CAL3D_API operator+(const Cal3DComString& strSrc1, const Cal3DComString& strSrc2);
	friend Cal3DComString CAL3D_API operator+(Cal3DComString& strSrc, LPCTSTR lpsz);
	friend Cal3DComString CAL3D_API operator+(LPCTSTR lpsz, Cal3DComString& strSrc);
	friend Cal3DComString CAL3D_API operator+(Cal3DComString& strSrc, BSTR bstrStr);
	friend Cal3DComString CAL3D_API operator+(BSTR bstrStr, Cal3DComString& strSrc);

	// Accessors for the String as an Array
	int		GetLength() const;
	bool	IsEmpty() const;
	void	Empty();
	TCHAR	GetAt(int nIndex);
	void	SetAt(int nIndex, TCHAR ch);
	TCHAR	operator[] (int nIndex);

	// Conversions
	void	MakeUpper();
	void	MakeLower();
	void	MakeReverse();
	void	TrimLeft();
	void	TrimRight();

	// Searching
	int		FindChar(TCHAR ch) const;
	int		ReverseFind(TCHAR ch) const;
	int		FindChar(TCHAR ch, int nStart) const;
	int		Find(LPCTSTR lpszSub);
	int		Find(LPCTSTR lpszSub, int nStart);
	int		FindOneOf(LPCTSTR lpszCharSet) const;

	// Extraction
	Cal3DComString Mid(int nFirst) const;
	Cal3DComString Mid(int nFirst, int nCount) const;
	Cal3DComString Left(int nCount) const;
	Cal3DComString Right(int nCount) const;
	Cal3DComString SpanIncluding(LPCTSTR lpszCharSet) const;
	Cal3DComString SpanExcluding(LPCTSTR lpszCharSet) const;

	// Comparison
	int Compare(Cal3DComString& str) const;
	int Compare(LPCTSTR lpsz) const;
	int CompareNoCase(Cal3DComString& str) const;
	int CompareNoCase(LPCTSTR lpsz) const;
	int Collate(Cal3DComString& str) const;
	int Collate(LPCTSTR lpsz) const;

	const BOOL operator>(Cal3DComString& strSrc);
	const BOOL operator<(Cal3DComString& strSrc);
	bool operator<(const Cal3DComString& strSrc);
	friend BOOL operator<(const Cal3DComString& str1, const Cal3DComString& str2);
	friend BOOL operator>(const Cal3DComString& str1, const Cal3DComString& str2);

	// Formatting
	void Format(LPCTSTR pszCharSet, ...);

	// Replacing
	int Replace(TCHAR chOld, TCHAR chNew);
	int Replace(LPCTSTR lpszOld, LPCTSTR lpszNew);
	
protected:
	LPTSTR	m_pszString;
	void	StringCopy(Cal3DComString& str, int nLen, int nIndex, int nExtra) const;
	void	StringCopy(int nSrcLen, LPCTSTR lpszSrcData);
	void	ConcatCopy(LPCTSTR lpszData);
	void	ConcatCopy(TCHAR ch);
	void	ConcatCopy(LPCTSTR lpszData1, LPCTSTR lpszData2);
	void	AllocString(int nLen);
	void	ReAllocString(int nLen);
};	

// Compare operations
bool operator==(const Cal3DComString& s1, const Cal3DComString& s2);
bool operator==(const Cal3DComString& s1, LPCTSTR s2);
bool operator==(LPCTSTR s1, const Cal3DComString& s2);
bool operator!=(const Cal3DComString& s1, const Cal3DComString& s2);
bool operator!=(const Cal3DComString& s1, LPCTSTR s2);
bool operator!=(LPCTSTR s1, const Cal3DComString& s2);

// Compare implementations
inline bool operator==(const Cal3DComString& s1, const Cal3DComString& s2)
	{ return s1.Compare(s2) == 0; }
inline bool operator==(const Cal3DComString& s1, LPCTSTR s2)
	{ return s1.Compare(s2) == 0; }
inline bool operator==(LPCTSTR s1, const Cal3DComString& s2)
	{ return s2.Compare(s1) == 0; }
inline bool operator!=(const Cal3DComString& s1, const Cal3DComString& s2)
	{ return s1.Compare(s2) != 0; }
inline bool operator!=(const Cal3DComString& s1, LPCTSTR s2)
	{ return s1.Compare(s2) != 0; }
inline bool operator!=(LPCTSTR s1, const Cal3DComString& s2)
	{ return s2.Compare(s1) != 0; }

#endif // !defined(CAL3D_COMSTRING_INCLUDED)
