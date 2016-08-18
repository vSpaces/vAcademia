
#include "StdAfx.h"
#include "QualitySettings.h"

CQualitySettings::CQualitySettings():
	m_quality(30)
{
	Update(m_quality);
}

void CQualitySettings::SetQuality(unsigned char quality)
{
	m_quality = quality;

	Update(m_quality);
}

int CQualitySettings::GetQuality()const
{
	return m_quality;
}

unsigned int CQualitySettings::GetPassCount()const
{
	return m_passCount;
}

unsigned int CQualitySettings::GetZeroEpsilon()const
{
	return m_zeroEpsilon;
}

unsigned int CQualitySettings::GetUVInvScale()const
{
	return m_uvInvScale;
}

unsigned int CQualitySettings::GetQuantizeKoef()const
{
	return m_quantizeKoef;
}

void CQualitySettings::Update(unsigned char quality)
{
	quality = quality - quality%5;

	if (quality == 0)
	{
		m_passCount = 9;
		m_zeroEpsilon = 64;
		m_quantizeKoef = 16;
		m_uvInvScale = 4;
	}
	else if (quality == 5)
	{
		m_passCount = 9;
		m_zeroEpsilon = 32;
		m_quantizeKoef = 16;
		m_uvInvScale = 4;
	}
	else if (quality == 10)
	{
		m_passCount = 9;
		m_zeroEpsilon = 30;
		m_quantizeKoef = 16;
		m_uvInvScale = 4;
	}
	else if (quality == 15)
	{
		m_passCount = 9;
		m_zeroEpsilon = 28;
		m_quantizeKoef = 16;
		m_uvInvScale = 4;
	}
	else if (quality == 20)
	{
		m_passCount = 9;
		m_zeroEpsilon = 24;
		m_quantizeKoef = 8;
		m_uvInvScale = 4;
	}
	else if (quality == 25)
	{
		m_passCount = 9;
		m_zeroEpsilon = 20;
		m_quantizeKoef = 8;
		m_uvInvScale = 4;
	}
	else if (quality == 30)
	{
		m_passCount = 9;
		m_zeroEpsilon = 16;
		m_quantizeKoef = 8;
		m_uvInvScale = 4;
	}
	else if (quality == 35)
	{
		m_passCount = 9;
		m_zeroEpsilon = 14;
		m_quantizeKoef = 8;
		m_uvInvScale = 4;
	}
	else if (quality == 40)
	{
		m_passCount = 9;
		m_zeroEpsilon = 16;
		m_quantizeKoef = 8;
		m_uvInvScale = 2;
	}
	else if (quality == 45)
	{
		m_passCount = 9;
		m_zeroEpsilon = 11;
		m_quantizeKoef = 4;
		m_uvInvScale = 4;
	}
	else if (quality == 50)
	{
		m_passCount = 9;
		m_zeroEpsilon = 12;
		m_quantizeKoef = 8;
		m_uvInvScale = 2;
	}
	else if (quality == 55)
	{
		m_passCount = 9;
		m_zeroEpsilon = 11;
		m_quantizeKoef = 4;
		m_uvInvScale = 2;
	}
	else if (quality == 60)
	{
		m_passCount = 9;
		m_zeroEpsilon = 10;
		m_quantizeKoef = 8;
		m_uvInvScale = 2;
	}
	else if (quality == 65)
	{
		m_passCount = 9;
		m_zeroEpsilon = 9;
		m_quantizeKoef = 4;
		m_uvInvScale = 2;
	}
	else if (quality == 70)
	{
		m_passCount = 9;
		m_zeroEpsilon = 9;
		m_quantizeKoef = 8;
		m_uvInvScale = 2;
	}
	else if (quality == 75)
	{
		m_passCount = 9;
		m_zeroEpsilon = 9;
		m_quantizeKoef = 4;
		m_uvInvScale = 2;
	}
	else if (quality == 80)
	{
		m_passCount = 9;
		m_zeroEpsilon = 8;
		m_quantizeKoef = 4;
		m_uvInvScale = 2;
	}
	else if (quality == 85)
	{
		m_passCount = 9;
		m_zeroEpsilon = 7;
		m_quantizeKoef = 4;
		m_uvInvScale = 2;
	}
	else if (quality == 90)
	{
		m_passCount = 9;
		m_zeroEpsilon = 6;
		m_quantizeKoef = 4;
		m_uvInvScale = 2;
	}
	else if (quality == 95)
	{
		m_passCount = 9;
		m_zeroEpsilon = 5;
		m_quantizeKoef = 4;
		m_uvInvScale = 2;
	}
	else if (quality == 100)
	{
		m_passCount = 9;
		m_zeroEpsilon = 4;
		m_quantizeKoef = 2;
		m_uvInvScale = 2;
	}
}

CQualitySettings::~CQualitySettings()
{
}