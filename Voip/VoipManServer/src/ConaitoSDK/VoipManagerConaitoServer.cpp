#include "StdAfx.h"
#include <algorithm>

#include "soundLog\MP3Encoder.h"
#include "include\ConaitoSDK\VoipManagerConaitoServer.h"
#include "include/PipeProtocol/ConnectToEVoipIn.h"
#include <audioclient.h>
#include <dsound.h>

void UserTalking(int nUserID, int nSampleRate, const short* pRawAudio, int nSamples, LPVOID pUserData)
{
	CVoipManagerConaitoServer *pVoIpMan = (CVoipManagerConaitoServer *) pUserData;
	pVoIpMan->HandleVoiceData( nUserID, nSampleRate, pRawAudio, nSamples);	
}

CVoipManagerConaitoServer::CVoipManagerConaitoServer(oms::omsContext* aContext) : 
CVoipManagerEVO( aContext)
, settings( aContext)
, voipWindow( this)
, microphoneEnabled( false)
, agcEnabled( false)
, denoisingEnabled( false)
, MP_WSTRING_INIT(currentEvoVoipDeviceGuid)
, MP_WSTRING_INIT(currentEvoVoipOutDeviceGuid)
, MP_VECTOR_INIT(inputDevicesInfo)
, MP_VECTOR_INIT(outputDevicesInfo)
{
	m_eventVoipLog = NULL;
	myUserID = 0;
	m_eventChannel = NULL;
	m_voipManagerPipe = NULL;
	clientInstance = NULL;
	connectionStatus = CS_DISCONNECTED;
	channelConnectionStatus = CS_DISCONNECTED;
	audioCapDeviceID = 0;
	tempID = -1;
//	wParamIDChannel = 0;

	if( context && context->mpWindow && context->mpWindow->GetMainWindowHandle() != NULL)
		voipWindow.Create( (HWND)(context->mpWindow->GetMainWindowHandle()), NULL, "Conaito fake voip window", WS_POPUPWINDOW);
	else
		voipWindow.Create( NULL, NULL, "Conaito fake voip main window", WS_POPUPWINDOW);
	//voipWindow.Create( NULL, NULL, "Conaito fake voip window", WS_POPUPWINDOW);
	bNeedLogin = true;
	bLaunchedSoundSystem = false;

	m_currentProxySettingsListIndex = 0;
	ResetDirectConnectFlags();

	m_mainSoundSystem = SOUNDSYSTEM_DSOUND;
#ifdef USE_MUMBLE_SDK
	OSVERSIONINFOEX ovi;
	memset(&ovi, 0, sizeof(ovi));

	ovi.dwOSVersionInfoSize=sizeof(ovi);
	GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&ovi));

	if ((ovi.dwMajorVersion > 6) || ((ovi.dwMajorVersion == 6) && (ovi.dwBuildNumber >= 6001))) {
		HMODULE hLib = LoadLibrary("AVRT.DLL");
		if (hLib != NULL) {
			FreeLibrary(hLib);
			m_mainSoundSystem = SOUNDSYSTEM_WASAPI;
			return;
		}
	}
#endif
	
}

CVoipManagerConaitoServer::~CVoipManagerConaitoServer(void)
{
	m_bVoipLog = false;	
	m_bChannel = false;
	m_voipManagerPipe = NULL;
	DestroyVoipSystem();
	DestroyEvoSystem();

	if( m_eventVoipLog != NULL)
	{
		SetEvent(m_eventVoipLog);
		//09.04.2014 лишь при закрытии приложения - иначе риск повисаний
		if (WaitForSingleObject( m_hVoipLogThread, 2000) == WAIT_TIMEOUT)
			TerminateThread(m_hVoipLogThread, 0);

		CloseHandle(m_eventVoipLog);
		m_eventVoipLog = NULL;
	}

	if( m_eventChannel != NULL)
	{
		SetEvent(m_eventChannel);
		//09.04.2014 лишь при закрытии приложения - иначе риск повисаний
		if (WaitForSingleObject( m_hChannelThread, 2000) == WAIT_TIMEOUT)
			TerminateThread(m_hChannelThread, 0);

		CloseHandle(m_eventChannel);
		m_eventChannel = NULL;
	}
	if( audioSource == VAS_VIRTUALCABLE)
	{
		//audioCaptureManager.StopCapture();
		//settings.SetReplacedAudioDeviceName(L"");
	}	
}

LPVOID CVoipManagerConaitoServer::GetClientInstance()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetClientInstance");
		WriteLogLine( str.GetBuffer());
	}
	return clientInstance;
}

CComString CVoipManagerConaitoServer::GetChannelPath()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetChannelPath");
		WriteLogLine( str.GetBuffer());
	}
	return channelPath;
};

void CVoipManagerConaitoServer::Initialize( bool abTwoApplication)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: Initialize");
		WriteLogLine( str.GetBuffer());
	}
	bTwoApplication = abTwoApplication;
	// Читаем настройки и инициализируем систему на них
	InitializeEvoSystem();

	// Проверим, возможно в прошлый раз упало не восстановив звуковое устройство
	/*std::wstring wsReplacedDevice = settings.GetReplacedAudioDeviceName();
	if( !wsReplacedDevice.empty())
	{
		if( audioCaptureManager.RestoreReplacedDeviceAfterError( wsReplacedDevice.c_str()) == ACERROR_NOERROR)
			settings.SetReplacedAudioDeviceName(L"");
	}*/

	m_currentInputLevel = 0;
	m_currentModeTopInputLevel = MAX_STABLE_AUDIO_LEVEL_VOICE;
	m_currentModeCurrentTopInputLevel = m_currentModeTopInputLevel;
	m_skipCurrentLevelCalculation = true;

	if( !bTwoApplication)
		initializeCompleteEvent.Create( NULL, TRUE/*bManualReset*/, FALSE/*bInitialState*/, NULL/*pEventAttributes*/);
	InitializeVoipSystem( L"", L"");
	m_bChannel = true;
	StartChannelThread();
	m_bVoipLog = true;
	StartVoipLog();	
}

void CVoipManagerConaitoServer::SetManagerPipe( CVoipManagerPipeEVO *apVoipManagerPipe)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: SetManagerPipe");
		WriteLogLine( str.GetBuffer());
	}
	m_voipManagerPipe = apVoipManagerPipe;
}

void	CVoipManagerConaitoServer::SetServerParams( std::string &aServerIP, int aTcpPort, int anUdpPort, std::vector<ProxyInfo> aProxyInfoList)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: SetServerParams");
		WriteLogLine( str.GetBuffer());
	}
	/*if( serverIP.Compare( aServerIP.c_str()) || tcpPort != atcpPort || udpPort != audpPort)
	{
		connectionStatus = CS_DISCONNECTED;
	}*/
	serverIP = aServerIP.c_str();
	
	tcpPort = aTcpPort; 
	udpPort = anUdpPort;

	m_proxySettingsMutex.lock();
	proxySettingsList.clear();
	httpsProxyInfo.proxyType = 2;
	httpsProxyInfo.sProxyUserName = L"max";
	httpsProxyInfo.sProxyPassword = L"111111";
	USES_CONVERSION;
	httpsProxyInfo.sProxyIP = A2W(aServerIP.c_str());
	httpsProxyInfo.sProxyIP += L":64738";
	proxySettingsList.push_back( httpsProxyInfo);
	std::vector<ProxyInfo>::iterator iter = aProxyInfoList.begin();
	for( ;iter != aProxyInfoList.end(); iter++)
		proxySettingsList.push_back( (ProxyInfo)(*iter));
	m_proxySettingsMutex.unlock();

	ResetDirectConnectFlags();

}

//! Подключиться к определенному серверу голосовой связи
voipError	CVoipManagerConaitoServer::ConnectImpl( bool aCheckConnecting)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: ConnectImpl %d",  (int)m_currentProxySettingsListIndex);
		WriteLogLine( str.GetBuffer());
	}

#ifdef USE_CONAITO_SDK_LIB
	if( !clientInstance)
		return ERROR_NOTINITIALIZED;

	if( aCheckConnecting && (connectionStatus == CS_CONNECTING || connectionStatus == CS_CONNECTED))
	{
		if( EVO_WRAPPER_IsConnecting( clientInstance) || EVO_WRAPPER_IsConnected(clientInstance))
			return ERROR_NOERROR;
	}	
#if CONAITO_SDK_LIB_VERSION == 3
	USES_CONVERSION;
	BOOL b = EVO_WRAPPER_Connect(clientInstance, A2W(serverIP.GetBuffer()), tcpPort, udpPort, 0, 0);
#else
	BOOL b = EVO_WRAPPER_Connect(clientInstance, serverIP, tcpPort, udpPort, 0, 0);
#endif

#endif
#ifdef USE_MUMBLE_SDK
	if( !clientInstance)
		return ERROR_NOTINITIALIZED;

/*	if( aCheckConnecting && (connectionStatus == CS_CONNECTING || connectionStatus == CS_CONNECTED))
	{
	//	if( MUMBLE_IsConnecting( clientInstance) || MUMBLE_IsConnected(clientInstance))
			return ERROR_NOERROR;
	}	*/
	//мамбл перед новым подключением должен успеть отключиться от старого
	if(aCheckConnecting && (connectionStatus == CS_CONNECTING || connectionStatus == CS_CONNECTED))
	{
		return ERROR_NOERROR;
	}
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: ConnectImpl connecting... server = %s %d",  serverIP.GetBuffer(), m_currentProxySettingsListIndex);
		WriteLogLine( str.GetBuffer());
	}
	BOOL b = TRUE;
#endif
	if( !b)
	{
		//CALLBACK_NO_PARAMS(onConnectFailed);
		//m_voipManagerPipe->SendConnectionStatus( CS_CONNECTFAILED);		
		return ERROR_CONNECTION_FAILED;
	}
	bNeedLogin = true;
	connectionStatus = CS_CONNECTING;
	channelConnectionStatus = CS_DISCONNECTED;

#ifdef USE_MUMBLE_SDK
	USES_CONVERSION;

	m_proxySettingsMutex.lock();
	if (m_currentProxySettingsListIndex > 0 && m_currentProxySettingsListIndex <= proxySettingsList.size())
	{
		m_directConnectNow = false;
		ProxyInfo info = proxySettingsList[m_currentProxySettingsListIndex-1];
		if( context->mpLogWriter)
		{		
			CComString str;
			str.Format("[VOIPSERVER]: ConnectImpl sProxyIP = %s, sProxyUserName = %s, sProxyPassword = %s, proxyType = %d",  W2A(info.sProxyIP.c_str()), W2A(info.sProxyUserName.c_str()), W2A(info.sProxyPassword.c_str()), info.proxyType);
			WriteLogLine( str.GetBuffer());
		}
		MUMBLE_SetProxySettings(clientInstance, info.sProxyIP.c_str(), info.sProxyUserName.c_str(), info.sProxyPassword.c_str(), info.proxyType);
	}
	else {
		std::wstring emptyString = L" ";
		MUMBLE_SetProxySettings(clientInstance, emptyString.c_str(), emptyString.c_str(), emptyString.c_str(), 0);
		m_directConnectNow = true;
	}
	m_proxySettingsMutex.unlock();

	b = MUMBLE_Connect(clientInstance, A2W(serverIP.GetBuffer()), tcpPort, udpPort, 0, 0);
#endif

	//
	//m_voipManagerPipe->SendConnectionStatus( CS_CONNECTING);
	//	
	return ERROR_NOERROR;
}

void CVoipManagerConaitoServer::JoinChannel( std::string &aRoomName)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: Join Channel %s", aRoomName.c_str());
		WriteLogLine( str.GetBuffer());
	}
	roomName = aRoomName.c_str();
	if( connectionStatus == CS_CONNECTED)
	{
#ifdef USE_CONAITO_SDK_LIB
		if( EVO_WRAPPER_IsAuthorized( clientInstance))
#endif
#ifdef USE_MUMBLE_SDK
		if( MUMBLE_IsAuthorized( clientInstance))	
#endif
		{
			// вызывается в потоке
			//OnAccepted();
			SetEvent(m_eventChannel);
		}
		else
		{
			if( context->mpLogWriter)
			{		
				CComString str;
				str.Format("[VOIPSERVER]: Accepting IsAuthorized is false");
				WriteLogLine( str.GetBuffer());
			}
		}
	}
	else
	{
		if( context->mpLogWriter)
		{		
			CComString str;
			str.Format("[VOIPSERVER]: Accepting connectionStatus != CS_CONNECTED  connectionStatus = %d",  connectionStatus);
			WriteLogLine( str.GetBuffer());
		}
	}
}

//! Отключиться от текущего сервера голосовой связи
voipError	CVoipManagerConaitoServer::DisconnectImpl()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: DisconnectImpl  connectionStatus = %d",  connectionStatus);
		WriteLogLine( str.GetBuffer());
	}

	bNeedLogin = true;
	if( connectionStatus == CS_DISCONNECTED ||
		connectionStatus == CS_DISCONNECTING)
		return ERROR_NOERROR;
#ifdef USE_CONAITO_SDK_LIB
	if( !EVO_WRAPPER_IsConnected( clientInstance) && !EVO_WRAPPER_IsConnecting( clientInstance))
#endif
#ifdef USE_MUMBLE_SDK
	if( !MUMBLE_IsConnected( clientInstance) && !MUMBLE_IsConnecting( clientInstance))
#endif
		return ERROR_NOERROR;

	DisconnectCurrentChannel();
	// надо бы подождать пока отцепится от канала..
	// ??
#ifdef USE_CONAITO_SDK_LIB
	EVO_WRAPPER_Disconnect( clientInstance);
#endif
#ifdef USE_MUMBLE_SDK
	MUMBLE_Disconnect( clientInstance);
#endif
//	connectionStatus = CVoipManagerConaitoServer::CONNECTION_STATUS::CS_DISCONNECTED;
	connectionStatus = CS_DISCONNECTED;

	if( m_voipManagerPipe != NULL)
		m_voipManagerPipe->SendConnectionStatus( connectionStatus);

	//CALLBACK_NO_PARAMS(onDisconnected);
	return ERROR_NOERROR;
}

//! Получение режима активации передачи звука
bool CVoipManagerConaitoServer::GetVoiceActivated()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetVoiceActivated");
		WriteLogLine( str.GetBuffer());
	}
	if( !clientInstance)
		return false;

	return voiceActivated;
}

bool CVoipManagerConaitoServer::GetMicrophoneEnabled()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetMicrophoneEnabled");
		WriteLogLine( str.GetBuffer());
	}
	return microphoneEnabled;
}

//! Принудительное начало передачи данных
void CVoipManagerConaitoServer::StartTransmitting()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: StartTransmitting");
		WriteLogLine( str.GetBuffer());
	}
	microphoneEnabled = true;
	if( clientInstance)
	{
		HandleTransmissionStateChanged();
	}
}

//! Принудительное окончание передачи данных
void CVoipManagerConaitoServer::StopTransmitting()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: StopTransmitting");
		WriteLogLine( str.GetBuffer());
	}
	microphoneEnabled = false;
	if( clientInstance)
	{
		HandleTransmissionStateChanged();
	}
}

//! Удалить менеджер голосовой связи
void CVoipManagerConaitoServer::Release()
{
/*	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: Release");
		WriteLogLine( str.GetBuffer());
	}
*/	MP_DELETE_THIS;
}

//! Возвращает громкость вашего голоса
float CVoipManagerConaitoServer::GetCurrentInputLevel()
{
/*	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetCurrentInputLevel");
		WriteLogLine( str.GetBuffer());
	}*/
	//return EVO_WRAPPER_GetCurrentInputLevel(clientInstance) / 20.0f;
#ifdef USE_MUMBLE_SDK
	CurrentInputLevelCalculation method = VACADEM;
	m_currentInputLevel = MUMBLE_GetCurrentInputLevel(clientInstance, method);
	m_currentInputLevel = m_currentInputLevel/32768;

//	updateMumble();
	
#endif
	return (float)m_currentInputLevel;
}

//! Возвращает громкость вашего голоса с учетом параметров усиления и порога
float CVoipManagerConaitoServer::GetCurrentVoiceInputLevelNormalized()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetCurrentVoiceInputLevelNormalized");
		WriteLogLine( str.GetBuffer());
	}
	if( GetMicActivationLevel() >= 0.9999)
		return 0;

	float currentLevel = 0;

	if( voiceActivated)
	{
		currentLevel = (GetCurrentInputLevel() - GetMicActivationLevel()) / (1.0f - GetMicActivationLevel());
	}
	else
	{
		currentLevel = GetCurrentInputLevel();
	}
	//currentLevel *= (1 + GetMicGainLevel() * 3);

	if( currentLevel < 0)	currentLevel = 0;
	if( currentLevel > 1.0f)	currentLevel = 1.0f;
	return currentLevel;
}

//! Устанавливает режим получения звука с компа с помощью драйвера виртуальной аудио-карты VAC4.10
/*bool CVoipManagerConaitoServer::SetInputVirtualAudioCableMode()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: SetInputVirtualAudioCableMode");
		WriteLogLine( str.GetBuffer());
	}
	ATLASSERT(false);
	return false;
}*/

bool CVoipManagerConaitoServer::InitializeVoipOnAudioDevice( const wchar_t* alpcDeviceGuid)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: InitializeVoipOnAudioDevice");
		WriteLogLine( str.GetBuffer());
	}
	ATLASSERT(false);
	return false;
}

bool CVoipManagerConaitoServer::SetUserPosition( int nUserID, float x, float y, float z)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: SetUserPosition");
		WriteLogLine( str.GetBuffer());
	}
#ifdef USE_CONAITO_SDK_LIB
	BOOL result = EVO_WRAPPER_SetUserPosition( clientInstance, nUserID, x, y, z);	
#endif
#ifdef USE_MUMBLE_SDK
	BOOL result = MUMBLE_SetUserPosition( clientInstance, nUserID, x, y, z);	
#endif
	return (result!=0);
}

////////////////////////////////////////////////////////////////////
// Protected
////////////////////////////////////////////////////////////////////
//! Создание окна и инициализации библиотеки EVOVOIP
voipError CVoipManagerConaitoServer::InitializeEvoSystem()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: InitializeEvoSystem");
		WriteLogLine( str.GetBuffer());
	}
	ATLASSERT( clientInstance == NULL);

	if( !voipWindow.IsWindow())
		return ERROR_UNKNOWN;
#ifdef USE_CONAITO_SDK_LIB
	BOOL b = EVO_WRAPPER_InitVoIP(voipWindow.m_hWnd, FALSE, &clientInstance);
#endif
#ifdef USE_MUMBLE_SDK
	BOOL b = MUMBLE_InitVoIP(voipWindow.m_hWnd, FALSE, &clientInstance, m_mainSoundSystem);
	if( context->mpLogWriter)
	{
		USES_CONVERSION;
		CComString str;
		str.Format("[VOIPSERVER]: Initialize for mumble");
		WriteLogLine( str.GetBuffer());
	}
#endif
	ATLASSERT(b);
	ATLASSERT(clientInstance);
	if( !clientInstance)
	{
		if( context->mpLogWriter)
		{
			USES_CONVERSION;
			CComString str;
			str.Format("[VOIPSERVER]: ERROR: clientInstance is null");
			WriteLogLine( str.GetBuffer());
		}
		return ERROR_NOTINITIALIZED;
	}
	
	WriteLogLine( "[VOIPSERVER]: GrabDevicesInfo");
	
	if( GrabDevicesInfo( false))
	{
		if( context->mpLogWriter)
		{
			USES_CONVERSION;
			CComString str;
			str.Format("[VOIPSERVER]: InitializeEvoSystem is success");
			WriteLogLine( str.GetBuffer());
		}
	}

	return ERROR_NOERROR;
}

void CVoipManagerConaitoServer::SetVoiceGainLevel( float aVal)
{
	if( context->mpLogWriter)
	{
		USES_CONVERSION;
		CComString str;
		str.Format("[VOIPSERVER]: SetVoiceGainLevel %f", (aVal));
		WriteLogLine( str.GetBuffer());
	}

	if(clientInstance != NULL)
#ifdef USE_CONAITO_SDK_LIB
		EVO_WRAPPER_SetVoiceGainLevel( clientInstance, (INT32)aVal);
#endif
#ifdef USE_MUMBLE_SDK
		MUMBLE_SetVoiceGainLevel( clientInstance, aVal);
#endif
}

void CVoipManagerConaitoServer::SetVoiceActivationLevel( float aVal)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: SetVoiceActivationLevel");
		WriteLogLine( str.GetBuffer());
	}
	if(clientInstance != NULL)
	{
		CComString str;
		str.Format("[VOIP_SERVER_ACT_LEVEL]: SetVoiceActivationLevel: %u", (int)aVal);
		WriteLogLine( str.GetBuffer());
#ifdef USE_CONAITO_SDK_LIB
		EVO_WRAPPER_SetVoiceActivationLevel( clientInstance, aVal);
#endif
#ifdef USE_MUMBLE_SDK
		MUMBLE_SetVoiceActivationLevel( clientInstance, (INT32)aVal);
#endif
	}
}

const wchar_t *CVoipManagerConaitoServer::GetCurrentEvoVoipDeviceGuid()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetCurrentEvoVoipDeviceGuid");
		WriteLogLine( str.GetBuffer());
	}
	return currentEvoVoipDeviceGuid.c_str();
}

const wchar_t *CVoipManagerConaitoServer::GetCurrentEvoVoipOutDeviceGuid()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetCurrentEvoVoipOutDeviceGuid");
		WriteLogLine( str.GetBuffer());
	}
	return currentEvoVoipOutDeviceGuid.c_str();
}



//! Инициализации голосовой связи на устройстве
voipError CVoipManagerConaitoServer::InitializeVoipSystem( const wchar_t* alpcDeviceGuid, const wchar_t* alpcOutDeviceGuid)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: InitializeVoipSystem");
		WriteLogLine( str.GetBuffer());
	}
	mutex.lock();
	if( alpcDeviceGuid != NULL && wcslen( alpcDeviceGuid)>0)
		currentEvoVoipDeviceGuid = alpcDeviceGuid;
	if( alpcOutDeviceGuid != NULL && wcslen( alpcOutDeviceGuid)>0)
		currentEvoVoipOutDeviceGuid = alpcOutDeviceGuid;

	if( bTwoApplication)
	{
		Run();
		mutex.unlock();
		return initializeErrorCode;
	}
	mutex.unlock();
	if( IsRunning())
	{
		Terminate();
		ViskoeThread::CThreadImpl<CVoipManagerConaitoServer>::Release();
	}
	initializeCompleteEvent.ResetEvent();

	Start();

	// Ждем инициализацию в течении 10 секунд
	if( !initializeCompleteEvent.WaitForEvent( 10000))
	{
		Terminate();
		ViskoeThread::CThreadImpl<CVoipManagerConaitoServer>::Release();

		// initialize failed
		if( context->mpLogWriter)
		{
			USES_CONVERSION;
			CComString str;
			str.Format("[VOIPSERVER]: InitializeVoipSystem FAILED because of 10 seconds timeout");
			WriteLogLine( str.GetBuffer());
		}

		DestroyVoipSystem();
		return ERROR_NOTINITIALIZED;
	}

	Stop();

	// initialize ok
	return initializeErrorCode;
}

DWORD	CVoipManagerConaitoServer::Run()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: run");
		WriteLogLine( str.GetBuffer());
	}
	initializeErrorCode = ERROR_NOERROR;

	if( context->mpLogWriter)
	{
		USES_CONVERSION;
		CComString str;
		str.Format("[VOIPSERVER]: InitializeVoipSystem: currentEvoVoipDeviceGuid = %s, currentEvoVoipOutDeviceGuid = %s", W2A(currentEvoVoipDeviceGuid.c_str()), W2A(currentEvoVoipOutDeviceGuid.c_str()));
		WriteLogLine( str.GetBuffer());
	}

	bool bDefaultsUsed = false;

	if( bLaunchedSoundSystem)
	{
		DestroyVoipSystem(false);
		if( bLaunchedSoundSystem)
		{
			initializeErrorCode = ERROR_NOTINITIALIZED;		
			return 0;
		}
	}

	BOOL b = FALSE;
#ifdef USE_CONAITO_SDK_LIB
	EVO_WRAPPER_EnableVoiceActivation(clientInstance, FALSE);	
	EVO_WRAPPER_SetVoiceActivationLevel(clientInstance, 0);
	EVO_WRAPPER_RegisterTalkingCallback(clientInstance, UserTalking, this);

	bool bUseDS = (bool)settings.GetUseDS();
	EVO_WRAPPER_SoundSystemType soundSystem = bUseDS ? SOUNDSYSTEM_DSOUND:SOUNDSYSTEM_WINMM;
#endif
#ifdef USE_MUMBLE_SDK
	MUMBLE_EnableVoiceActivation(clientInstance, FALSE);	
	MUMBLE_SetVoiceActivationLevel(clientInstance, 0);
//	MUMBLE_RegisterTalkingCallback(clientInstance, UserTalking, this);
// для vista and win7 нужно будет сделать определение и юзать SOUNDSYSTEM_WASAPI
//	SoundSystem soundSystem = settings.GetUseDS() ? SOUNDSYSTEM_DSOUND:SOUNDSYSTEM_WASAPI;
	EVO_WRAPPER_SoundSystemType soundSystem = m_mainSoundSystem;
#endif

	

	// Нужно определить, к какому устройству будем подключаться
	// Алгоритм:
	// Получаем из настроек номер и название устройства
	// Сравниваем их с текущими значениями
	// Если все совпадает, устанавливаем их
	// Иначе берем текущее устройство
	std::wstring	settingsDeviceGuid = currentEvoVoipDeviceGuid;
	std::wstring	settingsOutDeviceGuid = currentEvoVoipOutDeviceGuid;	

	int inputDeviceID = -1;
	/*if( settingsDeviceGuid != L"Default device")
	{*/
		if( (inputDeviceID = GetEvoVoipDeviceIDByGuid( settingsDeviceGuid.c_str())) == -1)
		{
			//! Использованное в прошлый раз устройство не найдено, используем текущее устройство
			inputDeviceID = GetCurrentInputDeviceID( soundSystem);
		}
		else
		{
			//! Использованное в прошлый раз устройство найдено, используем его
		}
	//}

	int outputDeviceID = -1;
	if( settingsOutDeviceGuid != L"Default device")
	{
		if( (outputDeviceID = GetEvoVoipOutDeviceIDByGuid( settingsOutDeviceGuid.c_str())) == -1)
		{
			//! Использованное в прошлый раз устройство не найдено, используем текущее устройство
			int outputDeviceID = GetCurrentOutputDeviceID( soundSystem);
		}
		else
		{
			//! Использованное в прошлый раз устройство найдено, используем его
		}
	}
	else
	{
#ifdef USE_MUMBLE_SDK
		outputDeviceID = GetDefaultOutputDeviceID( soundSystem);
#endif
	}

	if( context->mpLogWriter)
	{
		CComString str;
		str.Format("[VOIPSERVER]: Current outputDeviceID = %u, inputDeviceID = %u", outputDeviceID, inputDeviceID);
		WriteLogLine( str.GetBuffer());
	}

	BOOL bSuccess = false;	

	unsigned int uQuality = 3;//settings.GetVoiceQuality();
	unsigned int uFrequency = settings.GetRecorderFreq();
#ifdef USE_CONAITO_SDK_LIB
#if CONAITO_SDK_LIB_VERSION == 3
	bSuccess = EVO_WRAPPER_LaunchSoundSystem(clientInstance, soundSystem
		, inputDeviceID
		, outputDeviceID
		, uFrequency, uQuality
		, FALSE//stereo playback
		);
#else
	bSuccess = EVO_WRAPPER_LaunchSoundSystem(clientInstance, soundSystem
		, inputDeviceID
		, outputDeviceID
		, uFrequency, uQuality
#endif
#endif USE_CONAITO_SDK_LIB
#ifdef USE_MUMBLE_SDK
		bSuccess = MUMBLE_LaunchSoundSystem(clientInstance, soundSystem
		, inputDeviceID
		, outputDeviceID
		, uFrequency, uQuality
		, FALSE//stereo playback
		);
#endif


	if( !bSuccess)
	{
		inputDeviceID = GetDefaultInputDeviceID( soundSystem);
		outputDeviceID = GetDefaultOutputDeviceID( soundSystem);
		settingsOutDeviceGuid = L"Default device";

		if( context->mpLogWriter)
		{
			CComString str;
			str.Format("[VOIPSERVER]: 1. !bSuccess, Default: inputDeviceID = %u, outputDeviceID = %u", inputDeviceID, outputDeviceID);
			WriteLogLine( str.GetBuffer());
		}


#ifdef USE_CONAITO_SDK_LIB

#if CONAITO_SDK_LIB_VERSION == 3
		bSuccess = EVO_WRAPPER_LaunchSoundSystem(clientInstance, soundSystem
			, inputDeviceID
			, outputDeviceID
			, settings.GetRecorderFreq(), settings.GetVoiceQuality()
			, FALSE // stereo playback
			);
#else
		bSuccess = EVO_WRAPPER_LaunchSoundSystem(clientInstance, soundSystem
			, inputDeviceID
			, outputDeviceID
			, settings.GetRecorderFreq(), settings.GetVoiceQuality());
#endif

#endif USE_CONAITO_SDK_LIB
#ifdef USE_MUMBLE_SDK
			bSuccess = MUMBLE_LaunchSoundSystem(clientInstance, soundSystem
				, inputDeviceID
				, outputDeviceID
				, settings.GetRecorderFreq(), 3//settings.GetVoiceQuality()
				, FALSE // stereo playback
				);
#endif
		bDefaultsUsed = true;
	}	
	if( !bSuccess)
	{
		if( context->mpLogWriter)
		{
			CComString str;
			str.Format("[VOIPSERVER]: 2. !bSuccess, inputDeviceID = %u, outputDeviceID = %u", inputDeviceID, outputDeviceID);
			WriteLogLine( str.GetBuffer());
		}

		DestroyVoipSystem();

		if( !bTwoApplication)
			initializeCompleteEvent.SetEvent();

		initializeErrorCode = ERROR_NOTINITIALIZED;		
		return 0;
	}

	bLaunchedSoundSystem = true;
	currentEvoVoipDeviceGuid = settingsDeviceGuid;
	currentEvoVoipOutDeviceGuid = settingsOutDeviceGuid;
	//if( settingsDeviceGuid != L"Default device")
	currentEvoVoipDeviceGuid = GetDeviceGuidByEvoVoidID( inputDeviceID);	

#ifdef USE_CONAITO_SDK_LIB
		EVO_WRAPPER_EnableVoiceActivation(clientInstance, voiceActivated);
		float val = GET_RANGED_VAL(1, 20, micActivationLevel);
		EVO_WRAPPER_SetVoiceActivationLevel( clientInstance, (INT32)val);
		// убрано временно для теста по умолчанию
		//EVO_WRAPPER_EnableDenoising( clientInstance, TRUE);
		///
		EVO_WRAPPER_SetMasterVolume( clientInstance, SOUND_VOLUME_MAX);

#endif
#ifdef USE_MUMBLE_SDK
		MUMBLE_EnableVoiceActivation(clientInstance, voiceActivated);
		float val = GET_RANGED_VAL(1, 20, micActivationLevel);
		MUMBLE_SetVoiceActivationLevel( clientInstance, (INT32)val);

		MUMBLE_EnableDenoising( clientInstance, TRUE);
	
#endif
		m_iLogMode = settings.GetLogMode();	
		CComString str;
		str.Format("[VOIP_SERVER_ACT_LEVEL]: Run voiceActivated = %u, val = %u", (int)voiceActivated, (int)val);
		WriteLogLine( str.GetBuffer());

		OnGainLevelChanged();

#ifdef USE_CONAITO_SDK_LIB
	/*EVO_WRAPPER_EnableAutoPositioning( clientInstance, TRUE);
	EVO_WRAPPER_AutoPositionUsers(clientInstance);*/

	EVO_WRAPPER_SetUserVolume( clientInstance, EVO_WRAPPER_GetMyUserID(clientInstance), SOUND_VOLUME_MAX);
#endif
#ifdef USE_MUMBLE_SDK
		MUMBLE_SetUserVolume( clientInstance, MUMBLE_GetMyUserID(clientInstance), SOUND_VOLUME_MAX);
#endif
		SetDenoisingEnabled( denoisingEnabled);
		SetAutoGaneControlEnabled( agcEnabled);
#ifdef USE_CONAITO_SDK_LIB
	// !!!!!!!!!!!!!!!!//
	EVO_SetEncoderComplexity( clientInstance, COMPLEXITY_MIN);
#endif
//////////////////////////////////////////////////////////////////////////
	if( !bTwoApplication)
		initializeCompleteEvent.SetEvent();

	initializeErrorCode = bDefaultsUsed ? ERROR_DEFAULTS_USED : ERROR_NOERROR;
	return 0;
}

bool CVoipManagerConaitoServer::RebuildIfEmptyDevicesList()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: RebuildIfEmptyDevicesList");
		WriteLogLine( str.GetBuffer());
	}
	if( inputDevicesInfo.size() != 0)
		return true;

//	EVO_WRAPPER_SoundSystemType soundSystem = settings.GetUseDS() ? SOUNDSYSTEM_DSOUND:SOUNDSYSTEM_WINMM;
	if( inputDevicesInfo.size() != 0)
	{
		// нашли, что что-то подключили
		return true;
	}
	return false;
}

void CVoipManagerConaitoServer::DisconnectCurrentChannel()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: DisconnectCurrentChannel");
		WriteLogLine( str.GetBuffer());
	}
	if( channelConnectionStatus != CS_DISCONNECTED &&
		channelConnectionStatus != CS_DISCONNECTING)
	{
#ifdef USE_CONAITO_SDK_LIB
		EVO_WRAPPER_DoLeaveChannel( clientInstance);
#endif
#ifdef USE_MUMBLE_SDK
		MUMBLE_DoLeaveChannel( clientInstance);
#endif
		channelConnectionStatus = CS_DISCONNECTING;
	}
}

void CVoipManagerConaitoServer::DestroyVoipSystem(bool aNeedDisconnect)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: DestroyVoipSystem");
		WriteLogLine( str.GetBuffer());
	}

	if (aNeedDisconnect)//при смене устройства не отключаемся от сервера, только останавливаем устройства
		DisconnectImpl();

	if( clientInstance)
	{		
#ifdef USE_CONAITO_SDK_LIB
		bool bSuccess = EVO_WRAPPER_ShutdownSoundSystem( clientInstance);
#endif
#ifdef USE_MUMBLE_SDK
		bool bSuccess = MUMBLE_ShutdownSoundSystem( clientInstance)!=0;
#endif

		if(bSuccess)
			bLaunchedSoundSystem = false;
		else
		{
			if( context->mpLogWriter)
			{
				CComString str;
				str.Format("[VOIPSERVER]: 1. !bSuccess, EVO_WRAPPER_ShutdownSoundSystem");
				WriteLogLine( str.GetBuffer());
			}
		}
	}
}

//! Удаление окна и деинициализации библиотеки EVOVOIP
void CVoipManagerConaitoServer::DestroyEvoSystem()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: DestroyEvoSystem");
		WriteLogLine( str.GetBuffer());
	}
	if( clientInstance)
	{		
#ifdef USE_CONAITO_SDK_LIB
		EVO_WRAPPER_CloseVoIP( clientInstance);
#endif
#ifdef USE_MUMBLE_SDK
		MUMBLE_CloseVoIP( clientInstance);
#endif
		clientInstance = NULL;
	}

	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: MUMBLE_CloseVoIP success");
		WriteLogLine( str.GetBuffer());
	}

	if( voipWindow.IsWindow())
		voipWindow.DestroyWindow();
}

////////////////////////////////////////////////////////////////////////////////
// Events handling
////////////////////////////////////////////////////////////////////////////////
//! Вызывается после установки свойств
void CVoipManagerConaitoServer::OnDirectSoundUsedChanged()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnDirectSoundUsedChanged");
		WriteLogLine( str.GetBuffer());
	}
	//??
}

void CVoipManagerConaitoServer::OnGainLevelChanged()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnGainLevelChanged");
		WriteLogLine( str.GetBuffer());
	}
	if( clientInstance)
	{
		//float val = GET_RANGED_VAL(1000, 8000, micGainLevel);
		//EVO_WRAPPER_SetVoiceGainLevel( clientInstance, val);
	}
}

void CVoipManagerConaitoServer::OnActivationLevelChanged()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnActivationLevelChanged");
		WriteLogLine( str.GetBuffer());
	}
	if( clientInstance)
	{
		float val = GET_RANGED_VAL(1, 20, micActivationLevel);
		CComString str;
		str.Format("[VOIP_SERVER_ACT_LEVEL]: OnActivationLevelChanged: %u", (int)val);
		WriteLogLine( str.GetBuffer());
#ifdef USE_CONAITO_SDK_LIB
		EVO_WRAPPER_SetVoiceActivationLevel( clientInstance, val);
#endif
#ifdef USE_MUMBLE_SDK
		MUMBLE_SetVoiceActivationLevel( clientInstance, (INT32)val);
#endif
	}
}

void CVoipManagerConaitoServer::OnVoiceActivatedChanged()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnVoiceActivatedChanged");
		WriteLogLine( str.GetBuffer());
	}
	if( clientInstance)
	{
		HandleTransmissionStateChanged();

		OnActivationLevelChanged();
		OnAutoVolumeChanged();
	}
}

void CVoipManagerConaitoServer::OnStartTransmitting()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnStartTransmitting");
		WriteLogLine( str.GetBuffer());
	}
	if( clientInstance)
	{
		WriteLogLine( "[VOIP_SERVER] OnStartTransmitting");
#ifdef USE_CONAITO_SDK_LIB
		EVO_WRAPPER_StartTransmitting( clientInstance);
#endif
#ifdef USE_MUMBLE_SDK
		MUMBLE_StartTransmitting( clientInstance);
#endif
	}
}

void CVoipManagerConaitoServer::OnStopTransmitting()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnStopTransmitting");
		WriteLogLine( str.GetBuffer());
	}
	if( clientInstance)
	{
		WriteLogLine( "[VOIP_SERVER] OnStopTransmitting");
#ifdef USE_CONAITO_SDK_LIB
		EVO_WRAPPER_StopTransmitting( clientInstance);
#endif
#ifdef USE_MUMBLE_SDK
		MUMBLE_StopTransmitting( clientInstance);
#endif
	}
}

bool CVoipManagerConaitoServer::OnHandleTransmissionStateChanged( bool aMicrophoneEnabled, bool aVoiceActivated)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnHandleTransmissionStateChanged");
		WriteLogLine( str.GetBuffer());
	}
	microphoneEnabled = aMicrophoneEnabled;
	voiceActivated = aVoiceActivated;
	HandleTransmissionStateChanged();
	if( clientInstance)
		return microphoneEnabled & !voiceActivated;
	return false;
}

int CVoipManagerConaitoServer::GetMyUserID()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetMyUserID");
		WriteLogLine( str.GetBuffer());
	}
	if( clientInstance)
#ifdef USE_CONAITO_SDK_LIB
		return EVO_WRAPPER_GetMyUserID( clientInstance);
#endif
#ifdef USE_MUMBLE_SDK
		return MUMBLE_GetMyUserID( clientInstance);
#endif
	return 0;
}

void CVoipManagerConaitoServer::HandleTransmissionStateChanged()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: HandleTransmissionStateChanged");
		WriteLogLine( str.GetBuffer());
	}
	if( !clientInstance)
		return;
	CComString sLog;
	sLog.Format("[VOIP_SERVER] Transmission state changed: microphoneEnabled = %u, voiceActivated = %u", (int)microphoneEnabled, (int)voiceActivated);
	WriteLogLine( sLog.GetBuffer());


#ifdef USE_CONAITO_SDK_LIB
	if( microphoneEnabled)
	{
		if(voiceActivated)
		{
			EVO_WRAPPER_StopTransmitting( clientInstance);

			EVO_WRAPPER_EnableVoiceActivation( clientInstance, false);
			EVO_WRAPPER_EnableVoiceActivation( clientInstance, voiceActivated);
		}
		else
		{
			EVO_WRAPPER_EnableVoiceActivation( clientInstance, false);
			EVO_WRAPPER_StartTransmitting( clientInstance);

			m_voipManagerPipe->SendUserTalking( EVO_WRAPPER_GetMyUserID(clientInstance), userName.AllocSysString());
		}
	}
	else
	{
		EVO_WRAPPER_EnableVoiceActivation( clientInstance, false);

		EVO_WRAPPER_StopTransmitting( clientInstance);

		if( !voiceActivated)
		{
			m_voipManagerPipe->SendUserStoppedTalking( EVO_WRAPPER_GetMyUserID(clientInstance), userName.AllocSysString());
		}
	}
#endif
#ifdef USE_MUMBLE_SDK
	if( microphoneEnabled)
	{
		if(voiceActivated)
		{
			OnStopTransmitting();
			MUMBLE_EnableVoiceActivation( clientInstance, false);
			MUMBLE_EnableVoiceActivation( clientInstance, voiceActivated);
		}
		else
		{
			MUMBLE_EnableVoiceActivation( clientInstance, false);
			OnStartTransmitting();

			m_voipManagerPipe->SendUserTalking( MUMBLE_GetMyUserID(clientInstance), userName.AllocSysString());
		}
	}
	else
	{
		MUMBLE_EnableVoiceActivation( clientInstance, false);
		OnStopTransmitting();
		if( !voiceActivated)
		{
			m_voipManagerPipe->SendUserStoppedTalking( MUMBLE_GetMyUserID(clientInstance), userName.AllocSysString());
		}
	}
#endif

#ifdef USE_CONAITO_SDK_LIB
	if( EVO_WRAPPER_IsVoiceActivated( clientInstance))
#endif
#ifdef USE_MUMBLE_SDK
	if( MUMBLE_IsVoiceActivated( clientInstance))
#endif
	{
		WriteLogLine("[VOIP_SERVER_ACT_LEVEL] Voice activation is enabled 2");
	}
	else
	{
		WriteLogLine("[VOIP_SERVER_ACT_LEVEL] Voice activation is disabled 2");
	}

}

void CVoipManagerConaitoServer::OnVoiceQualityChanged()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnVoiceQualityChanged");
		WriteLogLine( str.GetBuffer());
	}
	if( connectionStatus == CS_DISCONNECTED ||
		connectionStatus == CS_DISCONNECTING)
		return;
	//Disconnect();
	ConnectImpl( false);
}

int CVoipManagerConaitoServer::CalculateAverageLevel( int nSampleRate, const short* pRawAudio, int nSamples)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: CalculateAverageLevel");
		WriteLogLine( str.GetBuffer());
	}
#ifdef USE_CONAITO_SDK_LIB
	return EVO_WRAPPER_GetCurrentInputLevel( clientInstance);
#endif
#ifdef USE_MUMBLE_SDK
	return (int)MUMBLE_GetCurrentInputLevel( clientInstance);
#endif
}

//! Вызывается при невозможности соединения с сервером
LRESULT CVoipManagerConaitoServer::OnConnectFailed()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnConnectFailed");
		WriteLogLine( str.GetBuffer());
	}
	if (connectionStatus != CS_DISCONNECTED && ConnectToNextInProxyList())
		return 0;

	connectionStatus = CS_CONNECTFAILED;
	//CALLBACK_NO_PARAMS(onConnectFailed);
	m_voipManagerPipe->SendConnectionStatus( connectionStatus);	
	return 0;
}

//! Вызывается при успешном соединении с сервером
LRESULT CVoipManagerConaitoServer::OnConnectSuccess()
{	
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnConnectSuccess");
		WriteLogLine( str.GetBuffer());
	}
	connectionStatus = CS_CONNECTED;	
/*#if CONAITO_SDK_LIB_VERSION == 3
	USES_CONVERSION;
	EVO_WRAPPER_DoLogin( clientInstance, A2W(userName.GetBuffer()), A2W(userPassword.GetBuffer()));
#else
	EVO_WRAPPER_DoLogin( clientInstance, userName, userPassword);
#endif*/
	//CALLBACK_NO_PARAMS(onConnectSuccess);
	m_voipManagerPipe->SendConnectionStatus( connectionStatus);

	return 0;
}

void CVoipManagerConaitoServer::DoLogin( const wchar_t *aUserName, const wchar_t *aUserPassword, const wchar_t *aRoomName)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: DoLogin");
		WriteLogLine( str.GetBuffer());
	}
	if( connectionStatus != CS_CONNECTED)
	{
		if( context->mpLogWriter)
		{		
			CComString str;
			str.Format("[VOIPSERVER]: DoLogin connectionStatus != CS_CONNECTED");
			WriteLogLine( str.GetBuffer());
		}
		return;
	}

	if( !bNeedLogin && userName == aUserName && (userPassword == aUserPassword)) {
#ifdef USE_CONAITO_SDK_LIB
		if (EVO_WRAPPER_IsAuthorized( clientInstance))
#endif
#ifdef USE_MUMBLE_SDK
		if (MUMBLE_IsAuthorized( clientInstance))	
#endif
		{
			if( context->mpLogWriter)
			{		
				CComString str;
				str.Format("[VOIPSERVER]: Not need DoLogin userName = %s, bNeedLogin = %d", userName.GetBuffer(), bNeedLogin);
				WriteLogLine( str.GetBuffer());
			}
			return;
		}
	}
	userName = aUserName;
	userPassword = aUserPassword;
	if( wcslen(aRoomName) > 0)
		roomName = aRoomName;

#ifdef USE_CONAITO_SDK_LIB
#if CONAITO_SDK_LIB_VERSION == 3
	USES_CONVERSION;
	EVO_WRAPPER_DoLogin( clientInstance, A2W(userName.GetBuffer()), A2W(userPassword.GetBuffer()));
#else
	EVO_WRAPPER_DoLogin( clientInstance, userName, userPassword);
#endif	
	
#endif

#ifdef USE_MUMBLE_SDK
	USES_CONVERSION;
	//MUMBLE_Disconnect(clientInstance);
	// пароль из UpdateVoipConnection() и requestedUserPassword в voipmanger.cpp
	MUMBLE_DoLogin( clientInstance, A2W(userName.GetBuffer()), A2W(userPassword.GetBuffer()));
	//уже подключены - делаем смену канала
	if (MUMBLE_IsAuthorized( clientInstance))
		OnAcceptedFromVoip();
#endif

	bNeedLogin = false;
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: DoLogin userName = %s, roomName = %s", userName.GetBuffer(), roomName.GetBuffer());
		WriteLogLine( str.GetBuffer());
	}
}

//! Вызывается при потери связи с сервером
LRESULT CVoipManagerConaitoServer::OnConnectionLost()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnConnectionLost");
		WriteLogLine( str.GetBuffer());
	}
	if (connectionStatus != CS_DISCONNECTED && ConnectToNextInProxyList())
		return 0;

	bNeedLogin = true;
	connectionStatus = CS_LOSTCONNECTION;
	m_voipManagerPipe->SendConnectionStatus( connectionStatus);

	return 0;
}

bool CVoipManagerConaitoServer::ConnectToNextInProxyList()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: ConnectToNextInProxyList");
		WriteLogLine( str.GetBuffer());
	}
	//if (m_directConnectNow) {
		if (!m_directConnectSuccess)
		{
			if( context->mpLogWriter)
			{		
				CComString str;
				str.Format("[VOIPSERVER]: !m_directConnectSuccess");
				WriteLogLine( str.GetBuffer());
			}

			m_directConnectFailed = true;
			m_currentProxySettingsListIndex++;
			m_proxySettingsMutex.lock();
			unsigned int sizeProxyList = proxySettingsList.size();
			m_proxySettingsMutex.unlock();
			if (m_currentProxySettingsListIndex > sizeProxyList)
			{
				m_currentProxySettingsListIndex = 0;
				return false;
			}
			if( context->mpLogWriter)
			{		
				CComString str;
				str.Format("[VOIPSERVER]: !m_directConnectSuccess %d", m_currentProxySettingsListIndex);
				WriteLogLine( str.GetBuffer());
			}
			ConnectImpl(false);
			return true;
		}
	//}
	return false;
}

LRESULT CVoipManagerConaitoServer::OnAcceptedFromVoip()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnAcceptedFromVoip");
		WriteLogLine( str.GetBuffer());
	}

	if (m_directConnectNow)
	{
		m_directConnectNow = false;
		m_directConnectSuccess = true;
		m_currentProxySettingsListIndex = 0;
	}
	// вызывается в потоке
	//OnAccepted();
	SetEvent(m_eventChannel);
	return 0;
}

//! Вызывается при успешной логинации
LRESULT CVoipManagerConaitoServer::OnAccepted()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnAccepted");
		WriteLogLine( str.GetBuffer());
	}
	if (roomName == "")
	{
		if( context->mpLogWriter)
		{		
			CComString str;
			str.Format("[VOIPSERVER]: roomName empty ");
			WriteLogLine( str.GetBuffer());
		}
		return 0;
	}
	//DisconnectCurrentChannel();
#ifdef USE_CONAITO_SDK_LIB
#if CONAITO_SDK_LIB_VERSION == 3
	USES_CONVERSION;
	EVO_WRAPPER_DoJoinChannel( clientInstance, A2W(roomName.GetBuffer()), L"", A2W(roomName.GetBuffer()), L"");
#else
	EVO_WRAPPER_DoJoinChannel( clientInstance, roomName, "", roomName, "");
#endif	
#endif
#ifdef USE_MUMBLE_SDK
	USES_CONVERSION;
	MUMBLE_DoJoinChannel( clientInstance, A2W(roomName.GetBuffer()), L"", A2W(roomName.GetBuffer()), L"");
#endif
	if( context->mpLogWriter)
	{
		USES_CONVERSION;
		CComString str;
		str.Format("[VOIPSERVER]: Accepted to server, roomName = %s", roomName.GetBuffer());
		WriteLogLine( str.GetBuffer());
	}

#ifdef USE_CONAITO_SDK_LIB
	EVO_SetServerTimeout( clientInstance, 60);
	EVO_SetTcpKeepAliveInterval( clientInstance, 10);
#endif

	OnVoiceActivatedChanged();
	m_voipManagerPipe->SendConnectionStatus( connectionStatus);
	return 0;
}

//! Вызывается при выходе пользователя из сеанса
LRESULT CVoipManagerConaitoServer::OnLoggedOut()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnLoggedOut");
		WriteLogLine( str.GetBuffer());
	}
	return 0;
}

//! Вызывается при отказе сервера в обслуживании пользователя
LRESULT CVoipManagerConaitoServer::OnKicked()
{
	return 0;
}

//! Вызывается при обновлении данных с сервера
LRESULT CVoipManagerConaitoServer::OnServerUpdate()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnServerUpdate");
		WriteLogLine( str.GetBuffer());
	}
	return 0;
}

//! Вызывается 
LRESULT CVoipManagerConaitoServer::OnCommandError(WPARAM wParam)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnCommandError");
		WriteLogLine( str.GetBuffer());
	}
	return 0;
}

LRESULT CVoipManagerConaitoServer::OnMyUserIDRecieved(WPARAM wParam)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnMyUserIDRecieved");
		WriteLogLine( str.GetBuffer());
	}
	m_voipManagerPipe->SendMyUserID( (int)wParam);
	return 0;
}

//! Вызывается при появлении нового пользователя в канале
LRESULT CVoipManagerConaitoServer::OnAddUser(WPARAM wParam)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnAddUser");
		WriteLogLine( str.GetBuffer());
	}
	User user = {0};
	BOOL b = GetEvoUserImpl( (INT32)wParam, &user);
	if ( b)
	{
		if( context->mpLogWriter)
		{		
			CWComString str;
			//std::wstring sUser = user.szNickName;
			str.Format(L"[VOIPSERVER]: OnAddUser user.nUserID==%d, wParam==%d", user.nUserID,  wParam);
			str += L"user.szNickName=="; str+= user.szNickName;
			WriteLogLine( str.GetBuffer());
		}
		
		mutexAvatars.lock();
		MapVoipUsers::iterator it = mapVoipUsers.find( user.nUserID);
		if( it == mapVoipUsers.end())
		{
			UserVolume userVolume;
			userVolume.name = user.szNickName;
			std::transform( userVolume.name.begin(), userVolume.name.end(), userVolume.name.begin(), ::towlower);
			int evoUserID = GetUserIDByName( userVolume.name.c_str());
			userVolume.volume = SOUND_VOLUME_MAX;
			if( evoUserID != -1)
			{
				userVolume.volume = GetUserVolume( evoUserID);
				if( context->mpLogWriter)
				{		
					CComString str;
					USES_CONVERSION;
					str.Format("[VOIPSERVER]: OnAddUser reCreate user==%d and set prev volume==%d", user.nUserID, userVolume.volume);
					WriteLogLine( str.GetBuffer());
				}
				it = mapVoipUsers.find( evoUserID);
				mapVoipUsers.erase(it);
			}
			mapVoipUsers.insert( MapVoipUsers::value_type( user.nUserID, userVolume));
			WriteLogLine( "[VOIPSERVER] SetUserVolume to new user");
#ifdef USE_CONAITO_SDK_LIB
			EVO_WRAPPER_SetUserVolume( clientInstance, user.nUserID, userVolume.volume);
#endif
#ifdef USE_MUMBLE_SDK
			MUMBLE_SetUserVolume( clientInstance, user.nUserID, userVolume.volume);
#endif
		}
		mutexAvatars.unlock();
	}
	else
	{
		if( context->mpLogWriter)
		{		
			CComString str;
			str.Format("[VOIPSERVER]: OnAddUser b==%d", b);
			WriteLogLine( str.GetBuffer());
		}
	}

	return 0;
}

//! Вызывается при обновлении данных по конкретному пользователю
LRESULT CVoipManagerConaitoServer::OnUpdateUser(WPARAM wParam)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnUpdateUser");
		WriteLogLine( str.GetBuffer());
	}
	return 0;
}

//! Вызывается при удалении пользователя из канала
LRESULT CVoipManagerConaitoServer::OnRemoveUser(WPARAM wParam)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnRemoveUser");
		WriteLogLine( str.GetBuffer());
	}
	ATLTRACE("OnRemoveUser \n");
	//m_voipManagerPipe->SendRemoveUser( (unsigned int)wParam);

	User user = {0};
	if ( GetEvoUserImpl( (INT32)wParam, &user))
	{
		mutexAvatars.lock();
		MapVoipUsers::iterator it = mapVoipUsers.find( user.nUserID);
		if( it != mapVoipUsers.end())
		{		
			mapVoipUsers.erase(it);
		}
		mutexAvatars.unlock();
	}
	return 0;
}

//! Вызывается при появлении на сервере нового канала
LRESULT CVoipManagerConaitoServer::OnAddChannel(WPARAM wParam)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnAddChannel");
		WriteLogLine( str.GetBuffer());
	}
	return 0;
}

//! Вызывается при изменении данных канала
LRESULT CVoipManagerConaitoServer::OnUpdateChannel(WPARAM wParam)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnUpdateChannel");
		WriteLogLine( str.GetBuffer());
	}
	return 0;
}

//! Вызывается при удалении канала на сервере
LRESULT CVoipManagerConaitoServer::OnRemoveChannel(WPARAM wParam)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnRemoveChannel");
		WriteLogLine( str.GetBuffer());
	}
	return 0;
}

LRESULT CVoipManagerConaitoServer::OnLogMumbleErrorCode(WPARAM wParam, LPARAM lParam)
{
	if( context->mpLogWriter && wParam != MIC_NO_ERROR)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnLogMumbleErrorCode code(see MumbleErrorCodes.h) = %u hr = 0x%lx", (int)wParam, (long)lParam);
		WriteLogLine( str.GetBuffer());
	}

	const int MICROFON_SUCCESS_NOW = 0;
	const int MICROFON_BUSY_NOW = 1;
	const int OUTPUT_BUSY_NOW = 2;
	const int MICROFON_NOT_FOUND = 3;
	const int INVALID_DEFAUILT_DEVICE_IN_VOICE_SYSTEM = 4;
	const int SOME_PROBLEM_IN_VOICE_SYSTEM = 5;

	int currentErrorCode = SOME_PROBLEM_IN_VOICE_SYSTEM;
	if (lParam == AUDCLNT_E_DEVICE_IN_USE && wParam == WASAPI_IN_MIC_INITIALIZE_FAILED || wParam == WASAPI_IN_GET_NEXT_PACKET_FAILED
		|| wParam == DIRECTSOUND_IN_LOST_DEVICE) 
	{
		if( (wParam == WASAPI_IN_GET_NEXT_PACKET_FAILED || wParam == DIRECTSOUND_IN_LOST_DEVICE) && inputDevicesInfo.size() > 2)
			return 0;
		currentErrorCode = MICROFON_BUSY_NOW;
	}

	else if (lParam == AUDCLNT_E_DEVICE_IN_USE && wParam == WASAPI_OUT_MIC_INITIALIZE_FAILED || wParam == WASAPI_OUT_GET_GETBUFFER_FAILED
		|| wParam == WASAPI_OUT_GET_GETCURRENTPADDING_FAILED
		|| wParam == DIRECTSOUND_OUT_LOST_DEVICE)
	{
		if( (wParam == WASAPI_OUT_GET_GETBUFFER_FAILED
			|| wParam == WASAPI_OUT_GET_GETCURRENTPADDING_FAILED
			|| wParam == DIRECTSOUND_OUT_LOST_DEVICE) && outputDevicesInfo.size() > 2)
			return 0;
		currentErrorCode = OUTPUT_BUSY_NOW;
	}
	else if (wParam == WASAPI_IN_INSTATIATE_ENUMERATOR_FAILED || wParam == WASAPI_IN_OPEN_DEVICE_FAILED 
		|| wParam == WASAPI_IN_REOPEN_DEFAULT_DEVICE_FAILED || wParam == DIRECTSOUND_IN_CAPTURE_CREATE_FAILED)
		currentErrorCode = MICROFON_NOT_FOUND;
	else if (wParam == DIRECTSOUND_OUT_CREATE_FAILED || wParam == WASAPI_OUT_INSTATIATE_ENUMERATOR_FAILED 
		|| wParam == WASAPI_OUT_OPEN_DEVICE_FAILED || wParam == WASAPI_OUT_REOPEN_DEFAULT_DEVICE_FAILED)
		return 0;
	else if (wParam == MIC_NO_ERROR)
		currentErrorCode = MICROFON_SUCCESS_NOW;
	else if ( IS_DSOUND_ERROR_CODE(wParam) && (lParam == DSERR_INVALIDCALL || lParam == DSERR_GENERIC))
		currentErrorCode = INVALID_DEFAUILT_DEVICE_IN_VOICE_SYSTEM;


	m_voipManagerPipe->SendDeviceErrorCode(currentErrorCode);

	return 0;
}

//! Вызывается при недопустимом имени канала или при попытке создания уже существующего канала
LRESULT CVoipManagerConaitoServer::OnInvalideChannelName(WPARAM wParam)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnInvalideChannelName");
		WriteLogLine( str.GetBuffer());
	}
#ifdef USE_MUMBLE_SDK
	//пытаемся присоединиться еще раз тк считаем что недостимых имен канала нет - а значит нам просто не пришел еще список каналов
	USES_CONVERSION;
	MUMBLE_DoJoinChannel( clientInstance, A2W(roomName.GetBuffer()), L"", A2W(roomName.GetBuffer()), L"");
#endif
	return 0;
}

//! Вызывается при удалении подключении к каналу на сервере
LRESULT CVoipManagerConaitoServer::OnChannelJoined(WPARAM wParam)
{
//	wParamIDChannel = wParam;
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnChannelJoined");
		WriteLogLine( str.GetBuffer());
	}
	channelConnectionStatus = CS_CONNECTED;
	TTCHAR szChannelPath[EVO_STRLEN];
	memset(szChannelPath, 0, EVO_STRLEN*sizeof(TTCHAR));
#ifdef USE_CONAITO_SDK_LIB
	if( !EVO_GetChannelPath( clientInstance, wParam, szChannelPath))
#endif
#ifdef USE_MUMBLE_SDK
	if( !MUMBLE_GetChannelPath( clientInstance, (INT32)wParam, szChannelPath))
#endif
		szChannelPath[0] = 0;
	
	m_voipManagerPipe->SendChannelJoined( szChannelPath);
	channelPath = szChannelPath;
	if( context->mpLogWriter)
	{	
		USES_CONVERSION;
		CComString str;
		str.Format("[VOIPSERVER]: OnChannelJoined szChannelPath = %s", channelPath.GetBuffer());
		WriteLogLine( str.GetBuffer());
	}
	return 0;
} 

//! Вызывается при удалении отключении от канала на сервере
LRESULT CVoipManagerConaitoServer::OnChannelLeft(WPARAM wParam)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnChannelLeft");
		WriteLogLine( str.GetBuffer());
	}
	ATLTRACE("OnChannelLeft\n");
	TTCHAR szChannelPath[EVO_STRLEN];
	memset(szChannelPath, 0, EVO_STRLEN*sizeof(TTCHAR));
#ifdef USE_CONAITO_SDK_LIB
	if( !EVO_GetChannelPath( clientInstance, wParam, szChannelPath))
#endif
#ifdef USE_MUMBLE_SDK
	if( !MUMBLE_GetChannelPath( clientInstance, (INT32)wParam, szChannelPath))
#endif
		szChannelPath[0] = 0;

	if( context->mpLogWriter)
	{
		USES_CONVERSION;
		CComString str;
		std::string sChannelPath = W2A(szChannelPath);
		str.Format("[VOIPSERVER]: OnChannelLeft %s", sChannelPath.c_str());
		WriteLogLine( str.GetBuffer());
	}

	m_voipManagerPipe->SendChannelLeft();
	channelConnectionStatus = CS_DISCONNECTED;
	return 0;
}


LRESULT CVoipManagerConaitoServer::OnAddFile(LPARAM lParam)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnAddFile");
		WriteLogLine( str.GetBuffer());
	}
	return 0;
}

LRESULT CVoipManagerConaitoServer::OnRemoveFile(LPARAM lParam)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnRemoveFile");
		WriteLogLine( str.GetBuffer());
	}
	return 0;
}

LRESULT CVoipManagerConaitoServer::OnFileTransferBegin(WPARAM wParam)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnFileTransferBegin");
		WriteLogLine( str.GetBuffer());
	}
	return 0;
}

LRESULT CVoipManagerConaitoServer::OnFileTransferCompleted(WPARAM wParam)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnFileTransferCompleted");
		WriteLogLine( str.GetBuffer());
	}
	return 0;
}

LRESULT CVoipManagerConaitoServer::OnFileTransferFailed(LPARAM lParam)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnFileTransferFailed");
		WriteLogLine( str.GetBuffer());
	}
	return 0;
}

LRESULT CVoipManagerConaitoServer::OnUserMessage(WPARAM wParam, LPARAM lParam)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnUserMessage");
		WriteLogLine( str.GetBuffer());
	}
	return 0;
}

LRESULT CVoipManagerConaitoServer::OnChannelMessage(WPARAM wParam, LPARAM lParam)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnChannelMessage");
		WriteLogLine( str.GetBuffer());
	}
	return 0;
}

LRESULT CVoipManagerConaitoServer::OnUserTalking(WPARAM wParam)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnUserTalking");
		WriteLogLine( str.GetBuffer());
	}
	User user = {0};
	BOOL b = GetEvoUserImpl( (INT32)wParam, &user);
	//ATLASSERT(b);
	if(b)
	{
		std::wstring s = user.szNickName;
		std::transform( s.begin(), s.end(), s.begin(), ::towlower);
		m_voipManagerPipe->SendUserTalking( (unsigned int)wParam, (wchar_t*)s.c_str());

		CComString sLog;
		sLog.Format("[VOIP_SERVER] UserStartedTalking. UserName is %s and id = %u", s.c_str(), (int)wParam);
		WriteLogLine( sLog.GetBuffer());
#ifdef USE_CONAITO_SDK_LIB
		if( EVO_WRAPPER_IsVoiceActivated( clientInstance))
#endif
#ifdef USE_MUMBLE_SDK
		if( MUMBLE_IsVoiceActivated( clientInstance))
#endif
		{
			WriteLogLine("[VOIP_SERVER_ACT_LEVEL] Voice activation is enabled 1");
		}
		else
		{
			WriteLogLine("[VOIP_SERVER_ACT_LEVEL] Voice activation is disabled 1");
		}	
	}
	return 0;
}

LRESULT CVoipManagerConaitoServer::OnUserStoppedTalking(WPARAM wParam)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnUserStoppedTalking");
		WriteLogLine( str.GetBuffer());
	}
	User user = {0};
	BOOL b = GetEvoUserImpl( (INT32)wParam, &user);
	
	if(b)
	{
		std::wstring s = user.szNickName;
		std::transform( s.begin(), s.end(), s.begin(), ::towlower);
		m_voipManagerPipe->SendUserStoppedTalking( (unsigned int)wParam, (wchar_t*)s.c_str());

		CComString sLog;
		sLog.Format("[VOIP_SERVER] UserStoppedTalking. UserName is %s", s.c_str());
		WriteLogLine( sLog.GetBuffer());
	}
	return 0;
}

BOOL CVoipManagerConaitoServer::GetEvoUserImpl( INT32 nUserID, User* lpUser)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetEvoUserImpl");
		WriteLogLine( str.GetBuffer());
	}
	BOOL b = FALSE;
		if( clientInstance)
#ifdef USE_CONAITO_SDK_LIB
		b = EVO_WRAPPER_GetUser(clientInstance, nUserID, lpUser);
#endif
#ifdef USE_MUMBLE_SDK
		b = MUMBLE_GetUser(clientInstance, nUserID, lpUser);
#endif
	return b;
}

int CVoipManagerConaitoServer::GetInputDeviceID( EVO_WRAPPER_SoundSystemType soundSystem, bool abDefault)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetInputDeviceID");
		WriteLogLine( str.GetBuffer());
	}
	if( !clientInstance)
	{
		if( context->mpLogWriter)
			WriteLogLine( "[VOIPSERVER]: GetInputDeviceID::clientInstance == null");
		return -1;
	}
	if( inputDevicesInfo.empty())
	{
		if( context->mpLogWriter)
			WriteLogLine( "[VOIPSERVER]: GetInputDeviceID::inputevicesInfo.size() == 0");
		return -1;
	}	

	vecDeviceInfoIt it = inputDevicesInfo.begin(), end = inputDevicesInfo.end();
	

	CWTMicContoller micController;
	std::wstring currentInDeviceGuid = micController.GetCurrentInDeviceGuid(true);
	//std::wstring currentInDeviceGuid = currentEvoVoipDeviceGuid;
#ifdef USE_MUMBLE_SDK	
	if (m_mainSoundSystem != SOUNDSYSTEM_WASAPI)
	{
		if( abDefault) it++;
	}
#endif	
	for (; it!=end; ++it)
	{
		if( abDefault || currentInDeviceGuid.empty())
			return it->nDeviceID;
		if( _wcsnicmp( currentInDeviceGuid.c_str(), it->sGUID, min(wcslen(currentInDeviceGuid.c_str()), wcslen(it->sGUID))) == 0)
			return it->nDeviceID;
	}
	return -1;
}

// Возвращает имя устройства по его EVO_VOIP номеру. В случае, если такого устройства нет в системе - возвращает ""
std::wstring CVoipManagerConaitoServer::GetDeviceGuidByEvoVoidID( int aiDeviceID)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetDeviceGuidByEvoVoidID");
		WriteLogLine( str.GetBuffer());
	}
	if( !clientInstance)
		return L"";

	vecDeviceInfoIt it = inputDevicesInfo.begin(), end = inputDevicesInfo.end();
	for (; it!=end; ++it)
	{
		if( aiDeviceID == it->nDeviceID)
			return it->sGUID;
	}

	return L"";
}

int CVoipManagerConaitoServer::GetEvoVoipDeviceIDByGuid( const wchar_t* alpcDeviceGuid)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetEvoVoipDeviceIDByGuid");
		WriteLogLine( str.GetBuffer());
	}
	if( !clientInstance)
		return -1;

	if( alpcDeviceGuid == NULL || wcslen(alpcDeviceGuid) == NULL)
		return -1;

	vecDeviceInfoIt it = inputDevicesInfo.begin(), end = inputDevicesInfo.end();
	for (; it!=end; ++it)
	{
		if( wcslen(it->sGUID) > 0 && _wcsnicmp( alpcDeviceGuid, it->sGUID, min(wcslen(alpcDeviceGuid), wcslen(it->sGUID))) == 0)
			return it->nDeviceID;
	}

	return -1;
}

int CVoipManagerConaitoServer::GetEvoVoipOutDeviceIDByGuid( const wchar_t* alpcDeviceGuid)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetEvoVoipOutDeviceIDByGuid");
		WriteLogLine( str.GetBuffer());
	}
	if( !clientInstance)
		return -1;

	if( alpcDeviceGuid == NULL || wcslen(alpcDeviceGuid) == NULL)
		return -1;

	vecDeviceInfoIt it = outputDevicesInfo.begin(), end = outputDevicesInfo.end();
	for (; it!=end; ++it)
	{
		if( wcslen(it->sGUID) > 0 && _wcsnicmp( alpcDeviceGuid, it->sGUID, min(wcslen(alpcDeviceGuid), wcslen(it->sGUID))) == 0)
			return it->nDeviceID;
	}

	return -1;
}

int CVoipManagerConaitoServer::GetOutputDeviceID( EVO_WRAPPER_SoundSystemType soundSystem, bool abDefault)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetOutputDeviceID");
		WriteLogLine( str.GetBuffer());
	}
	if( !clientInstance)
	{
		if( context->mpLogWriter)
			WriteLogLine( "[VOIPSERVER]: GetOutputDeviceID::clientInstance == null");
		return -1;
	}
	if( outputDevicesInfo.empty())
	{
		if( context->mpLogWriter)
			WriteLogLine( "[VOIPSERVER]: GetOutputDeviceID::outputDevicesInfo.size() == 0");
		return -1;
	}

	CWTMicContoller micController;
	std::wstring currentOutDeviceGuid = micController.GetCurrentOutDeviceGuid( true);
	//std::wstring currentOutDeviceGuid = currentEvoVoipOutDeviceGuid;
	if( context->mpLogWriter)
	{
		USES_CONVERSION;
		CComString str;
		std::string sDevice = W2A(currentOutDeviceGuid.c_str());
		str.Format("[VOIPSERVER]: GetOutputDeviceID: currentOutDeviceGuid = %s", sDevice.c_str());
		WriteLogLine( str.GetBuffer());
	}
	vecDeviceInfoIt it = outputDevicesInfo.begin(), end = outputDevicesInfo.end();
#ifdef USE_MUMBLE_SDK	
	if (m_mainSoundSystem != SOUNDSYSTEM_WASAPI)
	{
		if( abDefault) it++;
	}
#endif	
	for (; it!=end; ++it)
	{
		if( abDefault || currentOutDeviceGuid.empty())
			return it->nDeviceID;

		if( _wcsnicmp( currentOutDeviceGuid.c_str(), it->sGUID, min(wcslen(currentOutDeviceGuid.c_str()), wcslen(it->sGUID))) == 0)
			return it->nDeviceID;
	}

	return -1;
}

int CVoipManagerConaitoServer::GetCurrentInputDeviceID( EVO_WRAPPER_SoundSystemType soundSystem)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetCurrentInputDeviceID");
		WriteLogLine( str.GetBuffer());
	}
	return GetInputDeviceID( soundSystem, false);
}

int CVoipManagerConaitoServer::GetCurrentOutputDeviceID( EVO_WRAPPER_SoundSystemType soundSystem)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetCurrentOutputDeviceID");
		WriteLogLine( str.GetBuffer());
	}
	return GetOutputDeviceID( soundSystem, false);
}

int CVoipManagerConaitoServer::GetDefaultInputDeviceID( EVO_WRAPPER_SoundSystemType soundSystem)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetDefaultInputDeviceID");
		WriteLogLine( str.GetBuffer());
	}
	return GetInputDeviceID( soundSystem, true);
}

int CVoipManagerConaitoServer::GetDefaultOutputDeviceID( EVO_WRAPPER_SoundSystemType soundSystem)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetDefaultOutputDeviceID");
		WriteLogLine( str.GetBuffer());
	}
	return GetOutputDeviceID( soundSystem, true);
}

void CVoipManagerConaitoServer::HandleOnConnectRequest()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: HandleOnConnectRequest");
		WriteLogLine( str.GetBuffer());
	}
	/*ConnectToVoipServer( connectRequestParams.voipServerAddress
		, requestedRoomName
		, requestedUserName
		, requestedUserPassword
		, requestedAvatarID
		, connectRequestParams.recordServerAddress
		, connectRequestParams.recordPort);*/
	// пайпы
}

bool CVoipManagerConaitoServer::GrabInputDevicesInfo(EVO_WRAPPER_SoundSystemType soundSystem, bool abForce)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GrabInputDevicesInfo");
		WriteLogLine( str.GetBuffer());
	}
	if( !abForce && inputDevicesInfo.size() != 0)
	{
		WriteLogLine( "[VOIPSERVER]: GrabInputDevicesInfo::abForce = false  inputDevicesInfo.size() != 0");
		return true;
	}

	inputDevicesInfo.clear();
	// Input devices
#ifdef USE_CONAITO_SDK_LIB
	int nCount = EVO_WRAPPER_GetInputDevicesCount(clientInstance, soundSystem);
#endif
#ifdef USE_MUMBLE_SDK
	int nCount = MUMBLE_GetInputDevicesCount(clientInstance, soundSystem);
#endif

	if( context->mpLogWriter)
	{
		USES_CONVERSION;
		CComString str;
		str.Format("[VOIPSERVER]: GetInputDevicesCount = %d", nCount);
		WriteLogLine( str.GetBuffer());
	}

	if( nCount != 0)
	{
		inputDevicesInfo.reserve( nCount);
		SoundDevice	deviceInfo;
		for( int i=0; i<nCount; i++)
		{
#ifdef USE_CONAITO_SDK_LIB
			BOOL b = EVO_WRAPPER_GetInputDevice(clientInstance, soundSystem, i, &deviceInfo);
#endif
#ifdef USE_MUMBLE_SDK
			BOOL b = MUMBLE_GetInputDevice(clientInstance, soundSystem, i, &deviceInfo);
#endif
			if( b)
			{
#ifdef USE_CONAITO_SDK_LIB
				ConvertEVOToWin32Encoding( deviceInfo.szDeviceName);
#endif
				inputDevicesInfo.push_back( deviceInfo);
				if( context->mpLogWriter)
				{
					USES_CONVERSION;
					CComString str;
					std::string sDevice = W2A(deviceInfo.szDeviceName);
					std::string sGUID = W2A(deviceInfo.sGUID);
					str.Format("[VOIPSERVER]: input device %d is %s %s", i,sDevice.c_str(), sGUID.c_str());
					WriteLogLine( str.GetBuffer());
				}
			}
			else
			{
				if( context->mpLogWriter)
				{
					USES_CONVERSION;
					CComString str;
					str.Format("[VOIPSERVER]: EVO_WRAPPER_GetInputDevice failed");
					WriteLogLine( str.GetBuffer());
				}
			}
		}
	}
	return true;
}

bool CVoipManagerConaitoServer::GrabInputDevicesInfoForce( bool abForce)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GrabInputDevicesInfoForce");
		WriteLogLine( str.GetBuffer());
	}
#ifdef USE_CONAITO_SDK_LIB
	EVO_WRAPPER_SoundSystemType soundSystem = settings.GetUseDS() ? SOUNDSYSTEM_DSOUND:SOUNDSYSTEM_WINMM;
#endif
#ifdef USE_MUMBLE_SDK
	EVO_WRAPPER_SoundSystemType soundSystem = m_mainSoundSystem;
#endif
	return GrabInputDevicesInfo( soundSystem, abForce);
}

bool CVoipManagerConaitoServer::GrabOutputDevicesInfo(EVO_WRAPPER_SoundSystemType soundSystem, bool abForce)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GrabOutputDevicesInfoForce");
		WriteLogLine( str.GetBuffer());
	}
	if( !abForce && outputDevicesInfo.size() != 0) 
	{
		WriteLogLine( "[VOIPSERVER]: GrabOutputDevicesInfo::abForce = false  outputDevicesInfo.size() != 0");
		return true;
	}

	outputDevicesInfo.clear();
	// Output devices
	int nCount;
#ifdef USE_CONAITO_SDK_LIB
	nCount = EVO_WRAPPER_GetOutputDevicesCount(clientInstance, soundSystem);
#endif
#ifdef USE_MUMBLE_SDK
	nCount = MUMBLE_GetOutputDevicesCount(clientInstance, soundSystem);
#endif
	if( context->mpLogWriter)
	{
		USES_CONVERSION;
		CComString str;
		str.Format("[VOIPSERVER]: GetOutputDevicesCount = %d", nCount);
		WriteLogLine( str.GetBuffer());
	}

	if( nCount != 0)
	{
		outputDevicesInfo.reserve( nCount);
		SoundDevice	deviceInfo;
		for( int i=0; i<nCount; i++)
		{
#ifdef USE_CONAITO_SDK_LIB
			BOOL b = EVO_WRAPPER_GetOutputDevice(clientInstance, soundSystem, i, &deviceInfo);
#endif
#ifdef USE_MUMBLE_SDK
			BOOL b = MUMBLE_GetOutputDevice(clientInstance, soundSystem, i, &deviceInfo);
#endif
			if( b)
			{
#ifdef USE_CONAITO_SDK_LIB
				ConvertEVOToWin32Encoding( deviceInfo.szDeviceName);
#endif
				outputDevicesInfo.push_back( deviceInfo);

				if( context->mpLogWriter)
				{
					USES_CONVERSION;
					CComString str;
					std::string sDevice = W2A(deviceInfo.szDeviceName);
					std::string sGUID = W2A(deviceInfo.sGUID);
					str.Format("[VOIPSERVER]: output device %d is %s %s", i,sDevice.c_str(), sGUID.c_str());
					WriteLogLine( str.GetBuffer());
				}
			}
			else
			{
				if( context->mpLogWriter)
				{
					USES_CONVERSION;
					CComString str;
					str.Format("[VOIPSERVER]: EVO_WRAPPER_GetOutputDevice failed");
					WriteLogLine( str.GetBuffer());
				}
			}
		}
	}
	return true;
}


bool CVoipManagerConaitoServer::GrabOutputDevicesInfoForce( bool abForce)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GrabOutputDevicesInfoForce");
		WriteLogLine( str.GetBuffer());
	}
	EVO_WRAPPER_SoundSystemType soundSystem = settings.GetUseDS() ? SOUNDSYSTEM_DSOUND:SOUNDSYSTEM_WINMM;
	return GrabOutputDevicesInfo( soundSystem, abForce);
}

bool CVoipManagerConaitoServer::GrabDevicesInfoImpl(EVO_WRAPPER_SoundSystemType soundSystem, bool abForce)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GrabDevicesInfo");
		WriteLogLine( str.GetBuffer());
	}
	ATLASSERT( clientInstance);
	if( !clientInstance)
		return false;

	bool bRes = GrabInputDevicesInfo( soundSystem, abForce);
	if( bRes)
		bRes = GrabOutputDevicesInfo( soundSystem, abForce);			
	
	if(!bRes && !abForce)
	{
		DestroyVoipSystem();
		DestroyEvoSystem();
		return false;
	}	
	if( context->mpLogWriter)
	{
		USES_CONVERSION;
		CComString str;
		str.Format("[VOIPSERVER]: GrabDevicesInfo success");
		WriteLogLine( str.GetBuffer());
	}
	return true;
}

bool CVoipManagerConaitoServer::GrabDevicesInfo( bool abForce)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GrabDevicesInfo");
		WriteLogLine( str.GetBuffer());
	}
#ifdef USE_CONAITO_SDK_LIB
	EVO_WRAPPER_SoundSystemType soundSystem = settings.GetUseDS() ? SOUNDSYSTEM_DSOUND:SOUNDSYSTEM_WINMM;
#endif
#ifdef USE_MUMBLE_SDK
	EVO_WRAPPER_SoundSystemType soundSystem = m_mainSoundSystem;
#endif
	return GrabDevicesInfoImpl( soundSystem, abForce);
}

//! Обработка изменения автоматической регулировки громкости
void CVoipManagerConaitoServer::OnAutoVolumeChanged()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: OnAutoVolumeChanged");
		WriteLogLine( str.GetBuffer());
	}
	/*
	Если установлен режим ручной настройки громкости, 
	Ничего не делаем
	иначе
	Если это голос, то устанавливаем
	- максимальную громкость микрофона в устройствах
	- чувствительность выставляем на 80%
	- отсечение устанавливаем в 5%
	иначе (если это микшер)
	- громкость микрофона в устройствах в 1%
	- чувствительность выставляем на 0% - без усиления
	- отсечение устанавливаем в 0%		
	*/


	CWTMicContoller microphoneCtrl;

	if( GetAudioSource() == VAS_MICROPHONE)
	{		
	}
	else
	{
		float val = GET_RANGED_VAL(1000, 8000, 0);

#ifdef USE_CONAITO_SDK_LIB
		EVO_WRAPPER_SetVoiceGainLevel( clientInstance, val);
#endif
#ifdef USE_MUMBLE_SDK
		//MUMBLE_SetVoiceGainLevel( clientInstance, (INT32)val);
#endif

	}
}

bool CVoipManagerConaitoServer::SetUserVolume( const wchar_t* alwpcUserLogin, double adUserVolume)
{
	long volume = (long)(SOUND_VOLUME_MAX*adUserVolume);
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: SetUserVolume volume=%d ", volume);
		WriteLogLine( str.GetBuffer());
	}
	if( !clientInstance)
		return false;

	if( !alwpcUserLogin)
		return false;

	mutexAvatars.lock();
	int evoUserID = GetUserIDByName( alwpcUserLogin);
	if( evoUserID < 0)
	{
		if( evoUserID == -1)
		{
			UserVolume userVolume;
			userVolume.name = alwpcUserLogin;
			std::transform( userVolume.name.begin(), userVolume.name.end(), userVolume.name.begin(), ::towlower);
			userVolume.volume = volume;
			tempID--;
			mapVoipUsers.insert( MapVoipUsers::value_type( tempID, userVolume));
		}
		else
			SaveUserVolume( evoUserID, volume );
		mutexAvatars.unlock();
		CWComString s = L"[VOIPSERVER] ";
		s += alwpcUserLogin;
		s += L" is not found into voice register avatars";
		WriteLogLine( s.GetBuffer());
		return false;
	}	
	bool bChangeVolume = SaveUserVolume( evoUserID, volume );	
	if( context->mpLogWriter)
	{			
		CWComString str;
		str.Format(L"[VOIPSERVER]: SetUserVolume userLogin=%s, evoUserID=%d, volume=%d, bChangeVolume=%d", alwpcUserLogin, evoUserID, volume, (int)bChangeVolume);
		WriteLogLine( str.GetBuffer());
	}
#ifdef USE_CONAITO_SDK_LIB
	EVO_WRAPPER_SetUserVolume( clientInstance, evoUserID, volume );
#endif
#ifdef USE_MUMBLE_SDK
	MUMBLE_SetUserVolume( clientInstance, evoUserID, volume );
#endif
	mutexAvatars.unlock();
	return true;
}

void CVoipManagerConaitoServer::SetMasterVolume(float aVolume)
{
	MUMBLE_SetMasterVolume(clientInstance, (INT32)aVolume);
}

void CVoipManagerConaitoServer::SetEchoCancellation(bool anEnabled)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		if (anEnabled)
			str.Format("[VOIPSERVER]: Enable Mic EchoCancellation");
		else
			str.Format("[VOIPSERVER]: Disable Mic EchoCancellation");
		WriteLogLine( str.GetBuffer());
	}
	MUMBLE_SetEchoCancellation(clientInstance, anEnabled);
}

void CVoipManagerConaitoServer::SetMixerEchoCancellation(bool anEnabled)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		if (anEnabled)
			str.Format("[VOIPSERVER]: Enable MixerEchoCancellation");
		else
			str.Format("[VOIPSERVER]: Disable MixerEchoCancellation");
		WriteLogLine( str.GetBuffer());
	}
	MUMBLE_SetMixerEchoCancellation(clientInstance, anEnabled, m_mainSoundSystem);
}

void CVoipManagerConaitoServer::AppStartFindMicDevice() 
{
	MUMBLE_StartFindRealMic(clientInstance, m_mainSoundSystem);
}

void CVoipManagerConaitoServer::AppEndFindMicDevice() 
{
	wchar_t sFindedDevice[MUMBLE_STRLEN] = {0};
	wchar_t sFindedDeviceName[MUMBLE_STRLEN] = {0};
	wchar_t sFindedDeviceLine[MUMBLE_STRLEN] = {0};
	MUMBLE_EndFindRealMic(clientInstance, sFindedDevice, sFindedDeviceName, sFindedDeviceLine);

	if (m_voipManagerPipe)
		m_voipManagerPipe->SendAutoSelectedMic( sFindedDevice, sFindedDeviceName, sFindedDeviceLine);
}

bool CVoipManagerConaitoServer::SetUserGainLevel(  const wchar_t* alwpcUserLogin, double adUserGainLevel)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: SetUserGainLevel");
		WriteLogLine( str.GetBuffer());
	}
	if( !clientInstance)
		return false;

	mutexAvatars.lock();
	int evoUserID = GetUserIDByName( alwpcUserLogin);
	mutexAvatars.unlock();

	//EVO_SetUserGainLevel( clientInstance, evoUserID, adUserGainLevel*1000);
#ifdef USE_CONAITO_SDK_LIB
	EVO_SetUserGainLevel( clientInstance, evoUserID, adUserGainLevel);
#endif
#ifdef USE_MUMBLE_SDK
	MUMBLE_SetUserGainLevel( clientInstance, evoUserID, (INT32)adUserGainLevel);
#endif
	return true;
}

//! Автоматическое управление громкостью
bool CVoipManagerConaitoServer::IsAutoGaneControlEnabled()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: IsAutoGaneControlEnabled");
		WriteLogLine( str.GetBuffer());
	}
	if( !clientInstance)
		return false;

	return agcEnabled;
}
bool CVoipManagerConaitoServer::SetAutoGaneControlEnabled(bool abEnabled)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: SetAutoGaneControlEnabled");
		WriteLogLine( str.GetBuffer());
	}
	agcEnabled = abEnabled;

	if( !clientInstance)
		return false;
#ifdef USE_CONAITO_SDK_LIB
	if( !EVO_EnableAGC( clientInstance, abEnabled))
#endif
#ifdef USE_MUMBLE_SDK
	if( !MUMBLE_EnableAGC( clientInstance, abEnabled))
#endif
	{
		return false;
	}

	unsigned int uGainLevel = 2 * 8192;
	unsigned int uIncreaseVel = 12;
	int uDecreaseVel = -40;
	// убранно временно для теста по умолчанию
	//unsigned int uMaxVolume = 20;
	unsigned int uMaxVolume = 30;
	//
#ifdef USE_CONAITO_SDK_LIB
	return EVO_SetAGCSettings( clientInstance, uGainLevel, uIncreaseVel, uDecreaseVel, uMaxVolume);
#endif
#ifdef USE_MUMBLE_SDK
	return MUMBLE_SetAGCSettings( clientInstance, uGainLevel, uIncreaseVel, uDecreaseVel, uMaxVolume)!=0;
#endif
}
//! Автоматическое подавление шумов
bool CVoipManagerConaitoServer::IsDenoisingEnabled()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: IsDenoisingEnabled");
		WriteLogLine( str.GetBuffer());
	}
	if( !clientInstance)
		return false;
#ifdef USE_CONAITO_SDK_LIB
	return EVO_IsDenoising( clientInstance);
#endif
#ifdef USE_MUMBLE_SDK
	return MUMBLE_IsDenoising( clientInstance)!=0;
#endif
}

bool CVoipManagerConaitoServer::SetDenoisingEnabled(bool abEnabled)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: SetDenoisingEnabled");
		WriteLogLine( str.GetBuffer());
	}
	denoisingEnabled = abEnabled;

	if( !clientInstance)
		return false;
#ifdef USE_CONAITO_SDK_LIB
	return EVO_EnableDenoising( clientInstance, abEnabled);
#endif
#ifdef USE_MUMBLE_SDK
	return MUMBLE_EnableDenoising( clientInstance, abEnabled)!=0;
#endif
}

//! 
void CVoipManagerConaitoServer::HandleVoiceData( int nUserID, int nSampleRate, const short* pRawAudio, int nSamples)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: HandleVoiceData");
		WriteLogLine( str.GetBuffer());
	}
#ifdef USE_CONAITO_SDK_LIB
	if( EVO_WRAPPER_GetMyUserID(clientInstance) == nUserID)
#endif
#ifdef USE_MUMBLE_SDK
	if( MUMBLE_GetMyUserID(clientInstance) == nUserID)
#endif
	{
#ifdef USE_MUMBLE_SDK
		CurrentInputLevelCalculation method = VACADEM;
		m_currentInputLevel = MUMBLE_GetCurrentInputLevel(clientInstance, method);
		CVoipManagerEVO::HandleVoiceData( nUserID, nSampleRate, pRawAudio, nSamples);
		return;
#endif
		// Рассчитаем громкость звука - по максимальной амплитуде каждого набора сэмплов
		if( m_currentModeCurrentTopInputLevel > m_currentModeTopInputLevel)
			m_currentModeCurrentTopInputLevel -= (m_currentModeCurrentTopInputLevel - m_currentModeTopInputLevel) / 25;

		if( nSamples > 0 )
		{
			m_skipCurrentLevelCalculation = !m_skipCurrentLevelCalculation;
			if( !m_skipCurrentLevelCalculation)
			{
				short sampleValue = 0;
				long calculatedMedian = 0;
				long calculatedDisperse = 0;
				for( int i=0; i < nSamples; i++, pRawAudio++)
				{
					sampleValue = (*pRawAudio);
					calculatedMedian += sampleValue;

					if( sampleValue < 0)
						sampleValue = -sampleValue;

					if( calculatedDisperse < sampleValue)
						calculatedDisperse = sampleValue;
				}

				if( calculatedDisperse > m_currentModeCurrentTopInputLevel)
					m_currentModeCurrentTopInputLevel = calculatedDisperse;

				m_currentInputLevel = (m_currentInputLevel + ((double)calculatedDisperse - (double)calculatedMedian/nSamples) / m_currentModeCurrentTopInputLevel) / 2;
			}
		}
		else
		{
			m_currentInputLevel /= 2;
		}
	}

	CVoipManagerEVO::HandleVoiceData( nUserID, nSampleRate, pRawAudio, nSamples);
}

bool CVoipManagerConaitoServer::IsVoipSystemInitialized()
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: IsVoipSystemInitialized");
		WriteLogLine( str.GetBuffer());
	}
	if( !clientInstance)
		return false;
#ifdef USE_CONAITO_SDK_LIB
	return EVO_WRAPPER_IsSoundSystemInitialized( clientInstance);
#endif
#ifdef USE_MUMBLE_SDK
	return MUMBLE_IsSoundSystemInitialized( clientInstance)!=0;
#endif
}

void CVoipManagerConaitoServer::GetInputDevicesInfo( vecDeviceInfo **aInputDevicesInfo)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetInputDevicesInfo");
		WriteLogLine( str.GetBuffer());
	}
	*aInputDevicesInfo = &inputDevicesInfo;
}

void CVoipManagerConaitoServer::GetOutputDevicesInfo( vecDeviceInfo **aOutputDevicesInfo)
{
	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetOutputDevicesInfo");
		WriteLogLine( str.GetBuffer());
	}
	*aOutputDevicesInfo = &outputDevicesInfo;
}

//! Вызывается при успешном соединении с сервером
int CVoipManagerConaitoServer::GetConnectionStatus()
{	
/*	if( context->mpLogWriter)
	{		
		CComString str;
		str.Format("[VOIPSERVER]: GetConnectionStatus");
		WriteLogLine( str.GetBuffer());
	}*/
	return connectionStatus;
}


void CVoipManagerConaitoServer::WriteLogLine(const char* alpString)
{
	if( alpString && context->mpLogWriter)
	{
		context->mpLogWriter->WriteLnLPCSTR( alpString, ltDefault);
		if( m_voipManagerPipe != NULL)
			m_voipManagerPipe->SendLog( alpString);
	}
}

void CVoipManagerConaitoServer::WriteLogLine(const wchar_t* alpwString)
{
	if( alpwString)
	{
		USES_CONVERSION;		
		WriteLogLine( W2A( alpwString));
	}
}

void CVoipManagerConaitoServer::ResetDirectConnectFlags()
{
	m_directConnectFailed = false;
	m_directConnectNow = false;
	m_directConnectSuccess = false;
}

DWORD WINAPI ThreadVoipLog( void *lpvParam)
{
	((CVoipManagerConaitoServer*)lpvParam)->VoipLog();	
	return 0;
}

void CVoipManagerConaitoServer::StartVoipLog()
{
	WriteLogLine( "[VOIP_SERVER] StartVoipLog");
	m_eventVoipLog = CreateEvent( NULL, FALSE, FALSE, "eventVoipLog");
	CSimpleThreadStarter voipLogThreadStarter(__FUNCTION__);
	voipLogThreadStarter.SetRoutine(ThreadVoipLog);
	voipLogThreadStarter.SetParameter(this);
	m_hVoipLogThread = voipLogThreadStarter.Start();
	if (m_hVoipLogThread == NULL)
	{
		CComString sLog;
		sLog.Format( "[VOIP_SERVER] Critical error. CreateThread VoipLog failed. Error code is %u", GetLastError());
		WriteLogLine( sLog.GetBuffer());
	}
	else
		m_voipLogThreadId = voipLogThreadStarter.GetThreadID();
}

void CVoipManagerConaitoServer::VoipLog()
{
#ifdef USE_MUMBLE_SDK
	WriteLogLine( "[VOIP_SERVER]thread VoipLog");
	BOOL detectHang = false;
	int countWithHang = 0;
	int maxCntHangLimit = 5;
	
	int defaultSleepTime = 30000;
	int checkHangSleepTime = 2000;

	int sleepTime = defaultSleepTime;

	while( m_bVoipLog)
	{
		WaitForSingleObject( m_eventVoipLog, sleepTime);
		if( !clientInstance)
			continue;
		int myUserID = MUMBLE_GetMyUserID( clientInstance);
		wchar_t sLog[MUMBLE_LOG_STRLEN] = {0};
		int size = MUMBLE_GetUserChannelInfo( myUserID, sLog, MUMBLE_LOG_STRLEN, detectHang);
		if( wcslen(sLog) > 0)
		{
			WriteLogLine( sLog);	
		}

		if (detectHang)
		{
			sleepTime = checkHangSleepTime;
			countWithHang++;
			if (countWithHang > maxCntHangLimit) 
			{
				sleepTime = defaultSleepTime;
			}
		}
		else 
		{
			sleepTime = defaultSleepTime;
			countWithHang = 0;
		}
	}
#endif
}

DWORD WINAPI ThreadChannel( void *lpvParam)
{
	((CVoipManagerConaitoServer*)lpvParam)->ChannelThread();	
	return 0;
}

void CVoipManagerConaitoServer::StartChannelThread()
{
	WriteLogLine( "[VOIP_SERVER] StartChannelThread");
	m_eventChannel = CreateEvent( NULL, FALSE, FALSE, "eventChannel");
	CSimpleThreadStarter voipChannelThreadStarter(__FUNCTION__);
	voipChannelThreadStarter.SetRoutine(ThreadChannel);
	voipChannelThreadStarter.SetParameter(this);
	m_hChannelThread = voipChannelThreadStarter.Start();
	if (m_hChannelThread == NULL)
	{
		CComString sLog;
		sLog.Format( "[VOIP_SERVER] Critical error. CreateThread Channel failed. Error code is %u", GetLastError());
		WriteLogLine( sLog.GetBuffer());
	}
	else
		m_channelThreadId = voipChannelThreadStarter.GetThreadID();
}

void CVoipManagerConaitoServer::ChannelThread()
{
#ifdef USE_MUMBLE_SDK
	WriteLogLine( "[VOIPSERVER] thread SetChannel");
	while( m_bChannel)
	{
		WaitForSingleObject( m_eventChannel, 5000);
		if( !clientInstance)
			continue;
		if( connectionStatus != CS_CONNECTED)
			continue;
		if( roomName.IsEmpty())
			continue;
		int myUserID = MUMBLE_GetMyUserID( clientInstance);
		api::User user;
		if(!MUMBLE_GetUser( clientInstance, myUserID, &user))
			continue;
		/*if( wParamIDChannel != 0)
		{*/
		TTCHAR szChannelPath[EVO_STRLEN];
		memset(szChannelPath, 0, EVO_STRLEN*sizeof(TTCHAR));
		MUMBLE_GetChannelPath( clientInstance, (INT32)user.nChannelID, szChannelPath);
		USES_CONVERSION;
		std::string s = W2A(szChannelPath);
		if( !s.empty() && roomName == s.c_str())
			continue;
		//}
		WriteLogLine( "[VOIPSERVER] thread SetChannel IsAuthorized");
		if( MUMBLE_IsAuthorized( clientInstance))		
			OnAccepted();
		else
			DoLogin( userName.AllocSysString(), userPassword.AllocSysString(), roomName.AllocSysString());
		//JoinChannel( roomName);

	}
#endif
}
