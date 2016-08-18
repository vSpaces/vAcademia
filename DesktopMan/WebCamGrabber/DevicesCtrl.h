#pragma once

#include ".\RTWebCameraDevice.h"
#include ".\RTTVTunerDevice.h"

class CDevicesCtrl
{
public:
	CDevicesCtrl(void);
	~CDevicesCtrl(void);
	CRTBaseDevice* GetWebCameraDevice();
	CRTBaseDevice* GetWebCameraNextDevice();
	CRTBaseDevice* GetWebCameraDevice(int numDevice);
	CRTBaseDevice* GetWebCameraDevice(std::string displayDeviceName);
	void StopDevices();
public:
	// ¬ыполн€ет поиск установленных устройств
	bool	EnumerateDevices();
	int		GetNumWebCameraDevices();

private:
	CRTBaseDevice* IsMapContainsDevice( CRTBaseDevice* aDevice);

	CCaptureVideo	m_videoCap;
 	MP_VECTOR<CRTBaseDevice*> m_webCameraDevicesMap;
	std::vector<CRTBaseDevice*>::iterator m_webCamIterator;
	bool	m_isEnumeratedDevices;
};
