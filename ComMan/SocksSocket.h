#pragma once
#include "resource.h"       // main symbols

#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>

#include <atlbase.h>
#include <atlcom.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlctl.h>
#include <atlhost.h>

#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>

#include "IConnection.h"
#include "ProxySettings.h"
#include "SocketConnection.h"


#define	SOCKS_INTERVAL	1000

#define	USER_NAME_LENGTH	255
#define	PASSWORD_LENGTH		255
#define REPLY_PACKET_SIZE	255
#define ADDRESS_SIZE		25

/********************************************************************************************************\
*                                                                                                        *
*   Class Name   : CSocksSocket																				 *
*   Purpose      :                                                                                       *
*                                                                                                        *
*   Remarks      :                                                                                       *
*                                                                                                        *
\********************************************************************************************************/

class CSocksSocket
{
public:
	CSocksSocket();
	~CSocksSocket();

public:
	HRESULT SetSocksPort(unsigned short in_Port);
	HRESULT SetDestinationPort(unsigned short in_Port);
	HRESULT SetSocksAddress(const char* in_SocksAddress);
	HRESULT SetDestinationAddress(const char* in_DestinationAddress);
	HRESULT SetVersion(int in_Version);
	HRESULT SetUserName(const char* in_szUserName);
	HRESULT SetPassword(const char* in_szPassword);
	HRESULT SetSocksInterval(long in_MilliSeconds);
	HRESULT GetUserName(char* bsUserName);
	HRESULT GetVersion(long* m_Version);
	HRESULT GetLastErrorMessage(BSTR* szErrorMessage);
	HRESULT SetAuthentication(long m_Authentication);
	HRESULT SetLW(ILogWriter* pLW);

	int Connect(SOCKET* pSocket, SOCKET *pConnectSocket, SOCKERR& errorCode);
private:
	int ConnectToSocksFour( SOCKET in_Socket,
						const struct sockaddr FAR *in_szName,
						int nameLen,
						const struct sockaddr FAR *in_Socks,
						int socksLen,
						const char *szUserId );
	int ConnectToSocksFive( SOCKET in_Socket,
						const struct sockaddr FAR *in_szName,
						int nameLen,
						const struct sockaddr FAR *in_Socks,
						int socksLen);
	
	int	InitiateSocket();

	int Authentificate(SOCKET in_Socket);
	int Request5(SOCKET in_Socket, const struct sockaddr FAR *in_szName);
	int CloseSocket(SOCKET* s);
	void WriteLogError(const char*);

// Memeber Variables
public :
	BOOL	m_Authentication;
	int		m_Interval;
	BOOL	m_IsError;
private:
	int		m_Version;
	unsigned short		m_SocksPort;
	unsigned short		m_DestinationPort;
	char	m_SocksAddress[ADDRESS_SIZE];
	char	m_DesitnationAddress[ADDRESS_SIZE];
	char	m_UserName[USER_NAME_LENGTH];
	char	m_Password[PASSWORD_LENGTH];
	char	m_szErrorMessage[255];

	ILogWriter*	m_pLW;
};

