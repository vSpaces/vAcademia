#include "StdAfx.h"
#include "../include/SamplesBuffer.h"

SamplesPack::SamplesPack( byte* aData, unsigned int aDataSize)
{
	data = MP_NEW_ARR(unsigned char, aDataSize);
	if( data)
		memcpy( data, aData, aDataSize);
	dataSize = aDataSize;
}

SamplesPack::~SamplesPack()
{
	if( data)
	{
		MP_DELETE_ARR( data);
	}
}

CSamplesBuffer::CSamplesBuffer(void):
	MP_VECTOR_INIT(samplePacks)
{
	totalBytes = 0;
}

CSamplesBuffer::~CSamplesBuffer(void)
{
}

// Добавляет данные
void CSamplesBuffer::AddData( byte* aSamplesBuffer, unsigned int auDataSize)
{
	if( aSamplesBuffer == NULL)
		return;
	if( auDataSize == 0)
		return;

	MP_NEW_P2( pack, SamplesPack, aSamplesBuffer, auDataSize);	

	csSamplesPack.Lock();
	samplePacks.push_back( pack);
	totalBytes += auDataSize;
	csSamplesPack.Unlock();
}

// Возвращает количество доступных сэмплов
unsigned int CSamplesBuffer::GetAvailableDataSize()
{
	unsigned long	ret;
	csSamplesPack.Lock();
	ret = totalBytes;
	csSamplesPack.Unlock();
	return ret;
}

// Возвращает количество прочитанных сэмплов
int CSamplesBuffer::ReadData(byte* outBuffer, unsigned int bufferSize)
{
	if( !outBuffer)
		return 0;
	if( bufferSize == 0)
		return 0;

	csSamplesPack.Lock();

	int		maxBytesToRead = bufferSize;
	int		readedCount = 0;
	while( readedCount < maxBytesToRead && samplePacks.size() > 0)
	{
		bool copyAllByffer = true;
		SamplesPack* pack = *samplePacks.begin();

		int packBytes = pack->dataSize;
		if( readedCount + packBytes > maxBytesToRead)
		{
			packBytes = maxBytesToRead - readedCount;
			copyAllByffer = false;
		}

		memcpy( outBuffer, pack->data, packBytes);

		if( !copyAllByffer)
			pack->RemoveFirstBytes( packBytes);
		else
		{
			samplePacks.erase( samplePacks.begin());
			MP_DELETE( pack);
		}

		outBuffer += packBytes;
		readedCount += packBytes;
	}
	totalBytes -= readedCount;

	csSamplesPack.Unlock();

	return readedCount;
}

// Очищает буфер
void CSamplesBuffer::ClearAllData()
{
	csSamplesPack.Lock();
	SamplesPack* pack = NULL;

	VecSamplePacksIt it = samplePacks.begin(), end = samplePacks.end();
	for (;it!=end;it++)
	{
		pack = *it;
		MP_DELETE( pack);
	}
	samplePacks.clear();

	totalBytes = 0;

	csSamplesPack.Unlock();
}