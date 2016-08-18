// ClientSession.cpp: implementation of the CClientSession class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WinSock2.h"
#include "ClientSession.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define LOCAL_IP_STRING	("127.0.0.1")

#define DESIRED_WINSOCK_VERSION 0x0101  // we'd like winsock ver 1.1...
#define MINIMUM_WINSOCK_VERSION 0x0001  // ...but we'll take ver 1.0

typedef INT	SOCKERR;        			// A socket error code

// Конструктор
CClientSession::CClientSession()
{
	sckServer = INVALID_SOCKET;
	bRunning = true;
	hSendEvent = CreateEvent( NULL, TRUE, FALSE, NULL);
	dwBufferCapacity = 0;
	dwBufferSize = 0;
	InitializeCriticalSection( &csSendQueue);
	InitializeCriticalSection( &csBuffer);
}

// Деструктор
CClientSession::~CClientSession()
{
	Close();
	bRunning = false;
	SetEvent( hSendEvent);

	if (WaitForSingleObject( hSendThread, 2000) == WAIT_TIMEOUT)
		TerminateThread( hSendThread, 0);
	hSendThread = NULL;

	if (WaitForSingleObject( hRecvThread, 2000) == WAIT_TIMEOUT)
		TerminateThread( hRecvThread, 0);
	hRecvThread = NULL;

	DeleteCriticalSection( &csBuffer);
	DeleteCriticalSection( &csSendQueue);
	CloseHandle( hSendEvent);
}

// Открывает сессию
bool CClientSession::Open( LPCSTR aSocketAddress, UINT aSocketPort, unsigned long long aSessionID)
{
	Close();

	if (!CreateSocket( aSocketAddress, aSocketPort))
	{
		return false;
	}
	
	Send( 0, (BYTE*)&aSessionID, sizeof( aSessionID));

	return true;
}

bool IsIPAddress( LPCSTR alpHost)
{
	ATLASSERT( alpHost);
	int strLength = strlen( alpHost);
	if( strLength == 0)	return false;
	for ( int i=0; i<strLength; i++)
	{
		char ch = alpHost[i];
		if( ch!='.' || (ch < '0' && ch > '9'))	return false;
	}
	return true;
}

CComString GetIPViaHostName( LPCSTR alpHost)
{
	ATLASSERT( alpHost);
	if( !alpHost)	return LOCAL_IP_STRING;

	CComString shost(alpHost);
	WORD wVersionRequested;
	WSADATA wsaData;
	PHOSTENT hostinfo;
	CComString	ip( alpHost);
	wVersionRequested = MAKEWORD( 2, 0 );

	if ( WSAStartup( wVersionRequested, &wsaData ) == 0 )
	{

		if( gethostname ( shost.GetBuffer(), shost.GetLength()) == 0)
		{
			if((hostinfo = gethostbyname(alpHost)) != NULL)
			{
				ip = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
			}
		}

		WSACleanup( );
	}
	ATLASSERT( !ip.IsEmpty());
	if( ip.IsEmpty())	
		return LOCAL_IP_STRING;
	return ip;
}

bool CClientSession::CreateSocket( LPCSTR aSocketAddress, UINT aSocketPort)
{
	ATLASSERT( aSocketAddress);
	if( !aSocketAddress)	return false;

	SOCKERR serr;
	WSADATA wsadata;

	//  Initialize the sockets library
	serr = WSAStartup(DESIRED_WINSOCK_VERSION, &wsadata);
	if (serr != 0) 
	{
		::MessageBox(NULL, "Socket initialization error.", "Error", MB_ICONERROR | MB_OK);
		return false; 
	}

	if (wsadata.wVersion < MINIMUM_WINSOCK_VERSION) 
	{
		::MessageBox(NULL,"The minimum WinSock version - 1.0", "Error", MB_ICONERROR | MB_OK);
		return false; 
	}

	//  Create the socket
	sckServer = socket(AF_INET, SOCK_STREAM, 0);
	serr = (sckServer == INVALID_SOCKET) ? WSAGetLastError(): 0;

	if (serr != 0)
		return false;

	CComString	aSocketIP( aSocketAddress);
	if( !IsIPAddress( aSocketIP))
		aSocketIP = GetIPViaHostName( aSocketIP);

	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr( aSocketIP);
	sockAddr.sin_port = htons( aSocketPort);

	if (connect( sckServer, (SOCKADDR *)&sockAddr, sizeof(sockAddr)) != 0) 
		serr = WSAGetLastError();

	if (serr == 0)
	{
		hSendThread = CreateThread( NULL, 0, sendThread_, this, NULL, NULL);
		hRecvThread = CreateThread( NULL, 0, recvThread_, this, NULL, NULL);
	}
	else
	{
		closesocket( sckServer);
		sckServer = INVALID_SOCKET;
	}

	return serr == 0;
}

// Закрывает сессию
void CClientSession::Close()
{
	if (sckServer != INVALID_SOCKET)
	{
		shutdown( sckServer, SD_BOTH);
		closesocket( sckServer);
		sckServer = INVALID_SOCKET;
	}
}

// Отправляет сообщения
void CClientSession::Send( BYTE aType, const BYTE FAR *aData, int aDataSize)
{
	int iMessageSize = GetOutMessageSize( aType);
	if (iMessageSize > aDataSize)
	{
		::MessageBox( NULL, "Wrong out message size.", "Error", MB_ICONERROR | MB_OK);
		return;
	}

	CSendData* pSendData = new CSendData();
	DWORD dwHeaderSize = 3;
	DWORD dwBodySize = 0;
	if (iMessageSize < 0)
	{
		dwHeaderSize += 4;
		dwBodySize = aDataSize;
	}
	else
		dwBodySize = (DWORD)iMessageSize;
    pSendData->dwSize = dwHeaderSize + dwBodySize;
	pSendData->pData = new BYTE[ pSendData->dwSize];

	int i = 0;
	*((WORD* )(pSendData->pData + i)) = uSignature;
	i += sizeof( uSignature);
	*((BYTE* )(pSendData->pData + i)) = aType;
	i += sizeof( aType);
	if (iMessageSize < 0)
	{
		*((DWORD*)(pSendData->pData + i)) = dwBodySize;
		i += sizeof( dwBodySize);
	}
	if (dwBodySize > 0)
		memcpy( pSendData->pData + i, aData, dwBodySize);

	EnterCriticalSection( &csSendQueue);
	ATLTRACE( "push: ");
	for (i = 0;  i < pSendData->dwSize;  i++)
		ATLTRACE( "%c", *(pSendData->pData + i));
	ATLTRACE( "\n");
	oSendQueue.push_back( pSendData);
	SetEvent( hSendEvent);
	LeaveCriticalSection( &csSendQueue);
}

DWORD CClientSession::sendThread()
{
	CoInitialize(NULL);
	while (bRunning && WaitForSingleObject( hSendEvent, INFINITE) != WAIT_FAILED)
	{
		CSendData* pSendData = NULL;
		EnterCriticalSection( &csSendQueue);
		if (!oSendQueue.empty())
		{
			CSendQueue::iterator it = oSendQueue.begin();
			pSendData = *it;
			*it = NULL;
			oSendQueue.pop_front();
			if( oSendQueue.size() == 0)
				ResetEvent( hSendEvent);
		}
		LeaveCriticalSection( &csSendQueue);

		if (pSendData == NULL)
			continue;

		ATLTRACE( "pop: ");
		for (int i = 0;  i < pSendData->dwSize;  i++)
			ATLTRACE( "%c", *(pSendData->pData + i));
		ATLTRACE( "\n");

		int n = 0;
		if (sckServer != INVALID_SOCKET)
			n = ::send( sckServer, (char*)pSendData->pData, pSendData->dwSize, 0);

		if (n == SOCKET_ERROR)
			n = WSAGetLastError();

		delete[] pSendData->pData;
		delete pSendData;
	}

	CoUninitialize();
	return 0;
}

DWORD WINAPI CClientSession::sendThread_( VOID* param)
{
	return ((CClientSession*)param)->sendThread();
}

DWORD CClientSession::recvThread()
{
	CoInitialize(NULL);

	BYTE btBuff[ 1024];
	int nBuffLen = 0;
	while (bRunning)
	{
		if (sckServer == INVALID_SOCKET)
		{
			Sleep( 500);
			continue;
		}

		nBuffLen = recv( sckServer, (char*)&btBuff[0], 1024, 0);

		if (nBuffLen < 0)
		{
			Close();
			break;
		}

		if (nBuffLen != 0)
		{
			handleReceiveData( btBuff, nBuffLen);
		}
	}

	CoUninitialize();
	return 0;
}

void CClientSession::handleReceiveData( BYTE* aData, int aDataSize)
{
	EnterCriticalSection( &csBuffer);

	// Принимаем данные - копируем данные в буфер
	CopyToBuffer( aData, aDataSize);

	while (dwBufferSize >= 3)
	{
		// Формируем сообщение
		// по сгинатуре проверяем достоверность полученного сообщения
		if (!CheckSignature())
		{
			OnSignatureError( pBuffer, dwBufferSize);
			RemoveMessage( dwBufferSize);
			break;
		}

		// Определяем тип сообщения
		BYTE nType = pBuffer[ 2];
		int iHeaderSize = 3;

		// Обрабатываем сообщение - вызываем соответствующий обработчик
		int iMessageSize = HandleMessage( nType, pBuffer + iHeaderSize, dwBufferSize - iHeaderSize);
		if (iMessageSize == HM_NOT_ENOUGH_DATA)
			break;
		
		if (iMessageSize == HM_WRONG_TYPE)
		{
			iMessageSize = dwBufferCapacity;
		}
		else 
			iMessageSize += iHeaderSize;

		// Удаляем сообщение из буфера
		RemoveMessage( iMessageSize);
	}
	LeaveCriticalSection( &csBuffer);
}

void CClientSession::CopyToBuffer( BYTE* aData, int aDataSize)
{
	if (dwBufferCapacity < dwBufferSize + aDataSize)
	{
		int newSize = dwBufferCapacity;
		if (dwBufferCapacity > aDataSize)
			newSize += dwBufferCapacity;
		else
			newSize += aDataSize;
		BYTE* pNewBuffer = new BYTE[newSize];
		memcpy( pNewBuffer, pBuffer, dwBufferSize);
		pBuffer = pNewBuffer;
		dwBufferCapacity = newSize;
	}
	memcpy( pBuffer + dwBufferSize, aData, aDataSize);
	dwBufferSize += aDataSize;
}

bool CClientSession::CheckSignature()
{
	return (uSignature == (*(USHORT*)pBuffer));
}

void CClientSession::OnSignatureError( BYTE* aData, int aDataSize)
{
	// По-умолчанию закрываем сессию
	Close();
}

void CClientSession::RemoveMessage( int aMessageSize)
{
	if (dwBufferSize >= aMessageSize)
	{
		dwBufferSize -= aMessageSize;
		memcpy( pBuffer, pBuffer + aMessageSize, dwBufferSize);
	}
	else
	{
		dwBufferSize = 0;
	}
}

DWORD WINAPI CClientSession::recvThread_( VOID* param)
{
	return ((CClientSession*)param)->recvThread();
}

