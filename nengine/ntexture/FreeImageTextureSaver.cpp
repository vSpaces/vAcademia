
#include "StdAfx.h"
#include <Assert.h>
#include <glew.h>
#include "freeimage.h"
#include "TextureProc.h"
#include "FileFunctions.h"
#include "FreeImageTextureSaver.h"
#include "GlobalSingletonStorage.h"

CFreeImageTextureSaver::CFreeImageTextureSaver():
	m_dib(NULL),
	m_isGrayscaleForced(false)
{
}

void CFreeImageTextureSaver::CleanUp()
{
	if (m_dib)
	{
		FreeImage_Unload((FIBITMAP *)m_dib);
		m_dib = NULL;
	}
}

bool CFreeImageTextureSaver::SaveTexture(int oglTextureID, int width, int height, bool isTransparent, std::wstring& fileName, SCropRect* const rect)
{
	assert(m_dib == NULL);
	assert(width > 0);
	assert(height > 0);

	unsigned char* pictureData = NULL;

	if (oglTextureID != -1)
	{
		bool isTexture = GLFUNCR(glIsTexture(oglTextureID)) > 0;
		if (isTexture == GL_FALSE)
		{
			return false;
		}

		GLFUNC(glBindTexture)(GL_TEXTURE_2D, oglTextureID);
	
		if (isTransparent)
		{
			pictureData = MP_NEW_ARR(unsigned char, 4 * width * height);

			GLint w, h;
			GLFUNC(glGetTexLevelParameteriv)(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
			GLFUNC(glGetTexLevelParameteriv)(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
			if ((w == width) && (h == height))
			{
				GLFUNC(glGetTexImage)(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pictureData);
			}
			else
			{
				return false;
			}

			if (rect != NULL )
			{
				GetNewPictureData( width, height, rect, pictureData, 4);
			}
			
		}
		else
		{
			pictureData = MP_NEW_ARR(unsigned char, 3 * width * height);

			GLint w, h;
			GLFUNC(glGetTexLevelParameteriv)(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
			GLFUNC(glGetTexLevelParameteriv)(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
			if ((w == width) && (h == height))
			{
				GLFUNC(glGetTexImage)(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pictureData);
			}
			else
			{
				return false;
			}


			if (rect != NULL )
			{
			  GetNewPictureData( width, height, rect, pictureData, 3);
			}
		}



		FILE* checkFile = NULL;
		if ((checkFile = FileOpen(fileName, L"r")) != NULL)
		{
			fclose(checkFile);

			if (_wremove(fileName.c_str()) == -1)
			{
				MP_DELETE_ARR(pictureData);
				return false;
			}
		}
		
		unsigned int delta = 3 + (int)isTransparent;
		
		FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

		unsigned int bpp = isTransparent ? 32 : 24;
		unsigned int pitch = isTransparent ? ((((bpp * width) + 31) / 32) * 4): ((((bpp * width) + 23) / 24) * 3);

		m_dib = FreeImage_ConvertFromRawBits((BYTE *)pictureData, width, height, pitch, bpp, 0x00FF0000, 0x0000FF00, 0x000000FF, true);

		if (!m_dib)
		{
			MP_DELETE_ARR(pictureData);
			return false;
		}

		FreeImage_FlipVertical((FIBITMAP *)m_dib);

		fif = FIF_UNKNOWN;
		if ((fif = FreeImage_GetFIFFromFilenameU(fileName.c_str())) == FIF_UNKNOWN)
		{
			MP_DELETE_ARR(pictureData);
			return false;
		}

		if (fif == FIF_JPEG)
		{
			void* convertedDib = FreeImage_ConvertTo24Bits((FIBITMAP *)m_dib);
			if( convertedDib)
			{
				if (m_dib)
				{
					FreeImage_Unload((FIBITMAP *)m_dib);
				}
				m_dib = convertedDib;
			}
			delta = 3;
		}

		unsigned int w = FreeImage_GetWidth((FIBITMAP *)m_dib);
		unsigned int h = FreeImage_GetHeight((FIBITMAP *)m_dib);

		unsigned char* buffer = (unsigned char*)FreeImage_GetBits((FIBITMAP *)m_dib);
		unsigned int offset = 0;
		unsigned char tmp = 0;

		if (!m_isGrayscaleForced)
		{
			for (unsigned int i = 0; i < w * h; i++)
			{
				memcpy(&tmp, &buffer[offset], 1);
				memcpy(&buffer[offset], &buffer[offset + 2], 1);
				memcpy(&buffer[offset + 2], &tmp, 1);		

				offset += delta;
			}
		}
		else
		{
			for (unsigned int i = 0; i < w * h; i++)
			{
				unsigned char avg = (buffer[offset] + buffer[offset + 1] + buffer[offset + 2]) / 3;
				buffer[offset] = avg;
				buffer[offset + 1] = avg;
				buffer[offset + 2] = avg;
				
				offset += delta;
			}
		}
		
		BOOL res = FreeImage_SaveU(fif, (FIBITMAP *)m_dib, fileName.c_str());
		MP_DELETE_ARR(pictureData);
		CleanUp();
		return (res == TRUE);
	}

	CleanUp();

	return false;
}

bool CFreeImageTextureSaver::SaveTexture(int format, int width, int height, int channelsCount,  void* dataIn,
									  void** dataOut, unsigned long* dataSize, int quality)
{
	if (format != IMAGE_FORMAT_JPEG)
	{
		return false;
	}

	unsigned long outDataSize = 0;

	assert(m_dib == NULL);
	assert(width > 0);
	assert(height > 0);

	if (!dataOut)
	{
		return false;
	}

	assert(channelsCount > 2);
	bool isTransparent = (channelsCount == 4);
	
	unsigned bpp = channelsCount == 4 ? 32 : 24;
	unsigned pitch = isTransparent == true ? ((((bpp * width) + 31) / 32) * 4): (((((bpp * width) + 7) / 8) + 3)&0xfffffffc);

	m_dib = FreeImage_ConvertFromRawBits((BYTE *)dataIn, width, height, pitch, bpp, 0x00FF0000, 0x0000FF00, 0x000000FF, true);
	void* convertedDib = FreeImage_ConvertTo24Bits((FIBITMAP *)m_dib);
	if (convertedDib)
	{
		if (m_dib)
		{
			FreeImage_Unload((FIBITMAP *)m_dib);
		}
		m_dib = convertedDib;
	}

	FreeImage_FlipVertical((FIBITMAP *)m_dib);
	FreeImage_SetDotsPerMeterX((FIBITMAP *)m_dib, quality);
	FreeImage_SetDotsPerMeterY((FIBITMAP *)m_dib, quality);

	if (!m_dib)
	{
		return false;
	}

	unsigned int w = FreeImage_GetWidth((FIBITMAP *)m_dib);
	unsigned int h = FreeImage_GetHeight((FIBITMAP *)m_dib);

	unsigned char* buffer = (unsigned char*)FreeImage_GetBits((FIBITMAP *)m_dib);

	unsigned char tmp = 0;
	unsigned char* buffer_pointer = buffer;
	pitch = FreeImage_GetPitch((FIBITMAP *)m_dib);
	for (unsigned int y = 0; y < h; y++)
	{
		for (unsigned int x = 0; x < w ; x++, buffer_pointer+=3)
		{
			tmp = *buffer_pointer;
			buffer_pointer[0] = buffer_pointer[2];
			buffer_pointer[2] = tmp;
		}
		buffer_pointer+=pitch-w*3;
	}

	unsigned long tempBufferSize = FreeImage_GetPitch((FIBITMAP *)m_dib) * height; 
	*dataOut = (void*)MP_NEW_ARR(unsigned char, tempBufferSize);

	FREE_IMAGE_FORMAT fif = FIF_JPEG;

	FreeImageIO io;
	io.read_proc = _ReadProc;
	io.write_proc = _WriteProc;
	io.tell_proc = _TellProc;
	io.seek_proc = _SeekProc;

	SMemFile memFile;
	memFile.buffer = *dataOut;
	memFile.size = tempBufferSize;
	memFile.currentPosition = 0;

	int flags = 0;
	if( quality < 10) 
	{
		flags |= JPEG_QUALITYBAD;
	}
	else	if( quality < 25) 
	{
		flags |= JPEG_QUALITYAVERAGE;
	}
	else	if( quality < 50) 
	{
		flags |= JPEG_QUALITYNORMAL;
	}
	else	if( quality < 75) 
	{
		flags |= JPEG_QUALITYGOOD;
	}
	else
	{
		flags |= JPEG_QUALITYSUPERB;
	}

	FreeImage_SaveToHandle(fif, (FIBITMAP *)m_dib, &io, (fi_handle)&memFile.buffer, flags);

	outDataSize = memFile.currentPosition;

	if( dataSize)
	{
		*dataSize = outDataSize;
	}

	CleanUp();

	return (outDataSize != 0);
}

void CFreeImageTextureSaver::GetNewPictureData(int& width, int& height, SCropRect* const rect, unsigned char*& pictureData, const int size)
{
	unsigned char* newPictureData = MP_NEW_ARR(unsigned char, size * rect->width * rect->height);
	int j=0;

	if (rect->y != 0)
	{
		j = size * rect->y * width;
	}

	for (int i=0; (i < size * rect->width * rect->height) && (j < width * height * size); i += size * rect->width)
	{
		j += size * rect->x;

		memcpy(newPictureData + i ,pictureData + j, size * rect->width);

		j += (width - rect->x) * size;

	}
	memcpy(pictureData, newPictureData , size * rect->width * rect->height);

	MP_DELETE_ARR(newPictureData);

	width = rect->width;
	height = rect->height;
}

bool CFreeImageTextureSaver::StretchTexture(int srcX, int srcY, int srcWidth, int srcHeight, int destWidth, int destHeight
					, void* dataIn, void** dataOut, unsigned long* dataSize, int quality)
{
	unsigned long outDataSize = 0;

	FreeImageIO io;
	io.read_proc = _ReadProc;
	io.write_proc = NULL;
	io.tell_proc = _TellProc;
	io.seek_proc = _SeekProc;

	SMemFile memFile;
	memFile.buffer = dataIn;
	memFile.size = *dataSize;
	memFile.currentPosition = 0;

	m_dib = FreeImage_LoadFromHandle(FIF_JPEG, &io, (fi_handle)&memFile);

	int w = FreeImage_GetWidth((FIBITMAP *)m_dib);
	int h = FreeImage_GetHeight((FIBITMAP *)m_dib);

	if (srcX >= w || srcY >= h || srcWidth > w || srcHeight > h)
		return false;
	if (srcWidth+srcX > w || srcHeight+srcY > h)
		return false;
	
	void* convertedDib = FreeImage_Copy((FIBITMAP *)m_dib, srcX, srcY, srcWidth, srcHeight);
	if (convertedDib)
	{
		if (m_dib)
		{
			FreeImage_Unload((FIBITMAP *)m_dib);
		}
		m_dib = convertedDib;
	}

	if (!m_dib)
	{
		return false;
	}

	convertedDib = FreeImage_Rescale((FIBITMAP *)m_dib, destWidth, destHeight, FILTER_BILINEAR);
	if (convertedDib)
	{
		if (m_dib)
		{
			FreeImage_Unload((FIBITMAP *)m_dib);
		}
		m_dib = convertedDib;
	}

	if (!m_dib)
	{
		return false;
	}

	//saving
	w = FreeImage_GetWidth((FIBITMAP *)m_dib);
	h = FreeImage_GetHeight((FIBITMAP *)m_dib);

	unsigned long tempBufferSize = FreeImage_GetPitch((FIBITMAP *)m_dib) * h; 
	*dataOut = (void*)MP_NEW_ARR(unsigned char, tempBufferSize);

	FREE_IMAGE_FORMAT fif = FIF_JPEG;

	io.read_proc = _ReadProc;
	io.write_proc = _WriteProc;
	io.tell_proc = _TellProc;
	io.seek_proc = _SeekProc;

	memFile.buffer = *dataOut;
	memFile.size = tempBufferSize;
	memFile.currentPosition = 0;

	int flags = 0;
	if( quality < 10) 
	{
		flags |= JPEG_QUALITYBAD;
	}
	else	if( quality < 25) 
	{
		flags |= JPEG_QUALITYAVERAGE;
	}
	else	if( quality < 50) 
	{
		flags |= JPEG_QUALITYNORMAL;
	}
	else	if( quality < 75) 
	{
		flags |= JPEG_QUALITYGOOD;
	}
	else
	{
		flags |= JPEG_QUALITYSUPERB;
	}

	FreeImage_SaveToHandle(fif, (FIBITMAP *)m_dib, &io, (fi_handle)&memFile.buffer, flags);

	outDataSize = memFile.currentPosition;

	if( dataSize)
	{
		*dataSize = outDataSize;
	}

	CleanUp();

	return (outDataSize != 0);
}

void CFreeImageTextureSaver::SetForceGrayscale(bool isGrayscaleForced)
{
	m_isGrayscaleForced = isGrayscaleForced;
}

CFreeImageTextureSaver::~CFreeImageTextureSaver()
{
	CleanUp();
}