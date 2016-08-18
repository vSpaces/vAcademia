#include "StdAfx.h"
#include ".\VASEBitmap.h"

#include ".\MemLeaks.h"

VASEBitmap::VASEBitmap(void)
{
	m_encodedData = NULL;
	m_encodedDataSize = 0;
}

VASEBitmap::~VASEBitmap(void)
{
	SAFE_DELETE_ARRAY(m_encodedData);
}

CSize VASEBitmap::GetSize()
{
	return m_size;
}

CPoint VASEBitmap::GetPosition()
{
	return m_position;
}

void VASEBitmap::SetPosition(CPoint aPosition)
{
	m_position = aPosition;
}

void	VASEBitmap::SetSize(CSize aSize)
{
	m_size = aSize;
}

unsigned int VASEBitmap::GetBuffer( const void** appData, unsigned int* apDataSize)
{
	if( !appData)
		return 0;

	*appData = m_encodedData;
	if( apDataSize)
		*apDataSize = m_encodedDataSize;

	return m_encodedDataSize;
}

byte* VASEBitmap::GetBufferSetLength( unsigned int auBufferSize)
{
	SAFE_DELETE_ARRAY(m_encodedData);
// 	SAFE_FREE(m_encodedData);
// 	m_encodedData = (byte*)malloc( auBufferSize);
	m_encodedData = MP_NEW_ARR( byte, auBufferSize);
	m_allocatedDataSize = auBufferSize;
	m_encodedDataSize = 0;
	return m_encodedData;
}

void VASEBitmap::SetEncodedBufferSize( unsigned int auEncodedBufferSize)
{
	if( auEncodedBufferSize <= m_allocatedDataSize)
		m_encodedDataSize = auEncodedBufferSize;
}