// ComManager.h : Declaration of the CComManager

#ifndef __COMMANAGER_H_
#define __COMMANAGER_H_

#include "resource.h"       // main symbols
#include "HTTP.h"
#include "ResMan.h"	// Added by ClassView
#include "oms_context.h"
#include "res.h"
#include "ComManRes.h"
#include "iclientsession.h"
#include "ClientSession.h"
#include "HelperFunctions.h"
#include "HelperFunctions.h"
#include <sstream>
#include <algorithm>
#include "ConnectionManager.h"
#include "RM.h"

//using namespace oms;
using namespace res;

// Ошибки запросов
#define EC_NOERROR			0
#define EC_SERVERERROR		1
#define EC_QUERYNOTERROR	2
#define EC_SCRIPTERROR		3
#define EC_READERROR		4
#define EC_NOTIMPLERROR		5
//qeServerNotResolved, qeTimeOut


#define MAX_RES_PACKET_ZISE	65536
#define MIN_RES_PACKET_ZISE	8192

#define MANIFESTXML	L"\\META-INF\\manifest.xml"
//#define MODULESBASE	L"files\\"
#define MODULESBASE	L""

class resResource;
class CAsyncQuery;
class CAsyncCheckExist;

typedef MP_LIST<CClientSession*> TClientSessionList;

enum ESERVERSCONNECTIONGROUP	{ SCG_ALL, SCG_RESOURCES, SCG_ALLEXCEPTRESOURCES};

/////////////////////////////////////////////////////////////////////////////
// CComManager
class CComManager : 
	public cm::cmICommunicationManager,
	public resIResourceManager,
	public IResMan
{
public:
	CComManager(omsContext* aContext);
	virtual ~CComManager();

	// Comman manager

public:
	UINT uploadFileProc(BSTR localName, BSTR serverName);
	//long OnDraw(ATL_DRAWINFO& di);

public:

	// IComManager - внутренние функции использующие только внутри ComManager (ResManager) и из OMSPlayer
	long get_userBase(/*[out, retval]*/ BSTR *pVal);
	//long put_userBase(/*[in]*/ BSTR newVal);
	//long put_sessionID(/*[in]*/ long newVal);
	//long sendQueryStr(/*[in]*/ VARIANT aQuery, /*[in]*/ VARIANT aParams, /*[out, retval]*/ VARIANT* aResult);	
	//long put_localBase(/*[in]*/ BSTR newVal);	
	//long get_logID(/*[out, retval]*/ BSTR *pVal);
	//long put_logID(/*[in]*/ BSTR newVal);
//	long get_cacheBase( CComString &pVal);
	long put_cacheBase(/*[in]*/ BSTR newVal);
	//long put_PathIni(/*[in]*/ BSTR newVal);
	long get_LastError(/*[out, retval]*/ long *pVal);
	long put_LastError(/*[in]*/ long newVal);
	long uploadFile(BSTR localName, BSTR serverName, BOOL inThread);
	long get_repositoryBase(/*[out, retval]*/ BSTR *pVal);
	//long put_repositoryBase(/*[in]*/ BSTR newVal);
	long getResourceSize(/*[in]*/ BSTR aResPath, /*[in]*/ WORD aFlags, /*[out]*/ DWORD *aSize);
	
	// IComMan - внутренние функции использующие только внутри ComManager (ResManager)
	EQueryError sendQuery(BSTR aQuery, BSTR aParams, CComString &aResult);
	EQueryError sendQuerySync(BSTR aQuery, BSTR aParams, CComString &aResult);	
	EQueryError sendQueryPost(BSTR aQuery, BSTR aParams, CComString &aResult);
	EQueryError sendQueryData(/*[in]*/ BSTR aQuery, /*[in]*/ BSTR aParams, /*[in, out]*/ ULONG* aDataSize, /*[out, size_is( *aDataSize)]*/ BYTE** aData);
	EQueryError sendQueryDataSync(/*[in]*/ BSTR aQuery, /*[in]*/ BSTR aParams, /*[in, out]*/ ULONG* aDataSize, /*[out, size_is( *aDataSize)]*/ BYTE** aData);
	long getSessionID(/*[out]*/ long *pVal);
	long setSessionIDAndLogSendType(long newVal, int aiTypeSendLog, CComString aSessionKey);
	long getServer(/*[out]*/ BSTR *pVal);
	long setQueryBase(/*[in]*/ BSTR newVal);
	//long getProxy(/*[out]*/ BYTE* aProxyUsing, /*[out]*/ BSTR* aProxy, /*[out]*/ BSTR* aProxyUser, /*[out]*/ BSTR* aProxyPassword);
	long setLoginData(/*[in]*/ DWORD aDataSize, /*[in, size_is( aDataSize)]*/ BYTE* aData, /*[out]*/ int* aError);
	void setServerInfo(CServerInfo *pServerInfo, unsigned char *&ipElement, unsigned char *&pEnd);
	void setServerAdditionalInfo( CServerInfo *pServerInfo, unsigned char *&ipElement, unsigned char *&pEnd);
	//long createIComManager(/*[in]*/ ULONG* apContex);
	long getMaxCacheSize( /*[out]*/DWORD *pVal);
	long setMaxCacheSize( /*[in]*/DWORD newVal);
	long getCacheBase( CWComString &pVal);
	const wchar_t* getCacheBase();
	long getClientStringID(/*[out]*/ BSTR *pVal);
	long getConnectionStatus(/*[out]*/ ULONG* pStatus);

	//long createIResManager(/*[in]*/ ULONG* apContex);
	//long OpenAsynchResource(/*[in]*/ BSTR apwcURL, /*[out]*/ resIResource* &ares, LPVOID apNotifier, DWORD dwFlags=0);

	// IResMan - внутренние функции использующие только внутри ComManager (ResManager)
	long openResource( BSTR aResPath, DWORD aFlags, IResource** aRes);
	long openModule( BSTR aResPath, DWORD aFlags, IResModule** aRes);
	long resourceExists( BSTR aResPath,  DWORD aFlags, DWORD* aStatus, bool abCheckOnlyExistInList = false);
	long getFullPath( BSTR aResPath,  DWORD aFlags, CWComString &sFullPath);
	long getRemoteBase( wchar_t **pVal);
	long getLocalBase( wchar_t **pVal);
	//long setLocalBase( BSTR newVal);
	long getRepositoryBase( std::wstring &pVal);
	//long setRepositoryBase( BSTR newVal);
	long getUserBase( wchar_t **pVal);
	//long setUserBase( BSTR newVal);
	long setupResMan( BSTR aRegKey);
	//long setConnectType(  int iType );
	long getModuleExists( BSTR aaModuleID, DWORD aFlags, DWORD* aStatus, IResModule **ppModule);
	//long getErrorMessage( DWORD aError, wchar_t *aLang, wchar_t **aMessage);
	long openAsynchResource( BSTR aResPath,  DWORD aFlags, IResourceNotify* aNotify, IResource** aRes);
	long writeStillOpened();
	long openModuleByPath( BSTR aModulePath, IResModule** aRes);
	long importModule( BSTR aFileName, DWORD* aErrors);

	void PackResource(wchar_t* path, wchar_t* newPath);

	void LoadModuleMetaAndLoadScene(unsigned int aID, wchar_t *apModuleID, wchar_t *asScene);
	void OnLoadedModuleMeta(unsigned int aID, int iError);

	// cm::cmICommunicationManager
	virtual long getProxy(/*[out]*/ BYTE* aProxyUsing, /*[out]*/ BSTR* aProxy, /*[out]*/ BSTR* aProxyUser, /*[out]*/ BSTR* aProxyPassword);
	virtual void CreateSyncServerSession(cs::iclientsession** ppClientSession);
	virtual void CreateServiceServerSession(cs::iclientsession** ppClientSession);
	virtual void CreateServiceWorldServerSession(cs::iclientsession** ppClientSession);
	virtual void CreateVoipPlayServerSession(cs::iclientsession** ppClientSession, cs::CServerInfo *apServerInfo);
	virtual void CreateVoipRecordServerSession(cs::iclientsession** ppClientSession, cs::CServerInfo *apServerInfo);
	virtual void CreateVoipSelfRecordingServerSession(cs::iclientsession** ppClientSession, cs::CServerInfo *apServerInfo);
	virtual void CreateSharingServerSession(cs::iclientsession** ppClientSession, cs::CServerInfo *apServerInfo);
	virtual void DestroyServerSession(cs::iclientsession** ppClientSession);
	virtual bool SetCallbacks(cm::cmIComManCallbacks* apCallbacks);
	virtual void CreatePreviewServerSession(cs::iclientsession** ppClientSession, cs::CServerInfo *apServerInfo);
	virtual const wchar_t* Query(const wchar_t* apwcQuery, const wchar_t* apwcParams=NULL, int* aiErrorCode=NULL);
	virtual const wchar_t* QueryPost(const wchar_t* apwcQuery, const wchar_t* apwcParams=NULL, int* aiErrorCode=NULL);
	virtual unsigned char* QueryData(const wchar_t* apwcQuery, const wchar_t* apwcParams=NULL, 
		unsigned long* aDataSize=NULL, unsigned char** aData=NULL, int* aiErrorCode=NULL);
	virtual unsigned char* queryDataSync(const wchar_t* apwcQuery, const wchar_t* apwcParams=NULL, 
		unsigned long* aDataSize=NULL, unsigned char** aData=NULL, int* aiErrorCode=NULL);
	virtual int SendQuery(const wchar_t* apwcQuery, const wchar_t* apwcParams=NULL);
	virtual int SendQuery(const wchar_t* apwcQuery, const wchar_t* apwcParams, cm::cmIComManCallbacks* aHandler);
	//virtual int SendQueryLogin(const wchar_t* apwcParams=NULL, cm::cmIComManCallbacks* aHandler=NULL, int* aiErrorCode=NULL);
	virtual int SetLoginData(unsigned int aDataSize, const wchar_t* aData);
	virtual long PutPathIni(wchar_t *newVal);
	virtual void SetLocalBase( wchar_t *newVal);
	virtual void SetRepositoryBase( wchar_t *newVal);
	virtual void SetConnectType( byte aType );
	virtual bool IsWinSocketConnection();
	virtual cm::ichangeconnection* GetConnectionChanger();
	virtual cm::IConnectionManager* GetConnectionManager();

	virtual void DropAndFreezeConnection( USHORT	signature);
	virtual void UnfreezeConnection( USHORT	signature);
	virtual void ReloadUserSettings();
	virtual void SetProxyLoginPass(const wchar_t * aProxyUser, const wchar_t * aProxyPassword, unsigned char aProxyType, const wchar_t * aProxySetting);
	virtual void CloseAllSessions();
	virtual void DestroyComman();
	long setRemoteBase( wchar_t *newVal);
	long setServer( wchar_t *newVal);
	unsigned short setTcpPort( unsigned short newVal);
	long setCacheBase( wchar_t *newVal);	
	long setHost( wchar_t *newVal);	
	long setBuildType( wchar_t *newVal);	
	void LogLoginBeginTime();
	const std::wstring GetMyIP();
	unsigned char GetProxiesMask();
	long GetRecommendedPacketSize(/*int aThreadsCount*/);
	void RecalcRecommendedPacketSize(unsigned long aBytesPerSecond/*, int aThreadsCount*/);
	void SetNetworkBreaksEmulationDelay( int auDelay, int auConnectionsMask);
	
	// возвращает true, если авторизация прошла успешно
	//virtual int Login(const wchar_t* apwcLogin, const wchar_t* apwcPassword, int* aiErrorCode=NULL);
	//virtual bool Logout(int* aiErrorCode=NULL);
	virtual unsigned int GetConnectionStatus();
	virtual const wchar_t* GetServerName();
	virtual const wchar_t* GetServerIP();
	virtual const wchar_t* GetClientStringID();
	virtual bool ConnectToWorld(const wchar_t* apwcURL);
	virtual unsigned long long GetSessionID();
	const wchar_t *GetHost();
	const wchar_t *GetBuildType();

	// res::resIResourceManager
	virtual	void ClearInfoList();
//	virtual	omsresult OpenAsynchResource(const wchar_t* apwcURL, resIResource* &ares, LPVOID apNotifier, unsigned int dwFlags=0);
	virtual	omsresult OpenAsynchResource(const wchar_t* apwcURL, resIResource* &ares, IResourceNotify* apNotifier, unsigned int dwFlags = 0);
	virtual omsresult OpenResource(const wchar_t* apwcURL, resIResource* &ares, unsigned int dwFlags=0);
	virtual omsresult OpenToWrite(const wchar_t* apwcURL, resIWrite* &ares, unsigned int adwFlags=0);
	virtual bool ResourceExists(const wchar_t* apwcURL, unsigned int dwFlags, bool abCheckOnlyExistInList, unsigned int &aCodeError);
	virtual bool ResourceExists(const wchar_t* apwcURL, unsigned int &aCodeError);
	virtual bool ResourceExists(const wchar_t* apwcURL, unsigned int dwFlags, unsigned int &aCodeError);
	virtual IAsyncCheckExist *ResourceExistsAsync( resIAsynchResource* aAsynch, const wchar_t* apwcURL, unsigned int dwFlags, bool abCheckOnlyExistInList= false);
	virtual BOOL RemoveResource(const wchar_t* aPath);
	virtual bool GetModuleExists(const wchar_t* apwcModuleID, unsigned int dwFlags, void **ppModule, unsigned int *adwCodeError);
	virtual void DestroyModule( void **ppModule);
	virtual wchar_t* GetFullPath(const wchar_t* apwcURL, DWORD aFlags = RES_LOCAL);
	virtual wchar_t* GetLastError();
	virtual bool SetModuleBase(const wchar_t* apwcModuleBase);
	virtual bool GetModuleBase(const wchar_t** apwcModuleBase);
	virtual int SetModuleID(const wchar_t* apwcModuleID, bool aSync = false, wchar_t *asScene = NULL, unsigned int *apErrorCode = NULL);
	virtual bool GetModuleIDAndReady(wchar_t* apwcBuffer, unsigned int auBufferSize, unsigned int* auOutSize, bool* abReady);
	//virtual bool SetUIBase(const wchar_t* apwcUIBase);
	virtual bool SetUIBase(const wchar_t* apwcUIBase){
		setUIBase((BSTR)apwcUIBase); return true;
	}
	virtual omsresult AddResourceAliasAndPath(const wchar_t* apwcAlias, const wchar_t* apwcOwnerModuleID, 
						const wchar_t* apwcResModuleID, unsigned int dwFlags = 0);
	virtual omsresult CreateModule(const wchar_t* apwcModuleID, const wchar_t* apwcAlias, bool aRemote=false, bool aAsynchMode=false, resIModuleNotify* apResIModuleNotify = NULL);
	virtual omsresult OpenModule(const wchar_t* apwcModuleID, const wchar_t* apwcAlias, bool aAsynchMode=false, resIModuleNotify* apResIModuleNotify = NULL);
//	virtual omsresult CreateModule(const wchar_t* apwcModuleID, const wchar_t* apwcAlias, bool aRemote=false);
//	virtual omsresult OpenModule(const wchar_t* apwcModuleID, const wchar_t* apwcAlias);
	virtual omsresult CloseModule(const wchar_t* apwcModuleID, const wchar_t* apwcAlias);
	virtual omsresult CopyResource(const wchar_t* apwcSrcURL, const wchar_t* apwcDestURL);
	virtual omsresult DownloadModule(const wchar_t* apwcModuleID);
	virtual const wchar_t* Extract(const wchar_t* apwcSrc, const wchar_t* apwcSubPath);
	virtual const wchar_t* GetTaxonomy(const wchar_t* apwcModuleID);
	//virtual const wchar_t** GetRBRList(const wchar_t* apwcModuleID);
	virtual const wchar_t* GetCacheBase();
	virtual void SetUserBase(const wchar_t* sDir);
	virtual void GetUserBase(const wchar_t** psDir);
	virtual void GetUIBase(const wchar_t** psDir);
	virtual void GetUISkinBase(const wchar_t** psDir);
	virtual void SetUISkinBase(const wchar_t* psDir);
	virtual void GetIResourceAndClose(resIResource* ares, LPVOID *appRes);
	virtual void InitResourceManager();
	virtual void DestroyResourceManager();
	virtual void SetILogWriter(ILogWriter *lw);
	virtual bool IsConnectionResServer();
	virtual bool GetModuleIDByAlias(const wchar_t* apwcAlias, const wchar_t* apwcOwnerModuleID, wchar_t **apModuleID, DWORD aSize);
	virtual int PackUpdateClientFilesAndGetInfo( const wchar_t* aVersion, wchar_t *apBufferForFileName, unsigned int auBufferWideCharsCount, unsigned int &aDataSize);
	virtual int PackClientSetup(wchar_t *apBufferForFileName, unsigned int auBufferWideCharsCount, unsigned int &aDataSize);
	virtual void WriteUpdaterLog(const char *logValue);

	virtual std::string GetSessionsInfoList();
	virtual void changeLogWriteEnabled(bool isLogEnabled);

	void Update();

	// res::resIResourceManager
protected:

	bool mbLogWriteEnabled;

	unsigned __int64 timeTicksAuthBegin;

	typedef	struct _resAliasDesc
	{
		MP_WSTRING msAlias;
		MP_WSTRING msOwnerModuleID;
		MP_WSTRING msPath;
		MP_WSTRING msModuleID;
		IResModule *spModule;

		_resAliasDesc()
			: MP_WSTRING_INIT(msAlias), MP_WSTRING_INIT(msOwnerModuleID),
			  MP_WSTRING_INIT(msPath), MP_WSTRING_INIT(msModuleID), spModule(NULL)
		{

		}
		//	_resAliasDesc(const wchar_t* apwcAlias, const wchar_t* apwcPath)
		//	{ 
		//		msAlias.insert(0,apwcAlias);
		//		msPath.insert(0,apwcPath);
		//	}
	} resAliasDesc;

	//CComQIPtr< IResMan> spResourceMan;
	MP_WSTRING wsModuleID;
	MP_WSTRING msModuleBase;
	//CComPtr< IResModule> spModule;
	IResModule *mspModule;
	MP_WSTRING msDefaultModuleBase;
	MP_WSTRING msUserBase;
	MP_WSTRING msUIBase;
	MP_WSTRING msUISkinBase;
	MP_WSTRING msLocalRepBase; // путь к локальному хранилищу модулей 
	// (который у DownloadManager-а в ini прописан
	//  и который по lr: добавляется)
	CWComString mbstrFullPath;
	CWComString mbstrFullPathC;
	CWComString mbstrTaxonomy;
	CWComString mbstrCacheBase;
	bool mbUsed;
	std::wstring RefineURL(const wchar_t* apwcURL, IResModule** aModule, DWORD dwFlags, unsigned int &aCodeError);
	MP_VECTOR<resAliasDesc>	mvResAliases;

	void setModuleBase(BSTR sDir){
		if(!mbUsed){
			msDefaultModuleBase=sDir;
			return;
		}
		if(sDir==NULL || *sDir==L'\0'){
			msModuleBase=msDefaultModuleBase;
		}else{
			msModuleBase=sDir;
		}
		if (msModuleBase.length() > 0)
		{
			wchar_t wcLastCh=msModuleBase[msModuleBase.length()-1];
			if(wcLastCh!=L'\\' && wcLastCh!=L'/')
				msModuleBase.insert(msModuleBase.end(),L'\\');
		}
	};

	void getModuleBase(BSTR* psDir){
		psDir=(BSTR*)msModuleBase.data();
	};
	/*void setUserBase(BSTR sDir){
		msUserBase=sDir;
	}*/
	/*void getUserBase(BSTR* psDir){
		psDir=(BSTR*)msUserBase.data();
	}*/
	void setUIBase(BSTR sDir){
		msUIBase=sDir;
		if(msUIBase.empty()) return;
		wchar_t wcLastCh=msUIBase[msUIBase.length()-1];
		if(wcLastCh!=L'\\' && wcLastCh!=L'/')
			msUIBase.insert(msUIBase.end(),L'\\');
	};

	/*void getUIBase(BSTR* psDir){
		psDir=(BSTR*)msUIBase.data();
	};*/

	void setUISkinBase(BSTR sDir){
		msUISkinBase=sDir;
		if(msUISkinBase.empty()) return;
		wchar_t wcLastCh=msUISkinBase[msUISkinBase.length()-1];
		if(wcLastCh!=L'\\' && wcLastCh!=L'/')
			msUISkinBase.insert(msUISkinBase.end(),L'\\');
	};

	/*void getUISkinBase(BSTR* psDir){
		psDir=(BSTR*)msUISkinBase.data();
	};*/
	void CloseAllModule();

	unsigned long m_recommendedPacketSize;

/*public:
	void CloseResource(resResource* apRes);*/

protected:
	void SetLastError(EQueryError error);
	CAsyncQuery* CreateAsyncQuery();
	void checkLocalBase();
	void openSession(CClientSession* pClientSession, CServerInfo *apServerInfo, bool aIsAsynchConnect);
	void openSessions( ESERVERSCONNECTIONGROUP iConnectedTo = SCG_ALL); // 0 ко всем, 1 - к ресурсному (по xml), 2 - к не ресурсным остальным
	void closeSession(CClientSession* pClientSession);
	void closeSessions();
	void registerSession(CClientSession* aClientSession, bool isFront = false);
	

	// Контекс
	oms::omsContext* pContext;
	//// Менеджер socket-соединения
	// Идентификатор лога
	CComString sLogID;
	// Менеджер ресурсов
	CResMan oResMan;
	// Менеджер асинхронных соединений
	//CAsyncMan oAsyncMan;
	// Менеджер HTTP-запросов
	CHTTP oHTTP;	
	CClientSession oResSocket;
	CClientSession oResXMLSocket;
	CClientSession oResUploadSocket;

	// Ключ в регистри, где хранятся настройки
	CComString sRegKey;
	// путь к ini файлу
	CWComString sPathIni;
	// Код последней ошибки
	long	m_lLastErrorCode;
	// Локальный репозиторий
	CLocalRepository* pRepository;

	// Объект сессии с сервером карты
	// Объект сессии с сервером синхронизации
	CClientSession syncServerSession;
	// Объект сессии с сервером сервисов
	CClientSession serviceServerSession;
	// Объект сессии с сервером шаринга
	//CClientSession sharingServerSession; - не должно так как не одна сессия
	// Объект сессии с сервером воспроизведения голосовой связи
	CClientSession voipPlayServerSession;
	// Объект сессии с сервером записи голосовой связи
	CClientSession voipRecordServerSession;
	// Объект сессии с сервером записи голосовой связи для записи своего голоса
	CClientSession voipSelfRecordingServerSession;
	// Объект сессии с сервером генерации превью
	CClientSession previewServerSession;
	// Объект сессии с UDP сервером
	CClientSession uDPServerSession;
	// Объект сессии с сервером мира
	CClientSession serviceWorldServerSession;

private:
	ILogWriter* m_pLW;

	CComString sLogin;

	CServerInfo voipServerInfo;
	CServerInfo voipRecordServerInfo;
	CServerInfo voipSelfRecordingServerInfo;
	CServerInfo voipPlayServerInfo;
	CServerInfo sharingServerInfo;
	CServerInfo resServerInfo;
	CServerInfo resXMLServerInfo;
	CServerInfo resUploadServerInfo;
	CServerInfo syncServerInfo;
	CServerInfo serviceServerInfo;
	CServerInfo previewServerInfo;
	CServerInfo worldServerInfo;

	CConnectionManager* m_pConnectionManager;

	int connectCount;

	CWComString msHost;
	CWComString msBuildType;

	//int iLogServerPort;
	
	unsigned long long lSessionID;
	CComString sSessionKey;

	TClientSessionList	m_sessions;
	CMutex				m_sessionsLock;
	CMutex mutex;

	cm::cmIComManCallbacks* pCallbacks;
	CRITICAL_SECTION csQuery;
	CComString mbstrQueryResult;

	DWORD QueryThread( CAsyncQuery* aQuery, bool abSync);
	static DWORD WINAPI QueryThread_( LPVOID aQuery);

	DWORD QueryThreadLogin( CAsyncQuery* aQuery);
	static DWORD WINAPI QueryThreadLogin_( LPVOID aQuery);

	DWORD CheckExistThread( CAsyncCheckExist* pAsyncCheckExist);
	static DWORD WINAPI CheckExistThread_( LPVOID aQuery);

	CRITICAL_SECTION csQueryData;

	typedef MP_VECTOR< CAsyncQuery*> CAsyncQueryVector;
	typedef CAsyncQueryVector::iterator  CAsyncQueryVectorIterator;
	CAsyncQueryVector vecQueries;
#if LOGSESSSIONINFO
	CSessionsInfoMan *pSessionsInfoMan;
#endif
};

class CAsyncCheckExist : IAsyncCheckExist {
private:
	resIAsynchResource* pHandler;
public:
	//int nQueryID;
	HANDLE hQueryThread;
	CComManager *pHost;
	MP_WSTRING wsFilePath;
	unsigned int dwFlags;
	bool bCheckOnlyExistInList;
	CMutex mutex;

	CAsyncCheckExist():
		MP_WSTRING_INIT( wsFilePath)
	{
		//nQueryID = -1;
		hQueryThread = NULL;
		pHost = NULL;
		pHandler = NULL;
		dwFlags = RES_REMOTE;
		bCheckOnlyExistInList = false;
	}
	~CAsyncCheckExist() {
		stop();
	}
	void stop() {
		if (hQueryThread != NULL) {
			CloseHandle( hQueryThread);
			hQueryThread = NULL;
		}
	}
	void destroy(){
		MP_DELETE_THIS;
	}
	void setHandler( resIAsynchResource* apHandler)
	{
		mutex.lock();
		pHandler = apHandler;
		mutex.unlock();
	}

	resIAsynchResource* getHandler()
	{
		return pHandler;
	}
};

using namespace std;
class res_streambuf;
class resResource: public resIWrite
{
	istream* mpIStream;
	iostream* mpIOStream;
	res_streambuf* mpStreamBuf;
	IResource* spResource;
	MP_WSTRING msSrc;
	ILogWriter *lw;
	//CComManager* mpResMan;
public:
	void GetIResourceAndClose( IResource** appRes);

	// OMS implementation
public:
	resResource(/*CComManager* apResMan, */IResource* apResource, const wchar_t* apwcSrc, ILogWriter *aLw):
							MP_WSTRING_INIT(msSrc)
	{
		//mpResMan = apResMan;
		spResource = apResource;
		mpStreamBuf = NULL;
		mpIStream = NULL;
		mpIOStream = NULL;
		if(apwcSrc != NULL)
			msSrc = apwcSrc;
		lw = aLw;
	}
	virtual	~resResource();
	omsresult GetCreateResult()
	{
		return OMS_OK;
	}
	// resIResource, resIWrite
public:
	omsresult GetIStream(istream* &apistrm);
	omsresult GetStream(oms::Stream* &apOMSStream){ if(apOMSStream == NULL) apOMSStream = NULL; return OMS_ERROR_NOT_IMPLEMENTED; }
	void Close();

	// resIWrite
public:
	omsresult GetIOStream(iostream* &apiostrm);

	// CSCL implementation
	unsigned int	Read( unsigned char* aBuffer, unsigned int aBufferSize, unsigned int *aError = 0);
	unsigned int	Write( unsigned char* aBuffer, unsigned int aBufferSize);
	bool	Seek( long aPos, unsigned char aFrom);
	unsigned long	GetPos();
	unsigned long	GetSize();
	unsigned long	GetCachedSize();
	__int64	GetTime();
	const wchar_t* GetURL();
	virtual bool GetFileNameRefineLoaded(wchar_t **wsName, const wchar_t *pCurName);
	virtual void	Download( resIAsynchResource* aAsynch, const wchar_t* apwcLocalPath);
	virtual void	DownloadToCache( resIAsynchResource* aAsynch);
	virtual bool	Unpack(resIAsynchResource* aAsynch=NULL);
	virtual bool	UnpackToCache(resIAsynchResource* aAsynch=NULL);
	virtual unsigned long	GetDownloadedSize();
	virtual unsigned int GetState();
	virtual unsigned int GetUnpackingProgress();
	virtual bool	Upload(resIResource* apSrcRes, resIAsynchResource* aAsynch=NULL);
	virtual unsigned long	GetUploadedSize();
	virtual bool	RegisterAtResServer( resIAsynchResource* aAsynch);
};

typedef char_traits<char> _Traits;

#define RES_STRM_BUF_SIZE 512
class res_streambuf: public streambuf{
	resIResource* mpOMSStream;
	char mpchBuf[RES_STRM_BUF_SIZE];
	char* mpCurPos;
	char* mpLastPos;
public:
	res_streambuf(resIResource* apOMSStream){
		mpOMSStream=apOMSStream;
		mpLastPos=mpCurPos=mpchBuf;
		ZeroMemory(mpchBuf, RES_STRM_BUF_SIZE);
		//		ReadNextChunk();
	}
	virtual ~res_streambuf(){
		if(mpLastPos==mpchBuf){ // writing
			overflow();
		}
	}
	virtual streamsize showmanyc(){
		if (!mpOMSStream)
			return 0;
		return mpOMSStream->GetSize();
	}
	void ReadNextChunk(){
		mpCurPos=mpchBuf;
		unsigned int errorRead = 0;
		int iRead=(int)mpOMSStream->Read((unsigned char*)mpchBuf, RES_STRM_BUF_SIZE, &errorRead);
		//{fstream fs("c:\\tmp2.tmp",ios_base::out);
		//fs.write(mpchBuf,iRead);
		//}
		mpLastPos=mpchBuf+iRead;
	}
	void Flush(){
		if(mpCurPos==mpchBuf) return;
		mpOMSStream->Write((unsigned char*)mpchBuf,mpCurPos-mpchBuf);
		mpCurPos=mpchBuf;
	}
	virtual int_type underflow(){	// get a character from stream, but don't point past it
		if(mpCurPos==mpLastPos)
			ReadNextChunk();
		if(mpLastPos==mpchBuf) return (_Traits::eof());
		return _Traits::to_int_type(*mpCurPos);
	}
	virtual int_type overflow(int_type ach = _Traits::eof()){
		//ATLTRACE("%c",ach);
		if(ach==_Traits::eof()){
			Flush();
			return (_Traits::eof()); 
		}
		if(mpCurPos==(mpchBuf+RES_STRM_BUF_SIZE-1)){
			Flush();
		}
		*mpCurPos=(char)ach; mpCurPos++;
		return ach; 
	}
	virtual int_type uflow(){	// get a character from stream, point past it
		if(mpCurPos==mpLastPos)
			ReadNextChunk();
		if(mpLastPos==mpchBuf) return (_Traits::eof());
		return _Traits::to_int_type(*mpCurPos++);
	}
	virtual pos_type seekoff(off_type off, ios_base::seekdir dir,
		ios_base::openmode = ios_base::in | ios_base::out){
			long retpos = 0;
			switch (dir) {
			case ios::beg :
				//				fdir = SEEK_SET;
				break;
			case ios::cur :
				//				fdir = SEEK_CUR;
				if(off==0)
					return ((streampos)mpOMSStream->GetPos());
				break;
			case ios::end :
				//				fdir = SEEK_END;
				break;
			default:
				// error
				return(EOF);
			}

			//		if (filebuf::sync()==EOF)
			//			return EOF;
			//		if ((retpos=_lseek(x_fd, off, fdir))==-1L)
			//			return (EOF);
			return((streampos)retpos);
			//		return (streampos(_BADOFF));
		}
};

class CAsyncQuery
{
public:
	int nQueryID;
	HANDLE hQueryThread;
	CComManager *pHost;
	CComString bsQuery;
	CComString bsParams;
	cm::cmIComManCallbacks* pHandler;

	CAsyncQuery() {
		nQueryID = -1;
		hQueryThread = NULL;
		pHost = NULL;
		pHandler = NULL;
	}
	~CAsyncQuery() {
		stop();
	}
	void stop() {
		if (hQueryThread != NULL) {
			CloseHandle( hQueryThread);
			hQueryThread = NULL;
		}
	}
};

#endif //__COMMANAGER_H_