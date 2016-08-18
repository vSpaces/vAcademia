#pragma once

#include <xstring>
#include "../AudioCapture.h"
#include "XPAudioEngine.h"
#include "VistaAudioEngine.h"

class VOIPAUDIOCAPTURE_API CDefaultAudioDeviceManager
{
public:
	CDefaultAudioDeviceManager(void);
	~CDefaultAudioDeviceManager(void);

public:

	/** Заменяет текущее голосовое устройство на устройство Virtual Cable 1
	\return VOIPAC_ERROR
		ACERROR_NOERROR
		ACERROR_OUTDEVICENOTFOUND - не нашли устройство на которое нужно заменить
		ACERROR_DEVICECHANGEFAILED - не смогли заменить
		ACERROR_DEFAULTALLREADY - уже стоит такое устройство
	\details
	*/
	VOIPAC_ERROR ReplaceDefaultAudioDevice( const wchar_t* alpwOutDeviceName);

	/** Восстанавливает замененное голосовое устройство
	\return VOIPAC_ERROR
		ACERROR_NOERROR
		ACERROR_OUTDEVICENOTFOUND - не нашли устройство, которое нужно восстановить
		ACERROR_DEVICECHANGEFAILED - не смогли заменить
		ACERROR_DEFAULTALLREADY - не заменяли ничего
	\details
	*/
	VOIPAC_ERROR RestoreReplacedDevice();

	/** Восстанавливает замененное голосовое устройство
	\return VOIPAC_ERROR
		ACERROR_NOERROR
		ACERROR_OUTDEVICENOTFOUND - не нашли устройство, которое нужно восстановить
		ACERROR_DEVICECHANGEFAILED - не смогли заменить
		ACERROR_DEFAULTALLREADY - не заменяли ничего
	\details
	*/
	VOIPAC_ERROR RestoreReplacedDeviceAfterError( const wchar_t* alpwcDeviceName);

	/** Возвращает имя текущего устройства воспроизведения
	\param wchar_t * alpcwBuffer -
	\param unsigned int bufferSize -
	\return VOIPAC_ERROR
		ACERROR_NOERROR
	\details
	*/
	const wchar_t* GetReplacedAudioDeviceName();

	/** Возвращает имя текущего устройства записи
	\param wchar_t * alpcwBuffer -
	\param unsigned int bufferSize - size of input buffer in characters with last 0 character. So size == 1 can contains only END_OF_LINE == 0 character
		ACERROR_NOTINITIALIZED
		ACERROR_INDEVICENOTFOUND
		ACERROR_BUFFERTOOSMALL
		ACERROR_NOERROR
	*/
	VOIPAC_ERROR GetDefaultInDeviceGuid( wchar_t * alpcwBuffer, unsigned int bufferSize);
	//VOIPAC_ERROR GetDefaultInDeviceName( wchar_t * alpcwBuffer, unsigned int bufferSize);
	VOIPAC_ERROR GetFirstActiveMicrophone( wchar_t * alpcwBuffer, unsigned int bufferSize);
	VOIPAC_ERROR GetFirstActiveNoMicrophone( wchar_t * alpcwBuffer, unsigned int bufferSize);
	VOIPAC_ERROR GetMicrophones(std::vector<std::wstring> &av);
	VOIPAC_ERROR GetInDevices( std::vector<std::wstring> &av);
	VOIPAC_ERROR GetOutDevices( std::vector<std::wstring> &av);
	VOIPAC_ERROR GetNoMicInDevices( std::vector<std::wstring> &av);

	int GetCountInDevices();
	int GetCountOutDevices();

private:
	MP_WSTRING		m_ReplacedDeviceName;

	CXPAudioEngine		xpAudioEngine;
	CVistaAudioEngine	vistaAudioEngine;
	IAudioEngine*		m_pCurrentAudioEngine;
};
