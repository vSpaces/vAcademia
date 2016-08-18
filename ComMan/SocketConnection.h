#pragma once
#include "IConnection.h"
#include "ProxySettings.h"
#include "Mutex.h"
#include "DestroyableConnection.h"

class CSocketConnection :
	public CDestroyableConnection
{
public:
	CSocketConnection( unsigned int aDestroyCallCount);
	~CSocketConnection(void);

	virtual int connect(CServerInfo *pServerInfo, ILogWriter* pLW, SOCKERR& errorCode)=0;

	virtual void setILogWriter(ILogWriter* pLW);

	virtual int send( const char * buf, int len, int flags);
	virtual int recv( char * buf, int len, int flags);

	virtual void setErrorCode(int errorCode);
	virtual int getErrorCode();

	int shutdown();

	bool isSocketValid();
	long getSocketHandle();

	static int _connect( SOCKET aSocket, SOCKADDR * aAddress, int aSockAddrSize);

protected:
	SOCKET			m_socket;
	SOCKET			m_connectSocket;
	ILogWriter*		m_pLW;
	int				m_errorCode;
	CMutex			m_shutdownLock;
};
