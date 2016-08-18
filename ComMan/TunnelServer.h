#pragma once

#include "IConnection.h"
#include "WinSocketConnection.h"
#include "IConnection.h"
#include "ThreadAffinity.h"
#include "ITunnelPacket.h"
#include "HttpsPacketIn.h"
#include "HttpsPacketOut.h"
#include "RemoteSession.h"
#include "Mutex.h"
#include "ProxySettings.h"

class CTunnelServer
{
public:
	CTunnelServer(ProxySettings* aProxyConfig);
	~CTunnelServer(void);

	int connect(CServerInfo* pServerInfo, ILogWriter* pLW, SOCKERR& errorCode, unsigned short aTargetPort);
	int shutdown(unsigned short aPort);

	int send( const char * buf, int len, int flags, unsigned short port);
	int recv( char * buf, int len, int flags, unsigned short port);

	bool isTunnelSocketValid();
	long getSocketHandle();

	void destroy();

private:
	static DWORD WINAPI CTunnelServer::tunnelRecvThread_( VOID* param);
	DWORD recvThread();
	void ReleaseRecvThread();

	void handleReceiveData( BYTE* aData, unsigned int aDataSize);
	void CopyToBuffer( BYTE* aData, unsigned int aDataSize);
	bool CheckSignature();
	void RemoveMessage( unsigned int aMessageSize);
	void OnSignatureError();

private:
	void onAlive();
	void onCloseRemoteConnect(unsigned short aPort);
	void onHandshake( BYTE* aData, int aDataSize);
	void onRemoteConnected(unsigned short aPort);

	void CloseAllRemoteSessions();

	void SendHandshake();
	bool RemoteConnect(unsigned short aPort);
	void CreateRemoteSession(unsigned short aPort);
private:
	IConnection*		m_connection;
	HANDLE				m_hRecvThread;
	bool				m_bRunning;
	ILogWriter*			m_pLW;
	CServerInfo			m_ServerInfo;
	unsigned short		m_signature;

	BYTE*				m_pBuffer;
	DWORD				m_dwBufferCapacity;
	DWORD				m_dwBufferSize;
	CRITICAL_SECTION	m_csBuffer;
	CRITICAL_SECTION	m_csRemoteSessions;

	MP_MAP<unsigned short, CRemoteSession*>	m_remoteSessions;

	ITunnelPacketIn*	m_packetIn;

	unsigned int		m_tunnelSessionID;

	CMutex				m_mutexClose;
	CMutex				m_mutexConnect;

	ProxySettings			m_proxyConfig;

};
