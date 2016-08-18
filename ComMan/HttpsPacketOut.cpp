#include "stdafx.h"
#include "HttpsPacketOut.h"

CHttpsPacketOut::CHttpsPacketOut(unsigned short port, unsigned short signature, BYTE type)
:CTunnelPacketOut(port, signature, type)
{
	m_data = NULL;
	m_size = 0;
}

CHttpsPacketOut::~CHttpsPacketOut(void)
{
	if (m_data != NULL)
		MP_DELETE_ARR(m_data);
}

void CHttpsPacketOut::wrapPacket(const char* buf, int len)
{
	DWORD dwSize = 0;
	int offset = 0;
	m_size = dwSize = sizeof(m_signature) + sizeof(dwSize) + sizeof(m_port) + sizeof(m_type) + len;

	if (m_data != NULL)
		MP_DELETE_ARR(m_data);

	m_data = MP_NEW_ARR(BYTE, dwSize);

	*((WORD*)(m_data + offset)) = m_signature;
	offset += sizeof(m_signature);
	*((DWORD*)(m_data + offset)) = dwSize;
	offset += sizeof(dwSize);
	*((WORD*)(m_data + offset)) = m_port;
	offset += sizeof(m_port);
	*((BYTE*)(m_data + offset)) = m_type;
	offset += sizeof(m_type);
	if (len != 0)
		memcpy(m_data + offset, buf, len);
}

const char* CHttpsPacketOut::getData()
{
	return (char*)m_data;
}

int CHttpsPacketOut::getDataSize()
{
	return m_size;
}
