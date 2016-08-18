#ifndef __VOIPMAN_H_
#define __VOIPMAN_H_

#ifdef VOIPMAN_EXPORTS
#define VOIPMAN_API __declspec(dllexport)
#else
#define VOIPMAN_API __declspec(dllimport)
#endif

#ifdef WIN32
#define VOIPMAN_NO_VTABLE __declspec(novtable)
#else
#define VOIPMAN_NO_VTABLE
#endif

#include "oms_context.h"
#include "rmml.h"

using namespace rmml;

namespace voip
{
	typedef DWORD voipError;
	
	enum voipServerSessionType
	{ 
		VSST_RECORDING,
		VSST_PLAYING,
		VSST_SELF_RECORDING
	};

	enum voipAudioSource
	{ 
		VAS_NONE,
		VAS_MICROPHONE,
		VAS_MIXER,
		VAS_VIRTUALCABLE
	};

	enum voipPlayConnectionStatus
	{ 
		VPCS_DISCONNECTED = 0,
		VPCS_CONNECTED
	};

	enum voipSRModeStatusChanged
	{ 
		VSR_DISCONNECTED = 0,
		VSR_CONNECTING,
		VSR_CONNECT_FAILED,
		VSR_CONNECTED,
		VSR_DISCONNECTING
	};

	enum voipEvoClientWorkState
	{
		ECWS_NOT_LAUNCHED = 0,
		ECWS_LAUNCHED,
		ECWS_LAUNCH_FAILED,
		ECWS_CONNECTED_TO_CHANNEL,
		ECWS_CLOSING,
		ECWS_CLOSED
	};

	enum voipEvoClientSessionType
	{
		ECST_PLAYING = 0,
		ECST_RECORDIND,
		ECST_SELFRECORDING
	};

	struct VOIPMAN_NO_VTABLE IVoipCharacter
	{
		virtual void handleVoiceData( int nSampleRate, const short* pRawAudio, int nSamples) = 0;		
		virtual ml3DPosition getAbsolutePosition() = 0;
		virtual ml3DRotation getAbsoluteRotation() = 0;
		virtual  bool isSayingAudioFile(LPCSTR  alpcFileName) = 0;
	};

	struct VOIPMAN_NO_VTABLE IVoip3DSoundCalculator
	{
		virtual bool calculatePosition( voip::IVoipCharacter* pMyIVoipCharacter, voip::IVoipCharacter* pIVoipCharacter, ml3DPosition &userPos) = 0;
	};

	struct VOIPMAN_NO_VTABLE voipIVoipManCallbacks
	{
		virtual void onConnectSuccess() = 0;
		virtual void onConnectStart() = 0;
		virtual void onConnectFailed( const char* aVoipServerIP) = 0;
		virtual void onConnectionLost() = 0;
		virtual void onDisconnected() = 0;
		virtual void onKicked() = 0;
		virtual void onAddRoom( const char* alpcRoomName) = 0;
		virtual void onRemoveRoom( const char* alpcRoomName) = 0;
		virtual void onUserTalking( const char* alpcUserName, long long alRmmlID) = 0;
		virtual void onUserStoppedTalking( const char* alpcUserName, long long alRmmlID) = 0;
		virtual void onUserFileSaved( const char* alpcUserName, const char* alpcFileName) = 0;
		virtual void onAudioFilePlayed( const char* alpcUserName, const char* alpcFileName) = 0;
		virtual void onRecordingChanged( unsigned int auRecordID, bool abRecording, unsigned int auErrorCode = 0) = 0;
		virtual void onMixedAudioCreated( unsigned int auRecordID, const char* alpcFileName) = 0;
		virtual void onEvoClientWorkStateChanged( voip::voipEvoClientSessionType aSessionType, voip::voipEvoClientWorkState aWorkState, unsigned int auErrorCode) = 0;
		virtual void onSRModeStatusChanged(voipSRModeStatusChanged aStatus) = 0;
		virtual void onNotified(unsigned int auNotifyCode, unsigned int auNotifiedData) = 0;
		// для асинхронного режима
		virtual void onSetInputDevice( unsigned int auErrorCode, int aiToIncludeSoundDevice) = 0;
		virtual void onSetOutputDevice( unsigned int auErrorCode, int aiToIncludeSoundDevice) = 0;
		virtual void onGetInputDevices( const wchar_t *apMicDeviceNames, const wchar_t *apMixDeviceNames) = 0;
		virtual void onGetOutputDevices( const wchar_t *apDeviceNames) = 0;
		virtual void onRecievedDeviceErrorCode(int anErrorCode) = 0;
		virtual void onReInitializeVoipSystem() = 0;
		virtual void onRecievedFindedMicName(const wchar_t * aDeviceGuid, const wchar_t * aDeviceName, const wchar_t * aDeviceLineName) = 0;
	};

	//struct VOIPMAN_NO_VTABLE IVoipManagerClient
	struct VOIPMAN_NO_VTABLE IVoipManager
	{
		//! Устанавливает параметры подключения к серверу для воспроизведения определенной записи
		virtual bool	SetVoipPlayServerAddress(unsigned int aiRecordID
			, LPCSTR alpcVoipRecordServerAddress, unsigned int aiRecordPort
			, LPCSTR alpcVoipPlayServerAddress, unsigned int aiPlayPort) = 0;

		//! Устанавливает требуемые параметры подключения к определенной комнате
		virtual bool	SetVoipRecordServerAddress(LPCSTR alpcVoipServerAddress, unsigned int aiVoipPort, LPCSTR alpcRecordServerAddress
			, unsigned int aiRecordPort, LPCSTR alpcRoomName) = 0;

		//! Подключиться к группе общения голосовой связи
		virtual voipError	Connect( LPCSTR alpcLocationName, unsigned int auRealityID, LPCSTR alpcUserName, LPCSTR alpcPassword, long long alAvatarID) = 0;

		//! Восстанавливает подключение к текущему серверу голосовой связи
		virtual voipError	RestoreConnection() = 0;

		//! Отключиться от текущего сервера голосовой связи
		virtual voipError	Disconnect() = 0;

		//! Создать говорящего бота с номером
		virtual voipError	CreateServerTalkingBot( unsigned int aClientSideObjectID) = 0;

		//! Удалить говорящего бота с номером
		virtual voipError	DestroyServerTalkingBot( unsigned int aClientSideObjectID) = 0;

		//! Удалить менеджер голосовой связи
		virtual void Release() = 0;

		//! Установка параметров связи
		//! использовать ли DirectSound для воспроизведения звука
		//! значение по умолчанию: 1
		virtual void SetDirectSoundUsed( bool val) = 0;

		//! уровень усиления сигнала с микрофона. Диапазон значений 100-4000. 1000 - усиления нет. 4000 - x4, 100-x0.1
		//! значение по умолчанию: 1000
		virtual void SetMicGainLevel(float val) = 0;

		//! уровень начала передачи сигнала. Диапазон значений 0-20. 
		//! 0 - сигнал передается полностью, 10 - сигнал передается тольно если он сильнее чем 50% громкости, 
		//! 20 - сигнал передаваться не будет
		//! значение по умолчанию: 5
		virtual void SetMicActivationLevel(float val) = 0;

		//! частота дискретизации при записи. Значения: 8000, 16000, 32000
		//! значение по умолчанию: 8000
		virtual void SetRecorderFreq(unsigned int  val) = 0;

		//! качество звука. Диапазон значений 1-10. 1 - минимальное качество. 10 - максимальное качество
		//! значение по умолчанию: 4
		virtual void SetVoiceQuality(float val) = 0;

		//! Включение выключение режима hands-free
		virtual void SetVoiceActivated(bool enabled) = 0;

		//! Принудительное начало передачи данных
		virtual void StartTransmitting() = 0;

		//! Принудительное окончание передачи данных
		virtual void StopTransmitting() = 0;

		//! Добавляет обработчик нотификационных событий
		virtual bool AddCallback( voipIVoipManCallbacks* apCallback) = 0;

		//! Удаляет обработчик нотификационных событий
		virtual bool RemoveCallback( voipIVoipManCallbacks* apCallback) = 0;

		//! Вызывается синхронно перед началом записи
		virtual bool OnBeforeStartRecording() = 0;

		//! Начинает запись общения для текущей группы
		virtual bool StartRecording(unsigned int aRecordID, LPCSTR alpcLocationName, unsigned int auRealityID) = 0;

		//! Заканчивает запись общения для текущей группы
		virtual bool StopRecording(unsigned int aRecordID) = 0;

		//! Начинает получать событие начала и окончания записи звука
		virtual bool StartListenRecording( unsigned int aRecordID) = 0;

		//! Заканчивает получать событие начала и окончания записи звука
		virtual bool StopListenRecording( unsigned int aRecordID) = 0;

		// удалить запись (отсылает запрос на сервер)
		virtual bool DeleteRecord(unsigned int aRecordID) = 0;

		// удалить запись (отсылает запрос на сервер)
		virtual bool DeleteRecords( unsigned int *pRecIDs, unsigned int count) = 0;

		//! Начинает воспроизведение указанной записи для указанной реальности
		virtual bool StartPlayingRecord(unsigned int aRecordID, unsigned int aRealityID, const wchar_t* apwcLocationID) = 0;

		//! Заканчивает воспроизведение определенной записи
		virtual bool StopPlayingRecord() = 0;

		//! Приостанавливает воспроизведение определенной записи
		virtual void PauseServerAudio() = 0;

		//! Продолжает воспроизведение определенной записи
		virtual void ResumeServerAudio() = 0;

		//! Перематывает воспроизведение определенной записи
		virtual void RewindServerAudio() = 0;

		//! Посылает серверу команду на воспроизведение записанного файла
		virtual bool StartUserAudioPlayback( LPCSTR alpUserName, LPCSTR alpFileName, LPCSTR alpcCommunicationAreaName, unsigned int aiStartPosition = 0, unsigned int auSyncVersion = 0) = 0;

		//! Посылает серверу команду на остановку записанного файла
		virtual bool StopUserAudioPlayback( LPCSTR alpUserName) = 0;

		//! Посылает серверу команду на приостановку воспроизведения речи пользователя
		virtual bool PauseUserAudio( LPCSTR alpUserName) = 0;

		//! Посылает серверу команду на продолжение воспроизведения речи пользователя
		virtual bool ResumeUserAudio( LPCSTR alpUserName) = 0;

		//! Получение параметров связи
		virtual bool IsDirectSoundUsed() = 0;
		virtual float GetMicGainLevel() = 0;
		virtual float GetMicActivationLevel() = 0;
		virtual float GetVoiceQuality() = 0;
		virtual unsigned int GetRecorderFreq() = 0;
		virtual float GetCurrentInputLevel() = 0;
		virtual float GetCurrentInputLevelNormalized() = 0;
		virtual bool GetVoiceActivated() = 0;
		virtual bool GetMicrophoneEnabled() = 0;

		//! Возвращает тип источника звука - голос или звук с компа
		virtual voipAudioSource GetAudioSource() = 0;
		//! Возвращает режим настройки громкости. Автоматический или ручной
		virtual bool GetAutoVolume() = 0;
		//! Устанавливает режим настройки громкости. Автоматический или ручной
		virtual void SetAutoVolume(bool abAuto) = 0;

		virtual bool StartMicRecording() = 0;
		virtual bool StopMicRecording( LPWSTR *fileName, unsigned int aSize) = 0;

		virtual bool GetInputDevices( bool abForce) = 0;
		virtual bool GetOutputDevices( bool abForce) = 0;
		virtual bool CheckEnableCurrentDevice() = 0;
		//virtual bool GetInputDevices( wchar_t* alpcDeviceNames, int aiBufferLength) = 0;
		//! Выполняет инициализацию голосовой связи на указанном устройстве и линии
		//! В Vista и Win7 - линии не учитывается
		virtual bool SetInputDeviceMode( const wchar_t* alpcDeviceName, const wchar_t* alpcDeviceLine, int aiToIncludeSoundDevice) = 0;
		//! Выполняет инициализацию голосовой связи на указанном устройстве и линии
		//! В Vista и Win7 - линии не учитывается
		virtual bool SetOutputDeviceMode( const wchar_t* alpcDeviceName, const wchar_t* alpcDeviceLine, int aiToIncludeSoundDevice) = 0;
		//! Выполняет инициализацию голосовой связи на линии "Mixer" текущего звукового устройства
		//! В Vista и Win7 - не работает
		virtual bool SetInputMixerMode( const wchar_t* alpcDeviceName, const wchar_t* alpcDeviceLine, int aiToIncludeSoundDevice) = 0;
		//! Выполняет инициализацию голосовой связи на устройстве Virtual Audio Cable
		//virtual bool SetInputVirtualAudioCableMode( int aiToIncludeSoundDevice) = 0;
		//! Устанавливает уровень громкости от пользователя
		virtual bool SetUserVolume( const wchar_t* alwpcUserLogin, double adUserVolume) = 0;
		//! Устанавливает уровень усиления громкости от пользователя
		virtual bool SetUserGainLevel( const wchar_t* alwpcUserLogin, double adUserGainLevel) = 0;
		// общая громкость входящего звука пользователей
		virtual bool SetIncomingVoiceVolume( double adUserVolume) = 0;
		// подавление эха
		virtual bool SetEchoCancellation(bool anEnabled) = 0;
		virtual bool SetMixerEchoCancellation(bool anEnabled) = 0;
		//! Автоматическое управление громкостью
		virtual bool IsAutoGaneControlEnabled() = 0;
		virtual bool SetAutoGaneControlEnabled(bool abEnabled) = 0;
		//! Автоматическое подавление шумов
		virtual bool IsDenoisingEnabled() = 0;
		virtual bool SetDenoisingEnabled(bool abEnabled) = 0;

		//! вход в режим прослушивания своего голоса
		virtual int EnterSelfRecordingMode() = 0;
		//! начало записи своего голоса
		virtual int StartSelfRec() = 0;
		//! остановить запись своего голоса
		virtual int StopSelfRec() = 0;
		//! воспроизвечти записанный голос
		virtual int PlaySelfRec() = 0;
		//! выйти из режима прослушивания своего голоса
		virtual int ExitSelfRecordingMode() = 0;

		virtual void CloseVoipManServer() = 0;
		virtual void OnCommunicationAreaChanged() = 0;
		virtual bool IsMixerSet() = 0;

		virtual bool StartFindRealMicrophon() = 0;
		virtual bool EndFindRealMicrophon() = 0;
		virtual void CorrectOldSettings() = 0;
	};


	VOIPMAN_API omsresult CreateVoipManagerClient( oms::omsContext* aContext);
	VOIPMAN_API void DestroyVoipManagerClient( oms::omsContext* aContext);
}
#endif