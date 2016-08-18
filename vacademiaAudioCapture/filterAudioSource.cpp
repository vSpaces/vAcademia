
#include <streams.h>
#include <source.h>
#include <olectl.h>
#include <initguid.h>
#include "filterAudioSource.h"
#include "ProcessFinder.h"
#include "../Voip/mumble/mumble-1.2.3/src/mumble/AudioInFromStream/SharedMemoryStreamSound.h"
#include "../Common/AudioMixer.h"


#include <string>

#define FILTER_NAME		L"vAcademiaAudioCapture"

#undef NAME
#define NAME(x) (x)

#pragma warning(disable:4710)  

DEFINE_GUID(CLSID_vAcademiaAudioCapture,
0xfd641041, 0x8eba, 0x1ace, 0x91, 0x86, 0x01, 0xba, 0x10, 0x56, 0x8d, 0xa4);

const AMOVIESETUP_MEDIATYPE sudOpPinTypes =
{
    &MEDIATYPE_Audio,       // Major type
    &MEDIASUBTYPE_PCM     // Minor type 
};

const AMOVIESETUP_PIN sudOpPin =
{
    L"Output",              // Pin string name
    FALSE,                  // Is it rendered
    TRUE,                   // Is it an output
    FALSE,                  // Can we have none
    FALSE,                  // Can we have many
    &CLSID_NULL,            // Connects to filter
    L"",                   // Connects to pin
    1,                      // Number of types
    &sudOpPinTypes };       // Pin details

const AMOVIESETUP_FILTER sudBallax =
{
    &CLSID_vAcademiaAudioCapture,    // Filter CLSID
    FILTER_NAME,       // String name
    MERIT_NORMAL,       // Filter merit
    1,                      // Number pins
    &sudOpPin               // Pin details
};

CFactoryTemplate g_Templates[] = {
  { FILTER_NAME
  , &CLSID_vAcademiaAudioCapture
  , CVAcademiaCaptureFilter::CreateInstance
  , NULL
  , &sudBallax }
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

REGFILTER2 rf2FilterReg = {
    1,              // Version 1 (no pin mediums or pin category).
    MERIT_DO_NOT_USE,   // Merit.
    1,              // Number of pins.
    &sudOpPin        // Pointer to pin information.
};

STDAPI DllRegisterServer(void)
{
    HRESULT hr;
    IFilterMapper2 *pFM2 = NULL;

    hr = AMovieDllRegisterServer2(TRUE);
    if (FAILED(hr))
        return hr;

    hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC_SERVER,
            IID_IFilterMapper2, (void **)&pFM2);

    if (FAILED(hr))
        return hr;

    hr = pFM2->RegisterFilter(
        CLSID_vAcademiaAudioCapture,                // Filter CLSID. 
        FILTER_NAME,                       // Filter name.
        NULL,                            // Device moniker. 
        &CLSID_AudioInputDeviceCategory,  // Video compressor category.
		NULL,                       // Instance data.
        &rf2FilterReg                    // Pointer to filter information.
    );
    pFM2->Release();
    return hr;
}

STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2(FALSE);

} 

extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  dwReason, 
                      LPVOID lpReserved)
{
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}

CUnknown * WINAPI CVAcademiaCaptureFilter::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
    ASSERT(phr);
    CUnknown *punk = new CVAcademiaCaptureFilter(lpunk, phr);
    if(punk == NULL)
    {
        if(phr)
            *phr = E_OUTOFMEMORY;
    }
    return punk;

} // CreateInstance


CVAcademiaCaptureFilter::CVAcademiaCaptureFilter(LPUNKNOWN lpunk, HRESULT *phr) :
    CSource(NAME(FILTER_NAME), lpunk, CLSID_vAcademiaAudioCapture)
{
    ASSERT(phr);
    CAutoLock cAutoLock(&m_cStateLock);

    m_paStreams = (CSourceStream **) new CVAcademiaStream*[1];
    if(m_paStreams == NULL)
    {
        if(phr)
            *phr = E_OUTOFMEMORY;

        return;
    }
/*
	HWND vaWnd = NULL;

	CProcessWindowFinder	finder;		
	SFindedProcess process = finder.GetPlayerWindow("", "player.exe");
	if (process.window != NULL)
	{
		vaWnd = process.window;		
	}*/
	
    m_paStreams[0] = new CVAcademiaStream(phr, this, L"vAcademiaAudioCapture!", NULL);
    if(m_paStreams[0] == NULL)
    {
        if(phr)
            *phr = E_OUTOFMEMORY;

        return;
    }

} 

CVAcademiaStream::CVAcademiaStream(HRESULT *phr,
                         CVAcademiaCaptureFilter *pParent,
                         LPCWSTR pPinName, HWND vaWnd) :
    CSourceStream(NAME(FILTER_NAME),phr, pParent, pPinName),
    m_iDefaultRepeatTime(1)
{
    ASSERT(phr);
    CAutoLock cAutoLock(&m_cSharedState);

    SharedMemoryStreamSound* streamSound = new SharedMemoryStreamSound(L"vacademiaMic");
	streamSound->CreateCaptureBuffer(-1);
	m_captureObjects.push_back(streamSound);
	streamSound = new SharedMemoryStreamSound(L"vacademiaVoiceOut");
	streamSound->CreateCaptureBuffer(-1);
	m_captureObjects.push_back(streamSound);
	streamSound = new SharedMemoryStreamSound(L"vacademiaAudiere");
	streamSound->CreateCaptureBuffer(-1);
	m_captureObjects.push_back(streamSound);
	
	m_fillCnt = 0;
	m_startTime = timeGetTime();

	m_vaWnd = vaWnd;
	m_waitDataCnt = 0;
	m_isFirstFill = true;
	for (int i=0; i < m_captureObjects.size(); i++) 
	{
		if (m_captureObjects[i] == NULL)
		{
			if(phr)
				*phr = E_OUTOFMEMORY;
		}
	}
} 


//
// Destructor
//
CVAcademiaStream::~CVAcademiaStream()
{
    CAutoLock cAutoLock(&m_cSharedState);
	for (int i=0; i < m_captureObjects.size(); i++) 
	{
        delete m_captureObjects[i];
	}
	m_captureObjects.clear();

} // (Destructor)

HRESULT CVAcademiaStream::Set(REFGUID guidPropSet, DWORD dwID,
    void *pInstanceData, DWORD cbInstanceData, void *pPropData, 
    DWORD cbPropData)
{
    return E_NOTIMPL;
}

HRESULT CVAcademiaStream::Get(
    REFGUID guidPropSet,   // Which property set.
    DWORD dwPropID,        // Which property in that set.
    void *pInstanceData,   // Instance data (ignore).
    DWORD cbInstanceData,  // Size of the instance data (ignore).
    void *pPropData,       // Buffer to receive the property data.
    DWORD cbPropData,      // Size of the buffer.
    DWORD *pcbReturned     // Return the size of the property.
)
{
    if (guidPropSet != AMPROPSETID_Pin) 
        return E_PROP_SET_UNSUPPORTED;
    if (dwPropID != AMPROPERTY_PIN_CATEGORY)
        return E_PROP_ID_UNSUPPORTED;
    if (pPropData == NULL && pcbReturned == NULL)
        return E_POINTER;
    if (pcbReturned)
        *pcbReturned = sizeof(GUID);
    if (pPropData == NULL)  // Caller just wants to know the size.
        return S_OK;
    if (cbPropData < sizeof(GUID)) // The buffer is too small.
        return E_UNEXPECTED;
    *(GUID *)pPropData = PIN_CATEGORY_CAPTURE;
    return S_OK;
}

HRESULT CVAcademiaStream::QuerySupported(REFGUID guidPropSet, DWORD dwPropID,
    DWORD *pTypeSupport)
{
    if (guidPropSet != AMPROPSETID_Pin)
        return E_PROP_SET_UNSUPPORTED;
    if (dwPropID != AMPROPERTY_PIN_CATEGORY)
        return E_PROP_ID_UNSUPPORTED;
    if (pTypeSupport)
        // We support getting this property, but not setting it.
        *pTypeSupport = KSPROPERTY_SUPPORT_GET; 
    return S_OK;
}

bool CVAcademiaStream::StreamHaveEnoughData(SharedMemoryStreamSound* aStream, long aNeedLength)
{
	int dwReadyBytes = 0;
	int dwReadPosition;
	int dwCapturePosition;
	int dwBufferSize = aStream->GetBufferSize();

	aStream->GetCurrentPosition(&dwCapturePosition, &dwReadPosition);
	if (dwReadPosition < aStream->GetLastreadPos())
		dwReadyBytes = (dwBufferSize - aStream->GetLastreadPos()) + dwReadPosition;
	else
		dwReadyBytes = dwReadPosition - aStream->GetLastreadPos();

	if (static_cast<int>(dwReadyBytes) >= aNeedLength) {
		return true;
	}

	return false;
}

void CVAcademiaStream::FillBufferFromStream(SharedMemoryStreamSound* aStream, BYTE* apData, long aDatasize)
{
	HRESULT hr;
	LPVOID aptr1, aptr2;
	int nbytes1, nbytes2;

	
	if (FAILED(hr = aStream->Lock(aStream->GetLastreadPos(), aDatasize, &aptr1, &nbytes1, &aptr2, &nbytes2))) {
		return;
	}

	short *psMicLocal = new short[aDatasize/2]; 
	if (aptr1 && nbytes1)
		CopyMemory(psMicLocal, aptr1, nbytes1);

	if (aptr2 && nbytes2)
		CopyMemory(psMicLocal+nbytes1/2, aptr2, nbytes2);

	aStream->Unlock(aptr1, nbytes1, aptr2, nbytes2);

	aStream->SetLastReadPos((aStream->GetLastreadPos() + aDatasize) % aStream->GetBufferSize());

	BYTE *inputDataCopy = new BYTE[aDatasize];
	memcpy(inputDataCopy, apData, aDatasize);

	const short** bufferList = new const short*[2];
	bufferList[0] = psMicLocal;
	bufferList[1] = (const short*)inputDataCopy;
	
	CAudioMixer::MixBuffers(bufferList, 2, aDatasize/2, (short*)apData);
	delete[] bufferList;
	delete[] inputDataCopy;
	delete[] psMicLocal;
}

HRESULT CVAcademiaStream::FillBuffer(IMediaSample *pms)
{
    CheckPointer(pms,E_POINTER);

	BYTE *pData;
    long lDataLen;

    pms->GetPointer(&pData);
    lDataLen = pms->GetSize();

	CRefTime rtStart = m_rtSampleTime;
	int waitTime = m_timer.elapsed();

	while (!m_isFirstFill && m_iRepeatTime*1000 - waitTime > 0)
	{
		Sleep(1);
		waitTime = m_timer.elapsed();
	}

	ZeroMemory(pData, lDataLen);

	for (int i=0; i<m_captureObjects.size(); i++ )
	{
		SharedMemoryStreamSound* captureObject = m_captureObjects[i];
		bool haveEnoughForCapture = false;
		captureObject->CopyAudioDataBufferForRead();

		haveEnoughForCapture = StreamHaveEnoughData(captureObject, lDataLen);

		if (haveEnoughForCapture)
		{
			FillBufferFromStream(captureObject, pData, lDataLen);
		}	
		else
		{
			OutputDebugString(L"not haveEnoughForCapture\n");
		}
	}

	m_fillCnt++;
	int currTime = timeGetTime();
	int deltaTime = currTime - m_startTime;

	// Increment to find the finish time
	if (m_isFirstFill)
		m_rtSampleTime += (LONG)m_iRepeatTime;
	else 
	{
		m_rtSampleTime += (__int64)m_timer.elapsed()*10;
	}

    pms->SetTime((REFERENCE_TIME *) &rtStart,(REFERENCE_TIME *) &m_rtSampleTime);

    pms->SetSyncPoint(TRUE);
	
	m_isFirstFill = false;
	m_timer.restart();

    return NOERROR;
} // FillBuffer


//
// Notify
//
// Alter the repeat rate according to quality management messages sent from
// the downstream filter (often the renderer).  Wind it up or down according
// to the flooding level - also skip forward if we are notified of Late-ness
//
STDMETHODIMP CVAcademiaStream::Notify(IBaseFilter * pSender, Quality q)
{
    return NOERROR;
} // Notify


//
// GetMediaType
//

HRESULT CVAcademiaStream::GetMediaType(int iPosition, CMediaType *pmt)
{
    CheckPointer(pmt,E_POINTER);

    CAutoLock cAutoLock(m_pFilter->pStateLock());
    if(iPosition < 0)
    {
        return E_INVALIDARG;
    }

    // Have we run off the end of types?

    if(iPosition > 0)
    {
        return VFW_S_NO_MORE_ITEMS;
    }

    WAVEFORMATEX *pwf = (WAVEFORMATEX *) pmt->AllocFormatBuffer(sizeof(WAVEFORMATEX));
    if(NULL == pwf)
        return(E_OUTOFMEMORY);

    ZeroMemory(pwf, sizeof(WAVEFORMATEX));

    switch(iPosition)
    {
        case 0:
        {    
			pwf->wFormatTag = WAVE_FORMAT_PCM;
			pwf->nChannels = 1;
			pwf->nSamplesPerSec = 16000;
			pwf->wBitsPerSample = 16;
			pwf->nBlockAlign = pwf->nChannels * pwf->wBitsPerSample/8;
			pwf->nAvgBytesPerSec = pwf->nBlockAlign*pwf->nSamplesPerSec;
			pwf->cbSize = 0;
            break;
        }
    }

    pmt->SetType(&MEDIATYPE_Audio);
    pmt->SetFormatType(&FORMAT_WaveFormatEx);
    pmt->SetTemporalCompression(FALSE);

    // Work out the GUID for the subtype from the header info.
    const GUID SubTypeGUID = MEDIASUBTYPE_PCM;
    pmt->SetSubtype(&SubTypeGUID);
    pmt->SetSampleSize(pwf->nBlockAlign);

    return NOERROR;

} // GetMediaType


//
// CheckMediaType
//

HRESULT CVAcademiaStream::CheckMediaType(const CMediaType *pMediaType)
{
    CheckPointer(pMediaType,E_POINTER);


    return S_OK;  // This format is acceptable.

} // CheckMediaType


//
// DecideBufferSize
//
// This will always be called after the format has been sucessfully
// negotiated. So we have a look at m_mt to see what size image we agreed.
// Then we can ask for buffers of the correct size to contain them.
//
HRESULT CVAcademiaStream::DecideBufferSize(IMemAllocator *pAlloc,
                                      ALLOCATOR_PROPERTIES *pProperties)
{
	CheckPointer(pAlloc,E_POINTER);
    CheckPointer(pProperties,E_POINTER);

    CAutoLock cAutoLock(m_pFilter->pStateLock());
    HRESULT hr = NOERROR;

	float framesPerSecond = 1;

    WAVEFORMATEX *pwf = (WAVEFORMATEX *) m_mt.Format();
    pProperties->cBuffers = 1;
    pProperties->cbBuffer = pwf->nAvgBytesPerSec / framesPerSecond;

	m_iRepeatTime = 1000 / framesPerSecond;

    ASSERT(pProperties->cbBuffer);

    // Ask the allocator to reserve us some sample memory, NOTE the function
    // can succeed (that is return NOERROR) but still not have allocated the
    // memory that we requested, so we must check we got whatever we wanted

    ALLOCATOR_PROPERTIES Actual;
    hr = pAlloc->SetProperties(pProperties,&Actual);
    if(FAILED(hr))
    {
        return hr;
    }

    // Is this allocator unsuitable

    if(Actual.cbBuffer < pProperties->cbBuffer)
    {
        return E_FAIL;
    }
    // Make sure that we have only 1 buffer (we erase the ball in the
    // old buffer to save having to zero a 200k+ buffer every time
    // we draw a frame)

    ASSERT(Actual.cBuffers == 1);
    return NOERROR;

} // DecideBufferSize


//
// SetMediaType
//
// Called when a media type is agreed between filters
//
HRESULT CVAcademiaStream::SetMediaType(const CMediaType *pMediaType)
{
    CAutoLock cAutoLock(m_pFilter->pStateLock());

	// Pass the call up to my base class

    HRESULT hr = CSourceStream::SetMediaType(pMediaType);
	
	WAVEFORMATEX *pwf = (WAVEFORMATEX *) pMediaType->Format();

    if(SUCCEEDED(hr))
    {
        return NOERROR;
    } 

    return hr;

} // SetMediaType


//
// OnThreadCreate
//
// As we go active reset the stream time to zero
//
HRESULT CVAcademiaStream::OnThreadCreate()
{
    CAutoLock cAutoLockShared(&m_cSharedState);
    m_rtSampleTime = 0;

    // we need to also reset the repeat time in case the system
    // clock is turned off after m_iRepeatTime gets very big
    //m_iRepeatTime = m_iDefaultRepeatTime;

    return NOERROR;

} // OnThreadCreate


HRESULT CVAcademiaStream::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IKsPropertySet) 
	{
		return GetInterface((IKsPropertySet *) this, ppv);
    } 
	else 
	{
		return CSourceStream::NonDelegatingQueryInterface(riid, ppv);
    }
}