
#pragma once

#include "TextureLoadingInfo.h"

class CNTextureLoadingInfo : public CTextureLoadingInfo
{
public:
	CNTextureLoadingInfo();
	~CNTextureLoadingInfo();

	bool CanBeCompressed(unsigned char channelCount, unsigned char frameCount);
	bool CanUsePBO();

	bool CanUseHardwareMipmaps(unsigned char channelCount);
	bool IsTrilinearFilteringPermitted();

	void SetMarkNonPOT(bool isMarkNonPOT);
	bool IsMarkNonPOT();

	void CopyFrom(CNTextureLoadingInfo* loadInfo);

	void SetColorKey(unsigned char  keyType, unsigned char r, unsigned char g, unsigned char b);
	unsigned char  GetColorKey(unsigned char& r, unsigned char& g, unsigned char& b);

private:
	bool m_isMarkNonPOT;

	unsigned char m_r, m_g, m_b;
	unsigned char  m_colorKeyType;
};