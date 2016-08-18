
#pragma once

#include <string>
#include "ImageFormats.h"

typedef struct _SCropRect{
	int width;
	int height;
	int x;
	int y;
} SCropRect;

class ITextureSaver
{
public:
	virtual bool SaveTexture(int oglTextureID, int width, int height, bool isTextureTransparent, std::wstring& fileName, SCropRect* const rect = NULL) = 0;
	virtual bool SaveTexture(int format, int width, int height, int channelsCount, void* dataIn, void** dataOut, unsigned long* dataSize, int quality = 50) = 0;
	virtual bool StretchTexture(int srcX, int srcY, int srcWidth, int srcHeight, int destWidth, int destHeight
		, void* dataIn, void** dataOut, unsigned long* dataSize, int quality = 50) = 0;
	virtual void CleanUp() = 0;
	virtual void SetForceGrayscale(bool isGrayscaleForced) = 0;
};