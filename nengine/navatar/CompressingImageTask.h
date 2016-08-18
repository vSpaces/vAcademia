
#pragma once

#include "CommonAvatarHeader.h"

#include <vector>
#include "ITask.h"
#include "Texture.h"

class CCompressingImageTask : public ITask
{
public:
	CCompressingImageTask();
	~CCompressingImageTask();

	void SetParams(unsigned char* textureData, CTexture* texture, const std::string& cacheFileName, bool isTextureRGBA);

	void Start();

	void PrintInfo();

	bool IsCompressed()const;

	unsigned int GetSize();
	CTexture* GetTexture();
	unsigned char* GetData();
	std::string GetCacheFileName();

	void Destroy();	
	void SelfDestroy();

	bool IsOptionalTask() { return false; }

private:
	unsigned char* m_textureData;
	unsigned char* m_data;

	MP_STRING m_cacheFileName;

	CTexture* m_texture;

	unsigned int m_size;

	bool m_isCompressed;
	bool m_isTextureRGBA;
};