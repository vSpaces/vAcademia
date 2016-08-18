#include "StdAfx.h"
#include "ZLibWaveletPacker.h"
#define ZLIB_WINAPI
#include "zlib.h"
#include "WaveletHeader.h"


CZLibWaveletPacker::CZLibWaveletPacker(ITaskManager* taskManager, oms::omsContext* aContext):
m_taskManager(taskManager),
m_packedData(NULL),
m_easyPackedData(NULL),
m_allocatedSize(0),
m_packedSize(0),
m_easyPackedSize(0),
m_allocatedEasyPackedSize(0),
m_needStop(false)
{
	context = aContext;
	for (int i=0; i < MAX_PACK_TASKS; i++)
		m_yTask[i] = MP_NEW(CZLibPackTask);
	for (int i=0; i < MAX_PACK_TASKS; i++)
		m_yTask[i]->SetContext(aContext);

	m_uTask = MP_NEW(CZLibPackTask);
	m_vTask = MP_NEW(CZLibPackTask);
	m_uTask->SetContext(aContext);
	m_vTask->SetContext(aContext);
	m_threadStopByNeedStop = false;
	m_currentHeader = MP_NEW(CWaveletHeader);
	
}

bool CZLibWaveletPacker::Pack(bool anZipTo)
{
	bool isOk = CWaveletPacker::Pack();

	if (!isOk)
	{
		return false;
	}
	if (anZipTo)
		return Pack(this);
	else
		return true;
}

bool CZLibWaveletPacker::Pack(CWaveletPacker* aWPacker, bool anWaitingForEnd )
{
	CWaveletHeader* header = m_currentHeader;

	unsigned int size = 0;
	unsigned int offset = 0;
	unsigned int yOriginalSize = aWPacker->GetChannelSize(CHANNEL_Y);
	unsigned char partCount = GetPackSubdivisionCount(yOriginalSize);

	m_currPartCount = partCount;

	header->SetYPartCount(partCount);
	unsigned int headerSize = header->GetHeaderSize();

	for (unsigned char partID = 0; partID < partCount; partID++)
	{
		unsigned int partSize = GetPartSize(partCount, yOriginalSize, partID);

		m_yTask[partID]->Rewind();
		size += m_yTask[partID]->SetData(aWPacker->GetChannel(CHANNEL_Y) + offset, partSize);

		offset += partSize;
	}

	m_uTask->Rewind();
	size += m_uTask->SetData(aWPacker->GetChannel(CHANNEL_U), aWPacker->GetChannelSize(CHANNEL_U));

	m_vTask->Rewind();
	size += m_vTask->SetData(aWPacker->GetChannel(CHANNEL_V), aWPacker->GetChannelSize(CHANNEL_V));

	size += headerSize;
	AllocateBufferIfNeeded(size, 0);
	header->SetDataPointer(m_packedData);
	m_threadStopByNeedStop = false;

	if (m_taskManager != NULL)
	{
		for (unsigned char partID = 0; partID < partCount; partID++)
		{
			m_taskManager->AddTask(m_yTask[partID], MAY_BE_MULTITHREADED, 0);
		}
		m_taskManager->AddTask(m_uTask, MAY_BE_MULTITHREADED, 0);
		m_taskManager->AddTask(m_vTask, MAY_BE_MULTITHREADED, 0);

		if (!anWaitingForEnd)
			return true;

		bool isWaiting = true;
		while (isWaiting)
		{
			Sleep(1);

			if (m_needStop) {
				DestroyResultOnEndForTask(m_uTask);
				DestroyResultOnEndForTask(m_vTask);
				for (unsigned char partID = 0; partID < partCount; partID++)
					DestroyResultOnEndForTask(m_yTask[partID]);
				for (int i=partCount; i<MAX_PACK_TASKS; i++)
					SAFE_DELETE(m_yTask[i]);
				m_threadStopByNeedStop = true;
				return false;
			}

			isWaiting = ZlibThreadWaiting();
		}
	}
	else
	{
		for (unsigned char partID = 0; partID < partCount; partID++)
		{
			m_yTask[partID]->Start();
			m_yTask[partID]->OnDone();
		}
		m_uTask->Start();
		m_uTask->OnDone();
		m_vTask->Start();
		m_vTask->OnDone();
	}

	OnZlibThreadEnded(aWPacker);
	return true;
}

void CZLibWaveletPacker::OnZlibThreadEnded(CWaveletPacker* aWPacker)
{
	unsigned int ySize = 0;
	for (unsigned char partID = 0; partID < m_currPartCount; partID++)
	{
		int packedSize = m_yTask[partID]->GetPackedDataSize();
		ySize += packedSize;
		m_currentHeader->SetYPartSize(partID, packedSize);
	}
	unsigned int uSize = m_uTask->GetPackedDataSize();
	unsigned int vSize = m_vTask->GetPackedDataSize();


	m_currentHeader->SetUSize(uSize);
	m_currentHeader->SetYUVSize(aWPacker->GetChannelSize(CHANNEL_Y) + aWPacker->GetChannelSize(CHANNEL_U) + aWPacker->GetChannelSize(CHANNEL_V));
	m_currentHeader->SetWidth(aWPacker->GetWidth());
	m_currentHeader->SetHeight(aWPacker->GetHeight());
	m_currentHeader->SetQuality(aWPacker->GetQuality());

	unsigned char* packedData = m_packedData + m_currentHeader->GetHeaderSize();

	for (unsigned char partID = 0; partID < m_currPartCount; partID++)
	{
		memcpy(packedData, m_yTask[partID]->GetPackedData(), m_yTask[partID]->GetPackedDataSize());
		packedData += m_yTask[partID]->GetPackedDataSize();
	}
	memcpy(packedData, m_uTask->GetPackedData(), uSize);
	packedData += uSize;
	memcpy(packedData, m_vTask->GetPackedData(), vSize);

	m_packedSize = m_currentHeader->GetHeaderSize() + ySize + uSize + vSize;
}

bool CZLibWaveletPacker::ZlibThreadWaiting()
{
	bool isWaiting = false;

	isWaiting = false;

	if ((!m_uTask->IsPerformed()) || (!m_vTask->IsPerformed()))
	{
		isWaiting = true;
	}

	for (unsigned char partID = 0; partID < m_currPartCount; partID++)
		if (!m_yTask[partID]->IsPerformed())
		{
			isWaiting = true;
		}

	return isWaiting;
}

void CZLibWaveletPacker::DestroyResultOnEndForTask(CZLibPackTask* aTask)
{
	if (aTask)
		if ((aTask->IsPerformed()) || (m_taskManager->RemoveTask(aTask)))
		{
			SAFE_DELETE(aTask);
		}
		else
		{
			aTask->DestroyResultOnEnd();
		}
}

unsigned char* CZLibWaveletPacker::GetPackedData()
{
	return m_packedData;
}

unsigned int CZLibWaveletPacker::GetPackedDataSize()
{
	return m_packedSize;
}

unsigned char* CZLibWaveletPacker::GetEasyPackedData()
{
	CWaveletHeader header;
	header.SetYPartCount(0);
	unsigned int headerSize = header.GetHeaderSize();
	unsigned int size = GetChannelSize(CHANNEL_Y) + GetChannelSize(CHANNEL_U) + GetChannelSize(CHANNEL_V) + headerSize;
	AllocateBufferIfNeeded(0, size);
	header.SetDataPointer(m_easyPackedData);
	header.SetYPartCount(0);
	header.SetUSize(0);
	header.SetYUVSize(GetChannelSize(CHANNEL_Y) + GetChannelSize(CHANNEL_U) + GetChannelSize(CHANNEL_V));
	header.SetWidth(GetWidth());
	header.SetHeight(GetHeight());
	header.SetQuality(GetQuality());
	unsigned char* packedData = m_easyPackedData + headerSize;
	memcpy(packedData, GetChannel(CHANNEL_EASY_Y), GetChannelSize(CHANNEL_Y));
	packedData += GetChannelSize(CHANNEL_Y);
	memcpy(packedData, GetChannel(CHANNEL_EASY_U), GetChannelSize(CHANNEL_U));
	packedData += GetChannelSize(CHANNEL_U);
	memcpy(packedData, GetChannel(CHANNEL_EASY_V), GetChannelSize(CHANNEL_V));
	m_easyPackedSize = size;
	
	return m_easyPackedData;
}
unsigned int CZLibWaveletPacker::GetEasyPackedDataSize()
{
	return m_easyPackedSize;
}

void CZLibWaveletPacker::AllocateBufferIfNeeded(unsigned int size, unsigned int easyBufferSize)
{
	if (size > m_allocatedSize && size != 0)
	{
		if (m_packedData)
		{
			MP_DELETE_ARR( m_packedData);
		}

		m_packedData = MP_NEW_ARR( unsigned char, size);
		m_allocatedSize = size;
		m_packedSize = 0;
	}
	if (easyBufferSize > m_allocatedEasyPackedSize && easyBufferSize != 0)
	{
		if (m_easyPackedData)
		{
			MP_DELETE_ARR( m_easyPackedData);
		}

		m_easyPackedData = MP_NEW_ARR( unsigned char, easyBufferSize);
		m_allocatedEasyPackedSize = easyBufferSize;
		m_easyPackedSize = 0;
	}
}

void CZLibWaveletPacker::StopPacking()
{
	m_needStop = true;
}

void CZLibWaveletPacker::CleanUp()
{
	if (m_packedData)
	{
		MP_DELETE_ARR( m_packedData);
	}

	m_packedSize = 0;
	m_allocatedSize = 0;

	if (m_easyPackedData)
	{
		MP_DELETE_ARR( m_easyPackedData);
	}

	if (!m_threadStopByNeedStop) {
		if (m_uTask)
			SAFE_DELETE(m_uTask);
		if (m_vTask)
			SAFE_DELETE(m_vTask);
		for (int i=0; i < MAX_PACK_TASKS; i++) {
			if (m_yTask[i]) {
				SAFE_DELETE(m_yTask[i]);
			}
		}
	}
	if (m_currentHeader)
		SAFE_DELETE(m_currentHeader);
	m_allocatedEasyPackedSize = 0;
	m_easyPackedSize = 0;
}

CZLibWaveletPacker::~CZLibWaveletPacker()
{
	CleanUp();
}