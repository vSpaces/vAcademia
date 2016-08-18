
#include "StdAfx.h"
#include "MemoryResource.h"

CMemoryResource::CMemoryResource():
	m_pointer(0),
	m_fileData(NULL)
{
}

void CMemoryResource::Init(ifile* file)
{
	if (file)
	{
		m_size = file->get_file_size();
		m_fileData = MP_NEW_ARR(unsigned char, m_size);
		file->read(m_fileData, m_size);
	}
}

unsigned int CMemoryResource::GetSize()
{
	return m_size;
}

void CMemoryResource::SetSize(unsigned int size)
{
	m_size = size;
}

void CMemoryResource::SetData(const void* data)
{
	if (m_fileData)
	{
		MP_DELETE_ARR(m_fileData);
	}

	m_fileData = MP_NEW_ARR(unsigned char, m_size);
	memcpy(m_fileData, data, m_size);
}

void CMemoryResource::AttachData(const void* data)
{
	if (m_fileData)
	{
		MP_DELETE_ARR(m_fileData);
	}

	m_fileData = (unsigned char *)data;	
}

bool CMemoryResource::Read(void* data, unsigned int size)
{
	if (size > (m_size - m_pointer))
		size = m_size - m_pointer;

	memcpy(data, &m_fileData[m_pointer], size);
	return true;
}

char CMemoryResource::GetChar()
{
	char ch;
	ch = m_fileData[m_pointer];
	m_pointer++;
	return ch;
}

unsigned short CMemoryResource::GetShort()
{
	unsigned short ch;
	ch = (unsigned short)m_fileData[m_pointer];
	m_pointer += 2;
	return ch;
}

unsigned int CMemoryResource::GetLong()
{
	unsigned int ch;
	ch = (unsigned int)m_fileData[m_pointer];
	m_pointer +=4;
	return ch;
}

void* CMemoryResource::GetPointer()
{
	return m_fileData;
}

CMemoryResource::~CMemoryResource()
{
	if (m_fileData)
	{
		MP_DELETE_ARR(m_fileData);
	}
}