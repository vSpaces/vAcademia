// FileResource.cpp: implementation of the CFileResource class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "comman.h"
#include "ResMan.h"
#include "FileResource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileResource::CFileResource() : CResourceImpl()
{
	hFile =  INVALID_HANDLE_VALUE;
	pSrcResource = NULL;
}

CFileResource::~CFileResource()
{
	close();
}

long CFileResource::close()
{
	long hr = CResourceImpl::close();
	if (hr == E_NOTIMPL)
	{
		if (hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle( hFile);
		}
		hFile = INVALID_HANDLE_VALUE;
		hr = S_OK;
	}
	return hr;
}

long CFileResource::read(BYTE * aBuffer, DWORD aBufferSize, DWORD* aReadSize, DWORD *aErrorRead)
{	
	long hr = CResourceImpl::read( aBuffer, aBufferSize, aReadSize, aErrorRead);
	if (hr == E_NOTIMPL)
	{
		hr = S_OK;
		if (hFile == INVALID_HANDLE_VALUE)
			hr = E_FAIL;
		else if (!ReadFile( hFile, aBuffer, aBufferSize, aReadSize, NULL))
		{
			int nErr = GetLastError();
			if( aErrorRead != NULL)
				*aErrorRead = nErr;
			hr = E_FAIL;
		}
	}
	return hr;
}

long CFileResource::write(const BYTE * aBuffer, DWORD aBufferSize, DWORD* aWroteSize, DWORD aTotalSize)
{
	long hr = CResourceImpl::write( aBuffer, aBufferSize, aWroteSize, aTotalSize);
	if (hr == E_NOTIMPL)
	{
		hr = S_OK;
		if (hFile == INVALID_HANDLE_VALUE)
			hr = E_FAIL;
		else if (!WriteFile( hFile, aBuffer, aBufferSize, aWroteSize, NULL))
		{
			hr = E_FAIL;
		}
	}
	return hr;
}

long CFileResource::seek(DWORD aPos, BYTE aFrom)
{
	long hr = CResourceImpl::seek( aPos, aFrom);
	if (hr == E_NOTIMPL)
	{
		hr = S_OK;
		if (hFile == INVALID_HANDLE_VALUE)
			hr = E_FAIL;
		else
		{
			DWORD dwFlag = 
				(aFrom == RES_CURRENT ? FILE_CURRENT : 
					(aFrom == RES_BEGIN ? FILE_BEGIN :
						(aFrom == RES_END ? FILE_END : FILE_BEGIN)));
			SetFilePointer( hFile, aPos, 0, dwFlag);
		}
	}
	return hr;
}

long CFileResource::getPos(DWORD* aPos)
{
	long hr = CResourceImpl::getPos( aPos);
	if (hr == E_NOTIMPL)
	{
		hr = S_OK;
		if (hFile == INVALID_HANDLE_VALUE)
			hr = E_FAIL;
		else
			*aPos = SetFilePointer( hFile, 0, 0, FILE_CURRENT);
	}
	return hr;
}

long CFileResource::getSize(DWORD* aSize)
{
	long hr = CResourceImpl::getSize( aSize);
	if (hr == E_NOTIMPL)
	{
		hr = S_OK;
		if (hFile == INVALID_HANDLE_VALUE)
			hr = E_FAIL;
		else
		{
			*aSize = GetFileSize( hFile, NULL);
			dwFileSize = *aSize;
		}
	}
	return hr;
}

/*long CFileResource::getLastModifyDateTime(DWORD* aDateTime)
{
	long hr = CResourceImpl::getLastModifyDateTime( aDateTime);
	if (hr == E_NOTIMPL)
	{
		hr = S_OK;
		if (hFile == INVALID_HANDLE_VALUE)
			hr = E_FAIL;
		else
		{
			BY_HANDLE_FILE_INFORMATION tmp;
			hr = GetFileInformationByHandle( hFile, &tmp);
			if (hr == S_OK)
			{
				*aDateTime = tmp.ftLastWriteTime.dwHighDateTime;
			}
			
		}
	}
	return hr;
}*/

long CFileResource::setSize(DWORD aSize)
{
	long hr = CResourceImpl::setSize( aSize);
	if (hr == E_NOTIMPL)
	{
		hr = S_OK;
		if (hFile == INVALID_HANDLE_VALUE)
			hr = E_FAIL;
		else
		{
			seek(aSize, RES_BEGIN);
			if (!::SetEndOfFile( hFile))
				hr = E_FAIL;
			else
				dwFileSize = aSize;
		}
	}	
	return hr;
}

void CFileResource::open( const wchar_t* aResPath, DWORD aFlags, IResourceNotify *aNotify, const wchar_t* aResRefinePath)
{
/*#ifdef DEBUG
	CWComString s( aResPath);
	if( s.Find("pace.xml")!=-1)
		int ii=0;
#endif*/
	CResourceImpl::open( aResPath, (aFlags & ~RES_ANYWHERE) | RES_LOCAL, aNotify, aResRefinePath);
	if (aFlags & RES_CREATE)
	{
		CWComString sPath( sFullResPath);
		sPath.SetAt( sPath.ReverseFind( '\\') + 1, 0);
		makeDir( sPath);
		hFile = CreateFileW( sFullResPath, GENERIC_READ | GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	else if (aFlags & RES_TO_WRITE)
		hFile = CreateFileW( sFullResPath, GENERIC_READ | GENERIC_WRITE, 0, NULL,
			OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	else
		hFile = CreateFileW( sFullResPath, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	
	if (hFile == INVALID_HANDLE_VALUE)
	{
#if LOG_LOCAL_RESOURCE_ERROR	
		ILogWriter * lw = pResMan->GetLogWriter();
		if (lw != NULL)
		{
			lw->WriteLn(CLogValue("[WARNING] CFileResource::not open local res = ", sFullResPath.GetBuffer(), " error code==", GetLastError(), " flags==", aFlags));
		}
#endif
		aFlags |= RES_PATH_ERROR;
	}
#if LOG_LOCAL_RESOURCE	
	else
	{
		ILogWriter * lw = pResMan->GetLogWriter();
		if (lw != NULL)
		{
			lw->WriteLn(CLogValue("CFileResource::open local res= ", sFullResPath.GetBuffer()));
		}
	}
#endif
	
	initAfterOpen( aFlags);
}

void CFileResource::unpackRes( DWORD aCachingBlock, bool* aEnabled)
{
	//aCachingBlock = dwUnpackTotalSize = dwUnpackSize = dwUnpackTotalNumber = dwUnpackNumber = 0;
	aCachingBlock = 0;
	*aEnabled = false;
	unpackFile( sFullResPath);
}

void CFileResource::getTime( FILETIME& aTime) {
	if (hFile != INVALID_HANDLE_VALUE)
		GetFileTime( hFile, NULL, NULL, &aTime);
}