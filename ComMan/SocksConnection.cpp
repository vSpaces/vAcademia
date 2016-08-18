#include "stdafx.h"
#include "SocksConnection.h"

CSocksConnection::CSocksConnection(ProxySettings* aProxyConfig, unsigned int aDestroyCallCount)
:CSocketConnection( aDestroyCallCount)
{
	m_pLW = NULL;

	cs = MP_NEW(CSocksSocket);

	cs->SetVersion(aProxyConfig->type);

	USES_CONVERSION;
	cs->SetSocksAddress(W2A(aProxyConfig->ip.c_str()));
	cs->SetSocksPort(aProxyConfig->port);

	cs->SetAuthentication(aProxyConfig->useUser);
	cs->SetUserName(W2A(aProxyConfig->user.c_str()));
	cs->SetPassword(W2A(aProxyConfig->password.c_str()));
	cs->SetSocksInterval(1);
	m_errorCode = 0;
}

CSocksConnection::~CSocksConnection(void)
{
	MP_DELETE(cs);
}

int CSocksConnection::connect(CServerInfo *pServerInfo, ILogWriter* pLW, SOCKERR& errorCode)
{
	m_pLW = pLW;
	if (m_pLW != NULL)
		m_pLW->WriteLn("Start connecting to CSocksConnection");

	errorCode = 0;

	cs->SetLW(m_pLW);
	cs->SetDestinationPort(pServerInfo->tcpPort);
	cs->SetDestinationAddress(pServerInfo->sIP.GetBuffer());

	return cs->Connect(&m_socket, &m_connectSocket, errorCode);
}

void CSocksConnection::_destroy()
{
	MP_DELETE_THIS;
}

void CSocksConnection::setILogWriter(ILogWriter* pLW)
{
	m_pLW = pLW;
	cs->SetLW(pLW);
}
