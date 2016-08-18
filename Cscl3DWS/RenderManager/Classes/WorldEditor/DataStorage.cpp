#include "stdafx.h"
#include "datastorage.h"

CDataStorage::CDataStorage(unsigned int size):
	m_data(NULL),
	MP_STRING_INIT(m_name)
{
	Create(size);
}

void CDataStorage::SetObjectName(std::string name)
{
	m_name = name;
}

std::string CDataStorage::GetObjectName()const
{
	return m_name;
}

unsigned char CDataStorage::GetCommandID()const
{
	if ((m_size >= 1) && (m_position >= 1)) 
	{
		return m_data[0];
	}
	else
	{
		return 0;
	}
}

void CDataStorage::Create(unsigned int size)
{
	Destroy();

	m_data = MP_NEW_ARR(BYTE, size);
	memset(m_data, 0, size);
	m_size = size;

	m_position = 0;
}

BYTE* CDataStorage::GetData()
{
	return m_data;
}

unsigned int CDataStorage::GetSize()
{
	return m_size;
}

unsigned int CDataStorage::GetRealDataSize()
{
	return (unsigned int)m_position;
}

bool CDataStorage::IsEnoughSpace(unsigned int position, int dataSize)
{
	if (position + dataSize <= m_size)
	{
		return true;
	}

	return false;
}

void CDataStorage::ResizeStorage(unsigned int newSize)
{
	if (newSize <= m_size)
	{
		return;
	}

	BYTE* newData = MP_NEW_ARR(BYTE, newSize);
	memset(newData, 0, newSize);
	memcpy(&newData[0], &m_data[0], m_size*sizeof(BYTE));

	Destroy();
	m_data = newData;

	m_size = newSize;
}

void CDataStorage::Reset(int newPos)
{
	m_position = newPos;
}

void CDataStorage::Destroy()
{
	if (m_data != NULL)
	{
		MP_DELETE_ARR(m_data);
	}
}

bool CDataStorage::Add(std::string& val)
{
	short strLen = (short)val.length();

	if (!IsEnoughSpace(m_position, sizeof(strLen) + strLen))
	{
		ResizeStorage(2*(m_size + sizeof(strLen) + strLen));
	}

	Add(strLen);

	for (unsigned int i = 0; i < val.length(); i++)
	{
		Add(val[i]);
	}	
	
	return true;
}

bool CDataStorage::Add(std::wstring& val)
{
	USES_CONVERSION;
	std::string sval = W2A( val.c_str());
	return Add( sval);
}

bool CDataStorage::Read(std::string& val)
{
	short strLen = 0;

	if (!Read(strLen))
	{
		return false;
	}

	char* valBuf = MP_NEW_ARR(char, strLen+1);

	memcpy(&valBuf[0], &m_data[m_position], strLen);
	valBuf[strLen] = '\0';

	val = valBuf;

	m_position += strLen;	

	return true;
}

CDataStorage::~CDataStorage()
{
	Destroy();
}