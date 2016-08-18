#include <mmsystem.h>
#include <dsound.h>

#ifdef QT_DLL
#include <QtCore/QString>
#else
#include <string>
#endif

class StreamSound
{
public:
#ifdef QT_DLL
	StreamSound(QString aStreamID);
#else
	StreamSound(std::wstring aStreamID);
#endif
	~StreamSound(void);

	virtual HRESULT CreateCaptureBuffer(int aSize);
	virtual void DestroyCaptureBuffer();
	HRESULT GetCurrentPosition(int* aCapturePos, int* aReadPos);
	HRESULT Lock(DWORD aPosition, int aSize, void** apBuffer1, int* aBufferSize1, void** apBuffer2, int* aBufferSize2);
	HRESULT Unlock(void* apBuffer1, int aBufferSize1, void* apBuffer2, int aBufferSize2);


	void FillBufferBeforePositionAndSize(BYTE* apData, long &aDatasize, long aPosition);

	void FillBufferFromStream(BYTE* apData, long aDatasize);
	virtual int GetAvailableSize();

	bool AddData(void* apData, int aDataSize);
	bool ProcessCommand(int aCommandID);
	int GetBufferSize();
	void SetWithoutPreload();
#ifdef QT_DLL
	QString GetStreamID();
#else
	std::wstring GetStreamID();
#endif
	bool IsCaptureBufferCreated();
	void Clear();
	bool IsCleared();

	void StartPreload(bool anWithClear = false);
public:
	void SetLastReadPos(int aPos);
	int GetLastreadPos();

	virtual void CopyAudioDataBufferForRead(){};

private:
	int m_lastReadPos;
	

protected:
	virtual byte* GetAudioDataBuffer();
	virtual void LockAudioDataBufferForWrite(){};
	virtual void UnlockAudioDataBufferForWrite(){};

	virtual void SetCurrentWritePosValue(int aValue);
protected:
	byte* m_audioDataBuffer;
	int m_audioDataBufferSize;
	bool m_captureBufferCreated;
	int m_currentWritePos;
	int m_lastAddDataCnt;
	int m_currentAddDataCnt;
	HANDLE m_lockerMutex;
	bool m_dataWasAdded;

	bool m_cleared;

#ifdef QT_DLL
	QString m_streamID;
#else
	std::wstring m_streamID;
#endif

private:
	bool m_preloaded;
	int m_addedDataSizeForPreload;
	long m_timeFirstLock;
	bool m_firstLock;
	bool m_paused;

};