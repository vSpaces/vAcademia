#include <StdAfx.h>

#include "Frame.h"

CFrame::CFrame()
{
	this->data = NULL;
	this->size = 0;
	bUseBlock = FALSE;
}

CFrame::CFrame( CFrame *frame)
{	
	createData( frame);
}

CFrame::CFrame( void* data, int size)
{	
	createData( data, size);
}

CFrame::~CFrame()
{
	deleteData();
}

//////////////////////////////////////////////////////////////////////////

void *CFrame::getData()
{
	return data;
}

void *CFrame::getData( int pos)
{
	return (char*) data + pos;
}

int CFrame::getIntData()
{	
	return getIntData( size, 0);
}

int CFrame::getIntData( int size, int pos /* = 0 */)
{	
	unsigned char *bytes = MP_NEW_ARR( unsigned char, size);
	memcpy( bytes, ( unsigned char *) data + pos, size);
	int res = 0, shift = size;
	for ( int i = 0; i < size; i++)
	{
		shift--;
		res |= ((( int)bytes[ i] - 48 + 256) % 256) << ( 8 * shift);
	}
	MP_DELETE_ARR( bytes);
	
	return res;
}

float CFrame::getFloatData()
{	
	return getFloatData( size, 0); 
}

float CFrame::getFloatData( int size, int pos /* = 0 */)
{
	unsigned char *bytes = MP_NEW_ARR( unsigned char, size);
	memcpy( bytes, ( unsigned char *) data + pos, size);
	float fl = *(( float*)bytes);
	MP_DELETE_ARR( bytes);
	return fl;
}											 

void *CFrame::getCopyData()
{
	return getCopyData( size, 0);
}

void *CFrame::getCopyData( int size, int pos /* = 0 */)
{
	void *_data = MP_NEW_ARR( char, size);
	memcpy( _data, ( unsigned char *) data + pos, size);
	return _data;
}

void CFrame::getCopyData( void *data, int size, int destOffset /* = 0 */, int sourceOffset /* = 0 */)
{
	memcpy(( unsigned char *) data + destOffset, ( unsigned char *) this->data + sourceOffset, size);
}

int CFrame::getSize( int bytes /* = 1 */)
{
	return size / bytes;
}

void CFrame::createData( int size)
{
	this->data = MP_NEW_ARR( char, size);
}

void CFrame::createData( void* data, int size, BOOL bUseBlock /* = TRUE */)
{
	if ( size > 0 && data != NULL)
	{
		this->data = MP_NEW_ARR( char, size);
		memcpy( this->data, data, size);		
		setBlockType( bUseBlock);
	}
	else
	{
		this->data = NULL;
	}
	this->size = size;
}

void CFrame::createData( CFrame *frame)
{
	createData( frame->getData(), frame->getSize());
	setBlockType( frame->getBlockType());
}

void CFrame::createSilence( int size)
{
	if ( data != NULL && size > 0)	
		memset( data, 0, size);	
}

void CFrame::createSilence()
{
	if ( data != NULL && this->size > 0)	
		memset( data, 0, this->size);	
}

void CFrame::updateData( void *data, int size, BOOL bUseBlock /* = TRUE */)
{
	if ( size != getSize())
	{
		deleteData();
		createData( data, size, bUseBlock);
	}
	else
	{
		copyData( data, size);
		setBlockType( bUseBlock);
	}
}

void CFrame::updateData( void *data, int size, int destOffset, int sourceOffset /* = 0 */, BOOL bUseBlock /* = TRUE */)
{
	if ( size > getSize() || size != getSize() - destOffset)
	{
		deleteData();
		createData( size);		
	}	
	copyData( data, size, destOffset, sourceOffset);
	setBlockType( bUseBlock);
}

void CFrame::updateData( CFrame *frame, int destOffset /* = 0 */, int sourceOffset /*= 0*/, BOOL bUseBlock /*= TRUE*/)
{
	updateData( frame->getData(), frame->getSize(), destOffset, sourceOffset, bUseBlock);
}

int CFrame::copyData( const void *data, int size, int destOffset /* = 0 */, int sourceOffset /* = 0 */)
{
	memcpy(( unsigned char *) this->data + destOffset, ( unsigned char *) data + sourceOffset, size);
	this->size = size + destOffset;
	setBlockType( FALSE);
	return this->size;
}

int CFrame::copyBytes( const BYTE *data, int size, int destOffset /* = 0 */, int sourceOffset /* = 0 */)
{
	memcpy( ( BYTE *) this->data + destOffset, ( BYTE *) data + sourceOffset, size);
	this->size = size + destOffset;
	setBlockType( TRUE);
	return this->size;
}

int CFrame::copyData( CFrame *frame, int destOffset /* = 0 */, int sourceOffset /* = 0 */)
{
	if ( frame == NULL || frame->getData() == NULL || frame->getSize() < 1)
		return -1;
	memcpy(( unsigned char *) this->data + destOffset, ( unsigned char *) frame->getData() + sourceOffset, frame->getSize());
	this->size = frame->getSize() + destOffset;
	setBlockType( frame->getBlockType());
	return this->size;
}


int CFrame::copyIntData( int data, int size, int destOffset /* = 0 */, int sourceOffset /* = 0 */)
{	
	unsigned char *bytes = MP_NEW_ARR( unsigned char, size);
	int shift = size;
	for ( int i = 0; i < size; i++)
	{
		shift--;
		bytes[ i] = (( data >> ( shift * 8)) & 0xFF) + 48;
	}
	copyData( bytes, size, destOffset, sourceOffset);
	MP_DELETE_ARR( bytes);
	return this->size;
}

int CFrame::copyFloatData( float data, int size, int destOffset /* = 0 */, int sourceOffset /* = 0 */)
{	
	unsigned char *bytes = ( unsigned char*) &data;
	copyData( bytes, size, destOffset, sourceOffset);	
	return this->size;
}

void CFrame::deleteData()
{
	if ( data != NULL)
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
		size = 0;
	}
}

BOOL CFrame::getBlockType()
{
	return bUseBlock;
}

void CFrame::setBlockType( BOOL bUseBlock)
{
	this->bUseBlock = bUseBlock;
}

void CFrame::setEmptyData()
{
	size = 0;
}

//////////////////////////////////////////////////////////////////////////

char *CFrame::convertIntToChar( int iData, int size)
{
	char *s = MP_NEW_ARR( char, size);
	sprintf_s( s, size, "%d", iData);
	int len = strlen( s);
	for ( int i = len; i < size; i++)
		s[ i] = ' ';
	return s;
}
