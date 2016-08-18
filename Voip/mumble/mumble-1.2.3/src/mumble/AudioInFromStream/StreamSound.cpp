#include "mumble_pch.hpp" 
#include "stdafx.h"
#include "StreamSound.h"

typedef enum _SoundStreamCommand
{
	SOUND_PAUSE = 0,
	SOUND_STOP = 1,
	SOUND_REWIND = 2,
	SOUND_SEEK = 3,
	SOUND_RESUME = 4,
	SOUND_FORCE_STOP = 5,
} SoundStreamCommand;

#ifdef QT_DLL
StreamSound::StreamSound(QString aStreamID)
#else
StreamSound::StreamSound(std::wstring aStreamID)
#endif
{
	m_audioDataBuffer = NULL;

	m_audioDataBufferSize = 0;
	m_currentWritePos = 0;
	m_dataWasAdded = false;

	m_lockerMutex = CreateMutex(NULL, FALSE, NULL);

	m_streamID = aStreamID;
	m_cleared = false;
	m_captureBufferCreated = false;

	m_lastReadPos = 0;
	m_preloaded = false;
	m_addedDataSizeForPreload = 0;
	m_timeFirstLock = 0;
	m_firstLock = true;
	m_lastAddDataCnt = 0;
	m_currentAddDataCnt = 0;

	m_paused = false;
}

StreamSound::~StreamSound(void)
{
	CloseHandle(m_lockerMutex);

	DestroyCaptureBuffer();
}

byte* StreamSound::GetAudioDataBuffer()
{
	return m_audioDataBuffer;
}

void StreamSound::DestroyCaptureBuffer()
{
	if (m_audioDataBuffer != NULL)
		delete[] m_audioDataBuffer;
	m_audioDataBuffer = NULL;
}

#ifdef QT_DLL
QString StreamSound::GetStreamID()
#else
std::wstring StreamSound::GetStreamID()
#endif
{
	return m_streamID;
}

int StreamSound::GetBufferSize()
{
	return m_audioDataBufferSize;
}

HRESULT StreamSound::CreateCaptureBuffer(int aSize)
{
	if (aSize == 0)
		return DSERR_INVALIDPARAM;
	WaitForSingleObject(m_lockerMutex, INFINITE);
	if (m_cleared)
		return S_OK;
	
	DestroyCaptureBuffer();
	m_audioDataBuffer = new byte[aSize];
	m_audioDataBufferSize = aSize;

	m_currentWritePos = 0;
	m_captureBufferCreated = true;
	ReleaseMutex(m_lockerMutex);
	return S_OK;
}

void StreamSound::Clear()
{
	WaitForSingleObject(m_lockerMutex, INFINITE);
#ifdef QT_DLL
	m_streamID = QString();
#else
	m_streamID = L"";
#endif
	DestroyCaptureBuffer();
	m_cleared = true;
	ReleaseMutex(m_lockerMutex);
}

bool StreamSound::IsCleared()
{
	return m_cleared;
}

bool StreamSound::IsCaptureBufferCreated()
{
	bool res = true;
	WaitForSingleObject(m_lockerMutex, INFINITE);
	
	res = m_captureBufferCreated;

	ReleaseMutex(m_lockerMutex);
	return res;
}

HRESULT StreamSound::GetCurrentPosition(int* aCapturePos, int* aReadPos)
{
	WaitForSingleObject(m_lockerMutex, INFINITE);
	*aReadPos = m_currentWritePos;
	*aCapturePos = 0;
	ReleaseMutex(m_lockerMutex);
	return S_OK;
}

HRESULT StreamSound::Lock(DWORD aPosition, int aSize, void** apBuffer1, int* aBufferSize1, void** apBuffer2, int* aBufferSize2)
{
	//*apBuffer1 - указатель на участок в конце буфера, *apBuffer2 на участок в начале буфера
	WaitForSingleObject(m_lockerMutex, INFINITE);
	if (m_paused)
	{
		ReleaseMutex(m_lockerMutex);
		return DSERR_INVALIDCALL;
	}
	if (m_firstLock)
	{
		m_timeFirstLock = timeGetTime();
		m_firstLock = false;
	}
	if (timeGetTime() - m_timeFirstLock > 1000)
		m_preloaded = true;

	if (m_cleared || !m_preloaded)
	{
		ReleaseMutex(m_lockerMutex);
		return DSERR_INVALIDCALL;
	}
	
	if (!GetAudioDataBuffer()) {
		ReleaseMutex(m_lockerMutex);
		return DSERR_INVALIDCALL;
	}
	if (!m_dataWasAdded)
	{ //данных еще нет
		*apBuffer1 = NULL;
		*aBufferSize1 = 0;
		*apBuffer2 = NULL;
		*aBufferSize2 = 0;
		ReleaseMutex(m_lockerMutex);

		return S_OK;
	}
	if (aSize > m_audioDataBufferSize)
	{
		ReleaseMutex(m_lockerMutex);
		return DSERR_INVALIDPARAM;
	}
	int requestPosition = aPosition + aSize;


	if ((int)aPosition >= m_currentWritePos) 
	{
		int sizeToEnd = m_audioDataBufferSize - aPosition;
		if (aSize > sizeToEnd) // запрашиваемые данные находятся в конце буфера и в начале
		{
			*apBuffer1 = GetAudioDataBuffer() + aPosition;
			*aBufferSize1 = sizeToEnd;

			int sizeFromBegin = aSize - sizeToEnd;
			if (sizeFromBegin > m_currentWritePos)
			{
				// aSize больше новых данных в буфере
				ReleaseMutex(m_lockerMutex);
				return DSERR_INVALIDPARAM;
			}

			*apBuffer2 = GetAudioDataBuffer();
			*aBufferSize2 = sizeFromBegin;
		}
		else // запрашиваемые данные находятся до окончания буфера
		{
			*apBuffer1 = GetAudioDataBuffer() + aPosition;
			*aBufferSize1 = aSize;
			*apBuffer2 = NULL;
			*aBufferSize2 = 0;
		}
	}
	else // текущая позиция записи в буфер после запрашиваемых данных 
	{
		if (aSize > m_currentWritePos - (int)aPosition)
		{
			//aSize больше новых данных в буфере
			ReleaseMutex(m_lockerMutex);
			return DSERR_INVALIDPARAM;
		}
		*apBuffer1 = GetAudioDataBuffer() + aPosition;
		*aBufferSize1 = aSize;
		*apBuffer2 = NULL;
		*aBufferSize2 = 0;
	}
	return S_OK;
}

HRESULT StreamSound::Unlock(void* apBuffer1, int aBufferSize1, void* apBuffer2, int aBufferSize2)
{
	ReleaseMutex(m_lockerMutex);
	
	return S_OK;
}

bool StreamSound::AddData(void* apData, int aDataSize)
{
	//QString strLog = QLatin1String("add data ")+QString::number(aDataSize) ;
	//qWarning(strLog.toStdString().c_str());
	
	//m_audioDataBuffer - циклический буфер
	WaitForSingleObject(m_lockerMutex, INFINITE);
	if (m_cleared)
	{
		ReleaseMutex(m_lockerMutex);
		return false;
	}
	LockAudioDataBufferForWrite();
	if (!GetAudioDataBuffer())
	{
#ifdef QT_DLL
	qWarning("!GetAudioDataBuffer"); 
#endif
		UnlockAudioDataBufferForWrite();
		ReleaseMutex(m_lockerMutex);
		return false;
	}

	if (aDataSize > 0)
		m_dataWasAdded = true;
	if (aDataSize > m_audioDataBufferSize)
	{
		apData = ((byte*)apData) + (aDataSize - m_audioDataBufferSize);
		aDataSize = m_audioDataBufferSize;
	}
	int sizeToEnd = m_audioDataBufferSize - m_currentWritePos;
	if (sizeToEnd < aDataSize) //данные в конце и в начале буфера
	{
		memcpy(GetAudioDataBuffer()+m_currentWritePos, apData, sizeToEnd);
		int sizeFromBegin = aDataSize - sizeToEnd;
		memcpy(GetAudioDataBuffer(), ((byte*)apData)+sizeToEnd, sizeFromBegin);
		SetCurrentWritePosValue(sizeFromBegin);
	}
	else
	{
		memcpy(GetAudioDataBuffer()+m_currentWritePos, apData, aDataSize);
		SetCurrentWritePosValue(m_currentWritePos + aDataSize);
	}
	m_currentAddDataCnt++;

	if (!m_preloaded)
	{
		m_addedDataSizeForPreload += aDataSize;
	}

	if (!m_preloaded && m_addedDataSizeForPreload >= 160 * sizeof(short) * 100)
	{
		m_preloaded = true;
	}
	else if (m_audioDataBufferSize < 160 * sizeof(short) * 100)
	{
		//qWarning("preloading size bigger then m_audioDataBufferSize");
		m_preloaded = true;
	}

	if (m_currentWritePos == m_audioDataBufferSize)
		SetCurrentWritePosValue(0);
	UnlockAudioDataBufferForWrite();
	ReleaseMutex(m_lockerMutex);

	return true;
}

void StreamSound::SetCurrentWritePosValue(int aValue)
{
	m_currentWritePos = aValue % GetBufferSize();
}


void StreamSound::SetLastReadPos(int aPos)
{
	m_lastReadPos = aPos % GetBufferSize();
}

int StreamSound::GetLastreadPos()
{
	return m_lastReadPos;
}

void StreamSound::StartPreload(bool anWithClear)
{
	WaitForSingleObject(m_lockerMutex, INFINITE);

	m_preloaded = false;
	m_addedDataSizeForPreload = 0;
	m_timeFirstLock = 0;
	m_firstLock = true;
	ReleaseMutex(m_lockerMutex);
	if (anWithClear)
	{
		m_currentWritePos = 0;
		m_lastReadPos = 0;
		m_dataWasAdded = false;
	}
}

bool StreamSound::ProcessCommand(int aCommandID)
{
	switch (aCommandID)
	{
		case SOUND_PAUSE: 
		{
			//qWarning("pause");
			m_paused = true;
		}
		break;
		case SOUND_STOP: 
		{
			//qWarning("stop");
		}
		break;
		case SOUND_REWIND: 
		{
			//qWarning("rewind");
			StartPreload(true);
		}
		break;
		case SOUND_SEEK: 
		{
			//qWarning("seek");
			StartPreload(true);
		}
		break;
		case SOUND_RESUME:
		{
			//qWarning("resume");
			m_paused = false;
		} 
		break;
		case SOUND_FORCE_STOP:
		{
			//qWarning("force stop");
			StartPreload(true);
		} 
		break;
		default: 
		{
		}break;
	}
	return true;
}

void StreamSound::FillBufferBeforePositionAndSize(BYTE* apData, long &aDatasize, long aPosition)
{
	//QString s = "FillBufferBeforePositionAndSize " + QString::number(aPosition)  + "   " + QString::number(GetBufferSize()) + "   "+ QString::number(aDatasize);
	//qWarning(s.toStdString().c_str());
	if (aPosition >= GetBufferSize())
		return;
	if (aPosition < 0)
		return;

	if (aPosition > GetLastreadPos())
		aDatasize = aPosition - GetLastreadPos();
	else
		aDatasize = GetBufferSize() - GetLastreadPos() + aPosition;
	//s = "FillBufferBeforePositionAndSize " + QString::number(aDatasize) ;
	//qWarning(s.toStdString().c_str());
	return FillBufferFromStream(apData, aDatasize);
}

void StreamSound::FillBufferFromStream(BYTE* apData, long aDatasize)
{
	HRESULT hr;
	LPVOID aptr1, aptr2;
	int nbytes1, nbytes2;

	//QString s = "FillBufferFromStream " + QString::number(GetLastreadPos()) + "  " + QString::number(m_currentWritePos);
	//qWarning(s.toStdString().c_str());
	if (FAILED(hr = Lock(GetLastreadPos(), aDatasize, &aptr1, &nbytes1, &aptr2, &nbytes2))) {
		//qWarning("error in get data");
		return;
	}
//	qWarning("FillBufferFromStream ");
	if (aptr1 && nbytes1)
		CopyMemory(apData, aptr1, nbytes1);

	if (aptr2 && nbytes2)
		CopyMemory(apData+nbytes1, aptr2, nbytes2);

	Unlock(aptr1, nbytes1, aptr2, nbytes2);

	SetLastReadPos(GetLastreadPos() + aDatasize);
}

int StreamSound::GetAvailableSize()
{
	//*apBuffer1 - указатель на участок в конце буфера, *apBuffer2 на участок в начале буфера
	WaitForSingleObject(m_lockerMutex, INFINITE);
	if (m_paused)
	{
		ReleaseMutex(m_lockerMutex);
		return 0;
	}
	if (m_firstLock)
	{
		m_timeFirstLock = timeGetTime();
		m_firstLock = false;
	}
	if (timeGetTime() - m_timeFirstLock > 1000)
		m_preloaded = true;

	if (m_cleared || !m_preloaded)
	{
		ReleaseMutex(m_lockerMutex);
		return 0;
	}
	
	if (!GetAudioDataBuffer()) {
		ReleaseMutex(m_lockerMutex);
		return 0;
	}
	if (!m_dataWasAdded)
	{ //данных еще нет
		ReleaseMutex(m_lockerMutex);
		return 0;
	}
	if (GetLastreadPos() == m_currentWritePos && m_currentAddDataCnt == m_lastAddDataCnt) {
		//забрали весь буфер и новых данных не было
		ReleaseMutex(m_lockerMutex);
		return 0;
	}
	m_lastAddDataCnt = m_currentAddDataCnt;

	if ((int)GetLastreadPos() >= m_currentWritePos) 
	{
		int sizeToEnd = m_audioDataBufferSize - GetLastreadPos();
		int sizeFromBegin = m_currentWritePos;

		ReleaseMutex(m_lockerMutex);
		return sizeToEnd + sizeFromBegin;
	}
	else // текущая позиция записи в буфер после запрашиваемых данных 
	{
		ReleaseMutex(m_lockerMutex);
		return m_currentWritePos - (int)GetLastreadPos();
	}
}

void StreamSound::SetWithoutPreload()
{
	m_preloaded = true;
}