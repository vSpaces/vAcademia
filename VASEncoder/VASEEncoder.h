#pragma once

#include "vase.h"

class CFLNFrame;
class VASEBitmap;
class VASEFramesDifPack;

class VASE_API VASEEncoder
{
public:
	VASEEncoder(void);
	~VASEEncoder(void);

public:
	//! кодирует изображение
	bool	EncodeDIBBitmap( void* lpBits, unsigned int auWidth, unsigned int auHeight, unsigned int auBPP
								, VASEBitmap* apBitmap, void * lpMaskBits);
	//! кодирует разницу между изображениями
	bool	EncodeDifference( void* lpBitsFirst, unsigned int auWidthFirst, unsigned int auHeightFirst, unsigned int auBPPFirst
											, void* lpBitsSecond, unsigned int auWidthSecond, unsigned int auHeightSecond, unsigned int auBPPSecond
											, VASEFramesDifPack* apPack, void * lpMaskBits);
	void	SetEncodeQuality(int quality);

protected:
	//! кодирует изображение
	bool	EncodeFrameBlock( CFLNFrame* frame, unsigned int auBlockIndex
									, VASEBitmap* apBitmap);
private:
	int m_quality;
};
