
#include "StdAfx.h"
#include "ResizeableBuffer.h"

CResizeableBuffer::CResizeableBuffer()
{
	m_data = MP_NEW_ARR(unsigned char, BUFFER_SIZE_DELTA);
	m_size = 0;
	m_dataSize = BUFFER_SIZE_DELTA;
}

void CResizeableBuffer::Clear()
{
	m_size = 0;
}

void CResizeableBuffer::Truncate(int size)
{
	if (size <= m_size)
	{
		m_size = size;
	}
}

void CResizeableBuffer::AddData(void* data, int size)
{
	if (m_size + size >= m_dataSize)
	{
		void* tmp = m_data;
		m_data = MP_NEW_ARR(unsigned char, m_dataSize + BUFFER_SIZE_DELTA);
		memcpy(m_data, tmp, m_dataSize);
		MP_DELETE_ARR(tmp);

		m_dataSize += BUFFER_SIZE_DELTA;
	}

	memcpy((unsigned char*)m_data + m_size, data, size);
	m_size += size;
}

bool CResizeableBuffer::PopData(void* data, int size)
{
	if (m_size < size)
	{
		return false;
	}

	memcpy(data, (unsigned char *)m_data + (m_size - size), size);
	m_size -= size;

	return true;
}

int CResizeableBuffer::GetSize()const
{
	return m_size;
}

void* CResizeableBuffer::GetPointer()const
{
	return m_data;
}

CResizeableBuffer::~CResizeableBuffer()
{
	MP_DELETE_ARR(m_data);
}