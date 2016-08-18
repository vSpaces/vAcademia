#include "StdAfx.h"
#include ".\vaseencoder.h"
#include ".\VASEBitmap.h"
#include ".\VASEFramesDifPack.h"
#include ".\ImageCompressor.h"
#include "fln\FLNFrame.h"

#include ".\MemLeaks.h"

VASEEncoder::VASEEncoder(void)
{
	m_quality = 10;
}

VASEEncoder::~VASEEncoder(void)
{
}

//! кодирует изображение
bool	VASEEncoder::EncodeDIBBitmap( void* lpBits, unsigned int auWidth, unsigned int auHeight, unsigned int auBPP, VASEBitmap* apBitmap, void * lpMaskBits)
{
	// проверить параметры
	if( !apBitmap)
		return false;
	if( !lpBits)
		return false;
	if( auWidth == 0 || auHeight== 0)
		return false;
	if( auBPP != 32)
		return false;
	if( auWidth > 4096 || auHeight > 4096)
		return false;

	CFLNFrame firstFrame;
	if( !firstFrame.LoadFromDIB( auWidth, auHeight, lpBits, auBPP))
		return false;

	CVASEBmp	emptyFrame;

	// получить список областей
	unsigned int blocksCount = firstFrame.DefineChangeBlocks( true, emptyFrame, lpMaskBits);

	if( blocksCount != 1)
		return false;

	return EncodeFrameBlock( &firstFrame, 0, apBitmap);
}

//! кодирует разницу между изображениями
bool	VASEEncoder::EncodeDifference( void* lpBitsFirst, unsigned int auWidthFirst, unsigned int auHeightFirst, unsigned int auBPPFirst
													, void* lpBitsSecond, unsigned int auWidthSecond, unsigned int auHeightSecond, unsigned int auBPPSecond
													, VASEFramesDifPack* apPack, void * lpMaskBits)

{
	// проверить параметры
	if( !apPack)
		return false;
	if( !lpBitsFirst || !lpBitsSecond)
		return false;
	if( auWidthFirst == 0 || auHeightFirst == 0 || auWidthSecond == 0 || auHeightSecond == 0)
		return false;
	if( auWidthFirst != auWidthSecond)
		return false;
	if( auHeightFirst != auHeightSecond)
		return false;
	if( auBPPFirst != auBPPSecond)
		return false;
	if( auBPPFirst != 32)
		return false;
	if( auWidthFirst > 4096 || auHeightFirst > 4096)
		return false;

	CFLNFrame firstFrame;
	if( !firstFrame.LoadFromDIB( auWidthFirst, auHeightFirst, lpBitsFirst, auBPPFirst))
		return false;

	CFLNFrame secondFrame;
	if( !secondFrame.LoadFromDIB( auWidthSecond, auHeightSecond, lpBitsSecond, auBPPSecond))
		return false;

	// получить список областей
	unsigned int blocksCount = secondFrame.DefineChangeBlocks( false, firstFrame.bmp, lpMaskBits);
	
	if( blocksCount == 0)
		return false;

	// encode области
	for( unsigned int i=0; i<blocksCount; i++)
	{
		// create VASEBitmap для областей
		// initialize VASEBitmap для областей
		VASEBitmap* bitmap = apPack->AddBitmap();
		if( !EncodeFrameBlock( &secondFrame, i, bitmap))
		{
			// fill VASEFramesDifPack
			apPack->DeleteBitmap( bitmap);
		}
	}
	return true;
}

//! кодирует изображение
bool	VASEEncoder::EncodeFrameBlock( CFLNFrame* frame, unsigned int auBlockIndex, VASEBitmap* apBitmap)
{
	if( !apBitmap)
		return false;
	if( !frame)
		return false;

	byte* mem = NULL;
	unsigned int memSize = 0;
	CRect	rect;

	bool	success = false;
	if( frame->GetChangedBlock( auBlockIndex, NULL, &memSize, NULL))
	{
		//mem = (byte*)malloc( memSize);
		mem = MP_NEW_ARR( byte, memSize);
		if( frame->GetChangedBlock( auBlockIndex, mem, &memSize, &rect))
		{
			unsigned long dataSize = 0;
			void* pictureDataOut = NULL;
			CImageCompressor	compressor;
			CSize imageSize( rect.Width(), rect.Height());
			imageSize += CSize( 1, 1);
			if( compressor.CompressImageData( imageSize.cx, imageSize.cy, 4, mem, m_quality, apBitmap))
			{
				// initialize VASEBitmap для областей
				apBitmap->SetSize( imageSize);
				apBitmap->SetPosition( rect.TopLeft());
				success = true;
			}
		}
		SAFE_DELETE_ARRAY(mem);
		//free( mem);
	}

	return success;
}

void	VASEEncoder::SetEncodeQuality(int quality)
{
	m_quality = quality;
}
