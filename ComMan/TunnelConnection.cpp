#include "stdafx.h"
#include "TunnelConnection.h"

CTunnelConnection::CTunnelConnection(ProxySettings* aProxyConfig, unsigned int aDestroyCallCount)
: CDestroyableConnection(aDestroyCallCount)
{
	MP_NEW_P(tunnelClient, CTunnelClient, aProxyConfig);
	m_pTunnelClient = tunnelClient;
	m_pTunnelSession = NULL;
	m_pLW = NULL;
	m_errorCode = 0;
}

CTunnelConnection::~CTunnelConnection(void)
{
	shutdown();

	if (m_pTunnelClient != NULL)
	{
		shutdown();
		m_pTunnelClient->destroy();
		m_pTunnelClient = NULL;
	}
}

int CTunnelConnection::connect(CServerInfo* pServerInfo, ILogWriter* pLW, SOCKERR& errorCode)
{
	m_serverInfo = *pServerInfo;
	m_pLW = pLW;

	if (m_pLW != NULL)
	{
		CLogValue log("CTunnelConnection::connect to ", pServerInfo->sIP.GetBuffer(), " ", pServerInfo->tcpPort);
		m_pLW->WriteLn(log);
	}

	return m_pTunnelClient->connect(&m_serverInfo, pLW, errorCode, m_pTunnelSession);
}

int CTunnelConnection::shutdown()
{
	int result = -1;

	if (m_pTunnelSession != NULL)
	{
		TunnelSession* tunnelSession = m_pTunnelSession;
		m_pTunnelSession = NULL;
		result = m_pTunnelClient->shutdown(tunnelSession);
	}
	
	return result;
}

bool CTunnelConnection::isSocketValid()
{
	if (m_pTunnelSession != NULL)
		return m_pTunnelClient->isSocketValid(m_pTunnelSession);

	return false;
}

long CTunnelConnection::getSocketHandle()
{
	if (m_pTunnelSession != NULL)
		return m_pTunnelClient->getSocketHandle(m_pTunnelSession);

	return 0;
}

int CTunnelConnection::send( const char * buf, int len, int flags)
{
	if (m_pTunnelSession != NULL)
		return m_pTunnelClient->send( m_pTunnelSession, (const char*)buf, len, flags);

	return 0;
}

int CTunnelConnection::recv( char * buf, int len, int flags)
{
	if (m_pTunnelSession != NULL)
		return m_pTunnelClient->recv( m_pTunnelSession, buf, len, flags);

	return 0;
}

void CTunnelConnection::_destroy()
{
	MP_DELETE_THIS;
}

void CTunnelConnection::setILogWriter(ILogWriter* pLW)
{
	m_pLW = pLW;
}


void CTunnelConnection::setErrorCode(int errorCode)
{
	m_errorCode = errorCode;
}

int CTunnelConnection::getErrorCode()
{
	return m_errorCode;
}
