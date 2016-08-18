// RemoteResource.cpp: implementation of the CRemoteResource class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "comman.h"
#include "ResMan.h"
#include "RemoteResource.h"
#include "InfoQueryMan.h"
#include "ZLIBENGN.H"
#include "ThreadAffinity.h"
#include "CrashHandlerInThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CRemoteResource
CRemoteResource::CRemoteResource() : CResourceImpl()
{
	dwPos = 0;
	wFirstBlockSize = 0;
	flag = FALSE;
	bInfoSet = FALSE;
	bTimeSet = FALSE;
	dwFileSize = 0;
	pSrcResource = NULL;
	bSpecExtension = false;
	sResRefinePath = L"";
	hCacheFile = INVALID_HANDLE_VALUE;
	pCacheData = NULL;
}

CRemoteResource::~CRemoteResource()
{
	close();
}

void	CRemoteResource::updateInfo(BOOL aAlways)
{
	if( bInfoSet && !aAlways)	return;
	CResInfo oResInfo;
	/*if (sResPath.Find( L"ui") >= 0 || sResPath.Find( L"UI") >= 0)
	{
		int n = 0;
	}*/
	mutexClose.lock();
	if(pResMan != NULL)
		pResMan->getResourceInfo( sResPath, dwStatus, &oResInfo, true);
	else
	{
		mutexClose.unlock();
		return;
	}
	mutexClose.unlock();
	/*if (sResPath.Find( L"ui") >= 0 || sResPath.Find( L"UI") >= 0)
	{
		int n = 0;
	}*/
	setInfo( oResInfo.dwStatus, oResInfo.dwSize, oResInfo.oTime);
	bTimeSet = TRUE;
}

void	CRemoteResource::setInfo( DWORD aStatus, DWORD aSize, FILETIME aTime)
{
	oTime = aTime;	
	dwFileSize = aSize;
	if (!RES_IS_ERROR( aStatus))
	{
		dwStatus = (dwStatus & ~RES_ANYWHERE) | aStatus;
		/*if (dwSize < 0xFFFF)
			oResInfo.dwStatus |= RES_MEM_CACHE;*/
	}
	else
	{
		dwStatus = aStatus;
	}
	bInfoSet = TRUE;
}

void CRemoteResource::open( const wchar_t* aResPath, DWORD aFlags, IResourceNotify *aNotify, const wchar_t* aResRefinePath)
{
	if( (aFlags & RES_TO_WRITE)==0 && (aFlags & RES_NOT_SAVE_TO_CACHE)==0)
		aFlags |= RES_FILE_CACHE;
	CResourceImpl::open( aResPath, aFlags, aNotify, aResRefinePath);
	dwPos = 0;	
	dwCachedSize = 0;
	initAfterOpen( aFlags);
}

void CRemoteResource::getTime( FILETIME& aTime)
{
	if(!bTimeSet)	updateInfo(TRUE);
	aTime = oTime;
}

long CRemoteResource::close()
{
	long hr = CResourceImpl::close();
	if (hr == E_NOTIMPL)
		hr = S_OK;
	return hr;
}

long CRemoteResource::read(BYTE * aBuffer, DWORD aBufferSize, DWORD* aReadSize, DWORD* aError)
{
	ATLASSERT( !flag);
	flag = TRUE;
	/*ILogWriter * lw = pResMan->GetLogWriter();
	if(lw!=NULL)z
	{
		CLogValue log("RemoteResource: remote reading file: ", sResPath.GetBuffer());
		lw->WriteLnLPCSTR(log.GetData());
	}*/

#if LOG_REMOTE_RESOURCE
	ILogWriter * lw = pResMan->GetLogWriter();
	if(lw != NULL)
	{
		lw->WriteLn("[READING] CRemoteResource::read full path=", sFullResPath.GetBuffer(), " no full path:", sResPath.GetBuffer());
	}
#endif	
	long hr = CResourceImpl::read( aBuffer, aBufferSize, aReadSize, aError);
	if (hr == E_NOTIMPL)
	{
		//ATLTRACE("before read %d - %s", dwPos, (LPSTR)sFullResPath);
		BYTE* pData = NULL;
		DWORD dwNewStatus = dwStatus;
		*aReadSize = 0;

		bool bInFirstBlock = false;
		/*if (dwPos == 0 && aBufferSize <= wFirstBlockSize)
		{
			bInFirstBlock = true;
			*aReadSize = aBufferSize;
			memcpy( aBuffer, btFirstBlock, *aReadSize);
			dwNewStatus = dwStatus;
		}*/

		if(pCacheData!=NULL && dwCachedSize==dwFileSize && dwCachedSize>dwPos)
		{
			DWORD delta = dwCachedSize-dwPos;
			if( delta < aBufferSize)
			{
				rtl_memcpy( aBuffer, aBufferSize, &pCacheData[dwPos], delta);
				*aReadSize = delta;
				dwPos += delta;
			}
			else
			{
				rtl_memcpy( aBuffer, aBufferSize, &pCacheData[dwPos], aBufferSize);
				*aReadSize = aBufferSize;
				dwPos +=aBufferSize;
			}
			if (dwPos >= dwFileSize)
			{
				if (!CResourceImpl::isCashingNow() && (dwStatus & RES_FILE_CACHE))
				{
					CResourceImpl::saveToCache(pCacheData, dwFileSize);
				}

				MP_DELETE_ARR( pCacheData);
				pCacheData = NULL;
				dwCachedSize = 0;
			}
			hr = S_OK;
			flag = FALSE;
			return hr;
		}


		if (bInFirstBlock || pResMan->readRemoteResource( sFullResPath, dwPos, aBufferSize,
				aReadSize, &pData, &dwNewStatus, &dwFileSize, aError))
		{
			if (*aReadSize == 0)
			{
				ILogWriter * lw = pResMan->GetLogWriter();
				if(lw != NULL)
				{
					lw->WriteLn("[NOT READING] sFullResPath=", sFullResPath.GetBuffer());
				}
			}
			//ATLTRACE("after read %d - %s", dwPos, (LPSTR)sFullResPath);
			dwStatus |= dwNewStatus;	
			
			if((dwStatus & RES_PATH_ERROR) == RES_PATH_ERROR
				|| ((dwStatus & RES_READ_ERROR) == RES_READ_ERROR && dwPos==0))
			{
				ILogWriter * lw = pResMan->GetLogWriter();
				if(lw != NULL)
				{
					lw->WriteLn("[ERROR] RES_PATH_ERROR or RES_READ_ERROR sFullResPath=", sFullResPath.GetBuffer(), "  dwStatus==", dwStatus, "  dwPos==", dwPos);
				}
				bInfoSet = false;
				if(dwPos == 0 && pCacheData == NULL)
				{
					if(hCacheFile!=INVALID_HANDLE_VALUE)
					{
						CloseHandle(hCacheFile);
						hCacheFile = INVALID_HANDLE_VALUE;
					}
					DeleteFileW(sCacheFilePath);
					dwFileSize = 0;
				}
			}

			if( !bInfoSet)
			{
				setInfo( dwStatus, dwFileSize, FILETIME());
			}

			if (dwFileSize>0 && ((dwPos == 0) || (pCacheData == NULL)))
			{
				if(pCacheData!=NULL)
					MP_DELETE_ARR( pCacheData);

				pCacheData = MP_NEW_ARR( unsigned char, dwFileSize);
				memset(pCacheData, 0, dwFileSize);
			}
			if (pData != NULL)
			{
				dwStatus |= dwNewStatus;
				if(*aReadSize<aBufferSize)
					rtl_memcpy( aBuffer, aBufferSize, pData, *aReadSize);
				else
					rtl_memcpy( aBuffer, aBufferSize, pData, aBufferSize);

				if(*aReadSize<aBufferSize)
				{
					if( pCacheData != NULL)
						rtl_memcpy( pCacheData + dwPos, dwFileSize-dwPos, aBuffer, *aReadSize);
					dwPos += *aReadSize;
					dwCachedSize+=*aReadSize;
				}
				else
				{
					if( pCacheData != NULL)
						rtl_memcpy( pCacheData + dwPos, dwFileSize-dwPos, pData, *aReadSize);
					dwPos += aBufferSize;
					dwCachedSize+=*aReadSize;
					*aReadSize = aBufferSize;
				}

				MP_DELETE_ARR( pData);
			}

			if (dwFileSize>0 && dwPos >= dwFileSize)
			{
				if (!CResourceImpl::isCashingNow() && (dwStatus & RES_FILE_CACHE))
					CResourceImpl::saveToCache(pCacheData, dwFileSize);
				
				if( pCacheData != NULL)
				{
					MP_DELETE_ARR( pCacheData);
					pCacheData = NULL;
				}
				dwCachedSize = 0;
			}

			hr = S_OK;
		}
	}
	flag = FALSE;
	return hr;
}

long CRemoteResource::write(const BYTE * aBuffer, DWORD aBufferSize, DWORD* aWroteSize, DWORD aTotalSize)
{
	updateInfo();
	long hr = CResourceImpl::write( aBuffer, aBufferSize, aWroteSize, aTotalSize);
	if (hr == E_NOTIMPL)
	{
		dwStatus = dwStatusUploader;
		DWORD dwNewStatus = dwStatus;
	
		// надо ли делать проверку позиции и размер файла
		//if ( dwPos >= dwSize)
		//	return S_OK
		mutexClose.lock();		
		if(pResMan != NULL)
			hr = pResMan->writeRemoteResource( sFullResPath, (BYTE*)aBuffer, dwPos, aBufferSize, aWroteSize, aTotalSize, &dwNewStatus);
		mutexClose.unlock();
		if ( hr==0)
		{
			dwPos += *aWroteSize;
		}		
		dwStatus = dwNewStatus;
	}

	return hr;
}

long CRemoteResource::seek(DWORD aPos, BYTE aFrom)
{
	updateInfo();
	long hr = CResourceImpl::seek( aPos, aFrom);
	if (hr == E_NOTIMPL)
	{
		switch (aFrom) {
		case RES_BEGIN:
			dwPos = aPos;
			break;
		case RES_CURRENT:
			dwPos += aPos;
			break;
		case RES_END:
			if (aPos < dwFileSize)
				dwPos = dwFileSize - aPos;
			else
				dwPos = 0;
			break;
		default:
			dwPos = aPos;
		}
		hr = S_OK;
	}
	return hr;
}

long CRemoteResource::getPos(DWORD* aPos)
{
	updateInfo();
	long hr = CResourceImpl::getPos( aPos);
	if (hr == E_NOTIMPL)
	{
		*aPos = dwPos;
		hr = S_OK;
	}
	return hr;
}

long CRemoteResource::getSize(DWORD* aSize)
{
	updateInfo();
	long hr = CResourceImpl::getSize( aSize);
	if (hr == E_NOTIMPL)
	{
		if ( pSrcResource != NULL)
		{			
			if ( ((dwStatus & RES_UPLOAD) != 0) && ((dwStatus & RES_TO_WRITE) != 0))
			{
				DWORD dwFileSizeTemp = 0xFFFFFFFF;
				hr = pSrcResource->getSize( &dwFileSizeTemp);
				if( dwFileSizeTemp == -1)
				{
					ILogWriter * lw = pResMan->GetLogWriter();
					if(lw != NULL)
					{
						lw->WriteLnLPCSTR("SrcResource::size==-1");
					}
				}
				else
					dwFileSize = dwFileSizeTemp;
			}
		}
		*aSize = dwFileSize;
		hr = S_OK;
	}
	return hr;
}

long CRemoteResource::getUploadedSize( DWORD *aSize)
{
	long hr = CResourceImpl::getUploadedSize( aSize);
	if (hr == E_NOTIMPL)
	{
		*aSize = dwWriteCount;
		hr = S_OK;
	}
	return hr;
}

long CRemoteResource::getStatus(DWORD* aStatus)
{
	updateInfo();
	return CResourceImpl::getStatus( aStatus);
}

long CRemoteResource::getUploadInfo( byte &btStatusServerCode, std::wstring &sErrorText)
{
	updateInfo();

	dwStatus = dwStatusUploader;

	long hr = E_NOTIMPL;
	mutexClose.lock();
	if( !bEnableUploadThread || pResMan == NULL)
	{
		mutexClose.unlock();
		return E_FAIL;
	}
	mutexClose.unlock();
	int iError = pResMan->getUploadRemoteResource( sFullResPath, &dwStatus, &btStatusServerCode, sErrorText);
	if ( iError==0)
	{
		hr = S_OK;
	}
	else
	{
		hr = E_FAIL;
	}
	return hr;
}

//////////////////////////////////////////////////////////////////////////

long CRemoteResource::upload(IResource* aSrcRes, IResourceNotify* aNotify)
{
	long hr = CResourceImpl::upload( aSrcRes, aNotify);
	if (hr == E_NOTIMPL)
	{
		spResourceNotify = aNotify;
		pSrcResource = aSrcRes;

		CSimpleThreadStarter uploadThreadStarter(__FUNCTION__);		
		uploadThreadStarter.SetRoutine(uploadThread_);
		uploadThreadStarter.SetParameter(this);
		m_mutexDestroy.lock();
		if (!m_deleteResultGiven)
		{
			m_canDestroyFromUpload = false;
			hUploadThread = uploadThreadStarter.Start();	
		}
		m_mutexDestroy.unlock();
	}
	//
	return S_OK;
}

DWORD WINAPI CRemoteResource::uploadThread_(LPVOID param)
{
	CRemoteResource* aThis = (CRemoteResource*)param;
	return aThis->uploadThread();
}

int CRemoteResource::uploadThread()
{
	bEnableUploadThread = true;
	dwStatus |= RES_ASYNCH;
	if((dwStatus & RES_EXISTS) != RES_EXISTS)
		dwStatus |= RES_EXISTS;
	dwStatus &= ~RES_FILE_CACHE;
	if((dwStatus & RES_UPLOAD) != RES_UPLOAD)
		dwStatus |= RES_UPLOAD;
	dwStatusUploader = dwStatus;
	/*int res = */uploadRes();
	//bool success = (res == 0) ? false : true;
	dwStatus &= ~RES_ASYNCH;
	

	
	/*if (bEnableUploadThread)
	{
		if( success)
		{
			notifyResMan(RN_ASYNCH_UPLOADED);
		}
		else {
			// установить состояние ошибки и закончить
			// ??		
			//dwStatus = RES_SERVER_ERROR;
			//dwStatus = RES_QUERY_ERROR;
			notifyResMan(RN_UPLOAD_ERROR);
			//spResourceNotify->onResourceNotify( this, RN_UPLOAD_ERROR);
		}
	}*/
	HANDLE hThreadCopy = hUploadThread;
	hUploadThread = NULL;

	CheckAndDestroyIfNeeded(&m_canDestroyFromUpload);

	CloseHandle( hThreadCopy);
	
	return 0;
}

int CRemoteResource::uploadRes()
{
	// в цикле читаем и пишем
	bool successFlag = true;
	DWORD dwSize = 0;
	pSrcResource->getSize( &dwSize);
	FILETIME oTime1;
	std::wstring sErrorText;
	int iErrorCode = 0;
	getTime(oTime1);
	pResMan->addRemoteResourceInfo(sFullResPath, dwSize, oTime1);
	ILogWriter *pLW = pResMan->GetLogWriter();
	if(pLW!=NULL)
		pLW->WriteLnLPCSTR("[UPLOAD] uploading is start");
	for(;;){
		//#define MLRES_BUFSIZE 32768
#define MLRES_BUFSIZE 65536
		unsigned char pBuffer[MLRES_BUFSIZE + 1];
		DWORD dwReadSize = 0;
		pSrcResource->seek( dwWriteCount, RES_BEGIN);
		if( pSrcResource->read(pBuffer, MLRES_BUFSIZE, &dwReadSize) != S_OK){
			// установить состояние ошибки и закончить
			// ??
			//DWORD dwStatus__;
			//pSrcResource->getStatus( &dwStatus__);
			pSrcResource->getStatus( &dwStatus);
			sErrorText = L"error reading local file for uploading";
			iErrorCode = 100;
			successFlag = false;
			break;
		}
		if ( dwReadSize == 0)
			break;
		DWORD dwWroteSize = 0;
		int tryCount = 0;
		while(bEnableUploadThread)
		{
			iErrorCode = write( pBuffer, dwReadSize, &dwWroteSize, dwSize);
			if ( iErrorCode != 0)
			{
				successFlag = false;
				mutexClose.lock();
				if(pResMan != NULL)
					pLW = pResMan->GetLogWriter();
				mutexClose.unlock();
				if(pLW!=NULL)
					pLW->WriteLn("[UPLOAD] sFullResPath", sFullResPath.GetBuffer(), "  errorCode = ", iErrorCode);
				Sleep(3000);
				tryCount++;
				if(tryCount > 3)
					break;
				continue;
			}			
			tryCount = 0;
			successFlag = true;
			break;
		}
		if( !successFlag && iErrorCode > 0)
			break;
		if( dwWriteCount + dwWroteSize >= dwSize)
		{
			successFlag = true;
			if((dwStatus & RES_FILE_OPERATING_AT_SERVER) == RES_FILE_OPERATING_AT_SERVER)
				dwWriteCount = dwSize - 1;
			else
				dwWriteCount += dwWroteSize;
			break;
		}
		else
		{
			dwWriteCount += dwWroteSize;
			notifyResMan(RN_ASYNCH_UPLOADED);
		}

		if( !bEnableUploadThread)
			break;
		// ?? если записалось не все, то вообще-то желательно повторить операцию записи
	}
	if( !bEnableUploadThread)
		return successFlag;
	mutexClose.lock();
	if( pResMan != NULL)
		pLW = pResMan->GetLogWriter();
	mutexClose.unlock();
	if(pLW!=NULL)
		pLW->WriteLnLPCSTR("[UPLOAD] uploading is ended");
	if((dwStatus & RES_FILE_OPERATING_AT_SERVER) == RES_FILE_OPERATING_AT_SERVER)
	{
		if(pLW!=NULL)
			pLW->WriteLnLPCSTR("[UPLOAD] converting is start");		
		notifyResMan(RN_ASYNCH_UPLOADED);
		byte btStatusServerCode = 0;		
		while( pResMan != NULL && (dwStatus & RES_FILE_OPERATING_AT_SERVER) == RES_FILE_OPERATING_AT_SERVER)
		{
			Sleep(5000);
			long hr = getUploadInfo(btStatusServerCode, sErrorText);
			if(!bEnableUploadThread || hr == E_FAIL || RES_IS_ERROR(dwStatus))
			{
				successFlag = false;
				break;
			}
		}
		mutexClose.lock();
		if( pResMan != NULL)
			pLW = pResMan->GetLogWriter();
		mutexClose.unlock();
		if(pLW!=NULL)
		{
			CComString s;
			s.Format("[UPLOAD] result of convert successFlag {0}", (successFlag ? "success" : "failed"));
			pLW->WriteLnLPCSTR( s.GetBuffer());
		}
		if(successFlag)
		{
			dwWriteCount =  dwSize;
			notifyResMan( RN_ASYNCH_UPLOADED, 0, sErrorText.size()>0 ? (wchar_t*)sErrorText.c_str() : NULL);
		}
		else
		{
			notifyResMan( RN_OPERATE_ERROR, btStatusServerCode, sErrorText.size()>0 ? (wchar_t*)sErrorText.c_str() : NULL);			
		}
	}
	else
	{
		if(successFlag)
			notifyResMan(RN_ASYNCH_UPLOADED);
		else
			notifyResMan(RN_UPLOAD_ERROR, iErrorCode, (wchar_t*)sErrorText.c_str());
	}
	mutexClose.lock();
	if(pSrcResource != NULL)
	{
		pSrcResource->destroy();
		pSrcResource = NULL;
	}
	mutexClose.unlock();
	onUploadEnd();
	return successFlag;
}

void CRemoteResource::onUploadEnd()
{	
}

long CRemoteResource::registerAtResServer( IResourceNotify* aNotify)
{
	spResourceNotify = aNotify;
//	DWORD dwThreadID;

	CSimpleThreadStarter registerThreadStarter(__FUNCTION__);	
	registerThreadStarter.SetRoutine(registerAtResServerThread_);
	registerThreadStarter.SetParameter(this);
	hRegisterAtResServerThread = registerThreadStarter.Start();

	return S_OK;
}

DWORD WINAPI CRemoteResource::registerAtResServerThread_(LPVOID param)
{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();
	CRemoteResource* aThis = (CRemoteResource*)param;
	return aThis->registerAtResServerThread();
}

int CRemoteResource::registerAtResServerThread()
{
	//bEnableUploadThread = true;
	dwStatus |= RES_ASYNCH;
	if((dwStatus & RES_EXISTS) != RES_EXISTS)
		dwStatus |= RES_EXISTS;
	dwStatus &= ~RES_FILE_CACHE;
	/*if((dwStatus & RES_UPLOAD) != RES_UPLOAD)
		dwStatus |= RES_UPLOAD;*/
	//dwStatusUploader = dwStatus;
	//getTime(oTime);
	//pResMan->addRemoteResourceInfo(sFullResPath, dwSize, oTime);
	std::wstring sErrorText = L"not registered file at res server";
	if ( registerFileAtResServer() == S_OK)
	{
		notifyResMan( RN_ASYNCH_REGISTERED);
	}
	else
	{
		int iErrorCode = dwStatus;
		notifyResMan( RN_REGISTERED_ERROR, iErrorCode, (wchar_t*)sErrorText.c_str());
	}
	dwStatus &= ~RES_ASYNCH;	
	CloseHandle( hRegisterAtResServerThread);
	hUploadThread = NULL;
	return 0;
}

int CRemoteResource::registerFileAtResServer()
{
	pResMan->registerRemoteResource( sFullResPath.GetBuffer(), dwStatus);
	if ( dwStatus == RES_REMOTE)
	{
		return S_OK;		
	}	
	return E_FAIL;
}