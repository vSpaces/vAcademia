#pragma once

#include "IAudioEngine.h"
#include "../AudioCapture.h"

class VOIPAUDIOCAPTURE_API CXPAudioEngine : public IAudioEngine
{
public:
	CXPAudioEngine(void);
	~CXPAudioEngine(void);

	// реализация IAudioEngine
public:
	VOIPAC_ERROR GetCurrentOutAudioDeviceGuid( wchar_t* alpcwBuffer, unsigned int bufferSize);
	VOIPAC_ERROR SetCurrentOutAudioDeviceGuid( const wchar_t* alpcwDeviceName);
	VOIPAC_ERROR GetDefaultInDeviceGuid( wchar_t * alpcwBuffer, unsigned int bufferSize);
	VOIPAC_ERROR GetFirstActiveMicrophone( wchar_t * alpcwBuffer, unsigned int bufferSize){ return ACERROR_NOTIMPLEMENTED;};
	VOIPAC_ERROR GetFirstActiveNoMicrophone( wchar_t * alpcwBuffer, unsigned int bufferSize){ return ACERROR_NOTIMPLEMENTED;};

	VOIPAC_ERROR GetActiveMicrophones(std::vector<std::wstring> &av){ return ACERROR_NOTIMPLEMENTED;};
	VOIPAC_ERROR GetActiveInDevices(std::vector<std::wstring> &av){ return ACERROR_NOTIMPLEMENTED;};
	VOIPAC_ERROR GetActiveOutDevices(std::vector<std::wstring> &av){ return ACERROR_NOTIMPLEMENTED;};
	VOIPAC_ERROR GetActiveNoMicInDevices(std::vector<std::wstring> &av){ return ACERROR_NOTIMPLEMENTED;};
	

	int GetCountInDevices(){ return -1;};
	int GetCountOutDevices(){ return -1;};

private:
	VOIPAC_ERROR GetDefaultDeviceGuidImpl( wchar_t* alpcwBuffer, unsigned int bufferSize, const wchar_t* apwcRegistryKey);
};
