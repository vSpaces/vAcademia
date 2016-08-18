#pragma once

#include "ITunnelPacket.h"
#include "Mutex.h"

struct PacketInRecvGiver
{
	PacketInRecvGiver(ITunnelPacketIn* aPacket)
	{
		packet = aPacket;
		pos = 0;
	}

	ITunnelPacketIn*	packet;
	int					pos;
};

class CRemoteSession
{
public:
	CRemoteSession(void);
	~CRemoteSession(void);

public:
	MP_LIST<PacketInRecvGiver*>		m_packetInQueue;
	HANDLE							m_recvWaitEventHandle;
	HANDLE							m_connectWaitEventHandle;
};
