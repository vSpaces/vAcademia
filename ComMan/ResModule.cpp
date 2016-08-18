// ResModule.cpp: implementation of the CResModule class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResModule.h"
#include "ResMan.h"
#include "BigFileResource.h"
#include <libxml/xpath.h>
#include "ThreadAffinity.h"
#include "CrashHandlerInThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

class CLibXMLCS
{
friend class CLibXMLGuard;
	CRITICAL_SECTION mcsUsing;
public:
	CLibXMLCS(){
		InitializeCriticalSection(&mcsUsing);
	}
	~CLibXMLCS(){
		DeleteCriticalSection(&mcsUsing);
	}
private:
	void Lock(){
		EnterCriticalSection(&mcsUsing);
	}
	void Unlock(){
		LeaveCriticalSection(&mcsUsing);
	}
} goLibXMLCS;

class CLibXMLGuard
{
public:
	CLibXMLGuard(){
		goLibXMLCS.Lock();
	}
	~CLibXMLGuard(){
		goLibXMLCS.Unlock();
	}
};

//////////////////////////////////////////////////////////////////////
// CResModule
CResModule::CResModule() : 
	dwSize( 0), dwDownloadedSize( 0), hDownloadThread( NULL), bEnableDownloadThread( true), 
	hDownloadMetadataThread( NULL), bEnableDownloadMetadataThread( true), pXmlManifest( NULL),
	dwManifestSize( 0), dwManifestDownloadedSize( 0), pBigFileManagerReader( NULL)
{
	hDownloadEvent = CreateEvent( NULL, TRUE, TRUE, NULL);
	bUserModule = false;
}

CResModule::~CResModule()
{
	close();
}

void CResModule::open( const wchar_t* aModuleID, const wchar_t* aModulePath, const wchar_t* aModuleFile, DWORD aFlags)
{
	sModulePath = aModulePath;
	correctPath( sModulePath);
	if ( sModulePath.GetLength()>0 && sModulePath[ sModulePath.GetLength() - 1] != '\\')
		sModulePath += '\\';
	dwStatus = 0;
	dwSize = 0;
	dwDownloadedSize = 0;
	int errorCode = 0;

	if( (aFlags & RES_MODULE_PACK) == RES_MODULE_PACK)
		errorCode = pResMan->CreateBigFileManagerReaderImpl( &pBigFileManagerReader, aModuleFile);

#if LOG_LOCAL_RESOURCE	
	ILogWriter * lw = pResMan->GetLogWriter();
	if (lw != NULL)
	{
		lw->WriteLn(CLogValue("[WARNING]ResModule::open path = ", aModuleFile, "errorCode=="+errorCode));
	}
#endif
	if( errorCode > -1 && pBigFileManagerReader != NULL)
	{
		if( errorCode == 0)
		{
			unsigned int dwFileSize = 0;
			unsigned int realOffset = 0;
			__int64 time = 0;
			pBigFileManagerReader->OpenFile( "meta-inf/manifest.xml", dwFileSize, realOffset, time);
			if( dwFileSize>0)
			{
				dwStatus = RES_LOCAL | RES_MODULE_PACK | MDL_META;
				pBigFileManagerReader->OpenFile( "meta-inf/tech-data.xml", dwFileSize, realOffset, time);
				if( dwFileSize>0)
					dwStatus |= MDL_MODULE;
			}
		}
		else
			dwStatus = RES_LOCAL | RES_MODULE_PACK | MDL_META | MDL_MODULE;

		if (aModuleID != NULL && aModuleID[ 0] != 0)
			sModuleID = aModuleID;
		else
		{
			CComBSTR bsModuleID;
			if (SUCCEEDED( getIdentifier( &bsModuleID)))
				sModuleID = bsModuleID;
		}
		if (aModuleFile != 0 && aModuleFile[ 0] != 0)
			sModuleFile = aModuleFile;
		{
			sModuleFile = sModulePath;
		}
	}
	else
	{
		CWComString sUserPath = pResMan->getUserBase();
		if( !sUserPath.IsEmpty())
			sUserPath.Replace(L"/", L"\\");
		if( ((aFlags & RES_REMOTE) == RES_REMOTE) && !sUserPath.IsEmpty() && sModulePath.Find( sUserPath) > -1)
		{
			bUserModule = true;
			dwStatus = RES_REMOTE & RES_ANYWHERE | MDL_META | MDL_MODULE;
		}
		else
		{
			DWORD dwMetaStatus = pResMan->resourceExists( sModulePath + L"META-INF\\manifest.xml", RES_LOCAL | RES_REMOTE);
			if (RES_IS_EXISTS( dwMetaStatus))
			{
				if( dwMetaStatus & RES_FILE_CACHE)
					dwMetaStatus = RES_REMOTE;
				dwStatus = (dwMetaStatus & RES_ANYWHERE) | MDL_META;
			
				dwMetaStatus = pResMan->resourceExists( sModulePath + L"META-INF\\tech-data.xml", dwMetaStatus & RES_ANYWHERE);
				if (RES_IS_EXISTS( dwMetaStatus))
					dwStatus |= MDL_MODULE;
			}	
		}

		if (aModuleID != NULL && aModuleID[ 0] != 0)
			sModuleID = aModuleID;
		else
		{
			CComBSTR bsModuleID;
			if (SUCCEEDED( getIdentifier( &bsModuleID)))
				sModuleID = bsModuleID;
		}

		if (aModuleFile != 0 && aModuleFile[ 0] != 0)
			sModuleFile = aModuleFile;
		else
		{
			sModuleFile = sModulePath;
		}

		// Проверяем zip-файл есть
		IResource* spModuleFile = NULL;
		DWORD dwModuleStatus = 0;
		if (SUCCEEDED(pResMan->openResource( sModuleFile, RES_LOCAL, &spModuleFile)) &&
			spModuleFile != NULL && SUCCEEDED( spModuleFile->getStatus( &dwModuleStatus)) && (dwModuleStatus & RES_EXISTS) != 0)
		{
			//  zip-файла модуля есть - берем его размер
			spModuleFile->getSize( &dwSize);
			dwDownloadedSize = dwSize;
		}

		if(spModuleFile!=NULL)
		{
			spModuleFile->destroy();
		}
	}

	if (RES_IS_EXISTS( dwStatus))
	{
		dwStatus |= (aFlags & (~RES_ANYWHERE | RES_REMOTE));
		if ( wcsstr( aModulePath, L"user") == aModulePath)
			dwStatus |= RES_TO_WRITE;
	}

	if (!RES_IS_EXISTS( dwStatus))
		dwStatus |= RES_MODULE_ERROR;
}

bool CResModule::loadMetadata()
{
	if (pXmlManifest == NULL)
	{
		IResource* spManifest = NULL;
		DWORD dwManifestStatus = 0;
		if (FAILED_EX( openResource( L"META-INF\\manifest.xml", dwStatus & RES_EXISTS, &spManifest), dwStatus))
		{
			if(spManifest!=NULL)
				spManifest->destroy();
			return false;
		}
		if (FAILED_EX( spManifest->getStatus( &dwManifestStatus), dwStatus))
		{
			if(spManifest!=NULL)
				spManifest->destroy();
			return false;
		}
		if (!RES_IS_ERROR( dwManifestStatus))
		{
			DWORD dwXmlSize = 0;
			if (FAILED_EX( spManifest->getSize( &dwXmlSize), dwStatus))
			{
				if(spManifest!=NULL)
					spManifest->destroy();
				return false;
			}
			CWComString sUserBase = pResMan->getUserBase();
			if( sUserBase.GetLength()>0)
			{
				sUserBase.Replace(L"/", L"\\");
				if( sModulePath.Find(sUserBase)>-1)
				{
					dwStatus |= RES_UPLOAD;
				}				
			}
			dwManifestSize = dwXmlSize;
			dwManifestDownloadedSize = 0;
			BYTE* pXmlText = MP_NEW_ARR( BYTE, dwXmlSize + 1);
			DWORD pos = 0;
			DWORD aErrorCode = 0;
			
			while (SUCCEEDED_EX( spManifest->read( &pXmlText[ pos], __min( 3072, dwManifestSize - pos), &dwXmlSize, &aErrorCode), dwStatus) && dwXmlSize > 0)
			{
				if (!bEnableDownloadMetadataThread)
				{
					MP_DELETE_ARR( pXmlText);
					if(spManifest!=NULL)
						spManifest->destroy();
					return false;
				}
				pos += dwXmlSize;
				dwManifestDownloadedSize = pos;
			}
			if (!RES_IS_ERROR( dwStatus))
			{
				pXmlText[ pos] = 0;
				CLibXMLGuard oLXG;
				pXmlManifest = xmlReadMemory( (LPCSTR)pXmlText, pos, "", NULL, XML_PARSE_RECOVER);
			}
			MP_DELETE_ARR( pXmlText);
		}
		else
			dwStatus = (dwStatus & ~RES_ERROR) | (dwManifestStatus & RES_ERROR);
		if(spManifest!=NULL)
			spManifest->destroy();
	}
	return pXmlManifest != NULL;
}

void UTF8_to_BSTR( const xmlChar* aUTF8, wchar_t** aBSTR)
{
	//*aBSTR = SysAllocStringLen( NULL, strlen( (LPCSTR)aUTF8));
	int len = strlen( (LPCSTR)aUTF8);
	*aBSTR = MP_NEW_ARR( wchar_t, len + 1);
	wchar_t* bstr = *aBSTR;
	DWORD dwLen = 0;
	for (xmlChar ch = *aUTF8;  ch != '\0';  ch = *(++aUTF8))
	{
		if ((ch & 0x80) == 0 )
			*bstr = ch;
		else
		{
			int chTemp;
			unsigned uLength;
			if ((ch & 0xF0) == 0xF0 ) { 
				uLength = 4; 
				chTemp = 0xF0;
			}
			else if( (ch & 0xE0) == 0xE0 ) { 
				uLength = 3; 
				chTemp = 0xE0; 
			}
			else if( (ch & 0xC0) == 0xC0 ) { 
				uLength = 2; 
				chTemp = 0xC0; 
			}
			else break;
			uLength--;
			*bstr = (ch & ~chTemp) << (uLength * 6);

			ch=*(++aUTF8); uLength--;
			*bstr |= (ch & 0x3F) << (uLength * 6);
			if( uLength ){
				ch=*(++aUTF8); uLength--;
				*bstr |= (ch & 0x3F) << (uLength * 6);
				if( uLength ){
					ch=*(++aUTF8); uLength--;
					*bstr |= (ch & 0x3F);
				}
			}
		}
		bstr++;
		dwLen++;
	}
	*bstr=L'\0';
	//SysReAllocString( aBSTR, *aBSTR);
}

//*************************************************************************************************
/*! Convert string from unicode to UTF-8 */
struct MaskItem
{
	unsigned chMask;
	unsigned uShift;
	unsigned uSize;
	unsigned wcMax;
};
static const MaskItem _utf8string_pMaskItem[] = 
{
  { 0x80,  0*6, 1, 0x7F,   },   // 1 byte sequence
  { 0xC0,  1*6, 2, 0x7FF,  },   // 2 byte sequence
  { 0xE0,  2*6, 3, 0xFFFF, },   // 3 byte sequence
  { 0xF0,  3*6, 4, 0x1FFFFF, }, // 4 byte sequence
};

void BSTR_to_UTF8(
   const wchar_t* aBSTR,  //!< pointer to unicode string that is converterd
   xmlChar** aUTF8        //!< pointer to UTF8 buffer string is converterd to
   )
{
	CLibXMLGuard oLXG;

	xmlChar* pbszMax;
   xmlChar* pbszUTF8;
   unsigned uShift;
   const MaskItem* pMaskItem;
   wchar_t* bs = (wchar_t*)aBSTR;
   DWORD dwLength = wcslen(bs);//SysStringLen( bs);
   BSTR pwszEnd = bs + dwLength;

   *aUTF8 = (xmlChar*)xmlMemMalloc( dwLength * 2);

   pbszUTF8 = *aUTF8;
   pbszMax = *aUTF8 + dwLength * 2;

   while( (*bs != L'\0') && (bs != pwszEnd) && (pbszUTF8 < pbszMax) )
   {
      if( *bs < 0x80 ) 
      {
         *pbszUTF8 = (xmlChar)*bs;
         pbszUTF8++;
      }
      else
      {
         for (pMaskItem = &_utf8string_pMaskItem[1]; pMaskItem->chMask; pMaskItem++ )
         {
            if (*bs < pMaskItem->wcMax ) break;
         }

         // Check size for buffer
         if (pbszUTF8 > (pbszMax - pMaskItem->uSize)) break;

         uShift = pMaskItem->uShift;
         *pbszUTF8 = (xmlChar)(pMaskItem->chMask | (*bs >> uShift));
         pbszUTF8++;
         while (uShift > 0)
         {
            uShift -= 6;
            *pbszUTF8 = (xmlChar)(0x80 | (*bs >> uShift)) & 0xBF;
            pbszUTF8++;
         }
      }
      bs++;
   }

   *pbszUTF8 = '\0';
}

bool CResModule::getMetadataStr( const wchar_t* aXPath, wchar_t** aStr, xmlXPathObjectPtr* aResult, xmlNodePtr aRoot)
{
	bool bRes = loadMetadata();
	if (bRes)
	{
		CLibXMLGuard oLXG;
		xmlXPathContextPtr context;
		xmlXPathObjectPtr result;

		context = xmlXPathNewContext( pXmlManifest);
		if (context != NULL)
		{
			xmlChar* xsXPath;
			BSTR_to_UTF8( aXPath, &xsXPath);
			if (aRoot != NULL)
				context->node = aRoot;
			if (pXmlManifest->children && pXmlManifest->children[0].ns)
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
			bRes = (result != NULL && !xmlXPathNodeSetIsEmpty(result->nodesetval));
			if (bRes && result->nodesetval->nodeTab[0]->children[0].content != NULL)
				UTF8_to_BSTR( result->nodesetval->nodeTab[0]->children[0].content, aStr);
			if (aResult == NULL)
				xmlXPathFreeObject( result);
			else
				*aResult = result;
		}
		
	}
	return bRes;
}

bool CResModule::getMetadataLangStr( const wchar_t* aXPath, wchar_t** aStr, wchar_t** aLang)
{
	bool bRes;
	CComBSTR bsXPath( aXPath);
	bsXPath += CComBSTR( "/string");
	if (*aLang == NULL)
	{
		bRes = (getMetadataStr( bsXPath, aStr) &&
		        getMetadataStr( (bsXPath += CComBSTR( "/@language")), aLang));
	}
	else
	{
		bsXPath += CComBSTR( "[@language='");
		bsXPath += *aLang;
		bsXPath += "']";
		bRes = getMetadataStr( bsXPath, aStr);
	}
	return bRes;
}

bool CResModule::downloadModule()
{
	CWComString sModuleFilePath = sModuleFile;
	
	// открываем файл на сервере
	IResource* spRemoteModule = NULL;
	DWORD dwRemoteStatus = 0;
	if (FAILED( pResMan->openResource( sModuleFilePath, RES_REMOTE, &spRemoteModule)) ||
		FAILED( spRemoteModule->getStatus( &dwRemoteStatus)) || RES_IS_NOT_EXISTS(dwRemoteStatus))
	{
		//spRemoteModule->close();
		spRemoteModule->destroy();
		spRemoteModule = NULL;
		// ошибка: файла на сервере нет
		if(RES_IS_ERROR(dwRemoteStatus))
			dwStatus = dwRemoteStatus;
		else
			dwStatus = RES_MODULE_ERROR;
		return false;
	}
	// если файл локально есть, то его открываем
//	DWORD dwDownloadedSize;
	IResource* spModuleFile = NULL;
	DWORD dwModuleStatus = 0;
	if (SUCCEEDED( pResMan->openResource( sModuleFilePath, RES_LOCAL | RES_TO_WRITE, &spModuleFile)) 
		&& spModuleFile != NULL
		&& SUCCEEDED(spModuleFile->getStatus( &dwModuleStatus))
		&& (!RES_IS_ERROR(dwModuleStatus)) && (dwModuleStatus & RES_LOCAL))
	{
		//, берем его размер,
		spModuleFile->getSize(&dwDownloadedSize);
		// и делаем  spRemoteModule->seek
		spRemoteModule->seek(dwDownloadedSize, 0);
		spModuleFile->seek(dwDownloadedSize, 0);
	}
	else
	{
		// иначе (файла локально нет),
		if (spModuleFile != NULL)
		{
			spModuleFile->destroy();
			spModuleFile = NULL;
			//spModuleFile->close();
			//spModuleFile.Release();
		}
		// создаем 
		if (FAILED(pResMan->openResource( sModuleFilePath, RES_LOCAL | RES_CREATE, &spModuleFile)) 
			|| FAILED(spModuleFile->getStatus( &dwModuleStatus))
			|| RES_IS_NOT_EXISTS(dwModuleStatus))
		{
			// ошибка: создать не получилось
			spModuleFile->destroy();
			if(RES_IS_ERROR(dwModuleStatus))
				dwStatus = dwModuleStatus;
			else
				dwStatus = RES_MODULE_ERROR;
			return false;
		}
		dwDownloadedSize = 0;
	}

	// выкачиваем
	BYTE* pDataBlock = MP_NEW_ARR( BYTE, CACHING_BLOCK);
	DWORD dwReadBlock = 0;
	DWORD dwWritenBlock = 0;
	if (SUCCEEDED_EX( spRemoteModule->getSize( &dwSize), dwStatus))
	{
		while (bEnableDownloadThread && WaitForSingleObject( hDownloadEvent, INFINITE) == WAIT_OBJECT_0 &&
			   SUCCEEDED_EX( spRemoteModule->read( pDataBlock, CACHING_BLOCK, &dwReadBlock), dwStatus) &&
			   dwReadBlock > 0)
		{
			if (FAILED_EX( spModuleFile->write( pDataBlock, dwReadBlock, &dwWritenBlock), dwStatus))
				break;
			dwDownloadedSize += dwReadBlock;
		}
		MP_DELETE_ARR( pDataBlock);
	}

	spRemoteModule->destroy();
	spModuleFile->destroy();

	if (RES_IS_ERROR( dwStatus))
		return false;



	if (FAILED( unpack(TRUE)))
	{
		dwStatus = RES_UNPACK_ERROR;
		return false;
	}
	return true;
}

bool CResModule::downloadMetadata()
{
	CWComString sMetadataFilePath = sModulePath + L"META-INF\\manifest.xml";
	bool bRes = loadMetadata();

	// Если манифест загружен
	if (bRes && bEnableDownloadMetadataThread)
		dwStatus |= MDL_META;
	return bRes;
}

DWORD WINAPI CResModule::downloadThread_(LPVOID param)
{
	CResModule* aThis = (CResModule*)param;
	return aThis->downloadThread();
}

int CResModule::downloadThread()
{
	CoInitialize( NULL);
	bEnableDownloadThread = true;
	dwStatus |= RES_ASYNCH;
	downloadModule();
	dwStatus &= ~RES_ASYNCH;
	CloseHandle( hDownloadThread);
	hDownloadThread = NULL;
	CoUninitialize();
	return 0;
}

DWORD WINAPI CResModule::downloadMetadataThread_(LPVOID param)
{
	CResModule* aThis = (CResModule*)param;
	return aThis->downloadMetadataThread();
}

int CResModule::downloadMetadataThread()
{
	CoInitialize( NULL);
	bEnableDownloadMetadataThread = true;
	dwStatus |= RES_ASYNCH;
	downloadMetadata();
	dwStatus &= ~RES_ASYNCH;
	CloseHandle( hDownloadMetadataThread);
	hDownloadMetadataThread = NULL;
	CoUninitialize();
	return 0;
}

bool CResModule::openModule()
{
	bool bRes = true;// = (loadMetaInf() || (downloadModule() && loadMetaInf()));
	return bRes;
}

//////////////////////////////////////////////////////////////////////
// IResModule
long CResModule::close()
{
	bEnableDownloadThread = false;
	bEnableDownloadMetadataThread = false;
	resumeDownload();
	if(hDownloadThread){
		hDownloadThread = NULL;
	}
	if(hDownloadMetadataThread){
		hDownloadMetadataThread = NULL;
	}
	if (hDownloadEvent != NULL)
	{
		CloseHandle( hDownloadEvent);
		hDownloadEvent = NULL;
	}
	if (pXmlManifest != NULL)
		xmlFreeDoc( pXmlManifest);

	if( pBigFileManagerReader != NULL)
		pResMan->DestroyBigFileManagerImpl( pBigFileManagerReader, sModuleFile);
	pBigFileManagerReader = NULL;
	return S_OK;
}

void CResModule::destroy()
{
	MP_DELETE_THIS;
}

bool CResModule::isFileInPack( DWORD aFlags)
{
	if( pBigFileManagerReader != NULL 
		&& (dwStatus & RES_MODULE_PACK) != 0
		&& ((aFlags & RES_REMOTE) == 0 || (aFlags & RES_LOCAL) != 0))
		return true;
	return false;
}

long CResModule::openFileInPackFile( BSTR aResPath,  DWORD aFlags, IResourceNotify* aNotify, IResource** aRes)
{
	CWComString sResPath = aResPath;
	correctPath( sResPath);
	DWORD dwResStatus = (aFlags & ~(RES_ANYWHERE | RES_FILE_CACHE | RES_MEM_CACHE)) | RES_LOCAL | RES_MODULE_PACK;
	if( (aFlags & RES_ASYNCH) == RES_ASYNCH)
		dwResStatus |= RES_MEM_CACHE;
	CBigFileResource *pRes = MP_NEW( CBigFileResource);
	pRes->setManager( pResMan);
	pRes->setBigFileManagerReader( pBigFileManagerReader);
	pRes->open( sResPath.GetBuffer(), dwResStatus, aNotify, L"");
	*aRes = (IResource*)(pRes);
	return S_OK;
}

long CResModule::openAsynchResource( BSTR aResPath,  DWORD aFlags, IResourceNotify* aNotify, IResource** aRes)
{
	if( (aFlags & RES_ASYNCH) == 0)
	{
		aFlags |= RES_ASYNCH;
	}
	if( isFileInPack( aFlags))
	{
		return openFileInPackFile( aResPath, aFlags, aNotify, aRes);
	}
	CWComString sResPath = sModulePath + aResPath;
	correctPath( sResPath);
	
	DWORD cache = (RES_FILE_CACHE|RES_MEM_CACHE);
	if( ((aFlags & cache) == 0) ||
		((aFlags & cache) == cache))
	{
		aFlags |= RES_MEM_CACHE;
	}
	return pResMan->openResource( sResPath, aFlags, aRes, aNotify);
}

long CResModule::openResource( BSTR aResPath, DWORD aFlags, IResource** aRes)
{
	if( isFileInPack( aFlags))
	{
		return openFileInPackFile( aResPath,aFlags, NULL, aRes);
	}
	CWComString sResPath = sModulePath + aResPath;
	correctPath( sResPath);
	if ((aFlags & RES_ANYWHERE) == 0)
		aFlags |= RES_ANYWHERE;
	aFlags = (aFlags & ~RES_EXISTS) | (aFlags & (dwStatus & RES_EXISTS));
	if ((aFlags & RES_ANYWHERE) == 0)
		aFlags = RES_PATH_ERROR;

	if ((aFlags & RES_TO_WRITE) != 0)
	{
		if ((dwStatus & RES_TO_WRITE) == 0)
			aFlags |= RES_ACCESS_ERROR;
	}
	return pResMan->openResource( sResPath, aFlags, aRes);
}

long CResModule::resourceExists( BSTR aResPath, DWORD aFlags, DWORD* aStatus, bool abCheckOnlyExistInList)
{
	if (aStatus == NULL)
		return E_POINTER;

	if( isFileInPack( aFlags))
	{		
		CBigFileResource* aRes = NULL;
		openFileInPackFile(aResPath, aFlags, NULL, (IResource**)&aRes);
		if( aRes != NULL)
		{
			aRes->getStatus( aStatus);
			aRes->destroy();
		}
		return S_OK;
	}

	if ((aFlags & RES_EXISTS) == 0)
		aFlags |= RES_EXISTS;

	aFlags = (aFlags & ~RES_EXISTS) | (aFlags & (dwStatus & RES_EXISTS));

	if ((aFlags & RES_ANYWHERE) == 0)
		aFlags = RES_PATH_ERROR;

	CWComString sResPath = sModulePath + aResPath;
	*aStatus = pResMan->resourceExists( sResPath, aFlags, abCheckOnlyExistInList);
	
	return S_OK;
}

long CResModule::getStatus( DWORD* aStatus)
{
	if (aStatus == NULL)
		return E_POINTER;

	*aStatus = dwStatus;
	return S_OK;
}

long CResModule::getSize( DWORD* aSize)
{
	if (aSize == NULL)
		return E_POINTER;

	*aSize = dwSize;
	return S_OK;
}

long CResModule::getDownloadSize( DWORD* aSize)
{
	if (aSize == NULL)
		return E_POINTER;

	*aSize = dwDownloadedSize;
	return S_OK;
}

long CResModule::download( BOOL aWait)
{
	// если модуль есть в локальном хранилище
	IResModule *spModule = NULL;
	DWORD dwStatus1 = pResMan->getModuleExists( sModuleID, RES_LOCAL, &spModule);
	if ((dwStatus1 & RES_EXISTS) &&
		(!RES_IS_ERROR( dwStatus1) || (dwStatus1 & RES_ERROR) != RES_MODULE_NOT_READY)){
		// то скачивать ничего не будем
		spModule->destroy();
		spModule = NULL;
		return S_OK;
	}

	if (aWait)
		downloadModule();
	else
	{
		CSimpleThreadStarter downloadThreadStarter(__FUNCTION__);		
		downloadThreadStarter.SetRoutine(downloadThread_);
		downloadThreadStarter.SetParameter(this);
		hDownloadThread  = downloadThreadStarter.Start();
	}
	return S_OK;
}

long CResModule::getDownloadProcess(  BYTE* aPercent)
{
	if (aPercent == NULL)
		return E_POINTER;

	return S_OK;
}

long CResModule::unpack( BOOL aWait)
{
	CWComString sModuleFilePath = sModuleFile;
	IResource* spModuleFile = NULL;
	DWORD dwModuleStatus;
	pResMan->openResource( sModuleFilePath, RES_LOCAL, &spModuleFile);
	spModuleFile->getStatus( &dwModuleStatus);
	if ((dwModuleStatus & RES_EXISTS) != 0)
	{
		long hr = S_OK;
		if(FAILED(hr = spModuleFile->unpack( sModulePath.GetBuffer(), aWait, -1))){
			spModuleFile->getStatus(&dwStatus);
			dwStatus = (dwStatus & ~RES_ANYWHERE) | RES_LOCAL | MDL_PACKED;
			return hr;
		}
		open( sModuleID, sModulePath, sModuleFile, RES_LOCAL);
	}
	spModuleFile->destroy();
	return S_OK;
}

/*long CResModule::getUnpackProgress(  BYTE* aPercent, DWORD *pwUnpackTotalSize)
{
	if (aPercent == NULL)
		return E_POINTER;

	return S_OK;
}

long CResModule::pack( BOOL aWait)
{
	return S_OK;
}

long CResModule::getPackProgress( BYTE* aPercent)
{
	if (aPercent == NULL)
		return E_POINTER;

	return S_OK;
}*/

long CResModule::getMetadata(IResModuleMetadata **aMetadata)
{
	*aMetadata = (IResModuleMetadata *)(this);
	return S_OK;
}

long CResModule::copyResource(BSTR aSrcPath, IResModule *aDestModule, BSTR aDestPath)
{
	if (aDestPath == NULL || aDestPath[ 0] == 0)
		aDestPath = aSrcPath;

	CWComString sSrcPath = sModulePath + aSrcPath;
	
	return pResMan->copyResource( sSrcPath, aDestModule, CWComString( aDestPath));
}

long CResModule::getFullPath(BSTR aResPath, DWORD aFlags, CWComString &sFullPath)
{
	CWComString sPath = sModulePath;
	sPath += aResPath;
	//*aFullPath = 
	pResMan->getFullPath( sPath, aFlags, sFullPath);
	return S_OK;
}

long CResModule::copyFile(BSTR aSrcPath, BSTR aDestPath)
{
	return pResMan->copyResource( (CWComString)aSrcPath, (res::IResModule*)this, CWComString( aDestPath));		
}

long CResModule::extract(BSTR aSrcPath, BSTR aDestPath, CWComString &aFullPath)
{
	if (aDestPath == NULL || aDestPath[ 0] == 0)
		aDestPath = aSrcPath;

	CWComString sSrcPath = sModulePath + aSrcPath;
	correctPath( sSrcPath);
	
	if (SUCCEEDED( pResMan->copyResource( sSrcPath, NULL, CWComString( aDestPath), &aFullPath)))
	{
		return S_OK;
	}
	else
		return E_FAIL;
}

long CResModule::downloadMetadata(BOOL aWait)
{
	if (aWait)
		downloadMetadata();
	else
	{
		CSimpleThreadStarter downloadMetaThreadStarter(__FUNCTION__);		
		downloadMetaThreadStarter.SetRoutine(downloadMetadataThread_);
		downloadMetaThreadStarter.SetParameter(this);
		hDownloadMetadataThread  = downloadMetaThreadStarter.Start();
	}
	return S_OK;
}

long CResModule::removeResource(BSTR aPath, BOOL *aSuccess)
{
	CWComString sPath = sModulePath + aPath;
	correctPath( sPath);
	return pResMan->removeResource( sPath, aSuccess);
}

long CResModule::moveFile(BSTR aSrcPath, BSTR aDestPath, BOOL* aSuccess)
{
	CWComString sSrcPath = sModulePath + aSrcPath;
	correctPath( sSrcPath);
	CWComString sDestPath = sModulePath + aDestPath;
	correctPath( sDestPath);
	return pResMan->moveResource( sSrcPath, sDestPath, aSuccess);
}

long CResModule::removeFolder(BSTR aPath, BOOL aYesToAll, BOOL *aSuccess)
{
	CWComString sPath = sModulePath + aPath;
	correctPath( sPath);
	return pResMan->removeResource( sPath, aSuccess, aYesToAll);
}

///////////////////////////////////////////////////////////////////////
// IResModuleMetadata
long CResModule::getIdentifier(BSTR* aID)
{
	if (aID == NULL)
		return E_POINTER;

	long hRes = S_OK;
	if(bUserModule)
		return hRes;
	if (sModuleID.IsEmpty())
		hRes = (getMetadataStr( CComBSTR( "/lom/general/identifier/entry"), aID) ? S_OK : E_FAIL);
	else
		*aID = sModuleID.AllocSysString();
	return hRes;
}

long CResModule::getTitle(BSTR* aTitle, BSTR* aLanguage)
{
	if (aTitle == NULL || aLanguage == NULL)
		return E_POINTER;
	if(bUserModule)
		return S_OK;
	
	return (getMetadataLangStr( CComBSTR( "/lom/general/title"), aTitle, aLanguage) ? S_OK : E_FAIL);
}

long CResModule::getLanguage(BSTR *aLanguage)
{
	if (aLanguage == NULL)
		return E_POINTER;
	if(bUserModule)
		return S_OK;
	
	return (getMetadataStr( CComBSTR( "/lom/general/language"), aLanguage) ? S_OK : E_FAIL);
}

long CResModule::getDescription(BSTR *aDescription, BSTR* aLanguage)
{
	if (aDescription == NULL || aLanguage == NULL)
		return E_POINTER;
	if(bUserModule)
		return S_OK;
	
	return (getMetadataLangStr( CComBSTR( "/lom/general/description"), aDescription, aLanguage) ? S_OK : E_FAIL);
}

long CResModule::getMetadataString(BSTR aXPath, BSTR *aStr)
{
	if (aStr == NULL)
		return E_POINTER;
	if(bUserModule)
		return S_OK;
	
	return (getMetadataStr( aXPath, aStr) ? S_OK : E_FAIL);
}

long CResModule::getTaxonomy( CWComString &aTaxonomy)
{
	if(bUserModule)
		return S_OK;
	CLibXMLGuard oLXG;
	CWComString sXPath;
	sXPath = L"/lom/classification[purpose/source=\"LOMv1.0\"][purpose/value=\"discipline\"]/taxonPath/taxon/entry/string[@language=\"ru\"]";

	wchar_t *bsRes;
	xmlXPathObjectPtr xpObj = NULL;
	if (getMetadataStr( sXPath.AllocSysString(), &bsRes, &xpObj) && xpObj != NULL)
	{
		if (xpObj->nodesetval != NULL)
			for (int i = 0;  i < xpObj->nodesetval->nodeNr;  i++)
			{
				xmlChar* xcTaxonTitle = xmlNodeGetContent( xpObj->nodesetval->nodeTab[ i]);
				if (xcTaxonTitle != NULL)
				{
					UTF8_to_BSTR( xcTaxonTitle, &bsRes);
					aTaxonomy += bsRes;
					aTaxonomy += L" \\ ";
					xmlMemFree( xcTaxonTitle);
				}
			}
		xmlXPathFreeObject( xpObj);
	}
	if( aTaxonomy.GetLength()>3)
		aTaxonomy = aTaxonomy.Left( aTaxonomy.GetLength() - 3);
	/*if (*aTaxonomy != NULL)
		SysReAllocStringLen( aTaxonomy, *aTaxonomy, SysStringLen( *aTaxonomy) - 3);*/
	return S_OK;
}

long CResModule::getRelation(int aRelN, CWComString &aModuleID)
{
	if(bUserModule)
		return S_OK;
	CWComString sXPath;
	sXPath.Format( L"/lom/relation/resource/identifier/entry", aRelN);
	xmlXPathObjectPtr xPathRes;
	wchar_t *bsModuleID;
	aModuleID = L"";
	if (getMetadataStr( sXPath.AllocSysString(), &bsModuleID, &xPathRes) &&
		xPathRes->nodesetval->nodeNr > aRelN &&
		xPathRes->nodesetval->nodeTab[aRelN]->children[0].content != NULL)
	{
		wchar_t *wsModuleID = NULL;
		UTF8_to_BSTR( xPathRes->nodesetval->nodeTab[aRelN]->children[0].content, &wsModuleID);
		aModuleID = (BSTR)wsModuleID;
		if( wsModuleID != NULL)
			MP_DELETE_ARR(wsModuleID);
	}
	return S_OK;
}


long CResModule::getManifestSize(DWORD *aSize)
{
	if (aSize == NULL)
		return E_POINTER;

	*aSize = dwManifestSize;
	return S_OK;
}

long CResModule::getManifestDownloadedSize(DWORD *aSize)
{
	if (aSize == NULL)
		return E_POINTER;

	*aSize = dwManifestDownloadedSize;
	return S_OK;
}

long CResModule::pauseDownload()
{
	if (hDownloadEvent != NULL)
		ResetEvent( hDownloadEvent);
	return S_OK;
}

long CResModule::resumeDownload()
{
	if (hDownloadEvent != NULL)
		SetEvent( hDownloadEvent);
	return S_OK;
}
