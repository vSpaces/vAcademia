
#include "StdAfx.h"
#include "TextureLoadingInfo.h"

CTextureLoadingInfo::CTextureLoadingInfo():
	m_isMipMap(true),	
	m_canBeResized(false),
	m_canBeCompressed(false),
	m_isKeepInSystemMemory(false),
	m_filteringMode(FILTERING_MODE_LINEAR)
{
}

void CTextureLoadingInfo::SetMipMapStatus(bool isMipMap)
{
	m_isMipMap = isMipMap;
}

bool CTextureLoadingInfo::IsMipMap()const
{
	return m_isMipMap;
}

void CTextureLoadingInfo::SetFilteringMode(unsigned char mode)
{
	m_filteringMode = mode;
}

unsigned char CTextureLoadingInfo::GetFilteringMode()const
{
	return m_filteringMode;
}

void CTextureLoadingInfo::SetCanBeCompressed(bool canBeCompressed)
{
	m_canBeCompressed = canBeCompressed;
}

bool CTextureLoadingInfo::CanTextureBeCompressed()const
{
	return m_canBeCompressed;
}

bool CTextureLoadingInfo::CanBeCompressed(unsigned char /*channelCount*/, unsigned char /*frameCount*/)
{
	return m_canBeCompressed;
}

bool CTextureLoadingInfo::CanUsePBO()
{
	return false;
}

bool CTextureLoadingInfo::CanUseHardwareMipmaps(unsigned char /*channelCount*/)
{
	return true;
}

bool CTextureLoadingInfo::IsTrilinearFilteringPermitted()
{
	return true;
}

bool CTextureLoadingInfo::IsMarkNonPOT()
{
	return false;
}

void CTextureLoadingInfo::SetKeepInSystemMemory(bool isKeepInSystemMemory)
{
	m_isKeepInSystemMemory = isKeepInSystemMemory;
}

bool CTextureLoadingInfo::IsKeepInSystemMemory()const
{
	return m_isKeepInSystemMemory;
}

void CTextureLoadingInfo::SetCanBeResized(bool canBeResized)
{
	m_canBeResized = canBeResized;
}

bool CTextureLoadingInfo::CanBeResized()const
{
	return m_canBeResized;
}

CTextureLoadingInfo::~CTextureLoadingInfo()
{
}