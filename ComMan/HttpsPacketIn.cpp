#include "stdafx.h"
#include "HttpsPacketIn.h"

CHttpsPacketIn::CHttpsPacketIn(unsigned short signature)
:CTunnelPacketIn(signature)
{
	m_data = NULL;
	m_size = 0;
	m_type = 0;
}

CHttpsPacketIn::~CHttpsPacketIn(void)
{
	if (m_data != NULL)
		MP_DELETE_ARR(m_data);
}

unsigned int CHttpsPacketIn::analysePacket(const char* buf, unsigned len)
{
	if (len < 9) // header size
		return 0;

	int offset = 2;

	DWORD dwSize = 0;
	memcpy(&dwSize, buf + offset, sizeof(dwSize));
	offset += sizeof(dwSize);

	if (len < dwSize)
		return 0;

	memcpy(&m_port, buf + offset, sizeof(m_port));
	offset += sizeof(m_port);

	memcpy(&m_type, buf + offset, sizeof(m_type));
	offset += sizeof(m_type);

	if (dwSize > 9)
	{
		m_size = dwSize - offset;
		m_data = MP_NEW_ARR(BYTE, m_size);
		memcpy(m_data, buf + offset, m_size);
	}

	return dwSize;
}

const char* CHttpsPacketIn::getData()
{
	return (const char*)m_data;
}

int CHttpsPacketIn::getDataSize()
{
	return m_size;
}

BYTE CHttpsPacketIn::getType()
{
	return m_type;
}

unsigned short CHttpsPacketIn::getPort()
{
	return m_port;
}

void CHttpsPacketIn::destroy()
{
	MP_DELETE_THIS;
}

