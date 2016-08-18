#include "StdAfx.h"
#include ".\devicesctrl.h"
#include ".\CaptureVideo.h"

CDevicesCtrl::CDevicesCtrl(void):MP_VECTOR_INIT(m_webCameraDevicesMap)
{
	m_isEnumeratedDevices = false;
//	m_videoCap.UnIntializeVideo();
}

CDevicesCtrl::~CDevicesCtrl(void)
{
	StopDevices();
	m_videoCap.UnIntializeVideo();
	CRTBaseDevice* camDevice = NULL;
	for (m_webCamIterator=m_webCameraDevicesMap.begin(); m_webCamIterator!=m_webCameraDevicesMap.end(); m_webCamIterator++)
	{
		camDevice = *m_webCamIterator;
		m_webCameraDevicesMap.erase(m_webCamIterator);
		m_webCamIterator--;
		if (camDevice != NULL)
		{
			MP_DELETE( camDevice);			
		}
	}
}

void CDevicesCtrl::StopDevices()
{
	CRTBaseDevice* camDevice = NULL;
	if ((camDevice = GetWebCameraDevice()) != NULL)
	{
		do 
		{
			camDevice->Stop();
		} while ((camDevice = GetWebCameraNextDevice()) != NULL);
	}
}

bool	CDevicesCtrl::EnumerateDevices()
{
//	if (m_isEnumeratedDevices)
//		return true;
	m_isEnumeratedDevices = true;
	m_videoCap.InitializeVideo();

//	m_webCameraDevicesMap.clear();

	std::vector<CRTBaseDevice*> newDevicesMap;
	CRTBaseDevice* foundedDevice = NULL;
	newDevicesMap.clear();
	unsigned int i=0;
	// нет устройств чтобы проверить работу, приходили краши на инициализации
	/*unsigned int auTVTuners = m_videoCap.GetTVTunersCount();
	for( i=0; i < auTVTuners; i++)
	{
		MP_NEW_P(tvTuner, RTTVTunerDevice, i);
		if (tvTuner->GetName() == "vAcademiaCapture")
		{
			MP_DELETE( tvTuner);
			continue;
		}

		foundedDevice = IsMapContainsDevice(tvTuner);
		if (foundedDevice == NULL)
			newDevicesMap.push_back(tvTuner);
		else {
			((RTTVTunerDevice*)foundedDevice)->UpdateCameraIndex(i);
			newDevicesMap.push_back(foundedDevice);
			MP_DELETE( tvTuner);			
		}
	}*/

	unsigned int auWebCamerasCount = m_videoCap.GetVideoCamerasCount();
	for( i=0; i < auWebCamerasCount; i++)
	{
		MP_NEW_P( webCamera, RTWebCameraDevice, i);
		if (webCamera->GetName() == "vAcademiaCapture")
		{
			MP_DELETE( webCamera);
			continue;
		}
		foundedDevice = IsMapContainsDevice(webCamera);
		if (foundedDevice == NULL)
			newDevicesMap.push_back(webCamera);
		else {
			((RTWebCameraDevice*)foundedDevice)->UpdateCameraIndex(i);
			newDevicesMap.push_back(foundedDevice);			
			MP_DELETE( webCamera);
		}
	}

	m_webCameraDevicesMap.clear();
	m_webCameraDevicesMap = newDevicesMap;

	m_videoCap.UnIntializeVideo();

	return false;
}

CRTBaseDevice* CDevicesCtrl::GetWebCameraDevice()
{
	m_webCamIterator = m_webCameraDevicesMap.begin();
	if (m_webCameraDevicesMap.size() != 0)
		return m_webCameraDevicesMap[0];
	return NULL;
}

CRTBaseDevice* CDevicesCtrl::GetWebCameraNextDevice()
{
	if (m_webCamIterator != m_webCameraDevicesMap.end())
	{
		m_webCamIterator++;
		return *(m_webCamIterator-1);
	}
	return NULL;
}

int	CDevicesCtrl::GetNumWebCameraDevices()
{
	return (int)m_webCameraDevicesMap.size();
}

CRTBaseDevice* CDevicesCtrl::GetWebCameraDevice(int numDevice)
{
	if ((int)m_webCameraDevicesMap.size() > numDevice)
		return m_webCameraDevicesMap[numDevice];
	return NULL;
}

CRTBaseDevice* CDevicesCtrl::GetWebCameraDevice(std::string displayDeviceName)
{
	for (unsigned int i=0; i < m_webCameraDevicesMap.size(); i++)
	{
		if (m_webCameraDevicesMap[i]->GetDisplayDeviceName() == displayDeviceName)
			return m_webCameraDevicesMap[i];
	}
	return NULL;
}

CRTBaseDevice* CDevicesCtrl::IsMapContainsDevice( CRTBaseDevice* aDevice)
{
	for (unsigned int i=0; i < m_webCameraDevicesMap.size(); i++)
	{
		if (m_webCameraDevicesMap[i]->GetDisplayDeviceName() == aDevice->GetDisplayDeviceName())
			return m_webCameraDevicesMap[i];
	}
	return NULL;
}

