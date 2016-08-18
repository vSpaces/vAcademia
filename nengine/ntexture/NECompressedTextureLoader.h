#pragma once

#include "ITextureLoader.h"
#include <vector>

class CNECompressedTextureLoader : public ITextureLoader
{
public:
	CNECompressedTextureLoader();
	~CNECompressedTextureLoader();

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

	void CleanUp();

private:
	int m_width;
	int m_height;

	bool m_isTransparent;

	std::vector<void *> m_mipMapData;
	std::vector<int> m_mipMapSize;
	std::vector<unsigned int> m_mipMapFormat;
};