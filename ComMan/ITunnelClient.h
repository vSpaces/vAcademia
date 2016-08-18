#pragma once

#include "stdafx.h"
#include <Ws2tcpip.h>
#include "IConnection.h"
#include "TunnelServer.h"

struct TunnelSession // сессия для случая когда на один сервер будет несколько сокетных соединений
{
	TunnelSession()
	{
		pServer = NULL;
		sessionID = 0;
		destroy = false;
	}
	TunnelSession(CServerInfo* pServerInfo, CTunnelServer* pServer, unsigned int aSessionID)
	{
		this->serverInfo = *pServerInfo;
		this->pServer = pServer;
		this->sessionID = aSessionID;
		destroy = false;
	}

	CServerInfo		serverInfo;
	CTunnelServer*	pServer;
	unsigned int	sessionID;
	bool			destroy;
};

struct ITunnelClient
{
	virtual int connect(CServerInfo *pServerInfo, ILogWriter* pLW, SOCKERR& errorCode, TunnelSession*& pTunnelSession /* out */)=0;
	virtual int shutdown(TunnelSession*& pTunnelSession)=0;

	virtual int send( TunnelSession* pTunnelSession, const char * buf, int len, int flags)=0;
	virtual int recv( TunnelSession* pTunnelSession, char * buf, int len, int flags)=0;

	virtual bool isSocketValid(TunnelSession* pTunnelSession)=0;
	virtual long getSocketHandle(TunnelSession* pTunnelSession)=0;

	virtual void destroy()=0;
};