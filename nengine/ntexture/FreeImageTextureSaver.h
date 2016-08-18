
#pragma once

#include "ITextureSaver.h"

class CFreeImageTextureSaver : public ITextureSaver
{
public:
	CFreeImageTextureSaver();
	~CFreeImageTextureSaver();

	bool SaveTexture(int oglTextureID, int width, int height, bool isTextureTransparent, std::wstring& fileName,  SCropRect* const rect = NULL);
	bool SaveTexture(int format, int width, int height, int channelsCount, void* dataIn, void** dataOut, unsigned long* dataSize, int quality = 50);
	bool StretchTexture(int srcX, int srcY, int srcWidth, int srcHeight, int destWidth, int destHeight
		, void* dataIn, void** dataOut, unsigned long* dataSize, int quality = 50);
	void CleanUp();

	void SetForceGrayscale(bool isGrayscaleForced);
	
private:
	bool m_isGrayscaleForced;
	int m_pictureID;
	void* m_dib;

	void GetNewPictureData(int& width, int& height, SCropRect* const rect, unsigned char*& pictureData, const int size);
};