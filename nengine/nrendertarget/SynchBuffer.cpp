
#include "StdAfx.h"
#include "SynchBuffer.h"

CSynchBuffer::CSynchBuffer():
	m_size(0)
{
}

void CSynchBuffer::Clear()
{
	m_size = 0;
}

void CSynchBuffer::AddData(void* data, int size)
{
	memcpy(&m_buf[size], data, size);
}

void CSynchBuffer::AddUnsignedChar(unsigned char a)
{
	m_buf[m_size] = a;
	m_size++;
}

void CSynchBuffer::AddUnsignedShort(unsigned short a)
{
	unsigned short* tmp = (unsigned short*)&m_buf[m_size];
	*tmp = a;
	m_size += 2;
}

void CSynchBuffer::AddUnsignedInt(unsigned int a)
{
	unsigned int* tmp = (unsigned int*)&m_buf[m_size];
	*tmp = a;
	m_size += 4;
}

unsigned char* CSynchBuffer::GetData()
{
	return &m_buf[0];
}

int CSynchBuffer::GetDataSize()
{
	return m_size;
}

CSynchBuffer::~CSynchBuffer()
{
}