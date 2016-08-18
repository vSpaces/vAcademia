// CWComString.cpp : implementation file
#include "stdafx.h"
#include "WComString.h"
#include <atlbase.h>
#include <TCHAR.H>
#include <stdio.h>

#include "..\nmemoryprofiler\memoryprofilerFuncs.h"

//////////////////////////////////////////////////////////////////////
//
// Constructors/Destructor
//
//////////////////////////////////////////////////////////////////////

CWComString::CWComString()
{
	m_pszString = NULL;
	AllocString(0);
}

CWComString::CWComString(const CWComString& str)
{
	m_pszString = NULL;
	int nLen = str.GetLength();
	AllocString(nLen);
	wcsncpy_s(m_pszString, m_allocetedStringSize, (const wchar_t*) str, nLen);
}

CWComString::CWComString(const wchar_t* pszString)
{
	m_pszString = NULL;
	int nLen = wcslen(pszString);
	AllocString(nLen);
	wcsncpy_s(m_pszString, m_allocetedStringSize, pszString, nLen);
}

CWComString::CWComString(wchar_t ch, int nRepeat) 
{
	m_pszString = NULL;
	if (nRepeat > 0)
	{
		AllocString(nRepeat);
		for (int i=0; i < nRepeat; i++)
			m_pszString[i] = ch;
	}
}

CWComString::CWComString(const wchar_t* aCharData, int aDataLen)
{
	m_pszString = NULL;
	AllocString( aDataLen);
	wcsncpy_s(m_pszString, m_allocetedStringSize, aCharData, (aDataLen < 0 ? 0 : aDataLen));
}

CWComString::CWComString(HINSTANCE hInst, UINT nID)
{
	m_pszString = NULL;
	LoadString( hInst, nID);
}

CWComString::~CWComString()
{
	if(m_pszString != NULL)
		MP_DELETE_ARR(m_pszString);
}


//////////////////////////////////////////////////////////////////////
//
// Memory Manipulation
//
//////////////////////////////////////////////////////////////////////

void CWComString::AllocString(int nLen)
{
	ATLASSERT(nLen >= 0);
	if (m_pszString != NULL)
		MP_DELETE_ARR(m_pszString);
	m_pszString = MP_NEW_ARR(wchar_t, nLen+1);
	ATLASSERT(m_pszString != NULL);
	m_pszString[nLen] = '\0';
	m_allocetedStringSize = nLen+1;
}

void CWComString::ReAllocString(int nLen)
{
	ATLASSERT(nLen >= 0);
	m_pszString = MP_REALLOC_ARR(wchar_t, m_pszString, nLen+1);
	ATLASSERT(m_pszString != NULL);
	m_pszString[nLen] = '\0';
	m_allocetedStringSize = nLen+1;
}

void CWComString::StringCopy(int nSrcLen, const wchar_t* lpszSrcData)
{
	if (lpszSrcData == m_pszString)  return;
	AllocString(nSrcLen);
	memcpy(m_pszString, lpszSrcData, nSrcLen * sizeof(wchar_t));
	m_pszString[nSrcLen] = '\0';
}

void CWComString::StringCopy(CWComString &str, int nLen, int nIndex, int nExtra) const
{
	int nNewLen = nLen + nExtra;
	if (nNewLen != 0)
	{
		str.AllocString(nNewLen);
		memcpy((wchar_t*)str.GetString(), m_pszString+nIndex, nLen * sizeof(wchar_t));
	}
}

void CWComString::ConcatCopy(const wchar_t* lpszData)
{
	ATLASSERT(lpszData != NULL);

	// Save the existing string
	int nLen = GetLength();
	wchar_t* pszTemp = MP_NEW_ARR(wchar_t, nLen+1);
	memcpy(pszTemp, m_pszString, nLen * sizeof(wchar_t));
	pszTemp[nLen] = '\0';

	// Calculate the new string length and realloc memory
	int nDataLen = wcslen(lpszData);
	int nNewLen = nLen + nDataLen;
	ReAllocString(nNewLen);

	// Copy the strings into the new buffer
	memcpy(m_pszString, pszTemp, nLen * sizeof(wchar_t));
	memcpy(m_pszString+nLen, lpszData, nDataLen * sizeof(wchar_t));

	// Cleanup
	MP_DELETE_ARR(pszTemp);
}

void CWComString::ConcatCopy(wchar_t ch)
{
	// Save the existing string
	int nLen = GetLength();
	wchar_t* pszTemp = MP_NEW_ARR(wchar_t, nLen+1);
	memcpy(pszTemp, m_pszString, nLen * sizeof(wchar_t));
	pszTemp[nLen] = '\0';

	// Calculate the new string length and realloc memory
	int nNewLen = nLen + 1;
	ReAllocString(nNewLen);

	// Copy the strings into the new buffer
	memcpy(m_pszString, pszTemp, nLen * sizeof(wchar_t));
	m_pszString[nNewLen-1] = ch;

	// Cleanup
	MP_DELETE_ARR(pszTemp);
}

void CWComString::ConcatCopy(const wchar_t* lpszData1, const wchar_t* lpszData2)
{
	ATLASSERT(lpszData1 != NULL);
	ATLASSERT(lpszData2 != NULL);
	int nLen1 = wcslen(lpszData1);
	int nLen2 = wcslen(lpszData2);
	int nLen = nLen1 + nLen2;
	AllocString(nLen);
	memcpy(m_pszString, lpszData1, nLen1 * sizeof(wchar_t));
	memcpy(m_pszString+nLen1, lpszData2, nLen2 * sizeof(wchar_t)); 
}

BSTR CWComString::AllocSysString() const
{
	BSTR bstr = ::SysAllocStringLen(m_pszString, GetLength());
	if (bstr == NULL)
	{
		ATLASSERT(0);
		return NULL;
	}
	return bstr;
}

//////////////////////////////////////////////////////////////////////
//
// Insertion
//
//////////////////////////////////////////////////////////////////////

void CWComString::Insert(int nIndex, wchar_t ch)
{
	// Save the existing string
	int nLen = wcslen(m_pszString);
	wchar_t* pszTemp = MP_NEW_ARR(wchar_t, nLen+1);
	memcpy(pszTemp, m_pszString, nLen * sizeof(wchar_t));
	pszTemp[nLen] = '\0';

	// Calculate the new string length and realloc memory
	int nNewLen = nLen + 1;
	ReAllocString(nNewLen);

	if(nIndex < 0) nIndex = 0;
	if(nIndex > nLen) nIndex = nLen;
	if(nIndex > 0)
		memcpy(m_pszString, pszTemp, nIndex * sizeof(wchar_t));
	m_pszString[nIndex] = ch;
	if(nIndex < nLen)
		memcpy(m_pszString + nIndex + 1, pszTemp + nIndex, (nLen - nIndex) * sizeof(wchar_t));
	// Cleanup
	MP_DELETE_ARR(pszTemp);
}

void CWComString::Insert(int nIndex, const wchar_t* lpszStr)
{
	if(lpszStr == NULL || *lpszStr == '\0') return;
	// Save the existing string
	int nLen = wcslen(m_pszString);
	wchar_t* pszTemp = MP_NEW_ARR(wchar_t, nLen+1);
	memcpy(pszTemp, m_pszString, nLen * sizeof(wchar_t));
	pszTemp[nLen] = '\0';

	// Calculate the new string length and realloc memory
	int nArgLen = wcslen(lpszStr);
	int nNewLen = nLen + nArgLen;
	ReAllocString(nNewLen);

	if(nIndex < 0) nIndex = 0;
	if(nIndex > nLen) nIndex = nLen;
	if(nIndex > 0)
		memcpy(m_pszString, pszTemp, nIndex * sizeof(wchar_t));
	memcpy(m_pszString + nIndex, lpszStr, nArgLen * sizeof(wchar_t));
	if(nIndex < nLen)
		memcpy(m_pszString + nIndex + nArgLen, pszTemp + nIndex, (nLen - nIndex) * sizeof(wchar_t));
	MP_DELETE_ARR(pszTemp);
}

void CWComString::Insert(int nIndex, const CWComString& strSrc)
{
	Insert(nIndex, strSrc.GetString());
}

//////////////////////////////////////////////////////////////////////
//
// Accessors for the String as an Array
//
//////////////////////////////////////////////////////////////////////

void CWComString::Empty()
{
	if (wcslen(m_pszString) > 0)
		m_pszString[0] = '\0';
}

wchar_t CWComString::GetAt(int nIndex)
{
	int nLen = wcslen(m_pszString);
	ATLASSERT(nIndex >= 0);
	ATLASSERT(nIndex < nLen);
	return m_pszString[nIndex];
}

wchar_t CWComString::operator[] (int nIndex)
{
	int nLen = wcslen(m_pszString);
	ATLASSERT(nIndex >= 0);
	ATLASSERT(nIndex < nLen);
	return m_pszString[nIndex];
}

void CWComString::SetAt(int nIndex, wchar_t ch)
{
	int nLen = wcslen(m_pszString);
	ATLASSERT(nIndex >= 0);
	ATLASSERT(nIndex < nLen);
	m_pszString[nIndex] = ch;	
}

int CWComString::GetLength() const
{ 
	return wcslen(m_pszString); 
}

bool CWComString::IsEmpty() const
{ 
	return (m_pszString == NULL || m_pszString[ 0] == 0); 
}


//////////////////////////////////////////////////////////////////////
//
// Conversions
//
//////////////////////////////////////////////////////////////////////

void CWComString::MakeUpper()
{
	_wcsupr_s(m_pszString, m_allocetedStringSize);
}

void CWComString::MakeLower()
{
	_wcslwr_s(m_pszString, m_allocetedStringSize);
}

void CWComString::MakeReverse()
{
	_wcsrev(m_pszString);
}

void CWComString::TrimLeft()
{
	wchar_t* lpsz = m_pszString;

	while (_istspace(*lpsz))
		lpsz++;

	if (lpsz != m_pszString)
	{
		memmove(m_pszString, lpsz, (GetLength()+1) * sizeof(wchar_t));
	}
}

void CWComString::TrimRight()
{
	wchar_t* lpsz = m_pszString;
	wchar_t* lpszLast = NULL;

	while (*lpsz != '\0')
	{
		if (_istspace(*lpsz))
		{
			if (lpszLast == NULL)
				lpszLast = lpsz;
		}
		else
			lpszLast = NULL;
		lpsz++;
	}

	if (lpszLast != NULL)
		*lpszLast = '\0';
}


//////////////////////////////////////////////////////////////////////
//
// Searching
//
//////////////////////////////////////////////////////////////////////

int CWComString::Find(wchar_t ch) const
{
	return Find(ch, 0);
}

int CWComString::Find(wchar_t ch, int nStart) const
{
	if (nStart >= GetLength())
		return -1;
	wchar_t* lpsz = wcschr(m_pszString + nStart, (_TUCHAR) ch);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pszString);
}

int CWComString::Find(const wchar_t* lpszSub)
{
	return Find(lpszSub, 0);
}

int CWComString::Find(const wchar_t* lpszSub, int nStart)
{	
	if (wcslen(lpszSub) == 0)
	{
		return -1;
	}

	if (nStart > GetLength())
		return -1;

	wchar_t* lpsz = wcsstr(m_pszString + nStart, lpszSub);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pszString);
}

int CWComString::FindOneOf(const wchar_t* lpszCharSet) const
{
	wchar_t* lpsz = wcspbrk(m_pszString, lpszCharSet);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pszString);
}

int CWComString::ReverseFind(wchar_t ch) const
{
	int i;
	for (i = GetLength() - 1;  i >= 0 && m_pszString[ i] != ch;  i--);
	return i;
}


//////////////////////////////////////////////////////////////////////
//
// Assignment Operations
//
//////////////////////////////////////////////////////////////////////

const CWComString& CWComString::operator=(const CWComString& strSrc)
{
	if (m_pszString != strSrc.GetString())
		StringCopy(strSrc.GetLength(), strSrc.GetString());
	return *this;
}

const CWComString& CWComString::operator=(const wchar_t* lpsz)
{
	ATLASSERT(lpsz != NULL);
	StringCopy(wcslen(lpsz), lpsz);
	return *this;
}

//////////////////////////////////////////////////////////////////////
//
// Concatenation
//
//////////////////////////////////////////////////////////////////////

VA_RTL_API CWComString operator+(CWComString& strSrc1, CWComString& strSrc2)
{
	CWComString s;
	s.ConcatCopy((const wchar_t*) strSrc1, (const wchar_t*) strSrc2);
	return s;
}

VA_RTL_API CWComString operator+(const CWComString& strSrc1, const CWComString& strSrc2)
{
	CWComString s;
	s.ConcatCopy((const wchar_t*) strSrc1, (const wchar_t*) strSrc2);
	return s;
}

const CWComString& CWComString::operator+=(CWComString& strSrc)
{
	ConcatCopy((const wchar_t*) strSrc);
	return *this;
}

const CWComString& CWComString::operator+=(const wchar_t* lpsz)
{
	ConcatCopy(lpsz);
	return *this;
}

const CWComString& CWComString::operator+=(wchar_t ch)
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

CWComString CWComString::Mid(int nFirst) const
{
	return Mid(nFirst, GetLength() - nFirst);	
}

CWComString CWComString::Mid(int nFirst, int nCount) const
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

	CWComString newStr;
	StringCopy(newStr, nCount, nFirst, 0);
	return newStr;
}

CWComString CWComString::Left(int nCount) const
{
	if (nCount < 0)
		nCount = 0;

	CWComString newStr;
	StringCopy(newStr, nCount, 0, 0);
	return newStr;
}

CWComString CWComString::Right(int nCount) const
{
	if (nCount < 0)
		nCount = 0;

	CWComString newStr;
	StringCopy(newStr, nCount, GetLength() - nCount, 0);
	return newStr;
}

CWComString CWComString::SpanIncluding(const wchar_t* lpszCharSet) const
{
	ATLASSERT(lpszCharSet != NULL);
	ATLASSERT(::IsBadStringPtrW(lpszCharSet, (UINT_PTR)-1) == 0);
	return Left(wcsspn(m_pszString, lpszCharSet));
}

CWComString CWComString::SpanExcluding(const wchar_t* lpszCharSet) const
{
	ATLASSERT(lpszCharSet != NULL);
	ATLASSERT(::IsBadStringPtrW(lpszCharSet, -1) == 0);
	return Left(wcscspn(m_pszString, lpszCharSet));
}


//////////////////////////////////////////////////////////////////////
//
// Comparison
//
//////////////////////////////////////////////////////////////////////
	
int CWComString::Compare(CWComString& str) const
{
	ATLASSERT((const wchar_t*) str != NULL);
	ATLASSERT(::IsBadStringPtrW((const wchar_t*) str, -1) == 0);
	return wcscmp(m_pszString, (const wchar_t*) str);	
}

int CWComString::Compare(const wchar_t* lpsz) const
{
	ATLASSERT(lpsz != NULL);
	ATLASSERT(::IsBadStringPtrW(lpsz, -1) == 0);
	return wcscmp(m_pszString, lpsz);	
}

int CWComString::CompareNoCase(CWComString& str) const
{
	ATLASSERT((const wchar_t*) str != NULL);
	ATLASSERT(::IsBadStringPtrW((const wchar_t*) str, -1) == 0);
	return _wcsicmp(m_pszString, (const wchar_t*) str);	
}

int CWComString::CompareNoCase(const wchar_t* lpsz) const
{
	ATLASSERT(lpsz != NULL);
	ATLASSERT(::IsBadStringPtrW(lpsz, -1) == 0);
	return _wcsicmp(m_pszString, lpsz);	
}

int CWComString::Collate(const wchar_t* lpsz) const
{
	ATLASSERT(lpsz != NULL);
	ATLASSERT(::IsBadStringPtrW(lpsz, -1) == 0);
	return wcscoll(m_pszString, lpsz);	
}

int CWComString::Collate(CWComString &str) const
{
	ATLASSERT((const wchar_t*) str != NULL);
	ATLASSERT(::IsBadStringPtrW((const wchar_t*) str, -1) == 0);
	return wcscoll(m_pszString, (const wchar_t*) str);	
}


//////////////////////////////////////////////////////////////////////
//
// Formatting
//
//////////////////////////////////////////////////////////////////////
	
void CWComString::Format(const wchar_t* pszCharSet, ...)
{
	va_list vl;
	va_start(vl, pszCharSet);
	FormatV( pszCharSet, vl);
	va_end(vl);
}
	
void CWComString::FormatV(const wchar_t* pszCharSet, va_list vl)
{
	wchar_t* pszTemp = NULL;
	int nBufferSize = 0;
	int nRetVal = -1;

	do {
		nBufferSize += 100;
		if( pszTemp != NULL)
			MP_DELETE_ARR(pszTemp);
		pszTemp = MP_NEW_ARR(wchar_t, nBufferSize);	
		ZeroMemory( pszTemp, nBufferSize*sizeof(wchar_t));
		nRetVal = _vsnwprintf_s(pszTemp, nBufferSize, nBufferSize-1, pszCharSet, vl);
	} while (nRetVal < 0 && nBufferSize < (INT_MAX-101));

	int nNewLen = wcslen(pszTemp);
	AllocString(nNewLen);
	wcscpy_s(m_pszString, m_allocetedStringSize, pszTemp);
	MP_DELETE_ARR(pszTemp);
}


//////////////////////////////////////////////////////////////////////
//
// Replacing
//
//////////////////////////////////////////////////////////////////////

int CWComString::Replace(wchar_t chOld, wchar_t chNew)
{
	int nCount = 0;

	if (chOld != chNew)
	{
		wchar_t* psz = m_pszString;
		wchar_t* pszEnd = psz + GetLength();
		while(psz < pszEnd)
		{
			if (*psz == chOld)
			{
				*psz = chNew;
				nCount++;
			}
			psz++;
		}
	}

	return nCount;
}

int CWComString::Replace(const wchar_t* lpszOld, const wchar_t* lpszNew)
{
	int nSourceLen = wcslen(lpszOld);
	ATLASSERT( nSourceLen >= 0);
	if (nSourceLen == 0)
	{
		return 0;
	}
	int nReplaceLen = wcslen(lpszNew);
	ATLASSERT( nReplaceLen >= 0);

	int nCount = 0;
	wchar_t* lpszStart = m_pszString;
	wchar_t* lpszEnd = lpszStart + GetLength();
	wchar_t* lpszTarget;

	// Check to see if any changes need to be made
	while (lpszStart < lpszEnd)
	{
		while ((lpszTarget = wcsstr(lpszStart, lpszOld)) != NULL)
		{
			lpszStart = lpszTarget + nSourceLen;
			nCount++;
		}
		lpszStart += wcslen(lpszStart) + 1;
	}

	// Do the actual work
	if (nCount > 0)
	{
		int nOldLen = GetLength();
		int nNewLen = nOldLen + (nReplaceLen - nSourceLen) * nCount;
		if (GetLength() < nNewLen)
		{
			CWComString szTemp = m_pszString;
			ReAllocString(nNewLen);
			memcpy(m_pszString, (const wchar_t*) szTemp, nOldLen * sizeof(wchar_t));
		}

		lpszStart = m_pszString;
		lpszEnd = lpszStart + GetLength();

		while (lpszStart < lpszEnd)
		{
			while ((lpszTarget = wcsstr(lpszStart, lpszOld)) != NULL)
			{
				int nBalance = nOldLen - (lpszTarget - m_pszString + nSourceLen);
				memmove(lpszTarget + nReplaceLen, lpszTarget + nSourceLen, nBalance * sizeof(wchar_t));
				memcpy(lpszTarget, lpszNew, nReplaceLen * sizeof(wchar_t));
				lpszStart = lpszTarget + nReplaceLen;
				lpszStart[nBalance] = '\0';
				nOldLen += (nReplaceLen - nSourceLen);
			}
			lpszStart += wcslen(lpszStart) + 1;
		}
		ATLASSERT(m_pszString[GetLength()] == '\0');
	}
	return nCount;
}


void CWComString::CreateString(const wchar_t* aStrData, int aDataLen)
{
	ATLASSERT(aStrData != NULL);
	StringCopy(aDataLen, aStrData);
}

wchar_t* CWComString::GetBuffer(int count)
{
	if(count <= 0) return m_pszString;
	AllocString(count);
	return m_pszString;
}

int CWComString::Delete(int nIndex, int nCount){
	int iLen=GetLength();
	if(nIndex < 0 || nIndex >= iLen || nCount <= 0) return iLen;
	int iLenToCopy = iLen - nIndex - nCount;
	if(iLenToCopy > 0){
		wchar_t* psz=m_pszString + (nIndex + nCount);
		memcpy(m_pszString + nIndex, psz, iLenToCopy * sizeof(wchar_t));
	}else if(iLenToCopy < 0) return iLen;
	int iNewLen=nIndex+iLenToCopy;
	m_pszString[iNewLen]=0;
	return iNewLen;
}

wchar_t* CWComString::GetBufferSetLength(int count)
{
	AllocString( count);
	return (wchar_t*)GetString();
}

bool CWComString::LoadString(HINSTANCE hInst, UINT nID)
{
	USES_CONVERSION;
	UINT nLen = 0;
	int nSize = 256;
	do {
		nSize += 256;
		AllocString( nSize - 1);
		nLen = ::LoadStringW(hInst, nID, m_pszString, nSize);
	} while (nSize - nLen < sizeof( wchar_t));
	m_pszString[ nLen] = 0;
	return (nLen != 0);
}
