
#include "StdAfx.h"
#include <assert.h>
#include "FreeImageTextureLoader.h"
#include "HelperFunctions.h"
#include "TextureProc.h"

CFreeImageTextureLoader::CFreeImageTextureLoader():
	m_dib(NULL),
	m_frameCount(1),
	reverseRGB(FALSE),
	m_usageType(USAGE_TYPE_DIFFUSE_COMPRESSED)
{
}

unsigned char CFreeImageTextureLoader::GetFrameCount()
{
	return m_frameCount;
}

bool CFreeImageTextureLoader::ToPOT()
{
	if (((GetWidth() & (GetWidth() - 1)) == 0) && ((GetHeight() & (GetHeight() - 1)) == 0) && (GetWidth() > 1) && (GetHeight() > 1))
	{
		return false;
	}

	if (m_dib == NULL)
	{
		return false;
	}

	unsigned int height = 2;
	while (height * 2 <= GetHeight())
	{
		height *= 2;
	}

	unsigned int width = 2;
	while (width * 2 <= GetWidth())
	{
		width *= 2;

	}

	FIBITMAP* partImage = (FIBITMAP *)m_dib;
	m_dib = FreeImage_Rescale((FIBITMAP *)m_dib, width, height, FILTER_BILINEAR);
	FreeImage_Unload(partImage);
	
	return true;
}

bool CFreeImageTextureLoader::LoadFromMemory(void* pictureData, int width, int height, bool isTransparent)
{
	CleanUp();

	unsigned bpp = isTransparent == true ? 32 : 24;
	unsigned pitch = isTransparent == true ? ((((bpp * width) + 31) / 32) * 4): ((((bpp * width) + 23) / 24) * 3);
	m_dib = FreeImage_ConvertFromRawBits((BYTE *)pictureData, width, height, pitch, bpp, 0x00FF0000, 0x0000FF00, 0x000000FF, true);

	if (!m_dib)
	{
		return false;
	}

	OnLoaded(false);
	reverseRGB = true;
	return true;
}

bool CFreeImageTextureLoader::LoadTexture(std::wstring& fileName)
{
	bool isBmp = false;

	CleanUp();

	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	fif = FreeImage_GetFileTypeU(fileName.c_str(), 0);

	if (fif == FIF_UNKNOWN) 
	{
		fif = FreeImage_GetFIFFromFilenameU(fileName.c_str());
	}

	if (fif == FIF_UNKNOWN)
	{
		return false;
	}

	if ((fif == FIF_JPEG) || (fif == FIF_PNG))
	{
		/*FILE* fl = _wfopen(fileName.c_str(), L"rb");
		if (!fl)
		{
			return false;
		}

		fseek(fl, (fif == FIF_JPEG) ? 6 : 1, 0);
		fread(&m_usageType, 1, 1, fl);
		fclose(fl);*/

		//m_usageType = DetectUsageType(m_usageType);
	}

	if (FreeImage_FIFSupportsReading(fif))
	{
		m_dib = FreeImage_LoadU(fif, fileName.c_str());
	}

	if (!m_dib)
	{
		return false;
	}

	if (fif == FIF_BMP)
		isBmp = true;

	OnLoaded(isBmp);

	return true;
}

bool CFreeImageTextureLoader::LoadTexture(std::wstring& fileName, const void* buffer, int size)
{
	bool isBmp = false;

	CleanUp();

	FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeU(fileName.c_str(), 0);

	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilenameU(fileName.c_str());

	if (fif == FIF_UNKNOWN) // библиотека как то косячно определяет формат
	{
		int pointPos = fileName.rfind('.');
		std::wstring extension = fileName.substr(pointPos, fileName.length() - pointPos);
		if (extension == L".gif")
			fif = FIF_GIF;
	}

	if (fif == FIF_UNKNOWN)
	{
		return false;
	}

	if ((fif == FIF_JPEG) || (fif == FIF_PNG))
	{
		m_usageType = ((unsigned char*)buffer)[(fif == FIF_JPEG) ? 6 : 1];
		m_usageType = DetectUsageType(m_usageType);
	}

	if (fif == FIF_GIF)
	{
		FIMEMORY * memory = FreeImage_OpenMemory((BYTE *)buffer, size);
        FIMULTIBITMAP * fiMBM = FreeImage_LoadMultiBitmapFromMemory(fif, memory);
		FIBITMAP * page = FreeImage_LockPage(fiMBM, 0);
		m_dib = FreeImage_Clone(page);
		FreeImage_UnlockPage(fiMBM, page, FALSE);
		FreeImage_CloseMultiBitmap(fiMBM);
		FreeImage_CloseMemory(memory);
	}
	else
	{
		FreeImageIO io;
		io.read_proc = _ReadProc;
		io.write_proc = NULL;
		io.tell_proc = _TellProc;
		io.seek_proc = _SeekProc;

		SMemFile memFile;
		memFile.buffer = buffer;
		memFile.size = size;
		memFile.currentPosition = 0;

		m_dib = FreeImage_LoadFromHandle(fif, &io, (fi_handle)&memFile);

		if (fif == FIF_BMP)
			isBmp = true;
	}

	if (!m_dib)
	{
		return false;
	}

	OnLoaded(isBmp);

	return true;
}

void CFreeImageTextureLoader::EnlargeTexture()
{
	if ((GetWidth() != 1) && (GetHeight() != 1))
	if (((GetWidth() & (GetWidth() - 1)) == 0) && ((GetHeight() & (GetHeight() - 1)) == 0))
	{
		return;
	}

	unsigned int height = 2;
	while (height < GetHeight())
	{
		height *= 2;
	}

	unsigned int width = 2;
	while (width < GetWidth())
	{
		width *= 2;
	}

	if (m_dib == NULL)
	{
		return;
	}

	unsigned char* buffer = (unsigned char*)GetData(0);
	unsigned int bytePerPixel = 0;

	if (GetChannelCount() == 4)
		bytePerPixel = 4;
	else
		bytePerPixel = 3;

	assert(bytePerPixel != 0);

	unsigned int h = GetHeight();
	unsigned int w = GetWidth();

	unsigned bpp = FreeImage_GetBPP((FIBITMAP *)m_dib);
	if (bpp < 24)
		assert(false);

	FIBITMAP * partImage = (FIBITMAP *)m_dib;
	bool rgb = IsDirectChannelOrder();
	m_dib = FreeImage_AllocateT( FIT_BITMAP, width, height, bpp, 0, 0, 0);
	if (m_dib == NULL)
	{
		m_dib = partImage;
		return;
	}

	unsigned char* newBuffer = (unsigned char*)GetData(0);
	if (newBuffer == NULL)
	{
		FreeImage_Unload((FIBITMAP *)m_dib);
		m_dib = partImage;
		return;
	}

	unsigned int newW = GetWidth();

	int from = 0;
	int to = 0;
	w = w * bytePerPixel;
	while (w%4 != 0)
	{
		w++;
	}
	newW = newW * bytePerPixel;

	for (unsigned int i = 0; i < h; i++)
	{
		memcpy(&newBuffer[to], &buffer[from], w);
		from += w;
		to += newW;
	}

	if (rgb != IsDirectChannelOrder())
		++reverseRGB;

	FreeImage_Unload(partImage);
}

void CFreeImageTextureLoader::Crop(unsigned int width, unsigned int height)
{
	if ((width != GetWidth()) && (height != GetHeight()))
	{
		if (m_dib != NULL)
		{
			FIBITMAP * partImage = (FIBITMAP *)m_dib;
			m_dib = FreeImage_Copy((FIBITMAP *)m_dib, 0, 0, width, height);
			FreeImage_Unload(partImage);
		}
	}
}

void CFreeImageTextureLoader::Resize(int width, int height)
{
	if (m_dib != NULL)
	{
		FIBITMAP * partImage = (FIBITMAP *)m_dib;
		m_dib = FreeImage_Rescale((FIBITMAP *)m_dib, width, height, FILTER_BILINEAR);
		FreeImage_Unload(partImage);
	}
}

void CFreeImageTextureLoader::OnLoaded(bool isBmp)
{
	int bpp = FreeImage_GetBPP((FIBITMAP *)m_dib);
	void* convertedDib = NULL;
	if ((bpp == 8 && !FreeImage_GetTransparencyCount((FIBITMAP *)m_dib)) || (bpp == 16))
	{
		convertedDib = FreeImage_ConvertTo24Bits((FIBITMAP *)m_dib);
	}
	else if ((bpp != 24) && (bpp != 32))
	{
		convertedDib = FreeImage_ConvertTo32Bits((FIBITMAP *)m_dib);
	}

	if (isBmp && bpp == 32)
	{
		convertedDib = FreeImage_ConvertTo24Bits((FIBITMAP *)m_dib);
	}

	if( convertedDib)
	{
		if (m_dib)
			FreeImage_Unload((FIBITMAP *)m_dib);
		m_dib = convertedDib;
	}

	FreeImage_FlipVertical((FIBITMAP *)m_dib);

	m_frameCount = 1;
	if (GetWidth() * 6 == GetHeight()) 
	{
		m_frameCount = 6;
	}

	reverseRGB = false;
}

void CFreeImageTextureLoader::CleanUp()
{
	if (m_dib)
	{
		FreeImage_Unload((FIBITMAP *)m_dib);
		m_dib = NULL;
	}
}

unsigned char CFreeImageTextureLoader::GetChannelCount()const
{
	if (!m_dib)
	{
		return 0;
	}
	
	switch (FreeImage_GetBPP((FIBITMAP *)m_dib))
	{
	case 32:
		return 4;

	case 24:
		return 3;

	case 16:
		return 3;

	case 8:
		return 1;
	}

	return 0;
}

unsigned int CFreeImageTextureLoader::GetWidth()const
{
	if (!m_dib)
	{
		return 0;
	}

	return FreeImage_GetWidth((FIBITMAP *)m_dib);
}

unsigned int CFreeImageTextureLoader::GetHeight()const
{
	if (!m_dib)
	{
		return 0;
	}

	return FreeImage_GetHeight((FIBITMAP *)m_dib) / m_frameCount;
}

bool CFreeImageTextureLoader::ApplyKeyColor(unsigned char keyR, unsigned char keyG, unsigned char keyB)
{
	if (m_dib)
	{
		int bpp = FreeImage_GetBPP((FIBITMAP *)m_dib);
		void* convertedDib = NULL;
		if (bpp != 32)
		{
			convertedDib = FreeImage_ConvertTo32Bits((FIBITMAP *)m_dib);
		}

		if( convertedDib)
		{
			if (m_dib)
				FreeImage_Unload((FIBITMAP *)m_dib);
			m_dib = convertedDib;
		}

		int bytespp = FreeImage_GetLine((FIBITMAP *)m_dib) / FreeImage_GetWidth((FIBITMAP *)m_dib);
		unsigned int height = FreeImage_GetHeight((FIBITMAP *)m_dib);
		unsigned int width = FreeImage_GetWidth((FIBITMAP *)m_dib);
		for(unsigned int y = 0; y < height; y++) 
		{
			BYTE *bits = FreeImage_GetScanLine((FIBITMAP *)m_dib, y);
			for(unsigned x = 0; x < width; x++) 
			{
				if ((bits[FI_RGBA_RED] == keyR) && (bits[FI_RGBA_GREEN] == keyG) && (bits[FI_RGBA_BLUE] == keyB))
				{
					bits[FI_RGBA_ALPHA] = 0;
				}
				bits += bytespp;
			}
		}

		return true;
	}

	return false;
}

bool CFreeImageTextureLoader::ApplyKeyColorFromImage()
{
	if (m_dib)
	{
		int bpp = FreeImage_GetBPP((FIBITMAP *)m_dib);
		void* convertedDib = NULL;
		if (bpp != 32)
		{
			convertedDib = FreeImage_ConvertTo32Bits((FIBITMAP *)m_dib);
		}

		if( convertedDib)
		{
			if (m_dib)
				FreeImage_Unload((FIBITMAP *)m_dib);
			m_dib = convertedDib;
		}

		BYTE *bits = FreeImage_GetScanLine((FIBITMAP *)m_dib, 0);
		unsigned char keyR = bits[FI_RGBA_RED];
		unsigned char keyG = bits[FI_RGBA_GREEN];
		unsigned char keyB = bits[FI_RGBA_BLUE];

		int bytespp = FreeImage_GetLine((FIBITMAP *)m_dib) / FreeImage_GetWidth((FIBITMAP *)m_dib);
		unsigned int height = FreeImage_GetHeight((FIBITMAP *)m_dib);
		unsigned int width = FreeImage_GetWidth((FIBITMAP *)m_dib);
		for(unsigned int y = 0; y < height; y++) 
		{
			BYTE *bits = FreeImage_GetScanLine((FIBITMAP *)m_dib, y);
			for(unsigned x = 0; x < width; x++) 
			{
				if ((bits[FI_RGBA_RED] == keyR) && (bits[FI_RGBA_GREEN] == keyG) && (bits[FI_RGBA_BLUE] == keyB))
				{
					bits[FI_RGBA_ALPHA] = 0;
				}
				bits += bytespp;
			}
		}

		return true;
	}

	return false;
}

bool CFreeImageTextureLoader::ApplyDefaultAlpha(unsigned char alpha)
{
	if (m_dib)
	{
		if (GetChannelCount() < 4)
		{
			FIBITMAP* old = (FIBITMAP*)m_dib;
			m_dib = FreeImage_ConvertTo32Bits(old);
			FreeImage_Unload(old);
		}

		int bytespp = FreeImage_GetLine((FIBITMAP *)m_dib) / FreeImage_GetWidth((FIBITMAP *)m_dib);
		for(unsigned y = 0; y < FreeImage_GetHeight((FIBITMAP *)m_dib); y++) 
		{
			BYTE *bits = FreeImage_GetScanLine((FIBITMAP *)m_dib, y);
			for(unsigned x = 0; x < FreeImage_GetWidth((FIBITMAP *)m_dib); x++) 
			{
				bits[FI_RGBA_ALPHA] = alpha;
				bits += bytespp;
			}
		}
		return true;
	}

	return false;
}

void* CFreeImageTextureLoader::GetData(unsigned int frameID)const
{
	if (!m_dib)
	{
		return NULL;
	}

	if (0 == frameID)
	{
		return FreeImage_GetBits((FIBITMAP *)m_dib);
	}
	else if (frameID < m_frameCount)
	{
		return FreeImage_GetBits((FIBITMAP *)m_dib) + GetWidth() * GetHeight() * GetChannelCount();
	}
	else
	{
		return NULL;
	}
}

unsigned int CFreeImageTextureLoader::GetDataSize(unsigned int /*frameID*/)const
{	
	return (FreeImage_GetDIBSize((FIBITMAP *)m_dib) - sizeof(BITMAPINFO)) / m_frameCount;
}

unsigned int CFreeImageTextureLoader::GetUserData(unsigned int /*frameID*/)const
{
	return 0;
}

bool CFreeImageTextureLoader::IsDirectChannelOrder()const
{
	return (reverseRGB ? (FreeImage_GetRedMask((FIBITMAP *)m_dib) & 0x00FF0000) : !(FreeImage_GetRedMask((FIBITMAP *)m_dib) & 0x00FF0000)) > 0;
}

unsigned char CFreeImageTextureLoader::DetectUsageType(unsigned char type)
{
	if ('A' == type)
	{
		return USAGE_TYPE_DIFFUSE_NO_COMPRESS;
	}
	else if ('B' == type)
	{
		return USAGE_TYPE_DIFFUSE_SPLITTED_COMPRESSED;
	}
	else if ('C' == type)
	{
		return USAGE_TYPE_NORMAL_MAP;
	}
	else if ('D' == type)
	{
		return USAGE_TYPE_SPECULAR_MAP;
	}

	return USAGE_TYPE_DIFFUSE_COMPRESSED;
}

unsigned char CFreeImageTextureLoader::GetUsageType()
{
	return m_usageType;
}

CFreeImageTextureLoader::~CFreeImageTextureLoader()
{
	CleanUp();
}