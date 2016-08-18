#include "StdAfx.h"
#include ".\rtwebcameradevice.h"
#include <algorithm>

RTWebCameraDevice::RTWebCameraDevice( unsigned int auCameraIndex):MP_VECTOR_INIT(resolutions)
{
	m_uiCameraIndex = auCameraIndex;
	resolutionsHasEnumerated = FALSE;

	m_displayName = videoCap.GetDisplayDeviceName(m_uiCameraIndex);
	if (m_displayName == "none")
	{
		m_displayName = ""; 
	}
	m_name = videoCap.GetWebCamName(m_uiCameraIndex);
}

RTWebCameraDevice::~RTWebCameraDevice(void)
{
}

void RTWebCameraDevice::UpdateCameraIndex(unsigned int auCameraIndex)
{
	m_uiCameraIndex = auCameraIndex;
}

int	RTWebCameraDevice::PreparationForStart()
{
	isStarted = true;

	HRESULT hr = videoCap.InitializeVideo();
	if (FAILED(hr))
		return INIT_DIRECTSHOW_ERROR;
	if( videoCap.StartWebcam( m_uiCameraIndex))
	{
		return SUCCESSFULL;
	}
	return INIT_WEBCAM_ERROR;
}

// Ќачинает работу устройства. ѕосле вызова метода можно получать данные
int	RTWebCameraDevice::StartImpl(HWND ahWnd)
{
/*	HRESULT hr = videoCap.InitializeVideo();
	if (FAILED(hr))
		return INIT_DIRECTSHOW_ERROR;

	if( videoCap.StartWebcam( m_uiCameraIndex))
	{
	//	videoCap.SetVideoWindowHandle( ahWnd);
		return SUCCESSFULL;
	}
	*/
	if( videoCap.RealRunWebcam( m_uiCameraIndex))
	{
		return SUCCESSFULL;
	}
	return INIT_WEBCAM_ERROR;
}

void	RTWebCameraDevice::StopImpl()
{
//	videoCap.StopCapture();
	videoCap.UnIntializeVideo();
}

HWND	RTWebCameraDevice::CreateDialogPanel( HWND hParent)
{
	if( !IsStarted())
		return false;
/*
	if( dialog.IsWindow())
		return dialog.m_hWnd;

	if( dialog.Create( hParent, CWindow::rcDefault, NULL))
	{
		dialog.SetActiveDevice( this);
		dialog.SetCapVideoToResize( &videoCap);
		videoCap.SetVideoWindowHandle( dialog.GetVideoWindowHandle());
		return dialog.m_hWnd;
	}
	*/
	return 0;
}

HBITMAP	RTWebCameraDevice::GetSnapshotFrame(bool isFirstFrame)
{
	return videoCap.MakeSnapshot(isFirstFrame);
}

bool	RTWebCameraDevice::SetFrameSize( unsigned int auWidth, unsigned int auHeight)
{
	videoCap.SetFrameSize( auWidth, auHeight);
	return true;
	//проверку перенесена в  CCaptureVideo
/*
	if( !resolutionsHasEnumerated)
		EnumerateResolutions();

	WC_RESOLUTION	resolution = { auWidth, auHeight};
	if( std::find( resolutions.begin(), resolutions.end(), resolution) != resolutions.end())
	{
		// данные размеры поддерживаютс€ устройством
		videoCap.SetFrameSize( auWidth, auHeight);
		return true;
	}
	return false;
	*/
}

int	RTWebCameraDevice::EnumerateResolutions()
{
	resolutionsHasEnumerated = FALSE;
	resolutions.clear();

	CComPtr<ICaptureGraphBuilder2>	pBuilder = videoCap.GetICaptureGraphBuilder2WC();
	if( !pBuilder)
		return ENUMERATE_RESOLUTION_ERROR;

	CComPtr<IBaseFilter>	pSource = videoCap.GetSourceFilter();
	if( !pSource)
		return ENUMERATE_RESOLUTION_ERROR;

	CComPtr<IAMStreamConfig> pConfig;
	HRESULT hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
										NULL, pSource,
										IID_IAMStreamConfig, (void **)&pConfig);

	int iCount = 0;
	int iSize = sizeof(VIDEO_STREAM_CONFIG_CAPS);
	pConfig->GetNumberOfCapabilities( &iCount, &iSize);

	for (int i=0; i<iCount; i++)
	{
		AM_MEDIA_TYPE *pmt = 0;
		VIDEO_STREAM_CONFIG_CAPS caps;
		if (FAILED(pConfig->GetStreamCaps(i, &pmt, (byte*)&caps)))
			continue;

		WC_RESOLUTION	resolution = { caps.InputSize.cx, caps.InputSize.cy};

		if( std::find( resolutions.begin(), resolutions.end(), resolution) == resolutions.end())
		{
			resolutions.push_back( resolution);
		}
	}

	std::sort( resolutions.begin(), resolutions.end());

	resolutionsHasEnumerated = TRUE;
	return SUCCESSFULL;
}