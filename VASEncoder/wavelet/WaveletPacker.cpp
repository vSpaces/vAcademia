
#include "StdAfx.h"
#include "WaveletPacker.h"


CWaveletPacker::CWaveletPacker():
	m_data(NULL),
	m_width(0),
	m_height(0),
	m_passCount(6),
	m_uvInvScale(4),
	m_quantizeKoef(4),
	m_zeroEpsilon(64),
	m_uvInvScaleSq(16),
	m_lastChannelSize(0)
{
}

void CWaveletPacker::SetQuality(unsigned char quality)
{
	m_qualitySettings.SetQuality(quality);
	m_passCount = m_qualitySettings.GetPassCount();
	m_uvInvScale = m_qualitySettings.GetUVInvScale();
	m_uvInvScaleSq = m_uvInvScale * m_uvInvScale;
	m_zeroEpsilon = m_qualitySettings.GetZeroEpsilon();
	m_quantizeKoef = m_qualitySettings.GetQuantizeKoef();
	m_quantizeKoefDivTwo = m_quantizeKoef / 2;

	wlPackRoutines.SetQuality(m_passCount, m_zeroEpsilon, m_uvInvScale, m_quantizeKoef);
}

unsigned char CWaveletPacker::GetQuality()const
{
	return m_qualitySettings.GetQuality();
}

void CWaveletPacker::SetOptions(unsigned int passCount, unsigned int zeroEpsilon, unsigned int uvInvScale, 
								unsigned int quantizeKoef)
{
	m_passCount = passCount;
	m_zeroEpsilon = zeroEpsilon;
	m_uvInvScale = uvInvScale;
	m_uvInvScaleSq = uvInvScale * uvInvScale;
	m_quantizeKoef = quantizeKoef;
	m_quantizeKoefDivTwo = m_quantizeKoef / 2;

	wlPackRoutines.SetQuality(m_passCount, m_zeroEpsilon, m_uvInvScale, m_quantizeKoef);
}

void CWaveletPacker::GetOptions(unsigned int& passCount, unsigned int& zeroEpsilon, unsigned int& uvInvScale, 
								unsigned int& quantizeKoef)
{
	passCount = m_passCount;
	zeroEpsilon = m_zeroEpsilon;
	uvInvScale = m_uvInvScale;
	quantizeKoef = m_quantizeKoef;
}

void CWaveletPacker::SetImageData(unsigned char* data, unsigned int width, unsigned int height)
{
	m_data = data;
	m_width = width;
	m_height = height;
}
/*
__forceinline unsigned char CWaveletPacker::AroundDivision(unsigned char number, int shift)
{
	unsigned char res = number >> shift;
	int res2 = res+1;
	return (res2 != m_maxColorValue && (number - (res << shift)) >= m_quantizeKoefDivTwo) ? res2 : res;  
}
*/
bool CWaveletPacker::Pack()
{
	if ((0 == m_width) || (0 == m_height) || (!m_data))
	{
		return false;
	}
	
	// RGB -> YUV
	int channelSize = m_width * m_height;

	AllocateBuffersIfNeeded(channelSize);

	unsigned char* data = m_data;
	unsigned char* EndOfData = m_data + channelSize * 4;
	unsigned char* y = m_y;
	unsigned char* u = m_u;
	unsigned char* v = m_v;
	
	for ( ; data != EndOfData; data += 4, y++, u++, v++)
	{
		wlPackRoutines.RGBtoYUV(data, y, u, v);

		if (*y < m_quantizeKoef)
		{
			*y = m_quantizeKoef;
		}

		if (*u < m_quantizeKoef)
		{
			*u = m_quantizeKoef;
		}

		if (*v < m_quantizeKoef)
		{
			*v = m_quantizeKoef;
		}
	}

	// UV rescale
	int widthUV = (int)(m_width / m_uvInvScale);
	int heightUV = (int)(m_height / m_uvInvScale);
	unsigned int uvPitch = m_width * (m_uvInvScale - 1);

	unsigned char* u1 = m_u;
	unsigned char* v1 = m_v;
	unsigned char* currentU = m_u;
	unsigned char* currentV = m_v;
	int offsetWithuvInvScale = m_width - m_uvInvScale;

	int shift = 0;
	if (m_uvInvScaleSq == 16)
		shift = 4;
	else if (m_uvInvScaleSq == 4)
		shift = 2;

	for (int Y = 0; Y < heightUV; Y++)
	{
		for (int x = 0; x < widthUV; x++, u1 += m_uvInvScale, v1 += m_uvInvScale, currentU++, currentV++)
		{
			unsigned int u = 0, v = 0, yy, offset = 0, endOffset = m_uvInvScale;

			for (yy = 0; yy < m_uvInvScale; yy++)
			{
				for ( ; offset < endOffset; offset++)
				{
					u += *(u1 + offset);
					v += *(v1 + offset);
				}

				offset += offsetWithuvInvScale;
				endOffset += m_width;
			}
			
			*currentU = (int)(u >> shift);
			*currentV = (int)(v >> shift);
		}	

		u1 += uvPitch;
		v1 += uvPitch;
	}
	// DWT
	wlPackRoutines.EncodeWaveletSequence(m_passCount, m_zeroEpsilon * 2, m_y, m_easyY,  m_width, m_height);
	wlPackRoutines.EncodeWaveletSequence(m_passCount, m_zeroEpsilon, m_u, m_easyU, widthUV, heightUV);
	wlPackRoutines.EncodeWaveletSequence(m_passCount, m_zeroEpsilon, m_v, m_easyV, widthUV, heightUV);
/*
	// quantize
	int channelSizeUV = (int)(channelSize / m_uvInvScaleSq);
	unsigned char* yEnd = m_y + channelSize;
	unsigned char* uEnd = m_u + channelSizeUV;
	unsigned char* vEnd = m_v + channelSizeUV;
	y = m_y;
	u = m_u;
	v = m_v;

	shift = 0;
	if (m_quantizeKoef == 16)
		shift = 4;
	else if (m_quantizeKoef == 8)
		shift = 3;
	else if (m_quantizeKoef == 4)
		shift = 2;
	else if (m_quantizeKoef == 2)
		shift = 1;
	m_maxColorValue = 256 >> shift;

	for ( ; y != yEnd; y++)
	{
	//	*y = (unsigned char)(*y / m_quantizeKoef);
		*y = AroundDivision(*y, shift);
	}

	for ( ; u != uEnd; u++)
	{
	//	*u = (unsigned char)(*u / m_quantizeKoef);
		*u = AroundDivision(*u, shift);
	}

	for ( ; v != vEnd; v++)
	{
	//	*v = (unsigned char)(*v / m_quantizeKoef);
		*v = AroundDivision(*v, shift);
	}
*/
	return true;
}

void CWaveletPacker::CleanUp()
{
	if (m_lastChannelSize != 0)
	{
		MP_DELETE_ARR( m_y);
		MP_DELETE_ARR( m_u);
		MP_DELETE_ARR( m_v);

		MP_DELETE_ARR( m_easyY);
		MP_DELETE_ARR( m_easyU);
		MP_DELETE_ARR( m_easyV);

		m_lastChannelSize = 0;
	}
}

void CWaveletPacker::AllocateBuffersIfNeeded(int channelSize)
{
	if (m_lastChannelSize != channelSize)
	{
		CleanUp();

		m_y = MP_NEW_ARR( unsigned char, channelSize);
		m_u = MP_NEW_ARR( unsigned char, channelSize);
		m_v = MP_NEW_ARR( unsigned char, channelSize);

		m_easyY = MP_NEW_ARR( unsigned char, channelSize);
		m_easyU = MP_NEW_ARR( unsigned char, channelSize);
		m_easyV = MP_NEW_ARR( unsigned char, channelSize);
	}

	m_lastChannelSize = channelSize;
}

unsigned char* CWaveletPacker::GetChannel(unsigned char channelID)const
{
	unsigned char* ptr = NULL;

	switch (channelID)
	{
	case CHANNEL_Y:
		ptr = m_y;
		break;

	case CHANNEL_U:
		ptr = m_u;
		break;

	case CHANNEL_V:
		ptr = m_v;
		break;

	case CHANNEL_EASY_Y:
		ptr = m_easyY;
		break;

	case CHANNEL_EASY_U:
		ptr = m_easyU;
		break;

	case CHANNEL_EASY_V:
		ptr = m_easyV;
		break;
	}

	return ptr;
}

unsigned int CWaveletPacker::GetChannelSize(unsigned char channelID)const
{
	unsigned int channelSize = 0;

	switch (channelID)
	{
	case CHANNEL_Y:
		channelSize = m_lastChannelSize;
		break;

	case CHANNEL_U:
	case CHANNEL_V:
		channelSize = m_lastChannelSize / m_uvInvScaleSq;
		break;
	}
	
	return channelSize;
}

unsigned int CWaveletPacker::GetWidth()const
{
	return m_width;
}

unsigned int CWaveletPacker::GetHeight()const
{
	return m_height;
}

CWaveletPacker::~CWaveletPacker()
{
	CleanUp();
}