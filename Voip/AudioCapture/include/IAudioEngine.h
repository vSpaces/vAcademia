#pragma once

#include "../AudioCapture.h"

struct IAudioEngine
{
	//virtual VOIPAC_ERROR GetCurrentOutAudioDeviceName( wchar_t* alpcwBuffer, unsigned int bufferSize) = 0;
	//virtual VOIPAC_ERROR GetDefaultInDeviceName( wchar_t * alpcwBuffer, unsigned int bufferSize) = 0;
	virtual VOIPAC_ERROR GetCurrentOutAudioDeviceGuid( wchar_t* alpcwBuffer, unsigned int bufferSize) = 0;
	virtual VOIPAC_ERROR GetDefaultInDeviceGuid( wchar_t * alpcwBuffer, unsigned int bufferSize) = 0;
	virtual VOIPAC_ERROR GetFirstActiveMicrophone( wchar_t * alpcwBuffer, unsigned int bufferSize) = 0;	
	virtual VOIPAC_ERROR GetFirstActiveNoMicrophone( wchar_t * alpcwBuffer, unsigned int bufferSize) = 0;	
	//virtual VOIPAC_ERROR SetCurrentOutAudioDeviceName( const wchar_t* alpcwDeviceName) = 0;
	virtual VOIPAC_ERROR SetCurrentOutAudioDeviceGuid( const wchar_t* alpcwDeviceName) = 0;
	virtual int GetCountInDevices() = 0;
	virtual int GetCountOutDevices() = 0;
	virtual VOIPAC_ERROR GetActiveMicrophones( std::vector<std::wstring> &av) = 0;
	virtual VOIPAC_ERROR GetActiveInDevices( std::vector<std::wstring> &av) = 0;
	virtual VOIPAC_ERROR GetActiveOutDevices( std::vector<std::wstring> &av) = 0;
	virtual VOIPAC_ERROR GetActiveNoMicInDevices( std::vector<std::wstring> &av) = 0;
};