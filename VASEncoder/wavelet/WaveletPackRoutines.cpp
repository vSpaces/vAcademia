
#include "StdAfx.h"
#include <Math.h>
#include "WaveletPackRoutines.h"
#include <memory.h>

CWaveletPackRoutines::CWaveletPackRoutines()
{

}

CWaveletPackRoutines::~CWaveletPackRoutines()
{

}

void CWaveletPackRoutines::SetQuality(unsigned int passCount, unsigned int zeroEpsilon, unsigned int uvInvScale, 
									  unsigned int quantizeKoef)
{
	m_passCount = passCount;
	m_zeroEpsilon = zeroEpsilon;
	m_uvInvScale = uvInvScale;
	m_uvInvScaleSq = uvInvScale * uvInvScale;
	m_quantizeKoef = quantizeKoef;
	m_quantizeKoefDivTwo = m_quantizeKoef / 2;

	m_shift = 0;
	if (m_quantizeKoef == 16)
		m_shift = 4;
	else if (m_quantizeKoef == 8)
		m_shift = 3;
	else if (m_quantizeKoef == 4)
		m_shift = 2;
	else if (m_quantizeKoef == 2)
		m_shift = 1;
	m_maxColorValue = 256 >> m_shift;
}

__forceinline unsigned char CWaveletPackRoutines::AroundDivision(unsigned char number)
{
	unsigned char res = number >> m_shift;
	int res2 = res+1;
	return (res2 != m_maxColorValue && (number - (res << m_shift)) >= m_quantizeKoefDivTwo) ? res2 : res;
}

void CWaveletPackRoutines::EncodeWaveletSequence(unsigned char passCount, unsigned char zeroEpsilon,
						   unsigned char* data, unsigned char* easyData, int width, int height)
{
	int cnt = 0;
	int imageSize = width * height;
	int widthShift = 0;
	int offset = 0;
	int value = 0;
	int widthMulModResult = 0;
	int count = 0;
	unsigned char* offsetData = data;

	wlCommonRoutines.CalculateLookupsIfNeeded();
	wlCommonRoutines.AllocateWaveletBuffersIfNeeded(imageSize);
	
	memset(wlCommonRoutines.g_mask, 0, imageSize);
	memset(wlCommonRoutines.g_tmp, 0, imageSize);

	for (int passID = 0; passID < passCount; passID++)
	{
		int modValue = wlCommonRoutines.lookup_st2[passID + 1];
		int modResult = wlCommonRoutines.lookup_st2[passID];
		int widthSubModResult = width - 1 - modResult;
		int heightSubModResult = height - 1 - modResult;

		unsigned char currentZeroEpsilon = (unsigned char)(zeroEpsilon / (passID + 1));

		if (modResult < heightSubModResult)
		{
			widthMulModResult = modResult * width;
			for (int y = modResult; y < heightSubModResult; y += modValue)
			{
				count = passID * MAX_ROW_LENGTH;
				widthShift = y * width;
				for (int x = 0; x < width; x++)
				{
					if (!wlCommonRoutines.lookup_fnd[count])
					{
						offset = widthShift + x;
						offsetData = data+offset;
						value = (int)((*(offsetData-widthMulModResult) + *(offsetData + widthMulModResult)) >> 1) - *offsetData;
						if (abs(value) < currentZeroEpsilon)
						{
							*offsetData = 0;						
						}
						else						
							wlCommonRoutines.g_tmp[cnt] = AroundDivision(*offsetData);
						wlCommonRoutines.g_mask[offset] = 1;
						cnt++;
					}
					count++;
				}
			}
		}
		
		count = (passID + 1) * MAX_ROW_LENGTH;
		for (int y = 0; y < height; y++)
		{
			if (modResult < widthSubModResult)
			{
				widthShift = y * width;
				for (int x = modResult; x < widthSubModResult; x += modValue)
				{
					if (!wlCommonRoutines.lookup_fnd[count])
					{
						offset = widthShift + x;
						offsetData = data+offset;
						value = (int)((*(offsetData - modResult) + *(offsetData + modResult)) >> 1) - *offsetData;
						if (abs(value) < currentZeroEpsilon)
						{
							*offsetData = 0;
						}
						else
							wlCommonRoutines.g_tmp[cnt] = AroundDivision(*offsetData);
						wlCommonRoutines.g_mask[offset] = 1;
						cnt++;
					}
				}
			}
			count++;
		} 
	}

	unsigned char* maskPtr = wlCommonRoutines.g_mask;
	unsigned char* maskPtrEnd = maskPtr + imageSize;
	offsetData = data;
	for (; maskPtr != maskPtrEnd; maskPtr++, offsetData++)
		if (*maskPtr == 0)
		{
			wlCommonRoutines.g_tmp[cnt] = AroundDivision(*offsetData);
			cnt++;
		}

	memcpy(easyData, data, imageSize);
	memcpy(data, wlCommonRoutines.g_tmp, imageSize);
}