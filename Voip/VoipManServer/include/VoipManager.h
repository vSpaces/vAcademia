#pragma once

#include <map>
#include <vector>
#include "Mutex.h"
#include "EvoSettings.h"
#include "../../Comman/ProxySettings.h"
#include "DeviceList/WTMicContoller.h"

class CMP3Encoder;

class CVoip3DSoundCalcManager;
class CDataBuffer2;

#define MAX_STABLE_AUDIO_LEVEL_VOICE	3000
#define MAX_STABLE_AUDIO_LEVEL_SHARING	15000

struct ProxyInfo;

struct IVoipManWndEventHandlerEVO
{
	virtual void HandleOnConnectRequest() = 0;

	//! Вызывается при невозможности соединения с сервером
	virtual LRESULT OnConnectFailed() = 0;
	//! Вызывается при успешном соединении с сервером
	virtual LRESULT OnConnectSuccess() = 0;
	//! Вызывается при потери связи с сервером
	virtual LRESULT OnConnectionLost() = 0;
	//! Вызывается при успешной логинации
	virtual LRESULT OnAccepted() = 0;
	//! Вызывается при успешной логинации
	virtual LRESULT OnAcceptedFromVoip() = 0;
	//! Вызывается при выходе пользователя из сеанса
	virtual LRESULT OnLoggedOut() = 0;
	//! Вызывается при отказе сервера в обслуживании пользователя
	virtual LRESULT OnKicked() = 0;
	//! Вызывается при обновлении данных с сервера
	virtual LRESULT OnServerUpdate() = 0;
	//! Вызывается 
	virtual LRESULT OnCommandError(WPARAM wParam) = 0;
	//! Вызывается при появлении нового пользователя на сервере
	virtual LRESULT OnAddUser(WPARAM wParam) = 0;
	//! Вызывается при обновлении данных по конкретному пользователю
	virtual LRESULT OnUpdateUser(WPARAM wParam) = 0;
	//! Вызывается при удалении пользователя на сервере
	virtual LRESULT OnRemoveUser(WPARAM wParam) = 0;
	//! Вызывается при появлении на сервере нового канала
	virtual LRESULT OnAddChannel(WPARAM wParam) = 0;
	//! Вызывается при изменении данных канала
	virtual LRESULT OnUpdateChannel(WPARAM wParam) = 0;
	//! Вызывается при удалении канала на сервере
	virtual LRESULT OnRemoveChannel(WPARAM wParam) = 0;
	//! Вызывается при удалении подключении к каналу на сервере
	virtual LRESULT OnChannelJoined(WPARAM wParam) = 0;
	//! Вызывается при удалении отключении от канала на сервере
	virtual LRESULT OnChannelLeft(WPARAM wParam) = 0;
	virtual LRESULT OnAddFile(LPARAM lParam) = 0;
	virtual LRESULT OnRemoveFile(LPARAM lParam) = 0;
	virtual LRESULT OnFileTransferBegin(WPARAM wParam) = 0;
	virtual LRESULT OnFileTransferCompleted(WPARAM wParam) = 0;
	virtual LRESULT OnFileTransferFailed(LPARAM lParam) = 0;
	virtual LRESULT OnUserMessage(WPARAM wParam, LPARAM lParam) = 0;
	virtual LRESULT OnChannelMessage(WPARAM wParam, LPARAM lParam) = 0;
	virtual LRESULT OnUserTalking(WPARAM wParam) = 0;
	virtual LRESULT OnUserStoppedTalking(WPARAM wParam) = 0;
	virtual LRESULT OnInvalideChannelName(WPARAM wParam) = 0;
	virtual LRESULT OnMyUserIDRecieved(WPARAM wParam) = 0;
	virtual LRESULT OnLogMumbleErrorCode(WPARAM wParam, LPARAM lParam) = 0;
	//virtual LRESULT ProcessCommandById(WPARAM wParam, LPARAM lParam) = 0;
};

struct UserVolume
{
	std::wstring name;
	long volume;
};

class CVoipManagerEVO /*: public IVoipManager
	, public cs::imessagehandler
	, public ivoiceconnectionlistender*/
{
public:
	CVoipManagerEVO(oms::omsContext* aContext);
	~CVoipManagerEVO(void);

	//! Обработка сообщений
public:
	/*BEGIN_IN_MESSAGE_MAP()		
		IN_MESSAGE2( RT_RecordStartResult, OnRecordStartResult)
		IN_MESSAGE2( RT_RecordFinished, OnRecordFinished)
		IN_MESSAGE2( RT_AudioPlayed, OnAudioPlayed)
		IN_MESSAGE2( RT_MixedAudioCreated, OnMixedAudioCreated)
		IN_MESSAGE2( RT_PlayFinished, OnPlayFinished)
		IN_MESSAGE2( RT_PlaySessionConnected, OnPlaySessionConnected)
		IN_MESSAGE2( RT_AudioFilesInfo, OnAudioFilesInfo)
		IN_MESSAGE2( RT_EvoClientWorkStateChanged, OnEvoClientWorkStateChanged)
		IN_MESSAGE2( RT_Notified, OnNotified)
	END_IN_MESSAGE_MAP()*/

	// реализация voip::IVoipManager
public:
	void OnRecordConnectionLost(unsigned int auErrorCode);
	void OnRecordConnectionRestored();

	void OnPlayConnectionLost();
	void OnPlayConnectionRestored();

	void OnSelfRecordingConnectionLost();
	void OnSelfRecordingConnectionRestored();

	// реализация voip::IVoipManager
public:

	//! Подключиться к группе общения голосовой связи
	virtual voipError	Connect( LPCSTR alpcGroupName, LPCSTR alpcUserName, LPCSTR alpcPassword, long long alAvatarID);

	//! Восстанавливает подключение к текущему серверу голосовой связи
	voipError	RestoreConnection();

	//! Отключиться от текущего сервера голосовой связи
	//voipError	Disconnect();

	//! Создать говорящего бота с номером
	voipError	CreateServerTalkingBot( unsigned int aClientSideObjectID);

	//! Удалить говорящего бота с номером
	voipError	DestroyServerTalkingBot( unsigned int aClientSideObjectID);

	//! Удалить менеджер голосовой связи
	void Release();

	//! использовать ли DirectSound для воспроизведения звука
	//! значение по умолчанию: 1
	void SetDirectSoundUsed( bool val);

	//! уровень усиления сигнала с микрофона. Диапазон значений 100-4000. 1000 - усиления нет. 4000 - x4, 100-x0.1
	//! значение по умолчанию: 1000
	void SetMicGainLevel(float val);

	//! уровень начала передачи сигнала. Диапазон значений 0-20. 
	//! 0 - сигнал передается полностью, 10 - сигнал передается тольно если он сильнее чем 50% громкости, 
	//! 20 - сигнал передаваться не будет
	//! значение по умолчанию: 5
	void SetMicActivationLevel(float val);

	//! частота дискретизации при записи. Значения: 8000, 16000, 32000
	//! значение по умолчанию: 8000
	void SetRecorderFreq(unsigned int val);

	//! качество звука. Диапазон значений 1-10. 1 - минимальное качество. 10 - максимальное качество
	//! значение по умолчанию: 4
	void SetVoiceQuality(float val);

	//! Включение выключение режима hands-free
	virtual void SetVoiceActivated(bool enabled);

	//! Принудительное начало передачи данных
	virtual void StartTransmitting(){}

	//! Принудительное окончание передачи данных
	virtual void StopTransmitting(){}

	//! Вызывается синхронно перед началом записи
	virtual bool OnBeforeStartRecording();

	//! Устанавливает уровень громкости от пользователя
	virtual bool SetUserVolume( const wchar_t* alpwcUserLogin, double adUserVolume);

	//! Устанавливает уровень усиления громкости от пользователя
	virtual bool SetUserGainLevel( const wchar_t* alwpcUserLogin, double adUserGainLevel);

	//! Получение параметров связи
	virtual bool IsDirectSoundUsed();
	virtual float GetMicGainLevel();
	virtual float GetMicActivationLevel();
	virtual float GetVoiceQuality();
	virtual unsigned int GetRecorderFreq();
	virtual float GetCurrentInputLevel(){ return 0;}
	// возвращает уровень громкости записываемого звука с микрофона
	//virtual float GetCurrentInputLevelNormalized(){ return GetMicrophoneEnabled() ? GetCurrentVoiceInputLevelNormalized() : GetCurrentMicInputLevelNormalized();}
	// возвращает громкость вашего голоса с учетом параметров усиления и порога
	virtual float GetCurrentVoiceInputLevelNormalized(){ return 0;}	
	// возвращает уровень громкости записываемого звука с микрофона (waveIn)
	//virtual float GetCurrentMicInputLevelNormalized();
	//GetMicrophoneEnabled
	virtual int GetLogMode(){ return m_iLogMode;};
	virtual bool GetVoiceActivated(){ return voiceActivated;}
	virtual bool GetMicrophoneEnabled(){ return false;}

	//! Возвращает тип источника звука - голос или звук с компа
	voipAudioSource GetAudioSource();
	//! Устанавливает тип источника звука
	bool SetAudioSource(voipAudioSource aeAudioSource);
	//! Возвращает режим настройки громкости. Автоматический или ручной
	bool GetAutoVolume();
	//! Устанавливает режим настройки громкости. Автоматический или ручной
	void SetAutoVolume(bool abAuto);
	
	//virtual bool SetInputVirtualAudioCableMode() = 0;

	//! Автоматическое управление громкостью
	virtual bool IsAutoGaneControlEnabled();
	virtual bool SetAutoGaneControlEnabled(bool abEnabled);
	//! Автоматическое подавление шумов
	virtual bool IsDenoisingEnabled();
	virtual bool SetDenoisingEnabled(bool abEnabled);
	// Это временная заглушка. Нужно сделать так:
	/*
	VoipMan должен периодически проверять список подключенных устройств. В случае если он изменился
	сообщать в скрипт об этом. При этом если текущее устройство пропало - нало об этом сказать.
	Если список стал пустым - а был с устройствами, надо сказать подключите микрофон обратно.
	*/
	bool RebuildIfEmptyDevicesList(){return false;};

	//! вход в режим прослушивания своего голоса
	/*
	Возвращаем следующие ошибки:
	SR_NOT_AVAILABLE_DURING_RECORDING
	NOTINITIALIZED
	NOERROR
	*/
	virtual int EnterSelfRecordingMode();
	//! начало записи своего голоса
	/*
	NOERROR
	*/
	/*virtual int StartSelfRec();
	//! остановить запись своего голоса
	virtual int StopSelfRec();
	//! воспроизвечти записанный голос
	virtual int PlaySelfRec();
	//! выйти из режима прослушивания своего голоса
	virtual int ExitSelfRecordingMode();*/

	//! В потомках возвращает флаг успешной инициализации
	virtual bool IsVoipSystemInitialized(){ return false;}


protected:	

	/*virtual void SendConnectRequest( LPCSTR alpcVoipServerAddress, LPCSTR alpcRecordServerAddress
		, unsigned int aiRecordPort, LPCSTR alpcRoomName)
	{
		ATLASSERT( FALSE);
	}*/
	virtual void HandleOnConnectRequest(){}

protected:
	//! Прервать режим прослушивания своего голоса
	//int AbortSelfRecordingMode();
	//! Вызывается при вхождении в канал
	void OnChannelJoined(const char* alpcChannelName);
	//! Подключиться к каналу теста своего звука
	voipError ConnectToSelfVoiceChannel();

protected:
	//! Функции реализуются конкретными менеджерами
	virtual voipError	ConnectImpl(){ return ERROR_NOERROR;}
	virtual voipError	DisconnectImpl(){ return ERROR_NOERROR;}
	//virtual voipError	ChangeLocationImpl( LPCSTR alpcLocation){ return ERROR_NOERROR;}

	//! 
	virtual void HandleVoiceData( int nUserID, int nSampleRate, const short* pRawAudio, int nSamples);
	virtual bool SetUserPosition( int nUserID, float x, float y, float z);
	//void SetEvoVoipClientWorkState(voip::voipEvoClientSessionType aSessionType, voip::voipEvoClientWorkState aWorkState, unsigned int auError);

	//! Вызывается после установки свойств
	virtual	void OnDirectSoundUsedChanged() = 0;
	virtual	void OnGainLevelChanged() = 0;
	virtual	void OnActivationLevelChanged() = 0;
	virtual	void OnVoiceActivatedChanged() = 0;
	virtual	void OnVoiceQualityChanged() = 0;
	//! Обработка изменения автоматической регулировки громкости
	virtual void OnAutoVolumeChanged() = 0;

	int GetUserIDByName( LPCWSTR apwsName);
	LPCTSTR GetUserNameByID( int aiAvatarID);
	long GetUserVolume( int aiAvatarID);
	bool SaveUserVolume( int aiAvatarID, long adUserVolume);
#if VOIPMAN_BUILD_L
	CVoip3DSoundCalcManager *GetVoip3DSoundManagerID( int aiAvatarID);
#endif
	//!
	oms::omsContext* context;

	//! Параметры соединения
	bool  mUseDS;
	float micGainLevel;
	float micActivationLevel;
	float voiceQuality;
	int   m_iLogMode;
	bool  voiceActivated;
	unsigned int recFreq;
	CONNECTION_STATUS		connectionStatus;
	//MP_WSTRING	currentInDeviceGuid;
	//MP_WSTRING	currentOutDeviceGuid;
	bool				autoVolume;
	voipAudioSource		audioSource;

	typedef MP_MAP<int, UserVolume> MapVoipUsers;

#if VOIPMAN_BUILD_L
	//typedef	std::map<int/* voipID*/, CVoip3DSoundCalcManager* /* mp3Encoder*/> MapVoip3DSoundCalcManagerID;	
	typedef MP_MAP<int,CVoip3DSoundCalcManager*> MapVoip3DSoundCalcManagerID;
#endif

	void SetSRModeState( voip::voipSRModeStatusChanged	aState);
	void Destroy();

	MapVoipUsers	mapVoipUsers;
	CMutex			mutexAvatars;
#if VOIPMAN_BUILD_L
	MapVoip3DSoundCalcManagerID mapVoip3DSoundCalcManagerID;
#endif
	//CMutex			mutexLog;
	BOOL			bTerminating;

	//! Callbacks
	/*typedef	std::vector<voipIVoipManCallbacks*>	VecCallbacks;
	VecCallbacks	vecCallbacks;*/

	int					myUserID;
	CComString			serverIP;
	//CComString			roomName;
	CComString			userName;
	CComString			userPassword;
	std::vector<ProxyInfo> proxySettingsList;	

	//! Параметры предполагаемого соединения
	/*CComString			requestedRoomName;
	CComString			requestedUserName;
	CComString			requestedUserPassword;
	long long			requestedAvatarID;*/

	//! Параметры комнаты, в которой должно идти воспроизведение
	//bool				recordIsOnGoing;
	//CDataBuffer2*		m_pSndData2lex;

	//! Режим записи своего голоса
	//voip::voipSRModeStatusChanged		selfRecordingIsOnGoing;
	//bool								selfRecordingChannelConnected;

	//! Параметры запрашиваемого соединения их вне при работе в режиме SR
	CComString			inSRModeRequestedRoomName;
	CComString			inSRModeRequestedUserName;
	CComString			inSRModeRequestedUserPassword;
	long long			inSRModeRequestedAvatarID;


#if VOIPMAN_BUILD_L
	IVoip3DSoundCalculator	*m_pUser3DPositioning;
#endif
public:
};