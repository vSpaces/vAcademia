// ComManager.cpp : Implementation of CComManager

#include "stdafx.h"
#include "comman.h"
#include "ComManager.h"
#include "ResMan.h"
#include "ClientSession.h"
#include "UrlParser.h"
#include "shlobj.h"
#include "ThreadAffinity.h"
#include "../mdump/mdump.h"
#include "CrashHandlerInThread.h"
#include "Cscl3DWS.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define KN_CACHE	L"CacheBase"
#define KN_LOCAL	L"LocalBase"
#define KN_QUERY	L"QueryBase"
#define KN_REMOTE	L"RemoteBase"
#define KN_SERVER	L"Server"
#define KN_PORT		L"Port"
#define KN_HOST		L"domain"
#define KN_BUILDTYPE L"buildtype"

#define FL_RESMAN 1

#define CSCL_VERSION	OLESTR("2.4")	// 2.4	[06.01.07]

wchar_t glpcRootPath[MAX_PATH*2+2]= L"";

//using namespace res;
/////////////////////////////////////////////////////////////////////////////
// CComManager


CComManager::CComManager(omsContext* aContext) : oHTTP( aContext, NULL), pRepository( NULL), oResMan( &oHTTP), 
	oResSocket( "ResSocket", 0x1609), oResXMLSocket( "oResXMLSocket", 0x1609), oResUploadSocket( "oResUploadSocket", 0x9753)
	, syncServerSession("SyncServerSession", 0x4554), serviceServerSession("ServiceServerSession", 0x4554)
	, serviceWorldServerSession("ServiceWorldServerSession", 0x4774)
	, voipRecordServerSession("VoipRecordServerSession", 0x1619)
	, voipPlayServerSession("VoipPlayServerSession", 0x1619)
	, voipSelfRecordingServerSession("VoipSelfRecordingServerSession", 0x1619)
	, previewServerSession("PreviewServerSession", 0x2802)
	, uDPServerSession("UDPServerSession", 0x1111)
	, MP_LIST_INIT(m_sessions),
	MP_WSTRING_INIT( wsModuleID),
	MP_WSTRING_INIT( msModuleBase),
	MP_WSTRING_INIT( msDefaultModuleBase),
	MP_WSTRING_INIT( msUserBase),
	MP_WSTRING_INIT( msUIBase),
	MP_WSTRING_INIT( msUISkinBase),
	MP_WSTRING_INIT( msLocalRepBase),
	MP_VECTOR_INIT( mvResAliases),
	MP_VECTOR_INIT( vecQueries)
{
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	InitSecondaryThreadAffinityRoutine();
	mbLogWriteEnabled = false;
	pContext = aContext;

	oResMan.setContext(pContext);

	m_lLastErrorCode = 0;
	m_pLW = NULL;
	pCallbacks = NULL;

	InitializeCriticalSection( &csQuery);
	InitializeCriticalSection( &csQueryData);

	lSessionID = INCORRECT_SESSION_ID;
	oResMan.setResSocket( &oResSocket);
	oResMan.setResXMLSocket( &oResXMLSocket);
	oResMan.setResUploadSocket( &oResUploadSocket);

	mspModule = NULL;
	mbUsed = false;
	mbLogWriteEnabled = false;

	registerSession(&oResSocket);
	oResSocket.changeLogWriteEnabled(mbLogWriteEnabled, pContext->mpInfoMan);
	oResSocket.SetDetailedLogModeEnabled( true);
	registerSession(&oResXMLSocket);
	oResXMLSocket.changeLogWriteEnabled(mbLogWriteEnabled, pContext->mpInfoMan);
	registerSession(&oResUploadSocket);
	oResUploadSocket.changeLogWriteEnabled(mbLogWriteEnabled, pContext->mpInfoMan);

	serviceWorldServerSession.SetDetailedLogModeEnabled( true);

	msLocalRepBase=L"root\\files\\";

	voipServerInfo.sServerName = "VoipServer";
	voipRecordServerInfo.sServerName = "VoipRecordServer";
	voipPlayServerInfo.sServerName = "VoipPlayServer";
	resServerInfo.sServerName = "ResServer";
	resXMLServerInfo.sServerName = "ResXmlServer";
	resUploadServerInfo.sServerName = "ResUploadServer";
	syncServerInfo.sServerName = "SyncServer";
	serviceServerInfo.sServerName = "ServiceServer";
	previewServerInfo.sServerName = "PreviewServer";
	worldServerInfo.sServerName = "WorldServer";
	oResMan.LoadBigFileDll();


	m_pConnectionManager = MP_NEW(CConnectionManager);
	m_pConnectionManager->setContext(aContext);
	m_pConnectionManager->SubscribeHttpSettingsChanged(&oHTTP);
	connectCount = 0;
	m_recommendedPacketSize = 0;

#if LOGSESSSIONINFO
	MP_NEW_V( pSessionsInfoMan, CSessionsInfoMan, aContext);
	serviceWorldServerSession.SetSessionsInfoMan( 0, pSessionsInfoMan);
	serviceServerSession.SetSessionsInfoMan( 1, pSessionsInfoMan);
	oResSocket.SetSessionsInfoMan( 2, pSessionsInfoMan);
	oResXMLSocket.SetSessionsInfoMan( 3, pSessionsInfoMan);
	oResUploadSocket.SetSessionsInfoMan( 4, pSessionsInfoMan);
	syncServerSession.SetSessionsInfoMan( 5, pSessionsInfoMan);
	voipRecordServerSession.SetSessionsInfoMan( 6, pSessionsInfoMan);
	voipPlayServerSession.SetSessionsInfoMan( 7, pSessionsInfoMan);
	voipSelfRecordingServerSession.SetSessionsInfoMan( 8, pSessionsInfoMan);
	previewServerSession.SetSessionsInfoMan( 9, pSessionsInfoMan);
	uDPServerSession.SetSessionsInfoMan( 10, pSessionsInfoMan);
#endif
}

CComManager::~CComManager()
{	
	EnterCriticalSection( &csQuery);
	int size = vecQueries.size();
	for (int i = 0;  i < size;  i++) {
		if (vecQueries[ i] != NULL) {
			CAsyncQuery* pQuery = vecQueries[ i];
			MP_DELETE( pQuery);
		}
	}
	vecQueries.clear();
	LeaveCriticalSection( &csQuery);

	DeleteCriticalSection( &csQueryData);
	DeleteCriticalSection( &csQuery);

	mutex.lock();	// alex. ¬ параллельном потоке может идти добавление (см. ниже) и итератор сломаетс€
	std::vector<resAliasDesc>::iterator	mvit;
	//std::vector<std::wstring> moduleDeleted;
	for (mvit=mvResAliases.begin(); mvit!=mvResAliases.end(); mvit++)
	{
		if(mvit->spModule != NULL)
		{
			mvit->spModule->destroy();
			mvit->spModule = NULL;
		}
	}
	mvResAliases.clear();

	mutex.unlock();	//

	m_pConnectionManager->UnsubscribeHttpSettingsChanged(&oHTTP);
	m_pConnectionManager->PreDestroy();

	m_sessionsLock.lock();
	for (TClientSessionList::iterator it = m_sessions.begin(); it != m_sessions.end(); ++it)
	{
		((CClientSession*)*it)->PreDestroy();
	}
	m_sessions.clear();
	m_sessionsLock.unlock();

	//MP_DELETE(m_pConnectionManager); - #7359 - чтобы не падало при закрытии поток сессий на обращении к этому менеджеру (происходит только на закрытии)
	m_pConnectionManager = NULL;
#if LOGSESSSIONINFO
	MP_DELETE(pSessionsInfoMan);
	pSessionsInfoMan = NULL;
#endif
}

void CComManager::DestroyComman()
{
	MP_DELETE_THIS;
}

/*long CComManager::OnDraw(ATL_DRAWINFO& di)
{
	RECT& rc = *(RECT*)di.prcBounds;
	Rectangle(di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom);

	SetTextAlign(di.hdcDraw, TA_CENTER|TA_BASELINE);
	LPCTSTR pszText = _T("ATL 3.0 : ComManager");
	TextOut(di.hdcDraw, 
		(rc.left + rc.right) / 2, 
		(rc.top + rc.bottom) / 2, 
		pszText, 
		lstrlen(pszText));

	return S_OK;
}*/

////////////////////////////////////////////////////

/*long CComManager::get_cacheBase(CComString &pVal)
{
	return getCacheBase( pVal);
}*/

long CComManager::put_cacheBase(BSTR newVal)
{
	return setCacheBase( newVal);
}

long CComManager::get_repositoryBase(BSTR *pVal)
{
	std::wstring s;
	long res = getRepositoryBase( s);
	*pVal = (BSTR)s.c_str();
	return res;
	//return getRepositoryBase( pVal);
}

/*long CComManager::put_repositoryBase(BSTR newVal)
{
	return setRepositoryBase( newVal);
}*/

long CComManager::PutPathIni(wchar_t *newVal)
{
	sPathIni = newVal;
	if(setupResMan( newVal)!=S_OK)
		return E_FAIL;
	return S_OK;
}

////////////////////////////////////////////////////
// IComMan
EQueryError CComManager::sendQuery(BSTR aQuery, BSTR aParams, CComString &aResult)
{
	return oHTTP.sendQuery( aQuery, aParams, aResult);
}

EQueryError CComManager::sendQuerySync(BSTR aQuery, BSTR aParams, CComString &aResult)
{
	return oHTTP.sendQuerySync( aQuery, aParams, aResult);
}

EQueryError CComManager::sendQueryPost(BSTR aQuery, BSTR aParams, CComString &aResult)
{
	return oHTTP.sendQuery( aQuery, aParams, aResult, qtPost);
}

EQueryError CComManager::sendQueryData(BSTR aQuery, BSTR aParams, ULONG* aDataSize, BYTE** aData)
{
	return oHTTP.sendQueryData( aQuery, aParams, aDataSize, aData);
}

EQueryError CComManager::sendQueryDataSync(BSTR aQuery, BSTR aParams, ULONG* aDataSize, BYTE** aData)
{
	return oHTTP.sendQueryData( aQuery, aParams, aDataSize, aData, qtStandart, NULL, 0, false);
}

long CComManager::getSessionID( long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
	*pVal = oHTTP.get_sessionID();
	return S_OK;
}

void CComManager::openSession(CClientSession* pClientSession, CServerInfo *apServerInfo, bool aIsAsynchConnect)
{
	if(pClientSession!=NULL)
	{
		if(pClientSession->Open(apServerInfo, lSessionID, sSessionKey, m_pLW, m_pConnectionManager, aIsAsynchConnect))
		{
			if (m_pLW != NULL)
			{
				CLogValue log("openSession to ", apServerInfo->sServerName.GetBuffer(0), " with IP: ", (char*)apServerInfo->sIP.GetBuffer(), " and port=", (int)apServerInfo->tcpPort);
				m_pLW->WriteLnLPCSTR(log.GetData());
			}
		}
		else
		{
			if (m_pLW != NULL)
			{
				CLogValue log("ERROR: Cannot openSession to ", apServerInfo->sServerName.GetBuffer(0), " with IP: ", apServerInfo->sIP.GetBuffer(), " and port=", apServerInfo->tcpPort);
				m_pLW->WriteLnLPCSTR(log.GetData());
			}
		}

		registerSession(pClientSession, false);
	}
}

void CComManager::openSessions( ESERVERSCONNECTIONGROUP iConnectedTo)
{
	if ((iConnectedTo == SCG_ALL || iConnectedTo == SCG_RESOURCES) && oResMan.getTransferType() == RES_CONNECT_SOCKET )
	{
		openSession(&oResXMLSocket, &resXMLServerInfo, true);
		openSession(&oResSocket, &resServerInfo, true);
	}
	if( iConnectedTo == SCG_RESOURCES)
		return;

	if(oResMan.getTransferType() == RES_CONNECT_SOCKET)
	{
		openSession(&oResUploadSocket, &resUploadServerInfo, true);
	}
	openSession(&syncServerSession, &syncServerInfo, true);
	openSession(&serviceServerSession, &serviceServerInfo, true);
	openSession(&previewServerSession, &previewServerInfo, true);
}

void CComManager::closeSession(CClientSession* pClientSession)
{
	if(pClientSession!=NULL)
		pClientSession->Close();
}

void CComManager::closeSessions()
{
	if (oResMan.getTransferType() == RES_CONNECT_SOCKET )
	{
		closeSession(&oResSocket);
		closeSession(&oResXMLSocket);
		closeSession(&oResUploadSocket);
	}
	closeSession(&syncServerSession);
	closeSession(&serviceServerSession);
	closeSession(&previewServerSession);
}

void CComManager::CloseAllSessions()
{
	closeSessions();
	lSessionID = INCORRECT_SESSION_ID;
	serviceWorldServerSession.SetSessionID( 0);
	oHTTP.put_sessionID( 0);
}

void CComManager::registerSession(CClientSession* aClientSession, bool isFront)
{
	m_sessionsLock.lock();
	if (std::find(m_sessions.begin(), m_sessions.end(), aClientSession) == m_sessions.end())
	{
		if (isFront)
			m_sessions.push_front(aClientSession);
		else
			m_sessions.push_back(aClientSession);
	}
	m_sessionsLock.unlock();
}

long CComManager::setSessionIDAndLogSendType( long newVal, int aiTypeSendLog, CComString aSessionKey)
{
	if (m_pLW != NULL)
	{
		CLogValue log("sessionID = ", newVal);
		m_pLW->WriteLnLPCSTR(log.GetData());
	}
	SetSessionID( newVal);

	serviceWorldServerSession.SetSessionID( newVal);
	serviceWorldServerSession.SetSessionKey( aSessionKey);

	oHTTP.put_sessionID( newVal);

	lSessionID = newVal;	
	sSessionKey = aSessionKey;

	openSessions(SCG_RESOURCES);
	if(pContext->mpApp == NULL)
		return S_OK;

	pContext->mpApp->OnAuthorizated( aiTypeSendLog);

	openSessions(SCG_ALLEXCEPTRESOURCES);

	_FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	unsigned __int64 timeTicksAuthEnd = ((unsigned __int64)(ft.dwHighDateTime) << 32);
	timeTicksAuthEnd += ft.dwLowDateTime;
	unsigned __int64 delta = (timeTicksAuthEnd - timeTicksAuthBegin);
	if (m_pLW != NULL)
	{
		CLogValue log("Delta time of authorization and connect to servers in 100-nanosecond intervals = ", delta);
		m_pLW->WriteLnLPCSTR(log.GetData());
	}
	return S_OK;
}

long CComManager::getServer( BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
	CComString sVal( oHTTP.get_server());
	*pVal = sVal.AllocSysString();
	return S_OK;
}

long CComManager::setServer( wchar_t *newVal)
{
	if( newVal == NULL)
	{
		oHTTP.put_server( L"");
		worldServerInfo.sIP = "";
	}
	else
	{
		oHTTP.put_server( newVal);
		worldServerInfo.sIP = newVal;
	}

	CWComString sPath;
	GetEnvironmentVariableW(L"TEMP", sPath.GetBuffer( MAX_PATH2), MAX_PATH2+1);

	USES_CONVERSION;
	CWComString sIP = A2W( worldServerInfo.sIP.GetBuffer());
	CWComString sCacheBase = sPath + L"\\cache" + sIP + L"\\";
	oResMan.setCacheBase( sCacheBase);

	return S_OK;
}

const wchar_t *CComManager::GetHost()
{
	return msHost.GetBuffer();
}

long CComManager::setHost( wchar_t *newVal)
{
	msHost = newVal;
	return S_OK;
}

const wchar_t *CComManager::GetBuildType()
{
	return msBuildType.GetBuffer();
}

long CComManager::setBuildType( wchar_t *newVal)
{
	msBuildType = newVal;
	return S_OK;
}

unsigned short CComManager::setTcpPort( unsigned short newVal)
{
	worldServerInfo.tcpPort = newVal;
	return S_OK;
}
						 
long CComManager::getMaxCacheSize(DWORD *pVal)
{
	*pVal = oResMan.getMaxCacheSize();
	return S_OK;
}

long CComManager::setMaxCacheSize(DWORD newVal)
{
	oResMan.setMaxCacheSize(newVal);
	return S_OK;
}

long CComManager::setQueryBase( BSTR newVal)
{
	oHTTP.put_queryBase( newVal);
	return S_OK;
}

long CComManager::getProxy( BYTE* aProxyUsing,  BSTR* aProxy,  BSTR* aProxyUser,  BSTR* aProxyPassword)
{
	if (aProxyUsing == NULL || aProxy == NULL || aProxyUser == NULL || aProxyPassword == NULL)
		return E_POINTER;

	CComString sTemp;
	*aProxyUsing = (BYTE)oHTTP.get_proxyUsing();
	sTemp = oHTTP.get_proxy();
	*aProxy = sTemp.AllocSysString();
	sTemp = oHTTP.get_proxyUser();
	*aProxyUser = sTemp.AllocSysString();
	sTemp = oHTTP.get_proxyPassword();
	*aProxyPassword = sTemp.AllocSysString();
	return S_OK;
}

unsigned char CComManager::GetProxiesMask()
{
	unsigned char auMask = 0;

/*	if( m_proxyConfig.type == PT_SOCKS4)
		auMask |= PT_SOCKS4;
	else if( m_proxyConfig.type == PT_SOCKS4)
		auMask |= PT_SOCKS5;

	if( oHTTP.get_proxyUsing() != puDirect)
		auMask |= PT_HTTP;*/

	return auMask;
}

long CComManager::GetRecommendedPacketSize(/*int aThreadsCount*/)
{
	if (m_recommendedPacketSize == 0)
	{
		m_recommendedPacketSize = MIN_RES_PACKET_ZISE;
		RecalcRecommendedPacketSize( 0/*, aThreadsCount*/);
	}

	return m_recommendedPacketSize;
}

void CComManager::SetNetworkBreaksEmulationDelay( int auDelay, int auConnectionsMask)
{
	m_pConnectionManager->setNetworkBreaksEmulationDelay( auDelay, auConnectionsMask);
}

void CComManager::RecalcRecommendedPacketSize(unsigned long aBytesPerSecond/*, int aThreadsCount*/)
{
	// min packet size 8192, max - 65536
	if (aBytesPerSecond == 0)
		aBytesPerSecond = oResMan.getInitialSpeed();

	long packetSize = aBytesPerSecond / 2;

	long packetSizePow2 = MIN_RES_PACKET_ZISE;

	if (packetSize > MIN_RES_PACKET_ZISE)
	{
		while (packetSizePow2 < packetSize && packetSizePow2 <= MAX_RES_PACKET_ZISE)
		{
			packetSizePow2 <<= 1;
		}
		packetSizePow2 >>= 1;
	}

	if (m_pLW != NULL)
	{
		CLogValue log("recalcRecommendedPacketSize recommendedPacketSize = ", packetSizePow2);
		m_pLW->WriteLnLPCSTR(log.GetData());
	}

	m_recommendedPacketSize = packetSizePow2;
}

void CComManager::SetProxyLoginPass(const wchar_t * aProxyUser, const wchar_t * aProxyPassword, unsigned char aProxyType, const wchar_t * aProxySetting)
{	
	m_pConnectionManager->setProxyLoginPass(aProxyUser, aProxyPassword, aProxyType, aProxySetting);
}

bool CComManager::IsWinSocketConnection()
{	
	return m_pConnectionManager->isWinSocketConnection();
}

void CComManager::DropAndFreezeConnection( USHORT	signature)
{
	if( signature == syncServerSession.GetSignature())
		syncServerSession.DropAndFreeze();
	if( signature == serviceServerSession.GetSignature())
		serviceServerSession.DropAndFreeze();
	if( signature == voipPlayServerSession.GetSignature())
		voipPlayServerSession.DropAndFreeze();
	if( signature == voipRecordServerSession.GetSignature())
		voipRecordServerSession.DropAndFreeze();
	if( signature == voipSelfRecordingServerSession.GetSignature())
		voipSelfRecordingServerSession.DropAndFreeze();
	if( signature == previewServerSession.GetSignature())
		previewServerSession.DropAndFreeze();
	if( signature == uDPServerSession.GetSignature())
		uDPServerSession.DropAndFreeze();
	if( signature == serviceWorldServerSession.GetSignature())
		serviceWorldServerSession.DropAndFreeze();
	if( signature == oResSocket.GetSignature())
		oResSocket.DropAndFreeze();
	if( signature == oResXMLSocket.GetSignature())
		oResXMLSocket.DropAndFreeze();
	if( signature == oResUploadSocket.GetSignature())
		oResUploadSocket.DropAndFreeze();
}

void CComManager::UnfreezeConnection( USHORT	signature)
{
	if( signature == syncServerSession.GetSignature())
		syncServerSession.Unfreeze();
	if( signature == serviceServerSession.GetSignature())
		serviceServerSession.Unfreeze();
	if( signature == voipPlayServerSession.GetSignature())
		voipPlayServerSession.Unfreeze();
	if( signature == voipRecordServerSession.GetSignature())
		voipRecordServerSession.Unfreeze();
	if( signature == voipSelfRecordingServerSession.GetSignature())
		voipSelfRecordingServerSession.Unfreeze();
	if( signature == previewServerSession.GetSignature())
		previewServerSession.Unfreeze();
	if( signature == uDPServerSession.GetSignature())
		uDPServerSession.Unfreeze();
	if( signature == serviceWorldServerSession.GetSignature())
		serviceWorldServerSession.Unfreeze();
	if( signature == oResSocket.GetSignature())
		oResSocket.Unfreeze();
	if( signature == oResXMLSocket.GetSignature())
		oResXMLSocket.Unfreeze();
	if( signature == oResUploadSocket.GetSignature())
		oResUploadSocket.Unfreeze();
}

/*
USES_CONVERSION;

if (aProxyType == PT_HTTP)
{
ProxySettingsVec::iterator it = aProxySettingsList->begin();
ProxySettingsVec::iterator end = aProxySettingsList->end();
if (it != end)
{
char buf[6];
sprintf(&buf[0], "%u\0", it->port);
std::string proxy = W2A(it->ip.c_str());
proxy += ":";
proxy += buf;

oHTTP.put_proxyUsing( (EProxyUsing)it->enabled);
oHTTP.put_proxy( proxy.c_str());
oHTTP.put_proxyUser( W2A(it->user.c_str()));
oHTTP.put_proxyPassword( W2A(it->password.c_str()));
}
}
*/

void CComManager::ReloadUserSettings()
{
	if (m_pConnectionManager != NULL)
		m_pConnectionManager->ReloadSettings();
}

void CComManager::checkLocalBase()
{
	if( oResMan.getLocalBase()==NULL || oResMan.getLocalBase()[0]==0)
	{
		wchar_t lpcModuleFileName[MAX_PATH2+200];
		wchar_t lpcDllFullPathName[MAX_PATH2+200];
		LPWSTR	lpFileName;		
		::GetModuleFileNameW(NULL, lpcModuleFileName, MAX_PATH2+150);
		::GetFullPathNameW(lpcModuleFileName,MAX_PATH2+150,lpcDllFullPathName,&lpFileName);
		*lpFileName = 0;
		//oResMan.setLocalBase(CComBSTR(lpcDllFullPathName));
		SetLocalBase(lpcDllFullPathName);
	}
}

long CComManager::openModule(BSTR aResPath, DWORD aFlags, IResModule** aRes)
{
	checkLocalBase();
	return oResMan.openModule( aResPath, aFlags, aRes);
}

long CComManager::openModuleByPath(BSTR aModulePath, IResModule **aRes)
{
	checkLocalBase();
	return oResMan.openModuleByPath( aModulePath, aRes);
}

long CComManager::resourceExists( BSTR aResPath, DWORD aFlags, DWORD *aStatus, bool abCheckOnlyExistInList)
{
	if (aStatus == NULL)
	{
		if (m_pLW != NULL)
		{
			m_pLW->WriteLnLPCSTR(CLogValue("Test exist local res return E_POINTER").GetData());
		}
		return E_POINTER;
	}
	*aStatus = oResMan.resourceExists( aResPath, aFlags, abCheckOnlyExistInList);
	return S_OK;
}

long CComManager::getFullPath( BSTR aResPath, DWORD aFlags, CWComString &sFullPath)
{
	oResMan.getFullPath( aResPath, aFlags, sFullPath);
	return S_OK;
}

long CComManager::getRemoteBase( wchar_t **pVal)
{
	if (pVal == NULL)
		return E_POINTER;
	*pVal = (wchar_t *)oResMan.getRemoteBase();
	return S_OK;
}

long CComManager::setRemoteBase( wchar_t *newVal)
{
	oResMan.setRemoteBase( newVal);
	return S_OK;
}

long CComManager::getLocalBase( wchar_t **pVal)
{
	if (pVal == NULL)
		return E_POINTER;
	*pVal = (wchar_t *)oResMan.getLocalBase();
	return S_OK;
}

void CComManager::SetLocalBase( wchar_t *newVal)
{
	oResMan.setLocalBase( newVal);
}

long CComManager::getCacheBase( CWComString &pVal)
{
	pVal = oResMan.getCacheBase();
	return S_OK;
}

const wchar_t* CComManager::getCacheBase()
{
	return oResMan.getCacheBase();
}

long CComManager::setCacheBase( wchar_t *newVal)
{
	oResMan.setCacheBase( newVal);
	return S_OK;
}

long CComManager::getClientStringID(/*[out]*/ BSTR *pVal)
{
	//CComBSTR bsID = bsLogin;
	*pVal = sLogin.AllocSysString();
	return S_OK;
}

long CComManager::getConnectionStatus(ULONG* pStatus)
{
	ATLASSERT( pStatus);
	if( !pStatus)	return E_FAIL;
	*pStatus = GetConnectionStatus();
	return S_OK;
}

long CComManager::getRepositoryBase( std::wstring &pVal)
{	
	wchar_t *pw = (wchar_t *)oResMan.getRepositoryBase();
	pVal = pw;
	//MP_DELETE_ARR( wp);
	return S_OK;
}

void CComManager::SetRepositoryBase( wchar_t *newVal)
{
	oResMan.setRepositoryBase( newVal);
}

long CComManager::getUserBase( wchar_t **pVal)
{
	if (pVal == NULL)
		return E_POINTER;
	*pVal = (wchar_t *)oResMan.getUserBase();
	return S_OK;
}

#define	QueryResRegKeyPath(n, f)	\
lRes = RegQueryValueEx( hKey, ##n, 0, &dwType, NULL, &dwSize);	\
lRes = RegQueryValueEx( hKey, ##n, 0, &dwType,					\
				   (BYTE*)sValue.GetBuffer( dwSize), &dwSize);	\
if (lRes == ERROR_SUCCESS)										\
	oResMan.##f( sValue);

#define	QueryRegKeyPath(n, f)	\
lRes = RegQueryValueEx( hKey, ##n, 0, &dwType, NULL, &dwSize);	\
lRes = RegQueryValueEx( hKey, ##n, 0, &dwType,					\
				   (BYTE*)sValue.GetBuffer( dwSize), &dwSize);	\
if (lRes == ERROR_SUCCESS)										\
	##f( A2W(sValue.GetBuffer()));

#define	QueryIniKeyPath(n, f)	\
lRes = GetPrivateProfileStringW( L"paths", ##n, L"",\
					(LPWSTR)sValue.GetBuffer( MAX_PATH2), MAX_PATH2, sPathIni);	    \
					/*(LPSTR)sValue.GetBuffer( MAX_PATH2*2+2), MAX_PATH2, sPathIni);*/ \
if (lRes)																			\
	##f( sValue.GetBuffer());

#define	QueryIniKeySettings(n, f)	\
	lRes = GetPrivateProfileStringW( L"settings", ##n, L"",\
	(LPWSTR)sValue.GetBuffer( MAX_PATH2), MAX_PATH2, sPathIni);	    \
	if (lRes)\
##f( sValue.GetBuffer());

#define	QueryIniKeyPathUShort(n, f)	\
	lRes = GetPrivateProfileIntW( L"paths", ##n,\
	iValue, sPathIni);	    \
	if (lRes)																			\
		##f( (unsigned short)lRes);			

long CComManager::setupResMan( BSTR aRegKey)
{
	// „итаем из INI файла
	if (!sPathIni.IsEmpty())
	{
		LONG lRes;
		CWComString sValue;
		int iValue = 0;
		USES_CONVERSION;
		QueryIniKeyPath( KN_REMOTE, setRemoteBase);
		if(lRes==0)
		{
			setRemoteBase(L"vu/");
			//return E_FAIL;
		}
		QueryIniKeyPath( KN_QUERY, setQueryBase);
		if(lRes==0)
		{
			setQueryBase(L"vu/php_0_1/");	
			//return E_FAIL;
		}
		QueryIniKeyPath( KN_SERVER, setServer);
		if(lRes==0)
		{
			//setServer(L"178.63.80.136");
			setServer(L"virtuniver.loc");
			//return E_FAIL;
		}
		QueryIniKeyPathUShort( KN_PORT, setTcpPort);
		if(lRes==0)
		{
			setTcpPort(13333);
			//return E_FAIL;
		}
		QueryIniKeySettings( KN_HOST, setHost);
		if(lRes==0)
		{
			setHost( L"vacademia.com");
			//return E_FAIL;
		}
		QueryIniKeySettings( KN_BUILDTYPE, setBuildType);
		if(lRes==0)
		{
			setBuildType( L"base");
			//return E_FAIL;
		}
	}
	else
	{
		CComString sPath;
		GetEnvironmentVariable(_T("TEMP"), sPath.GetBuffer( MAX_PATH2), MAX_PATH2-1);
		CComString sBase;
		sBase.Format( "%s\\%S\\", (LPCTSTR)sPath, aRegKey);
		sPath = sBase;
		sBase.Format( "%s\\cache\\", (LPCTSTR)sPath);
		wchar_t *pw = sBase.AllocSysString();
		put_cacheBase( pw);
		
		sBase.Format( "%s\\vks\\", (LPCTSTR)sPath);
		pw = sBase.AllocSysString();
		SetLocalBase( pw);
	}
	return S_OK;
}

long CComManager::get_LastError(long *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_lLastErrorCode;
	return S_OK;
}

long CComManager::put_LastError(long newVal)
{
	// TODO: Add your implementation code here
	m_lLastErrorCode = newVal;
	return S_OK;
}

void CComManager::SetLastError(EQueryError error)
{
	switch(error) 
	{
	case qeServerError:
		m_lLastErrorCode = EC_SERVERERROR;
		break;
	case qeQueryNotFound:
		m_lLastErrorCode = EC_QUERYNOTERROR;
		break;
	case qeScriptError:
		m_lLastErrorCode = EC_SCRIPTERROR;
		break;
	case qeReadError:
		m_lLastErrorCode = EC_READERROR;
		break;
	case qeNotImplError:
		m_lLastErrorCode = EC_NOTIMPLERROR;
		break;
	default:
		m_lLastErrorCode = EC_NOERROR;
	}
}

typedef struct _UPLOADPARAMS 
{
	// Thread object
	CComManager* pComManager;
	// Upload file local name
	BSTR	sLocalUploadName;
	// Upload file server name
	BSTR	sServerUploadName;
} UPLOADPARAMS;


DWORD WINAPI uploadFileThread(LPVOID obj)
{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();
	UPLOADPARAMS*	params = (UPLOADPARAMS*)obj;
	DWORD	retCode = ((CComManager*)params->pComManager)->uploadFileProc(params->sLocalUploadName, params->sServerUploadName);
	//((CComManager*)params->pComManager)->Fire_OnUploaded(params->sLocalUploadName);
	MP_DELETE( params);
	return retCode;
}

long CComManager::uploadFile(BSTR localName, BSTR serverName, BOOL inThread)
{
	if( inThread)
	{
		UPLOADPARAMS*	params = MP_NEW(UPLOADPARAMS);
		params->pComManager = this;
		params->sLocalUploadName = localName;
		params->sServerUploadName = serverName;

		CSimpleThreadStarter playThreadStarter(__FUNCTION__);		
		playThreadStarter.SetRoutine(uploadFileThread);
		playThreadStarter.SetParameter(params);
		/*HANDLE hPlayThread = */playThreadStarter.Start();

		if (m_pLW != NULL)
		{
			CLogValue log("uploadFileThread thread created. ThreadID = ", playThreadStarter.GetThreadID());
			m_pLW->WriteLn(log);
		}
	}
	else
		uploadFileProc(localName, serverName);

	return S_OK;
}

UINT CComManager::uploadFileProc(BSTR localName, BSTR serverName)
{
	//IResMan*	spResMan;
	BOOL		writeError=FALSE;

	//spResMan = &oResMan;
	/*if( FAILED( QueryInterface(IID_IResMan, (void**)&spResMan)))
	{
		SetLastError(qeNotImplError);
		return 0;
	}*/

	IResource*	resLocal=NULL;
	IResource*	resRemote=NULL;
	IResource*	resLocalCopy=NULL;
	if( SUCCEEDED(openResource(localName, RES_LOCAL, &resLocal)) && resLocal != NULL)
	//if( SUCCEEDED(spResMan->openResource(localName, RES_LOCAL, &resLocal)))
	{
		DWORD dwStatus;
		resLocal->getStatus( &dwStatus);
		if ((dwStatus & RES_EXISTS) == 0)
		{
			if(resLocal!=NULL)
			{
				resLocal->destroy();
				resLocal = NULL;
			}
			SetLastError(qeReadError);
			return 0;
		}

		DWORD dwSize = 0, readed = 0;
		if( SUCCEEDED(resLocal->getSize(&dwSize)))
		{
			TCHAR*	buf = MP_NEW_ARR(TCHAR, dwSize+1);
			memset(buf, 0, dwSize+1);

			if( SUCCEEDED(resLocal->read((BYTE*)buf, dwSize, &readed)))
			{
				openResource(OLESTR("c:/copy.!!!"), RES_LOCAL|RES_CREATE|RES_TO_WRITE, &resLocalCopy);
				if( SUCCEEDED(openResource(serverName, RES_REMOTE|RES_CREATE|RES_TO_WRITE, &resRemote)))
				{
					DWORD dwStatus=0;
					if(SUCCEEDED(resRemote->getStatus(&dwStatus)) && dwStatus&RES_EXISTS)
					{
						//resRemote->write((BYTE*)buf, readed, &wrote);
						// Write cycle
						DWORD	dwWroteBytes=0;
						DWORD	dwRealWrote=0;
						DWORD	dwWriteBufSize = 0xFFFF;
						while(dwSize > 0)
						{
							if( dwWriteBufSize > dwSize)
								dwWriteBufSize = dwSize;
							if( SUCCEEDED(resRemote->write((BYTE*)((DWORD)buf + dwWroteBytes), dwWriteBufSize, &dwRealWrote)) &&
								dwRealWrote==dwWriteBufSize)
							{
								dwSize -= dwRealWrote;
								dwWroteBytes += dwRealWrote;
							}
							else	break;
						}
						writeError = (dwSize==0)?FALSE:TRUE;
					}
				}
				resRemote->destroy();
				resRemote = NULL;
			}
			MP_DELETE_ARR(buf);
		}
		if(resLocalCopy!=NULL)
		{
			resLocalCopy->destroy();
			resLocalCopy = NULL;
		}
	}
	if(resLocal!=NULL)
	{
		resLocal->destroy();
		resLocal = NULL;
	}

	if(writeError)
		SetLastError(qeServerError);
	return 0;
}

long CComManager::getModuleExists(BSTR aModuleID, DWORD aFlags, DWORD *aStatus, IResModule **ppModule)
{
	if (aStatus == NULL)
		return E_POINTER;
	*aStatus = oResMan.getModuleExists( aModuleID, aFlags, ppModule);
	return S_OK;
}

long CComManager::get_userBase(BSTR *pVal)
{
	return getUserBase( pVal);
}

long CComManager::writeStillOpened()
{
	if (oResMan.getOpenedResCount() > 0)
	{		
		oResMan.writeStillOpened();		
	}
	return S_OK;
}

long CComManager::importModule(BSTR aFileName, DWORD *aErrors)
{
	if (aErrors == NULL)
		return E_POINTER;

	*aErrors = oResMan.importModule( aFileName);

	return S_OK;
}

long CComManager::getResourceSize(BSTR aResPath, WORD aFlags, DWORD *aSize)
{
	if (aSize == NULL)
		return E_POINTER;
	*aSize = oResMan.getResourceSize( aResPath, aFlags);
	return S_OK;
}

void CComManager::CreateSyncServerSession(cs::iclientsession** ppClientSession)
{
	*ppClientSession = &syncServerSession;
	registerSession(&syncServerSession, true);
	syncServerSession.changeLogWriteEnabled(mbLogWriteEnabled, pContext->mpInfoMan);
}

void CComManager::CreateServiceServerSession(cs::iclientsession** ppClientSession)
{
	*ppClientSession = &serviceServerSession;
	registerSession(&serviceServerSession);
	serviceServerSession.changeLogWriteEnabled(mbLogWriteEnabled, pContext->mpInfoMan);
}

void CComManager::CreateServiceWorldServerSession(cs::iclientsession** ppClientSession)
{
	ATLASSERT( ppClientSession);
	*ppClientSession = NULL;

	connectCount++;
	if (m_pLW != NULL)
	{
		CLogValue log("CreateServiceWorldServerSession = ", connectCount);
		m_pLW->WriteLnLPCSTR(log.GetData());
	}

	// —сылка на сессию с сервером мира
	if(serviceWorldServerSession.GetConnectionStatus()==0)
	{
		lSessionID = SERVICE_WORLD_SESSION_ID;
		openSession(&serviceWorldServerSession, &worldServerInfo, false);
	}

	*ppClientSession = &serviceWorldServerSession;
	registerSession(&serviceWorldServerSession);
	serviceWorldServerSession.changeLogWriteEnabled(mbLogWriteEnabled, pContext->mpInfoMan);
}

void CComManager::CreateVoipSelfRecordingServerSession(cs::iclientsession** ppClientSession, cs::CServerInfo *apServerInfo)
{
	ATLASSERT( ppClientSession);
	*ppClientSession = NULL;

	// —сылка на сессию с голосовым сервером
	if ( voipSelfRecordingServerSession.GetConnectionStatus()==0)
	{
		cs::CServerInfo *pServerInfo = (cs::CServerInfo *)apServerInfo;
		voipSelfRecordingServerInfo = *pServerInfo;

		openSession( &voipSelfRecordingServerSession, &voipSelfRecordingServerInfo, false);
	}

	*ppClientSession = &voipSelfRecordingServerSession;
	registerSession(&voipSelfRecordingServerSession);
	voipSelfRecordingServerSession.changeLogWriteEnabled(mbLogWriteEnabled, pContext->mpInfoMan);
}

void CComManager::CreateVoipRecordServerSession(cs::iclientsession** ppClientSession, cs::CServerInfo *apServerInfo)
{
	ATLASSERT( ppClientSession);
	*ppClientSession = NULL;

	// —сылка на сессию с голосовым сервером
	if ( voipRecordServerSession.GetConnectionStatus()==0)
	{
		cs::CServerInfo *pServerInfo = (cs::CServerInfo *)apServerInfo;
		voipRecordServerInfo = *pServerInfo;

		openSession( &voipRecordServerSession, &voipRecordServerInfo, false);
	}

	*ppClientSession = &voipRecordServerSession;
	registerSession(&voipRecordServerSession);
	voipRecordServerSession.changeLogWriteEnabled(mbLogWriteEnabled, pContext->mpInfoMan);
}

void CComManager::CreateSharingServerSession(cs::iclientsession** ppClientSession, cs::CServerInfo *apServerInfo)
{
	ATLASSERT( ppClientSession);
	*ppClientSession = NULL;

	// —сылка на сессию с видео сервером
	CClientSession* pSharingServerSession = NULL;
	MP_NEW_V2( pSharingServerSession, CClientSession, "SharingServerSession", 0x2502);
#if LOGSESSSIONINFO
	pSharingServerSession->SetSessionsInfoMan( 11, pSessionsInfoMan);
#endif

	if ( pSharingServerSession->GetConnectionStatus()==0)
	{
		cs::CServerInfo *pServerInfo = (cs::CServerInfo *)apServerInfo;
		sharingServerInfo = *pServerInfo;

		openSession( pSharingServerSession, &sharingServerInfo, false);
	}

	*ppClientSession = pSharingServerSession;
	registerSession(pSharingServerSession);
	pSharingServerSession->changeLogWriteEnabled(mbLogWriteEnabled, pContext->mpInfoMan);
}

void CComManager::CreateVoipPlayServerSession(cs::iclientsession** ppClientSession, cs::CServerInfo *apServerInfo)
{
	ATLASSERT( ppClientSession);
	*ppClientSession = NULL;

	// —сылка на сессию с голосовым сервером
	if ( voipPlayServerSession.GetConnectionStatus()==0)
	{
		cs::CServerInfo *pServerInfo = (cs::CServerInfo *)apServerInfo;
		voipPlayServerInfo = *pServerInfo;

		openSession( &voipPlayServerSession, &voipPlayServerInfo, false);
	}

	*ppClientSession = &voipPlayServerSession;
	registerSession(&voipPlayServerSession);
	voipPlayServerSession.changeLogWriteEnabled(mbLogWriteEnabled, pContext->mpInfoMan);
}

void CComManager::CreatePreviewServerSession(cs::iclientsession** ppClientSession, cs::CServerInfo *apServerInfo)
{
	ATLASSERT( ppClientSession);
	*ppClientSession = NULL;

	cs::CServerInfo *pServerInfo = apServerInfo;
	// —сылка на сессию с сервером превью
	/*if ( pPreviewServerSession == NULL)
	{
		pPreviewServerSession = new CClientSession( "PreviewServerSession", 0x2802);
	}
	else
	{
		//if(previewServerInfo.sIP.CompareNoCase(reinterpret_cast<char*>(asServerIP)))
		if(previewServerInfo.sIP.CompareNoCase(pServerInfo->sIP))
		{
			pPreviewServerSession.Close();
		}
	}*/
	if ( previewServerSession.GetConnectionStatus() == 0 || previewServerInfo.sIP.CompareNoCase(pServerInfo->sIP))
	{
		previewServerInfo = *pServerInfo;
		openSession( &previewServerSession, &previewServerInfo, false);
	}

	/*if ( previewServerSession.GetConnectionStatus()==0)
	{
		long sessionID;
		getSessionID(&sessionID);
		if ( pPreviewServerSession.Open( &previewServerInfo, sessionID, m_pLW))
		{
			if ( m_pLW != NULL)
			{
				CLogValue log("Connected to PreviewServer with IP: ", previewServerInfo.sIP.GetBuffer(), " and port=", previewServerInfo.tcpPort);
				m_pLW->WriteLnLPCSTR(log.GetData());
			}
		}
		else
		{
			if ( m_pLW != NULL)
			{
				CLogValue log("Cannot connect to PreviewServer with IP: ", previewServerInfo.sIP.GetBuffer(), " and port=", previewServerInfo.tcpPort);
				m_pLW->WriteLnLPCSTR(log.GetData());
			}
		}
	}*/

	*ppClientSession = &previewServerSession;
	registerSession(&previewServerSession);
	previewServerSession.changeLogWriteEnabled(mbLogWriteEnabled, pContext->mpInfoMan);
}

void CComManager::DestroyServerSession(cs::iclientsession** ppClientSession)
{
	if (ppClientSession == NULL)
		return;
	if (*ppClientSession == NULL)
		return;
	(*ppClientSession)->SetMessageHandler( NULL);
	(*ppClientSession)->SetSessionListener( NULL);
	CClientSession* pClientSession = (CClientSession*)(*ppClientSession);
	pClientSession->Close();
	*ppClientSession = NULL;
}

/*
 оды возвращаемых ошибок:
¬озвращаемые значени€ - бинарное, целое со знаком:
> 0 - номер сессии
-1000001   - неправильный логин
-1000002   - неправильный пароль
-1999999 - незаданы либо логин, либо пароль
-2000000 - неверный sql запрос
//
ќшибки, определ€емые на стороне клиента:
12535 == 0x30F7 - сигнатура 
-1000003   - несовпадает сигнатура ответа сервера
*/
#define	LOGIN_SIGNATURE	0x30F7
#define	USE_SIGNATURE

void CComManager::setServerInfo( CServerInfo *pServerInfo, unsigned char *&ipElement, unsigned char *&pEnd)
{
	unsigned char caServerArrIP[4];
	for (int i = 0; i < 4; i++)
	{
		if(ipElement >= pEnd)
			break;
		caServerArrIP[i] = *(ipElement++);
	}
	if(ipElement >= pEnd)
		return;
	unsigned short iServerPort = *(unsigned short*)ipElement;
	ipElement += 2;
	unsigned short iServerUDPRecvPort = 0;
	unsigned short iServerUDPSendPort = 0;
	/*if(bUDPUse)
	{*/
	iServerUDPRecvPort = *(unsigned short*)ipElement;
	ipElement += 2;
	iServerUDPSendPort = *(unsigned short*)ipElement;
	ipElement += 2;
	//}
	pServerInfo->tcpPort = iServerPort;
	pServerInfo->udpRecvPort = iServerUDPRecvPort;
	pServerInfo->udpSendPort = iServerUDPSendPort;
	char serverIP[20];
	sprintf_s(&serverIP[0], 20, "%d.%d.%d.%d", caServerArrIP[0], caServerArrIP[1], caServerArrIP[2], caServerArrIP[3]);
	pServerInfo->sIP = serverIP;
}

void CComManager::setServerAdditionalInfo( CServerInfo *pServerInfo, unsigned char *&ipElement, unsigned char *&pEnd)
{
	unsigned short iStrLen = *(unsigned short*)ipElement;
	ipElement+=2;
	if(iStrLen==0 || ipElement >= pEnd)
		return;
	rtl_strncpy( pServerInfo->sServerName.GetBufferSetLength(iStrLen), iStrLen + 1,
					(const char *)ipElement, iStrLen);
	ipElement+=iStrLen;
	if(ipElement >= pEnd)
		return;
	iStrLen = *(unsigned short*)ipElement;
	ipElement+=2;
	if(iStrLen==0 || ipElement >= pEnd)
		return;
	rtl_strncpy( pServerInfo->sBuild.GetBufferSetLength(iStrLen), iStrLen + 1,
					(const char *)ipElement, iStrLen);
	ipElement+=iStrLen;
}

long CComManager::setLoginData(DWORD aDataSize, BYTE* aData, int* aError)
{
	int iQueryRes = qeNoError;
	if (aData != NULL && aDataSize >= sizeof( long))
	{
		long lSignature = (*(long*)aData);
		bool bCheckSignature = false;
#ifdef USE_SIGNATURE
		bCheckSignature = true;
// дл€ старой логинации города сигнатуру не провер€ем по любому
if(aDataSize == 28)
bCheckSignature = false;
#endif
		long lSignLength = bCheckSignature?4:0;
		if( bCheckSignature && lSignature != LOGIN_SIGNATURE)
		{
			switch(lSignature){
			case -1:
			case -2:
			case -3:
			case -4:
			case -5:
			case -6:
			case -7:
				iQueryRes = lSignature;
				break;
			default:
				iQueryRes = qeScriptError;
			}
		}
		else
		{
			long lSessionID1 = (*(long*)(aData + lSignLength));
			long lSessionLength = 4;
			if (lSessionID1 > 0)
			{
				unsigned char* ipElement = ((unsigned char*)(aData) + lSignLength + lSessionLength);
				unsigned char* pEnd = aData + aDataSize;
		
				int iFormat = 0; // 0 - старый формат, 1 - новый формат
				if(ipElement < pEnd){
					if(*ipElement == 255){
						iFormat = 1;
						ipElement++;
					}
				}

				//if(iFormat == 1){
				int iIPCount = 0;
				if(ipElement < pEnd){
					iIPCount = *(ipElement++);
				}
				for(int ii = 0; ii < iIPCount; ii++)
				{
					if(ipElement >= pEnd)
						break;
					int iType = (int)(*ipElement++);
					
					switch(iType){
					case 1: // MapServer
						ATLASSERT( FALSE);	//setServerInfo(&mapServerInfo, ipElement, pEnd);
						break;
					case 2: // ResServer
						setServerInfo(&resServerInfo, ipElement, pEnd);
						break;
					case 3: // ResXMLServer
						setServerInfo(&resXMLServerInfo, ipElement, pEnd);
						break;
					case 4: // SyncServer	
						setServerInfo(&syncServerInfo, ipElement, pEnd);
						break;
					case 5: // ServiceServer
						setServerInfo(&serviceServerInfo, ipElement, pEnd);
						break;
					case 6: // UploaderServer
						setServerInfo(&resUploadServerInfo, ipElement, pEnd);
						break;
					}
				}

				if(ipElement < pEnd)
				{
					iIPCount = *(ipElement);
					if(ipElement < pEnd)
						iIPCount = *(ipElement++);
					for(int ii = 0; ii < iIPCount; ii++)
					{
						if(ipElement >= pEnd)
							break;
						int iType = (int)(*ipElement++);

						switch(iType){
						case 1: // MapServer
							ATLASSERT( FALSE);	//setServerAdditionalInfo(&mapServerInfo, ipElement, pEnd);
							break;
						case 2: // ResServer
							setServerAdditionalInfo(&resServerInfo, ipElement, pEnd);
							break;
						case 3: // ResXMLServer
							setServerAdditionalInfo(&resXMLServerInfo, ipElement, pEnd);
							break;
						case 4: // SyncServer
							setServerAdditionalInfo(&syncServerInfo, ipElement, pEnd);
							break;
						case 5: // ServiceServer
							setServerAdditionalInfo(&serviceServerInfo, ipElement, pEnd);
							break;
						case 6: // UploaderServer
							setServerAdditionalInfo(&resUploadServerInfo, ipElement, pEnd);
							break;
						}
					}
				}

				int iTypeSendLog = (*(int*)(ipElement));
				ipElement += sizeof(int);
				if(ipElement < pEnd)
				{
					unsigned short iStrLen = *((unsigned short*)ipElement);
					ipElement += sizeof(unsigned short);
					if(ipElement < pEnd && iStrLen > 0)
					{						
						rtl_strncpy(sLogin.GetBufferSetLength(iStrLen), iStrLen + 1,
							(const char *)ipElement, iStrLen);
						ipElement += iStrLen;
					}
				}
				CComString sSessionKey1;
			//	ipElement += sizeof(int);
				if(ipElement < pEnd)
				{
					unsigned short iStrLen = *((unsigned short*)ipElement);
					ipElement += sizeof(unsigned short);
					if(ipElement < pEnd && iStrLen > 0)
					{						
						rtl_strncpy(sSessionKey1.GetBufferSetLength(iStrLen), iStrLen + 1,
							(const char *)ipElement, iStrLen);
						ipElement += iStrLen;
					}
				}

				setSessionIDAndLogSendType( lSessionID1, iTypeSendLog, sSessionKey1);
			}
			else	// if (lSessionID > 0)
			{
				iQueryRes = lSessionID1;
			}
		}
	}
	else
		iQueryRes = qeScriptError;
	*aError = iQueryRes;
	return S_OK;
}

int CComManager::SetLoginData(unsigned int aDataSize, const wchar_t* aData)
{
	int iError = 0;
	setLoginData(aDataSize, (BYTE*)aData, &iError);
	return iError;
}

bool CComManager::SetCallbacks(cm::cmIComManCallbacks* apCallbacks)
{
	EnterCriticalSection( &csQuery);
	pCallbacks = apCallbacks;
	LeaveCriticalSection( &csQuery);
	return false;
}

const wchar_t* CComManager::Query(const wchar_t* apwcQuery, const wchar_t* apwcParams, int* apiErrorCode){
	//CComVariant vResult;
	CComString vResult;
	EQueryError err = sendQuery((BSTR)apwcQuery, (BSTR)apwcParams, vResult);
	if( apiErrorCode != NULL)
		(*apiErrorCode) = (int)err;
	mbstrQueryResult = vResult;
	return mbstrQueryResult.AllocSysString();
}

const wchar_t* CComManager::QueryPost(const wchar_t* apwcQuery, const wchar_t* apwcParams, int* apiErrorCode){
	CComString vResult;
	EQueryError err = sendQueryPost((BSTR)apwcQuery, (BSTR)apwcParams, vResult);
	if( apiErrorCode != NULL)
		(*apiErrorCode) = (int)err;
	if(vResult == NULL)
		return NULL;
	mbstrQueryResult = vResult;
	return mbstrQueryResult.AllocSysString();
}

unsigned char* CComManager::QueryData(const wchar_t* apwcQuery, const wchar_t* apwcParams, 
								unsigned long* aDataSize, unsigned char** aData, int* aiErrorCode)
{
	if(!aDataSize) return NULL;
	if(!aData) return NULL;
	CComVariant vResult;
	EQueryError hr = sendQueryData((BSTR)apwcQuery, (BSTR)apwcParams, aDataSize, aData);
	if( aiErrorCode)
	{
		*aiErrorCode = (int)hr;
	}
	return *aData;
}

unsigned char* CComManager::queryDataSync(const wchar_t* apwcQuery, const wchar_t* apwcParams, 
									  unsigned long* aDataSize, unsigned char** aData, int* aiErrorCode)
{
	if(!aDataSize) return NULL;
	if(!aData) return NULL;
	CComVariant vResult;
	EQueryError errorCode = sendQueryDataSync((BSTR)apwcQuery, (BSTR)apwcParams, aDataSize, aData);
	if( aiErrorCode)
	{
		*aiErrorCode = (int)errorCode;
	}
	return *aData;
}

int CComManager::SendQuery(const wchar_t* apwcQuery, const wchar_t* apwcParams){
	return SendQuery( apwcQuery, apwcParams, pCallbacks);
}

CAsyncQuery* CComManager::CreateAsyncQuery()
{
	int nQueryID = -1;
	EnterCriticalSection( &csQueryData);
	CAsyncQuery* pQueryData = MP_NEW( CAsyncQuery);
	/*int i = 0;
	for (i = 0;  i < vecQueries.size();  i++)
	{
		if (vecQueries[ i] == NULL)
		{
			vecQueries[ i] = pQueryData;
			break;
		}
	}*/
	int i = vecQueries.size();	
	vecQueries.push_back( pQueryData);
	nQueryID = i;
	pQueryData->nQueryID = nQueryID;
	pQueryData->pHost = this;
	LeaveCriticalSection( &csQueryData);

	return pQueryData;
}

//************************************
// Method:    SendQuery
// ћетод должен вернуть свободный номер, закрепленный за запросом
//************************************
int CComManager::SendQuery( const wchar_t* apwcQuery, const wchar_t* apwcParams
						   , cm::cmIComManCallbacks* aHandler)
{
	CAsyncQuery* pQuery = CreateAsyncQuery();

	pQuery->bsQuery = apwcQuery;
	pQuery->bsParams = (BSTR)(apwcParams != NULL ? apwcParams : L"");
	pQuery->pHandler = aHandler;
	
	CSimpleThreadStarter queryThreadStarter(__FUNCTION__);	
	queryThreadStarter.SetRoutine(QueryThread_);
	queryThreadStarter.SetParameter((LPVOID)pQuery);
	pQuery->hQueryThread = queryThreadStarter.Start();

	if (m_pLW != NULL)
	{
		CLogValue log("CComManager::QueryThread_ thread created. ThreadID = ", queryThreadStarter.GetThreadID());
		m_pLW->WriteLn(log);
	}
	
	return pQuery->nQueryID;
}

DWORD WINAPI CComManager::QueryThread_( LPVOID aQuery) {
	CoInitialize( NULL);
	CAsyncQuery* pQuery = (CAsyncQuery*)aQuery;
	if (pQuery != NULL && pQuery->pHost != NULL)
		pQuery->pHost->QueryThread( pQuery, true);
	CoUninitialize();
	if( pQuery != NULL)
	MP_DELETE( pQuery);
	return 0;
}

DWORD CComManager::QueryThread( CAsyncQuery* aQuery, bool abSync) {
	CComString vResult;
	EQueryError errorCode = qeNoError;
	if( abSync)
	{
		errorCode = sendQuerySync(aQuery->bsQuery.AllocSysString(), aQuery->bsParams.AllocSysString(), vResult);
	}
	else
		errorCode = sendQuery( aQuery->bsQuery.AllocSysString(), aQuery->bsParams.AllocSysString(), vResult);
	if (aQuery->pHandler != NULL)
	{
		int iErrorCode = (int)errorCode;
		aQuery->pHandler->onQueryComplete( aQuery->nQueryID, vResult.AllocSysString(), &iErrorCode);
	}

	EnterCriticalSection( &csQuery);
	CloseHandle( aQuery->hQueryThread);
	aQuery->hQueryThread = NULL;
	int size = vecQueries.size();
	if (aQuery->nQueryID >= 0 && aQuery->nQueryID < size)
		vecQueries[ aQuery->nQueryID] = NULL;
	LeaveCriticalSection( &csQuery);

	return 0;
}

void CComManager::LogLoginBeginTime()
{
	_FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	timeTicksAuthBegin = ((unsigned long long)ft.dwHighDateTime) << 32;
	timeTicksAuthBegin += ft.dwLowDateTime;
}

const std::wstring CComManager::GetMyIP()
{
	CComString shost;
	WORD wVersionRequested;
	WSADATA wsaData;
	PHOSTENT hostinfo;
	CComString	ip;
	wVersionRequested = MAKEWORD( 2, 0 );

	if ( WSAStartup( wVersionRequested, &wsaData ) == 0 )
	{
		if((hostinfo = gethostbyname("")) != NULL)
		{
			ip = inet_ntoa (*(struct in_addr *)hostinfo->h_addr_list[0]);
		}

		WSACleanup( );
	}
	if( ip.IsEmpty())	
		ip = "";

	USES_CONVERSION;
	std::wstring wIP = A2W(ip);
	return wIP;
}

const wchar_t* CComManager::GetClientStringID()
{
	return sLogin.AllocSysString();
}

bool CComManager::ConnectToWorld(const wchar_t* apwcURL)
{
	// «аглушка bdima (07.03.2008)
	// Ќужно передать URL на сервер ћира
	// ѕолучить новые ip-адреса серверов и новые координаты аватара
	// ѕереконектить сервера
	// ѕереместить аватара
	
	//сейчас делаем только последние
	if (pContext == NULL)
		return false;
	CUrlParser url( apwcURL);
	cm::cmTeleportInfo oTeleport;
	oTeleport.x = (float)wcstod( url.GetParam( L"x", L"0"), NULL);
	oTeleport.y = (float)wcstod( url.GetParam( L"y", L"0"), NULL);
	oTeleport.z = (float)wcstod( url.GetParam( L"z", L"0"), NULL);
	oTeleport.rx = (float)wcstod( url.GetParam( L"rx", L"0"), NULL);
	oTeleport.ry = (float)wcstod( url.GetParam( L"ry", L"0"), NULL);
	oTeleport.rz = (float)wcstod( url.GetParam( L"rz", L"1"), NULL); // »справление #198. Ѕыло 0
	oTeleport.ra = (float)wcstod( url.GetParam( L"ra", L"0"), NULL);
	oTeleport.uRealityID = wcstoul( url.GetParam( L"rn", L"0"), NULL, 10);

	float sin_a = (float)sin( oTeleport.ra*M_PI / 360 );
	float cos_a = (float)cos( oTeleport.ra*M_PI / 360 );

	oTeleport.rx *= sin_a;
	oTeleport.ry *= sin_a;
	oTeleport.rz *= sin_a;
	oTeleport.ra = cos_a;
	float norm = (float)sqrt(oTeleport.rx*oTeleport.rx+oTeleport.ry*oTeleport.ry+oTeleport.rz*oTeleport.rz+oTeleport.ra*oTeleport.ra);
	if (fabs(norm) > 0.0f)
	{
		oTeleport.rx /= norm;
		oTeleport.ry /= norm;
		oTeleport.rz /= norm;
		oTeleport.ra /= norm;
	}
	
	pContext->mpMapMan->MoveAvatar( oTeleport);

	return true;
}

unsigned long long CComManager::GetSessionID(){
	return lSessionID;
}

// ”станавливаютс€ биты соединени€ с серверами:
//
// 1 - сервер логинации
// 2 - сервер ресурсов
// 4 - сервер лога
// 8 - сервер карты
// 16 - сервер синхронизации
//
//#define	LOGIN_SERVER_STATUS_BIT		0x1
#define	RES_SERVER_STATUS_BIT		0x2
#define	LOG_SERVER_STATUS_BIT		0x4
#define	MAP_SERVER_STATUS_BIT		0x8
#define	SYNC_SERVER_STATUS_BIT		0x10
#define	SERVICE_SERVER_STATUS_BIT	0x20
#define	VOIP_RECORD_SERVER_STATUS_BIT	0x40
#define	VOIP_PLAY_SERVER_STATUS_BIT		0x80
#define	SERVICE_WORLD_SERVER_STATUS_BIT	0x1
#define	VOIP_SELF_RECORDING_SERVER_STATUS_BIT	0x100

unsigned int CComManager::GetConnectionStatus()
{
	// ??
	unsigned int status = 0;

	/*if( oHTTP.get_sessionID() > 0)
		status |= LOGIN_SERVER_STATUS_BIT;*/

	if (oResMan.getTransferType() == RES_CONNECT_SOCKET)
	{		
		//if( oResSocket.GetConnectionStatus() == CClientSession::STATUS_CONNECTED)
		if( oResSocket.GetConnectionStatus() == CClientSession::STATUS_CONNECTED
			&& oResXMLSocket.GetConnectionStatus() == CClientSession::STATUS_CONNECTED)
			status |= RES_SERVER_STATUS_BIT;
	}
	else
	{
		// если ресурсы т€нем по http, то пока их можно т€нуть только с того же сервера что и логинаци€.
		// ѕоэтому считаем что если номер сессии установлен, то с ресурсным сервером соединились
		if( oHTTP.get_sessionID() > 0)
			status |= RES_SERVER_STATUS_BIT;
	}

/*	if( oLogSocket.getConnectionStatus() != LOG_CONNECTION_FAILED)
		status |= LOG_SERVER_STATUS_BIT;*/

	if( syncServerSession.GetConnectionStatus() == CClientSession::STATUS_CONNECTED)
	{
		status |= SYNC_SERVER_STATUS_BIT;
		status |= MAP_SERVER_STATUS_BIT;
	}

	if( serviceServerSession.GetConnectionStatus() == CClientSession::STATUS_CONNECTED)
	{
		status |= SERVICE_SERVER_STATUS_BIT;
	}

	if( serviceWorldServerSession.GetConnectionStatus() == CClientSession::STATUS_CONNECTED)
	{
		status |= SERVICE_WORLD_SERVER_STATUS_BIT;
	}

	if(	voipRecordServerSession.GetConnectionStatus() == CClientSession::STATUS_CONNECTED)
	{
		status |= VOIP_RECORD_SERVER_STATUS_BIT;
	}

	if(	voipSelfRecordingServerSession.GetConnectionStatus() == CClientSession::STATUS_CONNECTED)
	{
		status |= VOIP_SELF_RECORDING_SERVER_STATUS_BIT;
	}

	if( voipPlayServerSession.GetConnectionStatus() == CClientSession::STATUS_CONNECTED)
	{
		status |= VOIP_PLAY_SERVER_STATUS_BIT;
	}

	return status; // not implemented
}

const wchar_t* CComManager::GetServerName(){
	// ??
	return L"";
}

const wchar_t* CComManager::GetServerIP(){
	// ??
	return L"";
}

void CComManager::SetILogWriter(ILogWriter *lw)
{
	m_pLW = lw;
	oHTTP.SetILogWriter(lw);
	oResMan.SetILogWriter(lw);
	m_pConnectionManager->setILogWriter(lw);
}

std::string ConvertFloatToString(float number){
	char buffer [50];
	int n;
	n=sprintf_s (buffer, 50, "% 15.2f", number);
	return buffer;
}

std::string ConvertIntToString(int number){
	std::stringstream buff;
	buff << number;
	return buff.str();
}

std::string fillInfo(std::string name, CClientSession* pSession, int sendQueueSize, float receiveSpeed, float middleReceiveSpeed, float receivedSize, float sendSpeed, float middleSendSpeed, float sendedSize)
{
	std::string info;
	if (name != "" && pSession != NULL)
	{
		info += name;
		info += ": ";
		info += (pSession->GetStatus() == (int)pSession->STATUS_CONNECTED) ? "connected" : "not connected";
	}

	info += "\t";
	info += "sendQueueSize=";
	info += ConvertIntToString(sendQueueSize);
	info += "\r\n";

	info += "IN:\t";
	info += "speed=";
	info += ConvertFloatToString(receiveSpeed);
	info += "\t\t";
	info += "middleSpeed=";
	info += ConvertFloatToString(middleReceiveSpeed);
	info += "\t\t";
	info += "total=";
	info += ConvertFloatToString(receivedSize);
	info += "\r\n";

	info += "OUT:\t";
	info += "speed=";
	info += ConvertFloatToString(sendSpeed);
	info += "\t\t";
	info += "middleSpeed=";
	info += ConvertFloatToString(middleSendSpeed);
	info += "\t\t";
	info += "total=";
	info += ConvertFloatToString(sendedSize);
	info += "\r\n\r\n";
	return info;
}

void CComManager::changeLogWriteEnabled(bool isLogEnabled)
{
	mbLogWriteEnabled = isLogEnabled;

	m_sessionsLock.lock();
	TClientSessionList::iterator it = m_sessions.begin();
	TClientSessionList::iterator end = m_sessions.end();

	for ( ; it != end; ++it)
	{
		CClientSession* pSession = (CClientSession*)(*it);
		pSession->changeLogWriteEnabled(isLogEnabled, pContext->mpInfoMan);
	}
	m_sessionsLock.unlock();
}

std::string CComManager::GetSessionsInfoList()
{
	float totalSendSpeed = 0;
	float totalReceiveSpeed = 0;

	float totalMiddleSendSpeed = 0;
	float totalMiddleReceiveSpeed = 0;

	float totalSended = 0;
	float totalReceived = 0;

	int totalSendQueueSize = 0;

	int count = 0;

	std::string info;

	m_sessionsLock.lock();
	TClientSessionList::iterator it = m_sessions.begin();
	TClientSessionList::iterator end = m_sessions.end();
	for ( ; it != end; ++it)
	{
		CClientSession* pSession = (CClientSession*)(*it);

		float receiveSpeed = pSession->GetReceiveSpeed();
		float middleReceiveSpeed = pSession->GetMiddleReceiveSpeed();
		float receivedSize = pSession->GetReceivedSize();

		float sendSpeed = pSession->GetSendSpeed();
		float middleSendSpeed = pSession->GetMiddleSendSpeed();
		float sendedSize = pSession->GetSendedSize();

		int sendQueueSize = pSession->GetSendQueueSize();
		totalSendQueueSize += sendQueueSize;
		info += fillInfo(pSession->GetSessionName(), pSession, sendQueueSize, receiveSpeed, middleReceiveSpeed, receivedSize, sendSpeed, middleSendSpeed, sendedSize);

		totalSendSpeed += sendSpeed;
		totalReceiveSpeed += receiveSpeed;

		totalMiddleSendSpeed += middleSendSpeed;
		totalMiddleReceiveSpeed += middleReceiveSpeed;

		totalSended += sendedSize;
		totalReceived += receivedSize;

		++count;
	}
	m_sessionsLock.unlock();

	info += "\r\nTOTAL:";

	info += fillInfo("", NULL, totalSendQueueSize, totalReceiveSpeed, totalMiddleReceiveSpeed, totalReceived, totalSendSpeed, totalMiddleSendSpeed, totalSended);

	return info;
}

void CComManager::Update()
{
#ifdef LOGSESSSIONINFO
	pSessionsInfoMan->Log();
#endif
}

cm::ichangeconnection* CComManager::GetConnectionChanger()
{
	return m_pConnectionManager->GetConnectionChanger();
}

cm::IConnectionManager* CComManager::GetConnectionManager()
{
	return m_pConnectionManager;
}

std::wstring	GetApplicationRootDirectory()
{
	return glpcRootPath;
}