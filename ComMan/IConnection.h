#pragma once

#include "stdafx.h"
#include "serverInfo.h"
#include <Ws2tcpip.h>

using namespace cs;

#define CONNECTION_NO_ERROR									0
#define CONNECTION_SOCKET_ERROR								1
#define CONNECTION_PROXY_AUTH_ERROR							2
#define CONNECTION_PROXY_SERVER_ERROR						3
#define CONNECTION_REMOTE_CONNECT_ERROR						4
#define CONNECTION_GET_HOST_ERROR							5
#define CONNECTION_CREATE_SOCKET_ERROR						6
#define CONNECTION_INVALID_SOCKS_VERSION_ERROR				7
#define CONNECTION_CONNECT_ERROR							8
#define CONNECTION_SOCKS_SERVER_PROTOCOL_ERROR				9
#define CONNECTION_WSA_STARTUP_ERROR						10
#define CONNECTION_START_CONNECTION_THREAD_ERROR			11
#define CONNECTION_START_CLIENT_IS_DESTROY_ERROR			12

#define CONNECT_TIMEOUT 30000

struct IConnection
{
	virtual int connect(CServerInfo* pServerInfo, ILogWriter* pLW, SOCKERR& errorCode)=0;
	virtual int shutdown()=0;

	virtual int send( const char * buf, int len, int flags)=0;
	virtual int recv( char * buf, int len, int flags)=0;

	virtual bool isSocketValid()=0;
	virtual long getSocketHandle()=0;

	virtual void destroy()=0;
	virtual void setILogWriter(ILogWriter* pLW)=0;

	virtual void setErrorCode(int errorCode)=0;
	virtual int getErrorCode()=0;
};