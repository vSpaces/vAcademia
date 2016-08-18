// ResourceImpl.cpp: implementation of the CResourceImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "comman.h"
#include "ResMan.h"
#include "ResourceImpl.h"
#include "unzip32.h"
#include <math.h>
#include "FileFind.h"
#include "ThreadAffinity.h"
#include "CrashHandlerInThread.h"
#include "../VacademiaReportLib/XUnzip.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void makeDir(const wchar_t* aDirPath)
{
	wchar_t temp[MAX_PATH2];
	memset(&temp, 0, MAX_PATH2*sizeof(wchar_t));
	wcscat_s(temp, MAX_PATH2,aDirPath);

	wchar_t* start;
	wchar_t* point;
	for(start = point = temp; *point; point++)
	{
		if(*point == L'\\')
		{
			*point = L'\0'; 
			CreateDirectoryW(start, NULL);
			*point = L'\\';
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CResourceImpl::CResourceImpl() : CResManCreation(), dwStatus( RES_PATH_ERROR),
	pCacheData( NULL), dwCacheDataSize( 0), dwCachePos( 0), hCacheFile( INVALID_HANDLE_VALUE),
	bMemCacheCreating( false), bFileCacheCreating( false), hCacheThread( NULL),
	hUnpackThread( NULL), dwUnpackTotalNumber( 0), dwUnpackNumber( 0), dwUnpackTotalSize( 0), dwUnpackSize( 0), dwWriteCount( 0),
	bNotifyDownload(false), hDownloadThread( NULL), hUploadThread( NULL), hDownloadToCacheThread(NULL), bIsCashingNow(false), spResourceNotify(NULL)
{
	dwFileSize = 0;
	pSrcResource = NULL;
	bSpecExtension = false;
	sResRefinePath = L"";
	freeBytesAvailable = -1;
	dwStatusUploader = 0;
	dwCachedSize = 0;
	bEnableCacheThread = false;
	bEnableUnpackThread = false;
	bEnableUploadThread = false;

	m_needDestroyFromDestroy = false;
	m_canDestroyFromCache = true;
	m_canDestroyFromDownload = true;
	m_canDestroyFromUpload = true;
	m_canDestroyFromUnzip = true;
	m_canDestroyFromDownloadCache = true;
	m_deleteResultGiven = false;
}

CResourceImpl::~CResourceImpl()
{
	close();
}

void CResourceImpl::destroy()
{
	mutexClose.lock();

	if( pResMan != NULL)
		pResMan->notifyClose( this);
	if( spResourceNotify != NULL )
	{
		if( spResourceNotify->isNeedDestroyByComman())
			spResourceNotify->destroy();
		spResourceNotify = NULL;
	}
	pResMan = NULL;
	mutexClose.unlock();

	CheckAndDestroyIfNeeded(&m_needDestroyFromDestroy);
}

void CResourceImpl::CheckAndDestroyIfNeeded(volatile bool *aFlagToSet)
{
	m_mutexDestroy.lock();
	*aFlagToSet = true;
	bool canDelete = CanDelete();
	m_mutexDestroy.unlock();
	if (canDelete) {
		MP_DELETE_THIS;
	}
}

bool CResourceImpl::CanDelete()
{
	if (m_deleteResultGiven)
	{
		return false;
	}
	bool result  = false;
	if (m_needDestroyFromDestroy && m_canDestroyFromCache && m_canDestroyFromDownload 
		&& m_canDestroyFromUpload && m_canDestroyFromUnzip && m_canDestroyFromDownloadCache)
	{
		result = true;
		m_deleteResultGiven = true;
	}
	return result;
}

long CResourceImpl::close()
{
	if (hCacheThread != NULL)
	{
		bEnableCacheThread = false;
		notifyResMan(RN_DOWNLOAD_ERROR);
		
		hCacheThread = NULL;
	}

	if (hDownloadThread != NULL)
	{
		bEnableCacheThread = false;
		notifyResMan(RN_DOWNLOAD_ERROR);
		hDownloadThread = NULL;
	}

	if (hDownloadToCacheThread != NULL)
	{
		bEnableCacheThread = false;
		notifyResMan(RN_DOWNLOAD_ERROR);
		hDownloadToCacheThread = NULL;
	}

	if (hUploadThread != NULL)
	{
		bEnableUploadThread = false;
		hUploadThread = NULL;
		//notifyResMan(RN_UPLOAD_ERROR);
	}

	if (hUnpackThread != NULL)
	{
		bEnableUnpackThread = false;
		hUnpackThread = NULL;
	}

	if (pCacheData != NULL)
		MP_DELETE_ARR( pCacheData);
	pCacheData = NULL;
	dwCacheDataSize = 0;
	dwCachePos = 0;

	if (hCacheFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle( hCacheFile);
		hCacheFile = INVALID_HANDLE_VALUE;
	}

	mutexClose.lock();
	if(pSrcResource!=NULL)
	{
		pSrcResource->destroy();
		pSrcResource = NULL;
	}
	mutexClose.unlock();
		
	return E_NOTIMPL;
}

long CResourceImpl::read( BYTE* aBuffer, DWORD aBufferSize, DWORD* aReadSize, DWORD *aError)
{
	if (aBuffer == NULL)
		return E_POINTER;
	if (aReadSize == NULL)
		return E_POINTER;
	if( aBufferSize == 0)
	{
		*aReadSize = 0;
		return S_OK;
	}
	long hr = E_NOTIMPL;
	if (isMemCache())
	{
		if (dwCachePos < dwCacheDataSize)
		{
			*aReadSize = aBufferSize;
			if (dwCachePos + *aReadSize > dwCacheDataSize)
				*aReadSize = dwCacheDataSize - dwCachePos;
			rtl_memcpy( aBuffer, aBufferSize, pCacheData + dwCachePos, *aReadSize);
			dwCachePos += *aReadSize;
		}
		else
		{
			*aReadSize = 0;
			ILogWriter * lw = pResMan->GetLogWriter();
			if(lw != NULL && dwCacheDataSize == 0)
			{
				lw->WriteLn("[WARNING] dwCachePos >= dwCacheDataSize file sFullResPath=", sFullResPath.GetBuffer(), "  dwCacheDataSize=="+dwCacheDataSize);
			}
		}
		hr = S_OK;
	}
	else if (isFileCache())
	{
		if(!::ReadFile( hCacheFile, aBuffer, aBufferSize, aReadSize, NULL) || *aReadSize == 0)
		{
			int nErr = (int)GetLastError();
			if( aError != NULL)
				*aError = nErr;
			ILogWriter * lw = pResMan->GetLogWriter();
			if(lw != NULL)
			{
				lw->WriteLn("[WARNING] ReadFile cache file sFullResPath=", sFullResPath.GetBuffer(), "  code==", nErr, "  readSize==", *aReadSize);
			}	
		}
		hr = S_OK;
	}
		
	return hr;
}

long CResourceImpl::saveToCache(void* apCacheData, DWORD adwCacheDataSize)
{
	DWORD dwSize = 0;
	getSize( &dwSize);
	FILETIME oTime;
	getTime(oTime);

	ILogWriter * lw = pResMan->GetLogWriter();

	if (hCacheFile != INVALID_HANDLE_VALUE)
	{
		bFileCacheCreating = true;
	}
	else
	{
		ECacheFileErrors openError = cfeCreated;
		if(bSpecExtension)
			openError = pResMan->openCacheFile( sResRefinePath,
				dwStatus, dwSize, oTime, &hCacheFile, sCacheFilePath.GetBuffer( MAX_PATH2), MAX_PATH2);
		else
			openError = pResMan->openCacheFile( sResPath,
				dwStatus, dwSize, oTime, &hCacheFile, sCacheFilePath.GetBuffer( MAX_PATH2), MAX_PATH2);
		bFileCacheCreating = (openError == cfeCreated);
	}

	if ( bFileCacheCreating) 
	{
		dwStatus |= RES_FILE_CACHE;
		DWORD dwWritenBlock;
		if(!WriteFile(hCacheFile, apCacheData, adwCacheDataSize, &dwWritenBlock, NULL))
		{
			if(lw != NULL)
				lw->WriteLn("[COMMAN] Cache file write error for = ", sResRefinePath.GetBuffer(), "  code = ", GetLastError());
			return S_OK;
		}
		else
		{
			//if(lw != NULL)
			//	lw->WriteLn("[COMMAN] Cache file write success for = ", sResRefinePath.GetBuffer(), "  dwCacheDataSize = ", dwCacheDataSize);
		}
		if (adwCacheDataSize == dwWritenBlock)
		{
			if (!SetFileTime( hCacheFile, &oTime, NULL, &oTime))
			{
				int nErr = GetLastError();
				nErr += 0;
			}
		}
		else
		{
			if(lw != NULL)
				lw->WriteLn("[COMMAN] Cache file write error for = ", sResRefinePath.GetBuffer(), "  code = ", GetLastError(), "  dwCacheDataSize==", dwCacheDataSize, "  dwWritenBlock==", dwWritenBlock);
		}
		SetFilePointer(hCacheFile, 0, 0, FILE_BEGIN);
	}

	bFileCacheCreating = false;
	return S_OK;
}

long CResourceImpl::write(const BYTE * aBuffer, DWORD aBufferSize, DWORD* aWroteSize, DWORD aTotalSize)
{
	if (aBuffer == NULL)
		return E_POINTER;
	if (aWroteSize == NULL)
		return E_POINTER;
	if( aBufferSize == 0)
		return E_POINTER;
	if( aTotalSize == 0)
		return E_NOTIMPL;
	return E_NOTIMPL;
}

long CResourceImpl::seek(DWORD aPos, BYTE aFrom)
{
	long hr = E_NOTIMPL;
	if (isMemCache())
	{
		switch (aFrom) {
		case RES_BEGIN:
			dwCachePos = aPos;
			break;
		case RES_CURRENT:
			dwCachePos += aPos;
			break;
		case RES_END:
			if (aPos < dwCacheDataSize)
				dwCachePos = dwCacheDataSize - aPos;
			else
				dwCachePos = 0;
			break;
		default:
			dwCachePos = aPos;
		}
		hr = S_OK;
	}
	else if (isFileCache())
	{
		DWORD dwFlag = 
			(aFrom == RES_CURRENT ? FILE_CURRENT : 
				(aFrom == RES_BEGIN ? FILE_BEGIN :
					(aFrom == RES_END ? FILE_END : FILE_BEGIN)));
		SetFilePointer( hCacheFile, aPos, 0, dwFlag);
		hr = S_OK;
	}
	return hr;
}

long CResourceImpl::getPos(DWORD* aPos)
{
	if (aPos == NULL)
		return E_POINTER;

	long hr = E_NOTIMPL;
	if (isMemCache())
	{
		*aPos = dwCachePos;
		hr = S_OK;
	}
	else if (isFileCache())
	{
		*aPos = SetFilePointer( hCacheFile, 0, 0, FILE_CURRENT);
		hr = S_OK;
	}
	return hr;
}

/*long CResourceImpl::getLastModifyDateTime(DWORD* aDateTime)
{
	return E_NOTIMPL;
}*/

long CResourceImpl::getSize(DWORD* aSize)
{
	if (aSize == NULL)
		return E_POINTER;

	long hr = E_NOTIMPL;
	if (isMemCache())
	{
		*aSize = dwCacheDataSize;
		if(*aSize==0xFFFFFFFF)
		{
			ILogWriter * lw = pResMan->GetLogWriter();
			if(lw != NULL)
			{
				CLogValue log("[ERROR] - MemCache error size - ", (LPCTSTR)sResPath.GetBuffer());
				lw->WriteLnLPCSTR(log.GetData());
			}
		}
		hr = S_OK;
	}
	else if (isFileCache())
	{
		*aSize = GetFileSize( hCacheFile, NULL);
		dwFileSize = *aSize;
		hr = S_OK;
	}
	return hr;
}

long CResourceImpl::getCachedSize(DWORD* aSize)
{
	if (aSize == NULL)	
		return E_POINTER;
	*aSize = dwCachedSize;
	return S_OK;
}

long CResourceImpl::getCacheProgress( BYTE* aPercent)
{
	if (aPercent == NULL)
		return E_POINTER;
	
	DWORD dwSize;
	long hRes = getSize( &dwSize);
	if (hRes != S_OK)
		return hRes;

	if (dwSize != 0)
		*aPercent = BYTE(((float)dwCachedSize / dwSize) * 100);
	else
		*aPercent = 100;
	return S_OK;
}

long CResourceImpl::setSize(DWORD aSize)
{
	if( aSize == 0) return E_NOTIMPL;
	return E_NOTIMPL;
}

long CResourceImpl::getStatus(DWORD* aStatus)
{
	*aStatus = dwStatus;
	return S_OK;
}

//int max( int x, int y)  { return (x > y ? x : y);}

long CResourceImpl::unpack( const wchar_t* aPath, BOOL aWait, __int64 aFreeBytesAvailable)
{
	bool bCache = false;
	sUnpackPath.Empty();
	if ( aPath != NULL && wcslen( aPath)>0)
	{
		sUnpackPath = aPath;
		bCache = (wcsncmp( sUnpackPath.GetBuffer(), L"cache:", 6) == 0);
		if (bCache)
			sUnpackPath.Delete( 0, 6);
	}

	if (sUnpackPath.IsEmpty())
	{
		int nPos = sResPath.ReverseFind( L'\\');
		if (nPos == -1)
			sUnpackPath = sResPath;
		else
			sUnpackPath = sResPath.Left( nPos);
	}	
	if (bCache)
		pResMan->getFullPath( sUnpackPath.GetBuffer(), RES_FILE_CACHE, sUnpackPath);
	else
		pResMan->getFullPath( sUnpackPath.GetBuffer(), RES_LOCAL, sUnpackPath);
	
	bNotifyDownload = false;
	freeBytesAvailable = aFreeBytesAvailable;	
	if (aWait)
	{
		bool bEnabled = true;
		unpackRes( CACHING_BLOCK, &bEnabled);
	}
	else
	{
		dwStatus |= RES_ASYNCH;
		bNotifyDownload = ((dwStatus & RES_REMOTE) == RES_REMOTE);

		CSimpleThreadStarter unpackThreadStarter(__FUNCTION__);		
		unpackThreadStarter.SetRoutine(unpackThread_);
		unpackThreadStarter.SetParameter(this);
		m_mutexDestroy.lock();
		if (!m_deleteResultGiven)
		{
			m_canDestroyFromUnzip = false;
			hUnpackThread  = unpackThreadStarter.Start();
		}
		m_mutexDestroy.unlock();
	}
	return S_OK;
}

long CResourceImpl::getUnpackProgress( BYTE* aPercent, DWORD *pwUnpackTotalSize)
{
	if (aPercent == NULL)
		return E_POINTER;

	if((dwStatus & RES_UNPACK_ERROR) == RES_UNPACK_ERROR)
	{
		*aPercent = 100;
		return S_OK;
	}
	if( pwUnpackTotalSize != NULL)
		*pwUnpackTotalSize = dwUnpackTotalSize;
	if (dwUnpackTotalSize != 0)
		*aPercent = BYTE(((float)dwUnpackSize / dwUnpackTotalSize) * 100);
	else if (dwUnpackTotalNumber != 0)
		*aPercent = BYTE(((float)dwUnpackNumber / dwUnpackTotalNumber) * 100);
	else
	{
		if((dwStatus & RES_ASYNCH) == RES_ASYNCH)
			*aPercent = 0;
		else
			*aPercent = 100;
	}
	return S_OK;
}

void CResourceImpl::open( const wchar_t* aResPath, DWORD aFlags, IResourceNotify *aNotify, const wchar_t* aResRefinePath)
{
	if(aResRefinePath!=NULL && wcslen(aResRefinePath)>0)
	{
		sResRefinePath = aResRefinePath;
		bSpecExtension = true;
	}
	sResPath = aResPath;
	if ((aFlags & (RES_FILE_CACHE | RES_LOCAL)) == (RES_FILE_CACHE | RES_LOCAL))
	{
		sFullResPath = sResPath;
		correctCachePath( sFullResPath);
		sFullResPath = pResMan->getCacheBase() + sFullResPath;
		correctPath( sFullResPath);
	}
	else if ((aFlags & RES_LOCAL) > 0)
	{
		assemblePath( sFullResPath, sResPath, pResMan->getLocalBase());
	}
	else if ((aFlags & RES_REMOTE) > 0)
	{
		if(sResPath.Find(L"http://")==0 || sResPath.Find(L"ftp://")==0)
		{
			sFullResPath = sResPath;
			/*int pos = sResPath.Find("//", 0)+2;
			if(pos>-1)
			{
				CWComString sProtocol = sResPath.Left(pos);
				sResPath = sResPath.Right(sResPath.GetLength()-pos);
				sFullResPath = sResPath;
				correctRemotePath( sFullResPath);
				sFullResPath.Insert(0, sProtocol);
			}*/
			//correctRemotePath( sFullResPath);
		}
		else
		{
			sFullResPath = pResMan->getRemoteBase() + sResPath;
			correctRemotePath( sFullResPath);
		}

	}
	dwStatus = aFlags;
	pResMan->notifyOpen( this);
	spResourceNotify = aNotify;
}

void CResourceImpl::initAfterOpen( DWORD aStatus)
{
	dwStatus = aStatus;	
	//getSize( &dwCachedSize);	// 2006-04-05 BDima
	dwCachedSize = 0;			// 2006-04-05 BDima
	if ((dwStatus & (RES_MEM_CACHE | RES_FILE_CACHE)) != 0)
	{
		if ((dwStatus & RES_ASYNCH) == 0)
		{
			bool bEnabled = true;
			createCache( CACHING_BLOCK, &bEnabled);
		}
		else
		{
			dwStatus |= RES_ASYNCH;

			CSimpleThreadStarter cacheThreadStarter(__FUNCTION__);			
			cacheThreadStarter.SetRoutine(cacheThread_);
			cacheThreadStarter.SetParameter(this);
			m_mutexDestroy.lock();
			if (!m_deleteResultGiven)
			{
				m_canDestroyFromCache = false;
				hCacheThread  = cacheThreadStarter.Start();
			}
			m_mutexDestroy.unlock();
		}
	}
}

const wchar_t* CResourceImpl::getStrStatus()
{
	sStatus = L"";
	if ((dwStatus & RES_LOCAL) != 0)
		sStatus += L"Локальный, ";
	if ((dwStatus & RES_REMOTE) != 0)
		sStatus += L"С сервера, ";
	if ((dwStatus & RES_TO_WRITE) != 0)
		sStatus += L"Для записи, ";
	if ((dwStatus & RES_PATH_ERROR) != 0)
		sStatus += L"Не найден, ";
	if ((dwStatus & RES_SERVER_ERROR) != 0)
		sStatus += L"Ошибка сервера, ";
	if ((dwStatus & RES_SERVER_ERROR_TIMEOUT) != 0)
		sStatus += L"Ошибка - нет связи с сервером, ";
	if ((dwStatus & RES_QUERY_ERROR) != 0)
		sStatus += L"Ошибка запроса, ";
	if ((dwStatus & RES_SCRIPT_ERROR) != 0)
		sStatus += L"Ошибка скрипта, ";
	return sStatus.GetString();
}

bool CResourceImpl::isCashingNow()
{
	return bIsCashingNow;
}
	
void CResourceImpl::createCache( DWORD aCachingBlock, bool* aEnabled)
{
	bMemCacheCreating = true;
	bFileCacheCreating = true;
	dwCachedSize = 0;

	if (dwFileSize == 0xFFFFFFFF)
	{
		dwStatus = 0x01000000;
		ILogWriter * lw = pResMan->GetLogWriter();
		if(lw != NULL)
		{
			CLogValue log("[ERROR] - createCache: error size of file: ", (LPCTSTR)sResPath.GetBuffer(), "  dwSize==", (int)dwFileSize);
			lw->WriteLnLPCSTR(log.GetData());
		}
		return;
	}

	if ((dwStatus & RES_ERROR) != 0)
	{
		ILogWriter * lw = pResMan->GetLogWriter();
		if(lw != NULL)
		{
			CLogValue log("[ERROR] - createCache: status is  RES_ERROR in file: ", (LPCTSTR)sResPath.GetBuffer(), "  dwSize==", (int)dwFileSize);
			lw->WriteLnLPCSTR(log.GetData());
		}
		return;
	}

	if (dwStatus >= 0x01000000)
	{
		ILogWriter * lw = pResMan->GetLogWriter();
		if(lw != NULL)
		{
			CLogValue log("[ERROR] - createCache: status is 0x01000000 in file: ", (LPCTSTR)sResPath.GetBuffer(), "  dwSize==", (int)dwFileSize);
			lw->WriteLnLPCSTR(log.GetData());
		}
		return;
	}

	if (!RES_IS_EXISTS( dwStatus))
	{
		ILogWriter * lw = pResMan->GetLogWriter();
		if(lw != NULL)
		{
			CLogValue log("[ERROR] - createCache: file is not exist: ", (LPCTSTR)sResPath.GetBuffer(), "  dwSize==", (int)dwFileSize);
			lw->WriteLnLPCSTR(log.GetData());
		}
		return;
	}

	DWORD dwPosTmp = 0;
	getPos( &dwPosTmp);
	if (dwFileSize > 0)
	{
		FILETIME oTime;
		getTime( oTime);

		if (aCachingBlock > dwFileSize)
			aCachingBlock = dwFileSize;

		// Открываем файл в файловом кеше
		if ((dwStatus & RES_FILE_CACHE) != 0)
		{			
			ECacheFileErrors openError = cfeOpenned;
			if(bSpecExtension)
				openError = pResMan->openCacheFile( sResRefinePath,
								dwStatus, dwFileSize, oTime, &hCacheFile, sCacheFilePath.GetBuffer( MAX_PATH2), MAX_PATH2);
			else
				openError = pResMan->openCacheFile( sResPath,
								dwStatus, dwFileSize, oTime, &hCacheFile, sCacheFilePath.GetBuffer( MAX_PATH2), MAX_PATH2);
			if (openError != cfeOpenned)
				bFileCacheCreating = true;
			else
			{
				bFileCacheCreating = false;
				dwCachedSize = dwFileSize;
			}
		}

		// Выделяем память под кеш 
		if ((dwStatus & RES_MEM_CACHE) != 0 && dwFileSize!=0xFFFFFFFF)
		{
			dwCacheDataSize = dwFileSize;
			pCacheData = MP_NEW_ARR( BYTE, dwFileSize);
		}		

		// Если файла в кеше необходимо создать по мере считывания
		if (bFileCacheCreating && (dwStatus & RES_CACHE_READ) != 0)
		{
			CloseHandle( hCacheFile);
			hCacheFile = INVALID_HANDLE_VALUE;
			bFileCacheCreating = false;
		}

		// Иначме, если файла в кеше нет или необходимо создать кеш в памяти
		else if (bFileCacheCreating && hCacheFile != INVALID_HANDLE_VALUE || (dwStatus & RES_MEM_CACHE) != 0)
		{

			// Считываем данные
			seek( 0, RES_BEGIN);
			BYTE* pDataBlock = MP_NEW_ARR( BYTE, aCachingBlock);
			DWORD dwReadBlock = 0;
			DWORD dwWritenBlock = 0;
			dwCachedSize = 0;

			bIsCashingNow = true;

			/*ILogWriter * lw = pResMan->GetLogWriter();
			if(lw != NULL)
			{
				CLogValue log("[BD-RES] - file is being cached: ", (LPCTSTR)sResPath.GetBuffer());
				lw->WriteLnLPCSTR(log.GetData());
			}*/
			DWORD errorRead = 0;
			while (*aEnabled && dwCachedSize < dwFileSize && SUCCEEDED( read( pDataBlock, aCachingBlock, &dwReadBlock, &errorRead)))
			{
				if ((dwStatus & RES_FILE_CACHE) != 0 && bFileCacheCreating && hCacheFile != INVALID_HANDLE_VALUE)
					WriteFile( hCacheFile, pDataBlock, dwReadBlock, &dwWritenBlock, NULL);
				if ((dwStatus & RES_MEM_CACHE) != 0)
				{
					if(pCacheData==NULL && dwFileSize>0)
					{
						dwCacheDataSize = dwFileSize;
						pCacheData = MP_NEW_ARR( BYTE, dwFileSize);
					}
					else if(dwCacheDataSize<dwFileSize)
					{
						BYTE* pCacheDataTmp = MP_NEW_ARR( BYTE, dwFileSize);
						memset( pCacheDataTmp, 0, dwFileSize);
						rtl_memcpy( pCacheDataTmp, dwFileSize, pCacheData, dwCacheDataSize);
						MP_DELETE_ARR( pCacheData);
						dwCacheDataSize = dwFileSize;
						pCacheData = pCacheDataTmp;
					}
					if( pCacheData)
						rtl_memcpy( pCacheData + dwCachedSize, dwCacheDataSize-dwCachedSize, pDataBlock, dwReadBlock);
				}
				dwCachedSize += dwReadBlock;
				errorRead = 0;
			}

			bIsCashingNow = false;

			MP_DELETE_ARR( pDataBlock);
		}

		if (bFileCacheCreating)
		{
			// Если была ошибка, то удаляем кеш
			if (dwCachedSize < dwFileSize || !*aEnabled)
			{
				if ( pCacheData != NULL && (dwStatus & RES_MEM_CACHE) != 0)
				{
					MP_DELETE_ARR( pCacheData);
					pCacheData = NULL;
					dwCacheDataSize = 0;
					dwStatus &= ~RES_MEM_CACHE;
				}
				if ((dwStatus & RES_FILE_CACHE) == RES_FILE_CACHE)
				{
					CloseHandle( hCacheFile);
					hCacheFile = INVALID_HANDLE_VALUE;
					dwStatus &= ~RES_FILE_CACHE;
				}
				ILogWriter *pLW = pResMan->GetLogWriter();
				if( pLW != NULL)
				{
					if(bSpecExtension)
					{
						pLW->WriteLn("[ERROR] and delete file: ", sResRefinePath.GetBuffer(), " aEnabled==", *aEnabled, "  dwCachedSize==", (unsigned int)dwCachedSize, "  dwFileSize==", (unsigned int)dwFileSize);
						DeleteFileW(sResRefinePath);
					}
					else
					{
						pLW->WriteLn("[ERROR] and delete file: ", sResPath.GetBuffer(), " aEnabled==", *aEnabled, "  dwCachedSize==", (unsigned int)dwCachedSize, "  dwFileSize==", (unsigned int)dwFileSize);
						DeleteFileW(sResPath);
					}
				}
			}
			else if ((dwStatus & RES_FILE_CACHE) != 0)
			{
				if (hCacheFile != INVALID_HANDLE_VALUE)
				{
					if (!SetFileTime( hCacheFile, &oTime, NULL, &oTime))
					{
						int nErr = GetLastError();
						ILogWriter *pLW = pResMan->GetLogWriter();
						if( pLW != NULL)
						{
							pLW->WriteLn("[ERROR] SetFileTime file: ", sResPath.GetBuffer(), " nErr==", nErr);
						}
					}
					if (!CloseHandle( hCacheFile))
					{
						int nErr = GetLastError();
						ILogWriter *pLW = pResMan->GetLogWriter();
						if( pLW != NULL)
						{
							pLW->WriteLn("[ERROR] close file: ", sResPath.GetBuffer(), " nErr==", nErr);
						}
					}
					hCacheFile = INVALID_HANDLE_VALUE;
					
					ECacheFileErrors openError = cfeOpenned;
					if(bSpecExtension)
						openError = pResMan->openCacheFile( sResRefinePath,
												dwStatus, dwFileSize, oTime, &hCacheFile, sCacheFilePath.GetBuffer( MAX_PATH2), MAX_PATH2);
					else
						openError = pResMan->openCacheFile( sResPath,
												dwStatus, dwFileSize, oTime, &hCacheFile, sCacheFilePath.GetBuffer( MAX_PATH2), MAX_PATH2);
					
					if (openError != cfeOpenned)
					{
						dwStatus &= ~RES_FILE_CACHE;
						ILogWriter *pLW = pResMan->GetLogWriter();
						if( pLW != NULL)
						{
							int nErr = GetLastError();
							pLW->WriteLn("[ERROR] cannot open file: ", sResPath.GetBuffer(), " nErr==", nErr);
						}
						if (hCacheFile != INVALID_HANDLE_VALUE)
						{
							CloseHandle( hCacheFile);
							hCacheFile = INVALID_HANDLE_VALUE;
						}
					}
					else
					{
						if(bFileCacheCreating && dwFileSize>0 && hCacheFile!=NULL && hCacheFile!=INVALID_HANDLE_VALUE)
						{
							if(oTime.dwHighDateTime==0 && oTime.dwLowDateTime==0)
							{
								GetFileTime( hCacheFile, NULL, NULL, &oTime);
							}
							pResMan->addRemoteResourceInfo(sFullResPath, dwFileSize, oTime);
						}
					}
				}
			}
		}
	}
	dwCachedSize = dwFileSize;
	bFileCacheCreating = false;
	bMemCacheCreating = false;
	seek( dwPosTmp, RES_BEGIN);
}

/*void WINAPI SendApplicationMessageQQ(unsigned long a1, unsigned long a2, unsigned a3, unsigned a4, unsigned a5, 
								   unsigned a6, unsigned a7, unsigned a8,char a9, 
								   LPSTR pFile, LPSTR a11, unsigned long a12, char a13,  void* pVector){
	vector<std::string> *vFiles = (vector<std::string>*)pVector;
	char *pFileName = NULL;
	if((pFileName = strchr(pFile, '/')) || (pFileName = strchr(pFile, '\\'))){
		pFileName++;
	}
	else{
		pFileName = pFile;
	}
	vFiles->push_back(pFileName);	
}*/

int WINAPI unzPrint(LPSTR apsz, unsigned long aul){
	// ??
	if( apsz == NULL || aul == 0) return 0;
	return 0;
}

int WINAPI unzPrintToBuffer(LPSTR apsz, unsigned long aul){
	
	if( apsz == NULL || aul == 0) return 0;
	return 0;
}

int WINAPI unzReplace(LPSTR apsz){
	if( apsz == NULL) return 0;
	return 0;
}

int WINAPI unzService(LPCSTR aName, unsigned long aSize)
{
	LPUSERFUNCTIONS pUserFuncs = Wiz_GetUserFunctions();
	((CResourceImpl*)(pUserFuncs->pUserData))->dwUnpackNumber++;
	((CResourceImpl*)(pUserFuncs->pUserData))->dwUnpackSize += aSize;
	((CResourceImpl*)(pUserFuncs->pUserData))->notifyResMan(RN_ASYNCH_UNPACKED);
	if(aName == NULL) aName = NULL;
	return (((CResourceImpl*)(pUserFuncs->pUserData))->bEnableUnpackThread ? 0 : -1);
}


int WINAPI unzService2(LPCSTR aName, unsigned long aSize)
{
	if(aName == NULL || aSize == 0) return 0;
	return 0;
}

void CResourceImpl::unpackRes( DWORD aCachingBlock, bool* aEnabled)
{
	dwUnpackTotalSize = dwUnpackSize = dwUnpackTotalNumber = dwUnpackNumber = 0;
	if (hCacheFile == INVALID_HANDLE_VALUE)
	{
		dwStatus |= RES_FILE_CACHE;
		createCache( aCachingBlock, aEnabled);
	}
	if (*aEnabled && hCacheFile != INVALID_HANDLE_VALUE)
	{
		/*if (bNotifyDownload)
			notifyResMan(RN_ASYNCH_DOWNLOADED);*/
		notifyResMan(RN_ASYNCH_UNPACKED);
		FlushFileBuffers( hCacheFile);
		unpackFile( sCacheFilePath);
	}
}

void CResourceImpl::GetListFileFromDir( const wchar_t* aExtractDir, vector<std::wstring> &FileExtractDir){

	CFileFind find;
	std::wstring sFile;
	std::wstring sNewPath;
	std::wstring sPath = aExtractDir;

	if( sPath[ sPath.size()-1] == L'\\' )
	{
		//??
	}
	else
	{
		sPath  +=  L'\\';
	}
	sFile = sPath + L"*.*";
  
	wchar_t path_buffer[MAX_PATH2];
    wchar_t fname[_MAX_FNAME];
    wchar_t ext[_MAX_EXT];

	BOOL bFind = find.FindFile( sFile.c_str() );  
	while( bFind )
	{
		bFind = find.FindNextFile();
		if( find.IsDirectory() )
		{
			if( !find.IsDots() )
			{
				sNewPath = find.GetFilePath();
				
				_wsplitpath_s( sNewPath.c_str(), NULL, 0, NULL, 0, fname, _MAX_FNAME, ext, _MAX_EXT );
				_wmakepath_s( path_buffer, MAX_PATH2, NULL, NULL, fname, ext);
				
				GetListFileFromDir((sPath+path_buffer).c_str(), FileExtractDir);
			}
		}
		else 
		{
			if( !find.IsHidden())
			{
				sNewPath = find.GetFilePath();
				wchar_t *pStr = (wchar_t*)sNewPath.c_str();
				pStr++;
				sNewPath = pStr;
				sNewPath.insert(0, sPath.c_str());
				FileExtractDir.push_back( sNewPath );
			}  
		}
	}
}

void CResourceImpl::GetZipList(const wchar_t* aZipFile, vector<std::wstring> &FileZip)
{
	DCL oDCL;
	USERFUNCTIONS oUserFunc;
	ZeroMemory( &oDCL, sizeof( oDCL));
	ZeroMemory( &oUserFunc, sizeof( oUserFunc));
	oDCL.noflag = 1;
	oDCL.ndflag = 1;
	oDCL.nvflag = 1;
	//oDCL.ntflag = true;
	
	sExtractFile = aZipFile;
	sExtractDir = sUnpackPath.GetString();

	USES_CONVERSION;
	oDCL.lpszZipFN = W2A(sExtractFile.GetBuffer());
	oDCL.lpszExtractDir = W2A(sExtractDir.GetBuffer());
	oUserFunc.print = unzPrintToBuffer;
	oUserFunc.replace = unzReplace;
	oUserFunc.ServCallBk = unzService;
	oUserFunc.pUserData = &FileZip;

	/*int iErr = */Wiz_SingleEntryUnzip(/*countFileZip*/0, 0, 0, NULL, &oDCL, &oUserFunc);
}

BOOL CResourceImpl::CompareFileInList(const wchar_t* aZipFile, const wchar_t* aUnpackPath){
	// список файлов в директории, в которую собираемся распаковывать
	vector<std::wstring> FileExtractDir;
	// список файлов в zip
	vector<std::wstring> FileZip;
	GetListFileFromDir(aUnpackPath, FileExtractDir);
	GetZipList(aZipFile, FileZip);
	int sizeFE = FileExtractDir.size();
	int sizeFZ = FileZip.size();
	int i=0;
	int j=0;
    if(sizeFE<1/*!=sizeFZ*/){
		return FALSE;
    }
	BOOL bCheck = FALSE;
	while(i<sizeFE && j<sizeFZ){ 
		wchar_t *pStr = (wchar_t*)FileExtractDir.at(i).c_str();
		if(wcsstr(pStr, aUnpackPath))
		{
			pStr+=wcslen(aUnpackPath);
		}
		/*if(pStr2 != NULL){
			pStr=pStr2;
		}*/
		wchar_t *pStrInArch = NULL;
		while(j<sizeFZ){
			pStrInArch = (wchar_t*)FileZip.at(j).c_str();
			if(!wcscmp(pStr, pStrInArch)){
				break;
			}
			j++;
		}
		if(j>=sizeFZ){
			j=0;
			i++;
			continue;
		}
		bCheck = TRUE;
		j=0;
		HANDLE hfile = ::CreateFileW(pStr, /*FILE_READ_ATTRIBUTES*/NULL, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
		DWORD sizeFile;
		::GetFileSize(hfile, &sizeFile);
		hfile = ::CreateFileW(pStrInArch, /*FILE_READ_ATTRIBUTES*/NULL, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
		DWORD sizeZFile;
		::GetFileSize(hfile, &sizeZFile);
		if(sizeZFile!=sizeFile){
			return FALSE;
		}
		i++;
	}
	if(bCheck)
		return TRUE;
	else
		return FALSE;
}

void CResourceImpl::unpackFile( const wchar_t* aZipFile)
{
	/*if(CompareFileInList(aZipFile, sUnpackPath)){
		return;
	}*/
	/*DCL oDCL;
	USERFUNCTIONS oUserFunc;
	ZeroMemory( &oDCL, sizeof( oDCL));
	ZeroMemory( &oUserFunc, sizeof( oUserFunc));
	oDCL.noflag = true;
	oDCL.ndflag = true;
	oDCL.nvflag = true;*/

	sExtractFile = aZipFile;
	sExtractDir = sUnpackPath.GetString();
	WCHAR odlCurDir[MAX_PATH2];
	GetCurrentDirectoryW( MAX_PATH2, odlCurDir);	
	SetCurrentDirectoryW( sExtractDir);
	HZIP hzip = OpenZip( (void*)sExtractFile.GetBuffer(), NULL, ZIP_FILENAME);
	if( hzip == NULL)
	{
		dwStatus |= RES_UNPACK_ERROR;
	}	
	ZIPENTRY ze; 
	GetZipItem(hzip,-1,&ze);
	int numitems=ze.index;
	USES_CONVERSION;
	for (int i=0; i<numitems; i++)
	{ 
		GetZipItem(hzip,i,&ze);
		//CWComString s = sExtractDir + A2W(ze.name);
		CWComString s = CharToWide(ze.name);
		UnzipItem(hzip, i, (void*)s.GetBuffer() ,0,ZIP_FILENAME);
	}
	SetCurrentDirectoryW( odlCurDir);
	// CloseZip(hz);
	/*oDCL.lpszZipFN = (LPSTR)sExtractFile.GetBuffer();
	oDCL.lpszExtractDir = (LPSTR)sExtractDir.GetBuffer();
	oUserFunc.print = unzPrint;
	oUserFunc.replace = unzReplace;
	oUserFunc.ServCallBk = unzService;
	oUserFunc.pUserData = this;
	//char *pArrayFile[1024];
	//countFileZip = 0;
	int iErr = Wiz_SingleEntryUnzip(0, NULL, 0, NULL, &oDCL, &oUserFunc);
	oUserFunc.print = unzPrint;
	oDCL.nvflag = false;
	dwUnpackTotalSize = oUserFunc.TotalSize;
	dwUnpackTotalNumber = oUserFunc.NumMembers;
	if( freeBytesAvailable != -1 && freeBytesAvailable <= dwUnpackTotalSize)
	{
		dwStatus |= RES_UNPACK_ERROR;
		dwUnpackSize = dwUnpackTotalSize;
		dwUnpackNumber = dwUnpackTotalNumber;
		return;
	}
	if (dwUnpackTotalNumber > 0)
		iErr = Wiz_SingleEntryUnzip(0, NULL, 0, NULL, &oDCL, &oUserFunc);
	if (iErr != 0)
		dwStatus |= RES_UNPACK_ERROR;
	else
		dwStatus &= ~RES_UNPACK_ERROR;*/
}

BOOL CResourceImpl::unpackFileToMemory( const wchar_t*  aZipFile, const wchar_t*  aFileName, DWORD *aDataSize, BYTE **aData)
{
	UzpBuffer oBuffer;
	ZeroMemory( &oBuffer, sizeof( oBuffer));

	USERFUNCTIONS oUserFunc;
	ZeroMemory( &oUserFunc, sizeof( oUserFunc));
	oUserFunc.print = unzPrint;
	oUserFunc.replace = unzReplace;
	oUserFunc.ServCallBk = unzService2;
	oUserFunc.pUserData = this;

	CComString sZipFile = aZipFile;
	CComString sFileFile = aFileName;
	int iErr = Wiz_UnzipToMemory( (LPSTR)sZipFile.GetBuffer(), (LPSTR)sFileFile.GetBuffer(), &oUserFunc, &oBuffer);
	if (iErr == 0)
		return FALSE;

	*aDataSize = oBuffer.strlength;
	unsigned int allocSize = oBuffer.strlength+1;
	//*aData = (BYTE*)GlobalAllocPtr( GPTR, allocSize);
	*aData = MP_NEW_ARR( BYTE, allocSize);
	rtl_memcpy( *aData, allocSize, oBuffer.strptr, oBuffer.strlength);
	

	MP_DELETE_ARR( oBuffer.strptr);

	return TRUE;
}

DWORD WINAPI CResourceImpl::cacheThread_(LPVOID param)
{
	CResourceImpl* aThis = (CResourceImpl*)param;
	return aThis->cacheThread();
}

int CResourceImpl::cacheThread()
{
	bEnableCacheThread = true;
	dwStatus |= RES_ASYNCH;
	createCache( CACHING_BLOCK, &bEnableCacheThread);
	dwStatus &= ~RES_ASYNCH;

	notifyResMan(RN_ASYNCH_CACHED);
	HANDLE hThreadCopy = hCacheThread;
	hCacheThread = NULL;

	CheckAndDestroyIfNeeded(&m_canDestroyFromCache);

	CloseHandle( hThreadCopy);
	

	return 0;
}

void CResourceImpl::notifyResMan( DWORD aNotifyCode, DWORD aNotifyServerCode, wchar_t *aErrorText)
{
	mutexClose.lock();
	if (spResourceNotify != NULL && pResMan != NULL)
	{
		pResMan->onResourceNotify(spResourceNotify, this, aNotifyCode, aNotifyServerCode, aErrorText);
	}
	mutexClose.unlock();
}

DWORD WINAPI CResourceImpl::unpackThread_(LPVOID param)

{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();

	CResourceImpl* aThis = (CResourceImpl*)param;
	return aThis->unpackThread();
}

int CResourceImpl::unpackThread()
{
	bEnableUnpackThread = true;
	dwStatus |= RES_ASYNCH;
	bool bEnabled = bEnableUnpackThread;
	unpackRes( CACHING_BLOCK, &bEnabled);
	bEnableUnpackThread = bEnabled;
	dwStatus &= ~RES_ASYNCH;
	
	if (bEnableUnpackThread)
		notifyResMan(RN_ASYNCH_UNPACKED);

	HANDLE hThreadCopy = hUnpackThread;
	hUnpackThread = NULL;

	CheckAndDestroyIfNeeded(&m_canDestroyFromUnzip);

	CloseHandle( hThreadCopy);
	return 0;
}

bool CResourceImpl::downloadRes( bool aCacheOnly)
{
	ILogWriter * lw = pResMan->GetLogWriter();
	if(lw != NULL)
	{
		CLogValue log(" downloadRes: ", (LPCTSTR)sResPath.GetBuffer());
		lw->WriteLnLPCSTR(log.GetData());
	}
	dwStatus &= ~RES_CACHE_READ;
	dwStatus |= RES_FILE_CACHE;
	createCache( CACHING_BLOCK, &bEnableCacheThread);
	if (hCacheFile == INVALID_HANDLE_VALUE || aCacheOnly)
		return false;
	CWComString sLocalFileName = sResLocalPath;
	CWComString sResPathNew;
	if(bSpecExtension)
		sResPathNew = sResRefinePath;
	else
		sResPathNew = sResPath;

	FILETIME oTime;
	getTime(oTime);
	CloseHandle( hCacheFile);
	hCacheFile = INVALID_HANDLE_VALUE;
	if(pResMan == NULL)
		return false;
	bool bRes = pResMan->copyCacheToLocal( sCacheFilePath, sResPathNew, sLocalFileName, oTime, dwFileSize);
	if ( bRes)
	{
		/*HANDLE hLocalFile = CreateFile( sLocalFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hLocalFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle( hCacheFile);
			hCacheFile = hLocalFile;
			seek( 0, RES_BEGIN);
		}*/
		}	
	return bRes;
}

DWORD WINAPI CResourceImpl::downloadToCacheThread_(LPVOID param)
{
	CResourceImpl* aThis = (CResourceImpl*)param;
	return aThis->downloadToCacheThread();
}

int CResourceImpl::downloadToCacheThread()
{
	bEnableCacheThread = true;
	dwStatus |= RES_ASYNCH;
	downloadRes( true);
	dwStatus &= ~RES_ASYNCH;
	
	if (bEnableCacheThread)
		notifyResMan(RN_ASYNCH_DOWNLOADED);

	HANDLE hThreadCopy = hDownloadToCacheThread;
	hDownloadToCacheThread = NULL;

	CheckAndDestroyIfNeeded(&m_canDestroyFromDownloadCache);

	CloseHandle( hThreadCopy);
	
	return 0;
}

DWORD WINAPI CResourceImpl::downloadThread_(LPVOID param)
{
	CResourceImpl* aThis = (CResourceImpl*)param;
	return aThis->downloadThread();
}

int CResourceImpl::downloadThread()
{
	bEnableCacheThread = true;
	dwStatus |= RES_ASYNCH;
	bool resultDownload = downloadRes();
	dwStatus &= ~RES_ASYNCH;
	
	if (bEnableCacheThread && resultDownload)
		notifyResMan(RN_ASYNCH_DOWNLOADED);
	else
		notifyResMan(RN_DOWNLOAD_ERROR);

	HANDLE hThreadCopy = hDownloadThread;
	hDownloadThread = NULL;
	CheckAndDestroyIfNeeded(&m_canDestroyFromDownload);

	CloseHandle( hThreadCopy);
	
	return 0;
}

long CResourceImpl::getURL(BSTR *a_ppURL)
{
	// TODO: Add your implementation code here
	CWComString sPath = sFullResPath;
	*a_ppURL = sPath.AllocSysString();
	return S_OK;
}

bool CResourceImpl::getRefineLoadedResourcePath(wchar_t* pBuffer, unsigned int auBufferSize)
{
	if(bSpecExtension)
	{
		ATLASSERT(auBufferSize > sResRefinePath.GetLength());
		if( auBufferSize > (unsigned int)sResRefinePath.GetLength())
			wcscpy_s( pBuffer, auBufferSize, sResRefinePath.GetBuffer());
		else
			return false;
	}
	return bSpecExtension;
}

long CResourceImpl::unpackToMemory(BSTR aFileName, BOOL* aSuccessful, DWORD *aDataSize, BYTE **aData)
{
	if (aSuccessful == NULL || aDataSize == NULL || aData == NULL)
		return E_POINTER;
	*aSuccessful = TRUE;
	*aDataSize = 0;
	*aData = NULL;

	if ((dwStatus & RES_LOCAL) == 0)
	{
		*aSuccessful = FALSE;
		return S_OK;
	}

	CWComString sZipFile( sFullResPath);
	CWComString sFileName( aFileName);
	*aSuccessful = unpackFileToMemory( sZipFile.GetBuffer(), sFileName.GetBuffer(), aDataSize, aData);

	return S_OK;
}

long CResourceImpl::getDownloadedSize( DWORD* aSize)
{
	return getCachedSize( aSize);
}

long CResourceImpl::getUploadedSize( /* [out] */ DWORD *aSize){
	// ??
	if( aSize == NULL) return E_NOTIMPL;
	return E_NOTIMPL;
}

long CResourceImpl::download( IResourceNotify* aNotify, const wchar_t* apwcLocalPath)
{
	sResLocalPath = apwcLocalPath != NULL ? apwcLocalPath : L"";
	// Загружаем, если ресурс не является локальным
	if (RES_IS_EXISTS( dwStatus) && (dwStatus & (RES_LOCAL | RES_FILE_CACHE)) == RES_LOCAL)
	{
		if (aNotify != NULL)
		{
			spResourceNotify = aNotify;
			notifyResMan(RN_ASYNCH_DOWNLOADED);
		}
		return S_OK;
	}
	
	if (aNotify == NULL)
		downloadRes();
	else
	{
		spResourceNotify = aNotify;

		CSimpleThreadStarter downloadThreadStarter(__FUNCTION__);		
		downloadThreadStarter.SetRoutine(downloadThread_);
		downloadThreadStarter.SetParameter(this);
		m_mutexDestroy.lock();
		if (!m_deleteResultGiven)
		{
			m_canDestroyFromDownload = false;
			hDownloadThread  = downloadThreadStarter.Start();
		}
		m_mutexDestroy.unlock();
	}

	return S_OK;
}

long CResourceImpl::downloadToCache( IResourceNotify* aNotify)
{
	// Загружаем, если ресурс не является локальным
	ILogWriter * lw = pResMan->GetLogWriter();
	if(lw != NULL)
	{
		CLogValue log(" downloadToCache: ", (LPCTSTR)sResPath.GetBuffer());
		lw->WriteLnLPCSTR(log.GetData());
	}
	if (RES_IS_EXISTS( dwStatus) && (dwStatus & (RES_LOCAL | RES_FILE_CACHE)) == RES_LOCAL)
	{
		if (aNotify != NULL)
		{
			spResourceNotify = aNotify;
			notifyResMan(RN_ASYNCH_DOWNLOADED);
		}
		return S_OK;
	}
	
	if (aNotify == NULL)
		downloadRes( true);
	else
	{
		spResourceNotify = aNotify;

		CSimpleThreadStarter downloadThreadStarter(__FUNCTION__);		
		downloadThreadStarter.SetRoutine(downloadToCacheThread_);
		downloadThreadStarter.SetParameter(this);
		m_mutexDestroy.lock();
		if (!m_deleteResultGiven)
		{
			m_canDestroyFromDownloadCache = false;
			hDownloadToCacheThread  = downloadThreadStarter.Start();
		}
		m_mutexDestroy.unlock();
	}
	return S_OK;
}

long CResourceImpl::getDownloadProcess(  BYTE* aPercent)
{
	return getCacheProgress( aPercent);
}

long CResourceImpl::unpack2(const wchar_t *aPath, IResourceNotify *aNotify)
{
	spResourceNotify = aNotify;
	CWComString sPath = aPath != NULL ? aPath : L""; 
	unpack( sPath.GetBuffer(), aNotify == NULL, -1);
	return S_OK;
}

long CResourceImpl::unpackToCache(BSTR aPath, IResourceNotify *aNotify)
{
	spResourceNotify = aNotify;
	CWComString bsPath( L"cache:");
	bsPath += aPath;
	unpack( bsPath.GetBuffer(), aNotify == NULL, -1);
	return S_OK;
}

long CResourceImpl::upload(/*[in]*/ IResource* aSrcRes,  /*[in]*/ IResourceNotify* aNotify)
{
	if (aSrcRes == NULL)
		return E_POINTER;
	
	if (aNotify == NULL)
		return E_NOTIMPL;

	return E_NOTIMPL;
}
