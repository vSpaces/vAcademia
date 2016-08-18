
#include "Ks.h"
#include "Ksproxy.h"
#include <vector>

#include "Timer.h"

class SharedMemoryStreamSound;

class CVAcademiaCaptureFilter : public CSource
{
public:
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

private:   
    CVAcademiaCaptureFilter(LPUNKNOWN lpunk, HRESULT *phr);
}; 

class CVAcademiaStream : public CSourceStream, public IKsPropertySet
{
public:
    CVAcademiaStream(HRESULT *phr, CVAcademiaCaptureFilter *pParent, LPCWSTR pPinName, HWND hWnd);
    ~CVAcademiaStream();

    HRESULT FillBuffer(IMediaSample *pms);

    HRESULT DecideBufferSize(IMemAllocator *pIMemAlloc,
                             ALLOCATOR_PROPERTIES *pProperties);

    HRESULT SetMediaType(const CMediaType *pMediaType);

    HRESULT CheckMediaType(const CMediaType *pMediaType);
    HRESULT GetMediaType(int iPosition, CMediaType *pmt);

    HRESULT OnThreadCreate(void);

    STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);

	STDMETHOD(Set)(REFGUID guidPropSet, DWORD dwID,
		void *pInstanceData, DWORD cbInstanceData, void *pPropData, 
		DWORD cbPropData);

	STDMETHOD(Get)(
		REFGUID guidPropSet,   // Which property set.
		DWORD dwPropID,        // Which property in that set.
		void *pInstanceData,   // Instance data (ignore).
		DWORD cbInstanceData,  // Size of the instance data (ignore).
		void *pPropData,       // Buffer to receive the property data.
		DWORD cbPropData,      // Size of the buffer.
		DWORD *pcbReturned     // Return the size of the property.
	);


	STDMETHOD(QuerySupported)(REFGUID guidPropSet, DWORD dwPropID,
		DWORD *pTypeSupport);

	DECLARE_IUNKNOWN

	STDMETHOD(NonDelegatingQueryInterface)(REFIID riid, void **ppv);

private:
	bool StreamHaveEnoughData(SharedMemoryStreamSound* aStream, long aNeedLength);
	void FillBufferFromStream(SharedMemoryStreamSound* aStream, BYTE* apData, long aDatasize);

private:
    int m_iRepeatTime;                  // Time in msec between frames
    const int m_iDefaultRepeatTime;     // Initial m_iRepeatTime

    CCritSec m_cSharedState;            // Lock on m_rtSampleTime and m_captureObject
    CRefTime m_rtSampleTime;            // The time stamp for each sample
	std::vector<SharedMemoryStreamSound*> m_captureObjects;      

	HWND m_vaWnd;

	__int64 m_waitDataCnt;
	bool m_isFirstFill;

	__int64 m_fillCnt;
	int m_startTime;

	Timer m_timer;
}; 
    
