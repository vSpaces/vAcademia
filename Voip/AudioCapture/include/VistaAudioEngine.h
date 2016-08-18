#pragma once

#include "IAudioEngine.h"
#include "../AudioCapture.h"
#include <vector>

#if _MSC_VER >= 1400
//#include <mmdeviceapi.h>
#include <Mfobjects.h>        // IMFAudioMediatype
#include <MMDeviceApi.h>         // IMMDevice
#include <AudioEngineEndPoint.h> // AUDIO_ENDPOINT_CREATE_PARAMS
//#include <audiopolicyP.h>        // KSDATAFORMAT_WAVEFORMATEX

#endif


#if _MSC_VER >= 1400
	#include <mmdeviceapi.h>
#endif

struct VISTA_AUDIO_DEVICE_DESC
{
	MP_WSTRING name;
	MP_WSTRING guid;
	unsigned int type;

	VISTA_AUDIO_DEVICE_DESC():
		MP_WSTRING_INIT(name),
		MP_WSTRING_INIT(guid)
	{
		type = 0;
	}
};


class VOIPAUDIOCAPTURE_API CVistaAudioEngine : public IAudioEngine
{
public:
	CVistaAudioEngine(void);
	~CVistaAudioEngine(void);

	// реализация IAudioEngine
public:
	//VOIPAC_ERROR GetCurrentOutAudioDeviceName( wchar_t* alpcwBuffer, unsigned int bufferSize);
	//VOIPAC_ERROR GetDefaultInDeviceName( wchar_t * alpcwBuffer, unsigned int bufferSize);
	VOIPAC_ERROR GetCurrentOutAudioDeviceGuid( wchar_t* alpcwBuffer, unsigned int bufferSize);
	VOIPAC_ERROR GetDefaultInDeviceGuid( wchar_t * alpcwBuffer, unsigned int bufferSize);
	VOIPAC_ERROR GetFirstActiveMicrophone( wchar_t* alpcwBuffer, unsigned int bufferSize);
	VOIPAC_ERROR GetFirstActiveNoMicrophone( wchar_t* alpcwBuffer, unsigned int bufferSize);	
	VOIPAC_ERROR GetActiveMicrophones(std::vector<std::wstring> &av);
	VOIPAC_ERROR GetActiveInDevices(std::vector<std::wstring> &av);
	VOIPAC_ERROR GetActiveOutDevices(std::vector<std::wstring> &av);
	VOIPAC_ERROR GetActiveNoMicInDevices(std::vector<std::wstring> &av);

	VOIPAC_ERROR SetCurrentOutAudioDeviceGuid( const wchar_t* alpcwDeviceName);
	//VOIPAC_ERROR SetCurrentOutAudioDeviceName( const wchar_t* alpcwDeviceName);

	int GetCountInDevices();
	int GetCountOutDevices();

private:
	void EnumerateAllInDevices();
	void EnumerateAllOutDevices();

	typedef MP_VECTOR<VISTA_AUDIO_DEVICE_DESC> VecAudioDevices;
	typedef VecAudioDevices::iterator	VecAudioDevicesIt;
	typedef VecAudioDevices::const_iterator	VecAudioDevicesItConst;

#if _MSC_VER >= 1400
	void EnumerateAllDevicesImpl(EDataFlow dataFlow, VecAudioDevices& aDevices);	
	VOIPAC_ERROR GetCurrentAudioDeviceGuidImpl( wchar_t* alpcwBuffer, unsigned int bufferSize, EDataFlow dataFlow);
	//VOIPAC_ERROR GetCurrentAudioDeviceNameImpl( wchar_t* alpcwBuffer, unsigned int bufferSize, EDataFlow dataFlow);
	//VOIPAC_ERROR GetGuidFirstActiveDeviceByType( wchar_t* alpcwBuffer, unsigned int bufferSize, EndpointFormFactor dataType, bool aNoType);
	VOIPAC_ERROR GetFirstActiveDeviceByTypeImpl( wchar_t* alpcwBuffer, unsigned int bufferSize, EndpointFormFactor dataType, bool aNoType);
#endif

	std::wstring GetDeviceNameByGUID( const wchar_t* alpwcGuid, const VecAudioDevices& aDevices) const;
	std::wstring GetDeviceGUIDByName( const wchar_t* alpwcName, const VecAudioDevices& aDevices) const;

#if _MSC_VER >= 1400
	std::wstring GetGuidFirstActiveDeviceByType( EndpointFormFactor aType, const VecAudioDevices& aDevices, bool aNoType) const;
	//std::wstring GetNameFirstActiveDeviceByType( EndpointFormFactor aType, const VecAudioDevices& aDevices, bool aNoType) const;
	VOIPAC_ERROR GetActiveDevicesByType( EndpointFormFactor aType, const VecAudioDevices& aDevices, std::vector<std::wstring> &av) const;
	VOIPAC_ERROR GetActiveDevicesByNotType( EndpointFormFactor aType, const VecAudioDevices& aDevices, std::vector<std::wstring> &av) const;
#endif

	VecAudioDevices	vecAudioDevices;
	VecAudioDevices	vecCaptureAudioDevices;
};
