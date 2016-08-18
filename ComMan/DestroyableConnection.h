#pragma once
#include "Mutex.h"
#include "IConnection.h"

class CDestroyableConnection : public IConnection
{
public:
	CDestroyableConnection(unsigned int aDestroyCallCount);
	~CDestroyableConnection(void);

	virtual int connect(CServerInfo* pServerInfo, ILogWriter* pLW, SOCKERR& errorCode)=0;
	virtual int shutdown()=0;

	virtual int send( const char * buf, int len, int flags)=0;
	virtual int recv( char * buf, int len, int flags)=0;

	virtual bool isSocketValid()=0;
	virtual long getSocketHandle()=0;

	virtual void destroy();
	virtual void _destroy()=0;
	virtual void setILogWriter(ILogWriter* pLW)=0;

	virtual void setErrorCode(int errorCode)=0;
	virtual int getErrorCode()=0;

private: 
	int		m_destroyCount;
	CMutex	m_lock;
};
