#include "mumble_pch.hpp" 
#include "SharedMemoryStreamSound.h"

#ifdef QT_DLL
SharedMemoryStreamSound::SharedMemoryStreamSound(QString aStreamID):StreamSound(aStreamID)
#else
SharedMemoryStreamSound::SharedMemoryStreamSound(std::wstring aStreamID):StreamSound(aStreamID)
#endif
{
	m_sharedMemBlock = NULL;
	m_sharedMemoryCurrentWritePos = NULL;
	m_copyAudioDataBuffer = NULL;
	m_dataWasAdded = true;
	SetWithoutPreload();
}

SharedMemoryStreamSound::~SharedMemoryStreamSound(void)
{
	if (m_copyAudioDataBuffer)
		delete[] m_copyAudioDataBuffer;
}

void SharedMemoryStreamSound::DestroyCaptureBuffer()
{
	
}

byte* SharedMemoryStreamSound::GetAudioDataBuffer()
{
	return m_audioDataBuffer;
}

void SharedMemoryStreamSound::CopyAudioDataBufferForRead()
{
	LockAudioDataBufferForWrite();
	if (m_audioDataBuffer == NULL)
	{
		UnlockAudioDataBufferForWrite();
		return;
	}
	if (m_copyAudioDataBuffer == NULL)
		m_copyAudioDataBuffer = new byte[m_audioDataBufferSize];

	memcpy(m_copyAudioDataBuffer, m_audioDataBuffer, m_audioDataBufferSize);

	UnlockAudioDataBufferForWrite();
	m_audioDataBuffer = m_copyAudioDataBuffer;
}

void SharedMemoryStreamSound::LockAudioDataBufferForWrite()
{
	if (!m_sharedMemBlock)
		return;

	byte* dataBuffer = (byte*)m_sharedMemBlock->GetPointer();
	if (dataBuffer == NULL)
		return;

	m_audioDataBuffer = dataBuffer+sizeof(int)*2;

	m_sharedMemoryCurrentWritePos = (int*)dataBuffer;
	
	m_currentWritePos = m_sharedMemoryCurrentWritePos[0];
	m_currentAddDataCnt = m_sharedMemoryCurrentWritePos[1];
}

void SharedMemoryStreamSound::UnlockAudioDataBufferForWrite()
{
	if (!m_sharedMemBlock)
		return;

	if (m_audioDataBuffer == NULL)
		return;
	m_sharedMemBlock->FreePointer();
	m_audioDataBuffer = NULL;
	m_sharedMemoryCurrentWritePos = NULL;
}

void SharedMemoryStreamSound::SetCurrentWritePosValue(int aValue)
{
	if (m_sharedMemoryCurrentWritePos != NULL)
	{
		m_sharedMemoryCurrentWritePos[0] = aValue % GetBufferSize();
		m_sharedMemoryCurrentWritePos[1] = m_sharedMemoryCurrentWritePos[1] + 1;
		//QString s = "SetCurrentWritePosValue" + QString::number(m_sharedMemoryCurrentWritePos[1]) + " aValue " + QString::number(aValue);
		//qWarning(s.toStdString().c_str());
	}
}

HRESULT SharedMemoryStreamSound::CreateCaptureBuffer(int aSize)
{
	if (aSize == 0)
		return DSERR_INVALIDPARAM;

	WaitForSingleObject(m_lockerMutex, INFINITE);
	if (m_cleared)
		return S_OK;
	
	DestroyCaptureBuffer();
	if (aSize == -1)
	{
#ifdef QT_DLL
		m_sharedMemBlock = CSharedMemory::GetBlock(m_streamID.toStdWString().c_str());
#else
		m_sharedMemBlock = CSharedMemory::GetBlock(m_streamID.c_str());
#endif
	}
	else 
	{
#ifdef QT_DLL
		m_sharedMemBlock = CSharedMemory::CreateBlock(m_streamID.toStdWString().c_str(), aSize+sizeof(int)*2);
#else
		m_sharedMemBlock = CSharedMemory::CreateBlock(m_streamID.c_str(), aSize+sizeof(int)*2);
#endif
	}
	if (!m_sharedMemBlock)
		return DSERR_INVALIDPARAM;
	
	m_audioDataBuffer = NULL;
	if (aSize == -1)
		m_audioDataBufferSize = m_sharedMemBlock->GetSize()-sizeof(int)*2;
	else
		m_audioDataBufferSize = aSize;
	if (m_audioDataBufferSize < 0)
		m_audioDataBufferSize = 0;
	LockAudioDataBufferForWrite();
	if (aSize != -1) {
		SetCurrentWritePosValue(0);
	}
	if (m_sharedMemoryCurrentWritePos)
		m_sharedMemoryCurrentWritePos[1] = 0;
	UnlockAudioDataBufferForWrite();

	m_captureBufferCreated = true;
	ReleaseMutex(m_lockerMutex);
	return S_OK;
}

int SharedMemoryStreamSound::GetAvailableSize()
{
	if (!m_sharedMemBlock)
		CreateCaptureBuffer(-1);
	CopyAudioDataBufferForRead();
	
/*	QString s =  "m_currentAddDataCnt " + QString::number(m_currentAddDataCnt) 
		+ "m_lastAddDataCnt " + QString::number(m_lastAddDataCnt) + "GetLastreadPos() " +QString::number( GetLastreadPos()) 
		+ "m_currentWritePos " +  QString::number(m_currentWritePos) ;
	*/
	int res = StreamSound::GetAvailableSize();
	/*s = "res " + QString::number(res) + s;
	qWarning(s.toStdString().c_str());*/
	return res;
}