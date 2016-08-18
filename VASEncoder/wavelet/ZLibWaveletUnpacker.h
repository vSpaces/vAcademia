#pragma once

#include "WaveletUnpacker.h"
#include "..\..\nengine\nengine\ITaskManager.h"

class CZLibWaveletUnpacker : public CWaveletUnpacker
{
public:
	CZLibWaveletUnpacker(ITaskManager* taskManager);
	~CZLibWaveletUnpacker();

	void SetPackedData(unsigned char* data, int size);
	bool Unpack(bool abEasyPacked = false);

private:
	void AllocateBufferIfNeeded(int bufferSize);
	void CleanUp();

	unsigned char* m_packedData;
	unsigned int m_packedSize;
	
	unsigned char* m_unpackedData;
	unsigned int m_unpackedSize;
	unsigned int m_allocatedSize;
};