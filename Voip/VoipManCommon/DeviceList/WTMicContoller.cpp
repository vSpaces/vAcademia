#include "StdAfx.h"
#include "DeviceList\wtmiccontoller.h"
#include <mmsystem.h>

#include "..\Mixer\Mixer.h"
#include "..\Mixer\DSMixer.h"
#include "..\Mixer\DSMixer2.h"

/************************************************************************/
/* External functions
/************************************************************************/
CWTMicContoller::CDeviceName getDefaultInputDevice(bool aCanEnumerate);
CWTMicContoller::CDeviceName getDefaultOutputDevice(bool aCanEnumerate);

CWTMicContoller::CDevicesNamesVector getMixerDevicesList(CWTMicContoller::CDevicesNamesVector& listDevices, DWORD targetType);

/************************************************************************/
/* Interface class
/************************************************************************/
CWTMicContoller::CWTMicContoller(void)
{
}

CWTMicContoller::~CWTMicContoller(void)
{
}

/*CWTMicContoller::CDeviceName	CWTMicContoller::GetCurrentInDeviceName(bool aCanEnumerate) const
{
	return getDefaultInputDevice(aCanEnumerate);
}*/

CWTMicContoller::CDeviceName	CWTMicContoller::GetCurrentInDeviceGuid(bool aCanEnumerate) const
{
	return getDefaultInputDevice(aCanEnumerate);
}

/*CWTMicContoller::CDeviceName	CWTMicContoller::GetCurrentOutDeviceName() const
{
	return getDefaultOutputDevice();
}*/

CWTMicContoller::CDeviceName	CWTMicContoller::GetCurrentOutDeviceGuid(bool aCanEnumerate) const
{
	return getDefaultOutputDevice( aCanEnumerate);
}

CWTMicContoller::CDeviceName	CWTMicContoller::GetDeviceMicrophoneName(const CDeviceName& deviceName) const
{
	std::vector<std::wstring> microphones = CMixer::GetMicrophoneNames( deviceName);
	// вернем первый микрофон
	if( microphones.size() > 0)
		return microphones[0];
	return L"";
}

CWTMicContoller::CDeviceName	CWTMicContoller::GetOutDeviceLineName(const CDeviceName& deviceName) const
{
	std::vector<std::wstring> lines = CMixer::GetOutputLineNames( deviceName, MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT);
	// вернем первую линию
	if( lines.size() > 0)
		return lines[0];
	return L"";
}

CWTMicContoller::CDevicesNamesVector	CWTMicContoller::GetOutDeviceLineNames(const CDeviceName& deviceName) const
{
	return CMixer::GetOutputLineNames( deviceName, MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT);	
}

CWTMicContoller::CDevicesNamesVector	CWTMicContoller::GetDeviceMicrophoneNames(const CDeviceName& deviceName) const
{
	return CMixer::GetMicrophoneNames( deviceName);	
}

CWTMicContoller::CDevicesNamesVector	CWTMicContoller::GetDeviceMixerNames(const CDeviceName& deviceName) const
{
	return CMixer::GetMixersNames( deviceName);	
}

CWTMicContoller::CDeviceName	CWTMicContoller::GetDeviceMixerName(const CDeviceName& deviceName) const
{
	std::vector<std::wstring> mixers = CMixer::GetMixersNames( deviceName);
	// вернем первый микшер
	if( mixers.size() > 0)
	{
		CDSMixer::WriteLogNoNewLine("Mixer FOUND:"); 
		CDSMixer::WriteLog( mixers[0]);
		return mixers[0];
	}
	else
	{
		CDSMixer::WriteLog( "Mixer not found");
	}
	return L"";
}

bool	CWTMicContoller::SetEnabledDeviceLine(const CDeviceName& deviceName, const CDeviceName& lineName, BOOL abEnabled) const
{
	CDSMixer2 dsMixer2;
	return dsMixer2.SetEnabledInDeviceLine( deviceName, lineName, abEnabled) != 0;
}

bool	CWTMicContoller::SetDeviceLineVolume(const CDeviceName& deviceName, const CDeviceName& lineName, WORD auVolume) const
{
	CDSMixer2 dsMixer2;
	return dsMixer2.SetInLineVolume( deviceName, lineName, auVolume) != 0;
}

bool	CWTMicContoller::EnableOneDeviceLine(const CDeviceName& deviceName, const CDeviceName& lineName) const
{
	CDSMixer2 dsMixer2;
	return dsMixer2.EnableOneInDeviceLine( deviceName, lineName) != 0;		
}

bool	CWTMicContoller::EnableOneOutDeviceLine(const CDeviceName& deviceName, const CDeviceName& lineName) const
{
	return CMixer::SetOutputDeviceLine( deviceName, lineName);
}

bool	CWTMicContoller::IsExludeName(const wchar_t *deviceName) const
{
	return CMixer::IsExludeName( deviceName);	
}

bool	CWTMicContoller::IsMixerName(const wchar_t *deviceName) const
{
	return CMixer::IsMixerName( deviceName);	
}