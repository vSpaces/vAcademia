#pragma once
#include "SocketConnection.h"

class CSocketConnection;

class CWinSocketConnection :
	public CSocketConnection
{
public:
	CWinSocketConnection( unsigned int aDestroyCallCount);
	~CWinSocketConnection(void);

	int connect(CServerInfo *pServerInfo, ILogWriter* pLW, SOCKERR& errorCode);
	void _destroy();
	void setILogWriter(ILogWriter* pLW);
protected:

	bool SetSocketKeepAlive(SOCKET hSocket, BOOL bKeepAlive);
	bool SetSocketKeepAliveValue(SOCKET hSocket, unsigned long nKeepAliveTime, unsigned long nKeepAliveInterval);
};
