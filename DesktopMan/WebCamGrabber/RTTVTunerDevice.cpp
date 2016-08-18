#include "StdAfx.h"
#include ".\RTTVTunerDevice.h"
#include <algorithm>

const int RTTVTunerDevice::DEVICE_COMPOSITE = 1;
const int RTTVTunerDevice::DEVICE_SVIDEO = 2;
const int RTTVTunerDevice::DEVICE_TVTUNER = 3;

RTTVTunerDevice::RTTVTunerDevice( unsigned int auTunerIndex) : MP_VECTOR_INIT(resolutions), MP_VECTOR_INIT(supportedModes)
{
	m_uiTunerIndex = auTunerIndex;
	m_uiCurrentMode = RTTVTunerDevice::DEVICE_TVTUNER;

	supportedModes.push_back( RTTVTunerDevice::DEVICE_TVTUNER);
	supportedModes.push_back( RTTVTunerDevice::DEVICE_SVIDEO);
	supportedModes.push_back( RTTVTunerDevice::DEVICE_COMPOSITE);

	m_name = "TVTuner";
	m_displayName = videoCap.GetDisplayDeviceName(m_uiTunerIndex);
	if (m_displayName == "none")
	{
		m_displayName = ""; 
	}
}

RTTVTunerDevice::~RTTVTunerDevice(void)
{
}

void RTTVTunerDevice::UpdateCameraIndex(unsigned int auCameraIndex)
{
	m_uiTunerIndex = auCameraIndex;
}

int	RTTVTunerDevice::PreparationForStart()
{
	isStarted = true;

	for (unsigned int i = 0; i < supportedModes.size(); i++)
	{
		HRESULT hr = videoCap.InitializeVideo();
		if (FAILED(hr))
			return INIT_DIRECTSHOW_ERROR;
		if( StartSelectedTunerMode())
		{
			return SUCCESSFULL;
		}
		else
			videoCap.UnIntializeVideo();	
	}
	
	return INIT_WEBCAM_ERROR;
}

// Начинает работу устройства. После вызова метода можно получать данные
int	RTTVTunerDevice::StartImpl(HWND ahWnd)
{
//	videoCap.InitializeVideo();

// 	if( StartSelectedTunerMode())
// 	{
// 	//	videoCap.SetVideoWindowHandle( ahWnd);
// 		return true;
// 	}
	if( videoCap.RealRunTVTuner())
	{
		return SUCCESSFULL;
	}
	return INIT_WEBCAM_ERROR;
}

BOOL	RTTVTunerDevice::StartSelectedTunerMode()
{
	switch(m_uiCurrentMode) {
	case RTTVTunerDevice::DEVICE_COMPOSITE:
		return videoCap.StartCompositeVideo( m_uiTunerIndex);
	case RTTVTunerDevice::DEVICE_SVIDEO:
		return videoCap.StartSVideo( m_uiTunerIndex);
	case RTTVTunerDevice::DEVICE_TVTUNER:
		return videoCap.StartTVTuner( m_uiTunerIndex);
	default:
		ATLASSERT( FALSE);
	}
	return FALSE;
}

// Останавливает работу устройства. После вызова метода можно получать данные
void	RTTVTunerDevice::StopImpl()
{
//	videoCap.StopCapture();
	videoCap.UnIntializeVideo();
}

HWND	RTTVTunerDevice::CreateDialogPanel( HWND hParent)
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

HBITMAP	RTTVTunerDevice::GetSnapshotFrame(bool isFirstFrame)
{
	return videoCap.MakeSnapshot(isFirstFrame);
}

BOOL	RTTVTunerDevice::SetVideoMode( TUNER_MODE auMode)
{
	m_uiCurrentMode = auMode;
	return Start(m_hWnd);
}

LPCTSTR RTTVTunerDevice::GetVideoModeName( TUNER_MODE auMode)
{
	switch(auMode) {
		case RTTVTunerDevice::DEVICE_COMPOSITE:
			return "Composite video";
		case RTTVTunerDevice::DEVICE_SVIDEO:
			return "SVideo";
		case RTTVTunerDevice::DEVICE_TVTUNER:
			return "TVTuner";
		default:
			ATLASSERT( FALSE);
	}
	return "";
}

/*void	RTTVTunerDevice::EnumerateResolutions()
{
	BOOL resolutionsHasEnumerated = FALSE;

	RTWebCameraDevice::vecResolutions	resolutions;

	CComPtr<ICaptureGraphBuilder2>	pBuilder = videoCap.GetICaptureGraphBuilder2TV();
	if( !pBuilder)
		return;

	CComPtr<IBaseFilter>	pSource = videoCap.GetSourceFilter();
	if( !pSource)
		return;

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
			resolutions.push_back( resolution);
	}

	//std::sort( resolutions.begin(), resolutions.end());

	resolutionsHasEnumerated = TRUE;
}*/