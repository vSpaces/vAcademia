#include "stdafx.h"
#include "RemoteSession.h"

CRemoteSession::CRemoteSession(void):
			MP_LIST_INIT( m_packetInQueue)
{
	m_connectWaitEventHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_recvWaitEventHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CRemoteSession::~CRemoteSession(void)
{
	CloseHandle(m_connectWaitEventHandle);
	CloseHandle(m_recvWaitEventHandle);
}
