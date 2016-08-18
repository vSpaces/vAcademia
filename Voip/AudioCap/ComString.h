// CComString.h : header file
//
// CComString Header
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

#if !defined(AFX_COMSTRING_H__C514A391_FB4C_11D3_AABC_0000E215F0C2__INCLUDED_)
#define AFX_COMSTRING_H__C514A391_FB4C_11D3_AABC_0000E215F0C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CComString  
{
public: 
	void CreateString( LPCTSTR aStrData, int aDataLen);
	//void ReleaseBuffer(int count = -1);
	LPTSTR GetBuffer(int count = -1);
	CComString();
	CComString(const CComString& str);
	CComString(LPCTSTR pszString);
	CComString(BSTR bstrString);
	CComString(TCHAR ch, int nRepeat);
	CComString(LPCSTR aCharData, int aDataLen);
	CComString(HINSTANCE aInstance, UINT aResourceID);
	virtual ~CComString();

	bool LoadString(HINSTANCE hInst, UINT nID);

	// Assignment Operations
	const CComString& operator=(const CComString& strSrc);
	const CComString& operator=(LPCTSTR lpsz);
	const CComString& operator=(BSTR bstrStr);
	operator LPCTSTR() const	{ return m_pszString; }
	LPCTSTR	GetString() const	{ return m_pszString; }
	BSTR	AllocSysString();
	
	// Concatenation
	const CComString& operator+=(CComString& strSrc);
	const CComString& operator+=(LPCTSTR lpsz);
	const CComString& operator+=(BSTR bstrStr);
	const CComString& operator+=(TCHAR ch);

	friend CComString operator+(CComString& strSrc1, CComString& strSrc2);
	//friend CComString operator+(CComString& strSrc, LPCTSTR lpsz);
	//friend CComString operator+(LPCTSTR lpsz, CComString& strSrc);
	friend CComString operator+(CComString& strSrc, BSTR bstrStr);
	friend CComString operator+(const CComString& strSrc1, const CComString& strSrc2);
	friend CComString operator+(BSTR bstrStr, CComString& strSrc);

	// Insertion
	void	Insert(int nIndex, TCHAR ch);
	void	Insert(int nIndex, LPCTSTR lpszStr);
	void	Insert(int nIndex, const CComString& strSrc);
	void	Insert(int nIndex, BSTR bstrStr);

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
	int		Find(TCHAR ch)					const;
	int		Find(TCHAR ch, int nStart)		const;
	int     FindChar(TCHAR ch)				const;
	int     ReverseFind(TCHAR ch)			const;
	int     FindChar(TCHAR ch, int nStart)	const;
	int		FindOneOf(LPCTSTR lpszCharSet)	const;
	int		Find(LPCTSTR lpszSub);
	int		Find(LPCTSTR lpszSub, int nStart);

	// Extraction
	CComString Mid(int nFirst) const;
	CComString Mid(int nFirst, int nCount) const;
	CComString Left(int nCount) const;
	CComString Right(int nCount) const;
	CComString SpanIncluding(LPCTSTR lpszCharSet) const;
	CComString SpanExcluding(LPCTSTR lpszCharSet) const;

	// Comparison
	int Compare(CComString& str) const;
	int Compare(LPCTSTR lpsz) const;
	int CompareNoCase(CComString& str) const;
	int CompareNoCase(LPCTSTR lpsz) const;
	int Collate(CComString& str) const;
	int Collate(LPCTSTR lpsz) const;

        const BOOL operator>(CComString& strSrc);
        const BOOL operator<(CComString& strSrc);
//        bool operator<(const CComString& strSrc);
//        friend BOOL operator<(const CComString& str1, const CComString& str2);
//        friend BOOL operator>(const CComString& str1, const CComString& str2);

	// Formatting
	void Format(LPCTSTR pszCharSet, ...);
	void FormatV(LPCTSTR pszCharSet, va_list vl);

	// Replacing
	int Replace(TCHAR chOld, TCHAR chNew);
	int Replace(LPCTSTR lpszOld, LPCTSTR lpszNew);

	// Deleting
	int Delete(int nIndex, int nCount = 1);
	
	LPTSTR GetBufferSetLength(int count);

protected:
	LPTSTR	m_pszString;
	void	StringCopy(CComString& str, int nLen, int nIndex, int nExtra) const;
	void	StringCopy(int nSrcLen, LPCTSTR lpszSrcData);
	void	ConcatCopy(LPCTSTR lpszData);
	void	ConcatCopy(TCHAR ch);
	void	ConcatCopy(LPCTSTR lpszData1, LPCTSTR lpszData2);
	void	AllocString(int nLen);
	void	ReAllocString(int nLen);
};	

// Compare operations
bool operator==(const CComString& s1, const CComString& s2);
bool operator==(const CComString& s1, LPCTSTR s2);
bool operator==(LPCTSTR s1, const CComString& s2);
bool operator!=(const CComString& s1, const CComString& s2);
bool operator!=(const CComString& s1, LPCTSTR s2);
bool operator!=(LPCTSTR s1, const CComString& s2);
bool operator<(const CComString& s1, const CComString& s2);
bool operator<(const CComString& s1, LPCTSTR s2);
bool operator<(LPCTSTR s1, const CComString& s2);
bool operator>(const CComString& s1, const CComString& s2);
bool operator>(const CComString& s1, LPCTSTR s2);
bool operator>(LPCTSTR s1, const CComString& s2);

// Compare implementations
inline bool operator==(const CComString& s1, const CComString& s2)
	{ return s1.Compare(s2) == 0; }
inline bool operator==(const CComString& s1, LPCTSTR s2)
	{ return s1.Compare(s2) == 0; }
inline bool operator==(LPCTSTR s1, const CComString& s2)
	{ return s2.Compare(s1) == 0; }
inline bool operator!=(const CComString& s1, const CComString& s2)
	{ return s1.Compare(s2) != 0; }
inline bool operator!=(const CComString& s1, LPCTSTR s2)
	{ return s1.Compare(s2) != 0; }
inline bool operator!=(LPCTSTR s1, const CComString& s2)
	{ return s2.Compare(s1) != 0; }
inline bool operator<(const CComString& s1, const CComString& s2)
	{ return s1.Compare(s2) < 0; }
inline bool operator<(const CComString& s1, LPCTSTR s2)
	{ return s1.Compare(s2) < 0; }
inline bool operator<(LPCTSTR s1, const CComString& s2)
	{ return s2.Compare(s1) > 0; }
inline bool operator>(const CComString& s1, const CComString& s2)
	{ return s1.Compare(s2) > 0; }
inline bool operator>(const CComString& s1, LPCTSTR s2)
	{ return s1.Compare(s2) > 0; }
inline bool operator>(LPCTSTR s1, const CComString& s2)
	{ return s2.Compare(s1) < 0; }

#endif // !defined(AFX_COMSTRING_H__C514A391_FB4C_11D3_AABC_0000E215F0C2__INCLUDED_)
