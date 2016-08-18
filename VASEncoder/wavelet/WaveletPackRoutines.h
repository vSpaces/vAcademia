
#pragma once
#include "WaveletCommonRoutines.h"

class CWaveletPackRoutines
{
public:
	CWaveletPackRoutines();
	~CWaveletPackRoutines();
	__forceinline void RGBtoYUV(unsigned char* rgb, unsigned char* y, unsigned char* u, unsigned char* v)
	{
// 		*y = (int)((rgb[0] * 299 + rgb[1] * 587 + rgb[2] * 114) / 1000);
// 		*u = (int)((-rgb[0] * 169 - rgb[1] * 331 + rgb[2] * 500) / 1000) + 127;
// 		*v = (int)((rgb[0] * 500 - rgb[1] * 419 - rgb[2] * 81) / 1000) + 127;

		*y = (1224 * rgb[0] + 2404 * rgb[1] + 466 * rgb[2]) >> 12; 
		*u = ((-691 * rgb[0] - 1356 * rgb[1] + 2048 * rgb[2]) >> 12) + 128;
		*v = ((2048 * rgb[0] - 1714 * rgb[1] - 333 * rgb[2]) >> 12) + 128;
	}

	void EncodeWaveletSequence(unsigned char passCount, unsigned char zeroEpsilon,
							unsigned char* data, unsigned char* easyData, int width, int height);
	void SetQuality(unsigned int passCount, unsigned int zeroEpsilon, unsigned int uvInvScale, 
		unsigned int quantizeKoef);
	__forceinline unsigned char AroundDivision(unsigned char number);
	CWaveletsCommonRoutines wlCommonRoutines;

	unsigned int m_passCount;
	unsigned int m_zeroEpsilon;
	unsigned int m_uvInvScale;
	unsigned int m_uvInvScaleSq;
	unsigned int m_quantizeKoef;
	int m_quantizeKoefDivTwo;
	int m_maxColorValue;
	int m_shift;
};