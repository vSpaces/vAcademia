#pragma once
#include "Thread.h"
#include "../nengine/nengine/ITask.h"

class CZLibWaveletPacker;
class CCompressDiffTask: public ITask
{
public:
	CCompressDiffTask(oms::omsContext* aContext);
	~CCompressDiffTask(void);
	void AddData(VASEFramesDifPack** apPack, VASEFramesDifPack** apEasyPack, CRect** arrOfRect, int arrOfRectSize, int startPos, int stopPos, int numbersOfDiffOnThread
		, int dwordsOriginal, void *lpBitsSecond, int auBPPSecond);
	unsigned int EncodeFrameBlock(void* lpBits, unsigned int auWidth, unsigned int auHeight, unsigned int auBPP
		,void* apBitmap, unsigned int apBitmapSize);
	void AddEncodeParametrs(int quality);
	void Start();
	void StartThread();
	void PrintInfo();
	bool ChildThreadPerformed();

	void Destroy() { assert(false); }
	void SelfDestroy();

	bool IsOptionalTask() { return false; }

	CZLibWaveletPacker* waveletPacker;
	CZLibWaveletPacker* zlibPacker;

	VASEFramesDifPack* m_pPack;
	VASEFramesDifPack* m_pEasyPack;
	CRect** m_arrOfRect;
	int m_arrOfRectSize;
	int m_startPos;
	int m_stopPos;
	int m_numbersOfDiffOnThread;
	int m_dwordsOriginal;
	void *m_lpBitsSecond;
	int m_auBPPSecond;
	CThread m_thread;

	oms::omsContext* m_context;

private:
	bool m_zlibPackEnd;
};
