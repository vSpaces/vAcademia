
#pragma once

#define FILTERING_MODE_PIXEL			0
#define FILTERING_MODE_LINEAR			1
#define FILTERING_MODE_LINEAR_NO_MIPMAP	2

class CTextureLoadingInfo
{
public:
	CTextureLoadingInfo();
	~CTextureLoadingInfo();

	void SetKeepInSystemMemory(bool isKeepInSystemMemory);
	bool IsKeepInSystemMemory()const;

	void SetCanBeResized(bool canBeResized);
	bool CanBeResized()const;

	void SetMipMapStatus(bool isMipMap);
	bool IsMipMap()const;

	void SetFilteringMode(unsigned char mode);
	unsigned char GetFilteringMode()const;

	void SetCanBeCompressed(bool canBeCompressed);
	bool CanTextureBeCompressed()const;
	virtual bool CanBeCompressed(unsigned char channelCount, unsigned char frameCount);

	virtual bool CanUsePBO();

	virtual bool CanUseHardwareMipmaps(unsigned char channelCount);

	virtual bool IsTrilinearFilteringPermitted();

	virtual bool IsMarkNonPOT();

private:
	bool m_isMipMap;
	bool m_canBeResized;
	bool m_canBeCompressed;
	bool m_isKeepInSystemMemory;

	unsigned char m_filteringMode;
};