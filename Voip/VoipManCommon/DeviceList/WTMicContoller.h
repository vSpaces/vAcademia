#pragma once

#include <wtypes.h>
#include <vector>

class CWTMicContoller
{
public:
	typedef	std::wstring	CDeviceName;
	typedef	std::vector<CDeviceName> CDevicesNamesVector;

public:
	CWTMicContoller(void);
	~CWTMicContoller(void);

public:
	//CDeviceName	GetCurrentInDeviceName(bool aCanEnumerate = false) const;
	CDeviceName		GetCurrentInDeviceGuid(bool aCanEnumerate) const;
	//CDeviceName	GetCurrentOutDeviceName() const;
	CDeviceName		GetCurrentOutDeviceGuid(bool aCanEnumerate) const;

	CDeviceName	GetDeviceMicrophoneName(const CDeviceName& deviceName) const;
	CDeviceName	GetDeviceMixerName(const CDeviceName& deviceName) const;

	CDeviceName	GetOutDeviceLineName(const CDeviceName& deviceName) const;
	
	CDevicesNamesVector	GetOutDeviceLineNames(const CDeviceName& deviceName) const;
	CDevicesNamesVector	GetDeviceMicrophoneNames(const CDeviceName& deviceName) const;
	CDevicesNamesVector	GetDeviceMixerNames(const CDeviceName& deviceName) const;

	bool	SetEnabledDeviceLine(const CDeviceName& deviceName, const CDeviceName& lineName, BOOL abEnabled) const;
	bool	SetDeviceVolume(const CDeviceName& deviceName) const;
	bool	SetDeviceLineVolume(const CDeviceName& deviceName, const CDeviceName& lineName, WORD auVolume) const;
	bool	EnableOneDeviceLine(const CDeviceName& deviceName, const CDeviceName& lineName) const;
	bool	EnableOneOutDeviceLine(const CDeviceName& deviceName, const CDeviceName& lineName) const;

	bool	IsExludeName(const wchar_t *deviceName) const;
	bool	IsMixerName(const wchar_t *deviceName) const;
};
