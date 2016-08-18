// DataBuffer.cpp: implementation of the CDataBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataBuffer.h"

#define DATA_START_GROW_MEMSIZE 128
#define DATA_MAX_GROW_MEMSIZE 262144

//////////////////////////////////////////////////
///////////////////////////
// CDataBuffer
CDataBuffer::CDataBuffer( DWORD aSize) : pData( NULL), dwDataSize( 0), dwDataLength( 0)
{
	dataGrowSize = DATA_START_GROW_MEMSIZE;
	growRequestsCount = 0;
	grow( aSize);
	miGetIdx=0;
	miSetIdx=0;
}

CDataBuffer::~CDataBuffer()
{
	grow( 0);
}



bool CDataBuffer::grow(DWORD aNewSize)
{
	growRequestsCount++;
	if( dataGrowSize < DATA_MAX_GROW_MEMSIZE)
		dataGrowSize *= 2;

	if(aNewSize==0)
	{
		if (pData != NULL)
			MP_DELETE_ARR( pData);
		pData = NULL;
		dwDataSize = 0;
		dwDataLength = 0;
		return true;
	}
	DWORD dwMinSize = (dwDataSize < aNewSize ? dwDataSize : aNewSize);
	dwDataSize = aNewSize;
	if(pData==NULL)
	{
		pData = MP_NEW_ARR(BYTE, dwDataSize);
	}
	else
	{
		BYTE* pDataTmp = MP_NEW_ARR( BYTE, dwDataSize);
		rtl_memcpy( pDataTmp, dwDataSize, pData, dwMinSize);
		MP_DELETE_ARR(pData);
		pData = pDataTmp;
	}
	if (dwDataLength > dwDataSize)
		dwDataLength = dwDataSize;
	return true;
}

bool CDataBuffer::growFromBegin(DWORD aNewSize)
{
	if(aNewSize==0)
	{
		if (pData != NULL)
			MP_DELETE_ARR( pData);
		pData = NULL;
		dwDataSize = 0;
		dwDataLength = 0;
		return true;
	}
	DWORD dwMinSize = (dwDataSize < aNewSize ? dwDataSize : aNewSize);
	dwDataSize = aNewSize;
	if(pData==NULL)
	{
		pData = MP_NEW_ARR( BYTE, dwDataSize);
	}
	else
	{
		BYTE* pDataTmp = MP_NEW_ARR( BYTE, dwDataSize);
		rtl_memcpy( pDataTmp, dwDataSize, (void*)pData[dwDataLength-dwMinSize], dwMinSize);
		MP_DELETE_ARR(pData);
		pData = pDataTmp;
	}
	if (dwDataLength > dwDataSize)
		dwDataLength = dwDataSize;
	return true;
}

BYTE *CDataBuffer::add(const CByteVector& aData)
{
	int size = aData.size();
	if (!addData(sizeof(unsigned short), (BYTE*)&size))
		return NULL;
	return addData(aData);
}

BYTE *CDataBuffer::add(DWORD aDataSize, BYTE *aData)
{
	if (!addData(sizeof(DWORD), (BYTE*)&aDataSize))
		return NULL;
	return addData(aDataSize, aData);
}

BYTE *CDataBuffer::add(unsigned int aDataSize, BYTE *aData)
{
	if (!addData(sizeof(unsigned int), (BYTE*)&aDataSize))
		return NULL;
	return addData(aDataSize, aData);
}

BYTE *CDataBuffer::add(unsigned short aDataSize, BYTE *aData)
{
	if (!addData(sizeof(unsigned short), (BYTE*)&aDataSize))
		return NULL;
	return addData(aDataSize, aData);
}

BYTE *CDataBuffer::addStringUnicode(unsigned short aDataSize, BYTE *aData)
{
	unsigned short iDataSize = aDataSize*sizeof(wchar_t);
	if (!addData(sizeof(unsigned short), (BYTE*)&aDataSize))
		return NULL;
	return addData(iDataSize, aData);
}

BYTE *CDataBuffer::addData(DWORD aDataSize, BYTE *aData)
{
	if(aDataSize==0)
		return NULL;
	if (dwDataLength + aDataSize >= dwDataSize)
		grow( dwDataLength + aDataSize + dataGrowSize);
	DWORD pos = dwDataLength;
	if (miSetIdx!=0)
		pos = miSetIdx;
	BYTE* pNewData = &pData[ pos];
	rtl_memcpy( pNewData, (dwDataSize-pos), aData, aDataSize);
	if (miSetIdx==0)
		dwDataLength += aDataSize;
	return pNewData;
}

BYTE *CDataBuffer::addData(const CByteVector& aData)
{
	if (dwDataLength + aData.size() >= dwDataSize)
		grow( dwDataLength + aData.size() + dataGrowSize);
	BYTE* pNewData = &pData[ dwDataLength];
	std::copy( aData.begin(), aData.end(), pNewData);
	dwDataLength += aData.size();
	return pNewData;
}

BYTE *CDataBuffer::setData(BYTE* aData, DWORD aSize, DWORD destOffset /* = 0 */, DWORD sourceOffset /* = 0 */)
{
	if ( !isValidParams( aData, aSize, destOffset, sourceOffset))
		return NULL;
	if ( aSize + destOffset > getSize())	
		grow( aSize + destOffset);	
	BYTE* pNewData = &pData[destOffset];
	rtl_memcpy( pNewData, (dwDataSize - destOffset), aData + sourceOffset, aSize);
	if(destOffset+aSize>getLength())
		dwDataLength = destOffset+aSize;
	return pNewData;
}

BOOL CDataBuffer::isValidParams( const void *data, DWORD size, DWORD destOffset /* = 0 */, DWORD sourceOffset /* = 0 */)
{
	if ( data == NULL)
		return FALSE;
	if ( size <= 0)
		return FALSE;
	if ( destOffset < 0)
		return FALSE;
	if ( sourceOffset < 0)
		return FALSE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGlobalDataBuffer
bool CGlobalDataBuffer::grow(DWORD aNewSize)
{
	if(aNewSize==0)
	{
		if (pData != NULL)
			MP_DELETE_ARR( pData);
		pData = NULL;
		dwDataSize = 0;
		dwDataLength = 0;
		return true;
	}
	dwDataSize = aNewSize;
	if(pData==NULL){
		pData = MP_NEW_ARR(BYTE, dwDataSize);		
	}else
	{
		if (pData)
		{
			MP_DELETE_ARR(pData);
		}
		pData = MP_NEW_ARR(BYTE, dwDataSize);	
	}
	if (dwDataLength > dwDataSize)
		dwDataLength = dwDataSize;
	return true;
}


