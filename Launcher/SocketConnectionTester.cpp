#include "StdAfx.h"
#include "SocketConnectionTester.h"
#include "ClientSessionStateSender.h"
#include <ws2ipdef.h>

extern unsigned int	gSocketConnectionStatus;
extern unsigned int	gSocketConnectionErrorCode;

CSocketConnectionTester::CSocketConnectionTester(void)
{
}

CSocketConnectionTester::~CSocketConnectionTester(void)
{
}

void	CSocketConnectionTester::StartSocketConnection()
{
	Start();
}

void	CSocketConnectionTester::StopSocketConnection()
{
	if( IsRunning())
	{
		Terminate(0);
	}
}

DWORD CSocketConnectionTester::Run()
{
	gSocketConnectionStatus = 1;

	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD( 2, 0 );

	if ( (gSocketConnectionErrorCode = WSAStartup( wVersionRequested, &wsaData )) != 0)
	{
		gSocketConnectionStatus = 2;
		return 0;
	}

	std::string serverIP = CClientSessionStateSender::GetPropertyFromPlayerINI("paths","server");
	gSocketConnectionStatus = 3;

	SOCKERR serr = 0;
	SOCKET sck = socket(AF_INET, SOCK_STREAM, 0);;
	gSocketConnectionStatus = 4;
	if( sck == INVALID_SOCKET)
	{
		gSocketConnectionStatus = 5;
		gSocketConnectionErrorCode = WSAGetLastError();
		return 0;
	}

	gSocketConnectionStatus = 6;

	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr( serverIP.c_str());
	sockAddr.sin_port = htons( 13333);

	gSocketConnectionStatus = 7;

	int tos = 8;
	if(setsockopt(sck, IPPROTO_IP, IP_TOS, (LPCSTR)&tos, sizeof(tos)) == SOCKET_ERROR) 
	{
		gSocketConnectionStatus = 8;
		gSocketConnectionErrorCode = WSAGetLastError();
		return 0;
	}

	if (::connect( sck, (SOCKADDR *)&sockAddr, sizeof(sockAddr)) == 0) 
	{
		gSocketConnectionStatus = 9;
		gSocketConnectionErrorCode = 0;
		closesocket( sck);
	}
	else
	{
		gSocketConnectionStatus = 10;
		gSocketConnectionErrorCode = WSAGetLastError();
	}

	return 0;
}