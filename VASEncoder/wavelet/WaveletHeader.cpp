
#include "StdAfx.h"
#include "WaveletHeader.h"

CWaveletHeader::CWaveletHeader():
	m_data(NULL),
	m_yPartCount(0)
{
}

void CWaveletHeader::SetDataPointer(unsigned char* header)
{
	m_data = header;
}

void CWaveletHeader::SetYPartCount(unsigned char partCount)
{
	m_yPartCount = partCount;
	if (m_data)
		m_data[14] = m_yPartCount;
}

unsigned char CWaveletHeader::GetYPartCount()const
{
	return (m_data ? m_data[14] : 0);
}

void CWaveletHeader::SetYPartSize(unsigned char partID, unsigned int partSize)
{
	if (m_data)
	{
		m_data[14] = m_yPartCount;
		*(unsigned int*)&m_data[15 + partID * 4] = partSize;
	}
}

unsigned int CWaveletHeader::GetYPartSize(unsigned char partID)const
{
	return (m_data ? *(unsigned int*)&m_data[15 + partID * 4] : 0);
}

void CWaveletHeader::SetUSize(unsigned int uSize)
{
	if (m_data)
	{
		*(unsigned int*)&m_data[0] = uSize;
	}
}

unsigned int CWaveletHeader::GetUSize()const
{
	return (m_data ? *(unsigned int*)&m_data[0] : 0);
}

void CWaveletHeader::SetYUVSize(unsigned int yuvSize)
{
	if (m_data)
	{
		*(unsigned int*)&m_data[4] = yuvSize;
	}
}

unsigned int CWaveletHeader::GetYUVSize()const
{
	return (m_data ? *(unsigned int*)&m_data[4] : 0);
}

void CWaveletHeader::SetWidth(unsigned short width)
{
	if (m_data)
	{
		*(unsigned short*)&m_data[8] = width;
	}
}

unsigned short CWaveletHeader::GetWidth()const
{  
	return (m_data ? *(unsigned short*)&m_data[8] : 0);
}

void CWaveletHeader::SetHeight(unsigned short height)
{
	if (m_data)
	{
		*(unsigned short*)&m_data[10] = height;
	}
}

unsigned short CWaveletHeader::GetHeight()const
{
	return (m_data ? *(unsigned short*)&m_data[10] : 0);
}

void CWaveletHeader::SetQuality(unsigned short quality)
{
	if (m_data)
	{
		*(unsigned short*)&m_data[12] = quality;
	}
}

unsigned short CWaveletHeader::GetQuality()const
{
	return (m_data ? *(unsigned short*)&m_data[12] : 0);
}

unsigned int CWaveletHeader::GetHeaderSize()const
{
	if (m_yPartCount != 0)
	{
		return 15 + m_yPartCount * 4;
	}
	else
	{
		return 15 + GetYPartCount() * 4;
	}
}

CWaveletHeader::~CWaveletHeader()
{
}