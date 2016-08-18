#pragma once

#include "ITextureLoader.h"
#include "TextureLoadingInfo.h"

#define TEXTURE_LOADER_FREEIMAGE		0
#define TEXTURE_LOADER_NE_COMPRESSED	1

#define TEXTURE_LOADERS_COUNT		2

class CTimeProcessor;
extern CTimeProcessor* tp;
class CInternalLogWriter;
extern CInternalLogWriter* lw;

class CTextureLoader
{
public:
	CTextureLoader();
	~CTextureLoader();

	bool LoadFromMemory(int format, void* pictureData, int width, int height, bool isTransparent);
	bool LoadTexture(std::wstring& fileName);	
	bool LoadTexture(std::wstring& fileName, const void* buffer, int size);
	bool LoadTexture(std::string& fileName, const void* buffer, int size);

	void EnlargeTexture();
	void Crop(unsigned int width, unsigned int height);
	void Resize(int width, int height);
	bool ToPOT();

	bool IsDirectChannelOrder()const;
	bool IsInverseChannelOrder()const;

	unsigned char GetChannelCount()const;
	unsigned int GetWidth()const;
	unsigned int GetHeight()const;

	void* GetData(unsigned int frameID)const;
	unsigned int GetDataSize(unsigned int frameID)const;
	unsigned int GetUserData(unsigned int frameID)const;
	bool GetMask(unsigned char** mask, unsigned int& maskSize);

	bool ApplyKeyColor(unsigned char keyR, unsigned char keyG, unsigned char keyB);
	bool ApplyKeyColorFromImage();
	bool ApplyDefaultAlpha(unsigned char alpha);

	int GetFrameCount()const;

	void LoadToOpenGL(int oglTextureID, CTextureLoadingInfo* textureInfo, int& minFilter, int& magFilter);

	unsigned char GetUsageType()const;

	void CleanUp();

private:
	void ChangeTextureType();
	void ChangeTextureType(int format);

	unsigned int GetMaxTextureSize();

	unsigned char* AllocateMask(unsigned int& maskSize);
	void FillMask(unsigned char* mask, unsigned char* dataPtr, unsigned char* dataEnd);

	int GetSourceFormat();
	int GetNormalFormat();
	int GetCompressedFormat();

	unsigned int GetMipmapFiltration(CTextureLoadingInfo* textureInfo);

	unsigned char m_pictureTypeID;
	int m_maxTextureSize;
	ITextureLoader* m_textureLoaders[TEXTURE_LOADERS_COUNT];
};