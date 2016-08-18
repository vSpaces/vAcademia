#include "stdafx.h"
#include "datastorage.h"

CDataStorage::CDataStorage(unsigned int size):
	m_data(NULL),
	MP_STRING_INIT(m_name)
{
	Create(size);
}

CDataStorage::CDataStorage( BYTE *aData, unsigned int aSize):
MP_STRING_INIT(m_name)
{
	m_size = aSize;
	m_data = MP_NEW_ARR(BYTE, m_size);
	memcpy(m_data, aData, m_size);
	//m_data = aData;
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

bool CDataStorage::Add( const char *val)
{
	return Add( (char *)val);
}

bool CDataStorage::Add( char *val)
{
	short strLen = (short)strlen(val);

	if (!IsEnoughSpace(m_position, sizeof(strLen) + strLen))
	{
		ResizeStorage(2*(m_size + sizeof(strLen) + strLen));
	}

	Add(strLen);

	memcpy(&m_data[m_position], (BYTE*)val, strLen);
	m_position += strLen;
	/*for (unsigned int i = 0; i < val.length(); i++)
	{
	Add(val[i]);
	}*/	

	return true;
}

bool CDataStorage::Add(std::string& val)
{
	short strLen = (short)val.length();

	if (!IsEnoughSpace(m_position, sizeof(strLen) + strLen))
	{
		ResizeStorage(2*(m_size + sizeof(strLen) + strLen));
	}

	Add(strLen);

	memcpy(&m_data[m_position], (BYTE*)val.c_str(), strLen);
	m_position += strLen;
	/*for (unsigned int i = 0; i < val.length(); i++)
	{
		Add(val[i]);
	}*/	
	
	return true;
}

bool CDataStorage::Add( const wchar_t *val)
{
	return Add( (wchar_t *)val);
}

bool CDataStorage::Add( wchar_t *val)
{
	short strLen = (short) wcslen( val);
	short realSize = strLen * sizeof(wchar_t);

	if (!IsEnoughSpace(m_position, sizeof(realSize) + realSize))
	{
		ResizeStorage(2*(m_size + sizeof(realSize) + realSize));
	}

	Add(strLen);

	memcpy(&m_data[m_position], (BYTE*)val, realSize);
	m_position += realSize;
	/*for (unsigned int i = 0; i < val.length(); i++)
	{
	Add(val[i]);
	}*/	

	return true;
}

bool CDataStorage::Add(std::wstring& val)
{
	short strLen = (short)val.length();
	short realSize = strLen * sizeof(wchar_t);

	if (!IsEnoughSpace(m_position, sizeof(realSize) + realSize))
	{
		ResizeStorage(2*(m_size + sizeof(realSize) + realSize));
	}

	Add(strLen);

	memcpy(&m_data[m_position], (BYTE*)val.c_str(), realSize);
	m_position += realSize;
	/*for (unsigned int i = 0; i < val.length(); i++)
	{
		Add(val[i]);
	}*/	

	return true;
}

bool CDataStorage::Read(std::string& val)
{
	short strLen = 0;

	if (!Read(strLen))
	{
		return false;
	}
	
	if( strLen < 0)
		return false;

	if( strLen == 0)
		return true;

	char* valBuf = MP_NEW_ARR(char, strLen+1);

	memcpy(&valBuf[0], &m_data[m_position], strLen);
	valBuf[strLen] = '\0';

	val = valBuf;
	MP_DELETE_ARR(valBuf);

	m_position += strLen;	

	return true;
}

bool CDataStorage::Read(std::wstring& val)
{
	short strLen = 0;

	if (!Read(strLen))
	{
		return false;
	}

	if( strLen < 0)
		return false;

	if( strLen == 0)
		return true;

	wchar_t* valBuf = MP_NEW_ARR(wchar_t, (strLen+1) * sizeof(wchar_t));

	memcpy(&valBuf[0], &m_data[m_position], strLen * sizeof(wchar_t));
	valBuf[strLen] = '\0';

	val = valBuf;
	MP_DELETE_ARR(valBuf);
	m_position += strLen * sizeof(wchar_t);	

	return true;
}

#ifdef	MEMORY_PROFILING
bool CDataStorage::Read(MP_STRING& val)
{
	return Read((std::string&)val);
}

bool CDataStorage::Read(MP_WSTRING& val)
{
	return Read((std::wstring&)val);
}
#endif

CDataStorage::~CDataStorage()
{
	Destroy();
}