// ResMan.h: interface for the CResMan class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "HTTP.h"
#include "ClientSession.h"
#include "ResourceImpl.h"
#include "LocalRepository.h"
#include "ResSessionHandler.h"
#include "UploadSessionHandler.h"
#include "BigFileman.h"
#include <vector>

//#define LOG_LOCAL_RESOURCE 1
#define LOG_LOCAL_RESOURCE_ERROR 1
//#define LOG_REMOTE_RESOURCE 1

#define CACHING_BLOCK 0xFFFF
// Ошибки кеширования
enum ECacheFileErrors { cfeOpenned, cfeCreated, cfeNotCacheable};

class CInfoQueryMan;
struct CResInfo;

// Информация о модуле
struct CModuleInfo
{
	DWORD dwStatus;
	CWComString sModulePath;
	CWComString sModuleFile;
};

void correctPath( CWComString& aPath);
void correctRemotePath( CWComString& aPath);
void correctCachePath( CWComString& sCacheResPath);

void assemblePath( CWComString& aResultPath, const wchar_t* aPath, const wchar_t* aBase);

#define RES_RESET							0
#define RES_FILE_INFO						1 // для двух серверов: download и upload

// для сервера download
#define RES_FILE_READ						2
#define RES_FILE_MULTI_INFO					3
#define RES_ROOT_MULTI_INFO					4
#define PACK_UPDATE_CLIENT_FILES_AND_INFO	5
#define RES_ROOT_MULTI_INFO2				6
#define PACK_CLIENT_SETUP					7
#define WRITE_UPDATER_LOG					8
#define REGISTER_RESOURCE					9

// для сервера upload
#define RES_FILE_WRITE						2
#define RES_FILE_REMOVE						3
#define RES_DIR_REMOVE						4

#define METHOD_TRANS_PHP		1
#define METHOD_TRANS_SOCKET		2

#include "iclientsession.h"
#include "oms_context.h"
#include "rmml.h"

static int _RES_OPENED_NOTIF_DESC_INDEX = 0;
typedef struct _RES_OPENED_NOTIF_DESC
{
	CResourceImpl* res;
	int index;
} RES_OPENED_NOTIF_DESC;

// Менеджер ресурсов
class CResMan : public cs::isessionlistener  
{
public:
	//CResMan( IUnknown* aOwner, CHTTP* aHTTP, ILogWriter* lw = NULL);
	CResMan( CHTTP* aHTTP, ILogWriter* lw = NULL);

	virtual ~CResMan();

	void LoadBigFileDll();
	int CreateBigFileManagerReaderImpl( bigfile::IBigFileManagerReader **pBigFileMan, const wchar_t* aModuleFile);
	void DestroyBigFileDll();
	void DestroyBigFileManagerImpl( bigfile::IBigFileManagerReader *apBigFileMan, const wchar_t* aModuleFile);
	HMODULE	hBigFileModule;

	void clearInfoList();

	void setContext(oms::omsContext *apContext);

	void setResSocket( CClientSession* aResSocket);
	void setResXMLSocket( CClientSession* aResXMLSocket);
	void setResUploadSocket( CClientSession* aResUploadSocket);

	// Получить с сервера информацию для валидации кеше
	//bool getValidateInfoCache();

	// Проверить ресурс на спец. расширение и вылать путь к файлу с нормальным расширением.
	bool refineResourceURL(CWComString &sPath);

	void correctPointPath( CWComString &as);

	// Открыть ресурс
	long openResource( const wchar_t* aResPath, DWORD aFlags, IResource** aRes, IResourceNotify* aNotify = NULL);
	// Проверить существование ресурса
	DWORD resourceExists( const wchar_t* aResPath, DWORD aFlags, bool abCheckOnlyExistInList = false);
	// Возвращает информацию о ресурсе
	void getResourceInfo( const wchar_t* aResPath, DWORD aFlags, CResInfo* aResInfo, bool aFullInfo = true, BYTE* aFirstBlock = NULL, DWORD* aFirstBlockSize = NULL, bool abCheckOnlyExistInList = false);
	// Установить базовый путь
	inline void setRemoteBase( const wchar_t* aBasePath) { sRemoteBase = aBasePath;sRemoteBase += L"/";correctRemotePath(sRemoteBase);};
	// Вернуть базовый путь
	inline const wchar_t* getRemoteBase() { return sRemoteBase;}
	// Установить локальный базовый путь
	void setLocalBase( const wchar_t* aBasePath);
	// Вернуть локальный базовый путь
	inline const wchar_t* getLocalBase() { return sLocalBase;};
	// Установить базовый путь кеша
	void setCacheBase( const wchar_t* aBasePath);
	// Вернуть базовый путь кеша
	inline const wchar_t* getCacheBase() { return sCacheBase;};
	// Установить базовый путь для репозитория
	inline void setRepositoryBase( const wchar_t* aBasePath) {
		createRepository( aBasePath);
	};	
	// Вернуть базовый путь репозитория
	inline const wchar_t* getRepositoryBase() { return sRepositoryBase;};
	// Установить базовый путь, где хранятся модули, созданные пользователем
	inline void setUserBase( const wchar_t* aBasePath) { sUserBase = aBasePath; makeUserBase();};
	inline void setUserBase( wchar_t* aBasePath) { sUserBase = aBasePath; makeUserBase();};
	void makeUserBase();
	// Вернуть базовый путь, где хранятся модули, созданные пользователем
	inline const wchar_t* getUserBase() { return sUserBase;};
	// Установить базовый путь, где хранятся модули, созданные пользователем
	inline void setTempBase( const wchar_t* aBasePath) { sTempBase = aBasePath; makeDir( sTempBase);};
	inline void setTempBase( BSTR aBasePath) { sTempBase = aBasePath; makeDir( sTempBase);};
	// Вернуть базовый путь, где хранятся модули, созданные пользователем
	inline const wchar_t* getTempBase() { return sTempBase;};
	inline BSTR getTempBaseBSTR() { return sTempBase.AllocSysString();};
	// Открывает запрос файла с сервера
	//EQueryError openResourceQuery( const wchar_t* aResPath, CHttpQuery*& aQuery);
	// Закрывает запрос файла с сервера
	inline EQueryError closeResourceQuery( CHttpQuery*& aQuery) {
		return (pHTTP != NULL ? pHTTP->closeQuery( aQuery) : qeNoError);
	}
	// Считывает данные из запроса файла с сервера
	inline EQueryError readResourceQuery( CHttpQuery& aQuery, DWORD aBufferSize, BYTE aBuffer[], DWORD& aReadSize) {
		return (pHTTP != NULL ? pHTTP->readQueryData( aQuery, aBufferSize, aBuffer, aReadSize) : qeNoError);
	}
	// Считывает данные из файла на сервере
	bool readRemoteResource( const wchar_t* aResPath, DWORD aPos, DWORD aCount, DWORD* aDataSize, BYTE** aData, DWORD* aStatus, DWORD* aFileSize, DWORD *aError);
	// Записываем данные в файл на сервере
	int writeRemoteResource(const wchar_t* aResPath, BYTE *aData, DWORD dwPos, DWORD aSizeToWrite, DWORD* aWrote, DWORD aTotalSize, DWORD *aStatus);
	// Получаем информацию о загруженном на сервер ресурсе
	int getUploadRemoteResource(const wchar_t* aResPath, DWORD *aStatus, byte *aStatusCode, std::wstring &aErrorText);
	// запрос на упаковку файлов обновления на сервере и получение информации об архиве
	int packUpdateClientFilesAndGetInfo(const wchar_t *aVersion, wchar_t *apBufferForFileName, unsigned int auBufferWideCharsCount, unsigned int *aFileSize, int *aStatus);
	// запрос на упаковку сетапа клиента файла и получение информации об архиве
	int packClientSetup(wchar_t *apBufferForFileName, unsigned int auBufferWideCharsCount, unsigned int *aFileSize, int *aStatus);
	//	пишем логи апдейтера
	void writeUpdaterLog(const char *logValue);
	// регистрируем ресурс на сервере
	void registerRemoteResource( const wchar_t *asFullResPath, DWORD &aStatus);
	// Возвращает полный путь
	void getFullPath( const wchar_t* aFileName, DWORD aFlags, CWComString &sFullPath);
	// Открывает файл в кеше
	ECacheFileErrors openCacheFile( const wchar_t* aResPath, DWORD aFlags, DWORD aResSize, const FILETIME& aResTime, HANDLE* aCacheFile, wchar_t* aCacheFilePath, unsigned int auCahceFilePathLength);
	bool copyCacheToLocal( const wchar_t* aCashPath, const wchar_t* aResPath, CWComString& aLocalFilePath, FILETIME &oTime, DWORD dwFileSize);
	// Отрыть модуль
	long openModule( const wchar_t* aModuleID, DWORD aFlags, IResModule** aModule);
	long openModuleByPath( const wchar_t* aModulePath, IResModule** aModule);
	// получить рекомендуемый размер пакета на получение данных
	long getInitialSpeed();
	// Проверить существование модуля
	DWORD getModuleExists( const wchar_t* aModuleID, DWORD aFlags, IResModule **ppModule);
	bool moduleReady( const wchar_t* aModuleID, DWORD aFlags, IResModule **ppModule);
	// Возвращает размер ресурса при его существовании
	DWORD	getResourceSize( const wchar_t* aResPath, WORD aFlags);
	//
	long copyResource(const wchar_t* aSrcPath, IResModule *aDestModule, const wchar_t* aDestPath, CWComString* aFullPath = NULL);
	//
	long moveResource(const wchar_t* aSrcPath, const wchar_t* aDestPath, BOOL* aSuccess);
	//
	long removeResource(const wchar_t* aPath, BOOL* aSuccess, BOOL aYesAll = TRUE);
	//
	long clearTemporary();
	// Импорт упакованного модуля в репозиторий
	DWORD importModule( const wchar_t* aFileName);
	//
	long	onResourceNotify(IResourceNotify* a_pIResourceNotify, IResource* a_pIResource, DWORD aNotifyCode, DWORD aNotifyServerCode, wchar_t *aErrorText);

	// Resources open/close logging
	void SetILogWriter(ILogWriter * lw);
	ILogWriter* GetLogWriter();
	void notifyOpen( CResourceImpl* aResoure);
	void notifyClose( CResourceImpl* aResoure);
	void writeStillOpened();
	void CloseAllResource();
	int getOpenedResCount();
	DWORD getMaxCacheSize();
	void setMaxCacheSize(DWORD newVal);
	
	inline void SetTransferType( byte iType ){ iTransferType = iType;	}
	inline int getTransferType(){return iTransferType; }	

	void OnConnectLost();
	void OnConnectRestored();

	void addRemoteResourceInfo(CWComString sResPath, DWORD dwSize, FILETIME oTime);

	
	void CreatePathRefinerImpl();

	void AddFileInfoList(BYTE* aData, int aDataSize);
	bool AddRootInfoList(BYTE* aData, int aDataSize);
	bool AddRootInfoList2(BYTE* aData, int aDataSize);

protected:
	HANDLE hPathRefinerModule;


	int iTransferType; //метод передачи даных 1-PHP 2-Socket
	// Информация о локальном ресурсе
	bool getLocalInfo(const wchar_t* aBasePath, const wchar_t* aResPath, 
					  DWORD aType, CResInfo* aResInfo, bool aFullInfo);
	// Информация о закешированном ресурсе
	/*bool getCacheInfo(const wchar_t* aBasePath, const wchar_t* aResPath, 
					  DWORD aType, CResInfo* aResInfo, bool aFullInfo);*/
	// Возвращает информацию о модуле
	void getModuleInfo( const wchar_t* aModuleID, DWORD aFlags, CModuleInfo& aModuleInfo);
	// Проверяет наличие модуля в заданных папках относительно базового пути
	DWORD checkModule( const wchar_t* aBase, int aPathCount, const CWComString* sModulePath, DWORD aFlags, CWComString sModuleFile, CWComString &sModuleFullPath);

	void refineResourceURLAndGetResourceInfo( CWComString& sResPath, CWComString& pResRefinePath, CResInfo& oResInfo, const wchar_t* aResPath, DWORD aFlags, bool abCheckOnlyExistInList = false);

	bool isLocalModule( const wchar_t* aName);

protected:
	DWORD unpackAllToDisk( const wchar_t* aZipFile, const wchar_t* aDestPath);
	DWORD getModuleIdFromXML( DWORD aTextSize, BYTE* aXmlText, CWComString& aModuleID);
	bool createRepository( const wchar_t* aBase);
	bool copyOneResource(const wchar_t* aSrcPath, IResModule *aDestModule, const wchar_t* aDestPath, BYTE* aBuffer, DWORD aBufferSize);
	bool copyDirResource(const wchar_t* aSrcPath, IResModule *aDestModule, const wchar_t* aDestPath, BYTE* aBuffer, DWORD aBufferSize);
	DWORD unpackToMemory( const wchar_t* aZipFile, const wchar_t* aFileName, DWORD* aDataSize, BYTE** aData);

	// Полный путь, где хранятся ресурсы установленные на локальном диске
	CWComString sLocalBase;
	//// Полный путь, где хранятся ресурсы установленные на компакт диске
	//CWComString sCDRomBase;
	// Полный путь, где хранится кеш
	CWComString sCacheBase;
	// Базовый путь на сервере для поиска на нем ресурсов
	CWComString sRemoteBase;
	// Базовый путь, где хранятся модули
	CWComString sRepositoryBase;
	// Базовый путь, где хранятся модули, созданные пользователем
	CWComString sUserBase;
	// Базовый путь, где хранится модуль UI
	CWComString sUIBase;
	// Базовый путь ко временной папке
	CWComString sTempBase;
	// Ссылка на класс для доступа к серверу по HTTP протоколу
	CHTTP* pHTTP;
	//Ссылка на класс для доступа к серверу через Socket
	CClientSession* pResSocket; 
	CClientSession* pResXMLSocket; 
	CClientSession* pResUploadSocket; 
	// Лог-файл
	ILogWriter* m_pLW;

	// Маска, какие ресурсы нужно кешировать в файловом кеше
	WORD wCacheMask;
	// Максимальный размер кеша
	DWORD dwMaxCacheSize;
	// COM-объект - владелец всех порождаемых менеджером объектов
	//IUnknown* pOwner;
	// Локаьный репозиторий
	CLocalRepository* pRepository;
	MP_VECTOR< RES_OPENED_NOTIF_DESC> vcOpenResources;
	// Критическая секция на обработку уведомлений от ресурсов
	CRITICAL_SECTION csNotify;

	CResSessionHandler resSessionHandler;
	CUploadSessionHandler uploadSessionHandler;

	CInfoQueryMan *pInfoQueryMan;

	int bConnectionLosted;

	oms::omsContext *pContext;
	DWORD dwMainThreadID;

	DWORD m_initialSpeed;

	// проверка доступности модулей
protected:
	CRITICAL_SECTION			csModulesRequest;	
	// 1 - requested
	// 2 - done
	enum MODULE_REQUEST_STATUS { MRS_UNKNOWN, MRS_REQUESTED, MRS_KNOWN};
	typedef struct _MODULE_REQUEST_KEY
	{
		MP_WSTRING moduleName;
		DWORD flags;

		_MODULE_REQUEST_KEY(): MP_WSTRING_INIT( moduleName), flags(0)
		{}

		bool operator<(const _MODULE_REQUEST_KEY& another) const
		{
			int compareResult = another.moduleName.compare( moduleName);
			if( compareResult != 0)
				return compareResult < 0 ? true : false;

			return (another.flags < flags) ? true : false;
		}
	} MODULE_REQUEST_KEY;

	typedef MP_MAP<MODULE_REQUEST_KEY, MODULE_REQUEST_STATUS> MapModuleRequestStatuses;
	typedef MP_MAP<MODULE_REQUEST_KEY, HANDLE> MapModuleRequestEvents;

	MapModuleRequestStatuses	mapModuleRequestStatuses;
	MapModuleRequestEvents		mapModuleRequestEvents;
};

 