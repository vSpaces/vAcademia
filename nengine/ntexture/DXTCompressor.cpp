
#include "StdAfx.h"
#include "DXT.h"
#include "Util.h"
#include <glew.h>
#include "DXTCompressor.h"
#include "HelperFunctions.h"
#include "GlobalSingletonStorage.h"

#define MAX_LINEAR_TEXTURE_SIZE	4096

template <unsigned char channelCount> 
void MipImageX(unsigned char* inData, const unsigned short width, const unsigned short height)
{
	unsigned int size = width * height * channelCount;
	for (unsigned int cnt = 0, ofs = 0; cnt < size; cnt += channelCount)
	{
		for (unsigned char k = 0; k < channelCount; k++)
		{
			inData[ofs] = (inData[cnt] + inData[cnt + channelCount]) / 2;
			cnt++;
			ofs++;		
		}
	}
}

template <unsigned char channelCount>
void MipImageY(unsigned char* inData, const unsigned short width, const unsigned short height)
{
	unsigned int rowOffset = width * channelCount;
	unsigned int cnt = 0, ofs = 0;
	unsigned short newHeight = (unsigned short)(height / 2);

	for (unsigned short y = 0; y < newHeight; y++)
	{
		for (unsigned short x = 0; x < width * channelCount; x++)
		{
			inData[ofs] = (inData[cnt] + inData[cnt + rowOffset]) / 2;
			cnt++;
			ofs++;		
		}

		cnt += rowOffset;
	}
}

template <unsigned char channelCount>
void MipImage(unsigned char* inData, unsigned short oldWidth, unsigned short oldHeight, unsigned short width, unsigned short height)
{
	if (oldWidth == width)
	{
		MipImageY<channelCount>(inData, oldWidth, oldHeight);
	}
	else if (oldHeight == height)
	{
		MipImageX<channelCount>(inData, oldWidth, oldHeight);
	}
	else
	{
		MipImageY<channelCount>(inData, oldWidth, oldHeight);
		MipImageX<channelCount>(inData, oldWidth, height);
	}
}

template <unsigned char channelCount>
void SwapChannels(unsigned char* data, unsigned short width, unsigned short height)
{
	int size = width * height * channelCount;

	for (int ofs = 0; ofs < size; ofs += channelCount)
	{
		unsigned char tmp = data[ofs];
		data[ofs] = data[ofs + 2];
		data[ofs + 2] = tmp;
	}
}

bool CompressImage(void* inData, void** outData, unsigned short width, unsigned short height, 
				   char channelCount, bool isDirectOrder, bool isMipmapRequired, unsigned int& size)
{
	if ((width < 4) || (height < 4))
	{
		return false;
	}

	bool isChannelAddingPermitted = true;
	if (-3 == channelCount)
	{
		isChannelAddingPermitted = false;
		channelCount = 3;
	}

	if ((channelCount < 3) || (channelCount > 4))
	{
		return false;
	}

	if (!inData)
	{
		return false;
	}

	if (!outData)
	{
		return false;
	}

	if ((0 == width) || (width > MAX_LINEAR_TEXTURE_SIZE) || 
		(0 == height) || (height > MAX_LINEAR_TEXTURE_SIZE))
	{
		return false;
	}

	if ((!IsPowerOf2(width)) || (!IsPowerOf2(height)))
	{
		return false;
	}

	unsigned char* tmpMem = NULL;

	if (isChannelAddingPermitted)
	if (3 == channelCount)
	{
		int ofs = 0;
		unsigned char* _data = (unsigned char*)inData;
		tmpMem = MP_NEW_ARR(unsigned char, width * height * 4);
		inData = tmpMem;
		unsigned char* data = (unsigned char*)inData;
		
		for (int k = 0; k < width * height; k++, ofs += 3)
		{
			data[k * 4] = _data[ofs];
			data[k * 4 + 1] = _data[ofs + 1];
			data[k * 4 + 2] = _data[ofs + 2];
			data[k * 4 + 3] = 255;
		}
	}

	if (!isDirectOrder)
	{
		/*if (3 == channelCount)
		{
			SwapChannels<3>((unsigned char*)inData, width, height);
		}
		else
		{*/
			SwapChannels<4>((unsigned char*)inData, width, height);
		//}
	}

	unsigned char* _resData = /*(unsigned char *)memalign(16, (int)(width * height * 1.4))*/MP_NEW_ARR(unsigned char, width * height * 2 + 10000);
	unsigned char* resData =_resData;
	
	*(unsigned short*)resData = 1;
	resData += 2;
	*(unsigned short*)resData = (channelCount == 4);
	resData += 2;
	*(unsigned short*)resData = width;
	resData += 2;
	*(unsigned short*)resData = height;
	resData += 2;

	unsigned short levelCount = 0;

	int format = (channelCount == 3) ? GL_COMPRESSED_RGB_S3TC_DXT1_EXT : GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;

	bool isWorking = true;
	while (isWorking)
	{
		int size = 0;

		if (3 == channelCount)
		{
			CompressImageDXT1((const byte *)inData, resData + 8, width, height, size);
		}
		else
		{
			CompressImageDXT5((const byte *)inData, resData + 8, width, height, size);
		}

		*(unsigned int*)resData = size;
		resData += 4;
		*(unsigned int*)resData = format;
		resData += 4;

		resData += size;

		levelCount++;
		if ((1 == levelCount) && (!isMipmapRequired))
		{
			break;
		}

		if ((width == 1) && (height == 1))
		{
			break;
		}

		unsigned short oldWidth = width;
		unsigned short oldHeight = height;

		if (width != 1)
		{
			width /= 2;
		}

		if (height != 1)
		{
			height /= 2;
		}

		MipImage<4>((unsigned char *)inData, oldWidth, oldHeight, width, height);
	}

	*(unsigned short*)_resData = levelCount;
	*outData = (void *)_resData;

	size = resData - _resData;

	if (tmpMem)
	{
		MP_DELETE_ARR(tmpMem);
		tmpMem = NULL;
	}

	return true;
}