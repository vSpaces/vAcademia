
#pragma once

#include "WaveletPacker.h"
#include "..\..\nengine\nengine\ITaskManager.h"
#include "ZLibPackTask.h"
#include "ZLibPackGranularity.h"
#include "oms_context.h"

class CWaveletHeader;

class CZLibWaveletPacker : public CWaveletPacker
{
public:
	CZLibWaveletPacker(ITaskManager* taskManager, oms::omsContext* aContext);
	~CZLibWaveletPacker();

	bool Pack(bool anZipTo=true);
	bool Pack(CWaveletPacker* aWPacker, bool anWaitingForEnd = true);

	unsigned char* GetPackedData();
	unsigned int GetPackedDataSize();
	
	unsigned char* GetEasyPackedData();
	unsigned int GetEasyPackedDataSize();

	void StopPacking();

	void OnZlibThreadEnded(CWaveletPacker* aWPacker);
	bool ZlibThreadWaiting();
private:
	void DestroyResultOnEndForTask(CZLibPackTask* aTask);
	void AllocateBufferIfNeeded(unsigned int size, unsigned int easyBufferSize);
	void CleanUp();
	

	ITaskManager* m_taskManager;
	CZLibPackTask* m_yTask[MAX_PACK_TASKS];
	CZLibPackTask *m_uTask, *m_vTask;

	unsigned char* m_packedData;
	unsigned int m_packedSize;
	unsigned int m_allocatedSize;

	unsigned char* m_easyPackedData;
	unsigned int m_easyPackedSize;
	unsigned int m_allocatedEasyPackedSize;
	
	bool m_threadStopByNeedStop;
	bool m_needStop;

	oms::omsContext* context;

	int m_currPartCount;
	CWaveletHeader* m_currentHeader;
};