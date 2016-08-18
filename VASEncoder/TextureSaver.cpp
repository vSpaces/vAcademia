
#include "StdAfx.h"
#include <Assert.h>
#include "TextureSaver.h"

#include "freeimage.h"
#include "TextureProc.h"
#include "VASEBitmap.h"

#include ".\MemLeaks.h"

CTextureSaver::CTextureSaver():
	m_dib(NULL)
{
	FreeImage_Initialise();
}

void CTextureSaver::CleanUp()
{
	if (m_dib)
	{
		FreeImage_Unload((FIBITMAP *)m_dib);
		m_dib = NULL;
	}
}

bool CTextureSaver::CompressImageData(int pictureWidth, int pictureHeight, int channelsCount,  void* pictureData
									  ,int quality, VASEBitmap* apBitmap)
{
	CleanUp();

	unsigned long outDataSize = 0;

	if( pictureWidth == 0 || pictureHeight == 0 || m_dib != NULL)
		int ii=0;

	assert(m_dib == NULL);
	assert(pictureWidth > 0);
	assert(pictureHeight > 0);

	if( !apBitmap)
		return false;

	assert(channelsCount > 2);
	bool isTransparent = false;
	if (channelsCount == 4)
		isTransparent = true;

	unsigned bpp = channelsCount == 4 ? 32 : 24;
	unsigned pitch = isTransparent == true ? ((((bpp * pictureWidth) + 31) / 32) * 4): ((((bpp * pictureWidth) + 23) / 24) * 3);

	m_dib = FreeImage_ConvertFromRawBits((BYTE *)pictureData, pictureWidth, pictureHeight, pitch, bpp, 0x00FF0000, 0x0000FF00, 0x000000FF, true);
	void* convertedDib = FreeImage_ConvertTo24Bits((FIBITMAP *)m_dib);
	if( convertedDib)
	{
		if (m_dib)
			FreeImage_Unload((FIBITMAP *)m_dib);
		m_dib = convertedDib;
	}

	// FreeImage_FlipVertical((FIBITMAP *)m_dib);

	FreeImage_SetDotsPerMeterX((FIBITMAP *)m_dib, quality);
	FreeImage_SetDotsPerMeterY((FIBITMAP *)m_dib, quality);

	if (!m_dib)
	{
		return false;
	}

	unsigned int w = FreeImage_GetWidth((FIBITMAP *)m_dib);
	unsigned int h = FreeImage_GetHeight((FIBITMAP *)m_dib);

	unsigned char* buffer = (unsigned char*)FreeImage_GetBits((FIBITMAP *)m_dib);

	/*int fiPitch = FreeImage_GetPitch( (FIBITMAP*)m_dib);
	unsigned char tmp = 0;
	unsigned char* buffer_pointer = buffer;
	for (unsigned int i = 0; i < h; i++)
	{
		unsigned char* line_buffer = buffer_pointer;
		for (unsigned int j = 0; j < w; j++, line_buffer+=3)
		{
			tmp = *line_buffer;
			line_buffer[0] = line_buffer[2];
			line_buffer[2] = tmp;
		}
		buffer_pointer += fiPitch;
	}
	FreeImage_Save( FIF_BMP, (FIBITMAP*)m_dib, "c:\\22.bmp");*/

	unsigned long tempBufferSize = pictureWidth * pictureHeight * 3; // с учетом что в 3 раза по любому жмет
	if( tempBufferSize < 32768)
		tempBufferSize = 32768;	// иначе может не поместиться заголовок
	byte* apBitmapBuffer = apBitmap->GetBufferSetLength( tempBufferSize);

	//*pictureDataOut = (void*) new byte[tempBufferSize];

	FREE_IMAGE_FORMAT fif = FIF_JPEG;

	FreeImageIO io;
	io.read_proc = _ReadProc;
	io.write_proc = _WriteProc;
	io.tell_proc = _TellProc;
	io.seek_proc = _SeekProc;

	SMemFile memFile;
	memFile.buffer = apBitmapBuffer;
	memFile.size = tempBufferSize;
	memFile.currentPosition = 0;

	int flags = 0;
	if( quality < 10) 
		flags |= JPEG_QUALITYBAD;
	else	if( quality < 25) 
		flags |= JPEG_QUALITYAVERAGE;
	else	if( quality < 50) 
		flags |= JPEG_QUALITYNORMAL;
	else	if( quality < 75) 
		flags |= JPEG_QUALITYGOOD;
	else
		flags |= JPEG_QUALITYSUPERB;

	FreeImage_SaveToHandle(fif, (FIBITMAP *)m_dib, &io, (fi_handle)&memFile.buffer, flags);

	outDataSize = memFile.currentPosition;

	//if( dataSize)
	//	*dataSize = outDataSize;

	apBitmap->SetEncodedBufferSize( outDataSize);

	CleanUp();
	return outDataSize==0?false:true;
}


CTextureSaver::~CTextureSaver()
{
	FreeImage_DeInitialise();
}