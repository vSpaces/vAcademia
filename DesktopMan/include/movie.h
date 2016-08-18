#pragma once
//------------------------------------------------------------------------------
// File: vcdplyer.h
//
// Desc: DirectShow sample code
//       - Class header file for VMRPlayer sample
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------
#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>

#include <Qedit.h>
#include <strmif.h>
//#define DIRECT3D_VERSION         0x0900
//#include <d3d9.h>
//#include <Vmr9.h>
#include <dshow.h>
#include <string>
#include "mutex.h"


/* -------------------------------------------------------------------------
** CMovie - a DirectShow movie playback class.
** -------------------------------------------------------------------------
*/
enum EMovieMode { MOVIE_NOTOPENED = 0x00,
                  MOVIE_OPENED    = 0x01,
                  MOVIE_PLAYING   = 0x02,
                  MOVIE_STOPPED   = 0x03,
                  MOVIE_PAUSED    = 0x04 };

struct IMpegAudioDecoder;
struct IMpegVideoDecoder;
struct IQualProp;

class CMovie
{
private:
    // Our state variable - records whether we are opened, playing etc.
    EMovieMode       m_Mode;
    GUID             m_TimeFormat;

    IFilterGraph            *m_Fg;
    IGraphBuilder           *m_Gb;
    IMediaControl           *m_Mc;
    IMediaSeeking           *m_Ms;

	IMediaEventEx			*m_MeEx;
	IVideoWindow			 *m_Vw;

	////----------------Image capturing-----------------//
	ISampleGrabber*	m_pGrab;
	IBaseFilter* m_pSGFilter;
	bool bIsCOMInit;

    HRESULT FindInterfaceFromFilterGraph(
        REFIID iid, // interface to look for
        LPVOID *lp  // place to return interface pointer in
        );
	HRESULT AddSampleGrabFilterToGraph(IFilterGraph* apFilterGraph);
	HRESULT SetVideoFormat(GUID format);

public:
     CMovie();
    ~CMovie();

    HRESULT         OpenMovie(const wchar_t *lpFileName);
    DWORD           CloseMovie();

    BOOL            PlayMovie();
    BOOL            PauseMovie();
    BOOL            StopMovie();

    long            GetMovieEventCode();

    BOOL            GetNativeMovieSize(unsigned int *cx, unsigned int *cy);

    REFTIME         GetDuration();
    REFTIME         GetCurrentPosition();
    EMovieMode      StatusMovie();

    BOOL            SeekToPosition(REFTIME rt,BOOL bFlushData);
    void            SetBorderClr(COLORREF clr);

 

	HBITMAP			MakeSnapshot(bool isFirstShot);
	EMovieMode GetMode(){ return m_Mode;};
	CMutex mutex;

};

