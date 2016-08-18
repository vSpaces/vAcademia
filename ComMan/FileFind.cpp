/*
Module : FileFind.h
Purpose: Implementation of FileFind class for WTL which performs local file searches

Copyright (c) 2003 by iLYA Solnyshkin. 
E-mail: isolnyshkin@yahoo.com 
All rights reserved.
*/


#include "stdafx.h"
#include "FileFind.h"

////////////////////////////////////////////////////////////////////////////
// CFileFind implementation

CFileFind::CFileFind():
	MP_WSTRING_INIT(m_strRoot)
{
	m_pFoundInfo = NULL;
	m_pNextInfo = NULL;
	m_hContext = NULL;
	m_chDirSeparator = '\\';
	m_bGotLast = FALSE;
}

CFileFind::~CFileFind()
{
	Close();
}

void CFileFind::Close()
{
	if (m_pFoundInfo != NULL)
	{
		WIN32_FIND_DATAW* fi = (WIN32_FIND_DATAW*)m_pFoundInfo;
		MP_DELETE(fi);
		m_pFoundInfo = NULL;
	}

	if (m_pNextInfo != NULL)
	{
		WIN32_FIND_DATAW* ni = (WIN32_FIND_DATAW*)m_pNextInfo;
		MP_DELETE(ni);
		m_pNextInfo = NULL;
	}

	if (m_hContext != NULL && m_hContext != INVALID_HANDLE_VALUE)
	{
		CloseContext();
		m_hContext = NULL;
	}
}

void CFileFind::CloseContext()
{
	::FindClose(m_hContext);
	return;
}

BOOL CFileFind::FindFile(const wchar_t* pstrName /* = NULL */)
{
	Close();
	m_pNextInfo = MP_NEW(WIN32_FIND_DATAW);
	m_bGotLast = FALSE;

	if (pstrName == NULL)
		pstrName = L"*.*";

	lstrcpyW(((WIN32_FIND_DATAW*) m_pNextInfo)->cFileName, pstrName);

	m_hContext = ::FindFirstFileW(pstrName, (WIN32_FIND_DATAW*) m_pNextInfo);

	if (m_hContext == INVALID_HANDLE_VALUE)
	{
		DWORD dwTemp = ::GetLastError();
		Close();
		::SetLastError(dwTemp);
		return FALSE;
	}

	wchar_t pstrRoot[ MAX_PATH2 ];
	const wchar_t* pstr = _wfullpath((wchar_t*)pstrRoot, pstrName, MAX_PATH2);

	// passed name isn't a valid path but was found by the API
	ATLASSERT(pstr != NULL);
	if (pstr == NULL)
	{
		m_strRoot = L"";
		Close();
		::SetLastError(ERROR_INVALID_NAME);
		return FALSE;
	}
	else
	{
		// find the last forward or backward whack
		wchar_t* pstrBack  = wcsrchr(pstrRoot, L'\\');
		wchar_t* pstrFront = wcsrchr(pstrRoot, L'/');

		if (pstrFront != NULL || pstrBack != NULL)
		{
			if (pstrFront == NULL)
				pstrFront = (wchar_t*)pstrRoot;
			if (pstrBack == NULL)
				pstrBack = (wchar_t*)pstrRoot;

			// from the start to the last whack is the root

			if (pstrFront >= pstrBack)
				*pstrFront = L'\0';
			else
				*pstrBack = L'\0';
		}
		m_strRoot = L"";
	}

	return TRUE;
}

BOOL CFileFind::MatchesMask(DWORD dwMask) const
{
	if( m_hContext == NULL)
		return FALSE;

	if (m_pFoundInfo != NULL)
		return (!!(((LPWIN32_FIND_DATAW) m_pFoundInfo)->dwFileAttributes & dwMask));
	else
		return FALSE;
}

BOOL CFileFind::GetLastAccessTime(FILETIME* pTimeStamp) const
{
	if( m_hContext == NULL)
		return FALSE;
	if( pTimeStamp == NULL)
		return FALSE;

	if (m_pFoundInfo != NULL && pTimeStamp != NULL)
	{
		*pTimeStamp = ((LPWIN32_FIND_DATAW) m_pFoundInfo)->ftLastAccessTime;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CFileFind::GetLastWriteTime(FILETIME* pTimeStamp) const
{
	if( m_hContext == NULL)
		return FALSE;
	if( pTimeStamp == NULL)
		return FALSE;

	if (m_pFoundInfo != NULL && pTimeStamp != NULL)
	{
		*pTimeStamp = ((LPWIN32_FIND_DATAW) m_pFoundInfo)->ftLastWriteTime;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CFileFind::GetCreationTime(FILETIME* pTimeStamp) const
{
	if( m_hContext == NULL)
		return FALSE;

	if (m_pFoundInfo != NULL && pTimeStamp != NULL)
	{
		*pTimeStamp = ((LPWIN32_FIND_DATAW) m_pFoundInfo)->ftCreationTime;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CFileFind::IsDots() const
{
	if( m_hContext == NULL)
		return FALSE;

	// return TRUE if the file name is "." or ".." and
	// the file is a directory

	BOOL bResult = FALSE;
	if (m_pFoundInfo != NULL && IsDirectory())
	{
		LPWIN32_FIND_DATAW pFindData = (LPWIN32_FIND_DATAW) m_pFoundInfo;
		if (pFindData->cFileName[0] == '.')
		{
			if (pFindData->cFileName[1] == L'\0' ||
				(pFindData->cFileName[1] == L'.' &&
				 pFindData->cFileName[2] == L'\0'))
			{
				bResult = TRUE;
			}
		}
	}

	return bResult;
}

BOOL CFileFind::FindNextFile()
{
	if( m_hContext == NULL)
		return FALSE;

	if (m_pFoundInfo == NULL)
		m_pFoundInfo = MP_NEW(WIN32_FIND_DATAW);

	void* pTemp = m_pFoundInfo;
	m_pFoundInfo = m_pNextInfo;
	m_pNextInfo = pTemp;

	return ::FindNextFileW(m_hContext, (LPWIN32_FIND_DATAW) m_pNextInfo);
}

std::wstring CFileFind::GetFileURL() const
{
	if( m_hContext == NULL)
		return FALSE;

	std::wstring strResult = L"file://";
	strResult += GetFilePath();
	return strResult;
}

std::wstring CFileFind::GetRoot() const
{
	if( m_hContext == NULL)
		return FALSE;

	return m_strRoot;
}

std::wstring CFileFind::GetFilePath() const
{
	if( m_hContext == NULL)
		return FALSE;

	std::wstring strResult = m_strRoot;
	if (strResult[strResult.size()-1] != '\\' &&
		strResult[strResult.size()-1] != '/')
		strResult += m_chDirSeparator;
	strResult += GetFileName();
	return strResult;
}

std::wstring CFileFind::GetFileTitle() const
{
	if( m_hContext == NULL)
		return FALSE;

	std::wstring strFullName = GetFileName();
	wchar_t strResult[MAX_PATH2];

	_wsplitpath_s( strFullName.c_str(), NULL, 0, NULL, 0, strResult, MAX_PATH2, NULL, 0);
	return strResult;
}

std::wstring CFileFind::GetFileName() const
{
	if( m_hContext == NULL)
		return FALSE;

	std::wstring ret;

	if (m_pFoundInfo != NULL)
	{
		LPWIN32_FIND_DATAW pFindData = (LPWIN32_FIND_DATAW) m_pFoundInfo;
		ret = pFindData->cFileName;
	}
	return ret;
}

DWORD CFileFind::GetLength() const
{
	if( m_hContext == NULL)
		return FALSE;

	if (m_pFoundInfo != NULL)
		return ((LPWIN32_FIND_DATAW) m_pFoundInfo)->nFileSizeLow;
	else
		return 0;
}
