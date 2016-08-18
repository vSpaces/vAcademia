#include "StdAfx.h"
#include "../../WebCamGrabber/DevicesCtrl.h"
#include "../../include/selectors/videoinputselector.h"
#include "../../include/selectors/DesktopFrame.h"
#include "../../include/selectors/SafeRect.h"
#include "../../../nengine/ncommon/ThreadAffinity.h"


CVideoInputSelector::CVideoInputSelector(oms::omsContext* apContext, CDevicesCtrl* aDeviceController):
	MP_STRING_INIT(m_deviceDisplayName)
{
	context = apContext;
	m_currentFrame = NULL;
	m_firstCaptureFrame = true;
	m_deviceController = aDeviceController;
	MP_NEW_V2( m_deviceRunner, CVideoInputGraphRunning, this, true);
	m_runChecker = NULL;
	m_currentRect.SetRect(0,0,0,0);
	m_numActiveDevice = 0;
	m_deviceDisplayName = "_";
	m_deviceInitializing = false;
	m_needDeleteFromDeviceRunner = false;
	m_deviceWasStarted = false;
	m_isDeleted = false;

	csDestroy.Init();
}

CVideoInputSelector::~CVideoInputSelector(void)
{
	CleanUp();
	SAFE_DELETE(m_deviceRunner);
	SAFE_DELETE(m_runChecker);
	csDestroy.Term();
}

int CVideoInputSelector::GetType()
{
	return VIDEOIN_SELECTOR;
}

void CVideoInputSelector::SelectionComplete(int resultCode)
{
	// WEBCAMERA_ALREADY_USED - в m_deviceController устройство уже занято
	// USER_CHOSE_WEBCAMERA - пользователь выберет устройство из списка
	if (resultCode != WEBCAMERA_ALREADY_USED && resultCode != USER_CHOSE_WEBCAMERA)
		m_deviceWasStarted = true;
	InvokeEx<int, &IDesktopSelectorEvents::OnSelectionCompleted>(resultCode);
}

void CVideoInputSelector::SetResolutionComplete(int resultCode)
{
	// WEBCAMERA_ALREADY_USED - в m_deviceController устройство уже занято
	// USER_CHOSE_WEBCAMERA - пользователь выберет устройство из списка
	if (resultCode != WEBCAMERA_ALREADY_USED && resultCode != USER_CHOSE_WEBCAMERA)
		m_deviceWasStarted = true;
	InvokeEx<int, &IDesktopSelectorEvents::OnSelectionCompleted>(-resultCode);
}

// начинает выборку области
bool CVideoInputSelector::StartSourceSelection()
{
	bool deviceStartResult = false;
	m_deviceController->EnumerateDevices();
	if (m_deviceController->GetNumWebCameraDevices() > 1) // более одного устройства - пусть пользователь выбирает
	{
		SelectionComplete(USER_CHOSE_WEBCAMERA);
		return true;
	}
	// найдено одно устройство получаем его ИД и запускаем
	CRTBaseDevice* camDevice = NULL;
	camDevice = m_deviceController->GetWebCameraDevice(0);

	if (camDevice)
		m_deviceDisplayName = camDevice->GetDisplayDeviceName();

	m_deviceInitializing = true;
	m_deviceRunner->Start();
	return true;
}

// заканчивает выборку области
void CVideoInputSelector::CancelSourceSelection()
{
	if (m_deviceRunner->IsRunning() || m_deviceInitializing)
	{
		m_deviceRunner->SignalStop();
		SelectionComplete(WEBCAMERA_NOTRESPONCE);
	}
	if (m_runChecker)
		m_runChecker->SignalStop();
}

// возвращает последний полученный кадр
IDesktopFrame* CVideoInputSelector::GetCurrentFrame()
{
	return m_currentFrame;
}

// делает снимок нового кадра с текущими параметрами
IDesktopFrame* CVideoInputSelector::RecapFrame()
{
	void *imgBits = NULL;
	BITMAPINFO bmpInfo;
	CRect rect;
	CRTBaseDevice* webCam = m_deviceController->GetWebCameraDevice(m_deviceDisplayName);
	if (!webCam)
		return m_currentFrame;
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

	HBITMAP hCapturedImage = webCam->GetSnapshotFrame(m_firstCaptureFrame);
	m_firstCaptureFrame = false;
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

	return m_currentFrame;
}

// возвращает текущую область захвата
RECT CVideoInputSelector::GetCurrentSelectingRect()
{
	CRect rect(0,0,0,0);
	_WC_RESOLUTION resolution;
	resolution.height = 0;
	resolution.width = 0;

	if (m_currentRect.Width() == 0 && m_currentRect.Height() == 0 )
	{
		CRTBaseDevice* webCam = m_deviceController->GetWebCameraDevice(m_deviceDisplayName);
		if (!webCam)
			return rect; 

		std::vector<WC_RESOLUTION> supportedResolutions = webCam->GetSupportedResolutions();
		if (supportedResolutions.size() != 0)
			resolution = supportedResolutions[0];
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
bool CVideoInputSelector::IsCaptureAvailable()
{
	return true;
}

HWND CVideoInputSelector::GetCapturedWindow()
{
	return NULL;
}

bool CVideoInputSelector::Release()
{
	bool result = false;
	CRTBaseDevice* webCam = m_deviceController->GetWebCameraDevice(m_deviceDisplayName);
	if (m_deviceWasStarted && webCam)
		webCam->NeedStoping();
	csDestroy.Lock();
	if (!m_deviceInitializing)
	{
		result = true;
	}
	else
		m_needDeleteFromDeviceRunner = true;
	csDestroy.Unlock();
	if (result)
	{
		Destroy();
		return true;
	}
	return false;
}

void CVideoInputSelector::VideoInputGraphRunEnd()
{
	bool result = false;
	csDestroy.Lock();
	m_deviceInitializing = false;
	if (m_needDeleteFromDeviceRunner)
		result = true;
	csDestroy.Unlock();
	if (result)
	{
		CleanUp();
		m_isDeleted = true;
	}
}

void CVideoInputSelector::Destroy()
{
	MP_DELETE_THIS;
}

void CVideoInputSelector::CleanUp()
{
	ClearCapturedImage();
	CRTBaseDevice* webCam = m_deviceController->GetWebCameraDevice(m_deviceDisplayName);
	if (m_deviceWasStarted && webCam)
		webCam->Stop();
}

bool CVideoInputSelector::IsLive()
{
	return !m_isDeleted;
}

bool CVideoInputSelector::CanShowCaptureRect()
{
	return false;
}

void CVideoInputSelector::ClearCapturedImage()
{
	if( m_currentFrame != NULL)
	{
		MP_DELETE( m_currentFrame);
		m_currentFrame = NULL;
	}
}

CDevicesCtrl* CVideoInputSelector::GetDeviceController()
{
	return m_deviceController;
}

int CVideoInputSelector::GetWebNumsCameraDevices()
{
	return m_deviceController->GetNumWebCameraDevices();
}

std::string CVideoInputSelector::GetWebCameraName(int numDevice)
{
	return m_deviceController->GetWebCameraDevice(numDevice)->GetName();
}

std::vector<Resolution> CVideoInputSelector::GetWebCameraResolutions(int numDevice)
{
	CRTBaseDevice* camDevice = NULL;
	int i = 0;
	std::vector<WC_RESOLUTION> supportedWCResolutions;
	std::vector<Resolution> supportedResolutions;
	supportedResolutions.clear();
	camDevice = m_deviceController->GetWebCameraDevice(numDevice);
	if (!camDevice)
		return supportedResolutions;
	supportedWCResolutions = camDevice->GetSupportedResolutions();
	Resolution res;
	for (unsigned int i=0; i < supportedWCResolutions.size(); i++)
	{
		res.width = supportedWCResolutions[i].width;
		res.height = supportedWCResolutions[i].height;
		supportedResolutions.push_back(res);
	}
	return supportedResolutions;
}

bool CVideoInputSelector::SetResolution(Resolution newResolution, int numDevice, bool reRun)
{
	CRTBaseDevice* camDevice = NULL;
	m_deviceController->EnumerateDevices();
	camDevice = m_deviceController->GetWebCameraDevice(numDevice);
	if (!camDevice)
		return false;
	if (newResolution.width == 0 || newResolution.height == 0)
		return false;
	
	m_currentRect.left = 0;
	m_currentRect.right = newResolution.width;
	m_currentRect.top = 0;
	m_currentRect.bottom = newResolution.height;

	if (!reRun)
		return true;
	
	//bool result = camDevice->SetFrameSize(newResolution.width, newResolution.height);

	camDevice->Stop();
	
	SAFE_DELETE(m_runChecker);
	m_deviceInitializing = true;
	MP_NEW_V( m_runChecker, CVideoInputRunChecker, this);
	m_runChecker->Start();

	return true;
}

bool CVideoInputSelector::SetActiveDevice(int numDevice)
{
	CRTBaseDevice* camDevice = NULL;
	m_numActiveDevice = numDevice;
	camDevice = m_deviceController->GetWebCameraDevice(numDevice);
	
	if (!camDevice)
		return false;

	m_deviceDisplayName = camDevice->GetDisplayDeviceName();
	camDevice->Stop();

	SAFE_RELEASE( m_currentFrame);
	m_currentRect.SetRect(0,0,0,0);
	SAFE_DELETE(m_runChecker);
	m_deviceInitializing = true;
	MP_NEW_V( m_runChecker, CVideoInputRunChecker, this);
	m_runChecker->Start();

	return true;
}

//////////////////////////////////////////////////////////////////////////
//class CVideoInputRunChecker
//////////////////////////////////////////////////////////////////////////

CVideoInputRunChecker::CVideoInputRunChecker(CVideoInputSelector* selector)
{
	m_VISelector = selector;
	MP_NEW_V2( m_graphRunningThread, CVideoInputGraphRunning, m_VISelector, false);
}

CVideoInputRunChecker::~CVideoInputRunChecker(void)
{
	Stop();
	SAFE_DELETE(m_graphRunningThread);
}

// инициализация устройства
DWORD CVideoInputRunChecker::Run()
{
	DWORD resultCode = WEBCAMERA_NOTRESPONCE;
	HANDLE hndl = GetCurrentThread();
	SetSecondaryThreadAffinity( hndl);
	unsigned long startTime = timeGetTime();
	m_VISelector->m_firstCaptureFrame = true;
	m_graphRunningThread->Start();
	
	while( !ShouldStop())
	{
		if (!m_graphRunningThread->IsRunning())
		{
			m_graphRunningThread->GetExitCode(&resultCode);		
			break;
		}
		if (timeGetTime() - startTime > 20000)
		{
			m_graphRunningThread->SignalStop();
			resultCode = WEBCAMERA_NOTRESPONCE;
			break;
		}
	}
	m_graphRunningThread->SignalStop();
	if (resultCode != 0)
		m_VISelector->SetResolutionComplete(resultCode);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//class CVideoInputGraphRunning
//////////////////////////////////////////////////////////////////////////

CVideoInputGraphRunning::CVideoInputGraphRunning(CVideoInputSelector* selector, bool notifyAboutComplete)
{
	m_VISelector = selector;
	m_notifyAboutComplete = notifyAboutComplete;
}

CVideoInputGraphRunning::~CVideoInputGraphRunning(void)
{
//	Stop();
}

DWORD CVideoInputGraphRunning::Run()
{
	HANDLE hndl = GetCurrentThread();
	SetSecondaryThreadAffinity( hndl);
	DWORD resultCode = 0;
	int deviceStartResult = SUCCESSFULL;
	if( !ShouldStop())
	{
		CRTBaseDevice* webCam = m_VISelector->GetDeviceController()->GetWebCameraDevice(m_VISelector->m_deviceDisplayName);
		
		if (!webCam)
		{
			if (m_notifyAboutComplete)
				m_VISelector->SelectionComplete(WEBCAMERA_NOTFOUND);
			m_VISelector->VideoInputGraphRunEnd();
			return WEBCAMERA_NOTFOUND;
		}

		// ждем завершения работы устройства с другой сессии
		while (webCam->IsStarted() && webCam->IsNeedStoping())
		{
			if (ShouldStop())
				break;
			Sleep(20);
		}

		if (webCam->IsStarted())
			deviceStartResult = WEBCAMERA_ALREADY_USED;

		if (!webCam->IsStarted())
		{
			// устанавливаем разрешение до выполнения RenderStream() в webgrabere
			if (m_VISelector->m_currentRect.Width() != 0 && m_VISelector->m_currentRect.Height() != 0)
				webCam->SetFrameSize(m_VISelector->m_currentRect.Width(), m_VISelector->m_currentRect.Height());

			if( !ShouldStop())
				deviceStartResult = webCam->PreparationForStart();

			if (deviceStartResult == SUCCESSFULL)
			{
				if( !ShouldStop())
					deviceStartResult = webCam->Start(NULL);
			}
		}
	}
	if( !ShouldStop())
	{
		if (deviceStartResult == SUCCESSFULL)
		{
			if (m_VISelector->RecapFrame() == NULL)
				deviceStartResult = WEBCAMERA_CANNOTCAPTUREFRAME;
			else 
				deviceStartResult = SUCCESSFULL;
		}
		if (m_notifyAboutComplete)
			m_VISelector->SelectionComplete(deviceStartResult);
	}
	m_VISelector->VideoInputGraphRunEnd();
	return deviceStartResult;
}