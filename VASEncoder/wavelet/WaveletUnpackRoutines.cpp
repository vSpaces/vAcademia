
#include "StdAfx.h"
#include <memory.h>
#include "WaveletUnpackRoutines.h"

CWaveletUnpackRoutines::CWaveletUnpackRoutines()
{

}

CWaveletUnpackRoutines::~CWaveletUnpackRoutines()
{

}

void CWaveletUnpackRoutines::DecodeWaveletSequence(unsigned char passCount,
						   unsigned char* data, int width, int height, int quantizeShift, bool abEasyPacked)
{
	int cnt = 0;
	int widthMulModResult = 0;
	int widthShift = 0;
	int offset = 0;
	unsigned int imageSize = width * height;
	int count = 0;

	int y=0;
	int x=0;

	wlCommonRoutines.CalculateLookupsIfNeeded();
	wlCommonRoutines.AllocateWaveletBuffersIfNeeded(imageSize);

	unsigned char* offsetData = NULL;

	if (!abEasyPacked)
	{
		memcpy(wlCommonRoutines.g_tmp, data, imageSize);
		memset(wlCommonRoutines.g_mask, 0, imageSize);

		for (int passID = 0; passID < passCount; passID++)
		{
			int modValue = wlCommonRoutines.lookup_st2[passID + 1];
			int modResult = wlCommonRoutines.lookup_st2[passID];
			int widthSubModResult = width - 1 - modResult;
			int heightSubModResult = height - 1 - modResult;

			if (modResult < heightSubModResult)
			for (y = modResult; y < heightSubModResult; y += modValue)
			{
				count = passID * MAX_ROW_LENGTH;
				widthShift = y * width;
				for (x = 0; x < width; x++)
				{
					if (!wlCommonRoutines.lookup_fnd[count])
					{
						offset = widthShift + x;
						data[offset] = wlCommonRoutines.g_tmp[cnt] << quantizeShift;
						wlCommonRoutines.g_mask[offset] = 1;
						cnt++;
					}
					count++;
				}
			}
			
			count = (passID + 1) * MAX_ROW_LENGTH;
			for (y = 0; y < height; y++)
			{
				if (modResult < widthSubModResult)
				{
					widthShift = y * width;
					for (x = modResult; x < widthSubModResult; x += modValue)
					{
						if (!wlCommonRoutines.lookup_fnd[count])
						{		
							offset = widthShift + x;
							data[offset] = wlCommonRoutines.g_tmp[cnt] << quantizeShift;
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
		unsigned char* tmpData = NULL;
		offsetData = data;
		tmpData = wlCommonRoutines.g_tmp+cnt;
		for (; maskPtr != maskPtrEnd; maskPtr++, offsetData++)
		if (*maskPtr == 0)
		{
			*offsetData = *tmpData << quantizeShift;
			tmpData++;
		}
	}
	
	for (int passID = passCount - 1; passID >= 0; passID--)
	{
		int modValue = wlCommonRoutines.lookup_st2[passID + 1];
		int modResult = wlCommonRoutines.lookup_st2[passID];
		int widthSubModResult = width - 1 - modResult;
		int heightSubModResult = height - 1 - modResult;
		count = (passID + 1) * MAX_ROW_LENGTH;

		for (y = 0; y < height; y++)
		{
			if (modResult < widthSubModResult)
			{
				widthShift = y * width;
				for (x = modResult; x < widthSubModResult; x += modValue)
				{
					if (!wlCommonRoutines.lookup_fnd[count])
					{
						//offset = widthShift + x;
						offsetData = data+widthShift + x;
						if (0 == *offsetData)
						{
							*offsetData = (unsigned char)((*(offsetData - modResult) + *(offsetData + modResult)) >> 1);
						}
					}
				}
			}
			count++;
		}
		
		widthMulModResult = modResult * width;
		if (modResult < heightSubModResult)
		for (y = modResult; y < heightSubModResult; y += modValue)
		{
			count = passID * MAX_ROW_LENGTH;
			widthShift = y * width;
			for (x = 0; x < width; x++)
			{
				if (!wlCommonRoutines.lookup_fnd[count])
				{
					//offset = widthShift + x;
					offsetData = data+widthShift + x;
					if (0 == *offsetData)
					{
						*offsetData = (unsigned char)((*(offsetData - widthMulModResult) + *(offsetData + widthMulModResult)) >> 1);
					}
				}
				count++;
			}
		}
	}
}