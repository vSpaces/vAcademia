
#include "Ks.h"
#include "Ksproxy.h"
#include "../vacademiaAudioCapture/Timer.h"

class CVAcademiaStream;

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
    CVAcademiaStream(HRESULT *phr, CVAcademiaCaptureFilter *pParent, LPCWSTR pPinName, HWND hWnd, int width, int height);
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
	void SaveLastImage(unsigned char* data, unsigned int size);
	void RestoreLastImage(unsigned char* data, unsigned int size);

    int m_iImageHeight;                 // The current image height
    int m_iImageWidth;                  // And current image width
    int m_iRepeatTime;                  // Time in msec between frames
    const int m_iDefaultRepeatTime;     // Initial m_iRepeatTime

    PALETTEENTRY m_Palette[256];        // The optimal palette for the image

    CCritSec m_cSharedState;            // Lock on m_rtSampleTime and m_captureObject
    CRefTime m_rtSampleTime;            // The time stamp for each sample
    CVAcademiaCapture* m_captureObject;                     

	HWND m_vaWnd;

    enum Colour {Red, Blue, Green, Yellow};
    HRESULT SetPaletteEntries(Colour colour);

	unsigned char* m_lastImageData;
	unsigned int m_lastImageDataSize;

	bool m_isFirstFill;

	__int64 m_fillCnt;
	int m_startTime;
	Timer m_timer;
	Timer m_ReCaptureTimer;
}; 
    
