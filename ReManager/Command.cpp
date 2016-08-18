#include "stdafx.h"
#include "command.h"

Command::Command(COMMAND_TYPE type)
{
	m_pData = NULL;
	m_DataSize = 5;
	m_pData = (BYTE*)malloc(m_DataSize);
	m_pData[0] = type;
	m_dataPos = 1;
	((DWORD*)(&m_pData[1]))[0] = 0;
}

Command::~Command(void)
{
}

DWORD Command::GetDataSize()
{
	return m_DataSize;
}

COMMAND_TYPE Command::GetType()
{
	return (COMMAND_TYPE)m_pData[0];
}

BYTE * Command::GetData()
{
	return m_pData;
}

void Command::ResetSize(DWORD size)
{
	BYTE type = m_pData[0];
	if (m_pData != NULL && m_DataSize != 0)
		free(m_pData);
	m_DataSize = size + 1;
	m_pData = (BYTE*)malloc(m_DataSize);
	m_pData[0] = type;
	m_dataPos = 1;
}

void Command::AddData(BYTE * data, unsigned int size)
{
	assert(m_dataPos + size <= m_DataSize);

	memcpy(&m_pData[m_dataPos], data, size);
	m_dataPos += size;
}
