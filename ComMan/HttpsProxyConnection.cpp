#include "stdafx.h"
#include "HttpsProxyConnection.h"


CHttpsProxyConnection::CHttpsProxyConnection(ProxySettings* aProxyConfig, unsigned int aDestroyCallCount)
:CSocketConnection( aDestroyCallCount)
{
	m_proxyConfig = *aProxyConfig;
	m_pLW = NULL;
}

CHttpsProxyConnection::~CHttpsProxyConnection(void)
{
}

static const std::string base64_chars = 
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string CHttpsProxyConnection::base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for(i = 0; (i <4) ; i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for(j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while((i++ < 3))
			ret += '=';

	}

	return ret;

}


int CHttpsProxyConnection::connect(CServerInfo *pServerInfo, ILogWriter* pLW, SOCKERR& errorCode)
{
	m_pLW = pLW;

	if (m_pLW != NULL)
	{
		CLogValue log("CHttpsProxyConnection::connect proxy: used - ", m_proxyConfig.toString());
		m_pLW->Write(log);
	}

	errorCode = 0;

	unsigned short port = m_proxyConfig.port;

	if( m_connectSocket == INVALID_SOCKET)
	{
		errorCode = WSAGetLastError();
		return CONNECTION_SOCKET_ERROR;
	}

	struct hostent *hostInfo = NULL;

	if ( ( hostInfo = gethostbyname( WideToChar(m_proxyConfig.ip.c_str()) ) ) == NULL ) 
	{	
		if (m_pLW != NULL)
		{
			USES_CONVERSION;
			CLogValue log("Error gethostbyname = ", W2A(m_proxyConfig.ip.c_str()));
			m_pLW->WriteLn(log);
		}
		errorCode = WSAGetLastError();
		return CONNECTION_GET_HOST_ERROR;
	}

	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
//	sockAddr.sin_addr.s_addr = inet_addr( proxy);
	sockAddr.sin_addr = *((struct in_addr *)hostInfo->h_addr);
	sockAddr.sin_port = htons( port);

	int tos = 8;
	if(setsockopt(m_connectSocket, IPPROTO_IP, IP_TOS, (LPCSTR)&tos, sizeof(tos)) == SOCKET_ERROR) 
	{
		// serr = WSAGetLastError();
	}

	if (_connect( m_connectSocket, (SOCKADDR *)&sockAddr, sizeof(sockAddr)) != 0) 
	{
		if (m_pLW != NULL)
		{
			CLogValue log("Error connect");
			m_pLW->Write(log);
		}
		errorCode = WSAGetLastError();
		return CONNECTION_SOCKET_ERROR;
	}

	char strPort[6];
	_itoa_s(pServerInfo->tcpPort, strPort, 6, 10);

	std::string sendString = "CONNECT ";
	sendString += pServerInfo->sIP;
	sendString += ":";
	sendString += strPort;
	sendString += " HTTP/1.0\r\n";

	sendString += "Proxy-Connection: Keep-Alive\r\n";

	if (m_proxyConfig.useUser)
	{
		USES_CONVERSION;
		std::string user_pass = W2A(m_proxyConfig.user.c_str());
		user_pass += ":";
		user_pass += W2A(m_proxyConfig.password.c_str());

		std::string user_pass_base64 = base64_encode(reinterpret_cast<const unsigned char*>(user_pass.c_str()), user_pass.length());

		sendString += "Proxy-Authorization: basic ";
		sendString += user_pass_base64 + "\r\n\r\n";
	}
	else
		sendString += "\r\n";

	int sendedBytes = ::send(m_connectSocket, sendString.c_str(), sendString.length(), 0);
	if (sendedBytes < 0)
	{
		if (m_pLW != NULL)
		{
			CLogValue log("Error send");
			m_pLW->Write(log);
		}
		errorCode = WSAGetLastError();
		return CONNECTION_SOCKET_ERROR;
	}

	if (m_pLW != NULL)
	{
		CLogValue log("Request sended to server = ", m_proxyConfig.toString(), " request = ", sendString);
		m_pLW->Write(log);
	}

	const int BUFF_SIZE = 256;
	char recvBuf[BUFF_SIZE];

	memset(recvBuf, 0, BUFF_SIZE);
	int recvedBytes = ::recv(m_connectSocket, &recvBuf[0], BUFF_SIZE, 0);
	recvBuf[BUFF_SIZE - 1] = '\0';

	if (recvedBytes < 12)
	{
		if (m_pLW != NULL)
		{
			CLogValue log("Error recv ", recvedBytes);
			m_pLW->Write(log);
		}
		errorCode = WSAGetLastError();
		return CONNECTION_SOCKET_ERROR;
	}

	if (m_pLW != NULL)
	{
		CLogValue log("Request recved from server = ", m_proxyConfig.toString(), " recvBuf = ", recvBuf);
		m_pLW->Write(log);
	}

	if (memcmp("HTTP/1.0 407", recvBuf, 12) == 0 || memcmp("HTTP/1.1 407", recvBuf, 12) == 0) // ошибка авторизации
	{
		if (m_pLW != NULL)
		{
			CLogValue log("Auth error for proxy server = ", m_proxyConfig.toString());
			m_pLW->Write(log);
		}
		return CONNECTION_PROXY_AUTH_ERROR;
	}

	if (memcmp("HTTP/1.0 200", recvBuf, 12) == 0 || memcmp("HTTP/1.1 200", recvBuf, 12) == 0) // все ок
	{
		errorCode = 0;

		m_socket = m_connectSocket;
		m_connectSocket = INVALID_SOCKET;

		return CONNECTION_NO_ERROR;
	}

	if (m_pLW != NULL)
	{
		CLogValue log("End connect");
		m_pLW->Write(log);
	}

	return CONNECTION_PROXY_SERVER_ERROR;
}

void CHttpsProxyConnection::_destroy()
{
	MP_DELETE_THIS;
}

void CHttpsProxyConnection::setILogWriter(ILogWriter* pLW)
{
	m_pLW = pLW;
}