#include "StdAfx.h"
#define ZLIB_WINAPI
#include "zlib.h"
#include "WaveletHeader.h"
#include "ZLibWaveletUnpacker.h"

CZLibWaveletUnpacker::CZLibWaveletUnpacker(ITaskManager* taskManager):
	m_unpackedData(NULL),
	m_allocatedSize(0),
	m_packedData(NULL),
	m_unpackedSize(0),
	m_packedSize(0)	
{
}

void CZLibWaveletUnpacker::SetPackedData(unsigned char* data, int size)
{
	m_packedData = data;
	m_packedSize = size;
}

bool CZLibWaveletUnpacker::Unpack(bool abEasyPacked)
{
	CWaveletHeader header;
	header.SetDataPointer(m_packedData);

	int headerSize = header.GetHeaderSize();

	int packedSizeY = 0;
	for (unsigned char partID = 0; partID < header.GetYPartCount(); partID++)
	{
		packedSizeY += header.GetYPartSize(partID);
	}

	int packedSizeU = header.GetUSize();
	int packedSizeV = m_packedSize - headerSize - packedSizeY - packedSizeU;
	
	int unpackedSize = header.GetYUVSize();
	int width = header.GetWidth();
	int height = header.GetHeight();
	int quality = header.GetQuality();
	
	AllocateBufferIfNeeded(unpackedSize);

	int size = m_allocatedSize;
	int offset = 0;
	packedSizeY = 0;
	
	for (unsigned char partID = 0; partID < header.GetYPartCount(); partID++)
	{
		size = m_allocatedSize - offset;
		int res = uncompress(m_unpackedData + offset, (uLongf *)&size, m_packedData + headerSize + packedSizeY, header.GetYPartSize(partID));
		offset += size;
		packedSizeY += header.GetYPartSize(partID);
	}

	if (abEasyPacked)
	{
		memcpy(m_unpackedData, m_packedData + headerSize, unpackedSize);
	}
	else
	{
		size = m_allocatedSize - offset;
		uncompress(m_unpackedData + offset, (uLongf *)&size, m_packedData + headerSize + packedSizeY, packedSizeU);
		offset += size;
		size = m_allocatedSize - offset;
		uncompress(m_unpackedData + offset, (uLongf *)&size, m_packedData + headerSize + packedSizeU + packedSizeY, packedSizeV);
	}
	

	SetImageData(m_unpackedData, width, height);
	SetQuality(quality);
	CWaveletUnpacker::Unpack(abEasyPacked);

	return true;
}

void CZLibWaveletUnpacker::AllocateBufferIfNeeded(int bufferSize)
{
	if ((unsigned int)bufferSize > m_allocatedSize)
	{
		CleanUp();

		m_unpackedData = MP_NEW_ARR( unsigned char, bufferSize);
		m_allocatedSize = bufferSize;
		m_unpackedSize = 0;
	}
}

void CZLibWaveletUnpacker::CleanUp()
{
	if (m_unpackedData)
	{
		MP_DELETE_ARR( m_unpackedData);
	}

	m_unpackedSize = 0;
	m_allocatedSize = 0;
}

CZLibWaveletUnpacker::~CZLibWaveletUnpacker()
{
	CleanUp();
}