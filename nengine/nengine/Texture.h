
#pragma once

#include "CommonEngineHeader.h"

#include "NTextureLoadingInfo.h"
#include "BaseChangesInformer.h"
#include "IDynamicTexture.h"
#include "OcclusionQuery.h"
#include "CommonObject.h"
#include <string>
#include <vector>

#include "IAsynchResource.h"
#include "IAsynchResourceHandler.h"

#define TEXTURE_NOT_LOADED_YET	0
#define TEXTURE_LOADED			1
#define TEXTURE_LOAD_ERROR		2

#define KEY_COLOR_NOT_USED			0
#define KEY_COLOR_FROM_OPTIONS		1
#define KEY_COLOR_FROM_IMAGE		2

class CTexture : public CCommonObject, public IAsynchResourceHandler, public CBaseChangesInformer
{
public:
	CTexture();
	~CTexture();

	void SetObligatoryMipmaps(bool isEnabled);

	void QuerySizeFromOpenGL(int& width, int& height);

	void UpdateCurrentFiltration(const int minFilter, const int magFilter);
	void SetCurrentFiltration(const int minFilter, const int magFilter, const int anisotropy, const float mipmapBias);

	void SetNeedToCheckVisibility(bool isNeeded);
	__forceinline bool IsNeededToCheckVisibility()const
	{
		return m_isNeedToCheckVisibility;
	}

	__forceinline void SetKeyColor(unsigned char type)
	{
		m_keyColorType = type;
	}

	int GetKeyColorType()const;	

	__forceinline void SetPassFrameCount(const int frameCount)
	{
		m_passFrameCount = frameCount;
	}

	__forceinline int GetPassFrameCount()const
	{
		return m_passFrameCount;
	}

	__forceinline void KeepInSystemMemory(const bool isKeepInSystemMemory)
	{
		m_isKeepInSystemMemory = isKeepInSystemMemory;
	}

	__forceinline bool IsKeepInSystemMemory()const
	{
		return m_isKeepInSystemMemory;
	}

	__forceinline void SetDynamicTexture(IDynamicTexture* const texture)
	{
		m_dynamicTexture = texture;
	}

	__forceinline IDynamicTexture* GetDynamicTexture()
	{
		return m_dynamicTexture;
	}

	__forceinline float GetMipmapBias()
	{
		return m_mipmapBias;
	}

	__forceinline int GetMinFilter()
	{
		return m_minFilter;
	}

	__forceinline int GetMagFilter()
	{
		return m_magFilter;
	}

	int GetTextureType()const;
	void SetTextureType(const int type);

	int GetTextureWidth()const;
	int GetTextureRealWidth()const;
	void SetTextureWidth(const int width);
	void SetTextureRealWidth(const int width);

	int GetTextureHeight()const;
	int GetTextureRealHeight()const;
	void SetTextureHeight(const int height);
	void SetTextureRealHeight(const int height);

	void SetTextureSizeBeforeResize(const int width, const int height, const int realWidth, const int realHeight);
	int GetTextureRealWidthBeforeResize()const;
	int GetTextureRealHeightBeforeResize()const;
	int GetTextureHeightBeforeResize()const;
	int GetTextureWidthBeforeResize()const;

	bool IsNeedPreMultiply();
	void SetPreMultiplyStatus(bool isNeedPreMultiply);
	void PremultiplyAlpha();


	__forceinline int CTexture::GetNumber()const
	{
		return m_oglTextureID;
	}
	void SetNumber(const int number);

	bool IsTransparent()const;
	void SetTransparentStatus(const bool transparentStatus);

	bool IsYInvert()const;
	void SetYInvertStatus(const bool isYInvert);

	bool IsXInvert()const;
	void SetXInvertStatus(const bool isXInvert);

	void SetColor(const unsigned char r, const unsigned char g, const unsigned char b);
	void GetColor(unsigned char& r, unsigned char& g, unsigned char& b);

	bool GetPixelColor(const int x, const int y, unsigned char& r, unsigned char& g, unsigned char& b);
	
	void FreeResources();

	__forceinline bool IsEnabled()const
	{
		return m_isEnabled;
	}

	void SetMask(unsigned char* const mask, const unsigned int maskSize);
	bool IsPixelTransparent(const int x, const int y) const;

	__forceinline void CTexture::SetBindedStatus(const bool isBinded)
	{
		m_isBinded = isBinded;
	}
	__forceinline bool IsBinded()const
	{
		return m_isBinded;
	}

	void SetReallyUsedStatus(const bool isReallyUsed);
	__forceinline bool IsReallyUsed()const
	{
		return m_isReallyUsed;
	}

	void StartOcclusionQuery();
	void EndOcclusionQuery();
	bool GetOcclusionQueryResult();
	unsigned int GetOcclusionQueryPixels();

	void SetRawData(void* const rawData, const unsigned int dataSize, const bool isDirectChannelOrder);
	void DetachRawData();
	void* GetRawData();
	bool IsRawDirectChannelOrder()const;

	void SetLoadedStatus(const int loadedStatus);
	__forceinline int GetLoadedStatus()const
	{
		return m_loadedStatus;
	}

	__forceinline CNTextureLoadingInfo* GetLoadingInfo()
	{
		return &m_textureLoadingInfo;
	}

	void SetPath(const std::wstring& path);
	std::wstring GetPath()const;

	
	void SetHasNoMipmaps(const bool hasNoMipmaps);
	bool HasNoMipmaps()const;

	void SetMaxDecreaseKoef(const unsigned char koef);
	unsigned char GetMaxDecreaseKoef()const;

	void AddOwner(void* owner);
	void DeleteOwner(void* owner);
	std::vector<void *>* GetOwnersPtr();

public:
	void OnAsynchResourceLoadedPersent(IAsynchResource* asynch, unsigned char percent);
	void OnAsynchResourceLoaded(IAsynchResource* asynch);
	void OnAsynchResourceError(IAsynchResource* asynch);

private:
	void Free();

	int m_alpha;
	int m_oglTextureID;
	int m_type;
	int m_width;
	int m_realWidth;
	int m_height;
	int m_realHeight;
	int m_textureWidthBeforeResize;
	int m_textureHeightBeforeResize;
	int m_textureRealWidthBeforeResize;
	int m_textureRealHeightBeforeResize ;
	int m_passFrameCount;
	int m_anisotropy;
	int m_minFilter, m_magFilter;
	int m_loadedStatus;

	unsigned int m_rawDataSize;
	
	float m_mipmapBias;
	
	bool m_isTransparent;
	bool m_isYInvert;
	bool m_isXInvert;
	bool m_isEnabled;
	bool m_isBinded;
	bool m_isReallyUsed;
	bool m_hasNoMipmaps;
	bool m_isKeepInSystemMemory;
	bool m_isRawDirectChannelOrder;
	bool m_isNeedToCheckVisibility;

	unsigned char m_r, m_g, m_b;
	unsigned char m_maxDecreaseKoef;
	unsigned char* m_mask;
	unsigned int m_maskSize;
	unsigned char m_keyColorType;

	IDynamicTexture* m_dynamicTexture;

	COcclusionQuery* m_occlusionQuery;

	CNTextureLoadingInfo m_textureLoadingInfo;

	MP_WSTRING m_path;

	void* m_rawData;

	bool m_isNeedPreMultiply;
	bool m_isMipmapsObligatory;

	std::vector<void *> m_owners;
};
