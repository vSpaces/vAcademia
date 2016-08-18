#include "stdafx.h"
#include "TunnelServer.h"
#include "HttpsProxyConnection.h"
#include "CrashHandlerInThread.h"

CTunnelServer::CTunnelServer(ProxySettings* aProxyConfig):
			MP_MAP_INIT( m_remoteSessions)
{
	m_proxyConfig = *aProxyConfig;
	if (m_proxyConfig.enabled)
	{
		MP_NEW_P2(proxyConnection, CHttpsProxyConnection, &m_proxyConfig, 1);
		m_connection = proxyConnection;
	}
	else
	{
		MP_NEW_P(winSocketConnection, CWinSocketConnection, 1);
		m_connection = winSocketConnection;
	}

	m_hRecvThread = NULL;
	m_dwBufferCapacity = 0;
	m_dwBufferSize = 0;
	m_pBuffer = NULL;
	InitializeCriticalSection( &m_csBuffer);
	InitializeCriticalSection( &m_csRemoteSessions);
	m_signature = 0x1274;
	m_packetIn = NULL;
	m_tunnelSessionID = 0;
	m_pLW = NULL;
	m_bRunning = true;
}

CTunnelServer::~CTunnelServer(void)
{
	if (m_pLW != NULL)
	{
		CLogValue log("CTunnelServer::~CTunnelServer to ", m_ServerInfo.sIP.GetBuffer(), " session_id = ", m_tunnelSessionID);
		m_pLW->WriteLn(log);
	}

	m_bRunning = false;

	m_connection->shutdown();

	ReleaseRecvThread();

	CloseAllRemoteSessions();

	// ждем завершени€
	m_mutexConnect.lock();
	m_mutexConnect.unlock();

	DeleteCriticalSection( &m_csBuffer);
	DeleteCriticalSection( &m_csRemoteSessions);
	if ( m_pBuffer != NULL)
	{
		MP_DELETE_ARR( m_pBuffer);
		m_pBuffer = NULL;
	}

	m_connection->destroy();
	m_connection = NULL;

	if (m_pLW != NULL)
	{
		CLogValue log("CTunnelServer::~CTunnelServer to ", m_ServerInfo.sIP.GetBuffer(), " end", " session_id = ", m_tunnelSessionID);
		m_pLW->WriteLn(log);
	}
}

DWORD WINAPI CTunnelServer::tunnelRecvThread_( VOID* param)
{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();
	return ((CTunnelServer*)param)->recvThread();
}

void CTunnelServer::ReleaseRecvThread()
{
	if (m_pLW != NULL)
	{
		CLogValue log("CTunnelServer::ReleaseRecvThread to ", m_ServerInfo.sIP.GetBuffer(), " m_hRecvThread = ", m_hRecvThread, " session_id = ", m_tunnelSessionID);
		m_pLW->WriteLn(log);
	}
	if(m_hRecvThread != NULL)
	{
		DWORD result = WaitForSingleObject( m_hRecvThread, INFINITE);
		if (m_pLW != NULL)
		{
			CLogValue log("CTunnelServer::ReleaseRecvThread to ", m_ServerInfo.sIP.GetBuffer(), " WaitForSingleObject = ", result, " session_id = ", m_tunnelSessionID);
			m_pLW->WriteLn(log);
		}
		/*if (result == WAIT_TIMEOUT || result == WAIT_FAILED)
			int i = 0;*/
		m_mutexClose.lock();
		if(m_hRecvThread != NULL)
		{
			CloseHandle(m_hRecvThread);
			m_hRecvThread = NULL;
		}
		m_mutexClose.unlock();
	}
}

DWORD CTunnelServer::recvThread()
{
	if (m_pLW != NULL)
	{
		DWORD threadId = GetCurrentThreadId();
		CLogValue log("CTunnelServer::recvThread thread created. ThreadID = ", threadId, " session_id = ", m_tunnelSessionID);
		m_pLW->WriteLn(log);
	}

	BYTE btBuff[ 1024];
	int nBuffLen = 0;
	while (m_bRunning)
	{
		if (!m_connection->isSocketValid())
		{
			Sleep( 1000);
			//SOCKERR errorCode;
			//m_connection->connect( &m_ServerInfo, m_pLW, errorCode);
			continue;
		}

		nBuffLen = m_connection->recv( (char*)&btBuff[0], 1024, 0);

		if (!m_bRunning)
			break;

		if (nBuffLen <= 0)
		{
			if(m_connection->isSocketValid())
			{
				if (m_pLW != NULL)
				{
					int iError = WSAGetLastError();
					CLogValue log("CTunnelServer [ERROR][SESSION] Socket error: ", iError, " (", m_ServerInfo.sIP.GetString(), ":", (int)m_ServerInfo.tcpPort, ")", " session_id = ", m_tunnelSessionID);
					m_pLW->WriteLnLPCSTR(log.GetData());
				}
				m_connection->shutdown();

				CloseAllRemoteSessions();
			}
			continue;
		}

		if (nBuffLen != 0)
		{
			handleReceiveData( btBuff, nBuffLen);
		}
	}

	m_mutexClose.lock();
	m_hRecvThread = NULL;
	m_mutexClose.unlock();

	return 0;
}

void CTunnelServer::handleReceiveData( BYTE* aData, unsigned int aDataSize)
{
	EnterCriticalSection( &m_csBuffer);

	// ѕринимаем данные - копируем данные в буфер
	CopyToBuffer( aData, aDataSize);

	while (m_dwBufferSize >= 9) // header size
	{
		// по сигнатуре провер€ем достоверность полученного сообщени€
		if (!CheckSignature())
		{
			OnSignatureError();
			RemoveMessage( m_dwBufferSize);
			break;
		}

		if (m_packetIn == NULL)
		{
			MP_NEW_P(packetIn, CHttpsPacketIn, m_signature);
			m_packetIn = (ITunnelPacketIn*)packetIn;
		}

		unsigned int messageSize = m_packetIn->analysePacket((const char*)m_pBuffer, m_dwBufferSize);
		if (messageSize != 0)
		{
			if (m_packetIn->getType() == IN_NEW_PACKET_MSG)
			{
				unsigned short port = m_packetIn->getPort();

				EnterCriticalSection( &m_csRemoteSessions);
				bool remoteSessionExist = m_remoteSessions.find(port) != m_remoteSessions.end();
				LeaveCriticalSection( &m_csRemoteSessions);

				if (!remoteSessionExist)
				{
					if (m_pLW != NULL)
					{
						CLogValue log("input packet for port that can't be requested. Port: ", port, " session_id = ", m_tunnelSessionID);
						m_pLW->WriteLn(log);
					}

					RemoveMessage( messageSize);
					continue;
				}

				MP_NEW_P(recvGiver, PacketInRecvGiver, m_packetIn);

				EnterCriticalSection( &m_csRemoteSessions);

				if (m_remoteSessions.find(port) != m_remoteSessions.end())
				{
					m_remoteSessions[port]->m_packetInQueue.push_back(recvGiver);
					m_packetIn = NULL;
					SetEvent(m_remoteSessions[port]->m_recvWaitEventHandle);
				}

				LeaveCriticalSection( &m_csRemoteSessions);
			}
			else if (m_packetIn->getType() == IN_ALIVE_MSG)
			{
				onAlive();
			}
			else if (m_packetIn->getType() == IN_CLOSE_MSG)
			{
				onCloseRemoteConnect(m_packetIn->getPort());
			}
			else if (m_packetIn->getType() == IN_HANDSHAKE)
			{
				onHandshake((BYTE*)m_packetIn->getData(), m_packetIn->getDataSize());
			}
			else if (m_packetIn->getType() == IN_CONNECT)
			{
				onRemoteConnected(m_packetIn->getPort());
			}

			// ”дал€ем сообщение из буфера
			RemoveMessage( messageSize);
		}
		else
			break;
	}

	LeaveCriticalSection( &m_csBuffer);
}

void CTunnelServer::onAlive()
{
	CHttpsPacketOut packetOut(0, m_signature, OUT_ALIVE_MSG);
	packetOut.wrapPacket(NULL, 0);

	m_connection->send(packetOut.getData(), packetOut.getDataSize(), 0);
}

void CTunnelServer::onCloseRemoteConnect(unsigned short aPort)
{
	EnterCriticalSection(&m_csRemoteSessions);

	std::map<unsigned short, CRemoteSession*>::iterator it = m_remoteSessions.find(aPort);
	if (it != m_remoteSessions.end())
	{
		for (std::list<PacketInRecvGiver*>::iterator it2 = it->second->m_packetInQueue.begin(); it2 != it->second->m_packetInQueue.end(); ++it2)
		{
			(*it2)->packet->destroy();
			MP_DELETE(((PacketInRecvGiver*)(*it2)));
		}

		SetEvent(it->second->m_connectWaitEventHandle);
		SetEvent(it->second->m_recvWaitEventHandle);

		MP_DELETE(it->second);

		m_remoteSessions.erase(it);
	}

	LeaveCriticalSection(&m_csRemoteSessions);
}

void CTunnelServer::CloseAllRemoteSessions()
{
	EnterCriticalSection(&m_csRemoteSessions);

	std::map<unsigned short, CRemoteSession*>::iterator it;
	while ((it = m_remoteSessions.begin()) != m_remoteSessions.end())
	{
		for (std::list<PacketInRecvGiver*>::iterator it2 = it->second->m_packetInQueue.begin(); it2 != it->second->m_packetInQueue.end(); ++it2)
		{
			(*it2)->packet->destroy();
			MP_DELETE(((PacketInRecvGiver*)(*it2)));
		}

		SetEvent(it->second->m_connectWaitEventHandle);
		SetEvent(it->second->m_recvWaitEventHandle);

		MP_DELETE(it->second);

		m_remoteSessions.erase(it);
	}

	LeaveCriticalSection(&m_csRemoteSessions);
}

void CTunnelServer::onRemoteConnected(unsigned short aPort)
{
	EnterCriticalSection( &m_csRemoteSessions);

	bool remoteSessionExist = m_remoteSessions.find(aPort) != m_remoteSessions.end();
	if (remoteSessionExist)
		SetEvent(m_remoteSessions[aPort]->m_connectWaitEventHandle);

	LeaveCriticalSection( &m_csRemoteSessions);

}

void CTunnelServer::onHandshake(BYTE* aData, int aDataSize)
{
	if (aDataSize != sizeof(m_tunnelSessionID))
		return;

	memcpy(&m_tunnelSessionID, aData, sizeof(m_tunnelSessionID));

	if(m_pLW != NULL)
	{
		CLogValue log("CTunnelServer [SESSION][ON_HANDSHAKED] ", m_ServerInfo.sIP.GetString(), ":", (int)m_ServerInfo.tcpPort, " session_id = ", m_tunnelSessionID);
		m_pLW->WriteLnLPCSTR(log.GetData());
	}
}

void CTunnelServer::OnSignatureError()
{
	// ѕо-умолчанию обрываем соединение
	if(m_pLW != NULL)
	{
		CLogValue log("CTunnelServer [ERROR][SESSION] Signature Error - ", m_ServerInfo.sIP.GetString(), ":", (int)m_ServerInfo.tcpPort, " session_id = ", m_tunnelSessionID);
		m_pLW->WriteLnLPCSTR(log.GetData());
	}
	m_connection->shutdown();
}


void CTunnelServer::CopyToBuffer( BYTE* aData, unsigned int aDataSize)
{
	if (m_dwBufferCapacity < m_dwBufferSize + aDataSize)
	{
		int newSize = m_dwBufferCapacity;
		if (m_dwBufferCapacity > aDataSize)
			newSize += m_dwBufferCapacity;
		else
			newSize += aDataSize;		
		BYTE* pNewBuffer = MP_NEW_ARR(BYTE, newSize);
		if ( m_pBuffer != NULL)
		{
			rtl_memcpy( pNewBuffer, newSize, m_pBuffer, m_dwBufferSize);
			MP_DELETE_ARR( m_pBuffer);
		}
		m_pBuffer = pNewBuffer;
		m_dwBufferCapacity = newSize;
	}
	rtl_memcpy( m_pBuffer + m_dwBufferSize, m_dwBufferCapacity - m_dwBufferSize, aData, aDataSize);
	m_dwBufferSize += aDataSize;
}

bool CTunnelServer::CheckSignature()
{
	return (m_signature == (*(unsigned short*)m_pBuffer));
}

void CTunnelServer::RemoveMessage( unsigned int aMessageSize)
{
	if (m_dwBufferSize/*размер данных*/ > aMessageSize/*размер сообщений*/)
	{
		m_dwBufferSize -= aMessageSize;
		memmove( m_pBuffer, m_pBuffer + aMessageSize, m_dwBufferSize);
	}
	else
	{
		m_dwBufferSize = 0;
	}
}


int CTunnelServer::connect(CServerInfo *pServerInfo, ILogWriter* pLW, SOCKERR& errorCode, unsigned short aTargetPort)
{
	m_pLW = pLW;

	if (m_pLW != NULL)
	{
		CLogValue log("CTunnelServer::connect to ", pServerInfo->sIP.GetBuffer(), " session_id = ", m_tunnelSessionID);
		m_pLW->WriteLn(log);
	}

	m_mutexConnect.lock();

	if (!m_bRunning)
	{
		m_mutexConnect.unlock();
		return CONNECTION_SOCKET_ERROR;
	}

	if (!isTunnelSocketValid())
	{
		m_ServerInfo = *pServerInfo;

		int connectResultCode = m_connection->connect(&m_ServerInfo, pLW, errorCode);
		if (connectResultCode != CONNECTION_NO_ERROR)
		{
			m_mutexConnect.unlock();

			return connectResultCode;
		}

		if (m_hRecvThread == NULL)
		{
			CSimpleThreadStarter tunnelThreadStarter(__FUNCTION__);			
			tunnelThreadStarter.SetRoutine(tunnelRecvThread_);
			tunnelThreadStarter.SetParameter(this);
			m_hRecvThread  = tunnelThreadStarter.Start();						
		}
	}

	m_mutexConnect.unlock();

	if (!RemoteConnect(aTargetPort))
		return CONNECTION_REMOTE_CONNECT_ERROR;

	SendHandshake();

	if (m_pLW != NULL)
	{
		CLogValue log("CTunnelServer::connect to ", pServerInfo->sIP.GetBuffer(), " end", " session_id = ", m_tunnelSessionID);
		m_pLW->WriteLn(log);
	}

	return CONNECTION_NO_ERROR;
}

void CTunnelServer::SendHandshake()
{
	CHttpsPacketOut packetOut(0, m_signature, OUT_HANDSHAKE);
	packetOut.wrapPacket(NULL, 0);

	m_connection->send(packetOut.getData(), packetOut.getDataSize(), 0);
}

void CTunnelServer::CreateRemoteSession(unsigned short aPort)
{
	EnterCriticalSection( &m_csRemoteSessions);
	if (m_remoteSessions.find(aPort) == m_remoteSessions.end())
		m_remoteSessions[aPort] = MP_NEW(CRemoteSession);
	LeaveCriticalSection( &m_csRemoteSessions);
}

bool CTunnelServer::RemoteConnect(unsigned short aPort)
{
	CreateRemoteSession(aPort);

	CHttpsPacketOut packetOut(aPort, m_signature, OUT_CONNECT);
	packetOut.wrapPacket(NULL, 0);

	m_connection->send(packetOut.getData(), packetOut.getDataSize(), 0);

	if (m_pLW != NULL)
	{
		CLogValue log("CTunnelServer::RemoteConnect sended to port = ", aPort, " session_id = ", m_tunnelSessionID);
		m_pLW->WriteLn(log);
	}

	HANDLE connectHandle = NULL;

	EnterCriticalSection( &m_csRemoteSessions);
	if (m_remoteSessions.find(aPort) != m_remoteSessions.end())
		connectHandle = m_remoteSessions[aPort]->m_connectWaitEventHandle;
	LeaveCriticalSection( &m_csRemoteSessions);

	if (connectHandle != NULL)
	{
		DWORD waitResult = WaitForSingleObject( connectHandle, INFINITE);

		if (!m_bRunning)
			return false;

		if (waitResult != WAIT_FAILED)
		{

			if (m_pLW != NULL)
			{
				CLogValue log("CTunnelServer::RemoteConnect recved to port = ", aPort, " session_id = ", m_tunnelSessionID);
				m_pLW->WriteLn(log);
			}

			EnterCriticalSection( &m_csRemoteSessions);
			bool sessionExist = m_remoteSessions.find(aPort) != m_remoteSessions.end();
			if (sessionExist)
				ResetEvent(m_remoteSessions[aPort]->m_connectWaitEventHandle);
			LeaveCriticalSection( &m_csRemoteSessions);

			return sessionExist;
		}
	}

	onCloseRemoteConnect(aPort);

	return false;
}

int CTunnelServer::shutdown(unsigned short aPort)
{
	if (m_pLW != NULL)
	{
		CLogValue log("CTunnelServer::shutdown to ", m_ServerInfo.sIP.GetBuffer(), " port = ", m_ServerInfo.tcpPort, " end", " session_id = ", m_tunnelSessionID);
		m_pLW->WriteLn(log);
	}
	// не отправл€ем пакет, т.к. shutdown закроет соединение на туннельном сервере
/*	CHttpsPacketOut packetOut(aPort, m_signature, OUT_CLOSE_MSG);
	packetOut.wrapPacket(NULL, 0);

	if (m_connection->isSocketValid())
		m_connection->send(packetOut.getData(), packetOut.getDataSize(), 0);*/

	onCloseRemoteConnect(aPort);

	m_connection->shutdown();

	return 0;
}

int CTunnelServer::send( const char * buf, int len, int flags, unsigned short port)
{
	bool remoteConnetionClosed = false;
	EnterCriticalSection( &m_csRemoteSessions);
	if (m_remoteSessions.find(port) == m_remoteSessions.end())
		remoteConnetionClosed = true;
	LeaveCriticalSection( &m_csRemoteSessions);

	if (remoteConnetionClosed)
		return 0;

	CHttpsPacketOut packetOut(port, m_signature, OUT_NEW_PACKET_MSG);
	packetOut.wrapPacket(buf, len);

	return m_connection->send(packetOut.getData(), packetOut.getDataSize(), flags);
}

int CTunnelServer::recv( char * buf, int len, int flags, unsigned short port)
{
	int readBytes = 0;

	bool remoteConnetionClosed = false;
	EnterCriticalSection( &m_csRemoteSessions);
	if (m_remoteSessions.find(port) == m_remoteSessions.end())
		remoteConnetionClosed = true;
	LeaveCriticalSection( &m_csRemoteSessions);

	if (remoteConnetionClosed)
		return readBytes;

	PacketInRecvGiver* recvGiver = NULL;
	bool isDataExist = false;

	EnterCriticalSection(&m_csRemoteSessions);
	if (m_remoteSessions.find(port) != m_remoteSessions.end() && m_remoteSessions[port]->m_packetInQueue.size() != 0)
	{
		isDataExist = true;
		recvGiver = (PacketInRecvGiver*)(*(m_remoteSessions[port]->m_packetInQueue.begin()));

		readBytes = (recvGiver->packet->getDataSize() - recvGiver->pos) > len ? len : (recvGiver->packet->getDataSize() - recvGiver->pos);
		memcpy(buf, recvGiver->packet->getData() + recvGiver->pos, readBytes);
		recvGiver->pos += readBytes;

		if (recvGiver->pos == recvGiver->packet->getDataSize())
		{
			recvGiver->packet->destroy();
			MP_DELETE(recvGiver);

			m_remoteSessions[port]->m_packetInQueue.pop_front();
		}
	}
	LeaveCriticalSection(&m_csRemoteSessions);

	if (!isDataExist)
	{
		HANDLE recvHandle = NULL;
		EnterCriticalSection(&m_csRemoteSessions);
		if (m_remoteSessions.find(port) != m_remoteSessions.end())
			recvHandle = m_remoteSessions[port]->m_recvWaitEventHandle;
		LeaveCriticalSection(&m_csRemoteSessions);

		if (recvHandle != NULL)
		{
			DWORD waitResult = WaitForSingleObject( recvHandle, INFINITE);
			if (!m_bRunning)
			{
				return 0;
			}

			if (waitResult != WAIT_FAILED)
			{
				EnterCriticalSection(&m_csRemoteSessions);
				if (m_remoteSessions.find(port) != m_remoteSessions.end())
					ResetEvent(m_remoteSessions[port]->m_recvWaitEventHandle);
				LeaveCriticalSection(&m_csRemoteSessions);

				return this->recv(buf, len, flags, port);
			}
		}
	}

	return readBytes;
}

bool CTunnelServer::isTunnelSocketValid()
{
	return m_connection->isSocketValid();
}

long CTunnelServer::getSocketHandle()
{
	return m_connection->getSocketHandle();
}

void CTunnelServer::destroy()
{
	MP_DELETE_THIS;
}
