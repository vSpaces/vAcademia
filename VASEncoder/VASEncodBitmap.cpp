#include "StdAfx.h"
#include ".\vasencodbitmap.h"

#include ".\MemLeaks.h"

VASEncodBitmap::VASEncodBitmap(void)
{
	m_encodedData = NULL;
	m_encodedDataSize = 0;
}

VASEncodBitmap::~VASEncodBitmap(void)
{
	SAFE_FREE(m_encodedData);
}

CSize VASEncodBitmap::GetSize()
{
	return m_size;
}

CPoint VASEncodBitmap::GetPosition()
{
	return m_position;
}

void VASEncodBitmap::SetPosition(CPoint aPosition)
{
	m_position = aPosition;
}

void	VASEncodBitmap::SetSize(CSize aSize)
{
	m_size = aSize;
}

unsigned int VASEncodBitmap::GetBuffer( const void** appData, unsigned int* apDataSize)
{
	if( !appData)
		return 0;

	*appData = m_encodedData;
	if( apDataSize)
		*apDataSize = m_encodedDataSize;

	return m_encodedDataSize;
}

byte* VASEncodBitmap::GetBufferSetLength( unsigned int auBufferSize)
{
	SAFE_FREE(m_encodedData);
	m_encodedData = (byte*)malloc( auBufferSize);
	m_allocatedDataSize = auBufferSize;
	m_encodedDataSize = 0;
	return m_encodedData;
}

void VASEncodBitmap::SetEncodedBufferSize( unsigned int auEncodedBufferSize)
{
	if( auEncodedBufferSize <= m_allocatedDataSize)
		m_encodedDataSize = auEncodedBufferSize;
}