
#include "StdAfx.h"
#include "MapServerDataBlock.h"

CMapServerPacketOutBase::CMapServerPacketOutBase():
	m_count(0),
	MP_VECTOR_INIT(m_buffer)
{
	Init(0);
}

CMapServerPacketOutBase::CMapServerPacketOutBase(int /*reserveSize*/):
	m_count(0),
	MP_VECTOR_INIT(m_buffer)
{
	m_buffer.reserve(m_count);
	Init(0);
}

CMapServerPacketOutBase::~CMapServerPacketOutBase()
{
}

void CMapServerPacketOutBase::Init(int packageID)
{
	Add(packageID);
}

/*void CMapServerPacketOutBase::SetPackageID(int packageID)
{
	int* pPackageID = (int*)&m_buffer[0];
	*pPackageID = packageID;

	return;
}*/

void CMapServerPacketOutBase::Add(unsigned long value)
{
	Add((char *)&value, 4);
}
void CMapServerPacketOutBase::Add(unsigned int value)
{
	Add((char *)&value, 4);
}

void CMapServerPacketOutBase::Add(unsigned short value)
{
	Add((char *)&value, 2);
}

void CMapServerPacketOutBase::Add(unsigned char value)
{
	Add((char *)&value, 1);
}

void CMapServerPacketOutBase::Add(float value)
{
	Add((char *)&value, 4);
}

void CMapServerPacketOutBase::Add(int value)
{
	Add((char *)&value, 4);
}

void CMapServerPacketOutBase::Add(short value)
{
	Add((char *)&value, 2);
}

void CMapServerPacketOutBase::Add(char value)
{
	Add((char *)&value, 1);
}

void CMapServerPacketOutBase::Add(CComString& value)
{
	int size = value.GetLength();
	Add((char *)&size, 4);
	
	for (int i = 0; i < value.GetLength(); i++)
	{
		char p = value[i];
		Add(&p, 1);
	}
}

void CMapServerPacketOutBase::Add(std::string& value)
{
	int size = value.size();
	Add((char *)&size, 4);
	
	for (int i = 0; i < value.size(); i++)
	{
		char p = value[i];
		Add(&p, 1);
	}
}

void CMapServerPacketOutBase::Add(const wchar_t* apwcValue)
{
	unsigned short size = 0;
	if( apwcValue)
		size = wcslen( apwcValue);
	Add((char *)&size, 2);
	Add((char *)apwcValue, size*sizeof(wchar_t));
}

void CMapServerPacketOutBase::Add(char* pBuffer, int size)
{
	for (int i = 0; i < size; i++)
	{
		m_buffer.push_back(pBuffer[i]);
	}

	m_count += size;
}

void CMapServerPacketOutBase::Add(unsigned char* pBuffer, int size)
{
	for (int i = 0; i < size; i++)
	{
		m_buffer.push_back(pBuffer[i]);
	}
	m_count += size;
}

const unsigned char* CMapServerPacketOutBase::GetData() const
{
	return &m_buffer[0];
}

int CMapServerPacketOutBase::GetDataSize() const
{
	return m_count;
}
