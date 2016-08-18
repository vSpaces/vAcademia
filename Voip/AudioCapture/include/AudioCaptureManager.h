#pragma once

#include "../AudioCapture.h"

class CDefaultAudioDeviceManager;
class CAudioRepeater;

class VOIPAUDIOCAPTURE_API CAudioCaptureManager
{
public:
	CAudioCaptureManager(void);
	~CAudioCaptureManager(void);


public:

	/** Инициализирует захват звука, выполняя следующие шаги.
	1. Выполняет поиск устройства с именем Virtual Cable 1
	2. Если устройство не найдено возвращает ошибку
	3. Запоминает текущие устройства воспроизведения и захвата звука, для их дальнейшего восстановления
	4. Запускает AudioRepeater с Virtual Cable 1 на текущее устройство воспроизведения
	5. Устанавливает Virtual Cable 1 в качестве основного устройства воспроизведения и записи
	При возникновении ошибки в одном из шагов возвращает ошибку
	\return VOIPAC_ERROR
		ACERROR_NOERROR - все ок
		ACERROR_NOAUDIOCABLEINSTALLED - не найден инсталенный VAC
		ACERROR_REPEATERSTARTFAILED - не сработал шаг 4
		ACERROR_PRIMARYCHANGEFAILED - не сработал шаг 5
		ACERROR_ISBISY - уже капчится

	\details
	*/
	VOIPAC_ERROR	StartCapture();

	/** Прекращает захват звука и восстанавливает звуковые устройства по умолчанию
	1. Останавливает AudioRepeater с Virtual Cable 1 на текущее устройство воспроизведения
	2. Восстанавливает запомненные текущие устройства воспроизведения и захвата звука
	\return VOIPAC_ERROR
	ACERROR_NOERROR - все ок
	ACERROR_DEFAULTSRESTOREFAILED - не сработал шаг 2
	ACERROR_DEFAULTALLREADY - не был вызыван StartCapture
	\details
	*/
	VOIPAC_ERROR	StopCapture();

	/** Возвращает имя текущего устройства воспроизведения
	\param wchar_t * alpcwBuffer -
	\param unsigned int bufferSize -
	\return VOIPAC_ERROR
	ACERROR_NOERROR
	\details
	*/
	const wchar_t* GetReplacedAudioDeviceName();

	/** Восстанавливает устройство воспроизведения на указанное 
	\param wchar_t * alpcwBuffer -
	\param unsigned int bufferSize -
	\return VOIPAC_ERROR
		ACERROR_NOERROR
		ACERROR_OUTDEVICENOTFOUND - не нашли устройство, которое нужно восстановить
		ACERROR_DEVICECHANGEFAILED - не смогли заменить
		ACERROR_DEFAULTALLREADY - не заменяли ничего
	\details
	*/
	VOIPAC_ERROR	RestoreReplacedDeviceAfterError( const wchar_t* alpwcDeviceName);

private:
	bool							captureOnProgress;
	CDefaultAudioDeviceManager*		m_pDefaultAudioDeviceManager;
	CAudioRepeater*					m_pAudioRepeater;
};
