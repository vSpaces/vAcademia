#pragma once

#include "ITunnelClient.h"
#include <map>
#include "WinSocketConnection.h"
#include "ITunnelPacket.h"
#include "HttpsPacketOut.h"

typedef MP_MAP<CComString, CTunnelServer*> ServerListType;

class CTunnelClient: public ITunnelClient
{
public:
	CTunnelClient(ProxySettings* aProxyConfig);
	~CTunnelClient(void);

	int connect(CServerInfo *pServerInfo, ILogWriter* pLW, SOCKERR& errorCode, TunnelSession*& pTunnelSession);
	int shutdown(TunnelSession*& pTunnelSession);

	int send( TunnelSession* pTunnelSession, const char * buf, int len, int flags);
	int recv( TunnelSession* pTunnelSession, char * buf, int len, int flags);

	bool isSocketValid(TunnelSession* pTunnelSession);
	long getSocketHandle(TunnelSession* pTunnelSession);

	void destroy();

private:
	TunnelSession* createSession(CServerInfo *pServerInfo);
	void destroySession(TunnelSession*& pTunnelSession);

	int connectToServer(CServerInfo *pServerInfo, TunnelSession*& pTunnelSession, SOCKERR& errorCode);

private:
	ILogWriter*		m_pLW;

	unsigned short	m_signature;
	unsigned int	m_lastSessionID;

	ServerListType		m_servers;

	CRITICAL_SECTION	m_csServerList;

	ProxySettings		m_proxyConfig;

	CMutex				m_MutexDestroy;
	CMutex				m_MutexSession;
};
