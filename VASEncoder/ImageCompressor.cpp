
#include "StdAfx.h"
#include <Assert.h>
#include "ImageCompressor.h"

#include "freeimage.h"
#include "TextureProc.h"
#include "VASEBitmap.h"

CImageCompressor::CImageCompressor():
	m_dib(NULL)
{
	FreeImage_Initialise();
}

void CImageCompressor::CleanUp()
{
	if (m_dib)
	{
		FreeImage_Unload((FIBITMAP *)m_dib);
		m_dib = NULL;
	}
}

bool CImageCompressor::CompressImageData(int pictureWidth, int pictureHeight, int channelsCount,  void* pictureData
									  ,int quality, VASEBitmap* apBitmap)
{
	unsigned long outDataSize = 0;

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
	int size = pictureWidth*pictureHeight*channelsCount;

	m_dib = FreeImage_ConvertFromRawBits((BYTE *)pictureData, pictureWidth, pictureHeight, pitch, bpp, 0x00FF0000, 0x0000FF00, 0x000000FF, true);	
	void* convertedDib = FreeImage_ConvertTo24Bits((FIBITMAP *)m_dib);
	if( convertedDib)
	{
		CleanUp();
		m_dib = convertedDib;
	}

	if (!m_dib)
	{
		return false;
	}

	unsigned char* buffer = (unsigned char*)FreeImage_GetBits((FIBITMAP *)m_dib);

		unsigned long tempBufferSize = pictureWidth * pictureHeight * 3; // с учетом что в 3 раза по любому жмет
	if( tempBufferSize < 32768)
		tempBufferSize = 32768;	// иначе может не поместиться заголовок
	byte* apBitmapBuffer = apBitmap->GetBufferSetLength( tempBufferSize);

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

	apBitmap->SetEncodedBufferSize( outDataSize);

	CleanUp();
	return outDataSize==0?false:true;
}


CImageCompressor::~CImageCompressor()
{
	FreeImage_DeInitialise();
}