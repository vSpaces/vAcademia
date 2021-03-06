// Cal3DComString.cpp : implementation file
//
// Cal3DComString Implementation
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "cal3dComString.h"
#include <atlbase.h>
#include <TCHAR.H>
#include <stdio.h>


//////////////////////////////////////////////////////////////////////
//
// Constructors/Destructor
//
//////////////////////////////////////////////////////////////////////
Cal3DComString::Cal3DComString()
{
	m_pszString = NULL;
	AllocString(0);
}

Cal3DComString::Cal3DComString(const Cal3DComString& str)
{
	m_pszString = NULL;
	StringCopy(str.GetLength(), str.GetString());
}

Cal3DComString::Cal3DComString(Cal3DComString& str)
{
	m_pszString = NULL;
	StringCopy(str.GetLength(), str.GetString());
}

Cal3DComString::Cal3DComString(LPCTSTR pszString)
{
	m_pszString = NULL;
	int nLen = _tcslen(pszString);
	AllocString(nLen);
	_tcsncpy(m_pszString, pszString, nLen);
}

Cal3DComString::Cal3DComString(BSTR bstrString)
{
	USES_CONVERSION;
	m_pszString = NULL;
	int nLen = _tcslen(OLE2T(bstrString));
	AllocString(nLen);
	_tcsncpy(m_pszString, OLE2T(bstrString), nLen);
}

Cal3DComString::Cal3DComString(TCHAR ch, int nRepeat) 
{
	m_pszString = NULL;
	if (nRepeat > 0)
	{
		AllocString(nRepeat);
#ifdef _UNICODE
		for (int i=0; i < nRepeat; i++)
			m_pszString[i] = ch;
#else
		memset(m_pszString, ch, nRepeat);
#endif
	}
}

Cal3DComString::~Cal3DComString()
{
	free(m_pszString);
}


//////////////////////////////////////////////////////////////////////
//
// Memory Manipulation
//
//////////////////////////////////////////////////////////////////////

void Cal3DComString::AllocString(int nLen)
{
	ATLASSERT(nLen >= 0);
	if (m_pszString != NULL)
		free(m_pszString);
	m_pszString = (TCHAR*) calloc((nLen+1) * sizeof(TCHAR), 1);
	int ii=sizeof(TCHAR);
	ATLASSERT(m_pszString != NULL);
	m_pszString[nLen] = '\0';
}

void Cal3DComString::ReAllocString(int nLen)
{
	ATLASSERT(nLen >= 0);
	if (m_pszString != NULL)
	{
		free(m_pszString);
		m_pszString = NULL;
	}
	m_pszString = (TCHAR*) realloc(m_pszString, (nLen+1) * sizeof(TCHAR));
	ATLASSERT(m_pszString != NULL);
	m_pszString[nLen] = '\0';
}

void Cal3DComString::StringCopy(int nSrcLen, LPCTSTR lpszSrcData)
{
	AllocString(nSrcLen);
	memcpy(m_pszString, lpszSrcData, nSrcLen * sizeof(TCHAR));
	m_pszString[nSrcLen] = '\0';
}

void Cal3DComString::StringCopy(Cal3DComString &str, int nLen, int nIndex, int nExtra) const
{
	int nNewLen = nLen + nExtra;
	if (nNewLen != 0)
	{
		str.AllocString(nNewLen);
		memcpy(str.GetString(), m_pszString+nIndex, nLen * sizeof(TCHAR));
	}
}

void Cal3DComString::ConcatCopy(LPCTSTR lpszData)
{
	ATLASSERT(lpszData != NULL);

	// Save the existing string
	int nLen = GetLength();
	TCHAR* pszTemp = (TCHAR*) malloc((nLen+1) * sizeof(TCHAR));
	memcpy(pszTemp, m_pszString, nLen * sizeof(TCHAR));
	pszTemp[nLen] = '\0';

	// Calculate the new string length and realloc memory
	int nDataLen = _tcslen(lpszData);
	int nNewLen = nLen + nDataLen;
	ReAllocString(nNewLen);

	// Copy the strings into the new buffer
	memcpy(m_pszString, pszTemp, nLen * sizeof(TCHAR));
	memcpy(m_pszString+nLen, lpszData, nDataLen * sizeof(TCHAR));

	// Cleanup
	free(pszTemp);
}

void Cal3DComString::ConcatCopy(TCHAR ch)
{
	// Save the existing string
	int nLen = GetLength();
	TCHAR* pszTemp = (TCHAR*) malloc((nLen+1) * sizeof(TCHAR));
	memcpy(pszTemp, m_pszString, nLen * sizeof(TCHAR));
	pszTemp[nLen] = '\0';

	// Calculate the new string length and realloc memory
	int nNewLen = nLen + 1;
	ReAllocString(nNewLen);

	// Copy the strings into the new buffer
	memcpy(m_pszString, pszTemp, nLen * sizeof(TCHAR));
	m_pszString[nNewLen-1] = ch;

	// Cleanup
	//free(pszTemp);
}

void Cal3DComString::ConcatCopy(LPCTSTR lpszData1, LPCTSTR lpszData2)
{
	ATLASSERT(lpszData1 != NULL);
	ATLASSERT(lpszData2 != NULL);
	int nLen1 = _tcslen(lpszData1);
	int nLen2 = _tcslen(lpszData2);
	int nLen = nLen1 + nLen2;
	AllocString(nLen);
	memcpy(m_pszString, lpszData1, nLen1 * sizeof(TCHAR));
	memcpy(m_pszString+nLen1, lpszData2, nLen2 * sizeof(TCHAR)); 
}

BSTR Cal3DComString::AllocSysString()
{
#ifdef _UNICODE
	BSTR bstr = ::SysAllocStringLen(m_pszString, GetLength());
	if (bstr == NULL)
	{
		ATLASSERT(0);
		return NULL;
	}
#else
	int nLen = MultiByteToWideChar(CP_ACP, 0, m_pszString, GetLength(), NULL, NULL);
	BSTR bstr = ::SysAllocStringLen(NULL, nLen);
	if (bstr == NULL)
	{
		ATLASSERT(0);
		return NULL;
	}
	MultiByteToWideChar(CP_ACP, 0, m_pszString, GetLength(), bstr, nLen);
#endif

	return bstr;
}


//////////////////////////////////////////////////////////////////////
//
// Accessors for the String as an Array
//
//////////////////////////////////////////////////////////////////////

void Cal3DComString::Empty()
{
	if (_tcslen(m_pszString) > 0)
		m_pszString[0] = '\0';
}

TCHAR Cal3DComString::GetAt(int nIndex)
{
	int nLen = _tcslen(m_pszString);
	ATLASSERT(nIndex >= 0);
	ATLASSERT(nIndex < nLen);
	return m_pszString[nIndex];
}

TCHAR Cal3DComString::operator[] (int nIndex)
{
	int nLen = _tcslen(m_pszString);
	ATLASSERT(nIndex >= 0);
	ATLASSERT(nIndex < nLen);
	return m_pszString[nIndex];
}

void Cal3DComString::SetAt(int nIndex, TCHAR ch)
{
	int nLen = _tcslen(m_pszString);
	ATLASSERT(nIndex >= 0);
	ATLASSERT(nIndex < nLen);
	m_pszString[nIndex] = ch;	
}

int Cal3DComString::GetLength() const
{ 
	return _tcslen(m_pszString); 
}

bool Cal3DComString::IsEmpty() const
{ 
	return (GetLength() > 0) ? false : true; 
}


//////////////////////////////////////////////////////////////////////
//
// Conversions
//
//////////////////////////////////////////////////////////////////////

void Cal3DComString::MakeUpper()
{
	_tcsupr(m_pszString);
}

void Cal3DComString::MakeLower()
{
	_tcslwr(m_pszString);
}

void Cal3DComString::MakeReverse()
{
	_tcsrev(m_pszString);
}

void Cal3DComString::TrimLeft()
{
	LPTSTR lpsz = m_pszString;

	while (_istspace(*lpsz))
		lpsz = _tcsinc(lpsz);

	if (lpsz != m_pszString)
	{
		memmove(m_pszString, lpsz, (GetLength()+1) * sizeof(TCHAR));
	}
}

void Cal3DComString::TrimRight()
{
	LPTSTR lpsz = m_pszString;
	LPTSTR lpszLast = NULL;

	while (*lpsz != '\0')
	{
		if (_istspace(*lpsz))
		{
			if (lpszLast == NULL)
				lpszLast = lpsz;
		}
		else
			lpszLast = NULL;
		lpsz = _tcsinc(lpsz);
	}

	if (lpszLast != NULL)
		*lpszLast = '\0';
}


//////////////////////////////////////////////////////////////////////
//
// Searching
//
//////////////////////////////////////////////////////////////////////

int Cal3DComString::FindChar(TCHAR ch) const
{
	return FindChar(ch, 0);
}

int Cal3DComString::ReverseFind(TCHAR ch) const
{
	// find last single character
	LPTSTR lpsz = _tcsrchr(m_pszString, (_TUCHAR) ch);

	// return -1 if not found, distance from beginning otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pszString);
}

int Cal3DComString::FindChar(TCHAR ch, int nStart) const
{
	if (nStart >= GetLength())
		return -1;
	LPTSTR lpsz = _tcschr(m_pszString + nStart, (_TUCHAR) ch);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pszString);
}

int Cal3DComString::Find(LPCTSTR lpszSub)
{
	return Find(lpszSub, 0);
}

int Cal3DComString::Find(LPCTSTR lpszSub, int nStart)
{
	ATLASSERT(_tcslen(lpszSub) > 0);

	if (nStart > GetLength())
		return -1;

	LPTSTR lpsz = _tcsstr(m_pszString + nStart, lpszSub);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pszString);
}

int Cal3DComString::FindOneOf(LPCTSTR lpszCharSet) const
{
	LPTSTR lpsz = _tcspbrk(m_pszString, lpszCharSet);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pszString);
}


//////////////////////////////////////////////////////////////////////
//
// Assignment Operations
//
//////////////////////////////////////////////////////////////////////

const Cal3DComString& Cal3DComString::operator=(Cal3DComString& strSrc)
{
	if (m_pszString != strSrc.GetString())
		StringCopy(strSrc.GetLength(), strSrc.GetString());
	return *this;
}

const BOOL Cal3DComString::operator>(Cal3DComString& strSrc)
{
	return TRUE;
}

const BOOL Cal3DComString::operator<(Cal3DComString& strSrc)
{
	return Compare(strSrc);
}

BOOL operator<(const Cal3DComString& str1, const Cal3DComString& str2)
{
	return str1.Compare(str2);
}

BOOL operator>(const Cal3DComString& str1, const Cal3DComString& str2)
{
	return str1.Compare(str2);
}

bool Cal3DComString::operator<(const Cal3DComString& strSrc)
{
	return Compare(strSrc);
}

const Cal3DComString& Cal3DComString::operator=(LPCTSTR lpsz)
{
	ATLASSERT(lpsz != NULL);
	StringCopy(_tcslen(lpsz), lpsz);
	return *this;
}

const Cal3DComString& Cal3DComString::operator=(BSTR bstrStr)
{
	USES_CONVERSION;
	int nLen = _tcslen(OLE2T(bstrStr));
	StringCopy(nLen, OLE2T(bstrStr));
	return *this;
}


//////////////////////////////////////////////////////////////////////
//
// Concatenation
//
//////////////////////////////////////////////////////////////////////

Cal3DComString CAL3D_API operator+(Cal3DComString& strSrc1, Cal3DComString& strSrc2)
{
	Cal3DComString s;
	s.ConcatCopy((LPCTSTR) strSrc1, (LPCTSTR) strSrc2);
	return s;
}

Cal3DComString CAL3D_API operator+(const Cal3DComString& strSrc1, const Cal3DComString& strSrc2)
{
	Cal3DComString s;
	s.ConcatCopy((LPCTSTR) strSrc1, (LPCTSTR) strSrc2);
	return s;
}

Cal3DComString CAL3D_API operator+(Cal3DComString& strSrc, LPCTSTR lpsz)
{
	Cal3DComString s;
	s.ConcatCopy((LPCTSTR) strSrc, lpsz);
	return s;
}

Cal3DComString CAL3D_API operator+(LPCTSTR lpsz, Cal3DComString& strSrc)
{
	Cal3DComString s;
	s.ConcatCopy(lpsz, (LPCTSTR) strSrc);
	return s;
}

Cal3DComString CAL3D_API operator+(Cal3DComString& strSrc, BSTR bstrStr)
{
	USES_CONVERSION;
	Cal3DComString s;
	s.ConcatCopy((LPCTSTR) strSrc, OLE2T(bstrStr));
	return s;
}

Cal3DComString CAL3D_API operator+(BSTR bstrStr, Cal3DComString& strSrc)
{
	USES_CONVERSION;
	Cal3DComString s;
	s.ConcatCopy(OLE2T(bstrStr), (LPCTSTR) strSrc);
	return s;
}

const Cal3DComString& Cal3DComString::operator+=(Cal3DComString& strSrc)
{
	ConcatCopy((LPCTSTR) strSrc);
	return *this;
}

const Cal3DComString& Cal3DComString::operator+=(LPCTSTR lpsz)
{
	ConcatCopy(lpsz);
	return *this;
}

const Cal3DComString& Cal3DComString::operator+=(BSTR bstrStr)
{
	USES_CONVERSION;
	ConcatCopy(OLE2T(bstrStr));
	return *this;
}

const Cal3DComString& Cal3DComString::operator+=(TCHAR ch)
{
	USES_CONVERSION;
	ConcatCopy(ch);
	return *this;
}


//////////////////////////////////////////////////////////////////////
//
// Extraction
//
//////////////////////////////////////////////////////////////////////

Cal3DComString Cal3DComString::Mid(int nFirst) const
{
	return Mid(nFirst, GetLength() - nFirst);	
}

Cal3DComString Cal3DComString::Mid(int nFirst, int nCount) const
{
	if (nFirst < 0)
		nFirst = 0;
	if (nCount < 0)
		nCount = 0;

	if (nFirst + nCount > GetLength())
		nCount = GetLength() - nFirst;
	if (nFirst > GetLength())
		nCount = 0;

	ATLASSERT(nFirst >= 0);
	ATLASSERT(nFirst + nCount <= GetLength());

	Cal3DComString newStr;
	StringCopy(newStr, nCount, nFirst, 0);
	return newStr;
}

Cal3DComString Cal3DComString::Left(int nCount) const
{
	if (nCount < 0)
		nCount = 0;

	Cal3DComString newStr;
	StringCopy(newStr, nCount, 0, 0);
	return newStr;
}

Cal3DComString Cal3DComString::Right(int nCount) const
{
	if (nCount < 0)
		nCount = 0;

	Cal3DComString newStr;
	StringCopy(newStr, nCount, GetLength() - nCount, 0);
	return newStr;
}

Cal3DComString Cal3DComString::SpanIncluding(LPCTSTR lpszCharSet) const
{
	ATLASSERT(lpszCharSet != NULL);
#ifdef _UNICODE
	ATLASSERT(::IsBadStringPtrW(lpszCharSet, -1) == 0);
#else
	ATLASSERT(::IsBadStringPtrA(lpszCharSet, -1) == 0);
#endif
	return Left(_tcsspn(m_pszString, lpszCharSet));
}

Cal3DComString Cal3DComString::SpanExcluding(LPCTSTR lpszCharSet) const
{
	ATLASSERT(lpszCharSet != NULL);
#ifdef _UNICODE
	ATLASSERT(::IsBadStringPtrW(lpszCharSet, -1) == 0);
#else
	ATLASSERT(::IsBadStringPtrA(lpszCharSet, -1) == 0);
#endif
	return Left(_tcscspn(m_pszString, lpszCharSet));
}


//////////////////////////////////////////////////////////////////////
//
// Comparison
//
//////////////////////////////////////////////////////////////////////
	
int Cal3DComString::Compare(Cal3DComString& str) const
{
	ATLASSERT((LPCTSTR) str != NULL);
#ifdef _UNICODE
	ATLASSERT(::IsBadStringPtrW((LPCTSTR) str, -1) == 0);
#else
	ATLASSERT(::IsBadStringPtrA((LPCTSTR) str, -1) == 0);
#endif
	return _tcscmp(m_pszString, (LPCTSTR) str);	
}

int Cal3DComString::Compare(LPCTSTR lpsz) const
{
	ATLASSERT(lpsz != NULL);
#ifdef _UNICODE
	ATLASSERT(::IsBadStringPtrW(lpsz, -1) == 0);
#else
	ATLASSERT(::IsBadStringPtrA(lpsz, -1) == 0);
#endif
	return _tcscmp(m_pszString, lpsz);	
}

int Cal3DComString::CompareNoCase(Cal3DComString& str) const
{
	ATLASSERT((LPCTSTR) str != NULL);
#ifdef _UNICODE
	ATLASSERT(::IsBadStringPtrW((LPCTSTR) str, -1) == 0);
#else
	ATLASSERT(::IsBadStringPtrA((LPCTSTR) str, -1) == 0);
#endif
	return _tcsicmp(m_pszString, (LPCTSTR) str);	
}

int Cal3DComString::CompareNoCase(LPCTSTR lpsz) const
{
	ATLASSERT(lpsz != NULL);
#ifdef _UNICODE
	ATLASSERT(::IsBadStringPtrW(lpsz, -1) == 0);
#else
	ATLASSERT(::IsBadStringPtrA(lpsz, -1) == 0);
#endif
	return _tcsicmp(m_pszString, lpsz);	
}

int Cal3DComString::Collate(LPCTSTR lpsz) const
{
	ATLASSERT(lpsz != NULL);
#ifdef _UNICODE
	ATLASSERT(::IsBadStringPtrW(lpsz, -1) == 0);
#else
	ATLASSERT(::IsBadStringPtrA(lpsz, -1) == 0);
#endif
	return _tcscoll(m_pszString, lpsz);	
}

int Cal3DComString::Collate(Cal3DComString &str) const
{
	ATLASSERT((LPCTSTR) str != NULL);
#ifdef _UNICODE
	ATLASSERT(::IsBadStringPtrW((LPCTSTR) str, -1) == 0);
#else
	ATLASSERT(::IsBadStringPtrA((LPCTSTR) str, -1) == 0);
#endif
	return _tcscoll(m_pszString, (LPCTSTR) str);	
}


//////////////////////////////////////////////////////////////////////
//
// Formatting
//
//////////////////////////////////////////////////////////////////////
	
void Cal3DComString::Format(LPCTSTR pszCharSet, ...)
{
	va_list vl;
	va_start(vl, pszCharSet);

	TCHAR* pszTemp = NULL;
	int nBufferSize = 0;
	int nRetVal = -1;

	do {
		nBufferSize += 100;
		delete [] pszTemp;
		pszTemp = new TCHAR[nBufferSize];
		nRetVal = _vsnprintf(pszTemp, nBufferSize, pszCharSet, vl);
	} while (nRetVal < 0);

	int nNewLen = _tcslen(pszTemp);
	AllocString(nNewLen);
	_tcscpy(m_pszString, pszTemp);
	delete [] pszTemp;

	va_end(vl);
}


//////////////////////////////////////////////////////////////////////
//
// Replacing
//
//////////////////////////////////////////////////////////////////////

int Cal3DComString::Replace(TCHAR chOld, TCHAR chNew)
{
	int nCount = 0;

	if (chOld != chNew)
	{
		LPTSTR psz = m_pszString;
		LPTSTR pszEnd = psz + GetLength();
		while(psz < pszEnd)
		{
			if (*psz == chOld)
			{
				*psz = chNew;
				nCount++;
			}
			psz = _tcsinc(psz);
		}
	}

	return nCount;
}

int Cal3DComString::Replace(LPCTSTR lpszOld, LPCTSTR lpszNew)
{
	int nSourceLen = _tcslen(lpszOld);
	if (nSourceLen == 0)
		return 0;
	int nReplaceLen = _tcslen(lpszNew);

	int nCount = 0;
	LPTSTR lpszStart = m_pszString;
	LPTSTR lpszEnd = lpszStart + GetLength();
	LPTSTR lpszTarget;

	// Check to see if any changes need to be made
	while (lpszStart < lpszEnd)
	{
		while ((lpszTarget = _tcsstr(lpszStart, lpszOld)) != NULL)
		{
			lpszStart = lpszTarget + nSourceLen;
			nCount++;
		}
		lpszStart += _tcslen(lpszStart) + 1;
	}

	// Do the actual work
	if (nCount > 0)
	{
		int nOldLen = GetLength();
		int nNewLen = nOldLen + (nReplaceLen - nSourceLen) * nCount;
		if (GetLength() < nNewLen)
		{
			Cal3DComString szTemp = m_pszString;
			ReAllocString(nNewLen);
			memcpy(m_pszString, (LPCTSTR) szTemp, nOldLen * sizeof(TCHAR));
		}

		lpszStart = m_pszString;
		lpszEnd = lpszStart + GetLength();

		while (lpszStart < lpszEnd)
		{
			while ((lpszTarget = _tcsstr(lpszStart, lpszOld)) != NULL)
			{
				int nBalance = nOldLen - (lpszTarget - m_pszString + nSourceLen);
				memmove(lpszTarget + nReplaceLen, lpszTarget + nSourceLen, nBalance * sizeof(TCHAR));
				memcpy(lpszTarget, lpszNew, nReplaceLen * sizeof(TCHAR));
				lpszStart = lpszTarget + nReplaceLen;
				lpszStart[nBalance] = '\0';
				nOldLen += (nReplaceLen - nSourceLen);
			}
			lpszStart += _tcslen(lpszStart) + 1;
		}
		ATLASSERT(m_pszString[GetLength()] == '\0');
	}
	return nCount;
}


LPTSTR Cal3DComString::GetBufferSetLength(int count)
{
	AllocString( count);
	return GetString();
}

LPTSTR Cal3DComString::GetBuffer(int count) const
{
	return GetString();
}

void Cal3DComString::ReleaseBuffer(int count)
{

}