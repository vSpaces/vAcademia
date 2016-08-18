// ResMan.cpp: implementation of the CResMan class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "comman.h"
#include "ResMan.h"
#include "RemoteResource.h"
#include "FileResource.h"
#include "BigFileResource.h"
#include "ResModule.h"
#include "InfoQueryMan.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "ReadRemoteResourceQueryOut.h"
#include "ReadRemoteResourceIn.h"
#include "FileInfoQueryOut.h"
#include "MultiFileInfoQueryOut.h"
#include "RootFileInfoQueryOut.h"
#include "WriteRemoteResourceQueryOut.h"
#include "WriteRemoteResourceIn.h"
#include "FileInfoIn.h"
#include "UploadFileInfoIn.h"
#include "UploadFileInfoQueryOut.h"
#include "PackUpdateClientFilesAndGetInfoIn.h"
#include "PackUpdateClientFilesAndGetInfoQueryOut.h"
#include "RootFileInfo2QueryOut.h"
#include "PackClientSetupIn.h"
#include "WriteUpdaterLogQueryOut.h"
#include "RegisterRemoteResourceInfoIn.h"
#include "RegisterRemoteResourceQueryOut.h"

using namespace ResManProtocol;

const wchar_t* sManifest = L"<?xml version=\"1.0\" encoding=\"win-1251\"?>\n<lom>\n\t<general>\n\t\t<identifier>\n\t\t\t<entry>%s</entry>\n\t\t\t<catalog>x-oms-module</catalog>\n\t\t</identifier>\n\t\t<language>ru</language>\n\t</general>\n</lom>\n";
const wchar_t* sTechdata = L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<module>\n</module>\n";

wchar_t* predefinedLocalModules[] = {L"ui"
	, L"vu.common"
	, L"vu.test"
	, L"vu.tests"};

void correctPath( CWComString& aPath)
{
	bool bAddFirstSlash = (aPath.GetLength()>1 && aPath[0] == L'\\' && aPath[1] == L'\\');
	while (aPath.Replace( L'/', L'\\') != 0);
	while (aPath.Replace( L"\\\\", L"\\") != 0);
	if(bAddFirstSlash) aPath=L"\\"+aPath;
	aPath.MakeLower();
}

void correctRemotePath( CWComString& aPath)
{
	while (aPath.Replace( L'\\', L'/') != 0);
	while (aPath.Replace( L"//", L"/") != 0);
	aPath.MakeLower();
	aPath.Replace( L"http:/", L"http://");
	aPath.Replace( L"ftp:/", L"ftp://");
}

void correctCachePath( CWComString& sCacheResPath)
{
	sCacheResPath.Replace( L"/", L"\\");
	while (sCacheResPath.Replace( L"\\\\", L"\\") != 0);
//	sCacheResPath.Replace( ":", "_");
}

void assemblePath( CWComString& aResultPath, const wchar_t* aPath, const wchar_t* aBase)
{
	if (aPath[1] == L':' || (aPath[0] == L'\\' && aPath[1] == L'\\'))
		aResultPath = aPath;
	else
		aResultPath = CWComString( aBase) + aPath;
	correctPath( aResultPath);
}

LPWSTR (*lpGetRefineResourcePath)( LPWSTR sPath) = NULL;

HMODULE loadDLL( const wchar_t* alpcName)
{
	CWComString sModuleDirectory;
	GetModuleFileNameW( NULL, sModuleDirectory.GetBufferSetLength(MAX_PATH2), MAX_PATH2);

	CWComString sCurrentDirectory;
	GetCurrentDirectoryW( MAX_PATH2, sCurrentDirectory.GetBufferSetLength(MAX_PATH2));

	HMODULE handle = NULL;


	//SECFileSystem fs;
	//SetCurrentDirectory( fs.GetPath(sModuleDirectory) + alpcRelPath);
	sModuleDirectory.Replace(L"\\", L"/");
	sModuleDirectory.Replace(L"//", L"/");
	int pos = sModuleDirectory.ReverseFind(L'/');
	if(pos>-1)
	{
		sModuleDirectory = sModuleDirectory.Left(pos);
	}

	CWComString sPath = sModuleDirectory + L"/" + alpcName;
	sPath.Replace(L"\\", L"/");
	sPath.Replace(L"//", L"/");
	handle = ::LoadLibraryW( sPath);

	if (!handle)
	{
		sPath = sCurrentDirectory + L"/" + alpcName;
		sPath.Replace(L"\\", L"/");
		sPath.Replace(L"//", L"/");

		handle = ::LoadLibraryW( sPath);
	}
	
	SetCurrentDirectoryW( sCurrentDirectory);
	return handle;
}

//////////////////////////////////////////////////////////////////////
// CResMan
//CResMan::CResMan( IUnknown* aOwner, CHTTP* aHTTP, ILogWriter* lw) :
	//pOwner( aOwner), pHTTP( aHTTP), m_pLW( lw), wCacheMask( 0), dwMaxCacheSize( 100000000),
CResMan::CResMan( CHTTP* aHTTP, ILogWriter* lw) :
	pHTTP( aHTTP), m_pLW( lw), wCacheMask( 0), dwMaxCacheSize( 100000000),
	pRepository( NULL), MP_VECTOR_INIT( vcOpenResources), MP_MAP_INIT( mapModuleRequestStatuses),
	MP_MAP_INIT( mapModuleRequestEvents)
{	
	pContext = NULL;
	bConnectionLosted = 0;
	pResSocket = NULL;
	iTransferType = METHOD_TRANS_SOCKET;
	wCacheMask = RES_REMOTE;
	
	InitializeCriticalSection(&csNotify);
	InitializeCriticalSection(&csModulesRequest);

	ULARGE_INTEGER freeBytesAvailable;
	ULARGE_INTEGER totalNumberOfBytes;
	ULARGE_INTEGER totalNumberOfFreeBytes;

	GetDiskFreeSpaceExW(sCacheBase, &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes);

	__int64 cacheSize = (__int64)(totalNumberOfBytes.QuadPart * 0.15);
	if (cacheSize > MAX_CACHE_SIZE)
	{
		dwMaxCacheSize = MAX_CACHE_SIZE;
	}
	else
	{
		dwMaxCacheSize = (DWORD)cacheSize;
	}
	pInfoQueryMan = MP_NEW( CInfoQueryMan);
	hPathRefinerModule = NULL;
	CreatePathRefinerImpl();
	resSessionHandler.SetResMan(this);
	uploadSessionHandler.SetResMan(this);
	hBigFileModule = NULL;

	createRepository(L"root\\files\\");
	dwMainThreadID = GetCurrentThreadId();
}
//
CResMan::~CResMan()
{
	MP_DELETE( pInfoQueryMan);
	clearTemporary();
	if (pRepository != NULL)
		MP_DELETE( pRepository);
	DeleteCriticalSection(&csNotify);
	DeleteCriticalSection(&csModulesRequest);
	if(hPathRefinerModule)
		::FreeLibrary( (HMODULE)hPathRefinerModule);
	hPathRefinerModule = NULL;
	DestroyBigFileDll();
}

bool CResMan::isLocalModule( const wchar_t* aName)
{
	//CWComString s = aName;
	//s.MakeLower();
	for (UINT pn=0; pn<sizeof(predefinedLocalModules)/sizeof(predefinedLocalModules[0]); pn++)
	{
		if( wcscmp(aName, predefinedLocalModules[pn]) == 0)
		//if( s.Find( predefinedLocalModules[pn]) > -1)
		{
			return true;
		}		
	}	
	return false;
};

void CResMan::LoadBigFileDll()
{
	
#ifdef DEBUG
#define	BIGFILEMAN_MANAGER_DLL L"BigFileMan.dll"
#else
#define	BIGFILEMAN_MANAGER_DLL L"BigFileMan.dll"
#endif

	if ( !hBigFileModule)
		hBigFileModule = loadDLL( BIGFILEMAN_MANAGER_DLL);
}

int CResMan::CreateBigFileManagerReaderImpl( bigfile::IBigFileManagerReader **pBigFileMan, const wchar_t* aModuleFile)
{
	int errorCode = -1;
	if ( hBigFileModule)
	{
		unsigned int (*lpCreateBigFileManager)( bigfile::IBigFileManagerReader **apBigFileMan, const wchar_t* asModuleFile);
		(FARPROC&)lpCreateBigFileManager = (FARPROC) GetProcAddress( (HMODULE) hBigFileModule, "CreateBigFileManagerReader");
		if ( lpCreateBigFileManager)
			errorCode = lpCreateBigFileManager( pBigFileMan, aModuleFile);
	}
	return errorCode;
}

void CResMan::DestroyBigFileDll()
{
	ATLASSERT( pContext);
	if( !pContext)
		return;

	if( !hBigFileModule)
		return;

	FreeLibrary( (HMODULE)hBigFileModule);
	hBigFileModule = NULL;
}

void CResMan::DestroyBigFileManagerImpl( bigfile::IBigFileManagerReader *apBigFileMan, const wchar_t* aModuleFile)
{
	ATLASSERT( pContext);
	if( !pContext)
		return;

	if( !hBigFileModule)
		return;

	void (*lpDestroyBigFileManager)( bigfile::IBigFileManagerBase *apBigFileMan, const wchar_t* asModuleFile);
	(FARPROC&)lpDestroyBigFileManager = (FARPROC)GetProcAddress( (HMODULE)hBigFileModule, "DestroyBigFileManager");
	if( lpDestroyBigFileManager)
		lpDestroyBigFileManager( ( bigfile::IBigFileManager *)apBigFileMan, aModuleFile);
}

void CResMan::clearInfoList()
{
	pInfoQueryMan->clearResInfoList();
}

void CResMan::setContext(oms::omsContext *apContext)
{
	pContext = apContext;
}

void CResMan::setResSocket( CClientSession* aResSocket)
{
	ATLASSERT( aResSocket);
	if( aResSocket)
	{
		pResSocket = aResSocket;
		pResSocket->SetMessageHandler( &resSessionHandler);
		pResSocket->SetSessionListener( this);
	}
}

void CResMan::setResXMLSocket( CClientSession* aResXMLSocket)
{
	ATLASSERT( aResXMLSocket);
	if( aResXMLSocket)
	{
		pResXMLSocket = aResXMLSocket;
		pResXMLSocket->SetMessageHandler( &resSessionHandler);
		pResXMLSocket->SetSessionListener( this);
	}
}

void CResMan::setResUploadSocket( CClientSession* aResUploadSocket)
{
	ATLASSERT( aResUploadSocket);
	if( aResUploadSocket)
	{
		pResUploadSocket = aResUploadSocket;
		pResUploadSocket->SetMessageHandler( &uploadSessionHandler);
	}
}

void CResMan::OnConnectLost()
{
	if(m_pLW!=NULL)
		m_pLW->WriteLnLPCSTR( "[RESMAN]: The connect with ResServer was lost");
	if( bConnectionLosted < 2)
		bConnectionLosted++;
	if( bConnectionLosted == 1 && pContext != NULL && pContext->mpSM != NULL)
		pContext->mpSM->OnServerDisconnected(rmml::ML_SERVER_MASK_RES, NULL);
}

void CResMan::OnConnectRestored()
{
	if( bConnectionLosted > 0)
		bConnectionLosted--;
	if(m_pLW!=NULL)
		m_pLW->WriteLnLPCSTR( "[RESMAN]: The connect with ResServer was restored");
	if( bConnectionLosted == 0 && pContext != NULL && pContext->mpSM != NULL)
		pContext->mpSM->OnServerConnected(rmml::ML_SERVER_MASK_RES, true);
}

bool CResMan::refineResourceURL(CWComString &sPath)
{
	if( !lpGetRefineResourcePath)
		return false;
	if(sPath.Find(L"#")>-1)
	{
		assert(lpGetRefineResourcePath!=NULL);
		LPWSTR s = lpGetRefineResourcePath(sPath.GetBuffer());
		sPath = s;
		::SysFreeString( s);
		return true;
	}
	return false;
}

void CResMan::correctPointPath( CWComString &as)
{
	int pos = as.Find( L"/../", 0);
	while (pos>-1) 
	{
		CWComString s = as.Left( pos);
		int pos2 = s.ReverseFind( L'/');
		s = s.Left( pos2);
		as = s + as.Right( as.GetLength() - pos - 3);
		pos = as.Find( L"/../", 0);
	};
}

void CResMan::refineResourceURLAndGetResourceInfo( CWComString& sResPath, CWComString& pResRefinePath, CResInfo& oResInfo, const wchar_t* aResPath, DWORD aFlags, bool abCheckOnlyExistInList)
{
	if(refineResourceURL(sResPath))
	{
		pResRefinePath = sResPath;
		getResourceInfo( sResPath.GetBuffer(), aFlags, &oResInfo, true, NULL, NULL, abCheckOnlyExistInList);
		if(RES_IS_ERROR(oResInfo.dwStatus) || oResInfo.dwSize==0 || oResInfo.dwSize==0xFFFFFFFF)
		{
			sResPath = aResPath;
			oResInfo.sResPath = aResPath;
			oResInfo.dwStatus = aFlags;
		}
	}
	else
		getResourceInfo( sResPath.GetBuffer(), aFlags, &oResInfo, true, NULL, NULL, abCheckOnlyExistInList);
}

long CResMan::openResource( const wchar_t* aResPath, DWORD aFlags, IResource** aRes, IResourceNotify *aNotify)
{
	*aRes = NULL;
	CWComString sResPath( aResPath);

	/*CComString s = "";
	if( sResPath.Find(L"bt_login")>-1 || sResPath.Find(L"translations")>-1)
	{
		s =  "11;";
	}*/

	if (aResPath[0] != L'\\' && aResPath[1] != L'\\')
	{
		correctRemotePath( sResPath);
	}
	correctPointPath(sResPath);	
	CResInfo oResInfo;

	CWComString pResRefinePath = L"";

	if ((aFlags & RES_ANYWHERE) == 0)
		aFlags |= RES_ANYWHERE;
	if (RES_IS_ERROR( aFlags))
	{
		oResInfo.dwStatus = (aFlags & ~RES_ANYWHERE);
	}
	else
	{
		refineResourceURLAndGetResourceInfo( sResPath, pResRefinePath, oResInfo, aResPath, aFlags);
	}
		//
	if (oResInfo.bDirectory)
	{
		oResInfo.dwStatus = RES_PATH_ERROR;
	}
	DWORD dwResStatus = (aFlags & ~(RES_ANYWHERE | RES_FILE_CACHE)) | oResInfo.dwStatus;
	if ((oResInfo.dwStatus & (RES_LOCAL)) != 0)
	{
		CFileResource *pRes = MP_NEW( CFileResource);
		pRes->setManager( this);
		pRes->open( sResPath.GetBuffer(), dwResStatus, aNotify, pResRefinePath.GetBuffer());
		*aRes = (IResource*)(pRes);
	}
	else if ((oResInfo.dwStatus & RES_REMOTE) != 0)
	{
		CRemoteResource *pRes = MP_NEW( CRemoteResource);
		pRes->setManager( this);
		pRes->open( sResPath.GetBuffer(), dwResStatus, aNotify, pResRefinePath.GetBuffer());
		*aRes = (IResource*)(pRes);
	}
	else
	{
		CResourceImpl *pRes = MP_NEW( CResourceImpl);
		pRes->setManager( this);
		pRes->open( sResPath.GetBuffer(), dwResStatus, aNotify, pResRefinePath.GetBuffer());
		*aRes = (IResource*)(pRes);
	}
	return S_OK;
}

void CResMan::setCacheBase( const wchar_t* aBasePath)
{ 
	sCacheBase = aBasePath;
	correctPath( sCacheBase);
	makeDir( sCacheBase);
}

void CResMan::setLocalBase( const wchar_t* aBasePath)
{ 
	sLocalBase = aBasePath;
	correctPath( sLocalBase);
	makeDir( sLocalBase);
}

DWORD CResMan::resourceExists( const wchar_t* aResPath, DWORD aFlags, bool abCheckOnlyExistInList)
{
	CResInfo oResInfo;
	if ((aFlags & RES_ANYWHERE) == 0)
		aFlags |= RES_ANYWHERE;
	if (RES_IS_ERROR( aFlags))
	{
		/*if (m_pLW != NULL)
		{
			m_pLW->WriteLnLPCSTR(CLogValue("Test exist local res status RES_IS_ERROR").GetData());
		}*/
		oResInfo.dwStatus = (aFlags & ~RES_ANYWHERE);
	}
	else
	{
		//getResourceInfo( aResPath, aFlags, &oResInfo, true);
		CWComString sResPath = aResPath;
		int pos = sResPath.Find(L"#", 0);
		if(pos>-1)
			sResPath = sResPath.Left(pos);
		getResourceInfo( sResPath.GetBuffer(), aFlags, &oResInfo, true, NULL, NULL, abCheckOnlyExistInList);
		//refineResourceURLAndGetResourceInfo( sResPath, pResRefinePath, oResInfo, aResPath, aFlags, abCheckOnlyExistInList);
	}
	return oResInfo.dwStatus;
}

bool CResMan::getLocalInfo(const wchar_t* aBasePath, const wchar_t* aResPath, 
						   DWORD aType, CResInfo* aResInfo, bool aFullInfo)
{
	aResInfo->dwStatus = RES_PATH_ERROR;
	assemblePath( aResInfo->sResPath, aResPath, aBasePath);
	DWORD dwAttr;
	if ((dwAttr = GetFileAttributesW( aResInfo->sResPath)) != 0xFFFFFFFF)
	{
		aResInfo->dwStatus = aType;
		aResInfo->bDirectory = (dwAttr == FILE_ATTRIBUTE_DIRECTORY);
		if (aFullInfo)
		{
			HANDLE hResFile = CreateFileW( aResInfo->sResPath, 0,
				FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			if (hResFile != INVALID_HANDLE_VALUE)
			{
				aResInfo->dwSize = GetFileSize( hResFile, NULL);
				GetFileTime( hResFile, NULL, NULL, &aResInfo->oTime);
				CloseHandle( hResFile);
#if LOG_LOCAL_RESOURCE
				if (m_pLW != NULL)
				{
					m_pLW->WriteLnLPCSTR(CLogValue("success open local res for get info= ", aResInfo->sResPath.GetBuffer()).GetData());
				}
#endif
			}
			else
			{
#if LOG_LOCAL_RESOURCE_ERROR
				if (m_pLW != NULL)
				{
					m_pLW->WriteLnLPCSTR(CLogValue("[WARNING] not open local res for get info= ", aResInfo->sResPath.GetBuffer(), " error code==", GetLastError()).GetData());
				}
#endif
			}			
		}
	}
	/*int ii = GetLastError();
	if (m_pLW != NULL && ii > 0)
	{
		m_pLW->WriteLnLPCSTR(CLogValue("Not exist local res= ", aResInfo->sResPath.GetBuffer(), " error code==", ii).GetData());
	}*/
	return aResInfo->dwStatus == aType;
}

/*void CResMan::clearResItemInfo( const wchar_t* aResPath)
{
	CResInfo* aResInfo = new CResInfo();
	aResInfo->sResPath = sRemoteBase + aResPath;
	correctRemotePath( aResInfo->sResPath);
	aResInfo->sResPath.MakeLower();
	pInfoQueryMan->clearInvalidResItemInfo( aResInfo->sResPath);
}*/

#define MAX_WAIT_TIMEOUT 30000

void CResMan::getResourceInfo(const wchar_t* aResPath, DWORD aFlags,
							  CResInfo* aResInfo, bool aFullInfo, BYTE* aFirstBlock, DWORD* aFirstBlockSize, bool abCheckOnlyExistInList)
{
	aResInfo->dwStatus = RES_PATH_ERROR;
	aResInfo->dwSize = 0xFFFFFFFF;
	aResInfo->bDirectory = false;
	aResInfo->oTime.dwHighDateTime = 0;
	aResInfo->oTime.dwLowDateTime = 0;

	if( sRemoteBase.IsEmpty())
	{
		aFlags &= ~RES_REMOTE;
	}

	if ((aFlags & (RES_TO_WRITE | RES_CREATE)	) && (aFlags & RES_LOCAL))
	{
		getLocalInfo( sLocalBase, aResPath, RES_LOCAL, aResInfo, aFullInfo);
		aResInfo->dwStatus = RES_LOCAL | RES_TO_WRITE;
		return;
	}
	if ((aFlags & RES_LOCAL) != 0 &&
		getLocalInfo( sLocalBase, aResPath, RES_LOCAL, aResInfo, aFullInfo))
		return;
	if ((aFlags & RES_REMOTE) != 0 &&
		aResInfo->dwStatus == RES_PATH_ERROR)
	{
		aResInfo->dwStatus = RES_REMOTE | (aFlags & (RES_MEM_CACHE | RES_FILE_CACHE | RES_TO_WRITE | RES_UPLOAD));
		
		if(wcsstr(aResPath, L"http://")!=NULL || wcsstr(aResPath, L"ftp://")!=NULL)
		{
			aResInfo->sResPath = aResPath;
		}
		else
		{
			aResInfo->sResPath = sRemoteBase + aResPath;
		}
		correctRemotePath( aResInfo->sResPath);
		aResInfo->sResPath.MakeLower();
		if (aFullInfo) 
		{
			if (pInfoQueryMan->getResInfo( aResInfo->sResPath, aResInfo, NULL, NULL, abCheckOnlyExistInList))
			{
				aResInfo->dwStatus = (aFlags & ~RES_ANYWHERE) | aResInfo->dwStatus;
				return;
			}

			BYTE* pData = NULL;
			DWORD dwDataSize = 0;
			CWComString sParam;
			EQueryError iResult = qeNoError;
			bool bCacheInfoSet = pInfoQueryMan->getCacheInfoSetting();

			if (aFlags & RES_CREATE)	
			{
				if ( iTransferType == METHOD_TRANS_PHP )
				{
					return;
				}
				else if ( iTransferType == METHOD_TRANS_SOCKET )
				{
					// так как ресурс не существует и нам требуется создать ресурс, поэтому ставим
					// что ресурс существует и не отправляем запрос на сервер, просто выходим из функции
					aResInfo->dwStatus |= RES_EXISTS | RES_UPLOAD;					
					return;
				}
			  
			}
			else
			{
				if ( iTransferType == METHOD_TRANS_PHP )
				{
					// сюда не приходит и файла такого на сервере нет
					sParam.Format( L"file=%S", aResInfo->sResPath.GetBuffer());
					iResult = pHTTP->sendQueryData(
						CComBSTR( "file_info2.php"), CComBSTR( sParam), &dwDataSize, &pData);
				}
				else if ( iTransferType == METHOD_TRANS_SOCKET )
				{				
					//CWComString tempStr = aResInfo->sResPath;
					if( !bCacheInfoSet && aResInfo->sResPath.Find(sUserBase)==-1 && aResInfo->sResPath.Find( pInfoQueryMan->getRepositoryPathClientUpdate())==-1)
					{
						if (m_pLW != NULL)
							m_pLW->WriteLn("Send request to RES_ROOT_MULTI_INFO2");

						DWORD startQueryTime = timeGetTime();
						CRootFileInfo2QueryOut queryOut;
						iResult = (EQueryError) pResSocket->SendAndReceive( RES_ROOT_MULTI_INFO2, queryOut.GetData(), queryOut.GetDataSize(), &pData, &dwDataSize, 2*MAX_WAIT_TIMEOUT);

						if (m_pLW != NULL)
						{
							m_pLW->WriteLn("RES_ROOT_MULTI_INFO2 request result is: ", ((int)iResult), ", bCacheInfoSet = ", bCacheInfoSet);
						}

						if (iResult == qeNoError)
						{
							pResSocket->SetDetailedLogModeEnabled(false);
							//pResSocket->SetDetailedLogModeEnabled(true);
							DWORD endQueryTime = timeGetTime();
							DWORD averageBytesPS = 0xFFFFFFFF;
							DWORD averageKBitesPS = 0xFFFFFFFF;
							if( endQueryTime > startQueryTime)
							{
								averageBytesPS = dwDataSize / (endQueryTime - startQueryTime);
								averageKBitesPS = (averageBytesPS << 3);
								averageBytesPS <<= 10;
								
								if (m_pLW != NULL)
									m_pLW->WriteLn("Average res socket speed is: ", averageBytesPS, " bytes per second AKA ", averageKBitesPS, " kbites/s");
							}

							m_initialSpeed = averageBytesPS;
						}
					}
					else
					{
						if (m_pLW != NULL)
							m_pLW->WriteLn("Send request to RES_FILE_MULTI_INFO");

						CMultiFileInfoQueryOut queryOut( aResInfo->sResPath.GetBuffer());
						iResult = (EQueryError) pResSocket->SendAndReceive( RES_FILE_MULTI_INFO, queryOut.GetData(), queryOut.GetDataSize(), &pData, &dwDataSize, MAX_WAIT_TIMEOUT);

						if (m_pLW != NULL)
						{
							m_pLW->WriteLn("RES_FILE_MULTI_INFO request result is: ", ((int)iResult), ", bCacheInfoSet = ", bCacheInfoSet);
						}
					}
				}
			}

			BYTE btError = 0;
			if (iResult == qeNoError)
			{				
				if (pData != NULL && dwDataSize > 0)
				{
					CDataBuffer2 *dataBuffer = MP_NEW( CDataBuffer2);
					if ( iTransferType == METHOD_TRANS_PHP)
					{
						dataBuffer->attachData( pData, dwDataSize);
						pInfoQueryMan->analyseResInfo(aResPath, aFlags, dataBuffer, aResInfo, aFirstBlock, aFirstBlockSize);
					}
					else if ( iTransferType == METHOD_TRANS_SOCKET)
					{

						if( !bCacheInfoSet && aResInfo->sResPath.Find(sUserBase)==-1)
						{
							if (m_pLW != NULL)
							{
								m_pLW->WriteLn("RES_ROOT_MULTI_INFO2 request result is: ", ((int)iResult));
							}
							AddRootInfoList2( pData, dwDataSize);
							if (!pInfoQueryMan->getResInfo( aResInfo->sResPath, aResInfo, NULL, NULL, (pResSocket->GetConnectionStatus() == 1 ? true : false) ))
							{
								aResInfo->dwSize = 0;
								aResInfo->dwStatus = RES_PATH_ERROR;
							}
							aResInfo->dwStatus = (aFlags & ~RES_ANYWHERE) | aResInfo->dwStatus;
						}
						else
						{
							if (m_pLW != NULL)
							{
								m_pLW->WriteLn("RES_FILE_MULTI_INFO request result is: ", ((int)iResult));
							}
							btError = pInfoQueryMan->analyseMultiResInfo(aResInfo->sResPath, aFlags, pData, dwDataSize, aResInfo);
						}
					}
					MP_DELETE_ARR( pData);
					//GlobalFreePtr( pData);
					pData = NULL;										
					dataBuffer->attachData( NULL, 0);
					MP_DELETE( dataBuffer);
				}
				else
				{
					aResInfo->dwStatus = RES_SCRIPT_ERROR;
					pInfoQueryMan->update( aResInfo);	
					if (m_pLW != NULL)
					{
						CLogValue log("[ERROR] pData==NULL ResMan: not exist resource: ", aResInfo->sResPath.GetBuffer());
						m_pLW->WriteLnLPCSTR(log.GetData());
						btError = 0;
					}
				}
				if (m_pLW != NULL && btError!=0)
				{
					switch(btError)
					{
						case 1:
						{
							CLogValue log("[ERROR] ResMan: not exist resource: ", aResInfo->sResPath.GetBuffer(), "  error = FILE_RES_NOT_EXIST");
							m_pLW->WriteLnLPCSTR(log.GetData());
							break;
						}
						case 2:
						{
							CLogValue log("[ERROR] ResMan: not exist resource: ", aResInfo->sResPath.GetBuffer(), "  error = DIR_RES_EMPTY");
							m_pLW->WriteLnLPCSTR(log.GetData());
							break;
						}
						case 3:
						{
							CLogValue log("[ERROR] ResMan: not exist resource: ", aResInfo->sResPath.GetBuffer(), "  error = DIR_RES_NOT_EXIST");
							m_pLW->WriteLnLPCSTR(log.GetData());
							break;
						}
						case 4:
						{
							CLogValue log("[ERROR] Info resource already sended: ", aResInfo->sResPath.GetBuffer(), "  error = DIRINFO_ALREADY_SENDED");
							m_pLW->WriteLnLPCSTR(log.GetData());
							break;
						}
						case 5:
						{
							CLogValue log("[ERROR] ResMan: not exist resource: ", aResInfo->sResPath.GetBuffer(), "  error = RES_PATH_ERROR");
							m_pLW->WriteLnLPCSTR(log.GetData());
							break;
						}
						case 6:
						{
							CLogValue log("[ERROR] ResMan: not exist resource: ", aResInfo->sResPath.GetBuffer(), "  error = RES_SCRIPT_ERROR");
							m_pLW->WriteLnLPCSTR(log.GetData());
							break;
						}
						default:
						{
							CLogValue log("[ERROR] btError>6 ResMan: not exist resource: ", aResInfo->sResPath.GetBuffer());
							m_pLW->WriteLnLPCSTR(log.GetData());
							aResInfo->dwStatus = RES_SCRIPT_ERROR;
							break;
						}
					}
				}
			}
			else if (iResult == qeScriptError)
				aResInfo->dwStatus = RES_SCRIPT_ERROR;
			else if (iResult == qeQueryNotFound)
				aResInfo->dwStatus = RES_QUERY_ERROR;
			else
				aResInfo->dwStatus = RES_SERVER_ERROR;
			if(m_pLW)
			{
				if(iResult == qeTimeOut)
				{
					CLogValue log("[ERROR] Server error: timeout - ", aResInfo->sResPath.GetBuffer());
					aResInfo->dwStatus = RES_SERVER_ERROR_TIMEOUT;
					m_pLW->WriteLnLPCSTR(log.GetData());
				}
				else if(iResult == qeServerError)
				{
					if(bConnectionLosted)
					{
						CLogValue log("[ERROR] Server error: not connected, waiting restore - ", aResInfo->sResPath.GetBuffer());
						aResInfo->dwStatus |= RES_SERVER_ERROR_TIMEOUT;
						m_pLW->WriteLnLPCSTR(log.GetData());
					}
					else
					{
						aResInfo->dwStatus |= RES_SERVER_ERROR_NOTCONNECT;
						CLogValue log("[ERROR] Server error: not connected - ", aResInfo->sResPath.GetBuffer());
						m_pLW->WriteLnLPCSTR(log.GetData());
					}
				}
				else if(iResult == qeReadError)
				{
					CLogValue log("[ERROR] Cannot read remote data: ", aResInfo->sResPath.GetBuffer());
					m_pLW->WriteLnLPCSTR(log.GetData());
				}
				else if(iResult != qeNoError)
				{
					CLogValue log("[ERROR]Unknown ResMan Error ", aResInfo->sResPath.GetBuffer(), ". Error code == ", iResult);
					m_pLW->WriteLnLPCSTR(log.GetData());
				}
			}
		}
	}
}

/*EQueryError CResMan::openResourceQuery( const wchar_t* aResPath, CHttpQuery*& aQuery)
{
	CComBSTR bsParams( "file=");
	bsParams += aResPath;
	if ( iTransferType == METHOD_TRANS_PHP )
	{
		return pHTTP->openQuery( CComBSTR( "file_get.php"), bsParams, aQuery);
	}
	else
	if ( iTransferType == METHOD_TRANS_SOCKET )
	{
		//return pHTTP->openQuery( CComBSTR( "file_get.php"), bsParams, aQuery);
		CWComString tempStr = aResPath;
		//tempStr.MakeLower();
		tempStr.Replace("\\","/");
		BYTE *aData = NULL;
		DWORD *aDataSize = NULL;
		CFileInfoQueryOut queryOut((unsigned char*)tempStr.GetString());
		//EQueryError queryError = (EQueryError) pResSocket->SendAndReceive( RES_FILE_INFO, frame->getBytes(), frame->getSize(), &aData, aDataSize, MAX_WAIT_TIMEOUT);
		EQueryError queryError = (EQueryError) pResSocket->SendAndReceive( RES_FILE_INFO, queryOut.GetData(), queryOut.GetDataSize(), &aData, aDataSize, MAX_WAIT_TIMEOUT);
		//delete frame;
		if (queryError == qeNoError)
		{
			CFileInfoIn queryIn(aData, *aDataSize);
			if(!queryIn.Analyse()){
				// ??? вопрос какой код ошибки возвращать ???
				if(m_pLW!=NULL)
					m_pLW->WriteLnLPCSTR("[ANALYSE FALSE] CFileInfoQueryOut");
				ATLASSERT( FALSE);
				return qeReadError;
			}
			// раньше данные наверх не передавались ??? Нужно ли передавать ???
			//
		}
		return queryError;
	}
	return qeNotImplError;
}*/

bool CResMan::readRemoteResource(const wchar_t* aResPath, DWORD aPos, DWORD aCount,
								 DWORD *aDataSize, BYTE **aData,
								 DWORD *aStatus, DWORD *aFileSize, DWORD *aError)
{	
#if LOG_REMOTE_RESOURCE
	if(m_pLW != NULL)
	{
		m_pLW->WriteLn("[READ] readRemoteResource path=", aResPath);
	}
#endif	
	DWORD dwThreadID = GetCurrentThreadId();
	if( dwMainThreadID == dwThreadID && aResPath != NULL && !wcsstr( aResPath, L"scene.xml"))
	{
		if(m_pLW != NULL)
			m_pLW->WriteLn("ERROR readRemoteResource in Main Thread", aResPath);
		//assert(dwMainThreadID != dwThreadID);
	}	
	bool bRes = false;
	*aData = NULL;
	*aDataSize = 0;
	*aStatus = RES_PATH_ERROR;
	EQueryError iQueryRes;
	bool bError;
	int iTrying = 3;
	do {

		bError = false;
		iTrying--;

		BYTE* pData = NULL;
		DWORD dwDataSize = 0;
		while (true)
		{
			int nQueryID;

			// Запрашиваем с сервера блок данных из файла
			if (iTransferType == METHOD_TRANS_PHP)
			{
				CWComString sParam;
				sParam.Format( L"file=%S&pos=%u&count=%u", aResPath, aPos, aCount);
				iQueryRes = pHTTP->sendQueryData(
					CComBSTR( "file_read.php"), CComBSTR( sParam), &dwDataSize, &pData);
			}
			else if ( iTransferType == METHOD_TRANS_SOCKET )
			{			
				CWComString tempStr = aResPath;
				tempStr.Replace(L"\\",L"/");
				if(tempStr.Find(L"http://", 0)==0 || tempStr.Find(L"ftp://", 0)==0)
				{
					/*if(m_pLW != NULL)
						m_pLW->WriteLn("[READING HTTP RESOURCE] name = ", tempStr.GetBuffer());*/
					iQueryRes = pHTTP->sendQueryData(tempStr.AllocSysString(), L"", &dwDataSize, &pData, qtReadFile, NULL, 0, false);
					if (iQueryRes != qeNoError)
					{
						if(m_pLW != NULL)
							m_pLW->WriteLn("[ERROR READ HTTP RESOURCE] name = ", tempStr.GetBuffer());
						*aDataSize = *aFileSize = 0;
						bError = true;
						break;
					}
					*aStatus = RES_REMOTE | (*aStatus & RES_MEM_CACHE);
					*aDataSize = *aFileSize = dwDataSize;
					//*aData = (BYTE*)GlobalAlloc( GPTR, *aDataSize);
					*aData = MP_NEW_ARR( BYTE, *aDataSize);
					rtl_memcpy( *aData, *aDataSize, pData, *aDataSize);
					/*if(m_pLW != NULL)
					{
						m_pLW->WriteLn("[READED HTTP RESOURCE] size = ", (int)dwDataSize);
					}*/
					bRes = true;
					break;
				}
				CReadRemoteResourceQueryOut queryOut( tempStr.GetString(), aPos, aCount);
				if(tempStr.Find(L".xml", 0)>-1)
				{
					iQueryRes = (EQueryError) pResXMLSocket->SendAndReceive( 
								RES_FILE_READ, queryOut.GetData(), queryOut.GetDataSize(),
								&pData, &dwDataSize, MAX_WAIT_TIMEOUT, &nQueryID);
				}
				else
				{
					iQueryRes = (EQueryError) pResSocket->SendAndReceive( 
								RES_FILE_READ, queryOut.GetData(), queryOut.GetDataSize(),
								&pData, &dwDataSize, MAX_WAIT_TIMEOUT, &nQueryID);
				}
			}
			else
			{
				if(m_pLW != NULL)
					m_pLW->WriteLn("[ERROR QUERY ID] name = ", aResPath);
				iQueryRes = qeNotImplError;
				break;
			}

			// Если есть ошибки, не продолжаем
			if (iQueryRes != qeNoError)
			{
				if( iQueryRes == qeTimeOut /*|| iQueryRes == qeServerError*/)
				{
					*aError = qeTimeOut;
				}
				else
					if(m_pLW != NULL)
						m_pLW->WriteLn("[ERROR READ RESOURCE] name = ", aResPath);
				break;
			}

			// Создадим объект разбора пришедших данных
			CReadRemoteResourceIn queryIn(pData, dwDataSize);

			// Если данные некорректны, не продолжаем
			if(!queryIn.Analyse())
			{
				if(m_pLW!=NULL)
					m_pLW->WriteLnLPCSTR("[ANALYSE FALSE] CReadRemoteResourceIn");
				iQueryRes = qeReadError;
				*aError = qeReadError;
				ATLASSERT( FALSE);
				break;
			}

			bRes = true;
			byte btExist = queryIn.IsExist();
			if (btExist == 0)
			{
				if(m_pLW != NULL)
				{
					m_pLW->WriteLn("[ERROR READ] aResPath=", aResPath, "  is not exist in resServer");
				}
				*aStatus = RES_PATH_ERROR;
				if( wcsstr(aResPath, sUserBase) != NULL)
					*aError = qeReadError;
				else
					*aError = qeNotExistFileReadError;
				break;
			}

			*aStatus = RES_REMOTE | (*aStatus & RES_MEM_CACHE);
			*aFileSize = queryIn.GetResourceSize();
			*aDataSize = queryIn.GetResourceDataSize();
			if (*aFileSize==0xFFFFFFFF || *aDataSize == 0xFFFFFFFF || *aDataSize>*aFileSize)
			{
				CLogValue log("[ERROR] error size in packet for: ", aResPath, " dwFileSize=", (int)*aFileSize, " dwDataSize=", (int)*aDataSize);
				m_pLW->WriteLnLPCSTR(log.GetData());
			}
			if (*aFileSize==0 && *aDataSize == 0)
			{
				CLogValue log("[ERROR] error size in packet for: ", aResPath, " dwFileSize=", (int)*aFileSize, " dwDataSize=", (int)*aDataSize);
				m_pLW->WriteLnLPCSTR(log.GetData());
				iQueryRes = qeReadError;
				bError = true;
				break;
				//int nn = 0;
			}
			DWORD dwCount = (aPos >= *aFileSize ? 0 :
				(aPos + aCount < *aFileSize ? aCount : *aFileSize - aPos));			
			if (*aDataSize < dwCount)
			{
				CLogValue log("[ERROR] error size in packet for: ", aResPath, "  aDataSize=", (unsigned int)*aDataSize, " dwCount=", (unsigned int)dwCount);
				m_pLW->WriteLnLPCSTR(log.GetData());
				iQueryRes = qeReadError;
				bError = true;
				break;
			}
			//*aData = (BYTE*)GlobalAlloc( GPTR, *aDataSize);
			*aData = MP_NEW_ARR( BYTE, *aDataSize);
			rtl_memcpy( *aData, *aDataSize, queryIn.GetResourceData(), *aDataSize);
			break;
		}

		// Освобождаем память, выделенную под пришедшие данные
		if (pData != NULL)
		{
			dwDataSize = 0;
			MP_DELETE_ARR( pData);
			pData = NULL;
		}	
	} while (iTrying > 0 && bError);
	
	if (iQueryRes == qeQueryNotFound)
		*aStatus = RES_QUERY_ERROR;
	else if (iQueryRes == qeScriptError)
		*aStatus = RES_SCRIPT_ERROR;
	else if (iQueryRes == qeReadError)
		*aStatus = RES_READ_ERROR;
	else	if( iQueryRes != qeNoError)
		*aStatus = RES_SERVER_ERROR;

	if (iQueryRes != qeNoError)
		bRes = false;

	/*if( iQueryRes == qeServerError)
		*aError = qeTimeOut;*/

	return bRes;
}

int CResMan::writeRemoteResource(const wchar_t* aResPath, BYTE *aData, DWORD dwPos, DWORD aSizeToWrite, DWORD* aWrote, DWORD aTotalSize, DWORD *aStatus)
{
	//bool bRes = false;
	BYTE* pData = NULL;
	DWORD dwDataSize = 0;
	EQueryError iQueryRes;
	*aWrote = 0;

	bool bError = false;
	int iTrying = 3;
	do {
		iTrying--;
		CWriteRemoteResourceQueryOut queryOut( CComString(aResPath), dwPos, aData, aSizeToWrite, aTotalSize);
		iQueryRes = (EQueryError) pResUploadSocket->SendAndReceive( RES_FILE_WRITE, queryOut.GetData(), queryOut.GetDataSize(), &pData, &dwDataSize, MAX_WAIT_TIMEOUT);	

		// Если есть ошибки, то не продолжаем
		if ( iQueryRes != qeNoError)
			break;

		CWriteRemoteResourceIn queryIn(pData, dwDataSize);
		// Если данные некорректны, то не продолжаем
		if ( !queryIn.Analyse())
		{
			if(m_pLW!=NULL)
				m_pLW->WriteLnLPCSTR("[ANALYSE FALSE] CWriteRemoteResourceIn");
			iQueryRes = qeReadError;
			ATLASSERT( FALSE);
			break;
		}

		//int packetID = queryIn.GetPacketID();
		int errorID = queryIn.GetErrorID();
		// 1 - ошибка записи в файл
		// 2 - ощибка создания в файл
		if ( errorID == 0)
		{
			*aWrote = queryIn.GetWriteBytesCount();
			bError = false;
		}
		else
		{
			bError = true;
		}
		byte bUnpack = queryIn.GetUnpackStatus();
		if(bUnpack==1)
			*aStatus |= RES_FILE_OPERATING_AT_SERVER;
		else if(bUnpack==2)
		{
			*aStatus &= ~RES_FILE_OPERATING_AT_SERVER;
		}
		// Освобождаем память, выделенную под пришедшие данные
		if (pData != NULL)
		{
			dwDataSize = 0;
			MP_DELETE_ARR( pData);
			pData = NULL;
		}	
		
	} while (iTrying > 0 && bError);

	if ( iQueryRes == qeQueryNotFound)
		*aStatus |= RES_QUERY_ERROR;
	else if ( iQueryRes == qeScriptError)
		*aStatus |= RES_SCRIPT_ERROR;
	else if ( iQueryRes == qeReadError)
		*aStatus |= RES_WRITE_ERROR;
	else if( iQueryRes != qeNoError)
		*aStatus |= RES_SERVER_ERROR;

	return iQueryRes;
}

int CResMan::getUploadRemoteResource(const wchar_t* aResPath, DWORD *aStatus, byte *aStatusCode, std::wstring &aErrorText)
{
	EQueryError iQueryRes;
	BYTE* pData = NULL;
	DWORD dwDataSize = 0;

	bool bError = false;
	int iTrying = 3;
	do {
		iTrying--;
		CUploadFileInfoQueryOut queryOut( aResPath);
		iQueryRes = (EQueryError) pResUploadSocket->SendAndReceive( RES_FILE_INFO, queryOut.GetData(), queryOut.GetDataSize(), &pData, &dwDataSize, MAX_WAIT_TIMEOUT);	

		// Если есть ошибки, то не продолжаем
		if ( iQueryRes != qeNoError)
			break;

		CUploadFileInfoIn queryIn(pData, dwDataSize);
		// Если данные некорректны, то не продолжаем
		if ( !queryIn.Analyse())
		{
			if(m_pLW!=NULL)
				m_pLW->WriteLnLPCSTR("[ANALYSE FALSE] CUploadFileInfoIn");
			iQueryRes = qeReadError;
			ATLASSERT( FALSE);
			break;
		}

		*aStatusCode = queryIn.GetStatus();
		if(*aStatusCode == 1)
			*aStatus |= RES_FILE_OPERATING_AT_SERVER;
		else if(*aStatusCode==0)
		{
			*aStatus &= ~RES_FILE_OPERATING_AT_SERVER;
		}
		else if(*aStatusCode>1)
		{
			*aStatus |= RES_SERVER_ERROR;			
		}
		aErrorText = queryIn.GetErrorText();
		// Освобождаем память, выделенную под пришедшие данные
		if (pData != NULL)
		{
			dwDataSize = 0;
			MP_DELETE_ARR( pData);
			pData = NULL;
		}	

	} while (iTrying > 0 && bError);

	if ( iQueryRes == qeQueryNotFound)
		*aStatus |= RES_QUERY_ERROR;
	else if ( iQueryRes == qeScriptError)
		*aStatus |= RES_SCRIPT_ERROR;
	else if ( iQueryRes == qeReadError)
		*aStatus |= RES_WRITE_ERROR;
	else if( iQueryRes != qeNoError)
		*aStatus |= RES_SERVER_ERROR;

	return iQueryRes;
}

int CResMan::packClientSetup(wchar_t *apBufferForFileName, unsigned int auBufferWideCharsCount, unsigned int *aFileSize, int *aStatus)
{
	EQueryError iQueryRes;
	BYTE* pData = NULL;
	DWORD dwDataSize = 0;

	bool bError = false;
	int iTrying = 3;
	do {
		iTrying--;
		//CPackClientSetupQueryOut queryOut( aVersion);
		iQueryRes = (EQueryError) pResSocket->SendAndReceive( PACK_CLIENT_SETUP, NULL, 0, &pData, &dwDataSize, MAX_WAIT_TIMEOUT);	


		// Если есть ошибки, то не продолжаем
		if ( iQueryRes != qeNoError)
			break;

		CPackClientSetupIn queryIn(pData, dwDataSize);
		// Если данные некорректны, то не продолжаем
		if ( !queryIn.Analyse())
		{
			if(m_pLW!=NULL)
				m_pLW->WriteLnLPCSTR("[ANALYSE FALSE] CPackClientSetupIn");
			iQueryRes = qeReadError;
			ATLASSERT( FALSE);
			break;
		}

		int status = queryIn.GetStatus();
		if(status == 1)
		{
			CResInfoSaver *resInfoSaver = MP_NEW( CResInfoSaver);
			resInfoSaver->info.dwStatus = RES_REMOTE;
			queryIn.GetInfo( resInfoSaver);
			*aFileSize = resInfoSaver->info.dwSize;
			pInfoQueryMan->updateFileAndDir( resInfoSaver);

			CWComString sPath = resInfoSaver->info.sResPath;
			int pos = sPath.Find(sRemoteBase, 0);
			if(pos == 0)
				sPath = sPath.Right( sPath.GetLength() - sRemoteBase.GetLength());

			rtl_wcscpy(apBufferForFileName, auBufferWideCharsCount, sPath.GetBuffer());
			*aStatus = status;
		}
		else
		{
			*aStatus = status;
		}

		// Освобождаем память, выделенную под пришедшие данные
		if (pData != NULL)
		{
			dwDataSize = 0;
			MP_DELETE_ARR( pData);
			pData = NULL;
		}	

	} while (iTrying > 0 && bError);

	if( iQueryRes != qeNoError)
	{
		*aStatus = -1*iQueryRes;
	}

	return iQueryRes;
}

int CResMan::packUpdateClientFilesAndGetInfo(const wchar_t *aVersion, wchar_t *apBufferForFileName, unsigned int auBufferWideCharsCount, unsigned int *aFileSize, int *aStatus)
{
	EQueryError iQueryRes;
	BYTE* pData = NULL;
	DWORD dwDataSize = 0;

	bool bError = false;
	int iTrying = 3;
	do {
		iTrying--;
 		CPackUpdateClientFilesAndGetInfoQueryOut queryOut( aVersion);
		iQueryRes = (EQueryError) pResSocket->SendAndReceive( PACK_UPDATE_CLIENT_FILES_AND_INFO, queryOut.GetData(), queryOut.GetDataSize(), &pData, &dwDataSize, 10000);

		bError = (iQueryRes != qeNoError);
		// Если есть ошибки, то не продолжаем
		if ( !bError)
		{
			CPackUpdateClientFilesAndGetInfoIn queryIn(pData, dwDataSize);
			// Если данные некорректны, то не продолжаем
			if ( !queryIn.Analyse())
			{
				if(m_pLW!=NULL)
					m_pLW->WriteLnLPCSTR("[ANALYSE FALSE] CPackUpdateClientFilesAndGetInfoQueryIn");
				iQueryRes = qeReadError;
				ATLASSERT( FALSE);
				break;
			}

			int status = queryIn.GetStatus();
			if(status == 1)
			{
				CResInfoSaver *resInfoSaver = MP_NEW( CResInfoSaver);
				resInfoSaver->info.dwStatus = RES_REMOTE;
				queryIn.GetInfo( resInfoSaver);
				*aFileSize = resInfoSaver->info.dwSize;
				pInfoQueryMan->updateFileAndDir( resInfoSaver);

				CWComString sPath = resInfoSaver->info.sResPath;
				int pos = sPath.Find(sRemoteBase, 0);
				if(pos == 0)
					sPath = sPath.Right( sPath.GetLength() - sRemoteBase.GetLength());
				
				rtl_wcscpy(apBufferForFileName, auBufferWideCharsCount, sPath.GetBuffer());
				*aStatus = status;
			}
			else
			{
				*aStatus = status;
			}
			
			// Освобождаем память, выделенную под пришедшие данные
			if (pData != NULL)
			{
				dwDataSize = 0;
				MP_DELETE_ARR( pData);
				pData = NULL;
			}	
		}

	} while (iTrying > 0 && bError);

	if( iQueryRes != qeNoError)
	{
		*aStatus = -1*iQueryRes;
	}

	return iQueryRes;
}

void CResMan::writeUpdaterLog(const char *logValue)
{
	EQueryError iQueryRes;
	BYTE* pData = NULL;
	DWORD dwDataSize = 0;

	int iTrying = 3;
	do {
		iTrying--;
		CWriteUpdaterLogQueryOut queryOut( logValue);
		iQueryRes = (EQueryError) pResSocket->SendAndReceive( WRITE_UPDATER_LOG, queryOut.GetData(), queryOut.GetDataSize(), &pData, &dwDataSize, MAX_WAIT_TIMEOUT);	

		// Если есть ошибки, то не продолжаем
		if ( iQueryRes == qeNoError)
			break;

	} while (iTrying > 0);
}

void CResMan::registerRemoteResource( const wchar_t *asFullResPath, DWORD &aStatus)
{
	EQueryError iQueryRes;
	BYTE* pData = NULL;
	DWORD dwDataSize = 0;

	int iTrying = 3;
	do {
		iTrying--;
		CRegisterRemoteResourceQueryOut queryOut( asFullResPath);
		iQueryRes = (EQueryError) pResSocket->SendAndReceive( REGISTER_RESOURCE, queryOut.GetData(), queryOut.GetDataSize(), &pData, &dwDataSize, MAX_WAIT_TIMEOUT);
		CRegisterRemoteResourceInfoIn queryIn(pData, dwDataSize);
		// Если данные некорректны, то не продолжаем
		if ( !queryIn.Analyse())
		{
			if(m_pLW!=NULL)
				m_pLW->WriteLnLPCSTR("[ANALYSE FALSE] CPackUpdateClientFilesAndGetInfoQueryIn");
			aStatus = qeScriptError;
			ATLASSERT( FALSE);
			break;
		}

		int status = queryIn.GetStatus();
		if(status == 1)
		{
			CResInfoSaver *resInfoSaver = MP_NEW( CResInfoSaver);
			resInfoSaver->info.dwStatus = RES_REMOTE;
			queryIn.GetInfo( resInfoSaver);			
			pInfoQueryMan->updateFileAndDir( resInfoSaver);		
			aStatus = status;
		}
		else
		{
			if(m_pLW!=NULL)
			{
				CWComString sRes = L"registerRemoteResource resource ";
				sRes += asFullResPath;
				sRes += L" is failed status==";
				wchar_t ff[50];
				_itow_s( status, ff, 50, 10);
				sRes += ff;
				m_pLW->WriteLn( sRes.GetBuffer());
			}
			aStatus = status;
		}


	} while (iTrying > 0);
}

void CResMan::getFullPath(const wchar_t* aResPath, DWORD aFlags, CWComString &asFullPath)
{
	CWComString sFullPath;
	if (aFlags == 0) 
		aFlags = RES_LOCAL;
	if ((aFlags & RES_LOCAL) != 0)
		sFullPath = sLocalBase;
	else if ((aFlags & RES_FILE_CACHE) != 0)
		sFullPath = sCacheBase;
	if ((aFlags & RES_REMOTE) != 0)
		sFullPath = sRemoteBase;
	else
		sFullPath = sLocalBase;
	if (aResPath[0] == 'h' && aResPath[1] == 't' && aResPath[2] == 't' && aResPath[3] == 'p' && aResPath[4] == ':')
	{
		asFullPath = aResPath;
		return;
		//return sFullPath.AllocSysString();
	}
	if (aResPath[1] == ':' || (aResPath[0] == '\\' && aResPath[1] == '\\'))
		sFullPath = aResPath;
	else
		sFullPath = sFullPath + aResPath;
	correctPath( sFullPath);
	if ((aFlags & RES_REMOTE) == RES_REMOTE)
		refineResourceURL(sFullPath);
	asFullPath = sFullPath.GetBuffer();
	//wchar_t *fullPath = MP_NEW_ARR(wchar_t, sFullPath.GetLength());
	/**aFullPath = MP_NEW_ARR(wchar_t, sFullPath.GetLength());
	int size = sFullPath.GetLength()*sizeof(wchar_t);
	wcscpy( aFullPath, sFullPath.GetBuffer());*/
	//rtl_memcpy( fullPath, size, sFullPath.GetBuffer(), size);
	//return fullPath;
	//return sFullPath.AllocSysString();
}

#include "DataBuffer.h"

struct CCacheFileInfo
{
	char sName[ 255];
	DWORD dwSize;
	FILETIME ftTime;
};

struct CCacheInfo
{
	CDataBuffer oData;
	DWORD dwCacheSize;
};

int compareCacheFile( const void *elem1, const void *elem2)
{
	CCacheFileInfo* f1 = (CCacheFileInfo*)elem1;
	CCacheFileInfo* f2 = (CCacheFileInfo*)elem2;
	return CompareFileTime( &f1->ftTime, &f2->ftTime);
}

/*void getCacheInfo( LPTSTR aDirPathBuf, CCacheInfo* aCacheInfo)
{
	aCacheInfo->dwCacheSize = 0;
	WIN32_FIND_DATA oFindData;
	strcat( aDirPathBuf, "*.*");
	HANDLE hFind = FindFirstFile( aDirPathBuf, &oFindData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		CCacheFileInfo oFile;
		do {
			if ((oFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				aCacheInfo->dwCacheSize += oFindData.nFileSizeLow;
				strcpy( oFile.sName, oFindData.cFileName);
				oFile.dwSize = oFindData.nFileSizeLow;
				if (CompareFileTime( &oFindData.ftCreationTime, &oFindData.ftLastAccessTime) >= 0)
					oFile.ftTime = oFindData.ftCreationTime;
				else
					oFile.ftTime = oFindData.ftLastAccessTime;
				aCacheInfo->oData.addData( sizeof( oFile), (BYTE*)&oFile);
			}
		} while (FindNextFile( hFind, &oFindData));
		FindClose( hFind);
	}
	qsort( (BYTE*)aCacheInfo->oData, aCacheInfo->oData.getLength() / sizeof( CCacheFileInfo), sizeof( CCacheFileInfo), compareCacheFile);
}*/

DWORD CResMan::getMaxCacheSize()
{
	return dwMaxCacheSize;
}

void CResMan::setMaxCacheSize(DWORD newVal)
{
	dwMaxCacheSize = newVal;
}

ECacheFileErrors CResMan::openCacheFile(const wchar_t* aResPath, DWORD aFlags, DWORD aResSize, const FILETIME& aResTime,
										HANDLE *aCacheFile, wchar_t* aCacheFilePath, unsigned int auCahceFilePathLength)
{
	if (sCacheBase.IsEmpty() || (aFlags & wCacheMask) == 0)
		return cfeNotCacheable;

	ECacheFileErrors eResult = cfeOpenned;

	CWComString sCacheResPath( aResPath);
	correctCachePath(sCacheResPath);
	if(sCacheResPath.Find(L"http:\\", 0)==0 || sCacheResPath.Find(L"ftp:\\", 0)==0)
	{
		sCacheResPath.Replace(L"p:\\", L"p\\");
	}
	sCacheResPath = sCacheBase + sCacheResPath;
	if (aCacheFilePath != NULL)
		rtl_wcscpy( aCacheFilePath, auCahceFilePathLength, sCacheResPath);
	
	*aCacheFile = CreateFileW( sCacheResPath, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);


	if (*aCacheFile != INVALID_HANDLE_VALUE)
	{
		FILETIME oCacheTimeWrite;
		GetFileTime( *aCacheFile, NULL, NULL, &oCacheTimeWrite);
		DWORD  oCacheSize = 0;
		oCacheSize = GetFileSize( *aCacheFile, NULL);

		if (oCacheSize != aResSize ||
			(aResTime.dwHighDateTime > 0 && aResTime.dwLowDateTime > 0 && CompareFileTime( &oCacheTimeWrite, &aResTime) != 0))
		{
			CloseHandle( *aCacheFile);
			*aCacheFile = INVALID_HANDLE_VALUE;
			if(m_pLW != NULL)
			{
				CLogValue log("File is in cache but obsolete. ThreadID = ", GetCurrentThreadId(), ". Start downloading from server: ", sCacheResPath.GetBuffer());
				m_pLW->WriteLnLPCSTR(log.GetData());
			}
			SetFileAttributesW( sCacheResPath, FILE_ATTRIBUTE_NORMAL);
			if(!DeleteFileW( sCacheResPath))
			{
				if(m_pLW != NULL)
				{
					CLogValue log("[ERROR] Cannot delete cache file: ", sCacheResPath.GetBuffer(), "  errorCode="+GetLastError());
					m_pLW->WriteLnLPCSTR(log.GetData());
				}
			}
		}
	}

	if (*aCacheFile == INVALID_HANDLE_VALUE)
	{
		makeDir( sCacheResPath);
		DeleteFileW( sCacheResPath);
		*aCacheFile = CreateFileW( sCacheResPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (*aCacheFile == INVALID_HANDLE_VALUE)
		{
			eResult = cfeNotCacheable;
			if(m_pLW != NULL)
			{
				CLogValue log("[ERROR] Cannot create cache file: ", sCacheResPath.GetBuffer());
				m_pLW->WriteLnLPCSTR(log.GetData());
			}
		}
		else
			eResult = cfeCreated;
	}
	else
		eResult = cfeOpenned;
	return eResult;
}

bool CResMan::copyCacheToLocal( const wchar_t* aCashPath, const wchar_t* aResPath, CWComString& aLocalFilePath, FILETIME &oTime, DWORD adwFileSize)
{
	if (aLocalFilePath == L"")
		assemblePath( aLocalFilePath, aResPath, sLocalBase);

	CWComString sPath( aLocalFilePath);
	sPath.SetAt( sPath.ReverseFind( L'\\') + 1, 0);
	makeDir( sPath);
	HANDLE hCacheFile = CreateFileW( aLocalFilePath, 0, FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	bool bCopying = true;	
	if ( hCacheFile != INVALID_HANDLE_VALUE)
	{
		FILETIME oCacheTimeWrite;
		GetFileTime( hCacheFile, NULL, NULL, &oCacheTimeWrite);
		DWORD  oCacheSize = 0;
		oCacheSize = GetFileSize( hCacheFile, NULL);

		if (!(oCacheSize != adwFileSize || (oTime.dwHighDateTime > 0 && oTime.dwLowDateTime > 0 && CompareFileTime( &oCacheTimeWrite, &oTime) != 0)))
			bCopying = false;
		::CloseHandle( hCacheFile);		
		hCacheFile = INVALID_HANDLE_VALUE;
	}	
	BOOL bRes = true;
	if(bCopying)
	{		
		bRes = ::CopyFileW( aCashPath, aLocalFilePath, FALSE);
		if(bRes == 0)
		{
			int ff = GetLastError();
			ff+=0;
		}
	}
	return bRes != FALSE;
}

long CResMan::openModule( const wchar_t* aModuleID, DWORD aFlags, IResModule** aModule)
{
	*aModule = NULL;

	CModuleInfo oModuleInfo;

	if ( aModuleID[ 0] == ':')
		aModuleID++;

	if (( aFlags & RES_CREATE) != 0)
	{
		DWORD dwFlags = (aFlags & (RES_REMOTE | RES_LOCAL));
		CWComString sMetadataPath;
		IResource* spMetadata = NULL;
		DWORD dwWroten;
		CWComString sUserModulePath( sUserBase);	
		sUserModulePath += aModuleID;
		sUserModulePath += L"\\";
		sUserModulePath.MakeLower();
		bool bCreated = false;
		
		sMetadataPath = sUserModulePath + L"META-INF\\manifest.xml";
		if ( SUCCEEDED( openResource( sMetadataPath, dwFlags | RES_CREATE | RES_TO_WRITE, &spMetadata)))
		{
			//if (SUCCEEDED( spMetadata->getStatus( &dwStatus)))
			//{
			/*#define MANIFEST_BUFFER_SIZE 2048
			CWComString sMetadataBuf;
			//LoadString( _Module.GetModuleInstance(), IDS_MANIFEST,
			LoadString( NULL, IDS_MANIFEST,
			sMetadataBuf.GetBuffer( MANIFEST_BUFFER_SIZE+1), MANIFEST_BUFFER_SIZE);
			sMetadataBuf.Format( sMetadataBuf, aModuleID);*/
			CWComString sValue = sManifest;
			sValue.Format( sValue, aModuleID);
			if ( SUCCEEDED( spMetadata->write( (BYTE*)sValue.GetString(), sValue.GetLength(), &dwWroten, sValue.GetLength())) && dwWroten > 0)
			{
				bCreated = true;
			}
		}
		else
		{
			CLogValue log("[ERROR] cannot create META-INF\\manifest.xml");
			m_pLW->WriteLnLPCSTR(log.GetData());
		}

		if(bCreated)
		{
			/*CWComString sModuleFile = aModuleID;
			sModuleFile.MakeLower();
			oModuleInfo.sModulePath = sUserModulePath;
			sModuleFile.Replace( "\\", "_");
			sModuleFile.Replace( "/", "_");
			sModuleFile+=".zip";
			if(aFlags & RES_REMOTE){
				//aModuleInfo.sModulePath = arModulePath[ 0];
				oModuleInfo.dwStatus = RES_REMOTE;
				if (aFlags & (RES_CREATE | RES_TO_WRITE))
					oModuleInfo.dwStatus |= RES_TO_WRITE;
			}else{
				oModuleInfo.dwStatus = 0;
			}
			oModuleInfo.sModuleFile = oModuleInfo.sModulePath + sModuleFile;*/
			CResInfoSaver *resInfoSaver = MP_NEW( CResInfoSaver);
			resInfoSaver->info.bDirectory = false;
			spMetadata->getStatus( &resInfoSaver->info.dwStatus);
			resInfoSaver->info.dwStatus |= (RES_REMOTE | RES_FILE_CACHE);
			spMetadata->getSize( &resInfoSaver->info.dwSize);
			spMetadata->getTime( resInfoSaver->info.oTime);
			resInfoSaver->info.sResPath = sRemoteBase + sUserModulePath + L"meta-inf\\manifest.xml";
			pInfoQueryMan->updateFileAndDir( resInfoSaver);
			//oModuleInfo.dwStatus |= (aFlags & ~RES_EXISTS);
		}
		spMetadata->destroy();
		spMetadata = NULL;
		

		sMetadataPath = sUserModulePath + L"META-INF\\tech-data.xml";
		if ( SUCCEEDED( openResource( sMetadataPath, dwFlags | RES_CREATE | RES_TO_WRITE, &spMetadata)))
		{
			//if (SUCCEEDED( spMetadata->getStatus( &dwStatus)))
			//{
			/*#define TECHDATA_BUFFER_SIZE 2048
			CWComString sMetadataBuf;
			//LoadString( _Module.GetModuleInstance(), IDS_TECHDATA,
			LoadString( NULL, IDS_TECHDATA,
			sMetadataBuf.GetBuffer( TECHDATA_BUFFER_SIZE+1), TECHDATA_BUFFER_SIZE);
			sMetadataBuf.Format( sMetadataBuf, aModuleID);*/
			CWComString sValue = sTechdata;
			sValue.Format( sValue, aModuleID);
			/*if ( SUCCEEDED( spMetadata->write( (BYTE*)sValue.GetString(), sValue.GetLength(), &dwWroten, sValue.GetLength())) && dwWroten > 0)
				int ff = 0;*/
			spMetadata->write( (BYTE*)sValue.GetString(), sValue.GetLength(), &dwWroten, sValue.GetLength());
				//clearResItemInfo( sMetadataPath);
			//}
			//spMetadata.Release();
		}
		else
		{
			CLogValue log("[ERROR] cannot create META-INF\\tech-data.xml");
			m_pLW->WriteLnLPCSTR(log.GetData());
		}
		spMetadata->destroy();
		spMetadata = NULL;
	}
	//else
	//if(!bCreated)
	getModuleInfo( aModuleID, aFlags, oModuleInfo);

	CResModule* pResModule = MP_NEW( CResModule);
	pResModule->setManager( this);
	if( oModuleInfo.dwStatus & RES_EXISTS)
	{
		pResModule->open( aModuleID, oModuleInfo.sModulePath, oModuleInfo.sModuleFile, oModuleInfo.dwStatus);
	}
	else
		pResModule->setStatus( oModuleInfo.dwStatus);
	*aModule = (IResModule*)(pResModule);

	return S_OK;
}

long CResMan::openModuleByPath( const wchar_t* aModulePath, IResModule** aModule)
{
	//CComObject< CResModule>* pResModule;
	//CComObject< CResModule>::CreateInstance( &pResModule);
	CResModule *pResModule = MP_NEW( CResModule);
	pResModule->setManager( this);
	pResModule->open( NULL, aModulePath, NULL, RES_LOCAL);
	*aModule = (IResModule*)(pResModule);
	//pResModule->QueryInterface( IID_IResModule, (void**)aModule);

	return S_OK;
}

long CResMan::getInitialSpeed()
{
	return m_initialSpeed;
}

DWORD CResMan::checkModule( const wchar_t* aBase, int aPathCount, const CWComString* sModulePath, DWORD aFlags, CWComString sModuleFile, CWComString &sModuleFullPath)
{
	CWComString sManifestFile;
	CWComString sPackFile;
	DWORD dwStatus = 0;
	for (int i = 0;  i < aPathCount;  i++)
	{
		if( !(aFlags & RES_REMOTE))
		{
			sModuleFullPath = aBase;
			if (sModulePath[ i][1]!=L':' && (sModulePath[ i][0]!=L'\\' || sModulePath[ i][1]!=L'\\'))
				sModuleFullPath += sModulePath[ i];
			else
				sModuleFullPath = sModulePath[ i];
			sPackFile = sModuleFullPath + sModuleFile;
			if ( hBigFileModule != NULL && GetFileAttributesW( sPackFile) != 0xFFFFFFFF)
			{
				dwStatus = aFlags | RES_MODULE_PACK;
				break;
			}
			sManifestFile = sModuleFullPath + L"META-INF\\manifest.xml";
			if (GetFileAttributesW( sManifestFile) != 0xFFFFFFFF)
			{
				dwStatus = aFlags;
				break;
			}
		}
		else
		{
			sModuleFullPath = sModulePath[ i];
			sManifestFile = sModuleFullPath + L"META-INF\\manifest.xml";
			DWORD result = resourceExists(sManifestFile, aFlags);
			if (RES_IS_EXISTS(result))
			{
				dwStatus = aFlags;
				break;
			}
			if((result & RES_SERVER_ERROR_TIMEOUT) == RES_SERVER_ERROR_TIMEOUT)
			{
				dwStatus = result;
				break;
			}
		}

	}
	return dwStatus;
}

void CResMan::getModuleInfo( const wchar_t* aModuleID, DWORD aFlags, CModuleInfo& aModuleInfo)
{
	aModuleInfo.dwStatus = RES_REMOTE;

	if (wcslen( aModuleID) == 0)  return;

	CWComString sModuleFile( aModuleID);

	sModuleFile.Replace( L"\\", L"_");
	sModuleFile.Replace( L"/", L"_");

	#define PATH_TYPES 5
	CWComString arModulePath[ PATH_TYPES];
	arModulePath[ 0] = sRepositoryBase + sModuleFile;
	arModulePath[ 1] = sRepositoryBase + sModuleFile;
	arModulePath[ 1].Replace( L".", L"\\");
	arModulePath[ 2] = sUserBase + sModuleFile;
	arModulePath[ 3]= sUserBase + sModuleFile;
	arModulePath[ 3].Replace( L".", L"\\");
	arModulePath[ 4] = sUIBase + sModuleFile;

	for (int i = 0;  i < PATH_TYPES;  i++)
	{
		if (arModulePath[ i][ arModulePath[ i].GetLength() - 1] != '\\')
			arModulePath[ i] += L'\\';
	}

	sModuleFile += L".pack";
	//aModuleInfo.sModuleFile = sModuleFile;

	CWComString sFullPath = L"";
	if ((aFlags & (RES_LOCAL | RES_CREATE | RES_TO_WRITE)) != 0 && 
		(aModuleInfo.dwStatus = checkModule( sLocalBase, PATH_TYPES, &arModulePath[ 0], RES_LOCAL | RES_TO_WRITE, sModuleFile, sFullPath)) > 0)
	{
		aModuleInfo.sModulePath = sFullPath;
	}
	else if ((aFlags & (RES_LOCAL)) != 0 && 
		(aModuleInfo.dwStatus = checkModule( sLocalBase, PATH_TYPES - 1, arModulePath, RES_LOCAL, sModuleFile, sFullPath)) > 0)
	{
		aModuleInfo.sModulePath = sFullPath;
	}
	else if ((aFlags & (RES_REMOTE)) != 0 && 
		(aModuleInfo.dwStatus = checkModule( sRemoteBase, PATH_TYPES - 2, arModulePath, RES_REMOTE, sModuleFile, sFullPath)) > 0)
	{
		aModuleInfo.sModulePath = sFullPath;
	}
	
	aModuleInfo.sModuleFile = aModuleInfo.sModulePath + sModuleFile;
	aModuleInfo.dwStatus |= (aFlags & ~RES_EXISTS);
}

DWORD CResMan::getModuleExists( const wchar_t* aModuleID, DWORD aFlags, IResModule **ppModule)
{
	CModuleInfo oModuleInfo;
	if ((aFlags & RES_ANYWHERE) == 0)
	{
		aFlags |= (RES_REMOTE | RES_LOCAL | RES_MODULE_READY);
	}

	HANDLE ahRequestEvent = 0;
	MODULE_REQUEST_STATUS aModuleRequestStatus = MRS_UNKNOWN;
	MODULE_REQUEST_KEY moduleKey;

	bool bLocal = isLocalModule( aModuleID);
	if ( bLocal)
	{
		aFlags = (aFlags & ~RES_REMOTE) | RES_LOCAL;
	}
	else
	{
		// Блокируем одновременный запрос существования модуля		
		EnterCriticalSection( &csModulesRequest);
		
		moduleKey.moduleName = std::wstring( aModuleID);
		moduleKey.flags = aFlags;

		MapModuleRequestStatuses::iterator it = mapModuleRequestStatuses.find( moduleKey);
		if( it != mapModuleRequestStatuses.end())
		{
			aModuleRequestStatus = mapModuleRequestStatuses[moduleKey];
			ahRequestEvent = mapModuleRequestEvents[moduleKey];
		}
		else
		{
			mapModuleRequestStatuses[moduleKey] = MRS_REQUESTED;

			HANDLE requestEvent = CreateEvent( NULL, TRUE, FALSE, NULL);
			mapModuleRequestEvents[moduleKey] = requestEvent;
		}

		if( aModuleRequestStatus == MRS_UNKNOWN)
		{
			if( m_pLW)
				m_pLW->WriteLn("[RESMAN] Send getModuleInfo for ", aModuleID, ", aFlags = ", aFlags);
		}

		LeaveCriticalSection( &csModulesRequest);

		if( aModuleRequestStatus == MRS_REQUESTED && (aFlags & RES_REMOTE) == RES_REMOTE)
		{
			if (dwMainThreadID == GetCurrentThreadId())	
			{	
				//assert(false);			
					// для избежания повисаний временная мера
				if( m_pLW)
					m_pLW->WriteLn("[RESMAN] Query in main thread for module ", aModuleID);
				int t1 = GetTickCount();
				CWComString s = L"module load in main thread: "; s += aModuleID;
				MessageBoxW( NULL, s.GetBuffer(), L"!!!!!!!!!!!!", MB_OK);
				WaitForSingleObject( ahRequestEvent, 20000/*INFINITE*/);
				int t2 = GetTickCount();
				if( m_pLW)
					m_pLW->WriteLn("Query hangs for ", t2-t1, " ms");
			}
			else
			{
				WaitForSingleObject( ahRequestEvent, 60000/*INFINITE*/); // waiting max 1 minutes
			}
		}
		else
		{
			// ??
		}
	}

	getModuleInfo( aModuleID, aFlags, oModuleInfo);

	if( !bLocal && aModuleRequestStatus == MRS_UNKNOWN)
	{
		if (RES_IS_EXISTS( oModuleInfo.dwStatus))
		{
			if( m_pLW)
				m_pLW->WriteLn("[RESMAN] getModuleInfo result for ", aModuleID, ", module exists");
		}
		else
		{
			if( m_pLW)
				m_pLW->WriteLn("[RESMAN] getModuleInfo result for ", aModuleID, ", module NOT exists. oModuleInfo.dwStatus = ", oModuleInfo.dwStatus);
		}

		EnterCriticalSection( &csModulesRequest);
		mapModuleRequestStatuses[moduleKey] = MRS_KNOWN;
		SetEvent( mapModuleRequestEvents[moduleKey]);
		LeaveCriticalSection( &csModulesRequest);
	}

	// End of "Блокируем одновременный запрос существования модуля"

	if(( oModuleInfo.dwStatus & RES_MODULE_PACK) != 0)
		aFlags |= RES_MODULE_PACK;
	
	if (( oModuleInfo.dwStatus & RES_EXISTS) != 0 && 
		(aFlags & RES_MODULE_READY) != 0 && !moduleReady( aModuleID, aFlags, ppModule))
		oModuleInfo.dwStatus |= RES_MODULE_NOT_READY;
	return oModuleInfo.dwStatus;
}

class CStrArray : public CSimpleValArray< CWComString> {
};

bool CResMan::moduleReady( const wchar_t* aModuleID, DWORD aFlags, IResModule **ppModule)
{
	CModuleInfo oModuleInfo;
	if ((aFlags & RES_ANYWHERE) == 0)
		aFlags |= RES_LOCAL;
	CStrArray oModuleIDs;
	oModuleIDs.Add( aModuleID);
	bool bReady = true;
	DWORD dwStatus;
	int k = 0;
	while (bReady && k < oModuleIDs.GetSize())
	{
		IResModuleMetadata *spModuleMeta = NULL;
		dwStatus = 0;

		bReady = (SUCCEEDED( openModule( oModuleIDs[ k], aFlags, ppModule)));
		bool bRes = SUCCEEDED( (*ppModule)->getStatus( &dwStatus)) && SUCCEEDED( (*ppModule)->getMetadata( &spModuleMeta));
		if ( bReady && bRes && (dwStatus & RES_EXISTS) != 0 && spModuleMeta != NULL)
		{
			int i = 0;
			CWComString bstrRelModuleID;
			int j = 0;
			while (SUCCEEDED( spModuleMeta->getRelation( i, bstrRelModuleID)) && bstrRelModuleID.GetLength()>0)
			{
				CWComString sRelModuleID( bstrRelModuleID);
				for (j = 0;  j < oModuleIDs.GetSize();  j++)
					if (oModuleIDs[ j] == sRelModuleID)
						break;
				if (j >= oModuleIDs.GetSize())
					oModuleIDs.Add( sRelModuleID);
				i++;
			}
		}
		k++;
	}
	return bReady;
}

bool CResMan::createRepository(const wchar_t* aBase)
{
	sRepositoryBase = aBase;
	if (pRepository != NULL)
		MP_DELETE( pRepository);
	if (aBase[ 0] != 0 && (aBase[ 1] == ':' || (aBase[ 0] == '\\' && aBase[ 1] == '\\')))
	{
		 MP_NEW_V( pRepository, CLocalRepository, aBase);
	}
	else
	{
		 MP_NEW_V( pRepository, CLocalRepository, sLocalBase + aBase);
	}
	pInfoQueryMan->setRepositoryPath( aBase);
	return true;
}

bool CResMan::copyOneResource(const wchar_t* aSrcPath, IResModule *aDestModule, const wchar_t* aDestPath,
							  BYTE* aBuffer, DWORD aBufferSize)
{
	IResource* spSrcResource = NULL;
	IResource* spDestResource = NULL;
	DWORD dwSrcStatus, dwDestStatus;
	if (SUCCEEDED( openResource( aSrcPath, RES_LOCAL, &spSrcResource)) &&
		SUCCEEDED( spSrcResource->getStatus( &dwSrcStatus)) && (dwSrcStatus & RES_EXISTS) != 0 &&
		(aDestModule != NULL &&
			SUCCEEDED( aDestModule->openResource( CComBSTR( aDestPath),
				RES_LOCAL | RES_CREATE | RES_TO_WRITE, &spDestResource)) ||
			SUCCEEDED( openResource( aDestPath,
				RES_LOCAL | RES_CREATE | RES_TO_WRITE, &spDestResource))) &&
		SUCCEEDED( spDestResource->getStatus( &dwDestStatus)) && (dwDestStatus & RES_EXISTS) != 0)
	{
		DWORD dwBytes = 0;
		do {
			if (SUCCEEDED( spSrcResource->read( aBuffer, aBufferSize, &dwBytes)) && dwBytes > 0)
				spDestResource->write( aBuffer, dwBytes, &dwBytes);
		} while (dwBytes == aBufferSize);
		spSrcResource->destroy();
		spDestResource->destroy();
	}
	else
		return false;
	return true;
}

bool CResMan::copyDirResource(const wchar_t* aSrcPath, IResModule *aDestModule, const wchar_t* aDestPath,
							  BYTE* aBuffer, DWORD aBufferSize)
{
	WIN32_FIND_DATAW fdData;
	CWComString sSrcPath( aSrcPath);
	HANDLE hFind = FindFirstFileW( sSrcPath + L"*.*", &fdData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		CWComString sDestPath( aDestPath);
		int iSrcLen = sSrcPath.GetLength();
		int iDestLen = sDestPath.GetLength();
		do {
			sSrcPath += fdData.cFileName;
			sDestPath += fdData.cFileName;
			if ((fdData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
				copyOneResource( sSrcPath, aDestModule, sDestPath, aBuffer, aBufferSize);
			else if (fdData.cFileName[ 0] != L'.')
			{
				sSrcPath += L"\\";
				sDestPath += L"\\";
				copyDirResource( sSrcPath, aDestModule, sDestPath, aBuffer, aBufferSize);
			}
			sSrcPath.SetAt( iSrcLen, 0);
			sDestPath.SetAt( iDestLen, 0);
		} while (FindNextFileW( hFind, &fdData));
		FindClose( hFind);
	}
	return true;
}

long CResMan::copyResource(const wchar_t* aSrcPath, IResModule *aDestModule, const wchar_t* aDestPath, CWComString* aFullPath)
{
	CResInfo oResInfo;
	getResourceInfo( aSrcPath, RES_LOCAL, &oResInfo);
	if ((oResInfo.dwStatus & RES_EXISTS) != 0)
	{
		#define READ_BUFFER_SIZE 128000
		CWComString sDestPath;
		if (aDestModule != NULL || aDestPath[ 1] == ':')
			sDestPath = aDestPath;
		else
			sDestPath.Format( L"%S%S", sTempBase.GetBuffer(), aDestPath);
		correctPath( sDestPath);
		
		BYTE* pBuffer = MP_NEW_ARR(BYTE, READ_BUFFER_SIZE);
		if (oResInfo.bDirectory)
		{
			wchar_t cLast = oResInfo.sResPath[ oResInfo.sResPath.GetLength() - 1];
			if (cLast != L'\\' && cLast != L'/')
				oResInfo.sResPath += L'\\';
			cLast = sDestPath[ sDestPath.GetLength() - 1];
			if (cLast != L'\\' && cLast != L'/')
				sDestPath += L'\\';
			copyDirResource( oResInfo.sResPath, aDestModule, sDestPath, pBuffer, READ_BUFFER_SIZE);
		}
		else
		{
			wchar_t cLast = sDestPath[ sDestPath.GetLength() - 1];
			if (cLast != _T( '\\') && cLast != _T( '/'))
				copyOneResource( oResInfo.sResPath, aDestModule, sDestPath, pBuffer, READ_BUFFER_SIZE);
			else
			{
				const wchar_t* pFileName = wcsrchr( oResInfo.sResPath, L'\\');
				if (pFileName == NULL)
				{
					if ((pFileName = wcsrchr( oResInfo.sResPath, L'/')) == NULL)
						pFileName = ((const wchar_t*)oResInfo.sResPath) - 1;
				}
				sDestPath += (pFileName + 1);
				copyOneResource( oResInfo.sResPath, aDestModule, sDestPath, pBuffer, READ_BUFFER_SIZE);
			}
		}
		if (aFullPath != NULL)
			*aFullPath = sDestPath;
		//GlobalFreePtr( pBuffer);
		MP_DELETE_ARR( pBuffer);
	}
	return S_OK;
}

long CResMan::moveResource(const wchar_t* aSrcPath, const wchar_t* aDestPath, BOOL* aSuccess)
{
	CResInfo oResInfo;
	getResourceInfo( aSrcPath, RES_LOCAL, &oResInfo);
	BOOL bSuccess =FALSE;
	if ((oResInfo.dwStatus & RES_EXISTS) != 0)
	{
		CWComString sDestPath = aDestPath;
		correctPath( sDestPath);
		if (oResInfo.sResPath.GetAt(oResInfo.sResPath.GetLength()-1)=='\\')
			oResInfo.sResPath=oResInfo.sResPath.Left(oResInfo.sResPath.GetLength()-1);

		if (sDestPath.GetAt(sDestPath.GetLength()-1)=='\\')
			sDestPath=sDestPath.Left(sDestPath.GetLength()-1);
		bSuccess = MoveFileW( oResInfo.sResPath, sDestPath);
		//int iErr=GetLastError();
	}
	if (aSuccess != NULL)
		*aSuccess =  bSuccess;
	return S_OK;
}

BOOL deleteDir( const wchar_t* aSrcPath, BOOL aYesAll = TRUE)
{
	BOOL bSuccess = TRUE,bEmpty=TRUE;
	WIN32_FIND_DATAW fdData;
	CWComString sSrcPath( aSrcPath);
	int iSrcLen = sSrcPath.GetLength();
	sSrcPath += L"*.*";
	HANDLE hFind = FindFirstFileW( sSrcPath, &fdData);
	sSrcPath.SetAt( iSrcLen, 0);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			sSrcPath += fdData.cFileName;
			if ((fdData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{	if (aYesAll)
					bSuccess = bSuccess && DeleteFileW( sSrcPath);
				else
				{
					bEmpty=FALSE;
					break;
				}
				
			}
			else if (fdData.cFileName[ 0] != _T( '.'))
			{
				sSrcPath += L"\\";
				if (aYesAll)
					bSuccess = bSuccess && deleteDir( sSrcPath);
				else
				{
					bEmpty=FALSE;
					break;
				}
			}
			sSrcPath.SetAt( iSrcLen, 0);
		} while (FindNextFileW( hFind, &fdData));

		FindClose( hFind);
	}
	sSrcPath.SetAt( sSrcPath.GetLength() - 1, 0);
	bSuccess = bEmpty && bSuccess && RemoveDirectoryW( sSrcPath);
	return bSuccess;
}

long CResMan::removeResource( const wchar_t* aPath, BOOL* aSuccess, BOOL aYesAll)
{
	CResInfo oResInfo;
	getResourceInfo( aPath, RES_LOCAL, &oResInfo);
	if ((oResInfo.dwStatus & RES_EXISTS) != 0)
	{
		BOOL bSuccess;
		if (oResInfo.bDirectory)
			bSuccess = deleteDir( oResInfo.sResPath, aYesAll);
		else
			bSuccess = DeleteFileW( oResInfo.sResPath);
		if (aSuccess != NULL)
			*aSuccess =  bSuccess;
	}
	return S_OK;
}

long CResMan::clearTemporary()
{
	if (!sTempBase.IsEmpty())
		deleteDir( sTempBase);
	return S_OK;
}

long	CResMan::onResourceNotify(IResourceNotify* a_pIResourceNotify, IResource* a_pIResource, DWORD aNotifyCode, DWORD aNotifyServerCode, wchar_t *aErrorText)
{
	if( !a_pIResourceNotify)	return S_OK;
	// Lock		critical section
	EnterCriticalSection(&csNotify);

	if (a_pIResourceNotify != NULL)
		a_pIResourceNotify->onResourceNotify( a_pIResource, aNotifyCode, aNotifyServerCode, aErrorText);

	// Unlock	critical section
	LeaveCriticalSection(&csNotify);
	return S_OK;
}

void CResMan::addRemoteResourceInfo(CWComString sResPath, DWORD dwSize, FILETIME oTime)
{
	CResInfo oResInfo;
	oResInfo.sResPath = sResPath;
	oResInfo.oTime = oTime;
	oResInfo.dwSize = dwSize;
	oResInfo.dwStatus = RES_REMOTE;
	pInfoQueryMan->update(&oResInfo);
}

DWORD CResMan::importModule(const wchar_t* aFileName)
{
	BYTE* pXmlText;
	DWORD dwTextSize = 0;
	DWORD dwRes = unpackToMemory( aFileName, L"META-INF/manifest.xml", &dwTextSize, &pXmlText);
	if (RES_IS_ERROR( dwRes))
		return dwRes;

	CWComString sIdentifier;
	dwRes = getModuleIdFromXML( dwTextSize, pXmlText, sIdentifier);
	//GlobalFreePtr( pXmlText);
	MP_DELETE_ARR( pXmlText);
	if (RES_IS_ERROR( dwRes))
		return dwRes;

	CWComString sModulePath = sLocalBase + sRepositoryBase + sIdentifier + L"\\";
	CWComString sModuleFile = sModulePath + sIdentifier + L".pack";

	#define BUFFER_SIZE 32675
	BYTE arCopyBuffer[ BUFFER_SIZE];
	if (!copyOneResource( aFileName, NULL, sModuleFile, arCopyBuffer, BUFFER_SIZE))
		return RES_MODULE_ERROR;

	dwRes = unpackAllToDisk( sModuleFile, sModulePath);

	return dwRes;
}

DWORD CResMan::unpackToMemory(const wchar_t* aZipFile, const wchar_t* aFileName, DWORD *aDataSize, BYTE **aData)
{
	DWORD dwRes = 0;
	IResource* spZipFile = NULL;
	DWORD dwZipStatus;

	if (FAILED_EX( openResource( aZipFile, RES_LOCAL, &spZipFile), dwRes))
		return dwRes;

	if (FAILED_EX( spZipFile->getStatus( &dwZipStatus), dwRes))
	{
		spZipFile->destroy();
		return dwRes;
	}

	if (RES_IS_ERROR( dwZipStatus))
	{
		spZipFile->destroy();
		return dwZipStatus;
	}

	BOOL bSuccessful;
	if (FAILED_EX( spZipFile->unpackToMemory( CComBSTR( aFileName), &bSuccessful, aDataSize, aData), dwRes))
	{
		spZipFile->destroy();
		return dwRes;
	}
	spZipFile->destroy();
	if (!bSuccessful)
		return RES_UNPACK_ERROR;
	return dwRes;
}

DWORD CResMan::getModuleIdFromXML(DWORD aTextSize, BYTE *aXmlText, CWComString &aModuleID)
{
	xmlDocPtr pXmlManifest = xmlReadMemory( (const char*)aXmlText, aTextSize, "", NULL, XML_PARSE_RECOVER);
	if (pXmlManifest == NULL)
		return RES_METADATA_ERROR;
	
	xmlXPathContextPtr context = xmlXPathNewContext( pXmlManifest);
	if (context == NULL)
	{
		xmlFreeDoc( pXmlManifest);
		return RES_METADATA_ERROR;
	}

	CWComString sXPath( L"/lom/general/identifier[catalog='x-oms-module']/entry");

	int allocSize = sXPath.GetLength() + 1;
	xmlChar* xsXPath = (xmlChar*)xmlMemMalloc( allocSize);
	rtl_memcpy( xsXPath, allocSize, sXPath.GetBuffer(), sXPath.GetLength());
	xsXPath[ sXPath.GetLength()] = 0;

	xmlXPathObjectPtr result;
	if (pXmlManifest->children[0].ns)
	{
		xmlNsPtr aNs[1] = {pXmlManifest->children[0].ns};
		context->namespaces = aNs;
		context->nsNr = 1;
		result = xmlXPathEvalExpression( xsXPath, context);
		context->namespaces = NULL;
		context->nsNr = 0;
	}
	else
		result = xmlXPathEvalExpression( xsXPath, context);
	
	xmlMemFree( xsXPath);
	xmlXPathFreeContext(context);

	if (result == NULL || xmlXPathNodeSetIsEmpty(result->nodesetval) ||
		result->nodesetval->nodeTab[0]->children[0].content == NULL)
		return RES_METADATA_ERROR;

	aModuleID = (const wchar_t*)result->nodesetval->nodeTab[0]->children[0].content;
	return 0;
}

DWORD CResMan::unpackAllToDisk(const wchar_t* aZipFile, const wchar_t* aDestPath)
{
	DWORD dwRes = 0;
	IResource* spZipFile = NULL;
	DWORD dwZipStatus;

	if (FAILED_EX( openResource( aZipFile, RES_LOCAL, &spZipFile), dwRes))
		return dwRes;

	if (FAILED_EX( spZipFile->getStatus( &dwZipStatus), dwRes))
	{
		spZipFile->destroy();
		return dwRes;
	}

	if (RES_IS_ERROR( dwZipStatus))
	{
		spZipFile->destroy();
		return dwZipStatus;
	}

	CWComString sDestPath = aDestPath;
	if (FAILED_EX( spZipFile->unpack( sDestPath.GetBuffer(), TRUE, -1), dwRes))
	{
		spZipFile->destroy();
		return dwRes;
	}
	spZipFile->destroy();
	return dwRes;
}

DWORD	CResMan::getResourceSize( const wchar_t* aResPath, WORD aFlags)
{
	if( !resourceExists( aResPath, aFlags))	return 0;

	DWORD		resSize = 0;
	IResource*	spResource = NULL;
	if( SUCCEEDED( openResource( aResPath, aFlags, &spResource, NULL)))
	{
		if( spResource != NULL)
		{
			DWORD dwStatus;
			spResource->getStatus( &dwStatus);
			if ((dwStatus & RES_EXISTS) != 0)
			{
				spResource->getSize(&resSize);
			}
		}
	}
	if(spResource!=NULL)
	{
		spResource->destroy();
	}
	return resSize;
}

void CResMan::CreatePathRefinerImpl()
{

#ifdef DEBUG
#define	PATHREFINER_MANAGER_DLL	L"PathRefiner.dll"
#else
#define	PATHREFINER_MANAGER_DLL	L"PathRefiner.dll"
#endif

	if( !hPathRefinerModule)
	{
		hPathRefinerModule = loadDLL( PATHREFINER_MANAGER_DLL);
	}

	if( hPathRefinerModule)
	{
		(FARPROC&)lpGetRefineResourcePath = (FARPROC)GetProcAddress( (HMODULE)hPathRefinerModule, "GetRefineResourcePath");
		if( !lpGetRefineResourcePath)
		{
			::FreeLibrary((HMODULE)hPathRefinerModule);
			hPathRefinerModule = 0;
		}
	}
}

ILogWriter* CResMan::GetLogWriter()
{ 
	return m_pLW;
}

void CResMan::SetILogWriter(ILogWriter * lw)
{ 
	m_pLW = lw;
	pInfoQueryMan->setILogWriter(m_pLW);
}

void CResMan::notifyOpen( CResourceImpl* aResoure)
{
	EnterCriticalSection(&csNotify);
	RES_OPENED_NOTIF_DESC desc;
	desc.index = _RES_OPENED_NOTIF_DESC_INDEX++;
	desc.res = aResoure;
	/*if( desc.index == 86)
		int ii=0;
	if( aResoure->sResPath.Find("vu2008-02-20") != -1 
		&& aResoure->sResPath.Find("camera.xml") != -1 )
		int ii=0;*/
	vcOpenResources.push_back( desc);
	LeaveCriticalSection(&csNotify);
}

void CResMan::notifyClose( CResourceImpl* aResoure)
{
	bool found = false;
	EnterCriticalSection(&csNotify);
	std::vector< RES_OPENED_NOTIF_DESC>::iterator theIterator;
	for (theIterator = vcOpenResources.begin();  theIterator != vcOpenResources.end();  theIterator++)
	{
		if ((*theIterator).res == aResoure)
		{
			found = true;
			vcOpenResources.erase( theIterator);
			break;
		}
	}
	LeaveCriticalSection(&csNotify);
}

void	CResMan::writeStillOpened() 
{
	if( !m_pLW)
		return;
	m_pLW->WriteLnLPCSTR("Is still opened list");
	std::vector< RES_OPENED_NOTIF_DESC>::iterator theIterator;
	for (theIterator = vcOpenResources.begin();  theIterator != vcOpenResources.end();  theIterator++)
	{
		int triedIndex = (*theIterator).index;
		CLogValue logIndex("Is still opened index - ", triedIndex);
		m_pLW->WriteLnLPCSTR(logIndex.GetData());

		const wchar_t* lpcFileName = (const wchar_t*)((*theIterator).res->sResPath);
		if( lpcFileName)
		{
			CLogValue log("Is still opened - ", lpcFileName);
			m_pLW->WriteLnLPCSTR(log.GetData());
		}
	}
	m_pLW->WriteLnLPCSTR("End of still opened list");
}

void	CResMan::CloseAllResource() 
{
	EnterCriticalSection(&csNotify);
	std::vector< RES_OPENED_NOTIF_DESC>::iterator theIterator;
	for (theIterator = vcOpenResources.begin();  theIterator != vcOpenResources.end();  theIterator++)
	{
		CResourceImpl *pResImpl = (*theIterator).res;
		(*theIterator).res = NULL;
		pResImpl->destroy();		
	}
	vcOpenResources.clear();
	LeaveCriticalSection(&csNotify);
}

int CResMan::getOpenedResCount()
{
	return vcOpenResources.size();
}

void CResMan::AddFileInfoList(BYTE* aData, int aDataSize)
{
	pInfoQueryMan->analyseAddNewFileInfoList(aData, aDataSize, true);
}

bool CResMan::AddRootInfoList(BYTE* aData, int aDataSize)
{
	bool bRes = pInfoQueryMan->analyseRootFileInfoList(aData, aDataSize, false);
	if( bRes && m_pLW != NULL)
		m_pLW->WriteLnLPCSTR("got notify about cache info from Server");
	return bRes;
}

bool CResMan::AddRootInfoList2(BYTE* aData, int aDataSize)
{
	bool bRes = pInfoQueryMan->analyseRootFileInfoList2(aData, aDataSize);
	if( bRes && m_pLW != NULL)
		m_pLW->WriteLnLPCSTR("got notify about cache info from Server");
	return bRes;
}

void CResMan::makeUserBase()
{
	CWComString sPath;
	if (sUserBase[ 0] == '\\' && sUserBase[ 1] == '\\' || sUserBase[ 1] == ':')
		sPath = sUserBase;
	else
		sPath = sLocalBase + sUserBase;
	correctRemotePath(sUserBase);
	correctPath( sPath);
	makeDir( sPath);
	pInfoQueryMan->setUserPath( sUserBase);
}