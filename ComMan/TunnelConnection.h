#pragma once

#include "IConnection.h"
#include "ThreadAffinity.h"
#include "Mutex.h"
#include "TunnelClient.h"
#include "DestroyableConnection.h"

class CTunnelConnection:
	public CDestroyableConnection
{
public:
	CTunnelConnection(ProxySettings* aProxyConfig, unsigned int aDestroyCallCount);
	~CTunnelConnection(void);

public:
	int connect(CServerInfo *pServerInfo, ILogWriter* pLW, SOCKERR& errorCode);
	int shutdown();
	void setILogWriter(ILogWriter* pLW);

	int send( const char * buf, int len, int flags);
	int recv( char * buf, int len, int flags);

	void _destroy();

	bool isSocketValid();
	long getSocketHandle();

	void setErrorCode(int errorCode);
	int getErrorCode();

private:
	ILogWriter*		m_pLW;
	CServerInfo		m_serverInfo;

	ITunnelClient*	m_pTunnelClient;
	TunnelSession*	m_pTunnelSession;

	int				m_errorCode;
};
