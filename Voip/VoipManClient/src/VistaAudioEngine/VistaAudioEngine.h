#pragma once

#include <vector>


class CVistaAudioEngine
{
	struct VISTA_AUDIO_DEVICE_DESC
	{
		MP_WSTRING	name;
		MP_WSTRING	guid;
		DWORD			state;

		VISTA_AUDIO_DEVICE_DESC():
			MP_WSTRING_INIT(name),
			MP_WSTRING_INIT(guid)
		{

		}
	};

public:
	CVistaAudioEngine(void);
	~CVistaAudioEngine(void);

	// реализация IAudioEngine
public:
	bool MakeExceptOneDeviceEnabled(ILogWriter* apLW);

private:
	void EnumerateAllInDevices();
	int GetDeviceCountByState(DWORD adwState);
	bool ActivateStereoMixer();
	bool ActivateAnyDevice();
	bool ActivateDeviceByGuid(const wchar_t* apwcID, const wchar_t* apwcName);

	std::wstring GetDeviceNameByGUID( const wchar_t* alpwcGuid) const;
	std::wstring GetDeviceGUIDByName( const wchar_t* alpwcName) const;

	//typedef std::vector<CVistaAudioEngine::VISTA_AUDIO_DEVICE_DESC>	VecAudioDevices;
	typedef MP_VECTOR<CVistaAudioEngine::VISTA_AUDIO_DEVICE_DESC> VecAudioDevices;
	typedef VecAudioDevices::iterator	VecAudioDevicesIt;
	typedef VecAudioDevices::const_iterator	VecAudioDevicesItConst;
	VecAudioDevices	vecAudioDevices;

	ILogWriter* pLW;
};
