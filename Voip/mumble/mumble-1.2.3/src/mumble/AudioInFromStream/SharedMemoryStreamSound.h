#pragma once

#include "StdAfx.h"

#include <mmsystem.h>
#include "StreamSound.h"
#include "../../../../../../Common/SharedMemory.h"

class SharedMemoryStreamSound: public StreamSound
{
public:
#ifdef QT_DLL
	SharedMemoryStreamSound(QString aStreamID);
#else
	SharedMemoryStreamSound(std::wstring aStreamID);
#endif
	~SharedMemoryStreamSound(void);


	virtual HRESULT CreateCaptureBuffer(int aSize);
	virtual void DestroyCaptureBuffer();

	virtual void CopyAudioDataBufferForRead();
	virtual int SharedMemoryStreamSound::GetAvailableSize();
protected:
	virtual byte* GetAudioDataBuffer();
	virtual void LockAudioDataBufferForWrite();
	virtual void UnlockAudioDataBufferForWrite();

	virtual void SetCurrentWritePosValue(int aValue);
	
private:
	ISharedMemoryBlock* m_sharedMemBlock;
	int* m_sharedMemoryCurrentWritePos;
	byte* m_copyAudioDataBuffer;
};
