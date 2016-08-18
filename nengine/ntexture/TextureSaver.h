
#pragma once

#include "ITextureSaver.h"

#define TEXTURE_SAVER_FREE_IMAGE	0
#define TEXTURE_SAVER_NE_COMPRESSED	1

#define TEXTURE_SAVERS_COUNT		2

class CTextureSaver
{
public:
	CTextureSaver();
	~CTextureSaver();

	bool SaveTexture(int oglTextureID, int width, int height, bool isTextureTransparent, std::wstring& fileName, SCropRect* const rect = NULL);
	bool SaveTexture(int format, int width, int height, int channelsCount, void* dataIn, void** dataOut, unsigned long* dataSize, int quality = 50);
	bool StretchTexture(int srcX, int srcY, int srcWidth, int srcHeight, int destWidth, int destHeight
		, void* dataIn, void** dataOut, unsigned long* dataSize, int quality = 50);
	void CleanUp();

	void SetForceGrayscale(bool isGrayscaleForced);

private:
	void ChangeTextureType();
	void ChangeTextureType(int format);

	ITextureSaver* m_textureSavers[TEXTURE_SAVERS_COUNT];

	unsigned char m_pictureTypeID;
};