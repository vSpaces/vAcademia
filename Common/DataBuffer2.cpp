#include <stdafx.h>

#include "DataBuffer2.h"

#define MEMORY_SIZE_DEFAULT 1024

CDataBuffer2::CDataBuffer2()
{
	init();	
	bUseBlock = TRUE;	
}

/*CDataBuffer2::CDataBuffer2( CDataBuffer2 *DataBuffer2)
{	
	init();	
	createData( DataBuffer2);
}*/

CDataBuffer2::CDataBuffer2( const BYTE* data, int aSize, int destOffset /* = 0 */, int sourceOffset /* = 0 */)
{	
	init();
	createData( data, aSize, destOffset, sourceOffset);
}


CDataBuffer2::CDataBuffer2( void* data, int aSize, int destOffset /* = 0 */, int sourceOffset /* = 0 */)
{	
	init();
	createData( data, aSize, destOffset, sourceOffset);
}

CDataBuffer2::CDataBuffer2( CDataBuffer2 *DataBuffer2, int destOffset /* = 0 */, int sourceOffset /* = 0 */)
{	
	init();
	createData( DataBuffer2, destOffset, sourceOffset);
}


CDataBuffer2::CDataBuffer2( UCHAR v, int aSize)
{	
	init();
	createData( aSize);
	setDataByValue( v, aSize);
}

static int giDestCnt = 0;

CDataBuffer2::~CDataBuffer2()
{
giDestCnt++;
	deleteData();
}

static int giInitCnt = 0;

void CDataBuffer2::init()
{	
giInitCnt++;
	begin = 0;	
	bAttached = FALSE;
	_createData( MEMORY_SIZE_DEFAULT);
	reset();
}

//////////////////////////////////////////////////////////////////////////

void *CDataBuffer2::getData()
{
	return (UCHAR*) data + pos;
}

UCHAR *CDataBuffer2::getBytes()
{
	return (UCHAR*) data + pos;
}

void *CDataBuffer2::getCopyData()
{
	return getCopyData( size, pos);
}

void *CDataBuffer2::getCopyData( int aSize, int pos /* = 0 */)
{
	void *_data = MP_NEW_ARR( UCHAR, aSize);
	rtl_memcpy( _data, aSize, ( UCHAR *) data + pos, aSize);
	return _data;
}

void CDataBuffer2::getCopyData( void *data, int aSize, int destOffset /* = 0 */, int sourceOffset /* = 0 */)
{
	if ( !isValidParams( data, aSize) || aSize > size)
		return;

	memcpy(( UCHAR *) data + destOffset, ( UCHAR *) this->data + sourceOffset, aSize);
}

void CDataBuffer2::getCopyNewData( UCHAR **data, int aSize, int destOffset /* = 0 */, int sourceOffset /* = 0 */)
{
	if ( aSize < 1)
		return;
	ATLASSERT( FALSE);	// ниже - порча памяти если destOffset != 0
	*data = MP_NEW_ARR( UCHAR, aSize);
	rtl_memcpy(( UCHAR *) *(data) + destOffset, aSize - destOffset, ( UCHAR *) this->data + sourceOffset, aSize);
}

int CDataBuffer2::getSize( int bytes /* = 1 */)
{
	return (size - begin) / bytes;
}

void CDataBuffer2::createData( int aSize)
{
	grow( aSize);
}

void CDataBuffer2::createData( void* data, int aSize, int destOffset /* = 0 */, int sourceOffset /* = 0 */, BOOL /*bUseBlock  = TRUE */)
{	
	setData( data, aSize, destOffset, sourceOffset);		
}

void CDataBuffer2::createData( const BYTE* data, int aSize, int destOffset /* = 0 */, int sourceOffset /* = 0 */, BOOL /*bUseBlock = TRUE */)
{	
	setData( data, aSize, destOffset, sourceOffset);		
}

void CDataBuffer2::createData( CDataBuffer2 *DataBuffer2, int destOffset /* = 0 */, int sourceOffset /* = 0 */)
{
	createData( DataBuffer2->getData(), DataBuffer2->getSize(), destOffset, sourceOffset);
	setBlockType( DataBuffer2->getBlockType());
}

void CDataBuffer2::updateData( void *data, int aSize, BOOL bUseBlock /* = TRUE */)
{	
	updateData( data, aSize, 0, 0, bUseBlock);	
}

void CDataBuffer2::updateData( void *data, int aSize, int destOffset, int sourceOffset /* = 0 */, BOOL bUseBlock /* = TRUE */)
{
	if ( !isValidParams( data, aSize))
		return;

	grow( aSize + destOffset);	
	setData( data, aSize, destOffset, sourceOffset);
	setBlockType( bUseBlock);
}

int CDataBuffer2::setData( const void *data, int aSize, int destOffset /* = 0 */, int sourceOffset /* = 0 */)
{
	if ( !isValidParams( data, aSize, destOffset, sourceOffset))
		return -1;

	if ( aSize + destOffset > getSize())	
		grow( aSize + destOffset);	

	rtl_memcpy((UCHAR *) this->data + destOffset, memorySize-destOffset, (UCHAR *) data + sourceOffset, aSize);
	this->size =  getSize() < aSize + destOffset ? aSize + destOffset : this->size;
	setBlockType( TRUE);
	return this->size;
}

int CDataBuffer2::setData( CDataBuffer2 *DataBuffer2, int destOffset /* = 0 */, int sourceOffset /* = 0 */)
{
	if ( DataBuffer2 == NULL)
		return -1;
	return setData( DataBuffer2->getData(), DataBuffer2->getSize(), destOffset, sourceOffset);	
}

void CDataBuffer2::deleteData()
{
	if ( data != NULL && !bAttached)
	{
		if ( bUseBlock)
		{
			MP_DELETE_ARR( data);
		}
		else
		{
			unsigned int* _data = (unsigned int*)data;
			MP_DELETE( _data);
		}
		data = NULL;
		size = 0;
		memorySize = 0;
	}
}

BOOL CDataBuffer2::getBlockType()
{
	return bUseBlock;
}

void CDataBuffer2::setBlockType( BOOL bUseBlock)
{
	this->bUseBlock = bUseBlock;
}

void CDataBuffer2::setDataByValue( UCHAR v, int aSize, int destOffset /* = 0 */)
{
	if ( aSize < 1)
		return;
	if ( destOffset < 0 || aSize + destOffset > size)
		return;
	memset( (UCHAR *) data + destOffset, v, aSize);
}

//////////////////////////////////////////////////////////////////////////

void CDataBuffer2::attachData( UCHAR *aData, int aSize, int destOffset /* = 0 */)
{	
	deleteData();
	begin = destOffset;
	pos = begin;
	if ( aSize > memorySize)
		memorySize = __max( aSize, 2 * memorySize);
	data = aData;
	size = aSize;
	bAttached = TRUE;
}

int CDataBuffer2::addData( const void *data, int aSize, int sourceOffset /* = 0 */)
{
	return setData( data, aSize, size, sourceOffset);
}

int CDataBuffer2::addData( CDataBuffer2 *DataBuffer2, int sourceOffset /* = 0 */)
{
	return setData( DataBuffer2, size, sourceOffset);
}

void CDataBuffer2::addArray( UCHAR *data, WORD aSize)
{
	setData( (UCHAR *)&aSize, sizeof( aSize), size, 0);
	setData( data, aSize, size, 0);
}

void CDataBuffer2::addString( const char* str)
{
	addArray( ( UCHAR *) str, (WORD)strlen( str));
}

void CDataBuffer2::addString( const string& str)
{
	addArray( ( UCHAR *) str.c_str(), (WORD)str.size());
}

void CDataBuffer2::addString( const wstring& str)
{
	setData( (UCHAR *)str.size(), 2, size, 0);
	setData( ( UCHAR *)str.c_str(), str.size() * 2, size, 0);
//	addArray( ( UCHAR *) str.c_str(), str.size() * 2);
}

void CDataBuffer2::addString( const wchar_t* str)
{
	short len = (short)wcslen(str);
	setData( ( UCHAR *)&len, 2, size, 0);
	setData( ( UCHAR *)str, len * 2, size, 0);
}

void CDataBuffer2::addLargeArray( UCHAR *data, DWORD aSize)
{
	setData( (UCHAR *)&aSize, sizeof( aSize), size, 0);
	setData( data, aSize, size, 0);
}

void CDataBuffer2::addLargeString( const char* str)
{
	addLargeArray( ( UCHAR *) str, strlen( str));
}

void CDataBuffer2::addLargeString( const string& str)
{
	addLargeArray( ( UCHAR *) str.c_str(), str.size());
}


//////////////////////////////////////////////////////////////////////////

void CDataBuffer2::reset()
{
	pos = begin;	
}

void CDataBuffer2::seek( int aPos)
{
	pos = __min( getSize(), aPos + begin);
}

int CDataBuffer2::getPosition()
{
	return pos;
}

bool CDataBuffer2::endOfData()
{
	return pos >= getSize();
}

bool CDataBuffer2::mayBeReadAt( int aSize)
{
	if (pos < begin)
		return false;

	if (aSize < 0)
		return false;

	if (pos + aSize > size)
		return false;

	return true;
}

BOOL CDataBuffer2::readData( UCHAR **data, DWORD aSize, BOOL bCreate /* = FALSE */)
{
	if ( aSize == 0)
		return TRUE;
	if ( !mayBeReadAt( aSize))
		return FALSE;	
	if ( bCreate)
		getCopyNewData( data, aSize, 0, pos);
	else
		getCopyData( *data, aSize, 0, pos);
	pos += aSize;
	return TRUE;
}

BOOL CDataBuffer2::readArray( UCHAR **data, WORD &aSize, BOOL bCreate /* = FALSE */)
{
	if ( !read( aSize))
		return FALSE;
	return readData( data, aSize, bCreate);	
}

BOOL CDataBuffer2::readLargeArray( UCHAR **data, DWORD &aSize, BOOL bCreate /* = FALSE */)
{
	if ( !read( aSize))
		return FALSE;
	return readData( data, aSize, bCreate);
}

BOOL CDataBuffer2::readData( CDataBuffer2 *DataBuffer2, DWORD aSize /* = 0 */, BOOL bCreate /* = FALSE */)
{		
	if ( aSize == 0)
		aSize = getSize();
	DataBuffer2->setSize( aSize);
	if ( !mayBeReadAt( aSize))
		return FALSE;	
	if ( bCreate)
		DataBuffer2 = MP_NEW(CDataBuffer2);
	DataBuffer2->grow( aSize);
	if ( aSize > 0)
	{
		DataBuffer2->createData( data, aSize, 0, pos);
		pos += aSize;
	}	
	return TRUE;
}

BOOL CDataBuffer2::readArray( CDataBuffer2 *DataBuffer2, BOOL bCreate /* = FALSE */)
{	
	WORD aSize = 0;
	if ( !read( aSize) || aSize < 1)
		return FALSE;		
	return readData( DataBuffer2, aSize, bCreate);
}

BOOL CDataBuffer2::readLargeArray( CDataBuffer2 *DataBuffer2, BOOL bCreate /* = FALSE */)
{
	DWORD aSize = 0;
	if ( !read( aSize) || aSize < 1)
		return FALSE;		
	return readData( DataBuffer2, aSize, bCreate);
}

BOOL CDataBuffer2::readString( string &str)
{	
	WORD wSize = 0;
	UCHAR *buffer = NULL;
	if ( !readArray( &buffer, wSize, TRUE))	
		return FALSE;
	if ( wSize < 1)
		return FALSE;
	str.assign( (char*)buffer, wSize);
	MP_DELETE_ARR( buffer);
	return TRUE;
}

BOOL CDataBuffer2::readString( wstring &str)
{	
	WORD wSize = 0;
	if ( !read( wSize))
		return FALSE;
	UCHAR *buffer = NULL;
	if (!readData( &buffer, wSize * 2, TRUE))
		return FALSE;
	str.assign( (wchar_t*)buffer, wSize);
	MP_DELETE_ARR( buffer);
	return TRUE;
}

BOOL CDataBuffer2::readLargeString( string str)
{	
	DWORD dwSize = 0;
	UCHAR *buffer = NULL;
	if ( !readLargeArray( &buffer, dwSize, TRUE))
		return FALSE;
	if ( dwSize < 1)
		return FALSE;
	str.assign( (char*)buffer, dwSize);
	MP_DELETE_ARR( buffer);
	return TRUE;
}

BOOL CDataBuffer2::readDataOffset( UCHAR **data, DWORD aSize, DWORD destOffset)
{
	if ( aSize == 0)
		return TRUE;
	if ( !mayBeReadAt( aSize))
		return FALSE;	
	getCopyData( *data, aSize, destOffset, pos);
	pos += aSize;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

int CDataBuffer2::shiftl( unsigned int aPos)
{
	if ( (unsigned int)begin + aPos > (unsigned int)size)
		return 0;
	
	begin += aPos;
	pos = __min( (unsigned int)pos + aPos, (unsigned int)size);
	return size;
}

int CDataBuffer2::shiftr( unsigned int aPos)
{	
	if ( begin <	 1 || begin - aPos < 0)
		return 0;	
	begin -= aPos;	
	pos = __max( pos - aPos, 0);
	return size;
}


//////////////////////////////////////////////////////////////////////////

int CDataBuffer2::getMemorySize()
{
	return memorySize;
}

//////////////////////////////////////////////////////////////////////////

void CDataBuffer2::createBuffer( int aSize)
{
	_createData( aSize);
}

void CDataBuffer2::setDataLength( int aSize)
{
	size = aSize;
}

void CDataBuffer2::_createData( int aSize)
{
	memorySize = aSize;
	data = MP_NEW_ARR(UCHAR, memorySize);	
	size = 0;
}

void CDataBuffer2::setSize( int aSize)
{
	size = aSize;
}

void CDataBuffer2::grow( int aSize)
{
	if ( memorySize < aSize)
	{
		if ( memorySize == 0)
		{
			int _memorySize = __max( aSize, MEMORY_SIZE_DEFAULT);			
			_createData( _memorySize);
			reset();
			return;
		}
		int _memorySize = __max( aSize, 2 * memorySize);
		if ( size == 0)
		{			
			deleteData();			
			_createData( _memorySize);
			return;
		}
		int temp_size = size;				
		UCHAR *temp_data = MP_NEW_ARR(UCHAR, _memorySize);
		rtl_memcpy( temp_data, _memorySize, data, temp_size);
		deleteData();
		memorySize = _memorySize;
		attachData( temp_data, temp_size, begin);
		bAttached = FALSE;
	}
}

BOOL CDataBuffer2::isValidParams( const void *data, int size, int destOffset /* = 0 */, int sourceOffset /* = 0 */)
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

//////////////////////////////////////////////////////////////////////////