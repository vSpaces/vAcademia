
#include "StdAfx.h"
#include "WaveletUnpacker.h"


CWaveletUnpacker::CWaveletUnpacker():
	m_data(NULL),
	m_width(0),
	m_height(0),
	m_passCount(6),
	m_uvInvScale(4),
	m_quantizeKoef(8),
	m_lastBufferSize(0)
{
}

void CWaveletUnpacker::SetQuality(unsigned char quality)
{
	m_qualitySettings.SetQuality(quality);
	m_passCount = m_qualitySettings.GetPassCount();
	m_uvInvScale = m_qualitySettings.GetUVInvScale();
	m_quantizeKoef = m_qualitySettings.GetQuantizeKoef();
}

unsigned char CWaveletUnpacker::GetQuality()const
{
	return m_qualitySettings.GetQuality();
}

void CWaveletUnpacker::SetOptions(unsigned int passCount, unsigned int uvInvScale, unsigned int quantizeKoef)
{
	m_passCount = passCount;
	m_uvInvScale = uvInvScale;
	m_quantizeKoef = quantizeKoef;
}

void CWaveletUnpacker::GetOptions(unsigned int& passCount, unsigned int& uvInvScale, unsigned int& quantizeKoef)
{
	passCount = m_passCount;
	uvInvScale = m_uvInvScale;
	quantizeKoef = m_quantizeKoef;
}

void CWaveletUnpacker::SetImageData(unsigned char* data, unsigned int width, unsigned int height)
{
	m_data = data;
	m_width = width;
	m_height = height;
}

bool CWaveletUnpacker::Unpack(bool abEasyPacked)
{
	if ((0 == m_width) || (0 == m_height) || (!m_data))
	{
		return false;
	}
	
	// dequantize
	int channelSize = m_width * m_height;
	int uvInvScaleSq = m_uvInvScale * m_uvInvScale;
	int uvChannelSize = (int)(channelSize / uvInvScaleSq);
	int widthUV = (int)(m_width / m_uvInvScale);
	int heightUV = (int)(m_height / m_uvInvScale);
	int bufferSize = channelSize * 3;

	unsigned char* data = m_data;
	unsigned char* EndOfData = m_data + channelSize + 2 * uvChannelSize;
	
	int shift = 0;
	if (m_quantizeKoef == 16)
		shift = 4;
	else if (m_quantizeKoef == 8)
		shift = 3;
	else if (m_quantizeKoef == 4)
		shift = 2;
	else if (m_quantizeKoef == 2)
		shift = 1;

/*	if (abEasyPacked)
		for ( ; data != EndOfData; data++)
		{
			*data = *data << shift;
		}
*/
	// inverse DWT
	wlUnpackRoutines.DecodeWaveletSequence(m_passCount, m_data, m_width, m_height, shift, abEasyPacked);
	wlUnpackRoutines.DecodeWaveletSequence(m_passCount, m_data + channelSize, widthUV, heightUV, shift, abEasyPacked);
	wlUnpackRoutines.DecodeWaveletSequence(m_passCount, m_data + channelSize + uvChannelSize, widthUV, heightUV, shift, abEasyPacked);

	// YUV -> RGB without UV rescale in memory
	// we just need to process UV offsets keeping in mind UV rescale
	AllocateBufferIfNeeded(bufferSize);

	int rowSize = m_width * 3;
	int stride = m_uvInvScale * 3;
	data = m_rgb;
	EndOfData = m_rgb + rowSize;

	unsigned char* y = m_data;
	unsigned char* u = m_data + channelSize;
	unsigned char* v = m_data + channelSize + uvChannelSize;
	for (unsigned int yy = 0; yy < m_height; yy++)
	{
		for ( ; data != EndOfData; data += stride, u++, v++)
		{
			for (unsigned int i = 0; i < m_uvInvScale; i++)
			{
				wlUnpackRoutines.YUVtoRGB(*y, *u, *v, data + i * 3);
				y++;
			}
		}

		if (yy%m_uvInvScale < m_uvInvScale - 1)
		{
			u -= widthUV;
			v -= widthUV;
		}

		EndOfData += rowSize;
	}

	return true;
}

void CWaveletUnpacker::CleanUp()
{
	if (m_lastBufferSize != 0)
	{
		MP_DELETE_ARR( m_rgb);

		m_lastBufferSize = 0;
	}
}

void CWaveletUnpacker::AllocateBufferIfNeeded(int bufferSize)
{
	if (m_lastBufferSize != bufferSize)
	{
		CleanUp();

		m_rgb = MP_NEW_ARR( unsigned char, bufferSize);
	}

	m_lastBufferSize = bufferSize;
}

unsigned char* CWaveletUnpacker::GetRGB()const
{
	return m_rgb;
}

unsigned int CWaveletUnpacker::GetDataSize()const
{
	return m_width * m_height * 3;
}

unsigned int CWaveletUnpacker::GetWidth()const
{
	return m_width;
}

unsigned int CWaveletUnpacker::GetHeight()const
{
	return m_height;
}

CWaveletUnpacker::~CWaveletUnpacker()
{
	CleanUp();
}