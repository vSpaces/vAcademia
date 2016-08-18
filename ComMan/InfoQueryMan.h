#pragma once

#include <vector>
#include <map>

#include "DataBuffer2.h"
#include "Mutex.h"
#include "ILogWriter.h"

enum{ FILE_RES_EXIST, FILE_RES_NOT_EXIST, DIR_RES_EMPTY, DIR_RES_NOT_EXIST, DIRINFO_ALREADY_SENDED};

// Информация о ресурсе
struct CResInfo
{
	DWORD dwStatus;
	DWORD dwSize;
	CWComString sResPath;
	bool bDirectory;
	FILETIME oTime;
};

struct CResInfoSaver
{
	CResInfo info;
	CDataBuffer2 *pData;

	CResInfoSaver()
	{
		info.dwStatus = 0;
		info.dwSize = 0;
		pData = NULL;
	}

	~CResInfoSaver()
	{
		if( pData)
			MP_DELETE( pData);
	}

	CResInfoSaver( DWORD adwStatus, DWORD adwSize, FILETIME aoTime, BYTE *aData = NULL, WORD aSize = 0)
	{
		pData = NULL;
		info.dwStatus = adwStatus;
		info.dwSize = adwSize;
		info.oTime = aoTime;
		if ( aSize > 0 && aData != NULL)
		{
			MP_NEW_V2( pData, CDataBuffer2, aData, aSize);
		}
	}

	CResInfoSaver( CResInfo &resInfo, BYTE *aData = NULL, WORD aSize = 0)
	{
		pData = NULL;
		info.bDirectory = resInfo.bDirectory;	
		info.dwSize = resInfo.dwSize;
		info.dwStatus = resInfo.dwStatus;
		info.oTime = resInfo.oTime;
		info.sResPath = resInfo.sResPath;
		if ( aSize > 0 && aData != NULL)
		{
			MP_NEW_V2( pData, CDataBuffer2, aData, aSize);
		}
	}

	CResInfoSaver( CResInfoSaver &resInfoSaver)
	{
		pData = NULL;
		info.bDirectory = resInfoSaver.info.bDirectory;	
		info.dwSize = resInfoSaver.info.dwSize;
		info.dwStatus = resInfoSaver.info.dwStatus;
		info.oTime = resInfoSaver.info.oTime;
		info.sResPath = resInfoSaver.info.sResPath;
		if ( resInfoSaver.pData != NULL)
		{
			MP_NEW_V( pData, CDataBuffer2, resInfoSaver.pData);
		}
	}

	BOOL getResInfo( CResInfo *resInfo)
	{
		if ( resInfo == NULL)
			return FALSE;

		resInfo->dwStatus = info.dwStatus;
		resInfo->dwSize = info.dwSize;
		resInfo->oTime = info.oTime;
		return TRUE;
	}

	BOOL getData( BYTE **data, WORD *dwDataSize)
	{	
		if ( pData != NULL)
			return pData->readData( data, *dwDataSize);
		return FALSE;
	}
};

class CInfoQueryMan
{
public:
	CInfoQueryMan();
	virtual ~CInfoQueryMan();

public:
	//
	void clearResItemInfo( const wchar_t* aResPath, DWORD dwIfStatus = 0);
	//
	void clearInvalidResItemInfo( const wchar_t* aResPath);
	//
	void clearResInfoList();
	// ресурс существует в списке
	BOOL isQueryExist( const wchar_t* aResPath);
	// поиск ресурса в списке и возаращет информацию о нем
	int getResInfo( const wchar_t* aResPath, CResInfo* aResInfo, BYTE* aFirstBlock = NULL, WORD* aFirstBlockSize = NULL, /*bool abConnected = true,*/ bool abCheckOnlyExistInList = false);
	// Анализ полученной информации об одном ресурсе 
	BYTE analyseResInfo(const wchar_t* aResPath, DWORD aFlags, CDataBuffer2 *dataBuffer, CResInfo* aResInfo, BYTE* aFirstBlock = NULL, DWORD* aFirstBlockSize = NULL);
	// Анализ полученной информации об ресурсах модуля
	//void analyseMultiResInfo( const wchar_t* aResPath, DWORD aFlags, CDataBuffer2 *dataBuffer, CResInfo* aResInfo, BYTE* aFirstBlock = NULL, WORD* aFirstBlockSize = NULL);
	BYTE analyseMultiResInfo(const wchar_t* aResPath, DWORD aFlags, BYTE *pData, DWORD dwDataSize, CResInfo* aResInfo);

	void analyseAddNewFileInfoList(BYTE *pData, DWORD dwDataSize, bool aCheckExist);
	bool analyseRootFileInfoList(BYTE *pData, DWORD dwDataSize, bool aCheckExist);
	bool analyseRootFileInfoList2(BYTE *pData, DWORD dwDataSize);

	void setILogWriter(ILogWriter * lw);
	const wchar_t *getRepositoryPathClientUpdate();
	void setRepositoryPath(const wchar_t* aBase);
	void setUserPath(const wchar_t* aBase);

protected:
	CWComString GetPath( const wchar_t* aResPath);

protected:

	//
	typedef MP_MAP<CWComString, CResInfoSaver *> CResInfoSaverMap;
	typedef CResInfoSaverMap::iterator CResInfoSaverMapIter;
	CResInfoSaverMap resInfoSaverMap;

	typedef MP_MAP<CWComString, bool> CListRootModule;
	CListRootModule listRootModule;

	CMutex mutex;
	ILogWriter *m_pLW;
	bool bCacheInfoSet;
	CWComString sRepositoryPath;
	CWComString sRepositoryPathClientUpdate;
	CWComString sUserPath;

public:
	void update( CResInfo* aResInfo);
	void update( CWComString &aResPath, CResInfoSaver *resInfoSaver);
	void update( CResInfoSaver *resInfoSaver);
	void updateFileAndDir( CResInfoSaver *resInfoSaver);
	bool getCacheInfoSetting();
	void setCacheInfoSetting( bool aVal);
};
