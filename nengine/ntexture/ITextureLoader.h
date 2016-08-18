
#pragma once

#include <string>
#include "ImageFormats.h"

#define USAGE_TYPE_DIFFUSE_COMPRESSED			0
#define USAGE_TYPE_DIFFUSE_SPLITTED_COMPRESSED	1
#define USAGE_TYPE_DIFFUSE_NO_COMPRESS			2
#define USAGE_TYPE_NORMAL_MAP					3
#define USAGE_TYPE_SPECULAR_MAP					4

class ITextureLoader
{
public:
	virtual bool LoadFromMemory(void* pictureData, int width, int height, bool isTransparent) = 0;
	virtual bool LoadTexture(std::wstring& fileName) = 0;
	virtual bool LoadTexture(std::wstring& fileName, const void* buffer, int size) = 0;

	virtual void EnlargeTexture() = 0;
	virtual void Crop(unsigned int width, unsigned int height) = 0;
	virtual void Resize(int width, int height) = 0;
	virtual bool ToPOT() = 0;

	virtual bool IsDirectChannelOrder()const = 0;

	virtual unsigned char GetChannelCount()const = 0;
	virtual unsigned int GetWidth()const = 0;
	virtual unsigned int GetHeight()const = 0;

	virtual void* GetData(unsigned int frameID)const = 0;
	virtual unsigned int GetDataSize(unsigned int frameID)const = 0;
	virtual unsigned int GetUserData(unsigned int frameID)const = 0;

	virtual bool ApplyKeyColor(unsigned char keyR, unsigned char keyG, unsigned char keyB) = 0;
	virtual bool ApplyKeyColorFromImage() = 0;
	virtual bool ApplyDefaultAlpha(unsigned char alpha) = 0;

	virtual unsigned char GetFrameCount() = 0;

	virtual unsigned char GetUsageType() = 0;

	virtual void CleanUp() = 0;
};