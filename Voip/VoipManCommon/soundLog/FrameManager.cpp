#include <StdAfx.h>
#include "FrameManager.h"
#include "Frame.h"

CFrameManager::CFrameManager( int mode /* = NONE */, BOOL autoDeleteObject /*= TRUE */):
 MP_VECTOR_INIT(objects)
{
	this->mode = mode;
	this->autoDeleteObject = autoDeleteObject;	
	frameCount = 0;	
	lastFrameSize = 0;	
	totalSize = 0;
	//blockFrameCount = -1;
	blockFrameCount = 7;
	headSize = sizeof( unsigned short int);
	frameSize = 480;	
}

CFrameManager::~CFrameManager( void)
{	
	removeAllFrames();	
}

//////////////////////////////////////////////////////////////////////////

BOOL CFrameManager::addFrame( void *data, int &size, BOOL bUseBlock, int dataOffset /* = 0 */)
{	
	if ( mode == CFrameManager::NONE)
	{
		CFrame *frame = MP_NEW( CFrame);
		frame->createData( size);			
		frame->copyData( data, size, 0, dataOffset);
		frame->setBlockType( bUseBlock);	
		add( frame);
		frameCount++;
	}
	else
	{
		int sourceOffset = dataOffset;
		while ( size > 0)
		{
			CFrame *frame = MP_NEW( CFrame);
			frame->createData( frameSize);
			frame->copyData( data, size < frameSize ? size : frameSize, 0, sourceOffset);
			frame->setBlockType( bUseBlock);	
			add( frame);
			frameCount++;
			sourceOffset += frameSize;
			size -= frameSize;
		}
		size += frameSize;
	}
	return TRUE;
}

BOOL CFrameManager::updateFrame( int index, void *data, int size, BOOL bUseBlock /* = FALSE */, int destOffset /* = 0 */, int sourceOffset /* = 0 */)
{
	CFrame *frame = getFrame( index);

	//void *__data = frame->getCopyData();
	//frame->deleteData();
	//frame->createData( frameSize);
	//frame->copyData( __data, destOffset);
	frame->copyData( data, size, destOffset, sourceOffset);
	frame->setBlockType( bUseBlock);
	//MP_DELETE_ARR( __data);
	return TRUE;
}

BOOL CFrameManager::updateFrames( CFrame *frame)
{
	return updateFrames( frame->getData(), frame->getSize(), frame->getBlockType());
}

BOOL CFrameManager::updateFrames( void *data, int length, BOOL bUseBlock /* = FALSE */)
{
	if ( mode == CFrameManager::EQUAL_BY_SIZE && data != NULL && length > 0 && lastFrameSize > 0)
	{
		// сколько байтов не хватает до полного кадра
		int bytes = frameSize - lastFrameSize;
		// определяем число оставшийся байтов
		int newSize = length - bytes;
	
		if ( newSize > 0)
		{			
			updateFrame( frameCount - 1, data, bytes, bUseBlock, lastFrameSize);
			addFrame( data, newSize, bUseBlock, bytes);
			this->lastFrameSize = newSize;
		}
		else if ( newSize < 0)
		{
			updateFrame( frameCount - 1, data, length, bUseBlock, lastFrameSize);
			this->lastFrameSize += length;
		}			
		else
		{			
			updateFrame( frameCount - 1, data, bytes, bUseBlock, lastFrameSize);
			this->lastFrameSize = newSize;
			if ( blockFrameCount == -1)
				blockFrameCount = frameCount;
		}

		totalSize += length;
		return TRUE;
	}
	else if ( data != NULL && length > 0)
	{
		addFrame( data, length, bUseBlock);
		if ( length < frameSize)
			lastFrameSize = length;
		else
			lastFrameSize = 0;		
		totalSize += length;
		return TRUE;
	}
	return FALSE;
}

CFrame *CFrameManager::getFrame( int index)
{
	return get( index);
}

CFrame *CFrameManager::removeFrameAt( int index)
{
	if ( autoDeleteObject)
	{
		CFrame *frame = get( index);
		if ( frame == NULL)
			return NULL;
		totalSize -= frame->getSize();
	}	

	frameCount--;
	CFrame *frame = get( index);
	if ( frame != NULL && !autoDeleteObject)
	{
		totalSize -= frame->getSize();		
	}
	return frame;
}

void CFrameManager::removeAllFrames()
{
	clear();	
	frameCount = 0;
	//currentFrame = 0;
	lastFrameSize = 0;	
	totalSize = 0;
	blockFrameCount = 7;
}

void CFrameManager::clear()
{
	int count = getFrameCount();	
	std::vector< CFrame*>::iterator iter = objects.begin();
	std::vector< CFrame*>::iterator end_ = objects.end();
	DWORD i = 0;		
	for ( iter = objects.begin(); iter != end_; ++iter)
	{
		CFrame *frame = *iter;
		MP_DELETE( frame);
		i++;
	}
	assert( count == i);	
	objects.clear();
}

BOOL CFrameManager::isEmpty()
{
	return is_empty();
}

int CFrameManager::getFrameCount()
{
	return get_size();
}

long CFrameManager::getTotalSize()
{
	return totalSize;
}

int CFrameManager::getHeadSize()
{
	return headSize;
}

void CFrameManager::setFrameSize( int frameSize)
{
	this->frameSize = frameSize;
}

int CFrameManager::getFrameSize()
{
	return frameSize;
}

CFrame *CFrameManager::createPacket()
{
	int count = frameCount;
	CFrame *frame = MP_NEW( CFrame);	
	frame->createData( headSize + getTotalSize());
	frame->copyIntData( count, headSize);

	for ( int i = 0; i < count; i++)
	{
		CFrame *__frame = this->getFrame( i);
		int size = __frame->getSize();
		frame->copyData( __frame->getData(), size, headSize + i * size);
	}
	return frame;
}

void CFrameManager::parser( void *data, int size)
{
	CFrame *lexNumberFrame = MP_NEW( CFrame);	
	lexNumberFrame->createData( headSize);
	lexNumberFrame->copyData( data, headSize);
	lexNumberFrame->setBlockType( TRUE);
	int lexNumber = lexNumberFrame->getIntData();

	if ( lexNumber > 0)
	{
		int lexSize = ( size - headSize) / lexNumber; //3 * len;
		for ( int i = 0; i < lexNumber; i++)
		{
			addFrame( data, lexSize, 1, headSize + i * lexSize);		
		}
	}
	MP_DELETE( lexNumberFrame);
}

void CFrameManager::parser( CFrame *frame)
{
	parser( frame->getData(), frame->getSize());
}

//////////////////////////////////////////////////////////////////////////

BOOL CFrameManager::isBlockReady()
{
	return ( blockFrameCount != -1 && frameCount % blockFrameCount == 0);
}

int CFrameManager::getBlockFrameCount()
{
	return blockFrameCount;
}

int CFrameManager::getMode()
{
	return mode;
}

CFrame *CFrameManager::getData()
{
	CFrame *data = MP_NEW( CFrame);
	int size = get_size() * frameSize;
	data->createData( size);	
	
	std::vector< CFrame*>::iterator iter;
	DWORD i = 0;
	int sizeTest = 0;
	for ( iter = objects.begin(); iter != objects.end(); ++iter)
	{
		CFrame *frame = *iter;		
		int __size = frame->getSize();
		data->copyData( frame->getData(), __size, i * __size);
		i++;
		sizeTest += __size;
	}
	data->setBlockType( TRUE);
	//assert( sizeTest == size);
	return data;
}

CFrame *CFrameManager::getDubleData()
{
	char *data = MP_NEW_ARR( char, 2 * get_size() * frameSize);

	std::vector< CFrame*>::iterator iter;
	DWORD i = 0;
	for ( iter = objects.begin(); iter != objects.end(); ++iter)
	{
		CFrame *frame = *iter;
		char *frameData = ( char*) frame->getData();
		int size = frame->getSize();
		for ( int j = 0; j < size; j++)
		{
			data[ i] = frameData[ j];
			data[ i + 1] = frameData[ j];
			i += 2;
		}
	}	
	MP_NEW_P2( dataFrame, CFrame, data, 2 * get_size() * frameSize);
	MP_DELETE_ARR( data);
	return dataFrame;
}