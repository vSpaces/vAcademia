#include "StdAfx.h"
#include ".\vaswaveletencoder.h"
#include ".\compressdiffparams.h"
#include <math.h>
#include ".\VASEBitmap.h"
#include "fln\FLNFrame.h"
#include ".\VASEFramesDifPack.h"
#include ".\MemLeaks.h"
#define ZLIB_WINAPI
#include "zlib.h"
#include <MMSystem.h>
#include "ITaskManager.h"
#include "wavelet\WaveletCommonRoutines.h"
#include "wavelet\ZLibWaveletPacker.h"
#include "wavelet\ZLibWaveletUnpacker.h"

VASWaveletEncoder::VASWaveletEncoder(void* taskManager, oms::omsContext* aContext)
{
	tManager = (ITaskManager*)taskManager;
	MP_NEW_V( waveletUnpacker, CZLibWaveletUnpacker, NULL);
	MP_NEW_V2( waveletPacker, CZLibWaveletPacker, tManager, aContext);
	context = aContext;
	m_minDiffSize = 16;
	m_sizeOfDiff = m_minDiffSize;
	m_arrOfRectSize = 0;
	m_destroyingNow = false;
	arrOfRect = NULL;
}

VASWaveletEncoder::~VASWaveletEncoder(void)
{
	SAFE_DELETE(waveletUnpacker);
	SAFE_DELETE(waveletPacker);
	if (m_arrOfRectSize != 0)
	for (int i=0; i<m_arrOfRectSize; i++)
	{
		SAFE_DELETE(arrOfRect[i]);
	}
	SAFE_DELETE_ARRAY(arrOfRect);
	m_arrOfRectSize = 0;
}

void VASWaveletEncoder::SetEncoderQuality(int quality)
{
	waveletPacker->SetQuality(quality);
}

//! кодирует изображение
bool	VASWaveletEncoder::EncodeDIBBitmap( void* lpBits, unsigned int auWidth, unsigned int auHeight, unsigned int auBPP, VASEBitmap* apBitmap, VASEBitmap* apEasyEncodedBitmap)
{
	CSize imageSize( auWidth, auHeight);
	CRect rect = CRect(0,0,auWidth, auHeight);
	apBitmap->SetSize( imageSize);
	apBitmap->SetPosition( rect.TopLeft());
	if (apEasyEncodedBitmap != NULL)
	{
		apEasyEncodedBitmap->SetSize( imageSize);
		apEasyEncodedBitmap->SetPosition( rect.TopLeft());
	}
	int outDataSize = EncodeFrameBlock(lpBits, auWidth, auHeight, auBPP, apBitmap, apEasyEncodedBitmap);

	if (outDataSize > 0)
	{
		m_sizeOfDiff = m_minDiffSize;
		return true;
	}
	else
		return false;
}

inline bool IsPixDifference(BYTE r1, BYTE g1, BYTE b1, BYTE r2, BYTE g2, BYTE b2, int DOP)
{
	if (abs(r1 - r2) > DOP ||
		abs(g1 - g2) > DOP ||
		abs(b1 - b2) > DOP)
		return true;
	return false;
}

//! кодирует разницу между изображениями
bool	VASWaveletEncoder::EncodeDifference( void* lpBitsFirst, unsigned int auWidthFirst, unsigned int auHeightFirst, unsigned int auBPPFirst
									  , void* lpBitsSecond, unsigned int auWidthSecond, unsigned int auHeightSecond, unsigned int auBPPSecond
									  , VASEFramesDifPack* apPack, VASEFramesDifPack* apEasyPack)

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

	int DOP = 6;

	CRect rect;
	bool flagDifference = false;
	double del = (double)auHeightSecond/auWidthSecond;
	int numbersOfBlock;
	//int sizeOfBlock = 32;
	numbersOfBlock = (int)ceil((double)auWidthSecond/m_sizeOfDiff);
	int sizeOfBlockY = (int)ceil(del*m_sizeOfDiff);
	void* difImage = NULL;

	 
	int diffNumber=0;
	unsigned int blockSize, passCount,zeroEpsilon, quantizeKoef;
	waveletPacker->GetOptions(passCount, zeroEpsilon, blockSize, quantizeKoef);

	if (m_arrOfRectSize < numbersOfBlock*numbersOfBlock)
	{
		for (int i=0; i<m_arrOfRectSize; i++)
		{
			SAFE_DELETE(arrOfRect[i]);
		}
		SAFE_DELETE_ARRAY(arrOfRect);

		arrOfRect = MP_NEW_ARR( CRect*, numbersOfBlock*numbersOfBlock);
		for (int i=0; i<numbersOfBlock*numbersOfBlock; i++)
		{
			arrOfRect[i] = MP_NEW( CRect);
		}
		m_arrOfRectSize = numbersOfBlock*numbersOfBlock;
	}

	while (true)
	{
		for (int i=0; i<numbersOfBlock; i++)
		{
			for (int j=0; j<numbersOfBlock; j++)
			{
				rect = CRect(i*m_sizeOfDiff, j*sizeOfBlockY, (i+1)*m_sizeOfDiff > (int)auWidthSecond ? auWidthSecond : (i+1)*m_sizeOfDiff, (j+1)*sizeOfBlockY > (int)auHeightSecond ? auHeightSecond : (j+1)*sizeOfBlockY);
				rect = IncreaseBlock(rect, auWidthSecond, auHeightSecond, blockSize);
				if (DefineChangeBlocks(rect.TopLeft(), auWidthSecond, auHeightSecond, lpBitsFirst, rect.Width(), rect.Height(), auBPPFirst
					, lpBitsSecond, rect.Width(), rect.Height(), auBPPSecond, DOP))
				{
					flagDifference =true;
					*arrOfRect[diffNumber] = rect;
					diffNumber++;
				}
			}
		}
		if (!flagDifference)
			return false;

		if (diffNumber > numbersOfBlock*numbersOfBlock*0.65)
		{ // если дифов больше 65% изображения
			diffNumber = 1;
			*arrOfRect[0] = CRect(0,0,auWidthSecond,auHeightSecond);
			break;
		}
		if (diffNumber-1 > 100.0 * (auWidthSecond*auHeightSecond) / (1024*1024))//100 для 1024*1024 
		{
			m_sizeOfDiff *= 2; //будет перекрывать полностью 4 дифа меньших
			numbersOfBlock = (int)ceil((double)auWidthSecond/m_sizeOfDiff);
			sizeOfBlockY *= 2;
			diffNumber = 0;
		}
		else
			break;
	}
	
	int numbersOfDiffOnThread = 15;
	if (tManager)
		numbersOfDiffOnThread = 3;
	int numOfThreads = (int)ceil((float)diffNumber/numbersOfDiffOnThread);

	CCompressDiffTask** arrdt = MP_NEW_ARR( CCompressDiffTask*, numOfThreads);
	CCompressDiffTask* dt = NULL;

	_SYSTEM_INFO nfo;
	GetSystemInfo(&nfo);

	if (numOfThreads == 1 || nfo.dwNumberOfProcessors <= 1)
	{
		MP_NEW_V( dt, CCompressDiffTask, context);
		dt->AddData(&apPack, &apEasyPack, arrOfRect, m_arrOfRectSize, 0, diffNumber,numbersOfDiffOnThread,auWidthSecond,lpBitsSecond, auBPPSecond);
		dt->AddEncodeParametrs(waveletPacker->GetQuality());
		dt->Start();
		SAFE_DELETE(dt);
	}
	else
	{
		for (int j = 0; j < numOfThreads; j++)
		{
			MP_NEW_V( dt, CCompressDiffTask, context);
			dt->AddData(&apPack, &apEasyPack, arrOfRect, m_arrOfRectSize, j*numbersOfDiffOnThread, (((j+1)*numbersOfDiffOnThread <= diffNumber) ? (j+1)*numbersOfDiffOnThread : diffNumber)
				,numbersOfDiffOnThread,auWidthSecond,lpBitsSecond, auBPPSecond);
			dt->AddEncodeParametrs(waveletPacker->GetQuality());
			if (tManager)
				tManager->AddTask(dt, MAY_BE_MULTITHREADED, 0);
			else
				dt->StartThread();
			arrdt[j] = dt;
		}
		while (true)
		{
			bool performing = false;
			for (int i=0; i < numOfThreads; i++)
			{
				if (!arrdt[i]->IsPerformed())
					performing = true;
			}
			if (!performing)
				break;
			if (m_destroyingNow)
				break;
			Sleep(5);
		}
		for (int i=0; i<numOfThreads; i++)
		{
			SAFE_DELETE(arrdt[i]);
		}
		SAFE_DELETE_ARRAY(arrdt);
	}

	return true;
}

bool VASWaveletEncoder::DefineChangeBlocks(CPoint point, unsigned int auWidthOriginal, unsigned int auHeightOriginal
										   , void* lpBitsFirst, unsigned int auWidthFirst, unsigned int auHeightFirst, unsigned int auBPPFirst
											, void* lpBitsSecond, unsigned int auWidthSecond, unsigned int auHeightSecond, unsigned int auBPPSecond, unsigned int DOP)
{
	int pitch = auWidthOriginal * (int)((auBPPSecond+7) / 8);
	int dwordsOriginal = pitch >> 2;
	pitch = auWidthSecond * (int)((auBPPSecond+7) / 8);
	int dwords = pitch >> 2;
	int pos = 0;
	bool bDif = false;
	DWORD *p1 = NULL, *p2 = NULL;

	for (int i = 0; i < (int)auHeightSecond; ++i)
	{
		p1 = &((DWORD*)lpBitsFirst)[(i+point.y)*dwordsOriginal+point.x];
		p2 = &((DWORD*)lpBitsSecond)[(i+point.y)*dwordsOriginal+point.x];
		pos = 0;	
		for (int j = 0; j < dwords; ++j)
		{
			if (((DWORD*)p1)[pos] != ((DWORD*)p2)[pos])
			{
				BYTE r1 = ((BYTE*)(&((DWORD*)p1)[pos]))[0];
				BYTE g1 = ((BYTE*)(&((DWORD*)p1)[pos]))[1];
				BYTE b1 = ((BYTE*)(&((DWORD*)p1)[pos]))[2];

				BYTE r2 = ((BYTE*)(&((DWORD*)p2)[pos]))[0];
				BYTE g2 = ((BYTE*)(&((DWORD*)p2)[pos]))[1];
				BYTE b2 = ((BYTE*)(&((DWORD*)p2)[pos]))[2];

				if (IsPixDifference(r1, g1, b1, r2,g2, b2, DOP))
					return true;
			}
			pos++;
		}
	}
	return false;
}
unsigned int VASWaveletEncoder::EncodeFrameBlock(void* lpBits, unsigned int auWidth, unsigned int auHeight, unsigned int auBPP,VASEBitmap* apBitmap, VASEBitmap* apEasyEncodedBitmap)
{
	// проверить параметры
	if( !lpBits)
		return false;
	if( auWidth == 0 || auHeight== 0)
		return false;
	if( auBPP != 32)
		return false;
	if( auWidth > 4096 || auHeight > 4096)
		return false;
	if( !apBitmap)
		return false;

	waveletPacker->wlPackRoutines.wlCommonRoutines.CalculateLookupsIfNeeded();
	waveletPacker->wlPackRoutines.wlCommonRoutines.AllocateWaveletBuffersIfNeeded(auWidth*auHeight);
	waveletPacker->SetImageData((unsigned char*)lpBits, auWidth, auHeight);
	if (!waveletPacker->Pack())
		return 0;
	void* apBitmapBuffer = apBitmap->GetBufferSetLength( waveletPacker->GetPackedDataSize());
	apBitmap->SetEncodedBufferSize( waveletPacker->GetPackedDataSize());
	memcpy(apBitmapBuffer, (void*)waveletPacker->GetPackedData(), waveletPacker->GetPackedDataSize());

	if (apEasyEncodedBitmap != NULL)
	{
		void* pEasyPackedData = (void*)waveletPacker->GetEasyPackedData();
		void* apBitmapBuffer2 = apEasyEncodedBitmap->GetBufferSetLength( waveletPacker->GetEasyPackedDataSize());
		apEasyEncodedBitmap->SetEncodedBufferSize( waveletPacker->GetEasyPackedDataSize());
		memcpy(apBitmapBuffer2, pEasyPackedData, waveletPacker->GetEasyPackedDataSize());
	}
	return waveletPacker->GetPackedDataSize();

}

void* VASWaveletEncoder::Decode( void* lpBits, unsigned int dataSize, unsigned int auWidth, unsigned int auHeight, unsigned int auBPP, bool abEasyPacked)
{
	if( !lpBits)
		return false;
	if (dataSize == 0)
		return false;
	if( auWidth == 0 || auHeight== 0)
		return false;

	waveletUnpacker->SetPackedData((unsigned char*)lpBits, dataSize);
	if (!waveletUnpacker->Unpack(abEasyPacked))
		return NULL;
	return waveletUnpacker->GetRGB();
}

unsigned int VASWaveletEncoder::GetDecodedPictureSize()
{
	return waveletUnpacker->GetDataSize();
}

void VASWaveletEncoder::StopPacking()
{
	waveletPacker->StopPacking();
	m_destroyingNow = true;
}

CRect VASWaveletEncoder::IncreaseBlock(CRect rect, unsigned int maxWidth, unsigned int maxHeight, int bs)
{
	CRect newRect;
	newRect = rect;
	int steps = rect.top % bs;
	if (steps != 0 )
		newRect.top = rect.top + bs - steps;
	steps = rect.left % bs;
	if (steps != 0 )
		newRect.left = rect.left + bs - steps;
	
	steps = newRect.Height() % bs;
	if (steps != 0 && newRect.bottom + bs - steps < (int)maxHeight)
		newRect.bottom = newRect.bottom + bs - steps;

	steps = newRect.Width() % bs;
	if (steps != 0 && newRect.right + bs - steps < (int)maxWidth)
		newRect.right = newRect.right + bs - steps;

	return newRect;
}