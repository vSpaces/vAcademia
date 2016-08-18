#pragma once

#define IN_ALIVE_MSG					0
#define IN_CLOSE_MSG					1
#define IN_NEW_PACKET_MSG				2
#define IN_HANDSHAKE					3
#define IN_CONNECT						4

#define OUT_ALIVE_MSG					0
#define OUT_CLOSE_MSG					1
#define OUT_NEW_PACKET_MSG				2
#define OUT_HANDSHAKE					3
#define OUT_CONNECT						4

struct ITunnelPacketOut
{
	virtual void wrapPacket(const char* buf, int len)=0;
	virtual const char* getData()=0;
	virtual int getDataSize()=0;
};

class CTunnelPacketOut : public ITunnelPacketOut
{
public:
	CTunnelPacketOut(unsigned short port, unsigned short signature, BYTE type)
	{
		m_port = port;
		m_signature = signature;
		m_type = type;
	}
	~CTunnelPacketOut(){}

	virtual void wrapPacket(const char* buf, int len)=0;
	virtual const char* getData()=0;
	virtual int getDataSize()=0;

protected:
	unsigned short	m_port;
	unsigned short	m_signature;
	BYTE			m_type;
};

struct ITunnelPacketIn
{
	virtual unsigned int analysePacket(const char* buf, unsigned int len)=0;
	virtual const char* getData()=0;
	virtual int getDataSize()=0;
	virtual BYTE getType()=0;
	virtual unsigned short getPort()=0;
	virtual void destroy()=0;
};

class CTunnelPacketIn : public ITunnelPacketIn
{
public:
	CTunnelPacketIn(unsigned short signature)
	{
		m_port = 0;
		m_signature = signature;
	}
	~CTunnelPacketIn(){}

	virtual unsigned int analysePacket(const char* buf, unsigned len)=0;
	virtual const char* getData()=0;
	virtual int getDataSize()=0;
	virtual BYTE getType()=0;
	virtual unsigned short getPort()=0;
	virtual void destroy()=0;

protected:
	unsigned short	m_port;
	unsigned short	m_signature;
};
