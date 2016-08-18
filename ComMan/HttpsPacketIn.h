#pragma once
#include "itunnelpacket.h"

class CHttpsPacketIn :
	public CTunnelPacketIn
{
public:
	CHttpsPacketIn(unsigned short signature);
	~CHttpsPacketIn(void);

	unsigned int analysePacket(const char* buf, unsigned int len);
	const char* getData();
	int getDataSize();
	BYTE getType();
	unsigned short getPort();
	void destroy();

private:
	BYTE*	m_data;
	int		m_size;
	BYTE	m_type;
};