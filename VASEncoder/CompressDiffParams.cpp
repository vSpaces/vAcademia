#include "StdAfx.h"
#include ".\compressdiffparams.h"
#include "ThreadAffinity.h"
#include <math.h>
#include ".\VASEBitmap.h"
#include ".\VASEFramesDifPack.h"
#include "fln\FLNFrame.h"
#include ".\MemLeaks.h"
#define ZLIB_WINAPI
#include "zlib.h"
#include "wavelet\ZLibWaveletPacker.h"
#include "wavelet\WaveletCommonRoutines.h"
#include "..\logwriter\ILogWriter.h"
#include "CrashHandlerInThread.h"

CCompressDiffTask::CCompressDiffTask(oms::omsContext* aContext)
{
	MP_NEW_V2( waveletPacker, CZLibWaveletPacker, NULL, aContext);
	MP_NEW_V2( zlibPacker, CZLibWaveletPacker, NULL, aContext);
	m_context = aContext;
}

CCompressDiffTask::~CCompressDiffTask(void)
{
	DWORD dwCode = 0;
	dwCode = m_thread.GetCurrentState();

	if (dwCode == STILL_ACTIVE)
	{
		::WaitForSingleObject(m_thread.GetHandle(), INFINITE);// wait for stop
	}
	SAFE_DELETE(waveletPacker);
	SAFE_DELETE(zlibPacker);
	m_arrOfRect = NULL;
	m_arrOfRectSize = 0;
}
void CCompressDiffTask::AddData(VASEFramesDifPack** apPack, VASEFramesDifPack** apEasyPack, CRect** arrOfRect, int arrOfRectSize, int startPos, int stopPos, int numbersOfDiffOnThread
						   , int dwordsOriginal, void *lpBitsSecond, int auBPPSecond)
{
	m_pPack = *apPack;
	m_pEasyPack = *apEasyPack;
	m_arrOfRect = arrOfRect;
	m_arrOfRectSize = arrOfRectSize;

	m_startPos = startPos;
	m_stopPos = stopPos;
	m_numbersOfDiffOnThread = numbersOfDiffOnThread;
	m_dwordsOriginal = dwordsOriginal;
	m_lpBitsSecond = lpBitsSecond;
	m_auBPPSecond = auBPPSecond;
	
}

void CCompressDiffTask::PrintInfo()
{
	if (m_context)
	{		
		m_context->mpLogWriter->WriteLn("CompressDiffTask");
	}
}

void CCompressDiffTask::AddEncodeParametrs(int quality)
{
	waveletPacker->SetQuality(quality);
}
DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();
	CCompressDiffTask* task = (CCompressDiffTask*)lpParam;
	task->Start();
	task->OnDone();

	return 0;
}
//старый способ, используется если нет taskmanagera
void CCompressDiffTask::StartThread()
{
	m_thread.SetParameter(this);
	SetSecondaryThreadAffinity( m_thread.GetHandle());
	m_thread.SetThreadProcedure(ThreadProc);
	m_thread.Start();
}
void CCompressDiffTask::Start()
{
	CRect rect;
	int outDataSize=0;
	void* difImage = NULL;
	for (int i=m_startPos; i < m_stopPos; i++)
	{
		rect = *m_arrOfRect[i];
		VASEBitmap* bitmap = m_pPack->AddBitmap();
		VASEBitmap* apEasyEncodedBitmap = m_pEasyPack->AddBitmap();
		
		CSize imageSize( rect.Width(), rect.Height());
		bitmap->SetSize( imageSize);
		bitmap->SetPosition( rect.TopLeft());

		apEasyEncodedBitmap->SetSize( imageSize);
		apEasyEncodedBitmap->SetPosition( rect.TopLeft());

		void* apBitmapBuffer = NULL;
		difImage = MP_NEW_ARR( BYTE, (m_auBPPSecond>>2) * rect.Width() * rect.Height());
		DWORD *p1 = NULL, *p2 = NULL;

		int dwords = rect.Width();

		for (int y = 0; y < rect.Height(); y++)
		{
			p1 = &((DWORD*)m_lpBitsSecond)[(y+rect.TopLeft().y)*m_dwordsOriginal+rect.TopLeft().x];
			p2 = &((DWORD*)difImage)[y*dwords];
			memcpy(p2, p1, dwords*sizeof(DWORD));
		}
		outDataSize = EncodeFrameBlock(difImage, rect.Width(), rect.Height(), m_auBPPSecond, apBitmapBuffer, outDataSize);
		
		apBitmapBuffer = bitmap->GetBufferSetLength( outDataSize);
		
		memcpy(apBitmapBuffer, (void*)zlibPacker->GetPackedData(), zlibPacker->GetPackedDataSize());
		//memcpy(apBitmapBuffer, (void*)waveletPacker->GetPackedData(), waveletPacker->GetPackedDataSize());
		bitmap->SetEncodedBufferSize( outDataSize);
		if( outDataSize == 0)
		{
			m_pPack->DeleteBitmap( bitmap);
		}

		void* pEasyPackedData = (void*)waveletPacker->GetEasyPackedData();
		void* apBitmapBuffer2 = apEasyEncodedBitmap->GetBufferSetLength( waveletPacker->GetEasyPackedDataSize());
		apEasyEncodedBitmap->SetEncodedBufferSize( waveletPacker->GetEasyPackedDataSize());
		memcpy(apBitmapBuffer2, pEasyPackedData, waveletPacker->GetEasyPackedDataSize());
		
		SAFE_DELETE_ARRAY(difImage);

	}
}
unsigned int CCompressDiffTask::EncodeFrameBlock(void* lpBits, unsigned int auWidth, unsigned int auHeight, unsigned int auBPP,void* apBitmap, unsigned int apBitmapSize)
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

	waveletPacker->wlPackRoutines.wlCommonRoutines.CalculateLookupsIfNeeded();
	waveletPacker->wlPackRoutines.wlCommonRoutines.AllocateWaveletBuffersIfNeeded(auWidth*auHeight);
	waveletPacker->SetImageData((unsigned char*)lpBits, auWidth, auHeight);
	if (!waveletPacker->Pack(false))
		return 0;
	
	if (!zlibPacker->Pack(waveletPacker, true))
		return 0;

	return zlibPacker->GetPackedDataSize();
}

bool CCompressDiffTask::ChildThreadPerformed()
{
	return true;
}

void CCompressDiffTask::SelfDestroy()
{
	MP_DELETE_THIS;
}