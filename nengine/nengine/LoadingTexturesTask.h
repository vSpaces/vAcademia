
#pragma once

#include "CommonEngineHeader.h"

#include <vector>
#include "ITask.h"
#include "Texture.h"
#include "TextureLoader.h"
#include "IDynamicTexture.h"

typedef struct _SLoadingTexture
{
	CTextureLoader* textureLoader;
	CTexture* texture;
	MP_WSTRING path;
	MP_STRING cacheFileName;
	void* data;
	unsigned int size;
	bool isMipmap;
	bool isMaskNeeded;
	bool isCompressedExists;

	unsigned int textureWidth;
	unsigned int textureHeight;
	unsigned int realTextureWidth;
	unsigned int realTextureHeight;

	_SLoadingTexture();	
	~_SLoadingTexture();
} SLoadingTexture;

class CLoadingTexturesTask : public ITask
{
public:
	CLoadingTexturesTask();
	~CLoadingTexturesTask();

	void SetCriticalSection(CRITICAL_SECTION* cs);

	void SetTexture(SLoadingTexture* loadingTexture, int sizeDiv);
	SLoadingTexture* GetTexture();

	void Start();
	bool IsLoaded()const;
	void OnLoaded();
	void PrintInfo();

	void Destroy() { assert(false); }
	void SelfDestroy();

	bool IsOptionalTask() { return false; }

private:
	CRITICAL_SECTION* m_loaderCS;
	SLoadingTexture* m_loadingTexture;
	bool m_isLoaded;
	int m_siveDiv;
	CTextureLoader * m_textureLoader;
	CRITICAL_SECTION m_cs;
};