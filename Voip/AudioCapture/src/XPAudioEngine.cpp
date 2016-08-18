#include "StdAfx.h"
#include "../include/XPAudioEngine.h"
#include "../include/Helpers.h"
#include <xstring>
#include <dsound.h>

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
static bool setSetupPreferredAudioDevicesCount() 
{
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
						dwDataType, (BYTE *) &setupPreferredAudioDevicesCountKeyValue, dwSize))
					{
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
static bool setDefaultDeviceToRegistry(const wchar_t* deviceName, const wchar_t* registryKeyDeviceType)
{
	HKEY hKey;
	DWORD dwDisposition;
	bool ret = false;	//Return value, false by default

	if( !deviceName || (*deviceName) == 0)
		return false;
	if( !registryKeyDeviceType || (*registryKeyDeviceType) == 0)
		return false;

	if (ERROR_SUCCESS != ::RegOpenKeyExW(HKEY_CURRENT_USER, DEFAULT_PLAYBACK_DEVICE_REGISTRY_KEY, 
											0, KEY_ALL_ACCESS, &hKey))
	{
		if (ERROR_SUCCESS != ::RegCreateKeyExW(HKEY_CURRENT_USER, DEFAULT_PLAYBACK_DEVICE_REGISTRY_KEY,
												0, 0, REG_OPTION_NON_VOLATILE, KEY_WRITE, 0, &hKey, &dwDisposition))
		{
			return false;
		}
	}

	DWORD deviceNameLength = (DWORD)wcslen(deviceName)*sizeof(wchar_t);
	if (ERROR_SUCCESS == ::RegSetValueExW(hKey, registryKeyDeviceType, 0, REG_SZ,
							(const BYTE *) deviceName, deviceNameLength))
	{

		DWORD valuePreferredOnly = 0;
		if (ERROR_SUCCESS == ::RegSetValueExA(hKey, "PreferredOnly", 0, REG_DWORD, (BYTE *) &valuePreferredOnly, sizeof(DWORD))) 
		{
			if (ERROR_SUCCESS == ::RegSetValueExA(hKey, "SetupPreferredAudioDevicesCount", 0, REG_DWORD, (BYTE *) &valuePreferredOnly, sizeof(DWORD))) 
			{
				ret = true;
			}
		}

		setSetupPreferredAudioDevicesCount();
	}

	::RegCloseKey( hKey);

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
std::wstring getDefaultDeviceFromRegistry(const std::wstring & registryKeyDeviceType)
{
	HKEY hKey;
	std::wstring defaultDevice = L"";

	//Try to find the default audio device using the registry
	if (ERROR_SUCCESS == ::RegOpenKeyExW(HKEY_CURRENT_USER, DEFAULT_PLAYBACK_DEVICE_REGISTRY_KEY,
		0, KEY_QUERY_VALUE, &hKey))
	{

			DWORD dwDataType = REG_SZ;
			const DWORD dwInitialSize = 255;
			DWORD dwSize = dwInitialSize;
			wchar_t defaultDeviceKeyValue[dwInitialSize + 1];

			if (ERROR_SUCCESS == ::RegQueryValueExW(hKey, registryKeyDeviceType.c_str(), 0, &dwDataType,
				(BYTE *)&defaultDeviceKeyValue[0], &dwSize)) {

					defaultDevice = defaultDeviceKeyValue;
			}
			::RegCloseKey(hKey);			
	}

	//Cannot determine the default audio device
	return defaultDevice;
}


CXPAudioEngine::CXPAudioEngine(void)
{
}

CXPAudioEngine::~CXPAudioEngine(void)
{
}

VOIPAC_ERROR CXPAudioEngine::GetCurrentOutAudioDeviceGuid( wchar_t* alpcwBuffer, unsigned int bufferSize)
{
	return GetDefaultDeviceGuidImpl( alpcwBuffer, bufferSize, PLAYBACK_DEVICE_REGISTRY_KEY);
}

VOIPAC_ERROR CXPAudioEngine::GetDefaultInDeviceGuid( wchar_t* alpcwBuffer, unsigned int bufferSize)
{
	return GetDefaultDeviceGuidImpl( alpcwBuffer, bufferSize, RECORD_DEVICE_REGISTRY_KEY);
}

VOIPAC_ERROR CXPAudioEngine::GetDefaultDeviceGuidImpl( wchar_t* alpcwBuffer, unsigned int bufferSize, const wchar_t* apwcRegistryKey)
{
	std::wstring defaultDevice = getDefaultDeviceFromRegistry( apwcRegistryKey);
	wchar_t apwcGUID[512];
	ZeroMemory(apwcGUID, 512);
	if( defaultDevice.empty())
	{
		LPGUID pguid;
		VecOutDevicesParams vecDevices;
		if( FAILED( DirectSoundEnumerateW( &DirectSoundEnumerateAllRoutine, &vecDevices)))
			return ACERROR_ACCESSDENIED;
		if( vecDevices.size() == 0)
			return ACERROR_INDEVICENOTFOUND;
		else if( vecDevices.size() == 1)
			pguid = &vecDevices[ 0].outDeviceGuid;
		else
			pguid = &vecDevices[ 1].outDeviceGuid;

		apwcGUID[ 0] = 0;
		int pos = StringFromGUID2( *pguid, apwcGUID, 512);
		apwcGUID[ pos] = 0;
		_wcslwr_s(apwcGUID, pos + 1);
		defaultDevice = apwcGUID;
	}

	if( defaultDevice.length() > bufferSize - 1)
		return ACERROR_BUFFERTOOSMALL;
#if _MSC_VER >= 1400
	wcscpy_s( alpcwBuffer, bufferSize, defaultDevice.c_str());
#else
	wcscpy( alpcwBuffer, defaultDevice.c_str());
#endif
	return ACERROR_NOERROR;
}

VOIPAC_ERROR CXPAudioEngine::SetCurrentOutAudioDeviceGuid( const wchar_t* alpcwDeviceName)
{
	VecOutDevicesParams vecDevices;
	if( FAILED( DirectSoundEnumerateW( &DirectSoundEnumerateAllRoutine, &vecDevices)))
		return ACERROR_OUTDEVICENOTFOUND;

	VecOutDevicesParamsIt it = vecDevices.begin(), end = vecDevices.end();
	for (; it!=end; it++)
	{
		if( it->inDeviceName.compare( alpcwDeviceName) == 0)
			break;
	}
	if( it == end)
		return ACERROR_OUTDEVICENOTFOUND;
 
	bool errorCode;
	errorCode = setDefaultDeviceToRegistry( alpcwDeviceName, PLAYBACK_DEVICE_REGISTRY_KEY);

	if( !errorCode)
		return ACERROR_ACCESSDENIED;

	return ACERROR_NOERROR;
}