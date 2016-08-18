#include "StdAfx.h"
#include "..\include\DefaultAudioDeviceManager.h"

CDefaultAudioDeviceManager::CDefaultAudioDeviceManager(void):
	MP_WSTRING_INIT(m_ReplacedDeviceName)
{
	OSVERSIONINFOEXW ovi;
	memset(&ovi, 0, sizeof(ovi));
	ovi.dwOSVersionInfoSize=sizeof(ovi);
	GetVersionExW(reinterpret_cast<OSVERSIONINFOW *>(&ovi));

	if ((ovi.dwMajorVersion < 6) || (ovi.dwBuildNumber < 6001))
		m_pCurrentAudioEngine = &xpAudioEngine;
	else
		m_pCurrentAudioEngine = &vistaAudioEngine;
}

CDefaultAudioDeviceManager::~CDefaultAudioDeviceManager(void)
{
}

VOIPAC_ERROR CDefaultAudioDeviceManager::ReplaceDefaultAudioDevice( const wchar_t* alpwOutDeviceName)
{
	#define AUDIO_DEVICE_NAME_LENGTH 1024
	wchar_t	deviceName[ AUDIO_DEVICE_NAME_LENGTH];

	if( !alpwOutDeviceName)
		return ACERROR_OUTDEVICENOTFOUND;

	VOIPAC_ERROR errorCode;
	errorCode = m_pCurrentAudioEngine->GetCurrentOutAudioDeviceGuid( deviceName, AUDIO_DEVICE_NAME_LENGTH);
	if( errorCode != ACERROR_NOERROR)
		return errorCode;

	if( wcscmp( deviceName, alpwOutDeviceName) == 0)
	{
		m_ReplacedDeviceName = alpwOutDeviceName;
		return ACERROR_DEFAULTALLREADY;
	}

	errorCode = m_pCurrentAudioEngine->SetCurrentOutAudioDeviceGuid( alpwOutDeviceName);
	if( errorCode != ACERROR_NOERROR)
		return errorCode;

	m_ReplacedDeviceName = deviceName;

	return ACERROR_NOERROR;
}

VOIPAC_ERROR CDefaultAudioDeviceManager::RestoreReplacedDevice()
{
	if( m_ReplacedDeviceName.empty())
		return ACERROR_DEFAULTALLREADY;

	VOIPAC_ERROR errorCode;
	errorCode = ReplaceDefaultAudioDevice( m_ReplacedDeviceName.c_str());
	if( errorCode == ACERROR_NOERROR)
	{
		m_ReplacedDeviceName.clear();
	}

	return errorCode;
}

/*VOIPAC_ERROR CDefaultAudioDeviceManager::GetDefaultInDeviceName( wchar_t * alpcwBuffer, unsigned int bufferSize)
{
	if( m_pCurrentAudioEngine == NULL)
		return ACERROR_NOTINITIALIZED;

	return m_pCurrentAudioEngine->GetDefaultInDeviceName( alpcwBuffer, bufferSize);
}*/

VOIPAC_ERROR CDefaultAudioDeviceManager::GetDefaultInDeviceGuid( wchar_t * alpcwBuffer, unsigned int bufferSize)
{
	if( m_pCurrentAudioEngine == NULL)
		return ACERROR_NOTINITIALIZED;

	return m_pCurrentAudioEngine->GetDefaultInDeviceGuid( alpcwBuffer, bufferSize);
}

VOIPAC_ERROR CDefaultAudioDeviceManager::GetFirstActiveMicrophone( wchar_t * alpcwBuffer, unsigned int bufferSize)
{
	if( m_pCurrentAudioEngine == NULL)
		return ACERROR_NOTINITIALIZED;

	return m_pCurrentAudioEngine->GetFirstActiveMicrophone( alpcwBuffer, bufferSize);
}

VOIPAC_ERROR CDefaultAudioDeviceManager::GetFirstActiveNoMicrophone( wchar_t * alpcwBuffer, unsigned int bufferSize)
{
	if( m_pCurrentAudioEngine == NULL)
		return ACERROR_NOTINITIALIZED;

	return m_pCurrentAudioEngine->GetFirstActiveNoMicrophone( alpcwBuffer, bufferSize);
}


VOIPAC_ERROR CDefaultAudioDeviceManager::GetMicrophones( std::vector<std::wstring> &av)
{
	if( m_pCurrentAudioEngine == NULL)
		return ACERROR_NOTINITIALIZED;

	return m_pCurrentAudioEngine->GetActiveMicrophones( av);
}

VOIPAC_ERROR CDefaultAudioDeviceManager::GetInDevices( std::vector<std::wstring> &av)
{
	if( m_pCurrentAudioEngine == NULL)
		return ACERROR_NOTINITIALIZED;

	return m_pCurrentAudioEngine->GetActiveInDevices( av);
}

VOIPAC_ERROR CDefaultAudioDeviceManager::GetOutDevices( std::vector<std::wstring> &av)
{
	if( m_pCurrentAudioEngine == NULL)
		return ACERROR_NOTINITIALIZED;

	return m_pCurrentAudioEngine->GetActiveOutDevices( av);
}

VOIPAC_ERROR CDefaultAudioDeviceManager::GetNoMicInDevices( std::vector<std::wstring> &av)
{
	if( m_pCurrentAudioEngine == NULL)
		return ACERROR_NOTINITIALIZED;

	return m_pCurrentAudioEngine->GetActiveNoMicInDevices( av);
}

const wchar_t* CDefaultAudioDeviceManager::GetReplacedAudioDeviceName()
{
	return m_ReplacedDeviceName.c_str();
}

VOIPAC_ERROR CDefaultAudioDeviceManager::RestoreReplacedDeviceAfterError( const wchar_t* alpwcDeviceName)
{
	if( !alpwcDeviceName)
		return ACERROR_OUTDEVICENOTFOUND;


	std::wstring	wsCurrentlyReplacedDevice = m_ReplacedDeviceName;

	VOIPAC_ERROR errorCode;
	errorCode = ReplaceDefaultAudioDevice( alpwcDeviceName);

	m_ReplacedDeviceName = wsCurrentlyReplacedDevice;

	return errorCode;
}

int CDefaultAudioDeviceManager::GetCountInDevices()
{
	if( m_pCurrentAudioEngine == NULL)
		return -1;
	return m_pCurrentAudioEngine->GetCountInDevices();
}

int CDefaultAudioDeviceManager::GetCountOutDevices()
{
	if( m_pCurrentAudioEngine == NULL)
		return -1;
	return m_pCurrentAudioEngine->GetCountOutDevices();
}