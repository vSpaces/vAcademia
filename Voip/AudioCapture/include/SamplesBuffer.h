#pragma once

#include "../../../common/ViskoeThread.h"
#include <vector>
#include "../AudioCapture.h"

struct SamplesPack
{
	byte*	data;
	unsigned int		dataSize;

	SamplesPack( byte* aData, unsigned int aDataSize);
	~SamplesPack();	

	void RemoveFirstBytes( unsigned int aBytesCount)
	{
		if( aBytesCount >= dataSize)
			dataSize = 0;
		else
		{
			memmove( data, data + aBytesCount, dataSize - aBytesCount);
			dataSize -= aBytesCount;
		}
	}
};

class VOIPAUDIOCAPTURE_API CSamplesBuffer
{
public:
	CSamplesBuffer(void);
	~CSamplesBuffer(void);

public:
	// Добавляет данные
	void AddData( byte* aSamplesBuffer, unsigned int auDataSize);

	// Очищает буфер
	void ClearAllData();

	// реализация IMp3EncoderSource
public:
	// Возвращает количество доступных сэмплов
	unsigned int GetAvailableDataSize();

	// Возвращает количество прочитанных байт
	int ReadData(byte* outBuffer, unsigned int bufferSize);

private:
	//typedef std::vector<SamplesPack*> VecSamplePacks;
	typedef MP_VECTOR<SamplesPack*> VecSamplePacks;
	typedef VecSamplePacks::iterator VecSamplePacksIt;

	unsigned long	totalBytes;
	VecSamplePacks	samplePacks;

	CAutoCriticalSection csSamplesPack;
};
