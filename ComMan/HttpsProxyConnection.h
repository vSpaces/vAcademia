#pragma once
#include "SocketConnection.h"
#include "ProxySettings.h"

#include <string>

class CHttpsProxyConnection:
	public CSocketConnection
{
public:
	CHttpsProxyConnection(ProxySettings* aProxyConfig, unsigned int aDestroyCallCount);
	~CHttpsProxyConnection(void);

	int connect(CServerInfo *pServerInfo, ILogWriter* pLW, SOCKERR& errorCode);
	void _destroy();
	void setILogWriter(ILogWriter* pLW);

private:
	std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
private:
	ProxySettings m_proxyConfig;
};
