// FileResource.cpp: implementation of the CBigFileResource class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "comman.h"
#include "ResMan.h"
#include "BigFileResource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBigFileResource::CBigFileResource() : CResourceImpl()
{
	pSrcResource = NULL;
	pBigFileManagerReader = NULL;
	realOffset = 0;
	offset = 0;
	time = 0;
}

CBigFileResource::~CBigFileResource()
{
	close();
}

long CBigFileResource::close()
{
	long hr = CResourceImpl::close();
	if (hr == E_NOTIMPL)
	{
		offset = 0;
		hr = S_OK;
	}
	return hr;
}

long CBigFileResource::read(BYTE * aBuffer, DWORD aBufferSize, DWORD* aReadSize, DWORD *aErrorRead)
{
	*aReadSize = 0;	
	long hr = CResourceImpl::read( aBuffer, aBufferSize, aReadSize);
	if (hr == E_NOTIMPL)
	{
		hr = S_OK;
		if( dwFileSize == 0)
			hr = E_FAIL;
		else
		{
			if( aBufferSize + offset > dwFileSize)
			{
				aBufferSize = dwFileSize - offset;
				hr = S_OK;
			}
			int errorCode = 0;
			unsigned int readed = pBigFileManagerReader->ReadFile( aBuffer, aBufferSize, realOffset + offset, errorCode);
			if( readed != aBufferSize)
			{
#if LOG_LOCAL_RESOURCE_ERROR				
				ILogWriter * lw = pResMan->GetLogWriter();
				if (lw != NULL)
				{
					lw->WriteLn(CLogValue("[WARNING] CBigFileResource::not reading local res = ", sResPath.GetBuffer(), " error code==", errorCode, "  readed", readed, " bytes from ", aBufferSize, " bytes"));
				}
#endif
				dwFileSize = 0;
				realOffset = 0;
				if(aErrorRead != NULL)
					*aErrorRead = qeReadError;
			}
			else
			{
#if LOG_LOCAL_RESOURCE	
				ILogWriter * lw = pResMan->GetLogWriter();
				if (lw != NULL)
				{
					lw->WriteLn(CLogValue("CBigFileResource::read local res = ", sResPath.GetBuffer(), "  dwFileSize==", (int)dwFileSize, "  aReadSize==", (int)*aReadSize));
				}
#endif
				if(aErrorRead != NULL)
					*aErrorRead = 0;
				*aReadSize = aBufferSize;
				offset += aBufferSize;
			}
		}
	}
	return hr;
}

long CBigFileResource::write(const BYTE * aBuffer, DWORD aBufferSize, DWORD* aWroteSize, DWORD aTotalSize)
{
	long hr = CResourceImpl::write( aBuffer, aBufferSize, aWroteSize, aTotalSize);
	if (hr == E_NOTIMPL)
	{
		hr = S_OK;
	}
	return hr;
}

long CBigFileResource::seek(DWORD aPos, BYTE aFrom)
{
	long hr = CResourceImpl::seek( aPos, aFrom);
	if (hr == E_NOTIMPL)
	{
		hr = S_OK;
		if( dwFileSize == 0)
			hr = E_FAIL;
		else
			offset = aPos;
	}
	return hr;
}

long CBigFileResource::getPos(DWORD* aPos)
{
	long hr = CResourceImpl::getPos( aPos);
	if (hr == E_NOTIMPL)
	{
		hr = S_OK;
		if( dwFileSize == 0)
			hr = E_FAIL;
		else
			*aPos = offset;
	}
	return hr;
}

long CBigFileResource::getSize(DWORD* aSize)
{
	long hr = CResourceImpl::getSize( aSize);
	if (hr == E_NOTIMPL)
	{
		hr = S_OK;
		if( dwFileSize == 0)
			hr = E_FAIL;
		*aSize = dwFileSize;
	}
	return hr;
}

/*long CBigFileResource::getLastModifyDateTime(DWORD* aDateTime)
{
	long hr = CResourceImpl::getLastModifyDateTime( aDateTime);
	if (hr == E_NOTIMPL)
	{
		hr = S_OK;
		if (dwFileSize == 0)
			hr = E_FAIL;
		else
		{
			*aDateTime = time;

		}
	}
	return hr;
}*/

long CBigFileResource::setSize(DWORD aSize)
{
	long hr = CResourceImpl::setSize( aSize);
	if (hr == E_NOTIMPL)
	{
		hr = S_OK;
	}	
	return hr;
}

long CBigFileResource::getStatus(DWORD* aStatus)
{
	return CResourceImpl::getStatus( aStatus);
}

void CBigFileResource::open( const wchar_t* aResPath, DWORD aFlags, IResourceNotify *aNotify, const wchar_t* aResRefinePath)
{
	CResourceImpl::open( aResPath, (aFlags & ~RES_ANYWHERE) | RES_LOCAL, aNotify, aResRefinePath);
	
	correctRemotePath( sResPath);
	pResMan->correctPointPath( sResPath);
	if(sResPath[0] == '/')
		sResPath = sResPath.Right( sResPath.GetLength() - 1);
	if(!pBigFileManagerReader->OpenFile( CComString(sResPath.GetBuffer()).GetBuffer(), (unsigned int &)dwFileSize, realOffset, time))
	{
#if LOG_LOCAL_RESOURCE_ERROR	
		ILogWriter * lw = pResMan->GetLogWriter();
		if (lw != NULL)
		{
			lw->WriteLn(CLogValue("[WARNING] CBigFileResource::not open local res = ", sResPath.GetBuffer(), " error code==", GetLastError(), " flags==", aFlags));
		}
#endif
		//dwStatus = (aFlags & ~RES_EXISTS) & ~RES_SERVER_ERROR_TIMEOUT;
		dwStatus = RES_PATH_ERROR;
		dwFileSize = 0;
		realOffset = 0;
	}
	else
	{
		initAfterOpen( aFlags);
#if LOG_LOCAL_RESOURCE	
		ILogWriter * lw = pResMan->GetLogWriter();
		if (lw != NULL)
		{
			lw->WriteLn(CLogValue("CBigFileResource::open local res = ", sResPath.GetBuffer()));
		}
#endif
	}
}

void CBigFileResource::unpackRes( DWORD aCachingBlock, bool* aEnabled)
{
	//aCachingBlock = dwUnpackTotalSize = dwUnpackSize = dwUnpackTotalNumber = dwUnpackNumber = 0;
	aCachingBlock = 0;
	*aEnabled = FALSE;
	unpackFile( sFullResPath);
}

void CBigFileResource::getTime( FILETIME& aTime) {
	if(dwFileSize!=0)
	{
		aTime.dwHighDateTime = time >> 32;
		aTime.dwLowDateTime = (DWORD)time;
	}
}

void CBigFileResource::setBigFileManagerReader(bigfile::IBigFileManagerReader *apBigFileManagerReader)
{
	pBigFileManagerReader = apBigFileManagerReader;
}