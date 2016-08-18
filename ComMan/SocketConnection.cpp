#include "stdafx.h"
#include "SocketConnection.h"

CSocketConnection::CSocketConnection( unsigned int aDestroyCallCount)
 : CDestroyableConnection(aDestroyCallCount)
{
	m_socket = INVALID_SOCKET;
	// создаем здесь, чтобы если будет отмена соединения, то все операции по работе с сокетом прервались и если сокет будет создаваться позже то будет дедлок
	m_connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	m_pLW = NULL;
	m_errorCode = 0;
}

CSocketConnection::~CSocketConnection(void)
{
	shutdown();
	m_pLW = NULL;
}

int CSocketConnection::shutdown()
{
	if (m_pLW != NULL)
		m_pLW->WriteLn("CSocketConnection::shutdown");

	int result = 0;

	m_shutdownLock.lock();
	if (m_socket != INVALID_SOCKET)
	{
		result = ::shutdown( m_socket, SD_BOTH);
		closesocket( m_socket);
		m_socket = INVALID_SOCKET;
	}

	if (m_connectSocket != INVALID_SOCKET)
	{
		result = ::shutdown( m_connectSocket, SD_BOTH);
		closesocket( m_connectSocket);
		m_connectSocket = INVALID_SOCKET;
	}
	m_shutdownLock.unlock();

	return result;
}

bool CSocketConnection::isSocketValid()
{
	return m_socket != INVALID_SOCKET;
}

long CSocketConnection::getSocketHandle()
{
	return (long)m_socket;
}

int CSocketConnection::recv( char * buf, int len, int flags)
{
	return ::recv( m_socket, buf, len, flags);
}

int CSocketConnection::send(	const char * buf, int len, int flags)
{
	return ::send( m_socket, buf, len, flags);
}

void CSocketConnection::setILogWriter(ILogWriter* pLW)
{
	m_pLW = pLW;
}

void CSocketConnection::setErrorCode(int errorCode)
{
	m_errorCode = errorCode;
}

int CSocketConnection::getErrorCode()
{
	return m_errorCode;
}

int CSocketConnection::_connect( SOCKET aSocket, SOCKADDR * aAddress, int aSockAddrSize)
{
	return ::connect(aSocket, aAddress, aSockAddrSize);

/*
	//set the socket in non-blocking
	unsigned long iMode = 1;

	int iResult = ioctlsocket(aSocket, FIONBIO, &iMode);
	if (iResult != NO_ERROR)
	{	
		if (aLW != NULL)
			aLW->WriteLn("ioctlsocket failed with error: ", iResult);

		return -1;
	}

	// connect
	iResult = ::connect(aSocket, aAddress, aSockAddrSize);


	if (aLW != NULL)
		aLW->WriteLn("connect return: ", iResult);

	if ( iResult == 0)
	{	
		return 0;
	}

	// restart the socket mode
	iMode = 0;
	iResult = ioctlsocket(aSocket, FIONBIO, &iMode);
	if (iResult != NO_ERROR)
	{	
		if (aLW != NULL)
			aLW->WriteLn("ioctlsocket failed with error: ", iResult);

		return -1;
	}

	// check socket for ready
	TIMEVAL Timeout;
	Timeout.tv_sec = CONNECT_TIMEOUT / 1000;
	Timeout.tv_usec = 0;

	fd_set err;
	FD_ZERO(&err);
	FD_SET(aSocket, &err);

	// check if the socket is ready
	if ( (iResult = select( 0, NULL, NULL, &err, &Timeout)) != 1)
	{
		if (aLW != NULL)
			aLW->WriteLn("select failed with error: ", iResult);

		return -1;
	}

	if (err.fd_count != 0)
		return -1;
	
	return 0;*/
}