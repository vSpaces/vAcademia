#if !defined(DXMIXER2_INCLUDED)
#define DXMIXER2_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "strmif.h"
#include "control.h"
#include "uuids.h"
#include "atlcomcli.h"
#include <xstring>
#include <vector>

class CDSMixer2
{
public:
	CDSMixer2();
	virtual ~CDSMixer2();

public:
	typedef	std::wstring				DeviceLineName;
	typedef	std::vector<DeviceLineName>	VecDeviceLines;

	/************************************************************************/
	/* Input devices
	/************************************************************************/
	VecDeviceLines	GetInDevices() const;
	VecDeviceLines	GetOutDevices() const;
	VecDeviceLines	GetInDeviceLines(const std::wstring& awsDeviceName) const;
	DeviceLineName	GetInDeviceCurrentLineName(const std::wstring& awsDeviceName) const;
	WORD			GetInLineVolume(const std::wstring& awsDeviceName, const std::wstring& awsLineName) const;
	BOOL			SetInLineVolume(const std::wstring& awsDeviceName, const std::wstring& awsLineName, WORD dwVol);
	BOOL			SetEnabledInDeviceLine(const std::wstring& awsDeviceName, const std::wstring& awsLineName, BOOL abEnable);
	BOOL			EnableOneInDeviceLine(const std::wstring& awsDeviceName, const std::wstring& awsLineName);

	/************************************************************************/
	/* Output devices
	/************************************************************************/
	VecDeviceLines	GetOutDeviceLines(const std::wstring& awsDeviceName) const;
	DeviceLineName	GetOutDeviceCurrentLineName(const std::wstring& awsDeviceName) const;
	WORD			GetOutLineVolume(const std::wstring& awsDeviceName, const std::wstring& awsLineName) const;
	BOOL			SetOutLineVolume(const std::wstring& awsDeviceName, const std::wstring& awsLineName, WORD dwVol);
	BOOL			SetCurrentOutDeviceLine(const std::wstring& awsDeviceName, const std::wstring& awsLineName, BOOL abEnable);

private:
	CComQIPtr<IAMAudioInputMixer>	GetInDeviceVolumeControl(const std::wstring& awsDeviceName) const;
	CComQIPtr<IAMAudioInputMixer>	GetInLineVolumeControl(const std::wstring& awsDeviceName, const std::wstring& awsLineName) const;

	//
	CDSMixer2::VecDeviceLines	GetDevicesImpl(BOOL abInDevices) const;
	VecDeviceLines	GetDeviceLinesImpl(const std::wstring& awsDeviceName, BOOL abInDevices) const;
	CComQIPtr<IAMAudioInputMixer>	GetDeviceVolumeControlImpl(const std::wstring& awsDeviceName, BOOL abInDevice) const;
	CComQIPtr<IAMAudioInputMixer>	GetLineVolumeControlImpl(const std::wstring& awsDeviceName, const std::wstring& awsLineName, bool abInDevice) const;

	CComQIPtr<IBaseFilter>	GetOutDeviceVolumeControlImpl(const std::wstring& awsDeviceName, BOOL abInDevice) const;
	CComQIPtr<IBaseFilter>	GetOutLineVolumeControlImpl(const std::wstring& awsDeviceName, const std::wstring& awsLineName, bool abInDevice) const;
};

#endif // !defined(DXMIXER_INCLUDED)