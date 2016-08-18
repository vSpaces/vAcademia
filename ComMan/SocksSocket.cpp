
/********************************************************************************************************\
*                                                                                                        *
*   Component Name : SocksSocket Component																 *
*   Purpose        : Establish Socket through Socks Proxy Ver 4 and Ver 5                                *
*                                                                                                        *
*   Author		   : Ravi Kiran                                                                          *
*   email          : ravi824@rediffmail.com                                                              *
*                                                                                                        *
*   Copy Rights    : All copy rights reserved.                                                           *
*                                                                                                        *
\********************************************************************************************************/

#include "stdafx.h"
#include "SocksSocket.h"


/********************************************************************************************************\
*                                                                                                        *
*   Function Name: CSocksSocket																				 *
*   Purpose      :                                                                                       *
*   Parameters   :                                                                                       *
*       In       :                                                                                       *
*       Out      :                                                                                       *
*       InOut    :                                                                                       *
*   Return       :                                                                                       *
*                                                                                                        *
*   Remarks      :                                                                                       *
*                                                                                                        *
\********************************************************************************************************/
CSocksSocket::CSocksSocket()
{
	m_IsError = FALSE;
	m_SocksPort = 0;
	m_DestinationPort = 0;
	ZeroMemory(m_SocksAddress, ADDRESS_SIZE);
	ZeroMemory(m_DesitnationAddress, ADDRESS_SIZE);	
	m_pLW = NULL;
	InitiateSocket();						// call socket initiation
}
/********************************************************************************************************\
*                                                                                                        *
*   Function Name: ~CSocksSocket																				 *
*   Purpose      :                                                                                       *
*   Parameters   :                                                                                       *
*       In       :                                                                                       *
*       Out      :                                                                                       *
*       InOut    :                                                                                       *
*   Return       :                                                                                       *
*                                                                                                        *
*   Remarks      :                                                                                       *
*                                                                                                        *
\********************************************************************************************************/
CSocksSocket::~CSocksSocket()
{
	WSACleanup();
}

/********************************************************************************************************\
*                                                                                                        *
*   Function Name: InitiateSocket																		 *
*   Purpose      : Initiate socket and set default values                                                *
*   Parameters   :                                                                                       *
*       In       :                                                                                       *
*       Out      :                                                                                       *
*       InOut    :                                                                                       *
*   Return       :                                                                                       *
*                                                                                                        *
*   Remarks      :                                                                                       *
*                                                                                                        *
*                                                                                                        *
\********************************************************************************************************/
int	CSocksSocket::InitiateSocket()
{
	WSADATA wsaData;// if this doesn't work
	if ( WSAStartup( MAKEWORD( 1, 1 ), &wsaData ) != 0 ) 
	{
		return CONNECTION_WSA_STARTUP_ERROR;
	} 
	m_Interval	=	0;
	m_Version	=	PT_SOCKS4;
	m_Authentication = false;
	strcpy_s( m_szErrorMessage , 255, "No Message Stored.. \0" );
	memset(&m_UserName[0], 0, USER_NAME_LENGTH);
	memset(&m_Password[0], 0, PASSWORD_LENGTH);

	return CONNECTION_NO_ERROR;
}

HRESULT CSocksSocket::SetSocksPort(unsigned short in_Port)
{
	if ( in_Port > 0 )
		m_SocksPort = in_Port;

	return S_OK;
}

HRESULT CSocksSocket::SetDestinationPort(unsigned short in_Port)
{
	if ( in_Port > 0 )
		m_DestinationPort = in_Port;

	return S_OK;
}

HRESULT CSocksSocket::SetSocksAddress(const char* in_SocksAddress)
{
	strcpy_s( m_SocksAddress, 25, in_SocksAddress );
	return S_OK;
}

HRESULT CSocksSocket::SetDestinationAddress(const char* in_DestinationAddress)
{
	strcpy_s( m_DesitnationAddress, 25, in_DestinationAddress);
	return S_OK;
}

int CSocksSocket::Connect(SOCKET *pSocket, SOCKET *pConnectSocket, SOCKERR& errorCode)
{
	struct hostent *hostInfo;
	struct sockaddr_in Destination_Addr;					// connector's address information
	struct sockaddr_in Socks_Addr;							// connector's address information
	char *szSplit   = strchr( m_SocksAddress, ':' );
	int ret_val=-1;
	if ( szSplit ) 
	{
		*szSplit		=	0;
		m_SocksPort		=	(unsigned short)rtl_atoi( szSplit + 1 );
	}

	if ( ( hostInfo = gethostbyname( m_SocksAddress ) ) == NULL ) 
	{	
		m_IsError = true;
		errorCode = WSAGetLastError();

		WriteLogError("Error found resolving Socks host infor(socks)");
		return CONNECTION_GET_HOST_ERROR;
	}

	Socks_Addr.sin_family = AF_INET;						// host byte order 
	Socks_Addr.sin_port = htons( m_SocksPort );				// short, network byte order 
	Socks_Addr.sin_addr = *((struct in_addr *)hostInfo->h_addr);
	memset( &(Socks_Addr.sin_zero), 0, 8 );					// zero the rest of the struct 

	if ( (hostInfo = gethostbyname( m_DesitnationAddress ) ) == NULL ) 
	{														// get the host info 
		//gethostbyname
		m_IsError = true;
		errorCode = WSAGetLastError();
		WriteLogError("Error found resolving Destination host infor(socks)");
		return CONNECTION_GET_HOST_ERROR;
	}
	if ( *pConnectSocket == INVALID_SOCKET) 
	{
		m_IsError = true;
		errorCode = WSAGetLastError();
		WriteLogError("Error found Creation of Socket");
		return CONNECTION_CREATE_SOCKET_ERROR;
	}

	Destination_Addr.sin_family = AF_INET;					// host byte order 
	Destination_Addr.sin_port = htons( m_DestinationPort );	// short, network byte order 
	Destination_Addr.sin_addr = *((struct in_addr *)hostInfo->h_addr);
	memset( &(Destination_Addr.sin_zero), '\0', 8 );			// zero the rest of the struct 

	if ( m_Version == PT_SOCKS4 )
	{
		ret_val = ConnectToSocksFour( *pConnectSocket, 
							(struct sockaddr *)&Destination_Addr,
							sizeof(struct sockaddr),
							(struct sockaddr*)&Socks_Addr,
							sizeof(struct sockaddr),0 );
		if ( ret_val != CONNECTION_NO_ERROR ) 
		{
			errorCode = WSAGetLastError();
			WriteLogError("Connect to socks server error (socks4)");
			return ret_val;
		}
	}
	else if ( m_Version == PT_SOCKS5 )
	{
		ret_val = ConnectToSocksFive( *pConnectSocket, 
			(struct sockaddr *)&Destination_Addr,
			sizeof(struct sockaddr),
			(struct sockaddr*)&Socks_Addr,
			sizeof(struct sockaddr));
		if ( ret_val != CONNECTION_NO_ERROR ) 
		{
			errorCode = WSAGetLastError();
			WriteLogError("Connect to socks server error (socks5)");
			//Failed to Establish connection through Socks
			return ret_val;
		}
	}
	else
	{
		m_IsError = true;
		errorCode = WSAGetLastError();
		WriteLogError( "Error Invalid Socks Version" );
		return CONNECTION_INVALID_SOCKS_VERSION_ERROR;
	}
		
	*pSocket = *pConnectSocket;
	*pConnectSocket = INVALID_SOCKET;
	// Connected and returns the Socket established with socks server
	return CONNECTION_NO_ERROR;
}

HRESULT CSocksSocket::SetVersion(int in_Version)
{
	// accepts only Socks Version 4 and 5
	if(in_Version == PT_SOCKS4 || in_Version == PT_SOCKS5)
	{
		m_Version = in_Version;
		return S_OK;
	}

	return S_FALSE;
}

HRESULT CSocksSocket::SetUserName(const char* in_szUserName)
{
	if (in_szUserName != NULL)
	{
		strcpy_s( m_UserName, USER_NAME_LENGTH, in_szUserName );
		return S_OK;
	}

	return S_FALSE;
}

HRESULT CSocksSocket::SetPassword(const char* in_szPassword)
{
	if (in_szPassword != NULL)
	{
		strcpy_s( m_Password, PASSWORD_LENGTH, in_szPassword );
		return S_OK;
	}

	return S_FALSE;
}

HRESULT CSocksSocket::SetSocksInterval(long in_MilliSeconds)
{
	if( in_MilliSeconds >= 0 )			// accepts only positive values
	{
		m_Interval = in_MilliSeconds;
		return S_OK;
	}
	else
		return S_FALSE;
}

HRESULT CSocksSocket::GetUserName(char* bsUserName)
{
	// TODO: Add your implementation code here
	bsUserName = &m_UserName[0];
	return S_OK;
}

HRESULT CSocksSocket::GetVersion(long* m_Version)
{
	*m_Version = this->m_Version;
	return S_OK;
}

/********************************************************************************************************\
*                                                                                                        *
*   Function Name: GetLastErrorMessage																	 *
*   Purpose      : Returns last error message                                                            *
*   Parameters   :                                                                                       *
*       In       :                                                                                       *
*       Out      :                                                                                       *
*       InOut    :                                                                                       *
*   Return       :                                                                                       *
*                                                                                                        *
*   Remarks      :                                                                                       *
*                                                                                                        *
\********************************************************************************************************/

HRESULT CSocksSocket::GetLastErrorMessage(BSTR* szErrorMessage)
{
	//return m_szErrorMessage;
	BSTR sendBuf	=	A2BSTR(m_szErrorMessage);
	*szErrorMessage  =  sendBuf;
	return S_OK;
}

HRESULT CSocksSocket::SetAuthentication(long authentication)
{
	// TODO: Add your implementation code here
	m_Authentication = authentication;

	return S_OK;
}

HRESULT CSocksSocket::SetLW(ILogWriter* pLW)
{
	m_pLW = pLW;

	return 	pLW != NULL ? S_OK : S_FALSE;
}

/********************************************************************************************************\
*                                                                                                        *
*   Function Name: ConnectToSocksFour																	 *
*   Purpose      : Connects to Sockets server and returns value                                          *
*   Parameters   : SOCKET,sockaddr,nameLen,sockaddr,socksLen,szUserId                                    *
*       In       :                                                                                       *
*       Out      :                                                                                       *
*       InOut    :                                                                                       *
*   Return       :                                                                                       *
*                                                                                                        *
*   Remarks      :                                                                                       *
*                                                                                                        *
*                                                                                                        *
\********************************************************************************************************/
int CSocksSocket::ConnectToSocksFour( SOCKET in_Socket,
						const struct sockaddr FAR *in_szName,
						int nameLen,
						const struct sockaddr FAR *in_Socks,
						int socksLen,
						const char *szUserId ) 
{
	BYTE* packet = NULL;
	if ( in_Socks == NULL )									//If no socks server got passed...
	{
		//General Connect
		if ( CSocketConnection::_connect( in_Socket, (SOCKADDR*)in_szName, nameLen) != 0)		//If the return value is not 0, there was an error, so lets return it
		{
			m_IsError = true;
			WriteLogError("Can't connect to socks server" );
			return CONNECTION_CONNECT_ERROR;
		}
	}

	//Connecting to Socks Server
	if ( CSocketConnection::_connect( in_Socket, (SOCKADDR*)in_Socks, socksLen) != 0)		//If the return value is not 0, there was an error, so lets return it
	{
		m_IsError = true;
		WriteLogError("Can't connect to socks server" );
		return CONNECTION_CONNECT_ERROR;
	}
	    
	int lPacketLen = 9;										//Length of the Initialize packet
	if ( szUserId)
		lPacketLen += strlen(szUserId);						//If there is an extra userid, add its length to the packet length
    packet = MP_NEW_ARR(BYTE, lPacketLen);					//Allocate a packet
    packet[0] = m_Version == PT_SOCKS4 ? 4 : 5;									//Socks version
    packet[1] = 1;											//Connect code
    	
    memcpy( packet+2,
		(char *)&(((sockaddr_in *)in_szName)->sin_port),
		2 );												//Copy the port
    memcpy( packet+4,
		(char *)&(((sockaddr_in *)in_szName)->sin_addr.S_un.S_addr),
		4 );												//Copy the IP
	    
    if ( szUserId )											//If there was a userid, copy it now
    	memcpy( packet+8, szUserId, strlen(szUserId)+1 );
    else									
    	packet[8] = 0;
		
	if ( m_Interval == 0 )
		Sleep( SOCKS_INTERVAL);
	else
		Sleep( m_Interval);
		
    //Send the packet
	int bytesSend = send( in_Socket, (const char*)packet, lPacketLen, 0 );
	MP_DELETE_ARR(packet);									//Unallocate the packet
	if (bytesSend != lPacketLen)
	{
		m_IsError = true;
		WriteLogError( "Can't send request packet to socks server");
		return CONNECTION_SOCKET_ERROR;
	}

	BYTE reply[REPLY_PACKET_SIZE];							//Allocate memory for the reply packet
	memset( reply, 0 , REPLY_PACKET_SIZE);					//Set it to 0
	int bytesRecv = recv( in_Socket, (char*)&reply[0], REPLY_PACKET_SIZE, 0 );		//Get the reply packet
	reply[REPLY_PACKET_SIZE - 1] = '\0';
	if (bytesRecv == -1)
	{
		m_IsError = true;
		WriteLogError( "Can't recv reply packet from socks server");
		return CONNECTION_SOCKET_ERROR;								
	}
		
	if ( reply[0] != 0)										//Reply code should be 0
	{ 
        // SOCKS: Reply code is invalid
		m_IsError = true;
		WriteLogError( "Socks server protocol error");
	    return CONNECTION_SOCKS_SERVER_PROTOCOL_ERROR;
	}
	if ( reply[1] == 90 )									//Request granted!
	{				
		return CONNECTION_NO_ERROR;
	}

	m_IsError = true;
	switch ( reply[1] )										//Error codes
	{
		case 91:
			{
				WriteLogError( "SOCKS: request rejected or failed" );
            	break;
			}
		case 92:
			{
				WriteLogError( "SOCKS: request rejected because SOCKS server cannot connect to identd on the client");
            	break;
			}
		case 93:
			{
				WriteLogError( "SOCKS: request rejected because the client program and identd report different user-ids");
            	break;
			}
            			
		default:
			{
            	WriteLogError( "SOCKS: General Failure or Failed to connect destination server..");
				break;
			}
	}

	return CONNECTION_PROXY_SERVER_ERROR;
}


/********************************************************************************************************\
*                                                                                                        *
*   Function Name: Authenticate																			 *
*   Purpose      : Send authentication packet to socks server				                             *
*   Parameters   : SOCKET,sockaddr,nameLen,sockaddr,socksLen,szUserId                                    *
*       In       :                                                                                       *
*       Out      :                                                                                       *
*       InOut    :                                                                                       *
*   Return       :                                                                                       *
*                                                                                                        *
*   Remarks      :                                                                                       *
*                                                                                                        *
*                                                                                                        *
\********************************************************************************************************/
int CSocksSocket::Authentificate(SOCKET in_Socket)
{
	BYTE* authPacket = NULL;
	BYTE reply[REPLY_PACKET_SIZE];									//Allocate memory for the reply packet
	memset( reply, 0 , REPLY_PACKET_SIZE );							//Set it to 0

	int nameLen = strlen(&m_UserName[0]);
	int passLen = strlen(&m_Password[0]);

	int packetLength = 3+nameLen+passLen;

	authPacket = MP_NEW_ARR(BYTE, packetLength);					//Allocate a packet
	memset(authPacket, '\0' , packetLength);
	authPacket[0] = m_Version == PT_SOCKS4 ? 4 : 5;										//Socks version
	authPacket[1] = (BYTE)nameLen;										//user name length
	memcpy( authPacket+2, &m_UserName[0], nameLen);											
	authPacket[nameLen+2] = (BYTE)passLen;								//user name length
	memcpy( authPacket+3+nameLen, &m_Password[0], passLen);

	//Send the packet
	int bytesSend = send( in_Socket, (char*)authPacket, packetLength, 0);
	MP_DELETE_ARR(authPacket);										//Unallocate the packet
	if (bytesSend != packetLength)
	{
		m_IsError = true;
		WriteLogError("Can't send request packet to socks server");
		return CONNECTION_SOCKET_ERROR;								
	}

	memset( reply, 0 , REPLY_PACKET_SIZE );							//Set it to 0
	/*int r = */recv(in_Socket, (char*)reply, REPLY_PACKET_SIZE, 0 );	//Get the reply packet
	reply[REPLY_PACKET_SIZE - 1] = '\0';

	if ( reply[0] != 5)												//Reply code should be 0
	{ 
		m_IsError = true;
		WriteLogError( "Socks server protocol error");
		return CONNECTION_SOCKET_ERROR;
	}
	if ( reply[1] == 0 )											//Request granted!
	{
		return CONNECTION_NO_ERROR;
	}
	else
	{
		WriteLogError( "SocksServer: Authentificate error");
		return CONNECTION_PROXY_AUTH_ERROR;
	}
}


/********************************************************************************************************\
*                                                                                                        *
*   Function Name: Request5																				 *
*   Purpose      : send request packet for establish connection				                             *
*   Parameters   : SOCKET,sockaddr,nameLen,sockaddr,socksLen,szUserId                                    *
*       In       :                                                                                       *
*       Out      :                                                                                       *
*       InOut    :                                                                                       *
*   Return       :                                                                                       *
*                                                                                                        *
*   Remarks      :                                                                                       *
*                                                                                                        *
*                                                                                                        *
\********************************************************************************************************/

int CSocksSocket::Request5(SOCKET in_Socket, const struct sockaddr FAR *in_szName)
{
	BYTE* packet = NULL;
	BYTE reply[REPLY_PACKET_SIZE];							//Allocate memory for the reply packet
	memset( reply, 0 , REPLY_PACKET_SIZE );					//Set it to 0

	int lPacketLen = 10;
	packet = MP_NEW_ARR(BYTE, lPacketLen);					//Allocate a packet
	memset(packet , '\0' , 10);
	packet[0] = m_Version == PT_SOCKS4 ? 4 : 5;									//Socks version
	packet[1] = 1;											//No of methods
	packet[2] = 0;											//RESERVED
	packet[3] = 1;											//for ip v4 address
	memcpy( packet+4,										//Copy the IP
		(char *)&(((sockaddr_in *)in_szName)->sin_addr.S_un.S_addr),
		4 );											
	memcpy( packet+8,										//Copy the port
		(char *)&(((sockaddr_in *)in_szName)->sin_port),
		2 );

	//Send the packet
	int bytesSend = send( in_Socket, (char*)packet, lPacketLen, 0 );
	MP_DELETE_ARR(packet);									//Unallocate the packet
	if (bytesSend != lPacketLen)
	{
		m_IsError = true;
		WriteLogError( "Can't send request packet to socks server" );
		return CONNECTION_SOCKET_ERROR;								
	}

	memset( reply, 0 , REPLY_PACKET_SIZE);					//Set it to 0
	int bytesRecv = recv(in_Socket, (char*)reply, REPLY_PACKET_SIZE, 0 );	//Get the reply packet
	reply[REPLY_PACKET_SIZE - 1] = '\0';
	if (bytesRecv == -1)
	{
		m_IsError = true;
		WriteLogError( "Can't recv reply packet from socks server" );
		return CONNECTION_SOCKET_ERROR;								
	}

	if ( reply[0] != 5)										//Reply code should be 0
	{ 
		m_IsError = true;
		WriteLogError( "Socks server protocol error" );
		return CONNECTION_SOCKS_SERVER_PROTOCOL_ERROR;
	}
	if ( reply[1] == 0 )									//Request granted!
	{
		return CONNECTION_NO_ERROR;
	}

	m_IsError = true;

	switch ( reply[1])										//Error codes
	{

	case 1:
		{
			WriteLogError( "SOCKS: general SOCKS server failure" );
			break;
		}
	case 2:
		{
			WriteLogError( "SOCKS: connection not allowed by ruleset" );
			break;
		}
	case 3:
		{
			WriteLogError( "SOCKS: Network unreachable" );
			break;
		}
	case 4:
		{
			WriteLogError( "SOCKS: Host unreachable" );
			break;
		}
	case 5:
		{
			WriteLogError( "SOCKS: Connection refused" );
			break;
		}
	case 6:
		{
			WriteLogError( "SOCKS: TTL expired" );
			break;
		}
	case 7:
		{
			WriteLogError( "SOCKS: Command not supported" );
			break;
		}
	case 8:
		{
			WriteLogError( "SOCKS: Address type not supported" );
			break;
		}
	case 9:
		{
			WriteLogError( "SOCKS: to X'FF' unassigned" );
			break;
		}            				
	default:
		{
			WriteLogError( "SOCKS: General Failure or Failed to connect destination server." );
			break;
		}
	}

	return CONNECTION_PROXY_SERVER_ERROR;
}


/********************************************************************************************************\
*                                                                                                        *
*   Function Name: ConnectToSocksFive																	 *
*   Purpose      : Connects to Sockets server version five and returns value                             *
*   Parameters   : SOCKET,sockaddr,nameLen,sockaddr,socksLen,szUserId                                    *
*       In       :                                                                                       *
*       Out      :                                                                                       *
*       InOut    :                                                                                       *
*   Return       :                                                                                       *
*                                                                                                        *
*   Remarks      :                                                                                       *
*                                                                                                        *
*                                                                                                        *
\********************************************************************************************************/
int CSocksSocket::ConnectToSocksFive( SOCKET in_Socket,
						const struct sockaddr FAR *in_szName,
						int nameLen,
						const struct sockaddr FAR *in_Socks,
						int socksLen)
{
	BYTE* packet = NULL;
	if ( in_Socks == NULL )										//If no socks server got passed...
	{
		if ( CSocketConnection::_connect( in_Socket, (SOCKADDR*)in_szName, nameLen) != 0)			//Connecting to Socks Server
		{
			m_IsError = true;
			WriteLogError( "Can't connect to socks server" );
			return CONNECTION_SOCKET_ERROR;
		}
	}

    //General Connect
	if ( CSocketConnection::_connect( in_Socket, (SOCKADDR*)in_Socks, socksLen) != 0)			//Connecting to Socks Server
	{
		m_IsError = true;
		WriteLogError( "Can't connect to socks server" );
		return CONNECTION_SOCKET_ERROR;
	}
	    
    BYTE reply[REPLY_PACKET_SIZE];								//Allocate memory for the reply packet
	memset( reply, 0 , REPLY_PACKET_SIZE );						//Set it to 0

	int lPacketLen = 5;											//Length of the Initialize packet
	packet = (BYTE*)MP_NEW_ARR(BYTE, lPacketLen);				//Allocate a packet
	packet[0] = m_Version == PT_SOCKS4 ? 4 : 5;										//Socks version
    packet[1] = 3;												//Number of methods
	packet[2] = 0;												// NO AUTHENTICATION REQUIRED
	packet[3] = 2;												// USERNAME/PASSWORD
	packet[4] = REPLY_PACKET_SIZE;								// NO ACCEPTABLE METHODS
					
	if ( m_Interval == 0 )
		Sleep( SOCKS_INTERVAL );
	else
		Sleep( m_Interval );

    //Send the packet
	long bytesSend = send( in_Socket, (const char*)packet, lPacketLen, 0);
	MP_DELETE_ARR(packet);										//Unallocate the packet
	if (bytesSend != lPacketLen)
	{
		m_IsError = true;
		WriteLogError( "Can't send handshake packet to socks server" );
		return CONNECTION_SOCKET_ERROR;								
	}
		
	int bytesRecv = recv( in_Socket, (char*)reply, REPLY_PACKET_SIZE, 0 );	//Get the reply packet
	reply[REPLY_PACKET_SIZE - 1] = '\0';
	if (bytesRecv == -1)
	{
		m_IsError = true;
		WriteLogError( "Can't recv handshake packet from socks server" );
		return CONNECTION_SOCKET_ERROR;								
	}

	if ( reply[0] != 5)											//Reply code should be 0
	{ 
		m_IsError = true;
		WriteLogError( "Socks server protocol error\0" );
        return CONNECTION_SOCKS_SERVER_PROTOCOL_ERROR;
	}

	if ( reply[1] == 0)											//Request granted!
	{				
    	return Request5(in_Socket, in_szName);
	}
	else if (reply[1] == 2)										// need USERNAME/PASSWORD
	{
		int authResult = Authentificate(in_Socket);
		if (authResult == CONNECTION_NO_ERROR)
			return Request5(in_Socket, in_szName);

		return authResult;
	}

   	return CONNECTION_SOCKS_SERVER_PROTOCOL_ERROR;
}

int CSocksSocket::CloseSocket(SOCKET* s)
{
	int retval = 0;
	if (*s != INVALID_SOCKET)
	{
		shutdown( *s, SD_BOTH);
		retval = closesocket( *s);
		*s = INVALID_SOCKET;
	}

	return retval;
}

void CSocksSocket::WriteLogError(const char* str)
{
	if (m_pLW != NULL)
	{
		int err = GetLastError();
		CLogValue log("[ERROR][SOCKSSESSION] ", str, "last error = ", err);
		m_pLW->WriteLnLPCSTR(log.GetData());
	}
}
