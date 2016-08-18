
#pragma once

#include "QualitySettings.h"
#include "WaveletPackRoutines.h"

#define CHANNEL_Y	0
#define CHANNEL_U	1
#define CHANNEL_V	2
#define CHANNEL_EASY_Y	3
#define CHANNEL_EASY_U	4
#define CHANNEL_EASY_V	5

class CWaveletPacker
{
public:
	CWaveletPacker();
	~CWaveletPacker();

	void SetQuality(unsigned char quality);
	unsigned char GetQuality()const;

	void SetOptions(unsigned int passCount, unsigned int zeroEpsilon, unsigned int uvInvScale, 
								unsigned int quantizeKoef);
	void GetOptions(unsigned int& passCount, unsigned int& zeroEpsilon, unsigned int& uvInvScale, 
								unsigned int& quantizeKoef);

	void SetImageData(unsigned char* data, unsigned int width, unsigned int height);
//	__forceinline unsigned char AroundDivision(unsigned char number, int shift);
	virtual bool Pack();

	unsigned char* GetChannel(unsigned char channelID)const;
	unsigned int GetChannelSize(unsigned char channelID)const;

	unsigned int GetWidth()const;
	unsigned int GetHeight()const;

	CWaveletPackRoutines wlPackRoutines;
private:
	void AllocateBuffersIfNeeded(int channelSize);
	void CleanUp();

	unsigned char* m_data;
	unsigned int m_width;
	unsigned int m_height;

	unsigned int m_passCount;
	unsigned int m_zeroEpsilon;
	unsigned int m_uvInvScale;
	unsigned int m_uvInvScaleSq;
	unsigned int m_quantizeKoef;

	unsigned char* m_y;
	unsigned char* m_u;
	unsigned char* m_v;
	unsigned int m_lastChannelSize;

	unsigned char* m_easyY;
	unsigned char* m_easyU;
	unsigned char* m_easyV;

	int m_maxColorValue;
	int m_quantizeKoefDivTwo;

	CQualitySettings m_qualitySettings;
};