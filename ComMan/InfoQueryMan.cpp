#include "stdafx.h"
#include "infoqueryman.h"
#include "ComMan.h"
#include "MultiFileInfoIn.h"
#include "AddNewFileInfoListIn.h"
#include "RootFileInfoListIn.h"
#include "RootFileInfoList2In.h"
#include "res_consts.h"

using namespace res;
using namespace ResManProtocol;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int global_count = 0;
int global_count_erase = 0;
int global_count_remove = 0;


CInfoQueryMan::CInfoQueryMan():
			MP_MAP_INIT( resInfoSaverMap),
			MP_MAP_INIT( listRootModule)
{
	m_pLW = NULL;
	bCacheInfoSet = false;
}

CInfoQueryMan::~CInfoQueryMan()
{
	clearResInfoList();
	m_pLW = NULL;
}

void CInfoQueryMan::setILogWriter(ILogWriter * lw)
{ 
	m_pLW = lw;
}

const wchar_t * CInfoQueryMan::getRepositoryPathClientUpdate()
{
	return sRepositoryPathClientUpdate.GetString();
}

void CInfoQueryMan::setRepositoryPath(const wchar_t* aBase)
{
	sRepositoryPath = aBase;
	correctRemotePath( sRepositoryPath);
	sRepositoryPathClientUpdate = sRepositoryPath + L"/client/";
}

void CInfoQueryMan::setUserPath(const wchar_t* aBase)
{
	sUserPath = aBase;
	correctRemotePath( sUserPath);
}

//////////////////////////////////////////////////////////////////////////

void CInfoQueryMan::clearResItemInfo( const wchar_t* aResPath, DWORD dwIfStatus /* = 0 */)
{
	CWComString resPath = aResPath;
	correctRemotePath( resPath);
	if ( isQueryExist( resPath.GetBuffer()))
	{
		CResInfoSaverMapIter iter = resInfoSaverMap.find( resPath);
		CResInfoSaver *resInfoSaver = (CResInfoSaver *) iter->second;				
		if ( resInfoSaver != NULL && (dwIfStatus == 0 || (( resInfoSaver->info.dwStatus & dwIfStatus) == 0)))
		{			
			MP_DELETE( resInfoSaver);
			resInfoSaver = NULL;
			global_count_remove++;
			global_count_erase++;
			resInfoSaverMap.erase( iter);
		}
		else if ( resInfoSaver == NULL)
		{
			ATLASSERT( resInfoSaver != NULL);
			global_count_remove++;
			global_count_erase++;
			resInfoSaverMap.erase( iter);
		}
	}
}

void CInfoQueryMan::clearInvalidResItemInfo( const wchar_t* aResPath)
{
	CWaitAndSignal wait( mutex);
	// сначала проверяем директорию, в которой находится файл
	CWComString sPath = GetPath( aResPath);
	// проверяем статус ресурса, если она существует, то не удалять
	clearResItemInfo( sPath.GetBuffer(), RES_EXISTS | RES_TO_WRITE | RES_CREATE | RES_MEM_CACHE | RES_FILE_CACHE);
	clearResItemInfo( aResPath, RES_EXISTS | RES_TO_WRITE | RES_CREATE | RES_MEM_CACHE | RES_FILE_CACHE);	
}

void CInfoQueryMan::clearResInfoList()
{
	CWaitAndSignal wait( mutex);
	ATLASSERT( (global_count-global_count_remove) == resInfoSaverMap.size());	
	CResInfoSaverMapIter iter = resInfoSaverMap.begin(), iter_end = resInfoSaverMap.end();
	for ( ; iter != iter_end; ++iter)
	{
		CResInfoSaver *resInfoSaver = (CResInfoSaver *) iter->second;
		if ( resInfoSaver)
		{
			MP_DELETE( resInfoSaver);
			resInfoSaver = NULL;
			global_count_remove++;
		}
		global_count_erase++;
	}
	resInfoSaverMap.clear();
	ATLASSERT( global_count == global_count_erase);
	ATLASSERT( resInfoSaverMap.size() == 0);	
	ATLASSERT( global_count_remove == global_count_erase);
	global_count = 0;
	global_count_erase = 0;
	global_count_remove = 0;
	listRootModule.clear();
	setCacheInfoSetting( false);
}

BOOL CInfoQueryMan::isQueryExist( const wchar_t* aResPath)
{
	return ( resInfoSaverMap.find( aResPath) != resInfoSaverMap.end());
}

int CInfoQueryMan::getResInfo( const wchar_t* aResPath, CResInfo* aResInfo, BYTE* aFirstBlock /* = NULL */, WORD* aFirstBlockSize /* = NULL */, /*bool abConnected,*/ bool abCheckOnlyExistInList)
{
	CWaitAndSignal wait( mutex);
	CWComString sPath = GetPath( aResPath);
	correctRemotePath( sPath);

	if( resInfoSaverMap.size() == 0)
	{
		if(sPath.Find(L"/ui/", 0) > -1)
		{
			aResInfo->dwSize = 0;
			aResInfo->dwStatus = RES_PATH_ERROR;
			return -1;
		}
		if(/*abConnected && */wcsstr(aResPath, L"manifest.xml")!=NULL)
			return 0;
		return -1;
	}

	
	if ( isQueryExist( sPath.GetBuffer()))
	{
		// директории файла aResPath есть в списке ответов сервера
		//ATLTRACE( "\n CInfoQueryMan::getResInfo::aResPath is not exist = %s \n", aResPath);
		//ATLTRACE( "\n CInfoQueryMan::getResInfo::sPath is not exist = %s \n", sPath);		
		CResInfoSaver *resInfoSaver = ( CResInfoSaver *) resInfoSaverMap[ sPath];
		if ( resInfoSaver->info.dwStatus == RES_SCRIPT_ERROR)
		{
			/*if(resInfoSaverMap.size()>0 && sPath.Find("meta-inf", 0)==-1 
				&& (aResInfo->dwStatus & RES_TO_WRITE)!=RES_TO_WRITE)*/
			if((aResInfo->dwStatus & RES_TO_WRITE)!=RES_TO_WRITE)
				aResInfo->dwStatus = resInfoSaver->info.dwStatus;
			if(m_pLW!=NULL)
				m_pLW->WriteLn( CLogValue("[ERROR] this dir: ", sPath.GetBuffer(), " is not exist with status in list from resServer status==", (unsigned int)aResInfo->dwStatus));
			return -1;
		}
	}

	CWComString resPath = aResPath;	
	correctRemotePath( resPath);
	if ( !isQueryExist( resPath.GetBuffer()))
	{
		//ATLTRACE( "\n CInfoQueryMan::getResInfo::new resPath = %s \n", resPath.GetBuffer());
		if( resPath.Find(L"meta-inf/manifest.xml", 0)>-1 && listRootModule.find(resPath) == listRootModule.end())
			return 0;

		int posDiez = resPath.ReverseFind(L'#');
		int posTochka = resPath.ReverseFind(L'.');
		
		if((sUserPath.IsEmpty() || abCheckOnlyExistInList || (resPath.Find(sUserPath, 0) == -1 && posDiez <= posTochka)) && resPath.Find(L"tp://", 0) == -1
			&& (aResInfo->dwStatus & RES_CREATE)!=RES_CREATE
			&& (aResInfo->dwStatus & RES_TO_WRITE)!=RES_TO_WRITE
			&& (aResInfo->dwStatus & RES_UPLOAD)!=RES_UPLOAD)
		{
			if(m_pLW!=NULL)
				m_pLW->WriteLn( CLogValue("[ERROR] this file is not exist in list from resServer: ", resPath.GetBuffer()));
			aResInfo->dwSize = 0;
			aResInfo->dwStatus = RES_PATH_ERROR;
		}
		/*if(m_pLW!=NULL)
			m_pLW->WriteLn( CLogValue("[WARNING] this file is not exist in list from resServer: ", resPath.GetBuffer()));*/

		return -1;	
	}
	//ATLTRACE( "\n CInfoQueryMan::getResInfo::old resPath = %s \n", resPath.GetBuffer());
	CResInfoSaver *resInfoSaver = ( CResInfoSaver *) resInfoSaverMap[ resPath];
	resInfoSaver->getResInfo( aResInfo);
	if ( aFirstBlock != NULL && aFirstBlockSize != NULL && *aFirstBlockSize != 0)
	{
		resInfoSaver->getData( &aFirstBlock, aFirstBlockSize);
	}
	return 1;
}

BYTE CInfoQueryMan::analyseResInfo(const wchar_t* aResPath, DWORD aFlags, CDataBuffer2 *dataBuffer, CResInfo* aResInfo, BYTE* aFirstBlock /*= NULL */, DWORD* aFirstBlockSize /* = NULL */)
{
	DWORD dwDataSize = dataBuffer->getSize() - dataBuffer->getPosition();
	CResInfoSaver *resInfoSaver = MP_NEW(CResInfoSaver);
	BYTE status_ = 0;
	BYTE btError = 0;
	if ( !dataBuffer->read( status_))
	{
		ATLASSERT( status_ == 1);
		status_ = 0;
		btError = 5;
	}
	else
	{
		/*if ( status_ != 1)
			ATLTRACE( "\n CInfoQueryMan::analyseResInfo::RES_STATUS not equal '1' = '%s' \n", aResPath);		*/
	}

	aResInfo->dwStatus = (status_ == 1 ? (RES_REMOTE | (aFlags & RES_MEM_CACHE)) : 
	(status_ == 0 ? RES_PATH_ERROR : RES_SCRIPT_ERROR));

	if(status_!=1 && status_!=0)
		btError = 6;
	if (aResInfo->dwStatus & RES_REMOTE)
	{
		if ( !dataBuffer->read( aResInfo->dwSize))
		{
			aResInfo->dwSize = 0;
			ATLASSERT( aResInfo->dwSize > 0);
		}
		//else
		//	ATLASSERT( aResInfo->dwSize > 0);

		__int64 dwSecTime = 0;
		if ( !dataBuffer->read( dwSecTime))
		{
			dwSecTime = 0;
			//ATLASSERT( dwSecTime > 0);	
		}
		//else
		//	ATLASSERT( dwSecTime > 0);

		aResInfo->oTime.dwLowDateTime = (DWORD)dwSecTime;
		aResInfo->oTime.dwHighDateTime = dwSecTime >> 32;

		if ( aFirstBlock != NULL && aFirstBlockSize != NULL && *aFirstBlockSize != 0 && dwDataSize > 9)
		{
			DWORD wBlockSize = dwDataSize - 9;
			if (wBlockSize < *aFirstBlockSize)
				*aFirstBlockSize = wBlockSize;
			if ( dataBuffer->readData( &aFirstBlock, *aFirstBlockSize))
			{
				MP_NEW_V2(resInfoSaver->pData, CDataBuffer2, aFirstBlock, *aFirstBlockSize);
			}			
		}
	}
	resInfoSaver->info = *aResInfo;	
	CWComString sResPath = aResPath;
	update( sResPath, resInfoSaver);
	return btError;
}

BYTE CInfoQueryMan::analyseMultiResInfo(const wchar_t* aResPath, DWORD aFlags, BYTE *pData, DWORD dwDataSize, CResInfo* aResInfo)
{	
	CWaitAndSignal wait( mutex);
	CMultiFileInfoIn multiFileInfo(pData, dwDataSize);
	if(!multiFileInfo.Analyse())
	{
		if(m_pLW!=NULL)
			m_pLW->WriteLnLPCSTR("[ANALYSE FALSE] CMultiFileInfoIn");
		ATLASSERT( FALSE);
		return 255;
	}
	BYTE errorID = multiFileInfo.GetError();
	DWORD count = multiFileInfo.GetCount();

	if ( errorID == DIR_RES_EMPTY || errorID == DIR_RES_NOT_EXIST)
	{
		aResInfo->dwStatus = RES_SCRIPT_ERROR;
		MP_NEW_P(resInfoSaver, CResInfoSaver, *aResInfo);
		CWComString sPath = GetPath( aResPath);
		update( sPath, resInfoSaver);
		//ATLTRACE( "\n CInfoQueryMan::analyseMultiResInfo::Dir is not exist or is empty = '%s' \n", aResPath);
		return errorID;
	}
	else if ( errorID == FILE_RES_NOT_EXIST)
	{
		//aResInfo->dwStatus = RES_SCRIPT_ERROR;		
		update( aResInfo);
		//ATLTRACE( "\n CInfoQueryMan::analyseMultiResInfo::File is not exist = '%s' \n", aResPath);
	}
	else if ( errorID == FILE_RES_EXIST)
	{
		//BYTE bExist = 0;
		CResInfoSaver *resInfoSaver = MP_NEW(CResInfoSaver);
		multiFileInfo.GetFirstFileInfo(aFlags, aResInfo);
		resInfoSaver->info = *aResInfo;
		CWComString sResPath = aResPath;
		update( sResPath, resInfoSaver);
	}
	else if(errorID==DIRINFO_ALREADY_SENDED)
	{
		aResInfo->dwStatus = RES_SCRIPT_ERROR;
		update( aResInfo);
		//ATLTRACE( "\n CInfoQueryMan::analyseMultiResInfo::Dir Info server already sended = '%s' \n", aResPath);
	}
	else
	{
		ATLASSERT( FALSE);
	}

	for ( DWORD i = 0; i < count; i++)
	{
		CResInfoSaver *resInfoSaver = MP_NEW(CResInfoSaver);
		resInfoSaver->info.bDirectory = false;
		if(!multiFileInfo.GetFileInfo(i, aFlags, resInfoSaver))
		{
			MP_DELETE(resInfoSaver);
			continue;
		}
		CWComString sPath = resInfoSaver->info.sResPath;	
		correctRemotePath( sPath);
		resInfoSaver->info.sResPath = sPath.GetBuffer();
		update( resInfoSaver->info.sResPath, resInfoSaver);
	}
	if( m_pLW != NULL)
		m_pLW->WriteLn( CLogValue("got multi info about module: ", aResPath, " - from server"));
	return errorID;
}

void CInfoQueryMan::analyseAddNewFileInfoList(BYTE *pData, DWORD dwDataSize, bool aCheckExist)
{	
	CWaitAndSignal wait( mutex);
	CAddNewFileInfoListIn addNewFileInfoList(pData, dwDataSize);
	if(!addNewFileInfoList.Analyse( m_pLW))
	{
		if(m_pLW!=NULL)
		{
			m_pLW->WriteLn("[ANALYSE FALSE] CAddNewFileInfoListIn, dwDataSize = ", (unsigned int)dwDataSize);
			m_pLW->WriteBufferAsArray(pData, dwDataSize);
		}
		ATLASSERT( FALSE);
		return;
	}
	int count = (int)addNewFileInfoList.GetCount();

	for ( int i = 0; i < count; i++)
	{
		CResInfoSaver *resInfoSaver = MP_NEW(CResInfoSaver);
		resInfoSaver->info.bDirectory = false;
		if(!addNewFileInfoList.GetFileInfo(i, resInfoSaver))
			continue;
		CWComString sPath = resInfoSaver->info.sResPath;	
		correctRemotePath( sPath);
		resInfoSaver->info.sResPath = sPath.GetBuffer();
		if(aCheckExist)
			updateFileAndDir( resInfoSaver);
		else
			update( resInfoSaver->info.sResPath, resInfoSaver);
	}
	/*if( !bCacheInfoSet)
		bCacheInfoSet = true;*/
}

bool CInfoQueryMan::analyseRootFileInfoList(BYTE *pData, DWORD dwDataSize, bool aCheckExist)
{	
	CWaitAndSignal wait( mutex);
	CRootFileInfoListIn rootFileInfoList(pData, dwDataSize);
	if(!rootFileInfoList.Analyse())
	{
		if(m_pLW!=NULL)
			m_pLW->WriteLnLPCSTR("[ANALYSE FALSE] CAddRootInfoListIn");
		ATLASSERT( FALSE);
		return false;
	}
	int count = (int)rootFileInfoList.GetCount();

	for ( int i = 0; i < count; i++)
	{
		CResInfoSaver *resInfoSaver = MP_NEW(CResInfoSaver);
		resInfoSaver->info.bDirectory = false;
		if(!rootFileInfoList.GetFileInfo(i, resInfoSaver))
		{
			MP_DELETE(resInfoSaver);
			continue;
		}
		correctRemotePath( resInfoSaver->info.sResPath);
		if(aCheckExist)
			updateFileAndDir( resInfoSaver);
		else
			update( resInfoSaver->info.sResPath, resInfoSaver);
	}
	if( !bCacheInfoSet)
		bCacheInfoSet = true;
	return true;
}

bool CInfoQueryMan::analyseRootFileInfoList2(BYTE *pData, DWORD dwDataSize)
{	
	CWaitAndSignal wait( mutex);
	CRootFileInfoList2In rootFileInfoList(pData, dwDataSize);
	if(!rootFileInfoList.Analyse())
	{
		if(m_pLW!=NULL)
			m_pLW->WriteLnLPCSTR("[ANALYSE FALSE] RES_ROOT_MULTI_INFO2");
		ATLASSERT( FALSE);
		return false;
	}
	else
	{
		if(m_pLW != NULL)
			m_pLW->WriteLnLPCSTR("[RES_ROOT_MULTI_INFO2] Analyzed correctly.");
	}
	rootFileInfoList.ReadData( this);
	if(m_pLW != NULL)
	{
		CComString sLog; sLog.Format("[RES_ROOT_MULTI_INFO2] resInfoSaverMap.size() = %u", resInfoSaverMap.size());
		m_pLW->WriteLnLPCSTR(sLog.GetBuffer());
	}

	if( !bCacheInfoSet)
		bCacheInfoSet = true;

	return true;
}

//////////////////////////////////////////////////////////////////////////

CWComString CInfoQueryMan::GetPath( const wchar_t* aResPath)
{
	CWComString sPath = aResPath;
	int index1 = sPath.ReverseFind( '\\');
	int index2 = sPath.ReverseFind( '/');
	int index = index1 > index2 ? index1 : index2;
	if ( index > -1 && index < sPath.GetLength() - 1)
		sPath = sPath.Left( index + 1);	
	return sPath;
}

//////////////////////////////////////////////////////////////////////////

void CInfoQueryMan::update( CResInfo* aResInfo)
{	
	CWComString sPath = aResInfo->sResPath;
	correctRemotePath( sPath);
	if ( !isQueryExist( sPath.GetBuffer()))
	{
		MP_NEW_P(resInfoSaver, CResInfoSaver, *aResInfo);
		resInfoSaverMap.insert( CResInfoSaverMap::value_type( sPath, resInfoSaver));
		global_count++;
		if( sPath.Find(L"meta-inf/manifest.xml")>-1)
			listRootModule.insert( CListRootModule::value_type( sPath, true));
	}	
}

void CInfoQueryMan::update( CWComString &aResPath, CResInfoSaver *resInfoSaver)
{		
	correctRemotePath( aResPath);
	if ( !isQueryExist( aResPath))
	{
		resInfoSaverMap.insert( CResInfoSaverMap::value_type( aResPath, resInfoSaver));
		global_count++;
		if( aResPath.Find(L"meta-inf/manifest.xml")>-1)
			listRootModule.insert( CListRootModule::value_type( aResPath, true));
	}
	else
	{
		MP_DELETE( resInfoSaver);
	}
}

void CInfoQueryMan::update( CResInfoSaver *resInfoSaver)
{		
	update( resInfoSaver->info.sResPath, resInfoSaver);
}

void CInfoQueryMan::updateFileAndDir( CResInfoSaver *resInfoSaver)
{		
	CWComString sResPath = resInfoSaver->info.sResPath;
	correctRemotePath( sResPath);
	CWComString sDir = sResPath.Left(sResPath.ReverseFind('/')+1);
	clearResItemInfo(sDir.GetBuffer());
	if ( !isQueryExist( sResPath))
	{
		resInfoSaverMap.insert( CResInfoSaverMap::value_type( sResPath, resInfoSaver));
		global_count++;
		if( sResPath.Find(L"meta-inf/manifest.xml")>-1)
			listRootModule.insert( CListRootModule::value_type( sResPath, true));
	}
	else
	{
		CResInfoSaverMapIter iter = resInfoSaverMap.find( sResPath);
		CResInfoSaver *resInfoSaverOld = (CResInfoSaver *) iter->second;	
		if(resInfoSaverOld->info.dwSize == resInfoSaver->info.dwSize
			&& resInfoSaverOld->info.oTime.dwHighDateTime == resInfoSaver->info.oTime.dwHighDateTime
			&& resInfoSaverOld->info.oTime.dwLowDateTime == resInfoSaver->info.oTime.dwLowDateTime)
		{
			MP_DELETE( resInfoSaver);
			return;
		}
		MP_DELETE( resInfoSaverOld);
		resInfoSaverMap.erase( iter);
		resInfoSaverMap.insert( CResInfoSaverMap::value_type( sResPath, resInfoSaver));
	}
}

bool CInfoQueryMan::getCacheInfoSetting()
{
	return bCacheInfoSet;
}

void CInfoQueryMan::setCacheInfoSetting( bool aVal)
{
	bCacheInfoSet = aVal;
}