#pragma once

#include "CrashHandlerInThread.h"
#include "ProxySettings.h"
#include "ServerInfo.h"
#include "IConnection.h"
#include "ClientSession.h"
#include "EventSource.h"
#include "ViskoeThread.h"
#include "iclientsession.h"
#include "ProxyFinder.h"
#include <vector>
#include "UserSettings.h"
#include "oms_context.h"
#include "RM.h"
#include "Mutex.h"

using namespace cs;

class CClientSession;
struct IConnectionManager;

class ConnectThreadParams 
{
public:
	ConnectThreadParams(IConnection* aConnection, CServerInfo* aServerInfo, ILogWriter* apLW)
	{
		connection = aConnection;
		serverInfo = *aServerInfo;
		pLW = apLW;
		connectionResult = 0;
	}

	void destroy()
	{
		MP_DELETE_THIS;
	}

	IConnection* connection;
	ILogWriter* pLW;
	CServerInfo serverInfo;
	int connectionResult;
	HANDLE connectThreadHandle;
};

class ThreadContextObject
{
	friend class CClientSession;
public:
	ThreadContextObject(cm::IConnectionManager* aConnectionManager, ILogWriter* apLW, CClientSession* apClientSession, int aPtrCount)
	{
		pConnectionManager = aConnectionManager;
		pLW = apLW;
		pSession = apClientSession;

		m_ptrCount = aPtrCount;
		pConnection = NULL;

		m_hSendThreadCreateEvent = CreateEvent( NULL, TRUE, FALSE, NULL);
		m_hRecvThreadCreateEvent = CreateEvent( NULL, TRUE, FALSE, NULL);
		if (apLW != NULL)
			apLW->WriteLn("ThreadContextObject created ", this);

		m_isDestroying = false;
	}

	void destroy()
	{
		m_isDestroying = true;

		bool needDestroy = false;

		Lock();
		waitThreadsCreated();

		m_ptrCount--;
		needDestroy = m_ptrCount == 0;
		Unlock();

		reset();

		if (needDestroy)
		{
			if (pConnection != NULL)
			{
				pConnection->shutdown();
				pConnection->destroy();
			}

			MP_DELETE_THIS;
		}
	}

	void ThreadContextObject::reset()
	{
		waitThreadsCreated();

		Lock();
		{
			pConnectionManager = NULL;
			pLW = NULL;
			pSession = NULL;
		}
		Unlock();
	}

	bool isValid()
	{
		return	pConnectionManager != NULL 
				&& pSession != NULL;
	}

	inline bool IsDestroyingNow() { return m_isDestroying; }

	inline void ThreadContextObject::sendThreadCreated() { SetEvent(m_hSendThreadCreateEvent); }
	inline void ThreadContextObject::recvThreadCreated() { SetEvent(m_hRecvThreadCreateEvent); }
	void ThreadContextObject::waitThreadsCreated()
	{ 
		HANDLE arr[2] = {m_hSendThreadCreateEvent, m_hRecvThreadCreateEvent};
		WaitForMultipleObjects(2, &arr[0], true, INFINITE);
	}

	inline bool ThreadContextObject::TryToLock() { return m_mutex.tryToLock(); }
	inline void ThreadContextObject::Lock() { m_mutex.lock(); }
	inline void ThreadContextObject::Unlock() { m_mutex.unlock(); }

	cm::IConnectionManager*	pConnectionManager;
	ILogWriter*			pLW;
	CClientSession*		pSession;

	IConnection*		pConnection;

	CMutex				sending;
private:
	CMutex				m_mutex;
	int					m_ptrCount;

	HANDLE m_hSendThreadCreateEvent;
	HANDLE m_hRecvThreadCreateEvent;

	volatile bool		m_isDestroying;

};

typedef std::map<DWORD, HANDLE> ConnectionInfoMap;

class CProxySettingsChange: 
	public CEventSource<cm::ichangeconnection>
{
public:
	CProxySettingsChange(){}
	~CProxySettingsChange(){}

	void NotifySettingsChanged(std::vector<IProxySettings*>* aProxySettingsList);
	inline void Lock() { m_csSettingsChanged.lock(); }
	inline void UnLock() { m_csSettingsChanged.unlock(); }

private:
	CMutex					m_csSettingsChanged;
};

class CHttpProxySettingsChange: 
	public CEventSource<cm::IChangeHttpSettings>
{
public:
	CHttpProxySettingsChange(){}
	~CHttpProxySettingsChange(){}

	void NotifyHttpSettingsChanged(ProxyMode* aProxyMode, IProxySettings* aProxySettings);
	inline void Lock() { m_csSettingsChanged.lock(); }
	inline void UnLock() { m_csSettingsChanged.unlock(); }

private:
	CMutex		m_csSettingsChanged;
};

class CConnectionManager:
	public CEventSource<iclientsession>,
	public ViskoeThread::CThreadImpl<CConnectionManager>,
	public cm::ichangeconnection,
	public cm::IConnectionManager
{
public:
	CConnectionManager();
	~CConnectionManager(void);

	void setILogWriter(ILogWriter* aLW);
	void ReloadSettings();

	DWORD getSessionID();
	IConnection* getConnection(CServerInfo* aServerInfo, int& aConenctionResult, DWORD aConnectionSessionID);
	void abortConnect(DWORD aConnectionSessionID);
	bool isConnectionAborted(DWORD aConnectionSessionID);

	void PreDestroy();

	void setProxySettings( ProxyType aProxyType, std::vector<IProxySettings*>* aProxySettingsList, ProxyMode aProxyMode);
	void setProxyLoginPass(const wchar_t * aProxyUser, const wchar_t * aProxyPassword, unsigned char aProxyType, const wchar_t * aProxySetting);
	void setNetworkBreaksEmulationDelay( int auDelay, int auConnectionsMask);
	void setContext(oms::omsContext* aContext);

	bool isWinSocketConnection();
	ichangeconnection* GetConnectionChanger();
	ProxyMode getHttpProxyMode();
	IProxySettings* getHttpProxySettings();
	IProxySettings* getHttpsProxySettings();

	bool IsKnownProxyServer(std::string& userName, std::string& password, std::string& host, std::string& port);

	void notifyHttpSettingsChanged(bool abForce);
	// реализация CThreadImpl<CConnectionManager>
public:
	DWORD Run();

	// cm::ichangeconnection
public:
	void SubscribeToNotify(ichangeconnection* aSubscriber);
	void UnSubscribeFromNotify(ichangeconnection* aSubscriber);
	void OnChangeConnectionType(void* aProxyInfo){if(aProxyInfo == NULL) return;};

	// cm::IChangeHttpSettings
public:
	void SubscribeHttpSettingsChanged(cm::IChangeHttpSettings* aSubscriber);
	void UnsubscribeHttpSettingsChanged(cm::IChangeHttpSettings* aSubscriber);

private:
	IConnection* getConnectionToSocks(CServerInfo* aServerInfo, int& aConenctionResult, int& aSocketErrorCode, DWORD aConnectionSessionID, bool* isThrowProxyAuthError);
	IConnection* getConnectionToTunnel(CServerInfo* aServerInfo, int& aConenctionResult, int& aSocketErrorCode, DWORD aConnectionSessionID, bool* isThrowProxyAuthError);

	int connect(IConnection* aConnection, CServerInfo* aServerInfo, int& aSocketErrorCode, DWORD aConnectionSessionID);
	static DWORD WINAPI connectThread_( VOID* param);
	void closeAllConnection();
	void writeProxySettingsToLog(std::vector<IProxySettings*>* aProxySettingsList);
	void updateProxyLoginPass(std::vector<IProxySettings*>* aProxySettingsList, const wchar_t * aProxyUser, const wchar_t * aProxyPassword, const wchar_t * aProxySetting);

	IConnection* getConnectionToSocksImpl(CServerInfo* aServerInfo, int& aConnectionResult, int& aSocketErrorCode, DWORD aConnectionSessionID, ProxySettings* aProxySettings, bool* isThrowProxyAuthError);
	IConnection* getConnectionToTunnelImpl(CServerInfo* aServerInfo, int& aConenctionResult, int& aSocketErrorCode, DWORD aConnectionSessionID, ProxySettings* aProxySettings, bool* isThrowProxyAuthError);

	void SaveLastSuccessfulSettings(IProxySettings* aProxySetting);
	bool LoadLastSuccessfulSettings(IProxySettings* aProxySetting);
	//	void clearProxySettingsList(std::vector<IProxySettings*>* aProxySettingsList);
	//	void copyProxySettingsList(std::vector<IProxySettings*>* aSourceList, std::vector<IProxySettings*>* aDestList);
	void saveProxyLoginPassSettings(IProxySettings* aProxySetting);
	bool checkProxySettingsInList(IProxySettings* aProxySettings, ProxySettingsVec* aProxySettingsList);
private:
	ILogWriter*			m_pLW;

	oms::omsContext*	m_pContext;

	ProxySettings		m_firstSuccessProxySettings;
	volatile bool		m_isSuccessProxySettingsExist;

	// networks break emulation
	CEvent					m_eventConnectionBreakEmulation;
	int						m_connectionBreakEmulationTimeout;
	int						m_connectionBreakEmulationMask;

	ConnectionInfoMap		m_infoConnections;
	CMutex					m_csConnections;

	volatile bool			m_bRuning;
	bool					m_bNeedThrowProxyAuthError;

	DWORD					m_lastConnectionSessionID;

	CProxySettingsChange	m_settingsChangedNotifyer;
	CHttpProxySettingsChange m_httpSettingsChangedNotifyer;

	ProxySettingsVec		m_httpsProxySettingsList;
	ProxySettingsVec		m_socksProxySettingsList;
	CMutex					m_ProxySettingsLock;

	ProxySettings			m_httpProxySettings;
	ProxySettings			m_httpsProxySettings;

	ProxyMode				m_httpProxyMode;
	ProxyMode				m_httpsProxyMode;
	ProxyMode				m_socksProxyMode;

	ProxySettings			m_lastSuccessConnection;

	MP_MAP<DWORD, IConnection*>	m_connections;
	CMutex					m_connectionsLock;

	CUserSettings			m_userSettings;
};