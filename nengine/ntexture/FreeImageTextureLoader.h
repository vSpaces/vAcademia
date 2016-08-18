
#pragma once

#include "ITextureLoader.h"

class CFreeImageTextureLoader : public ITextureLoader
{
public:
	CFreeImageTextureLoader();
	~CFreeImageTextureLoader();

	bool LoadTexture(std::wstring& fileName);
	bool LoadTexture(std::wstring& fileName, const void* buffer, int size);
	bool LoadFromMemory(void* pictureData, int width, int height, bool isTransparent);

	unsigned char GetChannelCount()const;
	unsigned int GetWidth()const;
	unsigned int GetHeight()const;

	bool ApplyKeyColor(unsigned char keyR, unsigned char keyG, unsigned char keyB);
	bool ApplyKeyColorFromImage();
	bool ApplyDefaultAlpha(unsigned char alpha);

    void EnlargeTexture();
	bool ToPOT();
	void Resize(int width, int height);
	void Crop(unsigned int width, unsigned int height);

	void* GetData(unsigned int frameID)const;
	unsigned int GetDataSize(unsigned int frameID)const;
	unsigned int GetUserData(unsigned int frameID)const;

	bool IsDirectChannelOrder()const;

	unsigned char GetFrameCount();

	unsigned char GetUsageType();

private:
	void OnLoaded(bool isBmp);
	void CleanUp();

	unsigned char DetectUsageType(unsigned char type);

	void* m_dib;
	unsigned char m_frameCount;
	unsigned char m_usageType;
	bool reverseRGB;
};