
#include "StdAfx.h"
#include "TextureSaver.h"
#include "FreeImageTextureSaver.h"
#include "NECompressedTextureSaver.h"
#include "GlobalSingletonStorage.h"

CTextureSaver::CTextureSaver():
	m_pictureTypeID(TEXTURE_SAVER_FREE_IMAGE)
{
	m_textureSavers[TEXTURE_SAVER_FREE_IMAGE] = MP_NEW(CFreeImageTextureSaver);
	m_textureSavers[TEXTURE_SAVER_NE_COMPRESSED] = MP_NEW(CNECompressedTextureSaver);
}

bool CTextureSaver::SaveTexture(int oglTextureID, int width, int height, bool isTextureTransparent, std::wstring& fileName, SCropRect* const rect)
{
	ChangeTextureType();
	return m_textureSavers[m_pictureTypeID]->SaveTexture(oglTextureID, width, height, isTextureTransparent, fileName, rect);
}

bool CTextureSaver::SaveTexture(int format, int width, int height, int channelsCount, void* dataIn, void** dataOut, unsigned long* dataSize, int quality)
{
	ChangeTextureType(format);
	return m_textureSavers[m_pictureTypeID]->SaveTexture(format, width, height, channelsCount, dataIn, dataOut, dataSize, quality);
}

bool CTextureSaver::StretchTexture(int srcX, int srcY, int srcWidth, int srcHeight, int destWidth, int destHeight
								   , void* dataIn, void** dataOut, unsigned long* dataSize, int quality)
{
	return m_textureSavers[m_pictureTypeID]->StretchTexture(srcX, srcY, srcWidth, srcHeight, destWidth, destHeight, dataIn, dataOut, dataSize, quality);
}

void CTextureSaver::CleanUp()
{
	for (int k = 0; k < TEXTURE_SAVERS_COUNT; k++)
	if (m_textureSavers[k])
	{
		m_textureSavers[k]->CleanUp();
	}
}

void CTextureSaver::ChangeTextureType()
{
	m_pictureTypeID = TEXTURE_SAVER_FREE_IMAGE;
}

void CTextureSaver::ChangeTextureType(int format)
{
	m_pictureTypeID = ((format != IMAGE_FORMAT_NEC) && (format != IMAGE_FORMAT_NEC_NO_MIPMAPS)) ? TEXTURE_SAVER_FREE_IMAGE : TEXTURE_SAVER_NE_COMPRESSED;
}

void CTextureSaver::SetForceGrayscale(bool isGrayscaleForced)
{
	m_textureSavers[TEXTURE_SAVER_FREE_IMAGE]->SetForceGrayscale(isGrayscaleForced);
	m_textureSavers[TEXTURE_SAVER_NE_COMPRESSED]->SetForceGrayscale(isGrayscaleForced);
}

CTextureSaver::~CTextureSaver()
{
	CFreeImageTextureSaver* freeImageTextureSaver = (CFreeImageTextureSaver*)m_textureSavers[TEXTURE_SAVER_FREE_IMAGE];
	MP_DELETE(freeImageTextureSaver);

	CNECompressedTextureSaver* neCompressedTextureSaver = (CNECompressedTextureSaver*)m_textureSavers[TEXTURE_SAVER_NE_COMPRESSED];
	MP_DELETE(neCompressedTextureSaver);
}