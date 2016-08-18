#include "stdafx.h"
#include "TunnelClient.h"

CTunnelClient::CTunnelClient(ProxySettings* aProxyConfig):
	MP_VECTOR_INIT(m_servers)
{
	m_proxyConfig = *aProxyConfig;
	m_lastSessionID = 0;
	m_pLW = NULL;
	m_signature = 0;
	InitializeCriticalSection(&m_csServerList);
}

CTunnelClient::~CTunnelClient(void)
{
	EnterCriticalSection(&m_csServerList);

	for (ServerListType::iterator it = m_servers.begin(); it != m_servers.end(); ++it)
	{
		it->second->destroy();
	}

	m_servers.clear();

	LeaveCriticalSection(&m_csServerList);

	DeleteCriticalSection(&m_csServerList);
}

int CTunnelClient::connect(CServerInfo *pServerInfo, ILogWriter* pLW, SOCKERR& errorCode, TunnelSession*& pTunnelSession)
{
	pTunnelSession = createSession(pServerInfo);

	m_pLW = pLW;

	if (m_pLW != NULL)
	{
		CLogValue log("CTunnelClient::connect");
		m_pLW->WriteLn(log);
	}

	CServerInfo httpTunnelServerInfo;
	httpTunnelServerInfo = *pServerInfo;
	httpTunnelServerInfo.tcpPort = 443;

	int connectResultCode = CONNECTION_CONNECT_ERROR;

	//m_MutexServer.lock();
	if (pTunnelSession != NULL)
		connectResultCode = connectToServer(&httpTunnelServerInfo, pTunnelSession, errorCode);
	//m_MutexServer.unlock();

	if (connectResultCode != CONNECTION_NO_ERROR)
		destroySession(pTunnelSession);

	return connectResultCode;
}

int CTunnelClient::shutdown(TunnelSession*& pTunnelSession)
{
	destroySession(pTunnelSession);

	return 0;
}

int CTunnelClient::send( TunnelSession* pTunnelSession, const char * buf, int len, int flags)
{
	if (pTunnelSession->pServer != NULL)
		return pTunnelSession->pServer->send(buf, len, flags, pTunnelSession->serverInfo.tcpPort);

	return 0;
}

int CTunnelClient::recv( TunnelSession* pTunnelSession, char * buf, int len, int flags)
{
	if (pTunnelSession->pServer != NULL)
		return pTunnelSession->pServer->recv(buf, len, flags, pTunnelSession->serverInfo.tcpPort);

	return 0;
}

bool CTunnelClient::isSocketValid(TunnelSession* pTunnelSession)
{
	return pTunnelSession != NULL;
}

long CTunnelClient::getSocketHandle(TunnelSession* pTunnelSession)
{
	if (pTunnelSession->pServer != NULL)
		return pTunnelSession->pServer->getSocketHandle();

	return 0;
}

void CTunnelClient::destroy()
{
	MP_DELETE_THIS;
}

TunnelSession* CTunnelClient::createSession(CServerInfo *pServerInfo)
{
	m_MutexSession.lock();
	MP_NEW_P3(pTunnelSession, TunnelSession, pServerInfo, NULL, ++m_lastSessionID);
	m_MutexSession.unlock();
	return pTunnelSession;
}

void CTunnelClient::destroySession(TunnelSession*& pTunnelSession)
{
	m_MutexDestroy.lock();
	if (pTunnelSession != NULL)
	{
		if (pTunnelSession->pServer != NULL)
		{
			CTunnelServer* pServer = pTunnelSession->pServer;
			pTunnelSession->pServer = NULL;
			if (pServer != NULL)
				pServer->shutdown(pTunnelSession->serverInfo.tcpPort);
		}

		m_MutexSession.lock();
		if (pTunnelSession != NULL)
			MP_DELETE(pTunnelSession);
		m_MutexSession.unlock();
	}
	m_MutexDestroy.unlock();
}

int CTunnelClient::connectToServer(CServerInfo *pServerInfo, TunnelSession*& pTunnelSession, SOCKERR& errorCode)
{
	EnterCriticalSection(&m_csServerList);

	if (m_pLW != NULL)
	{
		CLogValue log("CTunnelClient::connectToServer");
		m_pLW->WriteLn(log);
	}

	CTunnelServer* pTunnelServer = NULL;

	if (m_servers.find(pServerInfo->sIP) != m_servers.end())
	{
		pTunnelServer = m_servers[pServerInfo->sIP];
	}
	else
	{
		MP_NEW_V(pTunnelServer, CTunnelServer, &m_proxyConfig);
		m_servers[pServerInfo->sIP] = pTunnelServer;
	}

	m_MutexDestroy.lock();
	if (pTunnelSession != NULL)
		pTunnelSession->pServer = pTunnelServer;
	m_MutexDestroy.unlock();

	LeaveCriticalSection(&m_csServerList);

	/*if (m_pLW != NULL)
	{
		CLogValue log("CTunnelServer go to connect");
		m_pLW->WriteLn(log);
	}*/

	int connectResult = CONNECTION_SOCKET_ERROR;

	m_MutexSession.lock();
	if (pTunnelSession != NULL && pTunnelSession->pServer != NULL)
		connectResult = pTunnelSession->pServer->connect(pServerInfo, m_pLW, errorCode, pTunnelSession->serverInfo.tcpPort);
	m_MutexSession.unlock();

	return connectResult;
}
