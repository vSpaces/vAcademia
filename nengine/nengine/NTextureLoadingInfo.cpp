
#include "StdAfx.h"
#include "NTextureLoadingInfo.h"
#include "GlobalSingletonStorage.h"

CNTextureLoadingInfo::CNTextureLoadingInfo():
	m_isMarkNonPOT(false),
	m_r(255), 
	m_g(255), 
	m_b(255),
	m_colorKeyType(0)
{
	SetCanBeCompressed(true);
}

bool CNTextureLoadingInfo::IsTrilinearFilteringPermitted()
{
	if (g->gi.GetVendorID() == VENDOR_INTEL)
	{
		return false;
	}

	if (g->gi.GetVendorID() == VENDOR_SIS)
	{
		return false;
	}

	return true;
}

bool CNTextureLoadingInfo::CanBeCompressed(unsigned char channelCount, unsigned char frameCount)
{
	if (g->rs.GetBool(NO_COMPRESSION))
	{
		return false;
	}

	if (!CanTextureBeCompressed())
	{
		return false;
	}

	// we will compress cubemaps only on NVidia (because of ATI Catalyst bug) 
	// and only if ARB_texture_compression is supported
	if ((6 == frameCount) && ((g->gi.GetVendorID() != VENDOR_NVIDIA) || (!g->ei.IsExtensionSupported(GLEXT_arb_texture_compression))))
	{
		return false;
	}

	if ((channelCount == 3) && (!g->rs.GetBool(NORMAL_TEXTURES_PACKING)))
	{
		return false;
	}

	if ((channelCount == 4) && (!g->rs.GetBool(TRANSPARENT_TEXTURES_PACKING)))
	{
		return false;
	}

	return g->gi.IsSaveCompressionSupported();
}

bool CNTextureLoadingInfo::CanUsePBO()
{
	if (g->gi.GetVendorID() == VENDOR_INTEL)
	{
		return false;
	}

	if (g->gi.GetVendorID() == VENDOR_SIS)
	{
		return false;
	}

	if (!g->ei.IsExtensionSupported(GLEXT_arb_pixel_buffer_object))
	{
		return false;
	}

	if (g->rs.GetBool(NO_PBO) == true)
	{
		return false;
	}

	return true;
}

bool CNTextureLoadingInfo::CanUseHardwareMipmaps(unsigned char /*channelCount*/)
{
	if (g->gi.GetVendorID() == VENDOR_INTEL)
	{
		return false;
	}

	if (g->gi.GetVendorID() == VENDOR_SIS)
	{
		return false;
	}

	return true;
}

void CNTextureLoadingInfo::SetMarkNonPOT(bool isMarkNonPOT)
{
	m_isMarkNonPOT = isMarkNonPOT;
}

bool CNTextureLoadingInfo::IsMarkNonPOT()
{
	return m_isMarkNonPOT;
}

void CNTextureLoadingInfo::CopyFrom(CNTextureLoadingInfo* loadInfo)
{
	m_isMarkNonPOT = loadInfo->m_isMarkNonPOT;
	SetMipMapStatus(loadInfo->IsMipMap());
	SetCanBeCompressed(loadInfo->CanTextureBeCompressed());
	SetFilteringMode(loadInfo->GetFilteringMode());	
	SetCanBeResized(loadInfo->CanBeResized());

	unsigned char r, g, b;
	unsigned char clrType = loadInfo->GetColorKey(r, g, b);
	SetColorKey(clrType, r, g, b);
}

void CNTextureLoadingInfo::SetColorKey(unsigned char  keyType, unsigned char r, unsigned char g, unsigned char b)
{
	m_colorKeyType = keyType;
	m_r = r;
	m_g = g;
	m_b = b;
}

unsigned char CNTextureLoadingInfo::GetColorKey(unsigned char& r, unsigned char& g, unsigned char& b)
{
	r = m_r;
	g = m_g;
	b = m_b;
	return m_colorKeyType;
}

CNTextureLoadingInfo::~CNTextureLoadingInfo()
{
}