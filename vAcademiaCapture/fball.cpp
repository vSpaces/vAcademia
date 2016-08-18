
#include <streams.h>
#include <olectl.h>
#include <initguid.h>
#include "ball.h"
#include "fball.h"
#include "ProcessFinder.h"

#define FILTER_NAME		L"vAcademiaCapture"

#undef NAME
#define NAME(x) (x)

#pragma warning(disable:4710)  

const AMOVIESETUP_MEDIATYPE sudOpPinTypes =
{
    &MEDIATYPE_Video,       // Major type
    &MEDIASUBTYPE_NULL     // Minor type 
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
    &CLSID_vAcademiaCapture,    // Filter CLSID
    FILTER_NAME,       // String name
    MERIT_NORMAL,       // Filter merit
    1,                      // Number pins
    &sudOpPin               // Pin details
};

CFactoryTemplate g_Templates[] = {
  { FILTER_NAME
  , &CLSID_vAcademiaCapture
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
        CLSID_vAcademiaCapture,                // Filter CLSID. 
        FILTER_NAME,                       // Filter name.
        NULL,                            // Device moniker. 
        &CLSID_VideoInputDeviceCategory,  // Video compressor category.
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
    CSource(NAME(FILTER_NAME), lpunk, CLSID_vAcademiaCapture)
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

	HWND vaWnd = NULL;

	CProcessWindowFinder	finder;		
	SFindedProcess process = finder.GetPlayerWindow("", "player.exe");
	if (process.window != NULL)
	{
		vaWnd = process.window;		
	}
	
	RECT rct;
	rct.right = 320;
	rct.left = 0;
	rct.bottom = 200;
	rct.top = 0;
	if (vaWnd)
	{
		GetClientRect(vaWnd, &rct);
	}
    m_paStreams[0] = new CVAcademiaStream(phr, this, L"vAcademiaCapture!", vaWnd, rct.right - rct.left, rct.bottom - rct.top);
    if(m_paStreams[0] == NULL)
    {
        if(phr)
            *phr = E_OUTOFMEMORY;

        return;
    }

} 

CVAcademiaStream::CVAcademiaStream(HRESULT *phr,
                         CVAcademiaCaptureFilter *pParent,
                         LPCWSTR pPinName, HWND vaWnd, int width, int height) :
    CSourceStream(NAME(FILTER_NAME),phr, pParent, pPinName),
    m_iImageWidth(width),
    m_iImageHeight(height),
    m_iDefaultRepeatTime(100),
	m_lastImageData(NULL),
	m_lastImageDataSize(0)
{
    ASSERT(phr);
    CAutoLock cAutoLock(&m_cSharedState);

    m_captureObject = new CVAcademiaCapture(vaWnd, m_iImageWidth, m_iImageHeight);
	m_vaWnd = vaWnd;
    if(m_captureObject == NULL)
    {
        if(phr)
            *phr = E_OUTOFMEMORY;
    }
	m_fillCnt = 0;
	m_isFirstFill = true;

	m_startTime = timeGetTime();
} 


//
// Destructor
//
CVAcademiaStream::~CVAcademiaStream()
{
    CAutoLock cAutoLock(&m_cSharedState);
    if (m_captureObject)
	{
        delete m_captureObject;
	}

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

void CVAcademiaStream::SaveLastImage(unsigned char* data, unsigned int size)
{
	if ((m_lastImageDataSize != size) && (m_lastImageData))
	{
		delete[] m_lastImageData;
		m_lastImageData = NULL;
	}

	if (!m_lastImageData)
	{
		m_lastImageData = new unsigned char[size];
		m_lastImageDataSize = size;
	}
	memcpy(m_lastImageData, data, size);
}

void CVAcademiaStream::RestoreLastImage(unsigned char* data, unsigned int size)
{
	if ((!m_lastImageData) || (m_lastImageDataSize != size))
	{
		memset(data, 0, size);
	}
	else
	{
		memcpy(data, m_lastImageData, size);
	}
}

HRESULT CVAcademiaStream::FillBuffer(IMediaSample *pms)
{
    CheckPointer(pms,E_POINTER);
    ASSERT(m_captureObject);

    BYTE *pData;
    long lDataLen;

    pms->GetPointer(&pData);
    lDataLen = pms->GetSize();

	CRefTime rtStart = m_rtSampleTime;
m_iRepeatTime = 50;
	/*int waitTime = m_timer.elapsed();
	while (!m_isFirstFill && m_iRepeatTime*1000 - waitTime > 0)
	{
		Sleep(1);
		waitTime = m_timer.elapsed();
	}*/

    ZeroMemory(pData, lDataLen);

	if (m_captureObject->GetImage(pData, lDataLen))
	{
		SaveLastImage(pData, lDataLen);
	}
	else
	{
		RestoreLastImage(pData, lDataLen);
	}
	m_fillCnt++;
	

	int currTime = timeGetTime();
	int deltaTime = currTime - m_startTime;

	if (m_isFirstFill)
		m_rtSampleTime += (LONG)m_iRepeatTime;
	else 
	{
		m_rtSampleTime += (__int64)m_timer.elapsed()*10;
	}
    // Increment to find the finish time
  //  m_rtSampleTime += (LONG)50;

    pms->SetTime((REFERENCE_TIME *) &rtStart,(REFERENCE_TIME *) &m_rtSampleTime);

	pms->SetSyncPoint(TRUE);
	m_timer.restart();
	m_isFirstFill = false;
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
    // Adjust the repeat rate.
    if(q.Proportion<=0)
    {
        m_iRepeatTime = 1000;        // We don't go slower than 1 per second
    }
    else
    {
        m_iRepeatTime = m_iRepeatTime*1000 / q.Proportion;
        if(m_iRepeatTime>1000)
        {
            m_iRepeatTime = 1000;    // We don't go slower than 1 per second
        }
        else if(m_iRepeatTime<10)
        {
            m_iRepeatTime = 10;      // We don't go faster than 100/sec
        }
    }

    // skip forwards
    if(q.Late > 0)
        m_rtSampleTime += q.Late;

    return NOERROR;

} // Notify


//
// GetMediaType
//
// I _prefer_ 5 formats - 8, 16 (*2), 24 or 32 bits per pixel and
// I will suggest these with an image size of 320x240. However
// I can accept any image size which gives me some space to bounce.
//
// A bit of fun:
//      8 bit displays get red balls
//      16 bit displays get blue
//      24 bit see green
//      And 32 bit see yellow
//
// Prefered types should be ordered by quality, zero as highest quality
// Therefore iPosition =
// 0    return a 32bit mediatype
// 1    return a 24bit mediatype
// 2    return 16bit RGB565
// 3    return a 16bit mediatype (rgb555)
// 4    return 8 bit palettised format
// (iPosition > 4 is invalid)
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

    if(iPosition > 4)
    {
        return VFW_S_NO_MORE_ITEMS;
    }

    VIDEOINFO *pvi = (VIDEOINFO *) pmt->AllocFormatBuffer(sizeof(VIDEOINFO));
    if(NULL == pvi)
        return(E_OUTOFMEMORY);

    ZeroMemory(pvi, sizeof(VIDEOINFO));

    switch(iPosition)
    {
        case 0:
        {    
            // Return our highest quality 32bit format

            // since we use RGB888 (the default for 32 bit), there is
            // no reason to use BI_BITFIELDS to specify the RGB
            // masks. Also, not everything supports BI_BITFIELDS

            SetPaletteEntries(Yellow);
            pvi->bmiHeader.biCompression = BI_RGB;
            pvi->bmiHeader.biBitCount    = 32;
            break;
        }

        case 1:
        {   // Return our 24bit format

            SetPaletteEntries(Green);
            pvi->bmiHeader.biCompression = BI_RGB;
            pvi->bmiHeader.biBitCount    = 24;
            break;
        }

        case 2:
        {       
            // 16 bit per pixel RGB565

            // Place the RGB masks as the first 3 doublewords in the palette area
            for(int i = 0; i < 3; i++)
                pvi->TrueColorInfo.dwBitMasks[i] = bits565[i];

            SetPaletteEntries(Blue);
            pvi->bmiHeader.biCompression = BI_BITFIELDS;
            pvi->bmiHeader.biBitCount    = 16;
            break;
        }

        case 3:
        {   // 16 bits per pixel RGB555

            // Place the RGB masks as the first 3 doublewords in the palette area
            for(int i = 0; i < 3; i++)
                pvi->TrueColorInfo.dwBitMasks[i] = bits555[i];

            SetPaletteEntries(Blue);
            pvi->bmiHeader.biCompression = BI_BITFIELDS;
            pvi->bmiHeader.biBitCount    = 16;
            break;
        }

        case 4:
        {   // 8 bit palettised

            SetPaletteEntries(Red);
            pvi->bmiHeader.biCompression = BI_RGB;
            pvi->bmiHeader.biBitCount    = 8;
            pvi->bmiHeader.biClrUsed        = iPALETTE_COLORS;
            break;
        }
    }

    // (Adjust the parameters common to all formats...)

    // put the optimal palette in place
    for(int i = 0; i < iPALETTE_COLORS; i++)
    {
        pvi->TrueColorInfo.bmiColors[i].rgbRed      = m_Palette[i].peRed;
        pvi->TrueColorInfo.bmiColors[i].rgbBlue     = m_Palette[i].peBlue;
        pvi->TrueColorInfo.bmiColors[i].rgbGreen    = m_Palette[i].peGreen;
        pvi->TrueColorInfo.bmiColors[i].rgbReserved = 0;
    }

    pvi->bmiHeader.biSize       = sizeof(BITMAPINFOHEADER);
    pvi->bmiHeader.biWidth      = m_iImageWidth;
    pvi->bmiHeader.biHeight     = m_iImageHeight;
    pvi->bmiHeader.biPlanes     = 1;
    pvi->bmiHeader.biSizeImage  = GetBitmapSize(&pvi->bmiHeader);
    pvi->bmiHeader.biClrImportant = 0;

    SetRectEmpty(&(pvi->rcSource)); // we want the whole image area rendered.
    SetRectEmpty(&(pvi->rcTarget)); // no particular destination rectangle

    pmt->SetType(&MEDIATYPE_Video);
    pmt->SetFormatType(&FORMAT_VideoInfo);
    pmt->SetTemporalCompression(FALSE);

    // Work out the GUID for the subtype from the header info.
    const GUID SubTypeGUID = GetBitmapSubtype(&pvi->bmiHeader);
    pmt->SetSubtype(&SubTypeGUID);
    pmt->SetSampleSize(pvi->bmiHeader.biSizeImage);

    return NOERROR;

} // GetMediaType


//
// CheckMediaType
//
// We will accept 8, 16, 24 or 32 bit video formats, in any
// image size that gives room to bounce.
// Returns E_INVALIDARG if the mediatype is not acceptable
//
HRESULT CVAcademiaStream::CheckMediaType(const CMediaType *pMediaType)
{
    CheckPointer(pMediaType,E_POINTER);

    if((*(pMediaType->Type()) != MEDIATYPE_Video) ||   // we only output video
       !(pMediaType->IsFixedSize()))                   // in fixed size samples
    {                                                  
        return E_INVALIDARG;
    }

    // Check for the subtypes we support
    const GUID *SubType = pMediaType->Subtype();
    if (SubType == NULL)
        return E_INVALIDARG;

    if((*SubType != MEDIASUBTYPE_RGB8)
        && (*SubType != MEDIASUBTYPE_RGB565)
        && (*SubType != MEDIASUBTYPE_RGB555)
        && (*SubType != MEDIASUBTYPE_RGB24)
        && (*SubType != MEDIASUBTYPE_RGB32))
    {
        return E_INVALIDARG;
    }

    // Get the format area of the media type
    VIDEOINFO *pvi = (VIDEOINFO *) pMediaType->Format();

    if(pvi == NULL)
        return E_INVALIDARG;

    // Check the image size. As my default ball is 10 pixels big
    // look for at least a 20x20 image. This is an arbitary size constraint,
    // but it avoids balls that are bigger than the picture...

    if((pvi->bmiHeader.biWidth < 20) || ( abs(pvi->bmiHeader.biHeight) < 20))
    {
        return E_INVALIDARG;
    }

    // Check if the image width & height have changed
    if(pvi->bmiHeader.biWidth != m_captureObject->GetImageWidth() || 
       abs(pvi->bmiHeader.biHeight) != m_captureObject->GetImageHeight())
    {
        // If the image width/height is changed, fail CheckMediaType() to force
        // the renderer to resize the image.
        return E_INVALIDARG;
    }


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

    VIDEOINFO *pvi = (VIDEOINFO *) m_mt.Format();
    pProperties->cBuffers = 1;
    pProperties->cbBuffer = pvi->bmiHeader.biSizeImage;

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

    if(SUCCEEDED(hr))
    {
        VIDEOINFO * pvi = (VIDEOINFO *) m_mt.Format();
        if (pvi == NULL)
            return E_UNEXPECTED;
       
		RECT rct;
		rct.right = 320;
		rct.left = 0;
		rct.bottom = 200;
		rct.top = 0;
		if (m_vaWnd)
		{
			GetClientRect(m_vaWnd, &rct);
		}
        CVAcademiaCapture* newCaptureObject = new CVAcademiaCapture(m_vaWnd, rct.right - rct.left, rct.top - rct.bottom);

        if (newCaptureObject)
        {
            delete m_captureObject;
            m_captureObject = newCaptureObject;
        }
        else
		{
            hr = E_OUTOFMEMORY;
		}

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
    m_iRepeatTime = m_iDefaultRepeatTime;

    return NOERROR;

} // OnThreadCreate


//
// SetPaletteEntries
//
// If we set our palette to the current system palette + the colours we want
// the system has the least amount of work to do whilst plotting our images,
// if this stream is rendered to the current display. The first non reserved
// palette slot is at m_Palette[10], so put our first colour there. Also
// guarantees that black is always represented by zero in the frame buffer
//
HRESULT CVAcademiaStream::SetPaletteEntries(Colour color)
{
    CAutoLock cAutoLock(m_pFilter->pStateLock());

    HDC hdc = GetDC(NULL);  // hdc for the current display.
    UINT res = GetSystemPaletteEntries(hdc, 0, iPALETTE_COLORS, (LPPALETTEENTRY) &m_Palette);
    ReleaseDC(NULL, hdc);

    if(res == 0)
        return E_FAIL;

    switch(color)
    {
        case Red:
            m_Palette[10].peBlue  = 0;
            m_Palette[10].peGreen = 0;
            m_Palette[10].peRed   = 0xff;
            break;

        case Yellow:
            m_Palette[10].peBlue  = 0;
            m_Palette[10].peGreen = 0xff;
            m_Palette[10].peRed   = 0xff;
            break;

        case Blue:
            m_Palette[10].peBlue  = 0xff;
            m_Palette[10].peGreen = 0;
            m_Palette[10].peRed   = 0;
            break;

        case Green:
            m_Palette[10].peBlue  = 0;
            m_Palette[10].peGreen = 0xff;
            m_Palette[10].peRed   = 0;
            break;
    }

    m_Palette[10].peFlags = 0;
    return NOERROR;

} // SetPaletteEntries

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


