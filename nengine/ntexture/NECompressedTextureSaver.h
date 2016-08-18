
#pragma once

#include "ITextureSaver.h"

class CNECompressedTextureSaver : public ITextureSaver
{
public:
	CNECompressedTextureSaver();
	~CNECompressedTextureSaver();

	bool SaveTexture(int oglTextureID, int width, int height, bool isTextureTransparent, std::wstring& fileName, SCropRect* const rect);
	bool StretchTexture(int srcX, int srcY, int srcWidth, int srcHeight, int destWidth, int destHeight
		, void* dataIn, void** dataOut, unsigned long* dataSize, int quality);
	// format is unused
	// dataIn must be NULL
	// quality is not supported now
	bool SaveTexture(int format, int width, int height, int channelsCount, void* dataIn, void** dataOut, unsigned long* dataSize, int quality = 50);

	void CleanUp();

	void SetForceGrayscale(bool isGrayscaleForced);
};