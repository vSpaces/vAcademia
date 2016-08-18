#pragma once

//#ifdef USE_CONAITO_SDK_LIB

#include "VoipWindowClient.h"
#include "../VoipManager.h"
#include "DefaultAudioDeviceManager.h"
#include "../VoipManagerPipe.h"

#include "iclientsession.h"
#include "../Comman/ProxySettings.h"

/* Порядок подключения:
1. VOIP_InitVoIP
2. VOIP_LaunchSoundSystem
3. VOIP_Connect -> OnConnectionSuccess / OnConnectFailed
4. VOIP_DoLogin -> OnAccepted -> OnAddChannel
4. VOIP_DoJoinChannel -> OnAddChannel -> OnAddUser
*/

/*
Параметры текущего подключения:
1. ServerName
2. UserName
3. Password
4. RoomName
5. 
*/

class CAudioCapture;

class CVoipManagerConaito : public IVoipManWndEventHandler
							, public CVoipManager
							, public ViskoeThread::CThreadImpl<CVoipManagerConaito>
							, public cm::ichangeconnection
{
	friend class CVoipWindowClient;
	friend class CVoipManagerPipe;

public:
	CVoipManagerConaito(oms::omsContext* aContext);
	~CVoipManagerConaito(void);

	// реализация voip::IVoipManager
public:
	voipError	ConnectImpl( bool aCheckConnecting);	
	voipError	ChangeLocationImpl( LPCSTR alpcLocation);
	voipError	DisconnectImpl();

	//! Удалить менеджер голосовой связи
	void Release();
	//! Принудительное начало передачи данных
	virtual void StartTransmitting();
	//! Принудительное окончание передачи данных
	virtual void StopTransmitting();
	//! Возвращает громкость вашего голоса
	virtual float GetCurrentInputLevel();	
	//! Возвращает громкость вашего голоса с учетом параметров усиления и порога
	virtual float GetCurrentVoiceInputLevelNormalized();
	//! Возвращает список устройств, с которых может передаваться звук
	//bool GetInputDevices( wchar_t* alpcDeviceGuids, int aiBufferLength);
	bool GetInputDevices( bool abForce);
	bool GetOutputDevices( bool abForce);
	bool CheckEnableCurrentDevice();
	//! Устанавливает режим получения звука с нового звукового устройства - конкретное
	bool SetInputDeviceMode( const wchar_t* alpcDeviceGuid, const wchar_t* alpcDeviceLine, int aiToIncludeSoundDevice);
	bool SetInputDeviceMode( const wchar_t* alpcDeviceGuid, const wchar_t* alpcDeviceLine, int aiVasDevice, int aiToIncludeSoundDevice);
	void OnSetInputDeviceMode( std::wstring &aDeviceGuid, std::wstring &aDeviceLine, int aiToIncludeSoundDevice, int aSource);
	//! Устанавливает режим получения звука с компа с помощью встроенной линии StereoMixer
	bool SetInputMixerMode( const wchar_t* alpcDeviceGuid, const wchar_t* alpcDeviceLine, int aiToIncludeSoundDevice);
	void OnSetInputMixerMode( std::wstring &aDeviceGuid, std::wstring &aMixerName, int aiToIncludeSoundDevice, int aSource);
	//! Устанавливает режим получения звука с компа с помощью драйвера виртуальной аудио-карты VAC4.10
	//void OnSetInputVirtualAudioCableMode( int aiToIncludeSoundDevice);
	//bool SetInputVirtualAudioCableMode( int aiToIncludeSoundDevice);

	bool SetOutputDeviceMode( const wchar_t* alpcDeviceGuid, const wchar_t* alpcDeviceLine, int aiToIncludeSoundDevice);
	void OnSetOutputDeviceMode( std::wstring &aOutDeviceName, std::wstring &aDeviceLine, int aiToIncludeSoundDevice);
	
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
	bool RebuildIfEmptyDevicesList();

	bool SetUserPosition( int nUserID, float x, float y, float z);
	void CloseVoipManServer();
	void OnCommunicationAreaChanged();
	bool IsMixerSet();
	bool StartFindRealMicrophon();
	bool EndFindRealMicrophon();

// ichangeconnection
public:
	bool IsEqualProxySettings(std::vector<IProxySettings*>* aProxyInfoList1, std::vector<IProxySettings*>* aProxyInfoList2);
	virtual void OnChangeConnectionType(void* aProxyInfo);
	virtual void SubscribeToNotify(ichangeconnection* aSubscriber){};
	virtual void UnSubscribeFromNotify(ichangeconnection* aSubscriber){};

protected:
	virtual void SendConnectRequest( LPCSTR alpcVoipServerAddress, unsigned int aiVoipPort, LPCSTR alpcRecordServerAddress
						, unsigned int aiRecordPort, LPCSTR alpcRoomName);
	virtual void HandleOnConnectRequest();
	//! Устанавливает новое звуковое устройство
	bool InitializeVoipOnAudioDevice( const wchar_t* alpcDeviceGuid, int aiVasDevice, std::wstring &aDeviceLine, std::wstring &aMixDeviceLine, int aiToIncludeSoundDevice);

protected:
	//! Возващает флаг успешной инициализации
	bool IsVoipSystemInitialized();
	//!
	unsigned int GetAudioSubsystemType();
	//! Создание окна и инициализации библиотеки EVOVOIP
	//voipError InitializeEvoSystem();
	//! Инициализации голосовой связи на устройстве	
	//! Деинициализации голосовой связи на устройстве
	void DestroyVoipSystem();
	//! Удаление окна и деинициализации библиотеки EVOVOIP
	void DestroyEvoSystem();

	/*//! Получение идентификаторов звуковых устройств
	int GetDefaultInputDeviceID();
	int GetDefaultOutputDeviceID();*/

	//int GetInputDeviceID( bool abDefault);
	//int GetOutputDeviceID( bool abDefault);

	//! Получение идентификаторов звуковых устройств
	/*int GetCurrentInputDeviceID();
	int GetCurrentOutputDeviceID();*/

	/** Возвращает EVO_VOIP номер устройства по его имени. В случае, если такого устройства нет в системе - возвращает -1
	*/
	int GetEvoVoipDeviceIDByGuid( const wchar_t* alpcDeviceGuid);

	/** Возвращает guid устройства по его EVO_VOIP номеру. В случае, если такого устройства нет в системе - возвращает ""
	*/
	std::wstring GetDeviceGuidByEvoVoidID( int aiDeviceID);
	std::wstring GetDeviceNameByGuid( const wchar_t* alpcDeviceGuid);
	std::wstring GetDeviceGuidByName( const wchar_t* alpcDeviceName, std::vector<SoundDevice> &aInputDevicesInfo);

	//! Получение режима активации передачи звука
	virtual bool GetVoiceActivated();
	virtual bool GetMicrophoneEnabled();

	// обработка событий
protected:
	void HandleTransmissionStateChanged();
	//! Вызывается после установки свойств
	virtual	void OnDirectSoundUsedChanged();
	virtual	void OnGainLevelChanged();
	virtual	void OnActivationLevelChanged();
	virtual	void OnVoiceActivatedChanged();
	virtual	void OnVoiceQualityChanged();
	//virtual	int CalculateAverageLevel( int nSampleRate, const short* pRawAudio, int nSamples);
	//! Обработка изменения автоматической регулировки громкости
	virtual void OnAutoVolumeChanged();
	//! Устанавливает уровень громкости от пользователя
	virtual bool SetUserVolume( const wchar_t* alwpcUserLogin, double adUserVolume);

	// общая громкость входящего звука пользователей
	virtual bool SetIncomingVoiceVolume( double adUserVolume);
	virtual bool SetEchoCancellation(bool anEnabled);
	virtual bool SetMixerEchoCancellation(bool anEnabled);

	//! Устанавливает уровень усиления громкости от пользователя
	virtual bool SetUserGainLevel( const wchar_t* alwpcUserLogin, double adUserGainLevel);
	//! 
	virtual void HandleVoiceData( int nUserID, int nSampleRate, const short* pRawAudio, int nSamples);

	//! Вызывается при невозможности соединения с сервером
	LRESULT OnConnectFailed();
	//! Вызывается при успешном соединении с сервером
	LRESULT OnConnectSuccess();
	//! Вызывается при потери связи с сервером
	LRESULT OnConnectionLost();
	//! Вызывается при успешной логинации
	LRESULT OnAccepted(){ return 0;};
	//! Вызывается при выходе пользователя из сеанса
	LRESULT OnLoggedOut(){return 0;};;
	//! Вызывается при отказе сервера в обслуживании пользователя
	LRESULT OnKicked();
	//! Вызывается при обновлении данных с сервера
	LRESULT OnServerUpdate(){return 0;};
	//! Вызывается 
	LRESULT OnCommandError(WPARAM wParam){return 0;};
	//! Вызывается при появлении нового пользователя на сервере
	LRESULT OnAddUser(WPARAM wParam, LPARAM lParam);
	//! Вызывается при обновлении данных по конкретному пользователю
	LRESULT OnUpdateUser(WPARAM wParam){return 0;};
	//! Вызывается при удалении пользователя на сервере
	LRESULT OnRemoveUser(WPARAM wParam);
	//! Вызывается при появлении на сервере нового канала
	LRESULT OnAddChannel(WPARAM wParam){return 0;};
	//! Вызывается при изменении данных канала
	LRESULT OnUpdateChannel(WPARAM wParam){return 0;};
	//! Вызывается при удалении канала на сервере
	LRESULT OnRemoveChannel(WPARAM wParam){return 0;};
	//! Вызывается при удалении подключении к каналу на сервере
	LRESULT OnChannelJoined(WPARAM wParam, LPARAM lParam);
	//! Вызывается при удалении отключении от канала на сервере
	LRESULT OnChannelLeft(WPARAM wParam);
	LRESULT OnAddFile(LPARAM lParam){return 0;};
	LRESULT OnRemoveFile(LPARAM lParam){return 0;};
	LRESULT OnFileTransferBegin(WPARAM wParam){return 0;};
	LRESULT OnFileTransferCompleted(WPARAM wParam){return 0;};
	LRESULT OnFileTransferFailed(LPARAM lParam){return 0;};
	LRESULT OnUserMessage(WPARAM wParam, LPARAM lParam){return 0;};
	LRESULT OnChannelMessage(WPARAM wParam, LPARAM lParam){return 0;};
	LRESULT OnUserTalking(WPARAM wParam, LPARAM lParam);
	LRESULT OnUserStoppedTalking(WPARAM wParam, LPARAM lParam);

	//BOOL GetEvoUserImpl( INT32 nUserID, User* lpUser);
	//BOOL GetEvoUserImpl( int aMsgID, INT32 nUserID, User* lpUser);

	//void OnUserTalking( int aUserID, const char *apNickName);
	//void OnUserStoppedTalking( int aUserID, const char *apNickName);
	//BOOL GetEvoUserImpl( int aMsgID, INT32 nUserID, int aUserID, const char *apNickName);	
	//void OnAddUser( int aUserID, const char *apNickName);

	// cообщение пайпов 
public:
	voipError InitializeVoipSystem( int aiVasDevice, int aiToIncludeSoundDevice, const wchar_t *alpcDeviceGuid, const wchar_t *alpcOutDeviceName);
	voipError ReInitializeVoipSystem();
	bool OnSendInitVoipSystem( int aErrorCode, bool abInitVoipSystem, int aiVasDevice, int aiToIncludeSoundDevice, std::wstring &aDeviceGuid, std::wstring &aOutDeviceName);
	bool OnGetInputDevices();
	bool OnGetOutputDevices();
	LRESULT OnInitVoipSystem( WPARAM wParam, LPARAM lParam);
	void OnGetMyUserID( int auserID);
	void OnDeviceErrorCode(int anErrorCode);
	void OnFindedMicName(const wchar_t * aDeviceGuid, const wchar_t * aDeviceName, const wchar_t * aDeviceLineName);

	bool IsGetInputDevices();
	voipError OnConnectImpl( unsigned int aErrorCode);

	/*void OnConnectFailed();
	void OnConnectSuccess();
	void OnConnectionLost();*/
	void OnConnectionStatus( int aConnectionStatus);
	//void OnKicked();
	void OnChannelJoined(wchar_t *szChannelPath);
	//void OnChannelLeft();
	//void OnRemoveUser(WPARAM wParam);
	void SetCurrentInputLevel( float aCurrentInputLevel);
private:
	MP_WSTRING				currentEvoVoipDeviceGuid;	
	MP_WSTRING				currentDeviceLine;
	MP_WSTRING				currentMixerGuid;
	MP_WSTRING				currentEvoVoipLineName;
	MP_WSTRING				currentEvoVoipMixLineName;

	MP_WSTRING				currentEvoVoipOutDeviceGuid;
	MP_WSTRING				currentEvoVoipOutDeviceLine;	

	// для правильного отображение в скрипте выбранного устройства c учетом устройства по умолчанию
	MP_WSTRING				currentMicrophoneDeviceGuid;
	MP_WSTRING				currentMixDeviceGuid;
	//

	CVoipWindowClient					voipWindowClient;
	//LPVOID						clientInstance;
	CONNECTION_STATUS			channelConnectionStatus;
	bool						microphoneEnabled;
	ConnectRequestParams		connectRequestParams;
	CEvoSettings				settings;
	bool						isLoggedIn;

	CEvent						initializeCompleteEvent;
	voipError					initializeErrorCode;

	CMutex						mutexConnect;
	int							audioCapDeviceID;
	bool						agcEnabled;
	bool						denoisingEnabled;

	typedef std::vector<SoundDevice> vecDeviceInfo;
	typedef vecDeviceInfo::iterator vecDeviceInfoIt;
	MP_VECTOR<SoundDevice>		inputDevicesInfo;
	MP_VECTOR<SoundDevice>		outputDevicesInfo;

	//! Текущая громкость
	double				m_currentInputLevel;
	// Максимально допустимая громкость для режима (установлена вручную на основе экспериментов)
	double				m_currentModeTopInputLevel;
	// Максимальная громкость текущего режима (для нормализации). При превышении m_currentModeTopInputLevel - начинает стремиться к ней
	double				m_currentModeCurrentTopInputLevel;

	//! Текущая громкость
	double				m_currentOutputLevel;
	// Максимально допустимая громкость для режима (установлена вручную на основе экспериментов)
	double				m_currentModeTopOutputLevel;
	// Максимальная громкость текущего режима (для нормализации). При превышении m_currentModeTopInputLevel - начинает стремиться к ней
	double				m_currentModeCurrentTopOutputLevel;

	bool				m_skipCurrentLevelCalculation;
	CVoipManagerPipe	*m_voipManagerPipe;
	int m_userID;
	bool bVoipSystemInitialized;
	bool bVoipSettingCorrect;
	CMutex mutexCurrentInputLevel;
	MP_WSTRING oldCurrentDeviceName;
	MP_WSTRING oldDeviceLine;
	MP_WSTRING oldMixerName;

	MP_WSTRING oldCurrentOutDeviceName;
	MP_WSTRING oldCurrentEvoVoipOutDeviceLine;
	bool	   outputDeviceWasChanged;


public:
	void SetInputDevices( vecDeviceInfo *apInputDevicesInfo);
	void SetOutputDevices( vecDeviceInfo *apOutputDevicesInfo);
	void CorrectOldSettings();
	void SetVoipSystemInitialized( bool abVoipSystemInitialized);
	void SendMessageToWnd( unsigned int auMSG, unsigned int data1 = 0, const wchar_t *data2 = NULL);
	oms::omsContext *GetOMSContext();
	void UpdateVoipConnection();	

	void CheckAudioDevices();

private:
	bool LogSystemDrivers();
	std::string ReadRegKey(std::string path, std::string key);
	std::wstring TryGetCurrentInDeviceGuid( bool aMicrophone/*, SoundDevice &sndDevice*/);
	bool DeviceIsEqual( std::wstring aDevice1, std::wstring aDevice2);
	bool DeviceIsIntoArrayName( std::wstring aDevice1, CWTMicContoller::CDevicesNamesVector &aDeviceGuidArray);

	void StartCheckAudioDevices();

	bool isSubscribedToNotify;
	HANDLE m_hCheckAudioDevicesThread;
	DWORD m_checkAudioDevicesThreadId;
	volatile bool m_bCheckAudioDevices;
	HANDLE m_eventCheckAudioDevices;
	CDefaultAudioDeviceManager *m_pDefaultAudioDeviceManager;
	CMutex mutexAudioDevice;
};

//#endif USE_CONAITO_SDK_LIB