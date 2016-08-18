
#include "StdAfx.h"
#include <glew.h>
#include "PBO.h"
#include "NECompressedTextureSaver.h"
#include "PBOInfo.h"
#include "HelperFunctions.h"
#include "GlobalSingletonStorage.h"

CNECompressedTextureSaver::CNECompressedTextureSaver()
{
}

bool CNECompressedTextureSaver::SaveTexture(int /*oglTextureID*/, int /*width*/, int /*height*/, bool /*isTextureTransparent*/, std::wstring& /*fileName*/, SCropRect* const /*rect*/)
{
	return false;
}

bool CNECompressedTextureSaver::StretchTexture(int /*srcX*/, int /*srcY*/, int /*srcWidth*/, int /*srcHeight*/, 
											   int /*destWidth*/, int /*destHeight*/, void* /*dataIn*/, 
											   void** /*dataOut*/, unsigned long* /*dataSize*/, int /*quality*/)
{
	return false;
}

bool CNECompressedTextureSaver::SaveTexture(int format, int width, int height, int channelsCount, void* /*dataIn*/, void** dataOut, unsigned long* dataSize, int quality)
{
	int necFormat = format;

	if ((!IsPowerOf2(width)) || (!IsPowerOf2(height)))
	{
		return false;
	}

	bool isUsePBO = (quality == 0);
	SPBOInfo* pboInfo = NULL;
		
	int allSize = 0;
	GLint size = -1;
	int level = 0;

	while (size != 0)
	{
		size = 0;
		GLFUNC(glGetTexLevelParameteriv)(GL_TEXTURE_2D, level, GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB, &size);
		if (level == 0)
		if (glGetError() != 0)
		{
			break;
		}
		
		if (0 == size)
		{
			break;
		}

		allSize += size;
		level++;

		if ((level == 1) && (necFormat == IMAGE_FORMAT_NEC_NO_MIPMAPS))
		{
			break;
		}
	}

	if (isUsePBO)
	{
		pboInfo = MP_NEW(SPBOInfo);
		MP_NEW_P5(_pbo, CPBO, allSize, 1, 1, true, PBO_USAGE_READ_TEXTURE);
		pboInfo->pbo = _pbo;
	}

	int offset = 0;

	int levelCount = level;
	if (allSize != 0)
	{
		level = 0;
		size = -1;

		allSize += levelCount * 8 + 8;

		void* data = NULL;

		GLint width, height;
		GLFUNC(glGetTexLevelParameteriv)(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
		GLFUNC(glGetTexLevelParameteriv)(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

		if (!isUsePBO)
		{
			*dataOut = MP_NEW_ARR(unsigned char, allSize);
			data = *dataOut;
			*dataSize = allSize;
		}
		else
		{
			pboInfo->allSize = allSize;
			pboInfo->mipmapCount = levelCount;
			pboInfo->blocksSize = MP_NEW_ARR(int, levelCount);
			pboInfo->mipmapsSize = MP_NEW_ARR(int, levelCount);
			pboInfo->blocks = MP_NEW_ARR(unsigned char*, levelCount);
			for (int k = 0; k < levelCount; k++)
			{
				pboInfo->blocksSize[k] = 8 + (int)(k == 0) * 8;
				pboInfo->blocks[k] = MP_NEW_ARR(unsigned char, pboInfo->blocksSize[k]);
			}

			data = pboInfo->blocks[0];
		}

		*(unsigned short*)data = (unsigned short)levelCount;
		data = ((unsigned short*)data + 1);
		*(unsigned short*)data = (channelsCount == 4);
		data = ((unsigned short*)data + 1);
		*(unsigned short*)data = (unsigned short)width;
		data = ((unsigned short*)data + 1);
		*(unsigned short*)data = (unsigned short)height;
		data = ((unsigned short*)data + 1);
			
		while (size != 0)
		{
			size = 0;
			GLFUNC(glGetTexLevelParameteriv)(GL_TEXTURE_2D, level, GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB, &size);

			if (0 == size)
			{
				break;
			}

			if (isUsePBO)
			{
				data = pboInfo->blocks[level];
				if (level == 0)
				{
					data = ((unsigned int*)data + 2);
				}
			}

			*(unsigned int*)data = size;
			data = ((unsigned int*)data + 1);
			
			GLint format;
			GLFUNC(glGetTexLevelParameteriv)(GL_TEXTURE_2D, level, GL_TEXTURE_INTERNAL_FORMAT, &format);

			*(unsigned int*)data = format;

			if (!isUsePBO)
			{
				data = ((unsigned int*)data + 1);

				GLFUNC(glGetCompressedTexImageARB)(GL_TEXTURE_2D, level, data);

				data = ((unsigned char*)data + size);
			}
			else
			{
				pboInfo->mipmapsSize[level] = size;
				pboInfo->pbo->Capture(level, offset);				
			}

			offset += size;
				
			level++;

			if ((level == 1) && (necFormat == IMAGE_FORMAT_NEC_NO_MIPMAPS))
			{
				break;
			}
		}

		if (isUsePBO)
		{
			*dataOut = pboInfo;
		}

		return true;
	}
	else
	{
		return false;
	}
}



void CNECompressedTextureSaver::CleanUp()
{
}

void CNECompressedTextureSaver::SetForceGrayscale(bool /*isGrayscaleForced*/)
{
}

CNECompressedTextureSaver::~CNECompressedTextureSaver()
{
	CleanUp();
}