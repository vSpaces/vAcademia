#include "StdAfx.h"
#include "ConnectionManager.h"

CConnectionManager::CConnectionManager() :
MP_VECTOR_INIT(m_socksProxySettingsList),
MP_VECTOR_INIT(m_httpsProxySettingsList),
MP_MAP_INIT(m_connections)
{
	m_pLW = NULL;
	m_pContext = NULL;
	m_connectionBreakEmulationTimeout = 0;
	m_eventConnectionBreakEmulation.Create(NULL, true, false);
	m_bRuning = true;
	m_bNeedThrowProxyAuthError = false;
	m_lastConnectionSessionID = 0;
	m_isSuccessProxySettingsExist = false;
}

CConnectionManager::~CConnectionManager(void)
{
	m_eventConnectionBreakEmulation.SetEvent();
	if( IsRunning())
		Stop();

	closeAllConnection();

	bool isAllConnectComplete = false;
	while (true)
	{
		isAllConnectComplete = true;
		m_csConnections.lock();
		for (ConnectionInfoMap::iterator it = m_infoConnections.begin(); it != m_infoConnections.end(); ++it)
		{
			if (it->second != NULL)
			{
				isAllConnectComplete = false;
				break;
			}
		}
		m_csConnections.unlock();

		if (isAllConnectComplete)
			break;

		Sleep(500);
	}

	ProxySettingsList::clearProxySettingsList(&m_httpsProxySettingsList);
	ProxySettingsList::clearProxySettingsList(&m_socksProxySettingsList);
}

void CConnectionManager::setILogWriter(ILogWriter* aLW)
{
	m_pLW = aLW;
	if( m_pLW == NULL)
		return;
	m_ProxySettingsLock.lock();
	writeProxySettingsToLog(&m_httpsProxySettingsList);
	writeProxySettingsToLog(&m_socksProxySettingsList);
	m_ProxySettingsLock.unlock();
}

void CConnectionManager::writeProxySettingsToLog(std::vector<IProxySettings*>* aProxySettingsList)
{
	if (m_pLW != NULL)
	{
		ProxySettingsVec::iterator it = aProxySettingsList->begin();
		ProxySettingsVec::iterator end = aProxySettingsList->end();
		for ( ; it != end; ++it)
		{
			IProxySettings* proxySettings = (IProxySettings*)(*it);
			CLogValue log("WriteProxySettingsToLog = ", proxySettings->toString());
			m_pLW->WriteLn(log);
		}
	}
}

void CConnectionManager::setProxySettings( ProxyType aProxyType, std::vector<IProxySettings*>* aProxySettingsList, ProxyMode aProxyMode)
{
	if (aProxySettingsList == NULL)
		return;

	m_bNeedThrowProxyAuthError = false;

	writeProxySettingsToLog(aProxySettingsList);

	m_ProxySettingsLock.lock();
	{
		m_firstSuccessProxySettings.clear();
		m_isSuccessProxySettingsExist = false;

		if (aProxyType == PT_HTTPS)
		{
			ProxySettingsList::copyProxySettingsList(aProxySettingsList, &m_httpsProxySettingsList);
			m_httpsProxyMode = aProxyMode;
		}
		else if (aProxyType == PT_HTTP)
		{
			m_httpProxyMode = aProxyMode;
			notifyHttpSettingsChanged(true);
		}
		else if (aProxyType == PT_SOCKS4 || aProxyType == PT_SOCKS5)
		{
			ProxySettingsList::copyProxySettingsList(aProxySettingsList, &m_socksProxySettingsList);
			m_socksProxyMode = aProxyMode;
		}
	}

	m_settingsChangedNotifyer.Lock();
	m_settingsChangedNotifyer.NotifySettingsChanged(aProxySettingsList);
	m_settingsChangedNotifyer.UnLock();

	m_ProxySettingsLock.unlock();
}

void CConnectionManager::notifyHttpSettingsChanged(bool abForce)
{
	if (!abForce && m_httpProxyMode != PM_SYSTEM)
		return;

	IProxySettings* proxySettings = NULL;

	if (m_httpProxyMode == PM_MANUAL)
		proxySettings = &m_httpProxySettings;
	else if (m_httpProxyMode == PM_SYSTEM)
	{
		m_ProxySettingsLock.lock();
		if (m_isSuccessProxySettingsExist && m_firstSuccessProxySettings.getType() == PT_HTTPS && m_firstSuccessProxySettings.enabled)
		{
			m_httpsProxySettings = m_firstSuccessProxySettings;
			proxySettings = &m_httpsProxySettings;
		}
		m_ProxySettingsLock.unlock();
	}

	if (!abForce && proxySettings == NULL)
		return;

	m_httpSettingsChangedNotifyer.Lock();
	m_httpSettingsChangedNotifyer.NotifyHttpSettingsChanged(&m_httpProxyMode, proxySettings);
	m_httpSettingsChangedNotifyer.UnLock();
}

void CConnectionManager::setProxyLoginPass(const wchar_t * aProxyUser, const wchar_t * aProxyPassword, unsigned char aProxyType, const wchar_t * aProxySetting)
{
	m_bNeedThrowProxyAuthError = false;

	if (aProxyType == PT_HTTPS)
	{
		updateProxyLoginPass(&m_httpsProxySettingsList, aProxyUser, aProxyPassword, aProxySetting);
	}
	else if ( aProxyType == PT_SOCKS4 || aProxyType == PT_SOCKS5)
	{
		updateProxyLoginPass(&m_socksProxySettingsList, aProxyUser, aProxyPassword, aProxySetting);
	}	

	m_ProxySettingsLock.lock();
	m_firstSuccessProxySettings.clear();
	m_isSuccessProxySettingsExist = false;
	m_ProxySettingsLock.unlock();

	Invoke<&iclientsession::ChangeConnection>();
}

void CConnectionManager::updateProxyLoginPass(std::vector<IProxySettings*>* aProxySettingsList, const wchar_t * aProxyUser, const wchar_t * aProxyPassword, const wchar_t * aProxySettingName)
{
	m_ProxySettingsLock.lock();
	ProxySettingsVec::iterator it = aProxySettingsList->begin();
	ProxySettingsVec::iterator end = aProxySettingsList->end();
	for ( ; it != end; ++it)
	{
		IProxySettings* proxySettings = (IProxySettings*)(*it);
		if (wcscmp( proxySettings->getName(), aProxySettingName) == 0)
		{
			proxySettings->setUseUser( true);
			proxySettings->setUser( (wchar_t*)aProxyUser);
			proxySettings->setPassword( (wchar_t*)aProxyPassword);
			break;
		}
	}

	ProxySettingsVec::iterator begin = aProxySettingsList->begin();
	if (it != end && it != begin)
	{
		std::swap(begin, it);
	}

	m_settingsChangedNotifyer.NotifySettingsChanged(aProxySettingsList);

	m_ProxySettingsLock.unlock();
}

void CConnectionManager::saveProxyLoginPassSettings(IProxySettings* aProxySetting)
{
	if (!m_bRuning)
		return;

	USES_CONVERSION;

	if (wcscmp(aProxySetting->getName(), L"user_settings") == 0)
	{
		unsigned short proxyType = aProxySetting->getType();
		if (proxyType == PT_SOCKS4 || proxyType == PT_SOCKS5)
		{
			m_userSettings.SaveSetting(std::string("socks_username"), std::string(W2A(aProxySetting->getUser())));
			//m_userSettings.SavePassSetting(std::string("socks_pass"), std::string(W2A(aProxySetting->getPassword())));
			m_userSettings.SaveSetting(std::string("socks_pass"), std::string(W2A(aProxySetting->getPassword())));
			m_userSettings.SaveSetting(std::string("socks_use_user"), std::string("yes"));
		}
		else if (proxyType == PT_HTTP)
		{	
			m_userSettings.SaveSetting(std::string("proxy_username"), std::string(W2A(aProxySetting->getUser())));
			//m_userSettings.SavePassSetting(std::string("proxy_pass"), std::string(W2A(aProxySetting->getPassword())));
			m_userSettings.SaveSetting(std::string("proxy_pass"), std::string(W2A(aProxySetting->getPassword())));
			m_userSettings.SaveSetting(std::string("proxy_use_user"), std::string("yes"));
		}
		else if (proxyType == PT_HTTPS)
		{		
			m_userSettings.SaveSetting(std::string("https_username"), std::string(W2A(aProxySetting->getUser())));
			//m_userSettings.SavePassSetting(std::string("https_pass"), std::string(W2A(aProxySetting->getPassword())));
			m_userSettings.SaveSetting(std::string("https_pass"), std::string(W2A(aProxySetting->getPassword())));
			m_userSettings.SaveSetting(std::string("https_use_user"), std::string("yes"));
		}
		m_userSettings.SaveGlobalSettings();
	}
}

void CConnectionManager::SaveLastSuccessfulSettings(IProxySettings* aProxySetting)
{
	if (!m_bRuning)
		return;

	char buf[10];
	USES_CONVERSION;

	m_userSettings.SaveSetting(std::string("ls_addres"), std::string(W2A(aProxySetting->getIP())));
	m_userSettings.SaveSetting(std::string("ls_name"), std::string(W2A(aProxySetting->getName())));
	_itoa_s( aProxySetting->getPort(), buf, 10, 10);
	m_userSettings.SaveSetting(std::string("ls_port"), buf);
	m_userSettings.SaveSetting(std::string("ls_use_user"), aProxySetting->isUseUser() ? std::string("1") : std::string("0"));
	m_userSettings.SaveSetting(std::string("ls_username"), std::string(W2A(aProxySetting->getUser())));
	//m_userSettings.SavePassSetting(std::string("ls_pass"), std::string(W2A(aProxySetting->getPassword())), false);
	m_userSettings.SaveSetting(std::string("ls_pass"), std::string(W2A(aProxySetting->getPassword())));
	_itoa_s( aProxySetting->getType(), buf, 10, 10);
	m_userSettings.SaveSetting(std::string("ls_type"), buf);
	if( aProxySetting->isUseUser() || aProxySetting->getType() != 2)
		m_userSettings.SaveSetting(std::string("ls_tunnel"), aProxySetting->isTunnel() ? std::string("1") : std::string("0"));
	m_userSettings.SaveSetting(std::string("ls_enabled"), aProxySetting->isEnabled() ? std::string("1") : std::string("0"));
	m_userSettings.SaveGlobalSettings();
}

bool CConnectionManager::LoadLastSuccessfulSettings(IProxySettings* aProxySetting)
{
	if (aProxySetting == NULL)
		return false;

	USES_CONVERSION; 

	m_userSettings.LoadGlobal();

	std::string enabled = m_userSettings.GetSetting("ls_enabled");
	if (enabled == "1")
		aProxySetting->setEnabled(true);

	std::string addres = m_userSettings.GetSetting("ls_addres");
	aProxySetting->setIP(A2W(addres.c_str()));

	std::string username = m_userSettings.GetSetting("ls_username");
	aProxySetting->setUser(A2W(username.c_str()));

	//std::string password = m_userSettings.GetPassSetting("ls_pass");
	std::string password = m_userSettings.GetSetting("ls_pass");
	aProxySetting->setPassword(A2W(password.c_str()));

	std::string name = m_userSettings.GetSetting("ls_name");
	aProxySetting->setName(A2W(name.c_str()));

	std::string use_user = m_userSettings.GetSetting("ls_use_user");
	if (use_user == "1")
		aProxySetting->setUseUser(true);

	std::string port = m_userSettings.GetSetting("ls_port");
	unsigned short iPort = 0;
	iPort = (unsigned short)rtl_atoi(port.c_str());
	aProxySetting->setPort(iPort);

	std::string type = m_userSettings.GetSetting("ls_type");
	unsigned short iType = 0;
	iType = (unsigned short)rtl_atoi(type.c_str());
	if (iType != PT_HTTP && iType != PT_HTTPS && iType != PT_SOCKS4 && iType != PT_SOCKS5)
		return false;

	aProxySetting->setType(iType);

	std::string tunnel = m_userSettings.GetSetting("ls_tunnel");
	if (tunnel == "1" && ( use_user == "1" || iType != PT_HTTPS))
		aProxySetting->setTunnel(true);

	// надо проверить что эти настройки возможно использовать при текущем типе подключения
	bool isExist = false;
	isExist = checkProxySettingsInList(aProxySetting, &m_socksProxySettingsList);
	if (!isExist)
		isExist = checkProxySettingsInList(aProxySetting, &m_httpsProxySettingsList);

	if (!isExist)
		return false;

	return true;
}

bool CConnectionManager::checkProxySettingsInList(IProxySettings* apProxySettings, ProxySettingsVec* aProxySettingsList)
{
	bool isExist = false;
	ProxySettings* aProxySettings = (ProxySettings*)apProxySettings;

	ProxySettingsVec::iterator begin = aProxySettingsList->begin();
	ProxySettingsVec::iterator it = begin;
	ProxySettingsVec::iterator end = aProxySettingsList->end();
	for ( ; it != end; ++it)
	{
		if (((IProxySettings*)*it)->isEqual(aProxySettings))
		{
			isExist = true;
			break;
		}
	}

	if (it != end && it != begin)
	{
		std::swap(begin, it);
	}

	return isExist;
}

bool CConnectionManager::IsKnownProxyServer(std::string& userName, std::string& password, std::string& host, std::string& port)
{
	if ((m_pContext) && (m_pContext->mpRM))
	{
		return m_pContext->mpRM->IsKnownProxyServer(userName, password, host, port);
	}
    
	return false;	
}

void CConnectionManager::ReloadSettings()
{
	m_userSettings.LoadGlobal();

	USES_CONVERSION;

	if (((m_userSettings.GetSetting("proxy_mode") == "") || (m_userSettings.GetSetting("proxy_mode") == "system")) &&
		((m_userSettings.GetSetting("socks_mode") == "") || (m_userSettings.GetSetting("socks_mode") == "system")) &&
		((m_userSettings.GetSetting("https_mode") == "") || (m_userSettings.GetSetting("https_mode") == "system")))
	{
		std::string userName, password, host, port;
		if (IsKnownProxyServer(userName, password, host, port))
		{
			m_userSettings.SaveSetting("proxy_mode", "manual");
			m_userSettings.SaveSetting("proxy_address", host);
			m_userSettings.SaveSetting("proxy_port", port);
			m_userSettings.SaveSetting("proxy_use_user", "yes");
			m_userSettings.SaveSetting("proxy_username", userName);
			m_userSettings.SaveSetting("proxy_pass", password);
			
			m_userSettings.SaveSetting("socks_mode", "manual");
			m_userSettings.SaveSetting("socks_address", host);
			m_userSettings.SaveSetting("socks_port", port);
			m_userSettings.SaveSetting("socks_use_user", "yes");
			m_userSettings.SaveSetting("socks_username", userName);
			m_userSettings.SaveSetting("socks_pass", password);

			m_userSettings.SaveSetting("https_mode", "manual");
			m_userSettings.SaveSetting("https_address", host);
			m_userSettings.SaveSetting("https_port", port);
			m_userSettings.SaveSetting("https_use_user", "yes");
			m_userSettings.SaveSetting("https_username", userName);
			m_userSettings.SaveSetting("https_pass", password);
			m_userSettings.SaveGlobalSettings();

			m_userSettings.LoadGlobal();
		}
	}

	// HTTP
	std::vector<IProxySettings*> httpProxySettingsList;

	ProxySettings* noHttpProxyUserSettings = MP_NEW(ProxySettings); // без проски
	noHttpProxyUserSettings->type = PT_HTTP;

	string sProxyMode = m_userSettings.GetSetting("proxy_mode");
	if (sProxyMode == "")
		sProxyMode = "system";

	ProxyMode proxyMode = PM_NO_PROXY;
	if( sProxyMode == "manual")
	{
		ProxySettings* manualUserSettings = MP_NEW(ProxySettings);

		manualUserSettings->enabled = true;
		manualUserSettings->type = PT_HTTP;
		manualUserSettings->ip = A2W(m_userSettings.GetSetting("proxy_address").c_str());
		manualUserSettings->port = (unsigned short)rtl_atoi(m_userSettings.GetSetting("proxy_port").c_str());
		manualUserSettings->name = L"user_settings";

		if(m_userSettings.GetSetting("proxy_use_user") == "yes")
		{
			manualUserSettings->useUser = true;
			manualUserSettings->user = A2W(m_userSettings.GetSetting("proxy_username").c_str());
			//manualUserSettings->password = A2W(m_userSettings.GetPassSetting("proxy_pass").c_str());
			manualUserSettings->password = A2W(m_userSettings.GetSetting("proxy_pass").c_str());
		}

		httpProxySettingsList.push_back(manualUserSettings);
		proxyMode = PM_MANUAL;

		m_httpProxySettings = *manualUserSettings;
	}
	else if (sProxyMode == "system" || sProxyMode == "no")
	{
		httpProxySettingsList.push_back(noHttpProxyUserSettings);

		CProxyFinder proxyFinder;
		proxyFinder.getAllProxySettings(&httpProxySettingsList, PT_HTTP);
		proxyMode = PM_SYSTEM;
	}
	else
	{
		httpProxySettingsList.push_back(noHttpProxyUserSettings);
		m_httpProxySettings = *noHttpProxyUserSettings;
	}

	setProxySettings(PT_HTTP, &httpProxySettingsList, proxyMode);
	ProxySettingsList::clearProxySettingsList(&httpProxySettingsList);


	// SOCKS 4 5
	std::vector<IProxySettings*> socksProxySettingsList;

	ProxySettings* noSocksProxyUserSettings = MP_NEW(ProxySettings); // без проски
	noSocksProxyUserSettings->type = PT_SOCKS5;

	sProxyMode = m_userSettings.GetSetting("socks_mode");
	if (sProxyMode == "")
		sProxyMode = "system";
	proxyMode = PM_NO_PROXY;
	if( sProxyMode == "manual")
	{
		ProxySettings* manualUserSettings = MP_NEW(ProxySettings);

		manualUserSettings->enabled = true;
		manualUserSettings->type = (unsigned short)(m_userSettings.GetSetting("socks_ver") == "4" ? PT_SOCKS4 : PT_SOCKS5);
		manualUserSettings->ip = A2W(m_userSettings.GetSetting("socks_address").c_str());
		manualUserSettings->port = (unsigned short)rtl_atoi(m_userSettings.GetSetting("socks_port").c_str());
		manualUserSettings->name = L"user_settings";

		if(m_userSettings.GetSetting("socks_use_user") == "yes")
		{
			manualUserSettings->useUser = true;
			manualUserSettings->user = A2W(m_userSettings.GetSetting("socks_username").c_str());
			//manualUserSettings->password = A2W(m_userSettings.GetPassSetting("socks_pass").c_str());
			manualUserSettings->password = A2W(m_userSettings.GetSetting("socks_pass").c_str());
		}

		socksProxySettingsList.push_back(manualUserSettings);
		proxyMode = PM_MANUAL;
	}
	else if (sProxyMode == "system" || sProxyMode == "no")
	{

		socksProxySettingsList.push_back(noSocksProxyUserSettings);
		CProxyFinder proxyFinder;
		proxyFinder.getAllProxySettings(&socksProxySettingsList, PT_SOCKS5);
		proxyMode = PM_SYSTEM;
	}
	else
	{
		socksProxySettingsList.push_back(noSocksProxyUserSettings);
	}

	setProxySettings(PT_SOCKS5, &socksProxySettingsList, proxyMode);
	ProxySettingsList::clearProxySettingsList(&socksProxySettingsList);


	// HTTPS
	std::vector<IProxySettings*> httpsProxySettingsList;

	ProxySettings* noHttpsProxyUserSettings = MP_NEW(ProxySettings); // без проски
	noHttpsProxyUserSettings->type = PT_HTTPS;

	sProxyMode = m_userSettings.GetSetting("https_mode");
	if (sProxyMode == "")
		sProxyMode = "system";
	proxyMode = PM_NO_PROXY;
	if( sProxyMode == "manual")
	{
		ProxySettings* manualUserSettings = MP_NEW(ProxySettings);

		manualUserSettings->enabled = true;
		manualUserSettings->type = PT_HTTPS;
		manualUserSettings->ip = A2W(m_userSettings.GetSetting("https_address").c_str());
		manualUserSettings->port = (unsigned short)rtl_atoi(m_userSettings.GetSetting("https_port").c_str());
		manualUserSettings->name = L"user_settings";

		if(m_userSettings.GetSetting("https_use_user") == "yes")
		{
			manualUserSettings->useUser = true;
			manualUserSettings->user = A2W(m_userSettings.GetSetting("https_username").c_str());
			//manualUserSettings->password = A2W(m_userSettings.GetPassSetting("https_pass").c_str());
			manualUserSettings->password = A2W(m_userSettings.GetSetting("https_pass").c_str());
		}

		httpsProxySettingsList.push_back(manualUserSettings);
		proxyMode = PM_MANUAL;
	}
	else if (sProxyMode == "system" || sProxyMode == "no")
	{
		httpsProxySettingsList.push_back(noHttpsProxyUserSettings);

		CProxyFinder proxyFinder;
		proxyFinder.getAllProxySettings(&httpsProxySettingsList, PT_HTTPS);
		proxyMode = PM_SYSTEM;
	}
	else
	{
		httpsProxySettingsList.push_back(noHttpsProxyUserSettings);
	}

	setProxySettings(PT_HTTPS, &httpsProxySettingsList, proxyMode);
	ProxySettingsList::clearProxySettingsList(&httpsProxySettingsList);


	// загрузка настроек последнего удачного соединения
	if ( LoadLastSuccessfulSettings(&m_lastSuccessConnection))
	{
		m_ProxySettingsLock.lock();
		m_firstSuccessProxySettings = m_lastSuccessConnection;
		m_isSuccessProxySettingsExist = true;
		m_ProxySettingsLock.unlock();
		if (m_lastSuccessConnection.getType() == PT_HTTPS)
		{
			updateProxyLoginPass(&m_httpsProxySettingsList, m_lastSuccessConnection.getUser(), m_lastSuccessConnection.getPassword(), m_lastSuccessConnection.getName());
		}
		else if ( m_lastSuccessConnection.getType() == PT_SOCKS4 || m_lastSuccessConnection.getType() == PT_SOCKS5)
		{
			updateProxyLoginPass(&m_socksProxySettingsList, m_lastSuccessConnection.getUser(), m_lastSuccessConnection.getPassword(), m_lastSuccessConnection.getName());
		}
		/*else
		{
			m_lastSuccessConnection.setTunnel( false);
		}*/


		notifyHttpSettingsChanged(false);
	}
	else
	{
		m_lastSuccessConnection.enabled = false;
		m_lastSuccessConnection.tunnel = false;
	}

	Invoke<&iclientsession::ChangeConnection>();
}

void CConnectionManager::setContext(oms::omsContext* aContext)
{		
	m_pContext = aContext;

	ReloadSettings();
}

void CConnectionManager::abortConnect(DWORD aConnectionSessionID)
{
	m_csConnections.lock();
	if (m_infoConnections.find(aConnectionSessionID) != m_infoConnections.end() && m_infoConnections[aConnectionSessionID] != NULL)
	{
		SetEvent(m_infoConnections[aConnectionSessionID]);
		if (m_pLW != NULL)
			m_pLW->WriteLn("CConnectionManager::abortConnect success");
	}
	else
	{
		if (m_pLW != NULL)
			m_pLW->WriteLn("CConnectionManager::abortConnect failed");
	}
	m_csConnections.unlock();
}

DWORD CConnectionManager::getSessionID()
{
	return ++m_lastConnectionSessionID;
}

bool CConnectionManager::isConnectionAborted(DWORD aConnectionSessionID)
{
	bool isAborted = false;
	m_csConnections.lock();
	{
		if (m_infoConnections.find(aConnectionSessionID) != m_infoConnections.end() && m_infoConnections[aConnectionSessionID] != NULL)
		{
			DWORD waitResult = WaitForSingleObject(m_infoConnections[aConnectionSessionID], 0);
			if (waitResult == WAIT_OBJECT_0)
				isAborted = true;
		}
		else
			isAborted = true;
	}
	m_csConnections.unlock();

	return isAborted;
}

IConnection* CConnectionManager::getConnection(CServerInfo* aServerInfo, int& aConenctionResult, DWORD aConnectionSessionID)
{
	if (!m_bRuning)
		return NULL;

	bool wasNeedThrowProxyAuthError = m_bNeedThrowProxyAuthError;

	// http proxy 
	// socks proxy 
	// использовать http-туннелирование

	/*
		Если нет сокс прокси, то обычный сокет, иначе сокс сокет
        Если сокет не открылся, то 
			Если есть туннелирование, то
				Если https proxy то туннель через него
				Иначе туннель через https

	*/

	m_csConnections.lock();
	m_infoConnections[aConnectionSessionID] = CreateEvent(NULL, TRUE, FALSE, NULL);
	ResetEvent(m_infoConnections[aConnectionSessionID]);
	m_csConnections.unlock();

	IConnection* connection = NULL;
	SOCKERR socketErrorCode = 0;

	bool isFirstSuccessConnectionExist = false;
	bool isFirstSuccessConnectionTunnel = false;
	m_ProxySettingsLock.lock();
	isFirstSuccessConnectionExist = m_isSuccessProxySettingsExist;
	isFirstSuccessConnectionTunnel = m_firstSuccessProxySettings.tunnel;
	m_ProxySettingsLock.unlock();

	if (isFirstSuccessConnectionExist)
	{
		bool donNotThrowProxyAuthError = false;
		if (isFirstSuccessConnectionTunnel)
			connection = getConnectionToTunnel(aServerInfo, aConenctionResult, socketErrorCode, aConnectionSessionID, &donNotThrowProxyAuthError);
		else
			connection = getConnectionToSocks(aServerInfo, aConenctionResult, socketErrorCode, aConnectionSessionID, &donNotThrowProxyAuthError);

		if (connection == NULL)
		{
			m_ProxySettingsLock.lock();
			m_isSuccessProxySettingsExist = false;
			m_ProxySettingsLock.unlock();
		}
	}

	if (connection == NULL && !isConnectionAborted(aConnectionSessionID))
	{
		// попытка соединения по всем настройкам
		connection = getConnectionToSocks(aServerInfo, aConenctionResult, socketErrorCode, aConnectionSessionID, &m_bNeedThrowProxyAuthError);

		if (connection == NULL)
			connection = getConnectionToTunnel(aServerInfo, aConenctionResult, socketErrorCode, aConnectionSessionID, &m_bNeedThrowProxyAuthError);

		if (connection == NULL)
		{
			aConenctionResult = CONNECTION_SOCKET_ERROR;

			if (m_pContext != NULL && m_pContext->mpSM != NULL && m_bNeedThrowProxyAuthError && !isConnectionAborted(aConnectionSessionID))
				m_pContext->mpSM->OnConnectToServerError(socketErrorCode);
		}
	}

	m_csConnections.lock();
	CloseHandle(m_infoConnections[aConnectionSessionID]);
	m_infoConnections[aConnectionSessionID] = NULL;
	m_csConnections.unlock();

	// если мы покдлючались после сброса настроек и не подключились
	if (connection == NULL && wasNeedThrowProxyAuthError == false)
		m_bNeedThrowProxyAuthError = true;

	m_connectionsLock.lock();
	//m_connections TODO
	m_connectionsLock.unlock();

	return connection;
}

bool CConnectionManager::isWinSocketConnection()
{
	bool result;
	m_ProxySettingsLock.lock();
	result = ((!m_firstSuccessProxySettings.tunnel && !m_firstSuccessProxySettings.enabled) ? true : false);
	m_ProxySettingsLock.unlock();

	return result;
}

cm::ichangeconnection* CConnectionManager::GetConnectionChanger()
{
	return this;
}

IConnection* CConnectionManager::getConnectionToSocks(CServerInfo* aServerInfo, int& aConnectionResult, int& aSocketErrorCode, DWORD aConnectionSessionID, bool* isThrowProxyAuthError)
{
	IConnection* connection = NULL;
	m_ProxySettingsLock.lock();
	std::vector<IProxySettings*> socksProxySettingsList;
	ProxySettingsList::copyProxySettingsList( &m_socksProxySettingsList, &socksProxySettingsList);
	m_ProxySettingsLock.unlock();

	bool isSuccessProxySettingsExist = false;
	ProxySettings successProxySettings;
	m_ProxySettingsLock.lock();
	isSuccessProxySettingsExist = m_isSuccessProxySettingsExist;
	if (isSuccessProxySettingsExist)
		successProxySettings = m_firstSuccessProxySettings;
	m_ProxySettingsLock.unlock();

	if (isSuccessProxySettingsExist)
	{
		return getConnectionToSocksImpl(aServerInfo, aConnectionResult, aSocketErrorCode, aConnectionSessionID, &successProxySettings, isThrowProxyAuthError);
	}

	ProxySettingsVec::iterator it = socksProxySettingsList.begin();
	ProxySettingsVec::iterator end = socksProxySettingsList.end();
	for ( ; it != end; ++it)
	{
		connection = getConnectionToSocksImpl(aServerInfo, aConnectionResult, aSocketErrorCode, aConnectionSessionID, (ProxySettings*)(*it), isThrowProxyAuthError);
		if (connection != NULL)
		{
			m_ProxySettingsLock.lock();
			if (!m_isSuccessProxySettingsExist)
			{
				m_firstSuccessProxySettings = *((ProxySettings*)(*it));
				m_firstSuccessProxySettings.tunnel = false;
				m_isSuccessProxySettingsExist = true;
				SaveLastSuccessfulSettings(&m_firstSuccessProxySettings);
				saveProxyLoginPassSettings(&m_firstSuccessProxySettings);

				notifyHttpSettingsChanged(false);
			}
			m_ProxySettingsLock.unlock();
			break;
		}

		if (isConnectionAborted(aConnectionSessionID))
			break;
	}

	ProxySettingsList::clearProxySettingsList(&socksProxySettingsList);

	return connection;
}

IConnection* CConnectionManager::getConnectionToSocksImpl(CServerInfo* aServerInfo, int& aConnectionResult, int& aSocketErrorCode, DWORD aConnectionSessionID, ProxySettings* aProxySettings, bool* isThrowProxyAuthError)
{
	IConnection* connection = NULL;

	if (aProxySettings->enabled) // если настройки прокси включены
	{
		MP_NEW_P2(socksConnection, CSocksConnection, aProxySettings, 2);
		connection = (IConnection*)socksConnection;
	}
	else
	{
		MP_NEW_P(winSocketConnection, CWinSocketConnection, 2);
		connection = (IConnection*)winSocketConnection;
	}

	aConnectionResult = connect(connection, aServerInfo, aSocketErrorCode, aConnectionSessionID);
	if (aConnectionResult != CONNECTION_NO_ERROR)
	{
		connection = NULL;

		if ( aConnectionResult == CONNECTION_PROXY_AUTH_ERROR 
			&& *isThrowProxyAuthError
			&& m_pContext != NULL 
			&& m_pContext->mpSM != NULL 
			&& !isConnectionAborted(aConnectionSessionID)
			)
		{
            m_pContext->mpSM->OnProxyAuthError(aProxySettings->name.c_str(), aProxySettings->type, aProxySettings->ip.c_str());
		}
	}

	return connection;
}

IConnection* CConnectionManager::getConnectionToTunnel(CServerInfo* aServerInfo, int& aConnectionResult, int& aSocketErrorCode, DWORD aConnectionSessionID, bool* isThrowProxyAuthError)
{
	IConnection* connection = NULL;

	m_ProxySettingsLock.lock();
	std::vector<IProxySettings*> httpsProxySettingsList;
	ProxySettingsList::copyProxySettingsList( &m_httpsProxySettingsList, &httpsProxySettingsList);
	m_ProxySettingsLock.unlock();

	bool isSuccessProxySettingsExist = false;
	ProxySettings successProxySettings;
	m_ProxySettingsLock.lock();
	isSuccessProxySettingsExist = m_isSuccessProxySettingsExist;
	if (isSuccessProxySettingsExist)
		successProxySettings = m_firstSuccessProxySettings;
	m_ProxySettingsLock.unlock();

	if (isSuccessProxySettingsExist)
	{
		return getConnectionToTunnelImpl(aServerInfo, aConnectionResult, aSocketErrorCode, aConnectionSessionID, &successProxySettings, isThrowProxyAuthError);
	}

	ProxySettingsVec::iterator it = httpsProxySettingsList.begin();
	ProxySettingsVec::iterator end = httpsProxySettingsList.end();
	for ( ; it != end; ++it)
	{
		connection = getConnectionToTunnelImpl(aServerInfo, aConnectionResult, aSocketErrorCode, aConnectionSessionID, (ProxySettings*)(*it), isThrowProxyAuthError);
		if (connection != NULL)
		{
			m_ProxySettingsLock.lock();
			if ( !m_isSuccessProxySettingsExist)
			{
				m_firstSuccessProxySettings = *((ProxySettings*)(*it));
				m_firstSuccessProxySettings.tunnel = true;
				m_isSuccessProxySettingsExist = true;
				SaveLastSuccessfulSettings(&m_firstSuccessProxySettings);
				saveProxyLoginPassSettings(&m_firstSuccessProxySettings);

				notifyHttpSettingsChanged(false);
			}
			m_ProxySettingsLock.unlock();

			break;
		}

		if (isConnectionAborted(aConnectionSessionID))
			break;
	}

	ProxySettingsList::clearProxySettingsList(&httpsProxySettingsList);

	return connection;
}

IConnection* CConnectionManager::getConnectionToTunnelImpl(CServerInfo* aServerInfo, int& aConenctionResult, int& aSocketErrorCode, DWORD aConnectionSessionID, ProxySettings* aProxySettings, bool* isThrowProxyAuthError)
{
	IConnection* connection = NULL;

	MP_NEW_P2(tunnelConnection, CTunnelConnection, aProxySettings, 2);
	connection = (IConnection*)tunnelConnection;
	aConenctionResult = connect(connection, aServerInfo, aSocketErrorCode, aConnectionSessionID);
	if (aConenctionResult != CONNECTION_NO_ERROR)
	{
		connection = NULL;

		if ( aConenctionResult == CONNECTION_PROXY_AUTH_ERROR 
			&& *isThrowProxyAuthError
			&& m_pContext != NULL 
			&& m_pContext->mpSM != NULL
			&& !isConnectionAborted(aConnectionSessionID)
			)
		{
			m_pContext->mpSM->OnProxyAuthError(aProxySettings->name.c_str(), aProxySettings->type, aProxySettings->ip.c_str());
		}
	}

	return connection;
}

int CConnectionManager::connect(IConnection* aConnection, CServerInfo* aServerInfo, int& aSocketErrorCode, DWORD aConnectionSessionID)
{
	int connectionResult = CONNECTION_NO_ERROR;
	if( !m_bRuning)
	{
		return CONNECTION_START_CLIENT_IS_DESTROY_ERROR;
	}
	MP_NEW_P3(params, ConnectThreadParams, aConnection, aServerInfo, m_pLW);

	CServerInfo serverInfo = *aServerInfo;

	CSimpleThreadStarter connectThreadStarter(__FUNCTION__);	
	connectThreadStarter.SetRoutine(connectThread_);
	connectThreadStarter.SetParameter(params);
	HANDLE connectThreadHandle = params->connectThreadHandle = connectThreadStarter.Start();

	if (connectThreadHandle == NULL)
	{
		if (m_pLW != NULL)
		{
			CLogValue log("[CONNECT_WAIT_THREAD] CreateConnectThread error = ", GetLastError(), " for server = ", serverInfo.sIP.GetBuffer()," port = ", serverInfo.tcpPort);
			m_pLW->WriteLn(log);
		}

		return CONNECTION_START_CONNECTION_THREAD_ERROR;
	}

	HANDLE arr[2];
	arr[0] = connectThreadHandle;
	m_csConnections.lock();
	arr[1] = m_infoConnections[aConnectionSessionID];
	m_csConnections.unlock();

	DWORD waitResult = WaitForMultipleObjects(2, &arr[0], FALSE, CONNECT_TIMEOUT);
	if (m_pLW != NULL)
	{
		CLogValue log("[CONNECT_WAIT_THREAD] waitResult = ", waitResult, " for server = ", serverInfo.sIP.GetBuffer()," port = ", serverInfo.tcpPort);
		m_pLW->WriteLn(log);
	}
	if (waitResult == WAIT_TIMEOUT || waitResult == WAIT_OBJECT_0 + 1 || waitResult == WAIT_FAILED) // не дождались || abortConnection || ошибка ожидания
	{

		aConnection->shutdown();
		WaitForSingleObject(connectThreadHandle, INFINITE);
		connectionResult = CONNECTION_CONNECT_ERROR;
	}
	else if (waitResult == WAIT_OBJECT_0) // дождались поток подключения
	{
		DWORD exitCode = 0;
		GetExitCodeThread(connectThreadHandle, &exitCode);
		connectionResult = exitCode;
	}
	CloseHandle(connectThreadHandle);

	aSocketErrorCode = aConnection->getErrorCode();

	if (connectionResult != CONNECTION_NO_ERROR)
	{
		aConnection->shutdown();
		aConnection->destroy();
	}

	return connectionResult;
}

void CConnectionManager::setNetworkBreaksEmulationDelay( int auDelay, int auConnectionsMask)
{
	m_connectionBreakEmulationTimeout = auDelay;
	m_connectionBreakEmulationMask = auConnectionsMask;
	if( auDelay == 0)
	{
		if( IsRunning())
		{
			m_eventConnectionBreakEmulation.SetEvent();
			Stop();
			m_eventConnectionBreakEmulation.ResetEvent();
		}
	}
	else
	{
		if( !IsRunning())
			Start();
	}
}

// реализация CThreadImpl<CConnectionManager>
DWORD CConnectionManager::Run()
{
	while( !ShouldStop())
	{
		if( m_connectionBreakEmulationTimeout > 0)
		{
			DWORD waitResult = WaitForSingleObject( m_eventConnectionBreakEmulation, m_connectionBreakEmulationTimeout);
			if( waitResult == WAIT_TIMEOUT)
			{
				Invoke<&iclientsession::EmulateBreak>( /*m_connectionBreakEmulationMask*/);
				m_connectionBreakEmulationTimeout = 0;
			}
		}
		else
		{
			Sleep( 1000);
		}
	}
	return 0;
}

DWORD WINAPI CConnectionManager::connectThread_( VOID* param)
{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();
	ConnectThreadParams* pParams  = (ConnectThreadParams*)param;

	if (pParams->pLW != NULL)
	{
		//CLogValue log("[CONNECT_THREAD] start connecting for server = ", pParams->serverInfo.sIP.GetBuffer(), " port = ", pParams->serverInfo.tcpPort);
		//pParams->pLW->WriteLn(log);
	}

	int socketErrorCode = 0;
	int connectResult = pParams->connection->connect(&(pParams->serverInfo), pParams->pLW, socketErrorCode);
	pParams->connection->setErrorCode(socketErrorCode);

	if (pParams->pLW != NULL)
	{
		//CLogValue log("[CONNECT_THREAD] end connecting for server = ", pParams->serverInfo.sIP.GetBuffer(), " port = ", pParams->serverInfo.tcpPort, " with socketErrorCode = ", socketErrorCode);
		//pParams->pLW->WriteLn(log);
	}

	pParams->connection->destroy();
	pParams->destroy();

	return connectResult;
}

void CConnectionManager::PreDestroy()
{
	setILogWriter(NULL);

	m_bRuning = false;
	closeAllConnection();
}

void CConnectionManager::closeAllConnection()
{
	m_csConnections.lock();
	for (ConnectionInfoMap::iterator it = m_infoConnections.begin(); it != m_infoConnections.end(); ++it)
	{
		if (it->second != NULL)
			SetEvent(it->second);
	}
	m_csConnections.unlock();
}

void CConnectionManager::SubscribeHttpSettingsChanged(cm::IChangeHttpSettings* aSubscriber)
{
	m_httpSettingsChangedNotifyer.Lock();
	m_httpSettingsChangedNotifyer.Subscribe(aSubscriber);
	m_httpSettingsChangedNotifyer.UnLock();

	notifyHttpSettingsChanged(true);
}

void CHttpProxySettingsChange::NotifyHttpSettingsChanged(ProxyMode* aProxyMode, IProxySettings* aProxySettings)
{
	Lock();
	InvokeEx2<void*, void*,	&cm::IChangeHttpSettings::OnChangeConnectionSettings>((void*)(void*)(aProxyMode),(aProxySettings));
	UnLock();
}

void CConnectionManager::UnsubscribeHttpSettingsChanged(cm::IChangeHttpSettings* aSubscriber)
{
	m_httpSettingsChangedNotifyer.Lock();
	m_httpSettingsChangedNotifyer.Unsubscribe(aSubscriber);
	m_httpSettingsChangedNotifyer.UnLock();
}

void CConnectionManager::SubscribeToNotify(ichangeconnection* aSubscriber)
{
	m_settingsChangedNotifyer.Lock();
	m_settingsChangedNotifyer.Subscribe(aSubscriber);
	m_settingsChangedNotifyer.UnLock();

	m_ProxySettingsLock.lock();
	if (m_httpsProxySettingsList.size() > 0)
	{			
		m_settingsChangedNotifyer.NotifySettingsChanged(&m_httpsProxySettingsList);
	}
	if (m_socksProxySettingsList.size() > 0) 
	{
		m_settingsChangedNotifyer.NotifySettingsChanged(&m_socksProxySettingsList);		
	}
	m_ProxySettingsLock.unlock();
}

void CConnectionManager::UnSubscribeFromNotify(ichangeconnection* aSubscriber)
{
	m_settingsChangedNotifyer.Lock();
	m_settingsChangedNotifyer.Unsubscribe(aSubscriber);
	m_settingsChangedNotifyer.UnLock();
}

void CProxySettingsChange::NotifySettingsChanged(std::vector<IProxySettings*>* aProxySettingsList)
{
	Lock();
	InvokeEx<void*, &cm::ichangeconnection::OnChangeConnectionType>((void*)(aProxySettingsList));
	UnLock();
}

ProxyMode CConnectionManager::getHttpProxyMode()
{
	return m_httpProxyMode;
}

IProxySettings* CConnectionManager::getHttpProxySettings()
{
	return (IProxySettings*)&m_httpProxySettings;
}

IProxySettings* CConnectionManager::getHttpsProxySettings()
{
	IProxySettings* proxySettings = NULL;

	m_ProxySettingsLock.lock();
	if (m_isSuccessProxySettingsExist && m_firstSuccessProxySettings.getType() == PT_HTTPS && m_firstSuccessProxySettings.enabled)
	{
		m_httpsProxySettings = m_firstSuccessProxySettings;
		proxySettings = &m_httpsProxySettings;
	}
	m_ProxySettingsLock.unlock();

	return proxySettings;
}
