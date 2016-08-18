#pragma once
#include "itunnelpacket.h"

class CHttpsPacketOut :
	public CTunnelPacketOut
{
public:
	CHttpsPacketOut(unsigned short port, unsigned short signature, BYTE type);
	~CHttpsPacketOut(void);

	void wrapPacket(const char* buf, int len);
	const char* getData();
	int getDataSize();

private:
	BYTE*	m_data;
	int		m_size;
};
