#pragma once


#include "VoipWindow.h"
#include "../VoipManager.h"
//#include "AudioCaptureManager.h"
#include "../VoipManagerPipeEVO.h"
#include "../../../Common/ViskoeThread.h"
#include "SimpleThreadStarter.h"
#include "proxyInfo.h"


#ifndef USE_CONAITO_SDK_LIB
	using namespace api;
	
#endif USE_CONAITO_SDK_LIB

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

class CVoipManagerConaitoServer : public IVoipManWndEventHandlerEVO
	, public CVoipManagerEVO
	, public ViskoeThread::CThreadImpl<CVoipManagerConaitoServer>
{
	friend class CVoipWindow;
	friend class CVoipManagerPipeEVO;
	//friend void UserTalking(int nUserID, int nSampleRate, const short* pRawAudio, int nSamples, LPVOID pUserData);

public:
	CVoipManagerConaitoServer(oms::omsContext* aContext);
	~CVoipManagerConaitoServer(void);

	LPVOID GetClientInstance();
	CComString GetChannelPath();

public:
	DWORD	Run();

	// реализация voip::IVoipManager
public:
	voipError	ConnectImpl( bool aCheckConnecting);	

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
	//! Устанавливает режим получения звука с компа с помощью драйвера виртуальной аудио-карты VAC4.10
	//bool SetInputVirtualAudioCableMode();
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

protected:
	/*virtual void SendConnectRequest( LPCSTR alpcVoipServerAddress, LPCSTR alpcRecordServerAddress
		, unsigned int aiRecordPort, LPCSTR alpcRoomName);*/
	virtual void HandleOnConnectRequest();
	//! Устанавливает новое звуковое устройство
	bool InitializeVoipOnAudioDevice( const wchar_t* alpcDeviceName);

	// убираем потоки
	//public:
	//void HandleOnConnectRequestImpl();
	//private:
	//static int HandleOnConnectRequestThread( CVoipManagerConaito *aThis);

	// через пайпы	
public:
	//! Инициализации голосовой связи на устройстве
	voipError InitializeVoipSystem( const wchar_t* alpcDeviceGuid, const wchar_t* alpcOutDeviceGuid);
	void SetVoiceGainLevel( float aVal);
	void SetVoiceActivationLevel( float aVal);
	void SetServerParams( std::string &aServerIP, int aTcpPort, int anUdpPort, std::vector<ProxyInfo> aProxyInfoList);
	void JoinChannel( std::string &aRoomName);

	//! Возващает флаг успешной инициализации
	bool IsVoipSystemInitialized();

	//! Получение идентификаторов звуковых устройств
	int GetCurrentInputDeviceID( EVO_WRAPPER_SoundSystemType soundSystem);
	int GetCurrentOutputDeviceID( EVO_WRAPPER_SoundSystemType soundSystem);
	virtual bool GetVoiceActivated();
	const wchar_t *GetCurrentEvoVoipDeviceGuid();
	const wchar_t *GetCurrentEvoVoipOutDeviceGuid();
protected:
	//!
	//unsigned int GetAudioSubsystemType();
	//! Создание окна и инициализации библиотеки EVOVOIP
	voipError InitializeEvoSystem();	
	//! Деинициализации голосовой связи на устройстве
	void DestroyVoipSystem(bool aNeedDisconnect=true);
	//! Удаление окна и деинициализации библиотеки EVOVOIP
	void DestroyEvoSystem();

	//! Отключение от текущего канала
	void DisconnectCurrentChannel();

	//! Получение идентификаторов звуковых устройств
	int GetDefaultInputDeviceID( EVO_WRAPPER_SoundSystemType soundSystem);
	int GetDefaultOutputDeviceID( EVO_WRAPPER_SoundSystemType soundSystem);

	int GetInputDeviceID( EVO_WRAPPER_SoundSystemType soundSystem, bool abDefault);
	int GetOutputDeviceID( EVO_WRAPPER_SoundSystemType soundSystem, bool abDefault);

	/** Возвращает EVO_VOIP номер устройства по его имени. В случае, если такого устройства нет в системе - возвращает -1
	*/
	int GetEvoVoipDeviceIDByGuid( const wchar_t* alpcDeviceGuid);
	int GetEvoVoipOutDeviceIDByGuid( const wchar_t* alpcDeviceGuid);

	/** Возвращает имя устройства по его EVO_VOIP номеру. В случае, если такого устройства нет в системе - возвращает ""
	*/
	std::wstring GetDeviceGuidByEvoVoidID( int aiDeviceID);

	//! Получение режима активации передачи звука	
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
	virtual	int CalculateAverageLevel( int nSampleRate, const short* pRawAudio, int nSamples);
	bool GrabDevicesInfoImpl(EVO_WRAPPER_SoundSystemType soundSystem, bool abForce);	
	bool GrabInputDevicesInfo(EVO_WRAPPER_SoundSystemType soundSystem, bool abForce);
	bool GrabOutputDevicesInfo(EVO_WRAPPER_SoundSystemType soundSystem, bool abForce);
	//! Обработка изменения автоматической регулировки громкости
	virtual void OnAutoVolumeChanged();
	//! Устанавливает уровень громкости от пользователя
	virtual bool SetUserVolume( const wchar_t* alwpcUserLogin, double adUserVolume);

	void SetMasterVolume(float aVolume);
	void SetEchoCancellation(bool anEnabled);
	void SetMixerEchoCancellation(bool anEnabled);
	void AppStartFindMicDevice();
	void AppEndFindMicDevice();

	//! Устанавливает уровень усиления громкости от пользователя
	//virtual bool SetUserGainLevel( const wchar_t* alwpcUserLogin, double adUserGainLevel);	
	//! 
public:
	virtual void HandleVoiceData( int nUserID, int nSampleRate, const short* pRawAudio, int nSamples);
	bool GrabDevicesInfo(bool abForce);
	bool GrabInputDevicesInfoForce( bool abForce);
	bool GrabOutputDevicesInfoForce( bool abForce);
protected:

	//! Вызывается при невозможности соединения с сервером
	LRESULT OnConnectFailed();
	//! Вызывается при успешном соединении с сервером
	LRESULT OnConnectSuccess();
	//! Вызывается при потери связи с сервером
	LRESULT OnConnectionLost();
	//! Вызывается при успешной логинации
	LRESULT OnAccepted();
	//! Вызывается при успешной логинации из eво или мамбл
	LRESULT OnAcceptedFromVoip();
	//! Вызывается при выходе пользователя из сеанса
	LRESULT OnLoggedOut();
	//! Вызывается при отказе сервера в обслуживании пользователя
	LRESULT OnKicked();
	//! Вызывается при обновлении данных с сервера
	LRESULT OnServerUpdate();
	//! Вызывается 
	LRESULT OnCommandError(WPARAM wParam);
	//! Вызывается при получении ид своего пользователя
	LRESULT OnMyUserIDRecieved(WPARAM wParam);
	//! Вызывается при появлении нового пользователя на сервере
	LRESULT OnAddUser(WPARAM wParam);
	//! Вызывается при обновлении данных по конкретному пользователю
	LRESULT OnUpdateUser(WPARAM wParam);
	//! Вызывается при удалении пользователя на сервере
	LRESULT OnRemoveUser(WPARAM wParam);
	//! Вызывается при появлении на сервере нового канала
	LRESULT OnAddChannel(WPARAM wParam);
	//! Вызывается при изменении данных канала
	LRESULT OnUpdateChannel(WPARAM wParam);
	//! Вызывается при удалении канала на сервере
	LRESULT OnRemoveChannel(WPARAM wParam);
	//! Вызывается при удалении подключении к каналу на сервере
	LRESULT OnChannelJoined(WPARAM wParam);
	//! Вызывается при удалении отключении от канала на сервере
	LRESULT OnChannelLeft(WPARAM wParam);
	LRESULT OnAddFile(LPARAM lParam);
	LRESULT OnRemoveFile(LPARAM lParam);
	LRESULT OnFileTransferBegin(WPARAM wParam);
	LRESULT OnFileTransferCompleted(WPARAM wParam);
	LRESULT OnFileTransferFailed(LPARAM lParam);
	LRESULT OnUserMessage(WPARAM wParam, LPARAM lParam);
	LRESULT OnChannelMessage(WPARAM wParam, LPARAM lParam);
	LRESULT OnUserTalking(WPARAM wParam);
	LRESULT OnUserStoppedTalking(WPARAM wParam);
	LRESULT OnInvalideChannelName(WPARAM wParam);
	LRESULT OnLogMumbleErrorCode(WPARAM wParam, LPARAM lParam);
	//BOOL GetEvoUserImpl(INT32 nUserID, User* lpUser);

	// сообщения с клиента и их обработка
public:
	void OnStartTransmitting();
	void OnStopTransmitting();
	bool OnHandleTransmissionStateChanged( bool aMicrophoneEnabled, bool aVoiceActivated);
	int GetMyUserID();

	//! Устанавливает уровень усиления громкости от пользователя
	virtual bool SetUserGainLevel(  const wchar_t* alwpcUserLogin, double adUserGainLevel);
	voipError	DisconnectImpl();

	BOOL GetEvoUserImpl(INT32 nUserID, User* lpUser);

	//! Вызывается при успешном соединении с сервером
	void DoLogin( const wchar_t *aUserName, const wchar_t *aUserPassword, const wchar_t *aRoomName);

private:
	MP_WSTRING				currentEvoVoipDeviceGuid;
	//std::wstring				currentEvoVoipLineName;	
	MP_WSTRING				currentEvoVoipOutDeviceGuid;
	
	CVoipWindow					voipWindow;
	LPVOID						clientInstance;
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

	MP_VECTOR<SoundDevice>		inputDevicesInfo;
	MP_VECTOR<SoundDevice>		outputDevicesInfo;

	//! Текущая громкость
	double				m_currentInputLevel;
	// Максимально допустимая громкость для режима (установлена вручную на основе экспериментов)
	double				m_currentModeTopInputLevel;
	// Максимальная громкость текущего режима (для нормализации). При превышении m_currentModeTopInputLevel - начинает стремиться к ней
	double				m_currentModeCurrentTopInputLevel;
	bool				m_skipCurrentLevelCalculation;

public:
	void GetInputDevicesInfo( vecDeviceInfo **aInputDevicesInfo);
	void GetOutputDevicesInfo( vecDeviceInfo **aOutputDevicesInfo);	
	void SetManagerPipe( CVoipManagerPipeEVO *apVoipManagerPipe);
	void Initialize( bool abTwoApplication);
	int GetConnectionStatus();
	void WriteLogLine(const char* alpString);
	void WriteLogLine(const wchar_t* alpwString);
	void VoipLog();
	void ChannelThread();

protected:
	int tcpPort; 
	int udpPort;
	CComString roomName;
	CVoipManagerPipeEVO *m_voipManagerPipe;
	CComString channelPath;
	bool bNeedLogin;
	bool bLaunchedSoundSystem;
	bool bTwoApplication;
	CMutex mutex;	
	
	void WriteLogLineImpl(const wchar_t* alpwString);

private: 
	void ResetDirectConnectFlags();
	bool ConnectToNextInProxyList();
	void StartVoipLog();
	void StartChannelThread();

	EVO_WRAPPER_SoundSystemType m_mainSoundSystem;

	bool m_directConnectFailed;
	bool m_directConnectSuccess;
	bool m_directConnectNow;
	unsigned int m_currentProxySettingsListIndex;

	CMutex m_proxySettingsMutex;

	HANDLE m_hVoipLogThread;
	DWORD m_voipLogThreadId;
	volatile bool m_bVoipLog;
	HANDLE m_eventVoipLog;

	HANDLE m_hChannelThread;
	DWORD m_channelThreadId;
	volatile bool m_bChannel;
	HANDLE m_eventChannel;
	int tempID;
	//int wParamIDChannel;
	ProxyInfo httpsProxyInfo;
};

