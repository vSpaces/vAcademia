#include "stdafx.h"
#include "WinSocketConnection.h"
#include "Winsock2.h"
#include "Mstcpip.h"

//#define TCP_KEEPALIVE  0x02    /* send KEEPALIVE probes when idle for pcb->keep_idle milliseconds */

CWinSocketConnection::CWinSocketConnection( unsigned int aDestroyCallCount)
:CSocketConnection( aDestroyCallCount)
{
	m_pLW = NULL;
}

CWinSocketConnection::~CWinSocketConnection(void)
{
}

int CWinSocketConnection::connect(CServerInfo *pServerInfo, ILogWriter* pLW, SOCKERR& errorCode)
{
	m_pLW = pLW;

	if ( m_connectSocket == INVALID_SOCKET)
	{
		errorCode = WSAGetLastError();
		return CONNECTION_SOCKET_ERROR;
	}

	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr( pServerInfo->sIP.GetBuffer());
	sockAddr.sin_port = htons( pServerInfo->tcpPort);

	if (_connect( m_connectSocket, (SOCKADDR *)&sockAddr, sizeof(sockAddr)) == 0) 
	{
		errorCode = 0;

		if( !SetSocketKeepAlive( m_connectSocket, TRUE))
		{
			if(m_pLW != NULL)
				m_pLW->WriteLn("SetSocketKeepAlive FAILED for server ", pServerInfo->sIP.GetBuffer(), " port ", pServerInfo->tcpPort);
		}
		else
		{
			if(m_pLW != NULL)
				m_pLW->WriteLn("SetSocketKeepAlive SUCCESS for server ", pServerInfo->sIP.GetBuffer(), " port ", pServerInfo->tcpPort);
		}

		if( !SetSocketKeepAliveValue( m_connectSocket, 60000/*60 секунд держим соединение даже если данных нет*/, 5000))
		{
			if(m_pLW != NULL)
				m_pLW->WriteLn("SetSocketKeepAliveValue FAILED for server ", pServerInfo->sIP.GetBuffer(), " port ", pServerInfo->tcpPort);
		}
		else
		{
			if(m_pLW != NULL)
				m_pLW->WriteLn("SetSocketKeepAliveValue SUCCESS for server ", pServerInfo->sIP.GetBuffer(), " port ", pServerInfo->tcpPort);
		}

		m_socket = m_connectSocket;
		m_connectSocket = INVALID_SOCKET;

		if( m_pLW != NULL)
			m_pLW->WriteLn("CWinSocketConnection CONNECT SUCCESS for server ", pServerInfo->sIP.GetBuffer(), " port ", pServerInfo->tcpPort);


		return CONNECTION_NO_ERROR;
	}

	errorCode = WSAGetLastError();

	if(m_pLW != NULL)
		m_pLW->WriteLn("CWinSocketConnection CONNECT FAILED for server ", pServerInfo->sIP.GetBuffer(), " port ", pServerInfo->tcpPort, " errorCode = ", errorCode);

	return CONNECTION_SOCKET_ERROR;
}

bool CWinSocketConnection::SetSocketKeepAlive(SOCKET hSocket, BOOL bKeepAlive)
{
	if (SOCKET_ERROR == setsockopt(hSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&bKeepAlive, sizeof(bKeepAlive)))
		return false;
	return true;
}

bool CWinSocketConnection::SetSocketKeepAliveValue(SOCKET hSocket, unsigned long nKeepAliveTime, unsigned long nKeepAliveInterval)
{
	tcp_keepalive inKeepAlive	= {0};
	tcp_keepalive outKeepAlive	= {0};
	unsigned long nInLen	= sizeof(tcp_keepalive);
	unsigned long nOutLen	= sizeof(tcp_keepalive);
	unsigned long nBytesReturn = 0;

	//
	inKeepAlive.onoff = 1;
	inKeepAlive.keepalivetime = nKeepAliveTime;
	inKeepAlive.keepaliveinterval = nKeepAliveInterval;

	//
	if ( SOCKET_ERROR == WSAIoctl(hSocket, SIO_KEEPALIVE_VALS,(LPVOID)&inKeepAlive, nInLen,(LPVOID)&outKeepAlive, nOutLen, &nBytesReturn, NULL, NULL))
		return false;
	return true;
}

void CWinSocketConnection::_destroy()
{
	MP_DELETE_THIS;
}

void CWinSocketConnection::setILogWriter(ILogWriter* pLW)
{
	m_pLW = pLW;
}