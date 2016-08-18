#include "StdAfx.h"
#include "DeviceList\wtmiccontoller.h"
#include "..\Mixer\DSMixer2.h"
#include <wtypes.h>
#include <mmsystem.h>
/**
* Registry path for the default audio device.
*/
static const wchar_t * DEFAULT_PLAYBACK_DEVICE_REGISTRY_KEY = L"Software\\Microsoft\\Multimedia\\Sound Mapper\\";

/**
* Registry key for the default playback audio device.
*/
static const wchar_t * PLAYBACK_DEVICE_REGISTRY_KEY = L"Playback";

/**
* Registry key for the default record audio device.
*/
static const wchar_t * RECORD_DEVICE_REGISTRY_KEY = L"Record";

/**
* Default audio device ID under Windows.
*/
static const int DEFAULT_DEVICE_ID = 0;

/**
* Sets the SetupPreferredAudioDevicesCount registry key.
*
* Part of setDefaultDevice()
*
* @return true if the registry key was changed, false otherwise
*/
static bool setSetupPreferredAudioDevicesCount() {
	static const char * SETUPPREFERREDAUDIODEVICESCOUNT_REGISTRY_KEY =
		"SYSTEM\\CurrentControlSet\\Control\\MediaResources\\SetupPreferredAudioDevices\\";
	HKEY hKey;

	if (ERROR_SUCCESS == ::RegOpenKeyExA(HKEY_LOCAL_MACHINE, SETUPPREFERREDAUDIODEVICESCOUNT_REGISTRY_KEY,
		0, KEY_ALL_ACCESS, &hKey)) {

			DWORD dwDataType = REG_DWORD;
			DWORD dwSize = 4;
			DWORD setupPreferredAudioDevicesCountKeyValue = 0;

			if (ERROR_SUCCESS == ::RegQueryValueExW(hKey, L"SetupPreferredAudioDevicesCount", 0, &dwDataType,
				(BYTE *) &setupPreferredAudioDevicesCountKeyValue, &dwSize)) {

					::RegCloseKey(hKey);

					::RegOpenKeyExW(HKEY_CURRENT_USER, DEFAULT_PLAYBACK_DEVICE_REGISTRY_KEY,
						0, KEY_ALL_ACCESS, &hKey);

					DWORD dwDisposition;
					::RegCreateKeyExW(HKEY_CURRENT_USER, DEFAULT_PLAYBACK_DEVICE_REGISTRY_KEY,
						0, 0, REG_OPTION_NON_VOLATILE,
						KEY_WRITE, 0, &hKey, &dwDisposition);

					if (ERROR_SUCCESS == ::RegSetValueExW(hKey, L"SetupPreferredAudioDevicesCount", 0,
						dwDataType, (BYTE *) &setupPreferredAudioDevicesCountKeyValue, dwSize)) {

							::RegCloseKey(hKey);
							return true;
						}
				}
		}

		::RegCloseKey(hKey);
		return false;
}

/**
* Sets the default audio device.
*
* Uses the Windows registry.
*
* There is no documentation at all about how to set a default audio device.
* This is an ugly trick but it seems to work...
*
* @param deviceName name of the default audio device
* @param registryKeyDeviceType registry key name, should be "Playback" or "Record"
* @return true if the default audio device was changed, false if an error occured
*/
static bool setDefaultDeviceToRegistry(const std::wstring & deviceName, const std::wstring & registryKeyDeviceType) {
	HKEY hKey;
	DWORD dwDisposition;
	bool ret = false;	//Return value, false by default

	::RegOpenKeyExW(HKEY_CURRENT_USER, DEFAULT_PLAYBACK_DEVICE_REGISTRY_KEY,
		0, KEY_ALL_ACCESS, &hKey);
	::RegCreateKeyExW(HKEY_CURRENT_USER, DEFAULT_PLAYBACK_DEVICE_REGISTRY_KEY,
		0, 0, REG_OPTION_NON_VOLATILE,
		KEY_WRITE, 0, &hKey, &dwDisposition);

	WORD deviceNameLength = deviceName.length()*sizeof(wchar_t);
	if (ERROR_SUCCESS == ::RegSetValueExW(hKey, registryKeyDeviceType.c_str(), 0, REG_SZ,
		(const BYTE *) deviceName.c_str(), deviceNameLength)) {
			ret = ret && true;
		}
		if (ERROR_SUCCESS == ::RegSetValueExW(hKey, L"UserPlayback", 0, REG_SZ,
			(const BYTE *) deviceName.c_str(), deviceNameLength)) {
				ret = ret && true;
			}
			if (ERROR_SUCCESS == ::RegSetValueExW(hKey, L"UserRecord", 0, REG_SZ,
				(const BYTE *) deviceName.c_str(), deviceNameLength)) {
					ret = ret && true;
				}

				/*DWORD valuePreferredOnly = 0;	//Why 0? I have no idea
				if (ERROR_SUCCESS == ::RegSetValueExA(hKey, "PreferredOnly", 0,
				REG_DWORD, (BYTE *) &valuePreferredOnly, sizeof(DWORD))) {
				ret = true && ret;
				}*/

				::RegCloseKey(hKey);

				/*ret = ret && */setSetupPreferredAudioDevicesCount();

				return ret;
}

/**
* Gets the default audio device given its registry key.
*
* Uses the Windows registry.
*
* Reads the registry keys:
* HKEY_CURRENT_USER\Software\Microsoft\Multimedia\Sound Mapper\Playback
* HKEY_CURRENT_USER\Software\Microsoft\Multimedia\Sound Mapper\Record
*
* @param registryKeyDeviceType registry key name, should be "Playback" or "Record"
* @return the default device from the registry key or null
*/
static CWTMicContoller::CDeviceName getDefaultDeviceFromRegistry(const std::wstring & registryKeyDeviceType) {
	HKEY hKey;

	//Try to find the default audio device using the registry
	if (ERROR_SUCCESS == ::RegOpenKeyExW(HKEY_CURRENT_USER, DEFAULT_PLAYBACK_DEVICE_REGISTRY_KEY,
		0, KEY_QUERY_VALUE, &hKey)) {

			DWORD dwDataType = REG_SZ;
			const DWORD dwInitialSize = 255;
			DWORD dwSize = dwInitialSize;
			wchar_t defaultDeviceKeyValue[dwInitialSize + 1];

			if (ERROR_SUCCESS == ::RegQueryValueExW(hKey, registryKeyDeviceType.c_str(), 0, &dwDataType,
				(BYTE *)&defaultDeviceKeyValue[0], (DWORD*)&dwSize)) {

					::RegCloseKey(hKey);
					return defaultDeviceKeyValue;
				}				
		}

		//Cannot determine the default audio device
		return L"";
}

CWTMicContoller::CDeviceName getDefaultInputDevice(bool aCanEnumerate)
{
	//CWTMicContoller::CDeviceName defaultDeviceName(getDefaultDeviceFromRegistry(RECORD_DEVICE_REGISTRY_KEY));
	CWTMicContoller::CDeviceName defaultDeviceName;
	// чтобы не было повисаний/подвисаний академии aCanEnumerate - true только при вызове из Voipman.exe
	if (aCanEnumerate) 
	{
		if (defaultDeviceName.empty())
		{
			CDSMixer2 dsMixer2;
			CDSMixer2::VecDeviceLines	lines = dsMixer2.GetInDevices();
			if( lines.size() > 1)
				defaultDeviceName = lines[1];
		}
	}
	return defaultDeviceName;
}

CWTMicContoller::CDeviceName getDefaultOutputDevice(bool aCanEnumerate)
{
	/*CWTMicContoller::CDeviceName defaultDeviceName(getDefaultDeviceFromRegistry(PLAYBACK_DEVICE_REGISTRY_KEY));
	if (defaultDeviceName.empty()) {
		WAVEOUTCAPSW outcaps;
		//We didn't find the default record audio device using the registry
		//Try using the device id = 0 => should be the default record audio device
		if (MMSYSERR_NOERROR == ::waveOutGetDevCapsW(DEFAULT_DEVICE_ID, &outcaps, sizeof(WAVEOUTCAPSW)))
		{
			wchar_t * deviceName = _wcsdup(outcaps.szPname);
			defaultDeviceName = deviceName;
		}
	}*/
	CWTMicContoller::CDeviceName defaultDeviceName;
	// чтобы не было повисаний/подвисаний академии aCanEnumerate - true только при вызове из Voipman.exe
	if (aCanEnumerate) 
	{
		if (defaultDeviceName.empty())
		{
			CDSMixer2 dsMixer2;
			CDSMixer2::VecDeviceLines	lines = dsMixer2.GetOutDevices();
			if( lines.size() > 1)
				defaultDeviceName = lines[1];
		}
	}
	return defaultDeviceName;
	return defaultDeviceName;
}

CWTMicContoller::CDevicesNamesVector getMixerDevicesList(CWTMicContoller::CDevicesNamesVector& listDevices, DWORD targetType) 
{
	listDevices.clear();

	unsigned nbDevices = ::mixerGetNumDevs();
	MIXERCAPSW mixercaps;	ZeroMemory(&mixercaps, sizeof(mixercaps));

	//For all the mixer devices
	for (unsigned deviceId = 0; deviceId < nbDevices; deviceId++) {

		//Gets the capacities of the mixer device
		if (MMSYSERR_NOERROR == ::mixerGetDevCapsW(deviceId, &mixercaps, sizeof(MIXERCAPSW))) {

			//For all the destinations available through the mixer device
			for (unsigned i = 0; i < mixercaps.cDestinations; i++) {
				MIXERLINEW mixerline;
				mixerline.cbStruct = sizeof(MIXERLINEW);
				mixerline.dwDestination = i;

				//Handle identifying the opened mixer device
				HMIXER hMixer;

				//Opens the mixer in order to get the handle identifying the opened mixer device
				if (MMSYSERR_NOERROR == ::mixerOpen(&hMixer, deviceId, NULL, NULL, MIXER_OBJECTF_MIXER)) {

					//Gets all information about the mixer line
					if (MMSYSERR_NOERROR == ::mixerGetLineInfoW((HMIXEROBJ) hMixer, &mixerline, MIXER_GETLINEINFOF_DESTINATION)) {

						//Checks if the mixer line is of type targetType
						if (mixerline.Target.dwType == targetType) {
							listDevices.push_back(std::wstring(mixercaps.szPname));
						}
					}
					//Closes the opened mixer device
					::mixerClose(hMixer);
				}
			}
		}
	}
	return listDevices;
}

CWTMicContoller::CDevicesNamesVector getOutputDeviceList(CWTMicContoller::CDevicesNamesVector& listDevices) 
{
	return getMixerDevicesList( listDevices, MIXERLINE_TARGETTYPE_WAVEOUT);
}

CWTMicContoller::CDevicesNamesVector getInputDeviceList(CWTMicContoller::CDevicesNamesVector& listDevices) 
{
	return getMixerDevicesList( listDevices, MIXERLINE_TARGETTYPE_WAVEIN);
}