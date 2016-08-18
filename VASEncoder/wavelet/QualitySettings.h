
#pragma once

class CQualitySettings
{
public:
	CQualitySettings();
	~CQualitySettings();

	void SetQuality(unsigned char quality);
	int GetQuality()const;

	unsigned int GetPassCount()const;
	unsigned int GetZeroEpsilon()const;
	unsigned int GetUVInvScale()const; 
	unsigned int GetQuantizeKoef()const;

private:
	void Update(unsigned char quality);

	unsigned char m_quality;

	unsigned int m_passCount;
	unsigned int m_zeroEpsilon;
	unsigned int m_uvInvScale;
	unsigned int m_quantizeKoef;
};