#include "stdafx.h"
#include "../include/project.h"

#include <stdarg.h>
#include <initguid.h>
#include <strsafe.h>
//#include "../include/vmrutil.h"

CMovie::CMovie() :
      m_Mode(MOVIE_NOTOPENED),
      m_Fg(NULL),
      m_Gb(NULL),
      m_Mc(NULL),
      m_Ms(NULL),
	  m_MeEx(NULL),
	  m_pGrab(NULL),
	  m_pSGFilter(NULL),
      m_TimeFormat(TIME_FORMAT_MEDIA_TIME)

{
	bIsCOMInit = false;
}

CMovie::~CMovie()
{
}

HRESULT CMovie::AddSampleGrabFilterToGraph(IFilterGraph* apFilterGraph)
{
	if( m_pGrab != NULL)
		return S_OK;
	if( !apFilterGraph)
		return E_FAIL;

	HRESULT hr = S_OK;
	//////////////////////////////////////////////////////////////////////////
	// Construct filter to grab video frames
	if( FAILED( hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (LPVOID *)&m_pSGFilter)))
		return hr;	

	hr = apFilterGraph->AddFilter(m_pSGFilter, L"Grabber");
	if( FAILED( hr))
		return hr;

	hr = m_pSGFilter->QueryInterface(IID_ISampleGrabber, (void **)&m_pGrab);
	if( FAILED( hr))
		return hr;

	hr = SetVideoFormat(MEDIASUBTYPE_RGB24);
	if (FAILED(hr))
		return hr;

	return hr;
}

HRESULT CMovie::SetVideoFormat(GUID format)
{
	if (!m_pGrab)
		return E_POINTER;
	AM_MEDIA_TYPE mt;
	memset(&mt,0,sizeof(mt));
	mt.majortype = MEDIATYPE_Video;
	mt.formattype = FORMAT_VideoInfo;
	mt.subtype = format;

	return m_pGrab->SetMediaType(&mt);
}

/******************************Public*Routine******************************\
* OpenMovie
*
\**************************************************************************/
HRESULT
CMovie::OpenMovie(
    const wchar_t *lpFileName
    )
{
    IUnknown        *pUnk;
    HRESULT         hRes;

    // Check input string
    if (lpFileName == NULL)
        return E_POINTER;

    hRes = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if(hRes == S_FALSE)
        CoUninitialize();
	else
		bIsCOMInit = true;

    hRes = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
                            IID_IUnknown, (LPVOID *)&pUnk);

    if(SUCCEEDED(hRes))
    {        
        hRes = pUnk->QueryInterface(IID_IFilterGraph, (LPVOID *)&m_Fg);
        if(FAILED(hRes))
        {
            pUnk->Release();
            return hRes;
        }

		hRes = AddSampleGrabFilterToGraph( m_Fg);
		if(FAILED(hRes))
		{
			m_Fg->Release(); m_Fg = NULL;
			return hRes;
		}
   
        hRes = pUnk->QueryInterface(IID_IGraphBuilder, (LPVOID *)&m_Gb);
		
        if(FAILED(hRes))
        {
            pUnk->Release();
            m_Fg->Release(); m_Fg = NULL;
            return hRes;
        }

		hRes = m_Gb->RenderFile( lpFileName, NULL);
        if(FAILED(hRes))
        {
            pUnk->Release();
            m_Fg->Release(); m_Fg = NULL;
            m_Gb->Release(); m_Gb = NULL;
            return hRes;
        }
		m_Mode = MOVIE_OPENED;

		hRes = pUnk->QueryInterface(IID_IVideoWindow, (LPVOID *)&m_Vw);
		if(FAILED(hRes))
		{
			m_Fg->Release(); m_Fg = NULL;
			return hRes;
		}

        hRes = pUnk->QueryInterface(IID_IMediaControl, (LPVOID *)&m_Mc);
        if(FAILED(hRes))
        {
            pUnk->Release();
            m_Fg->Release(); m_Fg = NULL;
            m_Vw->Release(); m_Vw = NULL;
            m_Gb->Release(); m_Gb = NULL;
            return hRes;
        }

		m_Vw->put_Visible( OAFALSE);
		m_Vw->put_AutoShow(OAFALSE);
      
		pUnk->QueryInterface(IID_IMediaEventEx, (LPVOID *)&m_MeEx);

        pUnk->QueryInterface(IID_IMediaSeeking, (LPVOID *)&m_Ms);
        pUnk->Release();

		if( m_Ms->SetTimeFormat( &m_TimeFormat) != S_OK)
		{
			m_TimeFormat = TIME_FORMAT_NONE;
		}

		hRes = m_pGrab->SetBufferSamples(TRUE);
		if( FAILED( hRes))
			return hRes;
		hRes = m_pGrab->SetOneShot(TRUE);
		if( FAILED( hRes))
			return hRes;

        return S_OK;
    }
    else
    {
        m_Fg = NULL;
    }

    return hRes;
}


/******************************Public*Routine******************************\
* CloseMovie
*
\**************************************************************************/
DWORD
CMovie::CloseMovie(
    )
{
    m_Mode = MOVIE_NOTOPENED;

    if(m_Mc)
    {
		m_Mc->Stop();

		if(m_Vw)
		{
			m_Vw->put_Visible(OAFALSE);
			m_Vw->put_Owner(NULL);
		}

		mutex.lock();
		SAFE_RELEASE(m_pGrab);
		mutex.unlock();
		SAFE_RELEASE(m_pSGFilter);

		SAFE_RELEASE(m_Mc);
		SAFE_RELEASE(m_MeEx);
		SAFE_RELEASE(m_Vw);
		SAFE_RELEASE(m_Ms);
		
		SAFE_RELEASE(m_Gb);
		SAFE_RELEASE(m_Fg);
    }

	if (bIsCOMInit)
	{
		QzUninitialize();
		bIsCOMInit = false;
	}
    return 0L;
}


/******************************Public*Routine******************************\
* CMovie::GetNativeMovieSize
*
\**************************************************************************/
BOOL
CMovie::GetNativeMovieSize(
    unsigned int *pcx,
    unsigned int *pcy
    )
{
    BOOL    bRet = FALSE;
	if( m_Vw)
	{
		m_Vw->get_Width( (long*)pcx);
		m_Vw->get_Height( (long*)pcy);
	}
    /*if(m_Wc)
    {
        bRet = (m_Wc->GetNativeVideoSize(pcx, pcy, NULL, NULL) == S_OK);
    }*/

    return bRet;
}

/******************************Public*Routine******************************\
* PlayMovie
*
\**************************************************************************/
BOOL
CMovie::PlayMovie(
    )
{
    REFTIME rt, rtAbs, rtDur;
    HRESULT hr=S_OK;

    rt = GetCurrentPosition();
    rtDur = GetDuration();

    //
    // If we are near the end of the movie seek to the start, otherwise
    // stay where we are.
    //
    rtAbs = rt - rtDur;
    if(rtAbs < (REFTIME)0)
    {
        rtAbs = -rtAbs;
    }

    if(rtAbs < (REFTIME)1)
    {
        SeekToPosition((REFTIME)0,FALSE);
    }

    //
    // Change mode after setting m_Mode but before starting the graph
    //
    m_Mode = MOVIE_PLAYING;
    hr = m_Mc->Run();
	/*if( hr != S_OK)
		return FALSE;*/
    return TRUE;
}


/******************************Public*Routine******************************\
* PauseMovie
*
\**************************************************************************/
BOOL
CMovie::PauseMovie(
    )
{
    m_Mode = MOVIE_PAUSED;

    HRESULT hr = m_Mc->Pause();
    return TRUE;
}

BOOL
CMovie::StopMovie(
    )
{
    m_Mode = MOVIE_STOPPED;
    HRESULT hr = m_Mc->Stop();
    return TRUE;
}

EMovieMode CMovie::StatusMovie()
{
    if(m_Mc)
    {
        FILTER_STATE    fs;
        HRESULT         hr;

        hr = m_Mc->GetState(100, (OAFilterState *)&fs);

        // Don't know what the state is so just stay at old state.
        if(hr == VFW_S_STATE_INTERMEDIATE)
        {
            return m_Mode;
        }

        switch(fs)
        {
            case State_Stopped:
                m_Mode = MOVIE_STOPPED;
                break;

            case State_Paused:
                m_Mode = MOVIE_PAUSED;
                break;

            case State_Running:
                m_Mode = MOVIE_PLAYING;
                break;
        }
    }

    return m_Mode;
}

long CMovie::GetMovieEventCode()
{
    HRESULT hr;
    long    lEventCode;
    LONG_PTR    lParam1, lParam2;

    if(m_MeEx != NULL)
    {
        hr = m_MeEx->GetEvent(&lEventCode, &lParam1, &lParam2, 0);
        if(SUCCEEDED(hr))
        {
            hr = m_MeEx->FreeEventParams(lEventCode, lParam1, lParam2);
            return lEventCode;
        }
    }

    return 0L;
}

REFTIME CMovie::GetDuration()
{
    HRESULT hr;
    LONGLONG Duration;

    // Should we seek using IMediaSelection
    if(m_TimeFormat != TIME_FORMAT_MEDIA_TIME)
    {
        hr = m_Ms->GetDuration(&Duration);
        if(SUCCEEDED(hr))
        {
            return double(Duration);
        }
    }
    else if(m_Ms != NULL)
    {
        hr = m_Ms->GetDuration(&Duration);
        if(SUCCEEDED(hr))
        {
            return double(Duration) / UNITS;
        }
    }

    return 0;
}

REFTIME CMovie::GetCurrentPosition()
{
    REFTIME rt = (REFTIME) 0;
    HRESULT hr;
    LONGLONG Position;

    // Should we return a media position
    if(m_TimeFormat != TIME_FORMAT_MEDIA_TIME)
    {
        hr = m_Ms->GetPositions(&Position, NULL);
        if(SUCCEEDED(hr))
        {
            return double(Position);
        }
    }
    else if(m_Ms != NULL)
    {
        hr = m_Ms->GetPositions(&Position, NULL);
        if(SUCCEEDED(hr))
        {
            return double(Position) / UNITS;
        }
    }

    return rt;
}

BOOL CMovie::SeekToPosition(
    REFTIME rt,
    BOOL bFlushData
    )
{
    HRESULT hr;
    LONGLONG llTime = LONGLONG(m_TimeFormat == TIME_FORMAT_MEDIA_TIME ? rt * double(UNITS) : rt );

    if(m_Ms != NULL)
    {
        FILTER_STATE fs;
        hr = m_Mc->GetState(100, (OAFilterState *)&fs);

        hr = m_Ms->SetPositions(&llTime, AM_SEEKING_AbsolutePositioning, NULL, 0);

        // This gets new data through to the renderers
        if(fs == State_Stopped && bFlushData)
        {
            hr = m_Mc->Pause();
            hr = m_Mc->GetState(INFINITE, (OAFilterState *)&fs);
            hr = m_Mc->Stop();
        }

        if(SUCCEEDED(hr))
        {
            return TRUE;
        }
    }

    return FALSE;
}

HBITMAP CMovie::MakeSnapshot(bool isFirstShot)
{
	if( !m_pGrab)
		return 0;

	HRESULT	hr=0;

	long EvCode;
	if (isFirstShot)
		hr = m_MeEx->WaitForCompletion(5000, &EvCode);

	AM_MEDIA_TYPE MediaType;

	mutex.lock();
	if( !m_pGrab)
	{
		mutex.unlock();
		return 0;
	}
	hr=m_pGrab->GetConnectedMediaType(&MediaType);
	mutex.unlock();
	if( FAILED( hr))
		return 0;

	// Get a pointer to the video header.
	VIDEOINFOHEADER *pVideoHeader = (VIDEOINFOHEADER*)MediaType.pbFormat;
	// The video header contains the bitmap information. 
	// Copy it into a BITMAPINFO structure.
	BITMAPINFO BitmapInfo;
	ZeroMemory(&BitmapInfo, sizeof(BitmapInfo));
	CopyMemory(&BitmapInfo.bmiHeader, &(pVideoHeader->bmiHeader), sizeof(BITMAPINFOHEADER));
	// Create a DIB from the bitmap header, and get a pointer to the buffer.
	unsigned long size = 0;
	void *buffer = NULL;
	HBITMAP hBitmap = 0;

	hBitmap = CreateDIBSection(0, &BitmapInfo, DIB_RGB_COLORS, &buffer, NULL, 0);

	if( hBitmap == 0)
		return 0;

	mutex.lock();
	if( m_pGrab == NULL)
	{
		mutex.unlock();
		return 0;
	}
	hr = m_pGrab->GetCurrentBuffer((long*)&size, NULL);
	
	if( SUCCEEDED(hr))
	{
		if( size == pVideoHeader->bmiHeader.biSizeImage)
		{
			hr = m_pGrab->GetCurrentBuffer((long*)&size, (long *)buffer);
		}
		else
		{
			void*	tbuffer = calloc(size, 1);
			hr = m_pGrab->GetCurrentBuffer((long*)&size, (long *)tbuffer);
			memcpy( buffer, tbuffer, size>pVideoHeader->bmiHeader.biSizeImage?pVideoHeader->bmiHeader.biSizeImage:size);
			free(tbuffer);
		}		
	}
	else
	{
		int ff = 0;
		hr += 1;
	}
	mutex.unlock();

	if (isFirstShot)
	{
		mutex.lock();
		if( m_pGrab == NULL)
		{
			mutex.unlock();
			return 0;
		}
		hr = m_pGrab->SetOneShot(FALSE);
		mutex.unlock();
		//m_Mc->Stop();
		StopMovie();
		SeekToPosition( 0, TRUE);
		// запускаем после того как установлено качество картинки
		//m_Mc->Run();
	}

	return hBitmap;
}



/*****************************Private*Routine******************************\
* SetBorderClr
*
\**************************************************************************/
void
CMovie::SetBorderClr(COLORREF clr)
{
    //m_Wc->SetBorderColor(clr);
}




