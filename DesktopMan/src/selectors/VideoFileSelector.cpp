#include "StdAfx.h"
#include "../../WebCamGrabber/DevicesCtrl.h"
#include "../../include/selectors/videofileselector.h"
#include "../../include/selectors/DesktopFrame.h"
#include "../../include/selectors/SafeRect.h"
#include "../../../nengine/ncommon/ThreadAffinity.h"


CVideoFileSelector::CVideoFileSelector(oms::omsContext* apContext, LPCWSTR aFileName):
	MP_WSTRING_INIT(m_fileName)
{
	context = apContext;
	m_currentFrame = NULL;
	m_firstCaptureFrame = true;
	m_fileName = aFileName;
	MP_NEW_V3( m_videoRunner, CVideoFileGraphRunning, this, true, aFileName);
	m_currentRect.SetRect(0,0,0,0);
	m_needVideoRunner = false;
	m_videoWasStarted = false;
	m_isDeleted = false;
	m_pMovie = NULL;
	m_currPos = 0;
	m_playing = true;
	csSeek.Init();
	csDestroy.Init();
}

CVideoFileSelector::~CVideoFileSelector(void)
{
	CleanUp();
	SAFE_DELETE(m_videoRunner);
	csSeek.Term();
	csDestroy.Term();
}

int CVideoFileSelector::GetType()
{
	return VIDEOFILE_SELECTOR;
}

void CVideoFileSelector::SelectionComplete(int resultCode)
{	
	/*if (resultCode != WEBCAMERA_ALREADY_USED && resultCode != USER_CHOSE_WEBCAMERA)
		m_deviceWasStarted = true;*/
	m_videoWasStarted = true;
	if( resultCode == 0 && m_pMovie != NULL)
	{
		m_duration = m_pMovie->GetDuration();
		if(context->mpLogWriter != NULL)
		{
			CComString s = "CVideoFileSelector::SelectionComplete m_duration=="; 
			char ff[50];
			int f = (int)m_duration;
			_itoa_s( f, ff, 50, 10);
			s += ff;
			context->mpLogWriter->WriteLnLPCSTR( s.GetBuffer());
		}
	}
	else
		m_duration = 0;
	InvokeEx<int, &IDesktopSelectorEvents::OnSelectionCompleted>(resultCode);
}

void CVideoFileSelector::SetBoardName( LPCSTR alpTarget)
{
	m_target = alpTarget;
}

/*void CVideoFileSelector::SetResolutionComplete(int resultCode)
{
	InvokeEx<int, &IDesktopSelectorEvents::OnSelectionCompleted>(-resultCode);
}*/

// начинает выборку области
bool CVideoFileSelector::StartSourceSelection()
{
	m_videoRunner->Start();
	return true;
	//SelectionComplete();
}

// заканчивает выборку области
void CVideoFileSelector::CancelSourceSelection()
{		
}

// возвращает последний полученный кадр
IDesktopFrame* CVideoFileSelector::GetCurrentFrame()
{
	return m_currentFrame;
}

// делает снимок нового кадра с текущими параметрами
IDesktopFrame* CVideoFileSelector::RecapFrame()
{
	void *imgBits = NULL;
	BITMAPINFO bmpInfo;
	CRect rect;
	if( !m_pMovie)
	{
		if(context != NULL && context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLnLPCSTR("CVideoFileSelector::RecapFrame m_pMovie == NULL");
		return m_currentFrame;
	}
	if( m_pMovie->GetMode() == MOVIE_NOTOPENED)
	{
		if(context != NULL && context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLnLPCSTR("CVideoFileSelector::RecapFrame mode == MOVIE_NOTOPENED");
		return m_currentFrame;
	}

	csSeek.Lock();
	if( !m_playing && !m_seek)
	{
		csSeek.Unlock();
		return m_currentFrame;
	}
	if( m_seek>0)
		m_seek--;
	csSeek.Unlock();
	HBITMAP hCapturedImage = NULL;

	rect = GetCurrentSelectingRect();

	CSafeRect safeRect(1280*4, 1024*4);
	safeRect.Init(rect.left, rect.top, rect.right, rect.bottom);
	rect.left = safeRect.GetX();
	rect.top = safeRect.GetY();
	rect.right = safeRect.GetX2()+1;
	rect.bottom = safeRect.GetY2()+1;

	HDC screenDC = GetDC(0);
	HDC hTempDCFrom, hTempDCTo;
	hTempDCFrom = CreateCompatibleDC( screenDC);
	hTempDCTo = CreateCompatibleDC(screenDC);

	HBITMAP hRightSizeCapturedImage = CreateDIBSection(rect.Width(), rect.Height(), 32, &imgBits, bmpInfo, true);
	bmpInfo.bmiHeader.biSizeImage = bmpInfo.bmiHeader.biWidth*abs(bmpInfo.bmiHeader.biHeight)*bmpInfo.bmiHeader.biBitCount/8;
	MakeWhiteBitmap(imgBits, rect.Width(), rect.Height());

	HGDIOBJ hBitmapOldTo = SelectObject(hTempDCTo, hRightSizeCapturedImage);

	//////////////////////////////////////////////////////////////////////////
	//BYTE* lpCurrImage = m_pMovie->GetCurrentImage();
	///////////////////////////////////////////////////////////////////////
	hCapturedImage = m_pMovie->MakeSnapshot(m_firstCaptureFrame);		
	HGDIOBJ hBitmapOldFrom = NULL; 
	if( hCapturedImage)
	{
		hBitmapOldFrom = SelectObject(hTempDCFrom, hCapturedImage);
		BitBlt(hTempDCTo, 0, 0, rect.Width(), rect.Height(), hTempDCFrom, 0, 0, SRCCOPY);
		SAFE_RELEASE( m_currentFrame);
		MP_NEW_V2( m_currentFrame, CDesktopFrame, imgBits, bmpInfo);
		DeleteObject( hCapturedImage);
		hCapturedImage = NULL;
	}
	DeleteObject( hRightSizeCapturedImage);
	hRightSizeCapturedImage = NULL;

	if (hBitmapOldFrom)
		SelectObject(hTempDCFrom, hBitmapOldFrom);
	SelectObject(hTempDCTo, hBitmapOldTo);	

	DeleteDC(hTempDCFrom);
	DeleteDC(hTempDCTo);
	ReleaseDC( 0, screenDC);

	if(!m_firstCaptureFrame)
	{
		EMovieMode mode = m_pMovie->StatusMovie();
		bool playing = ( mode != MOVIE_STOPPED && mode != MOVIE_PAUSED);		
		if( m_playing || m_seek > 0)
		{
			SetBarPosition( false);
			csSeek.Lock();
			if(m_seek > 0)
				m_seek--;
			csSeek.Unlock();
		}		
		m_playing = !VideoIsEnd() & playing;		
	}
	m_firstCaptureFrame = false;

	return m_currentFrame;
}

void CVideoFileSelector::SetPlayMode( bool aPlaying)
{
	m_playing = aPlaying;
}

// возвращает текущую область захвата
RECT CVideoFileSelector::GetCurrentSelectingRect()
{
	CRect rect(0,0,0,0);
	_WC_RESOLUTION resolution;
	resolution.height = 0;
	resolution.width = 0;

	if (m_currentRect.Width() == 0 && m_currentRect.Height() == 0 )
	{		
		if (!m_pMovie)
			return rect; 

		m_pMovie->GetNativeMovieSize( &resolution.width, &resolution.height);			
	}
	else
	{
		return m_currentRect;
	}

	rect.left = 0;
	rect.top = 0;
	rect.bottom = resolution.height;
	rect.right = resolution.width;

	return rect;	
}

//возвращает true если захват возможен
bool CVideoFileSelector::IsCaptureAvailable()
{
	return true;
}

HWND CVideoFileSelector::GetCapturedWindow()
{
	return NULL;
}

bool CVideoFileSelector::Release()
{	
	if(m_pMovie != NULL)
	{
		//m_pMovie->NeedStop();
	}
	if (m_videoWasStarted)
	{
		Destroy();
		return true;
	}
	csDestroy.Lock();	
	m_needVideoRunner = true;
	csDestroy.Unlock();
	return false;
}

void CVideoFileSelector::VideoFileGraphRunEnd()
{
	bool result = false;
	csDestroy.Lock();
	if (m_needVideoRunner)
		result = true;
	csDestroy.Unlock();
	if (result)
	{
		CleanUp();
		m_isDeleted = true;
	}
}

void CVideoFileSelector::Destroy()
{
	MP_DELETE_THIS;
}

void CVideoFileSelector::CleanUp()
{
	ClearCapturedImage();
	if( m_pMovie != NULL)
	{
		m_pMovie->CloseMovie();
		MP_DELETE( m_pMovie);
	}
	m_duration = 0;
	m_target.Empty();
}

bool CVideoFileSelector::IsLive()
{
	return !m_isDeleted;
}

bool CVideoFileSelector::CanShowCaptureRect()
{
	return false;
}

void CVideoFileSelector::ClearCapturedImage()
{
	if( m_currentFrame != NULL)
	{
		MP_DELETE( m_currentFrame);
		m_currentFrame = NULL;
	}
}

CMovie *CVideoFileSelector::GetNewMovie()
{
	if( m_pMovie != NULL)
	{
		m_pMovie->CloseMovie();
		MP_DELETE( m_pMovie);
	}
	m_playing = false;
	m_currPos = 0;
	m_duration = 0;
	m_pMovie = MP_NEW(CMovie);
	return m_pMovie;
}

void CVideoFileSelector::CloseMovie()
{
	if( m_pMovie != NULL)
	{
		m_pMovie->CloseMovie();
		MP_DELETE( m_pMovie);
		m_pMovie = NULL;
	}
	m_playing = false;
	m_currPos = 0;
	m_duration = 0;
}

bool CVideoFileSelector::Run()
{
	if( m_pMovie != NULL)
	{
		m_playing = true;
		m_pMovie->PlayMovie();
		return true;
	}
	return false;
}

void CVideoFileSelector::Seek( double aSecond)
{
	REFTIME rt;
	REFTIME rtDur;
	if( m_pMovie == NULL)
	{
		if(context != NULL && context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLnLPCSTR("CVideoFileSelector::Seek m_pMovie == NULL");
		return;
	}
	rtDur = m_duration;//m_pMovie->GetDuration();
	//REFTIME rtSeek = aSeek * rtDur / m_sizeBar;
	rt = aSecond;
	//rt = m_pMovie->GetCurrentPosition() + rtSeek;

	rt = max(0, min(rt, rtDur));

	m_pMovie->SeekToPosition(rt,TRUE);
	csSeek.Lock();
	m_seek = 4;
	csSeek.Unlock();
}

void CVideoFileSelector::Play()
{
	if(!m_pMovie)
	{
		if(context != NULL && context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLnLPCSTR("CVideoFileSelector::Play m_pMovie == NULL");
		return;
	}
	EMovieMode mode = m_pMovie->StatusMovie();
	if(mode != MOVIE_PLAYING)
	{
		m_playing = true;
		m_pMovie->PlayMovie();
	}
}

void CVideoFileSelector::Pause()
{
	if(!m_pMovie)
	{
		if(context != NULL && context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLnLPCSTR("CVideoFileSelector::Pause m_pMovie == NULL");
		return;
	}
	EMovieMode mode = m_pMovie->StatusMovie();
	if(mode == MOVIE_PLAYING)
	{
		m_pMovie->PauseMovie();
	}
}

void CVideoFileSelector::SetBarPosition( bool aForce)
{
	REFTIME currPos = m_pMovie->GetCurrentPosition();
	if( aForce || m_currPos + 0.24 < currPos || m_currPos > currPos)
	{
		m_currPos = currPos;
		std::vector<double> v1;
		v1.push_back(m_currPos);
		v1.push_back(m_duration);
		InvokeEx2< LPCSTR, vector<double>, &IDesktopSelectorEvents::OnSetBarPosition>(m_target.GetBuffer(), v1);
	}
}

bool CVideoFileSelector::VideoIsEnd()
{
	long lEventCode = m_pMovie->GetMovieEventCode();
	switch(lEventCode)
	{
	case EC_COMPLETE:
	case EC_ERRORABORT:
	case EC_USERABORT:
		{
			m_pMovie->StopMovie();
			m_pMovie->SeekToPosition( 0, FALSE);
			InvokeEx< LPCSTR, &IDesktopSelectorEvents::OnVideoEnd>(m_target.GetBuffer());
			return true;
		}
	}
	return false;
}

double CVideoFileSelector::GetDuration()
{
	if( m_pMovie == NULL)
		return 0;
	return m_duration;
}

//////////////////////////////////////////////////////////////////////////
//class CVideoInputGraphRunning
//////////////////////////////////////////////////////////////////////////

CVideoFileGraphRunning::CVideoFileGraphRunning(CVideoFileSelector* selector, bool notifyAboutComplete, LPCWSTR aFileName) : MP_WSTRING_INIT(m_fileName)
{
	m_VideoSelector = selector;
	m_notifyAboutComplete = notifyAboutComplete;
	m_fileName = aFileName;
}

CVideoFileGraphRunning::~CVideoFileGraphRunning(void)
{
	//	Stop();
}

DWORD CVideoFileGraphRunning::Run()
{
	HANDLE hndl = GetCurrentThread();
	SetSecondaryThreadAffinity( hndl);
	DWORD resultCode = 0;
	int videoStartResult = SUCCESSFULL;
	CMovie *pMovie = NULL;
	if( !ShouldStop())
	{	
		pMovie = m_VideoSelector->GetNewMovie();

		if(pMovie && !ShouldStop())
		{
			HRESULT hr = pMovie->OpenMovie( m_fileName.c_str());
			if( hr == S_OK)
			{				
				if( !ShouldStop())
					pMovie->SetBorderClr(RGB(0x00, 0x80, 0x80));
				videoStartResult  = 0;
			}
			else
			{
				m_VideoSelector->CloseMovie();
				videoStartResult = VIDEO_FILE_IS_NOT_OPEN;
			}
		}

		/*if (!webCam->IsStarted())
		{
			// устанавливаем разрешение до выполнения RenderStream() в webgrabere
			if (m_VISelector->m_currentRect.Width() != 0 && m_VISelector->m_currentRect.Height() != 0)*/
		//		webCam->SetFrameSize(m_VISelector->m_currentRect.Width(), m_VISelector->m_currentRect.Height());
			
		if (videoStartResult == 0)
		{
			if( !ShouldStop())
			{
				m_VideoSelector->SetPlayMode( true);
				if(pMovie->PlayMovie())
					videoStartResult = 0;
				else
				{
					videoStartResult = VIDEO_FILE_IS_NOT_PLAYING;
					m_VideoSelector->CloseMovie();
				}
			}
		}
	}
	if( !ShouldStop())
	{
		if (videoStartResult == 0/*SUCCESSFULL*/)
		{
			if (m_VideoSelector->RecapFrame() == NULL)
			{
				videoStartResult = VIDEO_FILE_IS_NOT_PLAYING;
				m_VideoSelector->CloseMovie();
			}
			else 
				videoStartResult = 0;//SUCCESSFULL;
		}
		if (m_notifyAboutComplete)
			m_VideoSelector->SelectionComplete(videoStartResult);
	}
	m_VideoSelector->VideoFileGraphRunEnd();
	
	return videoStartResult;
}