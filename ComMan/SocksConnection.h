#pragma once
#include "SocketConnection.h"
#include "SocksSocket.h"
#include "ProxySettings.h"

class CSocksConnection :
	public CSocketConnection
{
public:
	CSocksConnection(ProxySettings* aProxyConfig, unsigned int aDestroyCallCount);
	~CSocksConnection(void);

	int connect(CServerInfo *pServerInfo, ILogWriter* pLW, SOCKERR& errorCode);
	void _destroy();
	void setILogWriter(ILogWriter* pLW);

private:
	CSocksSocket* cs;
};
