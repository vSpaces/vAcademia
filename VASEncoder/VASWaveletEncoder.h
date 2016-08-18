#pragma once

namespace oms
{
	class omsContext;
}

class CZLibWaveletUnpacker;
class CZLibWaveletPacker;
class CFLNFrame;
class VASEBitmap;
class CCompressDiffTask;
class ITaskManager;

class VASE_API VASWaveletEncoder
{
public:
	typedef unsigned char uchar;
	typedef unsigned int  uint;

	VASWaveletEncoder(void* taskManager, oms::omsContext* aContext);
	~VASWaveletEncoder(void);

	void SetEncoderQuality(int quality);

	bool	EncodeDIBBitmap( void* lpBits, unsigned int auWidth, unsigned int auHeight, unsigned int auBPP, VASEBitmap* apBitmap, VASEBitmap* apEasyEncodedBitmap);
	bool	EncodeDifference( void* lpBitsFirst, unsigned int auWidthFirst, unsigned int auHeightFirst, unsigned int auBPPFirst
		, void* lpBitsSecond, unsigned int auWidthSecond, unsigned int auHeightSecond, unsigned int auBPPSecond
		, VASEFramesDifPack* apPack, VASEFramesDifPack* apEasyPack);
	
	unsigned int GetDecodedPictureSize();
	void* Decode( void* lpBits, unsigned int dataSize, unsigned int auWidth, unsigned int auHeight, unsigned int auBPP, bool abEasyPacked=false);
	void StopPacking();

private:
	bool DefineChangeBlocks(CPoint point, unsigned int auWidthOriginal, unsigned int auHeightOriginal, void* lpBitsFirst
		, unsigned int auWidthFirst, unsigned int auHeightFirst, unsigned int auBPPFirst
		, void* lpBitsSecond, unsigned int auWidthSecond, unsigned int auHeightSecond, unsigned int auBPPSecond, unsigned int DOP);
	unsigned int EncodeFrameBlock(void* lpBits, unsigned int auWidth, unsigned int auHeight, unsigned int auBPP
		,VASEBitmap* apBitmap, VASEBitmap* apEasyEncodedBitmap);
	CRect IncreaseBlock(CRect rect, unsigned int maxWidth, unsigned int maxHeight, int bs);
	
	CZLibWaveletPacker* waveletPacker;
	CZLibWaveletUnpacker* waveletUnpacker;
	ITaskManager* tManager;
	CRect** arrOfRect;

	 oms::omsContext* context;
	bool m_destroyingNow;

	int m_sizeOfDiff;
	int m_minDiffSize;
	int m_arrOfRectSize;
};
