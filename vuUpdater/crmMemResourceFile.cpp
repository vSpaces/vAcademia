// crmMemResourceFile.cpp: implementation of the crmMemResourceFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "crmMemResourceFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

crmMemResourceFile::crmMemResourceFile()
{
	lpData = NULL;
	dwSize = NULL;
	dwLocation = 0;
}

void crmMemResourceFile::Attach(void* data, DWORD size)
{
	dwLocation = 0;
	dwSize = size;
	lpData = data;
}

crmMemResourceFile::~crmMemResourceFile()
{
	Detach();
	lpData = NULL;
}

//////////////////////////////////////////////////////////////////////////
UINT	crmMemResourceFile::Read( TCHAR* pszBuffer, UINT uCount, UINT* puCount )
{
	if( !lpData)	return 0;
	UINT readed = uCount;

	if( dwLocation + uCount >= dwSize)
		readed = dwSize - dwLocation;

	memcpy( pszBuffer, (void*)((DWORD) lpData + dwLocation), readed);

	dwLocation += readed;
	if( puCount)	*puCount = readed;

	//if( readed == 0)	dwLocation += 1;

	return readed;
}

DWORD		crmMemResourceFile::Read( BYTE* pBuffer, DWORD size )
{
	if( !lpData)
	{
		//ThrowErr( FILEEX_ERR_OTHERIO_CURRENTLY_RUNNG, TRUE );
		return 0;
	}
	UINT readed = size;

	if( dwLocation + size >= dwSize)
		readed = dwSize - dwLocation;

	memcpy( pBuffer, (void*)((DWORD) lpData + dwLocation), readed);

	dwLocation += readed;

	//if( readed == 0)		dwLocation += 1;

	return readed;
}

void	crmMemResourceFile::Write( const TCHAR* pszBuffer, UINT uCount, UINT* puCount )
{
	ATLASSERT(false);
}

DWORD	crmMemResourceFile::GetFileSize()
{
	return dwSize;
}

LPVOID	crmMemResourceFile::Detach()
{
	LPVOID	ret = lpData;
	dwSize = 0;
	lpData = NULL;
	return ret;
}

DWORD	crmMemResourceFile::Seek( DWORD lToMove, DWORD dwMoveFrom)
{
	if( !lpData || lToMove+dwMoveFrom > dwSize)
	{
		//ThrowErr( FILEEX_ERR_OTHERIO_CURRENTLY_RUNNG, TRUE );
		return 0;
	}
	dwLocation = lToMove + dwMoveFrom;
	return dwLocation;
}