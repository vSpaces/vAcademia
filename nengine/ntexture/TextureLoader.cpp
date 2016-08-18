
#include "StdAfx.h"
#include <glew.h>
#include <assert.h>
#include "PBO.h"
#include "TextureLoader.h"
#include "HelperFunctions.h"
#include "FreeImageTextureLoader.h"
#include "NECompressedTextureLoader.h"
#include <windows.h>
#include "TimeProcessor.h"
#include "InternalLogWriter.h"
#include "GlobalSingletonStorage.h"

CTimeProcessor* tp = NULL;
CInternalLogWriter* lw = NULL;

CTextureLoader::CTextureLoader():
	m_pictureTypeID(TEXTURE_LOADER_FREEIMAGE),
	m_maxTextureSize(-1)
{
	m_textureLoaders[TEXTURE_LOADER_FREEIMAGE] = MP_NEW(CFreeImageTextureLoader);
	m_textureLoaders[TEXTURE_LOADER_NE_COMPRESSED] = MP_NEW(CNECompressedTextureLoader);
}


unsigned int CTextureLoader::GetMaxTextureSize()
{
	if (-1 == m_maxTextureSize)
	{
		GLint texSize; 
		GLFUNC(glGetIntegerv)(GL_MAX_TEXTURE_SIZE, &texSize);
		m_maxTextureSize = texSize;
	}

	return m_maxTextureSize;
}

void CTextureLoader::CleanUp()
{
	for (int k = 0; k < TEXTURE_LOADERS_COUNT; k++)
	if (m_textureLoaders[k])
	{
		m_textureLoaders[k]->CleanUp();
	}
}

bool CTextureLoader::ToPOT()
{
	if (((GetWidth() & (GetWidth() - 1)) == 0) && ((GetHeight() & (GetHeight() - 1)) != 0))
	{
		if (GetWidth() * 6 == GetHeight())
		{
			return false;
		}
	}

	return m_textureLoaders[m_pictureTypeID]->ToPOT();
}

bool CTextureLoader::LoadFromMemory(int format, void* pictureData, int width, int height, bool isTransparent)
{
	ChangeTextureType(format);
	return m_textureLoaders[m_pictureTypeID]->LoadFromMemory(pictureData, width, height, isTransparent);
}

bool CTextureLoader::LoadTexture(std::wstring& fileName)
{
	ChangeTextureType();
	return m_textureLoaders[m_pictureTypeID]->LoadTexture(fileName);
}

bool CTextureLoader::LoadTexture(std::wstring& fileName, const void* buffer, int size)
{
	ChangeTextureType();
	return m_textureLoaders[m_pictureTypeID]->LoadTexture(fileName, buffer, size);
}

bool CTextureLoader::LoadTexture(std::string& fileName, const void* buffer, int size)
{
	USES_CONVERSION;
	std::wstring fileNameW = A2W( fileName.c_str());
	return LoadTexture( fileNameW, buffer, size);
}

void CTextureLoader::EnlargeTexture()
{
	m_textureLoaders[m_pictureTypeID]->EnlargeTexture();
}

void CTextureLoader::Crop(unsigned int width, unsigned int height)
{
	m_textureLoaders[m_pictureTypeID]->Crop(width, height);
}

void CTextureLoader::Resize(int width, int height)
{
	m_textureLoaders[m_pictureTypeID]->Resize(width, height);
}

unsigned char CTextureLoader::GetChannelCount()const
{
	return m_textureLoaders[m_pictureTypeID]->GetChannelCount();
}

unsigned int CTextureLoader::GetWidth()const
{
	return m_textureLoaders[m_pictureTypeID]->GetWidth();
}

unsigned int CTextureLoader::GetHeight()const
{
	return m_textureLoaders[m_pictureTypeID]->GetHeight();
}

bool CTextureLoader::ApplyKeyColor(unsigned char keyR, unsigned char keyG, unsigned char keyB)
{
	return m_textureLoaders[m_pictureTypeID]->ApplyKeyColor(keyR, keyG, keyB);
}

bool CTextureLoader::ApplyKeyColorFromImage()
{
	return m_textureLoaders[m_pictureTypeID]->ApplyKeyColorFromImage();
}

bool CTextureLoader::ApplyDefaultAlpha(unsigned char alpha)
{
	return m_textureLoaders[m_pictureTypeID]->ApplyDefaultAlpha(alpha);
}

void* CTextureLoader::GetData(unsigned int frameID)const
{
	return m_textureLoaders[m_pictureTypeID]->GetData(frameID);
}

unsigned int CTextureLoader::GetDataSize(unsigned int frameID)const
{	
	return m_textureLoaders[m_pictureTypeID]->GetDataSize(frameID);
}

unsigned int CTextureLoader::GetUserData(unsigned int frameID)const
{
	return m_textureLoaders[m_pictureTypeID]->GetUserData(frameID);
}

bool CTextureLoader::IsDirectChannelOrder()const
{
	return m_textureLoaders[m_pictureTypeID]->IsDirectChannelOrder();
}

bool CTextureLoader::IsInverseChannelOrder()const
{
	return !IsDirectChannelOrder();
}

void CTextureLoader::ChangeTextureType()
{
	m_pictureTypeID = TEXTURE_LOADER_FREEIMAGE;
}

void CTextureLoader::ChangeTextureType(int format)
{
	m_pictureTypeID = ((format != IMAGE_FORMAT_NEC) && (format != IMAGE_FORMAT_NEC_NO_MIPMAPS)) ? TEXTURE_LOADER_FREEIMAGE : TEXTURE_LOADER_NE_COMPRESSED;
}

int CTextureLoader::GetFrameCount()const
{
	return m_textureLoaders[m_pictureTypeID]->GetFrameCount();
}

int CTextureLoader::GetSourceFormat()
{
	if (GetChannelCount() == 3)
	{
		if (IsDirectChannelOrder())
		{
			return GL_RGB;
		}
		else
		{
			return GL_BGR_EXT;
		}
	}
	else
	{
		if (IsDirectChannelOrder())
		{
			return GL_RGBA;
		}
		else
		{
			return GL_BGRA_EXT;
		}
	}
}

int CTextureLoader::GetNormalFormat()
{
	if (GetChannelCount() == 3)
	{
		return GL_RGB;
	}
	else
	{
		return GL_RGBA;
	}
}

int CTextureLoader::GetCompressedFormat()
{
	if (GetChannelCount() == 3)
	{
		return GL_COMPRESSED_RGB;
	}
	else
	{
		return GL_COMPRESSED_RGBA;
	}
}

void CTextureLoader::LoadToOpenGL(int oglTextureID, CTextureLoadingInfo* textureInfo, int& minFilter, int& magFilter)
{
	if (!textureInfo)
	{
		return;
	}

	bool isMipMap = textureInfo->IsMipMap();
	bool isLinearFiltering = (textureInfo->GetFilteringMode() != FILTERING_MODE_PIXEL);
	unsigned int width = GetWidth();
	unsigned int height = GetHeight();
	bool canBeCompressed = textureInfo->CanBeCompressed(GetChannelCount(), (unsigned char)GetFrameCount());

	bool isNeedToResize = false;
	unsigned int maxTextureSize = GetMaxTextureSize();
	while ((width > maxTextureSize) || (height > maxTextureSize))
	{
		width = (int)(width / 2);
		height = (int)(height / 2);
		isNeedToResize = true;
	}

	if (isNeedToResize)
	{
		Resize(width, height);
	}
	
	if (TEXTURE_LOADER_NE_COMPRESSED == m_pictureTypeID)
	{
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, oglTextureID);
		GLFUNC(glPixelStorei)(GL_UNPACK_ALIGNMENT, 4);

		if (textureInfo->IsMarkNonPOT())
		if ((!IsPowerOf2(GetWidth())) || (!IsPowerOf2(GetHeight())))
		{
			unsigned char* errorRedTexture = MP_NEW_ARR(unsigned char, 4 * 4 * 4);
			memset(errorRedTexture, 0xFFFF00FF, 4 * 4 * 4);
            
			GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			minFilter = GL_NEAREST;
			magFilter = GL_NEAREST;
			GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, errorRedTexture);

			MP_DELETE_ARR(errorRedTexture);

			return;
		}		

		// проверка по сути на поддержку экстеншена
		if (textureInfo->CanUseHardwareMipmaps(GetChannelCount()))
		{
			GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_FALSE);
		}

		GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		minFilter = GL_NEAREST;
		magFilter = GL_NEAREST;
	
		int allSize = 0;
		int i = 0;
		for (i = 0; i < GetFrameCount(); i++)
		{
			allSize += GetDataSize(i);
		}

		CPBO* pbo = NULL;
	
		int offset = 0;		

		for (i = 0; i < GetFrameCount(); i++)
		{
			if (pbo)
			{	
				pbo->Capture();
				GLFUNC(glCompressedTexImage2D)(GL_TEXTURE_2D, i, GetUserData(i), width, height, 0, GetDataSize(i), (void*)offset);
				offset += GetDataSize(i);
				pbo->Uncapture();			
			}
			else
			{
				GLFUNC(glCompressedTexImage2D)(GL_TEXTURE_2D, i, GetUserData(i), width, height, 0, GetDataSize(i), GetData(i));
			}

			width = width / 2;
			height = height / 2;

			if (width == 0)
			{
				width = 1;
			}
			if (height == 0)
			{
				height = 1;
			}
		}

		if (pbo)
		{
			MP_DELETE(pbo);
		}

		if (textureInfo->GetFilteringMode() == FILTERING_MODE_LINEAR)
		{
			minFilter = GetMipmapFiltration(textureInfo);
			magFilter = GL_LINEAR;
			GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);

		}
		else if (textureInfo->GetFilteringMode() == FILTERING_MODE_LINEAR_NO_MIPMAP)
		{
			magFilter = GL_LINEAR;
			minFilter = GL_LINEAR;
			GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}			
	}
	else if (GetFrameCount() != 6)
	{
		if (textureInfo->IsMarkNonPOT())
		if ((!IsPowerOf2(GetWidth())) || (!IsPowerOf2(GetHeight())))
		{
			unsigned char* errorRedTexture = MP_NEW_ARR(unsigned char, 4 * 4 * 4);
			memset(errorRedTexture, 0xFFFF00FF, 4 * 4 * 4);
        
			GLFUNC(glPixelStorei)(GL_UNPACK_ALIGNMENT, 4);
			GLFUNC(glBindTexture)(GL_TEXTURE_2D, oglTextureID);
			magFilter = GL_NEAREST;
			minFilter = GL_NEAREST;
			GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, errorRedTexture);

			MP_DELETE_ARR(errorRedTexture);

			return;
		}

		void* ptr = GetData(0);

		GLFUNC(glPixelStorei)(GL_UNPACK_ALIGNMENT, 4);
		GLFUNC(glBindTexture)(GL_TEXTURE_2D, oglTextureID);
		if (textureInfo->GetFilteringMode() == FILTERING_MODE_PIXEL)
		{
			magFilter = GL_NEAREST;
			GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else
		{
			magFilter = GL_LINEAR;
			GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		bool canUseHardwareMipmaps = textureInfo->CanUseHardwareMipmaps(GetChannelCount());

		if (!isMipMap)
		{
			if (canUseHardwareMipmaps)
			{
				GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_FALSE);
			}

			if (!isLinearFiltering)
			{
				minFilter = GL_NEAREST;
				GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			}
			else
			{
				minFilter = GL_LINEAR;
				GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}

			if (canBeCompressed)
			{
				GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, GetCompressedFormat(), width, height, 0, GetSourceFormat(), GL_UNSIGNED_BYTE, ptr);
			}
			else
			{
				GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, GetNormalFormat(), width, height, 0, GetSourceFormat(), GL_UNSIGNED_BYTE, ptr);
			}
		}
		else	
		{
			if (!isLinearFiltering)
			{
				minFilter = GL_NEAREST;
				GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			}
			else
			{
				minFilter = GetMipmapFiltration(textureInfo);
				GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
			}

			if (canUseHardwareMipmaps)
			{
				GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, canUseHardwareMipmaps ? GL_TRUE : GL_FALSE);
			}

			if (canBeCompressed)
			{
				GLFUNC(gluBuild2DMipmaps)(GL_TEXTURE_2D, GetCompressedFormat(), width, height, GetSourceFormat(), GL_UNSIGNED_BYTE, ptr);		
			}
			else
			{
				if (canUseHardwareMipmaps)
				{ 
					GLFUNC(glTexImage2D)(GL_TEXTURE_2D, 0, GetNormalFormat(), width, height, 0, GetSourceFormat(), GL_UNSIGNED_BYTE, ptr);
				}
				else
				{
					GLFUNC(gluBuild2DMipmaps)(GL_TEXTURE_2D, GetNormalFormat(), width, height, GetSourceFormat(), GL_UNSIGNED_BYTE, ptr);	
				}
			}
		}
	}
	else
	{
		GLFUNC(glEnable)(GL_TEXTURE_CUBE_MAP_ARB);

		minFilter = GetMipmapFiltration(textureInfo);
		GLFUNC(glTexParameteri)(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, minFilter);
		magFilter = GL_LINEAR;
		GLFUNC(glTexParameteri)(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		GLFUNC(glBindTexture)(GL_TEXTURE_CUBE_MAP_ARB, oglTextureID);
		GLFUNC(glPixelStorei)(GL_UNPACK_ALIGNMENT, 1);

		GLFUNC(glTexParameterf)(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		GLFUNC(glTexParameterf)(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		GLFUNC(glTexParameteri)(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		for (int part = 0; part < 6; part++)
		{
			int target = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + part;
			void* ptr = GetData(part);
			if (!canBeCompressed)
			{
				GLFUNC(gluBuild2DMipmaps)(target, GetNormalFormat(), width, height, GetSourceFormat(), GL_UNSIGNED_BYTE, ptr);		
			}
			else
			{
				GLFUNC(glTexImage2D)(target, 0, GL_COMPRESSED_RGB_ARB, width, height, 0, GetSourceFormat(), GL_UNSIGNED_BYTE, ptr);
				GLFUNC(glGenerateMipmapEXT)(GL_TEXTURE_CUBE_MAP);	
			}
		}

		GLFUNC(glBindTexture)(GL_TEXTURE_CUBE_MAP_ARB, 0);

		GLFUNC(glDisable)(GL_TEXTURE_CUBE_MAP_ARB);
	}
}

unsigned char CTextureLoader::GetUsageType()const
{
	return m_textureLoaders[m_pictureTypeID]->GetUsageType();
}

void CTextureLoader::FillMask(unsigned char* mask, unsigned char* dataPtr, unsigned char* dataEnd)
{
	unsigned char bitSetArr[8] = {1, 2, 4, 8, 16, 32, 64, 128};
	unsigned char* pBit = &bitSetArr[0];

	unsigned char* maskPos = mask;
	*maskPos = 0;

	int k = 0;
	while (dataPtr < dataEnd)
	{
		k++;
	
		if (*dataPtr > 0)
		{
			*maskPos = *maskPos + *pBit;
		}

		if (*pBit == 128)
		{
			maskPos++;
			*maskPos = 0;
			pBit = &bitSetArr[0];
		}
		else
		{
			pBit++;
		}

		dataPtr += 4;
	}
}

unsigned char* CTextureLoader::AllocateMask(unsigned int& maskSize)
{
	int width = GetWidth();
	int height = GetHeight();

	unsigned char* mask = NULL;
	if ((width * height) % 8 > 0)
	{
		maskSize = (width * height) / 8 + 2;
	}
	else
	{
		maskSize = (width * height) / 8 + 1;
	}

	mask = MP_NEW_ARR(unsigned char, maskSize);

	return mask;
}

bool CTextureLoader::GetMask(unsigned char** mask, unsigned int& maskSize)
{
	if (GetChannelCount() != 4)
	{
		return false;
	}

	*mask = AllocateMask(maskSize);

	int width = GetWidth();
	int height = GetHeight();

	unsigned char* dataPtr = (unsigned char *)GetData(0);
	unsigned char* dataEnd = dataPtr + 4 * width * height;		

	if (!IsDirectChannelOrder())
	{
		dataPtr += 3;		
	}

	FillMask(*mask, dataPtr, dataEnd);

	return true;
}

unsigned int CTextureLoader::GetMipmapFiltration(CTextureLoadingInfo* textureInfo)
{
	if (textureInfo->IsTrilinearFilteringPermitted())
	{
		return GL_LINEAR_MIPMAP_LINEAR;
	}
	else
	{
		return GL_LINEAR_MIPMAP_NEAREST;
	}
}

CTextureLoader::~CTextureLoader()
{
	CFreeImageTextureLoader* freeImageTextureLoader = (CFreeImageTextureLoader*)m_textureLoaders[TEXTURE_LOADER_FREEIMAGE];
	MP_DELETE(freeImageTextureLoader);

	CNECompressedTextureLoader* neCompressedTextureLoader = (CNECompressedTextureLoader*)m_textureLoaders[TEXTURE_LOADER_NE_COMPRESSED];
	MP_DELETE(neCompressedTextureLoader);
}