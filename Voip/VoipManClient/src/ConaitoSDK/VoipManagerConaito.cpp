#include "StdAfx.h"

//#ifdef USE_CONAITO_SDK_LIB

#include "..\..\include\ConaitoSDK\VoipManagerConaito.h"
#include "..\..\include\RecordServer\AudioSlicer.h"
#include "..\AudioCapture\include\DefaultAudioDeviceManager.h"
#include "..\Mixer\DSMixer2.h"
#include "rm.h"
#include "ILogWriter.h"
//for log device drivers
#include <setupapi.h>
#include <devguid.h>
#include <regstr.h>
//end for log device drivers

#define AUDIO_SUBSYSTEM_XP		1
#define AUDIO_SUBSYSTEM_VISTA	2

class CInitDeviceInfo
{
public:
	int errorCode; 
	bool bInitVoipSystem;
	int iVasDevice;
	std::wstring sDeviceName;
	std::wstring sOutDeviceName;
	CInitDeviceInfo( int aErrorCode, bool abInitVoipSystem, int aiVasDevice, std::wstring aDeviceGuid, std::wstring aOutDeviceName)
	{
		errorCode = aErrorCode;
		bInitVoipSystem = abInitVoipSystem;
		iVasDevice = aiVasDevice;
		sDeviceName = aDeviceGuid;
		sOutDeviceName = aOutDeviceName;
	}
};

//////////////////////////////////////////////////////////////////////////

CVoipManagerConaito::CVoipManagerConaito(oms::omsContext* aContext) : 
		CVoipManager( aContext)
		, settings( aContext)
		, voipWindowClient( this)
		, microphoneEnabled( false)
		, agcEnabled( false)
		, denoisingEnabled( false)
		, outputDeviceWasChanged( true)
		,MP_WSTRING_INIT(currentEvoVoipDeviceGuid)
		,MP_WSTRING_INIT(currentDeviceLine)
		,MP_WSTRING_INIT(currentMixerGuid)
		,MP_WSTRING_INIT(currentEvoVoipLineName)
		,MP_WSTRING_INIT(currentEvoVoipMixLineName)
		,MP_WSTRING_INIT(currentEvoVoipOutDeviceGuid)
		,MP_WSTRING_INIT(currentEvoVoipOutDeviceLine)
		,MP_WSTRING_INIT(oldCurrentDeviceName)
		,MP_WSTRING_INIT(oldDeviceLine)
		,MP_WSTRING_INIT(oldMixerName)
		,MP_WSTRING_INIT(oldCurrentOutDeviceName)
		,MP_WSTRING_INIT(oldCurrentEvoVoipOutDeviceLine)
		,MP_VECTOR_INIT(inputDevicesInfo)
		,MP_VECTOR_INIT(outputDevicesInfo)
		,MP_WSTRING_INIT(currentMicrophoneDeviceGuid)
		,MP_WSTRING_INIT(currentMixDeviceGuid)
		
{
	//clientInstance = NULL;
	connectionStatus = CS_DISCONNECTED;
	channelConnectionStatus = CS_DISCONNECTED;
	audioCapDeviceID = 0;

	// Читаем настройки и инициализируем систему на них
	//InitializeEvoSystem();
	voipWindowClient.Create( (context && context->mpWindow) ? (HWND)context->mpWindow->GetMainWindowHandle() : 0, NULL, "voipMan client window", WS_POPUPWINDOW);
	//voipWindowClient.Create( NULL, NULL, "voipMan client window", WS_POPUPWINDOW);
	if( !voipWindowClient.IsWindow())
		ATLASSERT(false);
		//return ERROR_UNKNOWN;


	m_currentInputLevel = 0;
	m_currentModeTopInputLevel = MAX_STABLE_AUDIO_LEVEL_VOICE;
	m_currentModeCurrentTopInputLevel = m_currentModeTopInputLevel;
	m_skipCurrentLevelCalculation = true;
	m_iLogMode = settings.GetLogMode();
	//currentEvoVoipDeviceGuid = L"Default device";
	//currentMicrophoneDeviceGuid = L"Default device";
	//currentMixDeviceGuid = L"Default device";
	currentEvoVoipOutDeviceGuid = L"Default device";

	initializeCompleteEvent.Create( NULL, TRUE/*bManualReset*/, FALSE/*bInitialState*/, NULL/*pEventAttributes*/);
	
	m_userID = 0;
	bVoipSystemInitialized = false;
	bVoipSettingCorrect = false;
	MP_NEW_V2( m_voipManagerPipe, CVoipManagerPipe, context, this);

	m_voipManagerPipe->Run();

	LogSystemDrivers();
	isSubscribedToNotify = false;
	m_bCheckAudioDevices = true;
	m_eventCheckAudioDevices = INVALID_HANDLE_VALUE;
	m_pDefaultAudioDeviceManager = NULL;
}

CVoipManagerConaito::~CVoipManagerConaito(void)
{
	m_bCheckAudioDevices = false;
	SetEvent(m_eventCheckAudioDevices);
	//09.04.2014 лишь при закрытии приложения - иначе риск повисаний
	if (WaitForSingleObject( m_hCheckAudioDevicesThread, 2000) == WAIT_TIMEOUT)
		TerminateThread(m_hCheckAudioDevicesThread, 0);
	CloseHandle( m_eventCheckAudioDevices);
	if(  GetAudioSubsystemType() > AUDIO_SUBSYSTEM_XP)
	{
		if( m_pDefaultAudioDeviceManager != NULL)
			MP_DELETE( m_pDefaultAudioDeviceManager);
	}

	DestroyVoipSystem();
	DestroyEvoSystem();

	if (isSubscribedToNotify && context->mpComMan)
		context->mpComMan->GetConnectionChanger()->UnSubscribeFromNotify(this);

}

//! Подключиться к определенному серверу голосовой связи
voipError	CVoipManagerConaito::ConnectImpl( bool aCheckConnecting)
{
	// !!!!!! callback //
	/*if( !clientInstance)
		return ERROR_NOTINITIALIZED;*/
	if( serverIP.IsEmpty())
	{
		if( context->mpLogWriter != NULL)
		{
			CComString str;
			str.Format("[VOIP]: connecting to serverIP is empty");
			WriteLogLine( str.GetBuffer());
		}
		return ERROR_NOERROR;
	}
	std::vector<IProxySettings*> totalProxySettingsVec;
	changeProxyListMutex.lock();
	std::vector<IProxySettings*>::iterator it = socksProxySettingsVec.begin();
	std::vector<IProxySettings*>::iterator itEnd = socksProxySettingsVec.end();
	for (; it != itEnd; it++)
	{
		if (!wcscmp((*it)->getName(), L"no_proxy"))
			continue;
		totalProxySettingsVec.push_back((IProxySettings*)(*it));
	}
	it = httpsProxySettingsVec.begin();
	itEnd = httpsProxySettingsVec.end();
	for (; it != itEnd; it++)
	{
		if (!wcscmp((*it)->getName(), L"no_proxy"))
			continue;
		totalProxySettingsVec.push_back((IProxySettings*)(*it));
	}
	
	m_voipManagerPipe->SendConnectImpl( serverIP.GetBuffer(), &totalProxySettingsVec, voipServerPort, voipServerPort, aCheckConnecting);
	
	changeProxyListMutex.unlock();

	if( context->mpLogWriter != NULL)
	{
		USES_CONVERSION;
		CComString str;
		str.Format("[VOIP]: connecting to EVOIP serverIP = %s, tcpPort = %d, updPort = %d check connecting operation with current parameters : %d", serverIP.GetBuffer(), voipServerPort, voipServerPort, (int)aCheckConnecting);
		WriteLogLine( str.GetBuffer());
	}	

	return ERROR_NOERROR;
}

//! Подключиться к определенному серверу голосовой связи
voipError	CVoipManagerConaito::OnConnectImpl( unsigned int aErrorCode)
{
	// !!!!!! callback //
	/* if( !clientInstance)
	return ERROR_NOTINITIALIZED; */
	if( aErrorCode == -1 || aErrorCode == ERROR_NOTINITIALIZED)
	{
		if( context->mpLogWriter != NULL)
		{
			CComString str;
			str.Format("[VOIP]: OnConnectImpl::aErrorCode = %d", aErrorCode);
			WriteLogLine( str.GetBuffer());
		}		
		ATLASSERT(false);
		return aErrorCode;
	}

	if( aErrorCode != ERROR_NOERROR)
	{
		CALLBACK_ONE_PARAM(onConnectFailed, serverIP.GetBuffer());
		if( context->mpLogWriter != NULL)
		{
			CComString str;
			str.Format("[VOIP]: OnConnectImpl::onConnectFailed::aErrorCode = %d", aErrorCode);
			WriteLogLine( str.GetBuffer());
		}		
		return ERROR_CONNECTION_FAILED;
	}
	if (connectionStatus != CS_CONNECTED)
	{
		connectionStatus = CS_CONNECTING;
		channelConnectionStatus = CS_DISCONNECTED;

		// ! Connection established
		CALLBACK_NO_PARAMS(onConnectStart);
	}
	OnConnectToEVoipServer();
	if( context != NULL && context->mpLogWriter != NULL)
		context->mpLogWriter->WriteLnLPCSTR( "[VOIP] OnConnectImpl::onConnectStart");

	return ERROR_NOERROR;
}

voipError	CVoipManagerConaito::ChangeLocationImpl( LPCSTR alpcLocation)
{
	// закомментировано для того, чтобы проверку соединения делать перед самым обращением к EVO
	/*if( connectionStatus == CS_CONNECTED)
	{*/
		m_voipManagerPipe->SendDoLogin( userName.AllocSysString(), userPassword.AllocSysString(), roomName.AllocSysString());
		m_voipManagerPipe->SendJoinChannel( roomName);
		if( context->mpLogWriter != NULL)
		{
			CComString str;
			str.Format("[VOIP]: ChangeLocationImpl::roomName = %s", roomName.GetBuffer());
			WriteLogLine( str.GetBuffer());
		}		
	//}
	return ERROR_NOERROR;
}

//! Отключиться от текущего сервера голосовой связи
voipError	CVoipManagerConaito::DisconnectImpl()
{
	if( connectionStatus == CS_DISCONNECTED ||
		connectionStatus == CS_DISCONNECTING)
		return ERROR_NOERROR;

	// callback !!!!!!!!!!!!!!!!
	m_voipManagerPipe->SendDisconnect();
	///
	channelConnectionStatus = CS_DISCONNECTING;
	connectionStatus = CS_DISCONNECTED;
	CALLBACK_NO_PARAMS(onDisconnected);
	if( context->mpLogWriter != NULL)
		context->mpLogWriter->WriteLnLPCSTR( "[VOIP] DisconnectImpl");
	return ERROR_NOERROR;
}

//! Получение режима активации передачи звука
bool CVoipManagerConaito::GetVoiceActivated()
{
	/*if( !clientInstance)
		return false;*/

	return voiceActivated;
}

bool CVoipManagerConaito::GetMicrophoneEnabled()
{
	return microphoneEnabled;
}

//! Принудительное начало передачи данных
void CVoipManagerConaito::StartTransmitting()
{
	microphoneEnabled = true;
	/*if( clientInstance)
	{*/
	HandleTransmissionStateChanged();
	//}
}

//! Принудительное окончание передачи данных
void CVoipManagerConaito::StopTransmitting()
{
	microphoneEnabled = false;
	/*if( clientInstance)
	{*/
	HandleTransmissionStateChanged();
	//}
}

//! Удалить менеджер голосовой связи
void CVoipManagerConaito::Release()
{
	MP_DELETE_THIS;
}

void CVoipManagerConaito::CloseVoipManServer()
{
	if( GetAudioSubsystemType() == AUDIO_SUBSYSTEM_XP)
	{
		CWTMicContoller microphoneCtrl;
		std::wstring sDeviceName = GetDeviceNameByGuid(currentMicrophoneDeviceGuid.c_str());
		CWTMicContoller::CDeviceName lineName = microphoneCtrl.GetDeviceMicrophoneName( sDeviceName);
		CComString sLog = (audioSource == VAS_MICROPHONE ? "microphone is now set" : "mixer is now set");
		WriteLogLine( sLog.GetBuffer());
		if( audioSource != VAS_MICROPHONE && lineName.size() > 0 && microphoneCtrl.EnableOneDeviceLine( sDeviceName, lineName))
		{
			USES_CONVERSION;
			sLog = "change input to microphone: ";
			sLog = W2A(sDeviceName.c_str());
			sLog += "line==";
			sLog += W2A(lineName.c_str());
			WriteLogLine( sLog.GetBuffer());
			currentEvoVoipLineName = lineName;
		}
	}
	DisconnectImpl();
	m_voipManagerPipe->CloseVoipManServer();
}

//! Возвращает громкость вашего голоса
float CVoipManagerConaito::GetCurrentInputLevel()
{
	return (float)m_currentInputLevel;
}

//! Устанавливает громкость вашего голоса
void CVoipManagerConaito::SetCurrentInputLevel( float aCurrentInputLevel)
{
	mutexCurrentInputLevel.lock();
	m_currentInputLevel = aCurrentInputLevel;
	mutexCurrentInputLevel.unlock();
}

//! Возвращает громкость вашего голоса с учетом параметров усиления и порога
float CVoipManagerConaito::GetCurrentVoiceInputLevelNormalized()
{
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

/*
Возвращает список устройств входных аудио линий, с которых можно попробовать записать звук.
Формат:
Текущее устройство|Текущая выбранная линия;
Первое устройство|1-я линия|2-я линия;
Второе устройство|1-я линия|2-я линия;
Третье устройство|1-я линия|2-я линия
*/
unsigned int CVoipManagerConaito::GetAudioSubsystemType()
{
//	return AUDIO_SUBSYSTEM_VISTA;
	if( context->mpRM == NULL)
		return -1;
	unsigned int osMajor, osMinor, osBuild;
	context->mpRM->GetOSVersion( &osMajor, &osMinor, &osBuild);
	if (osMajor > 6 || (osMajor == 6 && osBuild >= 6001))
		return AUDIO_SUBSYSTEM_VISTA;

	return AUDIO_SUBSYSTEM_XP;
}

bool CVoipManagerConaito::GetInputDevices( bool abForce)
{
	if( inputDevicesInfo.size() == 0 || abForce)
	{		
		//m_voipManagerPipe->SendGetInputDevices( true, abForce);
		m_voipManagerPipe->SendGetDevices( true, abForce);
		return false;
	}
	OnGetInputDevices();
	return true;
}

bool CVoipManagerConaito::DeviceIsEqual( std::wstring aDevice1, std::wstring aDevice2)
{	
	if( aDevice1.size() > 0 && aDevice2.size() > 0 && (aDevice1.find(aDevice2.c_str()) == 0 || aDevice2.find(aDevice1.c_str()) == 0))
		return true;
	return false;
}

bool CVoipManagerConaito::DeviceIsIntoArrayName( std::wstring aDevice1, CWTMicContoller::CDevicesNamesVector &aDeviceGuidArray)
{
	for( CWTMicContoller::CDevicesNamesVector::iterator it = aDeviceGuidArray.begin(); it != aDeviceGuidArray.end(); ++it)
	{
		if( DeviceIsEqual( *it, aDevice1))
			return true;
	}
	return false;
}

bool CVoipManagerConaito::OnGetInputDevices()
{
	CComString sLog;
	sLog.Format("[VOIP] OnGetInputDevices inputDevicesInfo.size==%d", inputDevicesInfo.size());
	WriteLogLine( sLog.GetBuffer());
	int aiBufferLength = 1024;
//	wchar_t alpcDeviceGuids[ 1024];
	wchar_t alpcTemp[ 10];
	ZeroMemory(alpcTemp, 10);
	std::wstring sMicDeviceNames;
	std::wstring sMixDeviceNames;

	int audioSystem = GetAudioSubsystemType();

	_itow_s( audioSystem,  alpcTemp, 10, 10);
	sMicDeviceNames = alpcTemp; sMixDeviceNames = alpcTemp;

	int iCurrentDeviceLineIndex = -1;
	int iSimilarDeviceLineIndex = -1;	// номер устройства, у которого совпадает "часть" имени	
	int indexMic = -1;
	int indexMix = -1;
	CDSMixer2 dsMixer;

	CWTMicContoller micController;

	int iCurrentDeviceMixLineIndex = -1;   // номер mix-устройства
	int iSimilarDeviceMixLineIndex = -1;   // номер mix-устройства, у которого совпадает "часть" имени
	std::wstring currentMixLine = currentEvoVoipMixLineName;
	std::wstring currentDevice = currentEvoVoipDeviceGuid;
	std::wstring currentLine = currentEvoVoipLineName;
	
	if( GetAudioSubsystemType() == AUDIO_SUBSYSTEM_XP)
	{
		if( currentDevice.empty())
		{
			currentDevice = TryGetCurrentInDeviceGuid( false);
			currentLine.clear();

			CComString str;
			USES_CONVERSION;
			str.Format("[VOIP]: OnGetInputDevices currentMicrophoneDeviceGuid == '' and change to = %s", W2A( currentEvoVoipDeviceGuid.c_str()));
			WriteLogLine( str.GetBuffer());
		}
		std::wstring sDeviceName = GetDeviceNameByGuid( currentDevice.c_str());
		if( currentLine.empty() )
			currentLine = micController.GetDeviceMicrophoneName( sDeviceName);	
		if( currentMixLine.empty() /*|| bDefaultMixUsed*/)
		{
			if( !currentMixDeviceGuid.empty())
			{
				currentMixLine = micController.GetDeviceMixerName( currentMixDeviceGuid);
			}
			if( currentMixLine.empty() && currentMixDeviceGuid != sDeviceName)
			{
				currentMixLine = micController.GetDeviceMixerName( sDeviceName);
				if( !currentMixLine.empty())
					currentMixDeviceGuid = currentDevice;
			}
		}
		if( currentMixLine.size() == 0)
		{
			USES_CONVERSION;
			sLog.Format("[VOIP] OnGetInputDevices currentMixLine.size() == 0, currentDevice == %s and set currentMixLine from currentLine", W2A(currentDevice.c_str()));
			WriteLogLine( sLog.GetBuffer());
			currentMixLine = currentLine;
			currentMixDeviceGuid = currentDevice;
		}
	}

	if ( inputDevicesInfo.size() > 1)
	{
		// все устройства помещаем в строку через разделитель ';'
		vecDeviceInfoIt it = inputDevicesInfo.begin(), end = inputDevicesInfo.end();
		it++;	// первое устройство пропускаем		

		CWTMicContoller::CDevicesNamesVector micNames;
		if( audioSystem == AUDIO_SUBSYSTEM_XP)
			micNames = micController.GetDeviceMicrophoneNames( it->szDeviceName);
		else
		{
			mutexAudioDevice.lock();
			if( m_bCheckAudioDevices && m_pDefaultAudioDeviceManager == NULL)
				m_pDefaultAudioDeviceManager = MP_NEW( CDefaultAudioDeviceManager);
			m_pDefaultAudioDeviceManager->GetMicrophones( micNames);
			mutexAudioDevice.unlock();
		}
		indexMic++;
		indexMix++;
	
		for (; it!=end; ++it)
		{
			if(  micController.IsExludeName(it->szDeviceName))
				continue;

			if( audioSystem == AUDIO_SUBSYSTEM_XP)
			{							
				std::wstring s = L";"; s += it->sGUID;s += L"="; s += it->szDeviceName; 
				bool bNeedMicAdding = true;
				bool bNeedMixAdding = true;
				// все линии устройства помещаем в строку через разделитель '|'
				CDSMixer2::VecDeviceLines allDeviceLines = dsMixer.GetInDeviceLines(it->szDeviceName);
				for(CDSMixer2::VecDeviceLines::iterator itLines = allDeviceLines.begin(); itLines!=allDeviceLines.end(); ++itLines)
				{
					if( !micController.IsMixerName( itLines->c_str()))
					{
						if( bNeedMicAdding)
						{
							sMicDeviceNames += s;
							bNeedMicAdding = false;
						}
						sMicDeviceNames += L'|';
						sMicDeviceNames += itLines->c_str();
						if( iCurrentDeviceLineIndex == -1 && DeviceIsEqual( it->sGUID, currentMicrophoneDeviceGuid) && DeviceIsEqual( currentLine, *itLines))
							iCurrentDeviceLineIndex = indexMic;
						indexMic++;
					}

					if( !DeviceIsIntoArrayName( itLines->c_str(), micNames))
					{
						if( bNeedMixAdding)
						{
							sMixDeviceNames += s;
							bNeedMixAdding = false;
						}
						sMixDeviceNames += L'|';
						sMixDeviceNames += itLines->c_str();						
						if( iCurrentDeviceMixLineIndex== -1 && DeviceIsEqual( it->sGUID, currentMixDeviceGuid) && DeviceIsEqual( currentMixLine, *itLines))
							iCurrentDeviceMixLineIndex = indexMix;
						indexMix++;
					}				
					//bufferIndex += iLen;
				}
			}
			else
			{				
				/*if( !bDefaultUsed)
				{*/
					if( !micController.IsMixerName( it->szDeviceName))
					{
						sMicDeviceNames += L";"; sMicDeviceNames += it->sGUID; sMicDeviceNames += L"="; sMicDeviceNames += it->szDeviceName;
						if( DeviceIsEqual( it->sGUID, currentMicrophoneDeviceGuid))
							iCurrentDeviceLineIndex = indexMic;
						if( iCurrentDeviceLineIndex == -1 && iSimilarDeviceLineIndex == -1)
						{
							CWComString s1 = it->sGUID; s1.MakeLower();
							CWComString s2 = currentDevice.c_str(); s2.MakeLower();
							if( s1.Find( s2.GetBuffer()) > -1 || s2.Find( s1.GetBuffer()) > -1)
							{
								iSimilarDeviceLineIndex = indexMic;
							}
						}
						indexMic++;
					}
				/*}
				if( !bDefaultMixUsed)
				{*/
					if( !DeviceIsIntoArrayName( it->sGUID, micNames))
					{
						sMixDeviceNames += L";"; sMixDeviceNames += it->sGUID; sMixDeviceNames += L"="; sMixDeviceNames += it->szDeviceName;
						if( DeviceIsEqual( it->sGUID, currentMixDeviceGuid))
							iCurrentDeviceMixLineIndex = indexMix;
											
						if( iCurrentDeviceMixLineIndex == -1 && iSimilarDeviceMixLineIndex == -1)
						{
							CWComString s1 = it->sGUID; s1.MakeLower();
							CWComString s2 = currentDevice.c_str(); s2.MakeLower();
							if( s1.Find( s2.GetBuffer()) > -1 || s2.Find( s1.GetBuffer()) > -1)
							{
								iSimilarDeviceMixLineIndex = indexMix;
							}
						}
						indexMix++;
					}
				//}
			}
		}
	}
	else
	{
		iCurrentDeviceLineIndex = 0;
		iCurrentDeviceMixLineIndex = 0;
	}
	// если не нашли точного совпадения, возьмем частичное
	if( /*!bDefaultUsed &&*/ iCurrentDeviceLineIndex == -1)
		iCurrentDeviceLineIndex = iSimilarDeviceLineIndex;

	// если не нашли точного совпадения, возьмем частичное
	if( /*!bDefaultMixUsed &&*/ iCurrentDeviceMixLineIndex == -1)
		iCurrentDeviceMixLineIndex = iSimilarDeviceMixLineIndex;

	// номер текущего устройства
	sMicDeviceNames += L";";
	sMixDeviceNames += L";";

	ZeroMemory(alpcTemp, 10);
	_itow_s( iCurrentDeviceLineIndex, alpcTemp, 10, 10);
	sMicDeviceNames += alpcTemp;
	ZeroMemory(alpcTemp, 10);
	_itow_s( iCurrentDeviceMixLineIndex, alpcTemp, 10, 10);
	sMixDeviceNames += alpcTemp;
	
	CALLBACK_TWO_PARAMS( onGetInputDevices, sMicDeviceNames.c_str(), sMixDeviceNames.c_str());	
	return true;
}

bool CVoipManagerConaito::GetOutputDevices( bool abForce)
{
	if( outputDevicesInfo.size() == 0 || abForce)
	{
		m_voipManagerPipe->SendGetOutputDevices( true, abForce);
		return false;
	}
	OnGetOutputDevices();
	return true;
}

bool CVoipManagerConaito::OnGetOutputDevices()
{
	CComString sLog;
	sLog.Format("[VOIP] OnGetOutputDevices outputDevicesInfo.size==%d", outputDevicesInfo.size());
	WriteLogLine( sLog.GetBuffer());

	int audioSystem = GetAudioSubsystemType();

	wchar_t alpcTemp[ 10];
	ZeroMemory(alpcTemp, 10);
	_itow_s( audioSystem,  alpcTemp, 10, 10);
	std::wstring sDeviceNames = alpcTemp;

	int iCurrentDeviceLineIndex = -1;
	int iSimilarDeviceLineIndex = 0;	// номер устройства, у которого совпадает "часть" имени	
	int index = -1;
	CDSMixer2 dsMixer;

	CWTMicContoller micController;
	std::wstring currentDevice = currentEvoVoipOutDeviceGuid;
	std::wstring currentLine = currentEvoVoipOutDeviceLine;
	if( currentDevice.empty())
	{
		currentDevice = micController.GetCurrentOutDeviceGuid( true);
		currentLine.clear();
	}

	bool bDefaultUsed = false;
	if( currentDevice == L"Default device")
		bDefaultUsed = true;

	if( currentLine.empty() || bDefaultUsed)
		currentLine = micController.GetOutDeviceLineName( GetDeviceNameByGuid(currentDevice.c_str()));
	
	if ( outputDevicesInfo.size() > 1)
	{
		// все устройства помещаем в строку через разделитель ';'
		vecDeviceInfoIt it = outputDevicesInfo.begin(), end = outputDevicesInfo.end();
		//if ( outputDevicesInfo.size() > 1)
		//{
			it++;	// первое устройство пропускаем
		//}

		for (; it!=end; ++it)
		{
			if(  micController.IsExludeName(it->szDeviceName))
				continue;
			sDeviceNames += L";";
			sDeviceNames += it->sGUID;
			sDeviceNames += L"=";
			sDeviceNames += it->szDeviceName;			

			if( audioSystem == AUDIO_SUBSYSTEM_XP)
			{
				// все линии устройства помещаем в строку через разделитель '|'
				//CDSMixer2::VecDeviceLines allDeviceLines = dsMixer.GetOutDeviceLines(it->sGUID);
				CWTMicContoller::CDevicesNamesVector allDeviceLines = micController.GetOutDeviceLineNames(it->sGUID);
				for(CWTMicContoller::CDevicesNamesVector::iterator itLines = allDeviceLines.begin(); itLines!=allDeviceLines.end(); ++itLines)
				{
					index = index + 1;

					sDeviceNames += L'|';
					sDeviceNames += itLines->c_str();

					if( !bDefaultUsed)
					{
						if( DeviceIsEqual( it->sGUID, currentDevice) &&  DeviceIsEqual( currentLine, *itLines))
							iCurrentDeviceLineIndex = index;
					}										
				}
			}
			else
			{
				index = index + 1;

				if( !bDefaultUsed)
				{
					if( DeviceIsEqual( it->sGUID, currentDevice))
						iCurrentDeviceLineIndex = index;

					if( iCurrentDeviceLineIndex == -1 && iSimilarDeviceLineIndex == -1)
					{
						/*if( wcsstr( it->sGUID, currentDevice.c_str()) != NULL)
						{
							iSimilarDeviceLineIndex = index;
						}*/
						CWComString s1 = it->sGUID; s1.MakeLower();
						CWComString s2 = currentDevice.c_str(); s2.MakeLower();
						if( s1.Find( s2.GetBuffer()) > -1 || s2.Find( s1.GetBuffer()) > -1)
						{
							iSimilarDeviceLineIndex = index;
						}
					}
				}
			}
		}
	}
	// если не нашли точного совпадения, возьмем частичное
	if( !bDefaultUsed && iCurrentDeviceLineIndex == -1)
		iCurrentDeviceLineIndex = iSimilarDeviceLineIndex;
	
	sDeviceNames += L';';

	ZeroMemory(alpcTemp, 10);
	_itow_s( iCurrentDeviceLineIndex, alpcTemp, 10, 10);
	sDeviceNames += alpcTemp;

	CALLBACK_ONE_PARAM( onGetOutputDevices,  sDeviceNames.c_str());
	return true;
}


void CVoipManagerConaito::OnSetInputDeviceMode( std::wstring &aDeviceGuid, std::wstring &aDeviceLine, int aiToIncludeSoundDevice, int aSource)
{	
	currentEvoVoipDeviceGuid = aDeviceGuid;
	currentMicrophoneDeviceGuid = aDeviceGuid;
	if( currentMixDeviceGuid.empty() && currentMicrophoneDeviceGuid != L"Default device")
		currentMixDeviceGuid = aDeviceGuid;
	if( GetAudioSubsystemType() == AUDIO_SUBSYSTEM_XP)
	{
		CWTMicContoller microphoneCtrl;
		std::wstring enableDevice = GetDeviceNameByGuid( currentEvoVoipDeviceGuid.c_str());

		if( microphoneCtrl.EnableOneDeviceLine( enableDevice , aDeviceLine.c_str()))
		{
			currentEvoVoipLineName = aDeviceLine;
		}
		else
		{
			CWTMicContoller::CDeviceName lineName = microphoneCtrl.GetDeviceMicrophoneName( enableDevice );
			if( microphoneCtrl.EnableOneDeviceLine( enableDevice , lineName))
			{
				currentEvoVoipLineName = lineName;
			}
		}
	}
	else
	{
		if( !_wcsicmp(currentMicrophoneDeviceGuid.c_str(), L"Default device"))
		{
			currentMicrophoneDeviceGuid = TryGetCurrentInDeviceGuid( true);
			/*std::wstring sMicrophone = TryGetCurrentInDeviceGuid( true);
			if ( inputDevicesInfo.size() > 1 && !wcsicmp(inputDevicesInfo[0].sGUID, L"default device"))
				currentMicrophoneDeviceGuid = inputDevicesInfo[1].sGUID;
			else if( inputDevicesInfo.size() > 0)
				currentMicrophoneDeviceGuid = inputDevicesInfo[0].sGUID;
			if( !sMicrophone.empty() && currentMicrophoneDeviceGuid != sMicrophone && wcsicmp(sMicrophone.c_str(), L"default device"))
			{
				if( aSource == VAS_MICROPHONE)
				{
					currentMicrophoneDeviceGuid = L"";
					SetInputDeviceMode( sMicrophone.c_str(), L"", VAS_MICROPHONE, 0);
				}
				else
					currentMicrophoneDeviceGuid = sMicrophone;
			}*/
			CComString str;
			USES_CONVERSION;
			str.Format("[VOIP]: OnSetInputDeviceMode currentMicrophoneDeviceGuid == 'Default device' and change to = %s", W2A( currentEvoVoipDeviceGuid.c_str()));
			WriteLogLine( str.GetBuffer());
		}
	}

	if( aSource != 0)
		audioSource = VAS_MICROPHONE;

	OnAutoVolumeChanged();

	if( context->mpLogWriter)
	{
		CComString str;
		USES_CONVERSION;
		str.Format("[VOIP]: OnSetInputDeviceMode successfully finished  abToIncludeSoundDevice = %d, currentEvoVoipLineName = %s", aiToIncludeSoundDevice, W2A( currentEvoVoipLineName.c_str()));
		WriteLogLine( str.GetBuffer());
	}
	CALLBACK_TWO_PARAMS( onSetInputDevice, 0, aiToIncludeSoundDevice);
}

bool CVoipManagerConaito::SetInputDeviceMode( const wchar_t* alpcDeviceGuid, const wchar_t* alpcDeviceLine, int aiToIncludeSoundDevice)
{
	return SetInputDeviceMode( alpcDeviceGuid, alpcDeviceLine, VAS_MICROPHONE, aiToIncludeSoundDevice);
}

bool CVoipManagerConaito::SetInputDeviceMode( const wchar_t* alpcDeviceGuid, const wchar_t* alpcDeviceLine, int aiVasDevice, int aiToIncludeSoundDevice)
{
	/* 
	aiToIncludeSoundDevice:
	1 - меняем в системе устройство и результат передаем в скрипт
	0 - меняем в системе устройство и результат передаем в скрипт
	-1 - только запоминаем название устройства и линии, будет установлено при пользовательском запросе.
	*/
	if( context->mpLogWriter)
	{
		USES_CONVERSION;
		CComString str;
		str.Format("[VOIP]: SetInputDeviceMode: alpcDeviceGuid = %s, alpcDeviceLine = %s", (alpcDeviceGuid==NULL)?"NULL":W2A(alpcDeviceGuid), (alpcDeviceLine==NULL)?"NULL":W2A(alpcDeviceLine));
		WriteLogLine( str.GetBuffer());
	}

	std::wstring wsDeviceToSet;
	std::wstring wsDeviceLineToSet;
	CWTMicContoller microphoneCtrl;

	m_currentModeTopInputLevel = MAX_STABLE_AUDIO_LEVEL_VOICE;
	m_currentModeCurrentTopInputLevel = m_currentModeTopInputLevel;

	audioSource = VAS_NONE;

	if( !alpcDeviceGuid || !(*alpcDeviceGuid))	// устройство не определено
	{
		wsDeviceToSet = TryGetCurrentInDeviceGuid( true);
		//wsDeviceToSet = L"Default device";
	}
	else
	{
		if( microphoneCtrl.IsMixerName(alpcDeviceGuid))
			wsDeviceToSet = TryGetCurrentInDeviceGuid( true);
		wsDeviceToSet = alpcDeviceGuid;
	}

	if( GetAudioSubsystemType() == AUDIO_SUBSYSTEM_XP)
	{
		if( !alpcDeviceLine || !(*alpcDeviceLine) || microphoneCtrl.IsMixerName(alpcDeviceLine))	// устройство не определено или микшер
		{
			wsDeviceLineToSet = microphoneCtrl.GetDeviceMicrophoneName( wsDeviceToSet);
		}
		else
		{
			wsDeviceLineToSet = alpcDeviceLine;
		}
	}

	if( context->mpLogWriter)
	{
		USES_CONVERSION;
		CComString str;
		str.Format("[VOIP]: currentEvoVoipDeviceGuid = %s, wsDeviceToSet = %s, wsDeviceLineToSet = %s, abToIncludeSoundDevice = %d", W2A(currentEvoVoipDeviceGuid.c_str()), W2A(wsDeviceToSet.c_str()), W2A(wsDeviceLineToSet.c_str()), aiToIncludeSoundDevice);
		WriteLogLine( str.GetBuffer());
	}

	if( aiToIncludeSoundDevice == -1)
	{
		currentMicrophoneDeviceGuid = wsDeviceToSet;
		currentEvoVoipLineName = wsDeviceLineToSet;
		return true;
	}

	if( !alpcDeviceGuid || !(*alpcDeviceGuid) || wsDeviceToSet != currentEvoVoipDeviceGuid || outputDeviceWasChanged /*|| currentMicrophoneDeviceGuid != wsDeviceToSet*/)
	{	
		std::wstring mixerLine = L"";
		if( !InitializeVoipOnAudioDevice( wsDeviceToSet.c_str(), aiVasDevice, wsDeviceLineToSet, mixerLine, aiToIncludeSoundDevice))
			return false;		
	}
	else
	{
		/*if( aiVasDevice == VAS_VIRTUALCABLE)
			OnSetInputVirtualAudioCableMode( abToIncludeSoundDevice);
		else*/
			OnSetInputDeviceMode( wsDeviceToSet, wsDeviceLineToSet, aiToIncludeSoundDevice, VAS_MICROPHONE);
		WriteLogLine( "[VOIP]:  SetInputDeviceMode::currentEvoVoipDeviceGuid == currentInDeviceName");
	}

	return true;
}

void CVoipManagerConaito::OnSetInputMixerMode( std::wstring &aDeviceGuid, std::wstring &aMixerName, int aiToIncludeSoundDevice, int aSource)
{
	CWTMicContoller microphoneCtrl;
	currentMixDeviceGuid = aDeviceGuid;
	if( currentMicrophoneDeviceGuid.empty() && currentMixDeviceGuid != L"Default device")
		currentMicrophoneDeviceGuid = aDeviceGuid;
	bool bWinXP = (GetAudioSubsystemType() == AUDIO_SUBSYSTEM_XP);
	if( bWinXP)
	{
		std::wstring enableDevice = GetDeviceNameByGuid( currentMixDeviceGuid.c_str());
		if( microphoneCtrl.EnableOneDeviceLine( enableDevice, aMixerName))
		{
			currentEvoVoipMixLineName = aMixerName;
		}		
		else	if( microphoneCtrl.EnableOneDeviceLine( enableDevice, microphoneCtrl.GetDeviceMixerName( enableDevice)))
		{
			currentEvoVoipMixLineName = microphoneCtrl.GetDeviceMixerName( enableDevice);
		}
		else
		{
			// находим устройство, потом когда пользователь активирует в скрипте. будет переустановка устройства в evoip
			vecDeviceInfoIt it = inputDevicesInfo.begin(), end = inputDevicesInfo.end();
			for (; it!=end; ++it)
			{
				enableDevice = it->sGUID;
				if( microphoneCtrl.EnableOneDeviceLine( enableDevice, microphoneCtrl.GetDeviceMixerName( enableDevice)))
				{
					currentEvoVoipMixLineName = microphoneCtrl.GetDeviceMixerName( enableDevice);
					currentMixDeviceGuid = enableDevice;
					break;
				}
			}
			if( it == end)
			{												
				CALLBACK_TWO_PARAMS( onSetInputDevice, 111, aiToIncludeSoundDevice);
				if( context->mpLogWriter)
				{
					CComString str;
					USES_CONVERSION;
					str.Format("[VOIP]: OnSetInputMixerMode failed set mixer line currentMixDeviceGuid == %s, enableDevice == %s", W2A(currentMixDeviceGuid.c_str()), W2A(enableDevice.c_str()));
					WriteLogLine( str.GetBuffer());
				}
				return;
			}
		}
		//
	}
	else
	{
		if( !_wcsicmp(currentMixDeviceGuid.c_str(), L"Default device"))
		{
			currentMixDeviceGuid = TryGetCurrentInDeviceGuid( false);				
			CComString str;
			USES_CONVERSION;
			str.Format("[VOIP]: OnSetInputMixerMode currentMixDeviceGuid == 'Default device' and change to = %s", W2A( currentEvoVoipDeviceGuid.c_str()));
			WriteLogLine( str.GetBuffer());
		}
	}

	if( aSource != 0)
		audioSource = VAS_MIXER;

	OnAutoVolumeChanged();

	if( context->mpLogWriter)
	{
		CComString str;
		USES_CONVERSION;
		str.Format("[VOIP]: OnSetInputMixerMode successfully finished  abToIncludeSoundDevice = %d,  bWinXP==%d,  aDeviceGuid==%s,  currentEvoVoipMixLineName==%s", aiToIncludeSoundDevice, (int)bWinXP, W2A(aDeviceGuid.c_str()), W2A(currentEvoVoipMixLineName.c_str()));
		WriteLogLine( str.GetBuffer());
	}
	CALLBACK_TWO_PARAMS( onSetInputDevice, 0, aiToIncludeSoundDevice);
}

bool CVoipManagerConaito::SetInputMixerMode( const wchar_t* alpcDeviceGuid, const wchar_t* alpcDeviceLine, int aiToIncludeSoundDevice)
{
	if( context->mpLogWriter)
	{
		CComString str = "[VOIP]: SetInputMixerMode";		
		WriteLogLine( str.GetBuffer());
	}

	m_currentModeTopInputLevel = MAX_STABLE_AUDIO_LEVEL_SHARING;
	m_currentModeCurrentTopInputLevel = m_currentModeTopInputLevel;

	CWTMicContoller microphoneCtrl;
	CWTMicContoller::CDeviceName mixerName;
	if( alpcDeviceLine != NULL)
		mixerName = alpcDeviceLine;

	audioSource = VAS_NONE;

	std::wstring currentInDeviceName;
	if( !alpcDeviceGuid || !(*alpcDeviceGuid))	// устройство не определено
	{
		currentInDeviceName = TryGetCurrentInDeviceGuid( false);
		if( GetAudioSubsystemType() == AUDIO_SUBSYSTEM_XP)
		{
			mixerName = microphoneCtrl.GetDeviceMixerName( currentInDeviceName);
			if( mixerName.size() == 0)
			{
				USES_CONVERSION;
				CComString sLog1;
				sLog1.Format("[VOIP] SetInputMixerMode mixerName.size() == 0, currentInDeviceName == %s", W2A(currentInDeviceName.c_str()));
				WriteLogLine( sLog1.GetBuffer());
				currentInDeviceName = L"";
			}
		}
	}
	else
	{
		currentInDeviceName = alpcDeviceGuid;
		if( GetAudioSubsystemType() != AUDIO_SUBSYSTEM_XP)
		{
			CWTMicContoller::CDevicesNamesVector micNames;			
			mutexAudioDevice.lock();
			if( m_bCheckAudioDevices && m_pDefaultAudioDeviceManager == NULL)
				m_pDefaultAudioDeviceManager = MP_NEW( CDefaultAudioDeviceManager);
			m_pDefaultAudioDeviceManager->GetMicrophones( micNames);
			mutexAudioDevice.unlock();
			if( DeviceIsIntoArrayName( alpcDeviceGuid, micNames))
			{
				currentInDeviceName = L"";
				wchar_t s[1024];
				VOIPAC_ERROR errorCode = m_pDefaultAudioDeviceManager->GetFirstActiveNoMicrophone( s, 1024);
				if( errorCode == ACERROR_NOERROR)
					currentInDeviceName = s;
			}
		}			
	}

	if( aiToIncludeSoundDevice == -1)
	{
		currentMixDeviceGuid = currentInDeviceName;
		currentEvoVoipMixLineName = mixerName;
		return true;
	}	

	if( !alpcDeviceGuid || !(*alpcDeviceGuid) || currentInDeviceName != currentEvoVoipDeviceGuid || outputDeviceWasChanged /*|| currentMixDeviceGuid != currentInDeviceName*/)
	{	
		std::wstring deviceLine = L"";
		if( !InitializeVoipOnAudioDevice( currentInDeviceName.c_str(), VAS_MIXER, deviceLine, mixerName, aiToIncludeSoundDevice))
		{
			return false;
		}
		return true;
	}
	OnSetInputMixerMode( currentInDeviceName, mixerName, aiToIncludeSoundDevice, VAS_MIXER);	
	WriteLogLine( "[VOIP]:  SetInputMixerMode::currentEvoVoipDeviceGuid == currentInDeviceName");
	return true;
}

bool CVoipManagerConaito::SetOutputDeviceMode( const wchar_t* alpcDeviceGuid, const wchar_t* alpcOutDeviceLine, int aiToIncludeSoundDevice/*, int aiVasDevice*/)
{
	if( context->mpLogWriter)
	{
		USES_CONVERSION;
		CComString str;
		str.Format("[VOIP]: Remember new output device in vars ( real setting with changed unput device): alpcDeviceGuid = %s, alpcOutDeviceLine = %s", (alpcDeviceGuid==NULL)?"NULL":W2A(alpcDeviceGuid), (alpcOutDeviceLine==NULL)?"NULL":W2A(alpcOutDeviceLine));
		//str.Format("[VOIP]: Set output device[1]: alpcDeviceGuid = %s", (alpcDeviceGuid==NULL)?"NULL":W2A(alpcDeviceGuid));
		WriteLogLine( str.GetBuffer());
	}

	std::wstring wsDeviceToSet;
	std::wstring wsDeviceLineToSet;
	CWTMicContoller microphoneCtrl;

	m_currentModeTopOutputLevel = MAX_STABLE_AUDIO_LEVEL_VOICE;
	m_currentModeCurrentTopOutputLevel = m_currentModeTopOutputLevel;

	//outAudioSource = VAS_NONE;

	if( !alpcDeviceGuid || !(*alpcDeviceGuid))	// устройство не определено
	{
		//wsDeviceToSet = microphoneCtrl.GetCurrentOutDeviceName();
		wsDeviceToSet = L"Default device";
	}
	else
	{
		wsDeviceToSet = alpcDeviceGuid;
	}

	if( GetAudioSubsystemType() == AUDIO_SUBSYSTEM_XP)
	{
		if( !alpcOutDeviceLine || !(*alpcOutDeviceLine))	// линия не определена
		{
			wsDeviceLineToSet = microphoneCtrl.GetOutDeviceLineName( wsDeviceToSet);
		}
		else
		{
			wsDeviceLineToSet = alpcOutDeviceLine;
		}
	}
	
	//////////////////////////////////////////////////////////////////////////

	if( context->mpLogWriter)
	{
		USES_CONVERSION;
		CComString str;
		str.Format("[VOIP]: currentEvoVoipOutDeviceGuid = %s, wsDeviceToSet = %s, wsDeviceLineToSet = %s, abToIncludeSoundDevice = %d", W2A(currentEvoVoipOutDeviceGuid.c_str()), W2A(wsDeviceToSet.c_str()), W2A(wsDeviceLineToSet.c_str()), aiToIncludeSoundDevice);
		WriteLogLine( str.GetBuffer());
	}

	if( currentEvoVoipOutDeviceGuid != wsDeviceToSet)
	{	
		// запомним текущие устройства
		oldCurrentOutDeviceName = currentEvoVoipOutDeviceGuid;
		oldCurrentEvoVoipOutDeviceLine = currentEvoVoipOutDeviceLine;

		// запишем требование установить новые устройства
		currentEvoVoipOutDeviceGuid = wsDeviceToSet.c_str();
		currentEvoVoipOutDeviceLine = wsDeviceLineToSet;

		outputDeviceWasChanged = true;
/*
		bool result = true;
		CONNECTION_STATUS	currentConnectionStatus = connectionStatus;
		// переинициализируем систему с новыми устройствами
		voipError errorCode = InitializeVoipSystem( -1, abToIncludeSoundDevice, currentEvoVoipDeviceGuid.c_str(), currentEvoVoipOutDeviceGuid.c_str());	
		return result;*/
	}
	/*else
	{		
		OnSetOutputDeviceMode( currentEvoVoipOutDeviceGuid, wsDeviceLineToSet, abToIncludeSoundDevice);
		WriteLogLine( "[VOIP]:  SetOutputDeviceMode::currentEvoVoipDeviceGuid == currentOutDeviceName");
	}*/

	return true;
}

void CVoipManagerConaito::OnSetOutputDeviceMode( std::wstring &aDeviceGuid, std::wstring &aDeviceLine, int aiToIncludeSoundDevice)
{
	// восстановим название устройства, т.к. оно к микшеру не относится
	CWTMicContoller microphoneCtrl;
	currentEvoVoipOutDeviceGuid = aDeviceGuid;
	currentEvoVoipOutDeviceLine = aDeviceLine;
	
	// сделаешь тебе как надо
	if( GetAudioSubsystemType() == AUDIO_SUBSYSTEM_XP)
	{
		std::wstring enableDevice = currentEvoVoipOutDeviceGuid;
		if( currentEvoVoipOutDeviceGuid == L"Default device")
		{
			enableDevice = microphoneCtrl.GetCurrentOutDeviceGuid( true);
		}
		enableDevice = GetDeviceNameByGuid( enableDevice.c_str());
		if( microphoneCtrl.EnableOneOutDeviceLine( enableDevice, aDeviceLine))
		{
			currentEvoVoipOutDeviceLine = aDeviceLine;
		}
		else	if( microphoneCtrl.EnableOneOutDeviceLine( enableDevice, microphoneCtrl.GetOutDeviceLineName( enableDevice)))
		{
			currentEvoVoipOutDeviceLine = microphoneCtrl.GetOutDeviceLineName( enableDevice);
		}
	}
	/*else if( !_wcsicmp(currentEvoVoipOutDeviceGuid.c_str(), L"Default device"))
	{
		currentEvoVoipOutDeviceGuid = microphoneCtrl.GetCurrentOutDeviceGuid( true);
	}*/

	//outAudioSource = VAS_MIXER;

	// для регулировки громкости при необходимости напиши функцию
	//OnAutoVolumeChanged();

	if( context->mpLogWriter)
	{
		CComString str;
		str.Format("[VOIP]: Set output device[2] successfully finished  abToIncludeSoundDevice = %d", aiToIncludeSoundDevice);
		WriteLogLine( str.GetBuffer());
	}
	CALLBACK_TWO_PARAMS( onSetOutputDevice, 0, aiToIncludeSoundDevice);
}

bool CVoipManagerConaito::InitializeVoipOnAudioDevice( const wchar_t* alpcDeviceGuid, int aiVasDevice, std::wstring &aDeviceLine, std::wstring &aMixDeviceLine, int aiToIncludeSoundDevice)
{
	if( context->mpLogWriter)
	{
		USES_CONVERSION;
		CComString str;
		str.Format("[VOIP]: InitializeVoipOnAudioDevice: alpcDeviceGuid = %s, currentEvoVoipDeviceGuid = %s, abToIncludeSoundDevice = %d", (alpcDeviceGuid==NULL)?"NULL":W2A(alpcDeviceGuid), W2A(currentEvoVoipDeviceGuid.c_str()), aiToIncludeSoundDevice);
		WriteLogLine( str.GetBuffer());
	}

	if( !alpcDeviceGuid)
	{
		WriteLogLine( "[VOIP]: alpcDeviceGuid == null");
		return false;	// некорректныbUserModyle =й параметр
	}

	// запомним текущие устройства
	oldCurrentDeviceName = currentEvoVoipDeviceGuid;
	if( currentDeviceLine.size() > 0)
		oldDeviceLine = currentDeviceLine;
	if( currentMixerGuid.size() > 0)
		oldMixerName = currentMixerGuid;

	// запишем требование установить новые устройства
	currentEvoVoipDeviceGuid = alpcDeviceGuid;
	if( aDeviceLine.size() > 0)
		currentDeviceLine = aDeviceLine;
	if( aMixDeviceLine.size() > 0)
		currentMixerGuid = aMixDeviceLine;
	

	bool result = true;
	CONNECTION_STATUS	currentConnectionStatus = connectionStatus;
	// переинициализируем систему с новыми устройствами
	voipError errorCode = InitializeVoipSystem( aiVasDevice, aiToIncludeSoundDevice, alpcDeviceGuid, currentEvoVoipOutDeviceGuid.c_str());	
	return result;
}

bool CVoipManagerConaito::SetUserPosition( int nUserID, float x, float y, float z)
{
	ATLASSERT(false);
	return true;
}

/*void EvaluateMixers()
{
	HMIXER hMixer;
	HRESULT hr;

	std::vector<std::wstring>	wsNames;
	unsigned int mixerID = 0xFFFFFFFF;
	int deviceCount = waveInGetNumDevs();
	for( int idevice=0; idevice<deviceCount; idevice++)
	{
		WAVEINCAPSW caps;
		::waveInGetDevCapsW(idevice, &caps, sizeof(caps));

		MMRESULT err = mixerGetID((HMIXEROBJ)idevice, &mixerID, MIXER_OBJECTF_WAVEIN);
		if( err != MMSYSERR_NOERROR)
			continue;
		if( mixerID == -1)
			continue;

		hr = mixerOpen(&hMixer, mixerID, 0, 0, 0);
		if (FAILED(hr))
		{
			continue;
		}

		DWORD kinds[] = {MIXERLINE_COMPONENTTYPE_DST_WAVEIN, MIXERLINE_COMPONENTTYPE_DST_VOICEIN, MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE};
		for( int idev=0; idev<sizeof(kinds)/sizeof(kinds[0]); idev++)
		{
			int ii=0;
			switch(kinds[idev]) {
			case MIXERLINE_COMPONENTTYPE_DST_WAVEIN:
				ii=0;
				break;
			case MIXERLINE_COMPONENTTYPE_DST_VOICEIN:
				ii=0;
				break;
			case MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE:
				ii=0;
				break;
			};
			MIXERLINEW mxl;
			MIXERCONTROLW mc[2];
			MIXERLINECONTROLSW mxlc;
			DWORD kind, count;

			kind = kinds[idev];

			mxl.cbStruct = sizeof(mxl);
			mxl.dwComponentType = kind;

			hr = mixerGetLineInfoW((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);
			if (FAILED(hr))
			{
				continue;
			}

			count = mxl.cConnections;

			for(UINT i = 0; i < count; i++)
			{
				mxl.dwSource = i;
				mixerGetLineInfoW((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_SOURCE);

				mc[0].cbStruct = sizeof(mc);
				mc[1].cbStruct = sizeof(mc);
				mxlc.cbStruct = sizeof(mxlc);
				mxlc.dwLineID = mxl.dwLineID;
				mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
				mxlc.cControls = 1;
				mxlc.cbmxctrl = sizeof(MIXERCONTROLW);
				mxlc.pamxctrl = (LPMIXERCONTROLW)&mc[0];
				hr = mixerGetLineControlsW((HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);
			}
		}
		mixerClose(hMixer);
	}
	
}*/

void CVoipManagerConaito::UpdateVoipConnection()
{
	if(context->mpMapMan != NULL)
		context->mpMapMan->UpdateVoipConnection();
}

//! Инициализации голосовой связи на устройстве
voipError CVoipManagerConaito::InitializeVoipSystem( int aiVasDevice, int aiToIncludeSoundDevice, const wchar_t *alpcDeviceGuid, const wchar_t *alpcOutDeviceName)
{
	m_voipManagerPipe->SendInitVoipSystem( aiVasDevice, aiToIncludeSoundDevice, alpcDeviceGuid, alpcOutDeviceName);
	return 0;
}

//! Инициализации голосовой связи на устройстве
voipError CVoipManagerConaito::ReInitializeVoipSystem()
{
	connectionStatus = CS_DISCONNECTED;
	channelConnectionStatus = CS_DISCONNECTED;
	CALLBACK_NO_PARAMS(onConnectionLost);
	CALLBACK_NO_PARAMS(onReInitializeVoipSystem);
	m_voipManagerPipe->SendInitVoipSystem( audioSource, true, currentEvoVoipDeviceGuid.c_str(), currentEvoVoipOutDeviceGuid.c_str());
	CComString str;
	USES_CONVERSION;
	str.Format("[VOIP]: ReInitializeVoipSystem audioSource = %d, currentEvoVoipDeviceGuid = %s, currentDeviceLine = %s, currentMixerGuid = %s", (int)audioSource, W2A(currentEvoVoipDeviceGuid.c_str()), W2A(currentDeviceLine.c_str()), W2A(currentMixerGuid.c_str()));
	WriteLogLine( str.GetBuffer());
	return 0;
}

bool CVoipManagerConaito::OnSendInitVoipSystem( int aErrorCode, bool abInitVoipSystem, int aiVasDevice, int aiToIncludeSoundDevice, std::wstring &aDeviceGuid, std::wstring &aOutDeviceName)
{
	MP_NEW_P5( pDeviceInfo, CInitDeviceInfo, aErrorCode, abInitVoipSystem, aiVasDevice, aDeviceGuid, aOutDeviceName);
	::SendMessage( voipWindowClient.m_hWnd, WM_INITVOIPSYSTEM, (WPARAM)pDeviceInfo, aiToIncludeSoundDevice);
	return true;
}

void CVoipManagerConaito::OnFindedMicName(const wchar_t * aDeviceGuid, const wchar_t * aDeviceName, const wchar_t * aDeviceLineName)
{
	CALLBACK_THREE_PARAMS( onRecievedFindedMicName, aDeviceGuid, aDeviceName, aDeviceLineName);
}

void CVoipManagerConaito::OnDeviceErrorCode(int anErrorCode)
{
	CALLBACK_ONE_PARAM( onRecievedDeviceErrorCode, anErrorCode);
}

LRESULT CVoipManagerConaito::OnInitVoipSystem( WPARAM wParam, LPARAM lParam)
{
	CInitDeviceInfo *pDeviceInfo = (CInitDeviceInfo*)wParam;
	int iToIncludeSoundDevice = (int)lParam;
	if(pDeviceInfo->errorCode != ERROR_NOERROR)
	{
		if( currentEvoVoipDeviceGuid == oldCurrentDeviceName)
		{
			CALLBACK_TWO_PARAMS( onSetInputDevice, pDeviceInfo->errorCode, iToIncludeSoundDevice);
			return false;
		}
				
		// восстановим предыдущие устройства. Даже если будет ошибка, то функция ее не корректирует
		if( pDeviceInfo->iVasDevice >= 0)
		{
			currentEvoVoipDeviceGuid = oldCurrentDeviceName;
			if(  pDeviceInfo->iVasDevice == 2)
				currentMixerGuid = oldMixerName;
			else
				currentDeviceLine = oldDeviceLine;
			InitializeVoipSystem( pDeviceInfo->iVasDevice, iToIncludeSoundDevice, oldCurrentDeviceName.c_str(), currentEvoVoipOutDeviceGuid.c_str());
		}
		else
		{
			currentEvoVoipOutDeviceGuid = oldCurrentOutDeviceName;
			currentEvoVoipOutDeviceLine = oldCurrentEvoVoipOutDeviceLine;
			InitializeVoipSystem( pDeviceInfo->iVasDevice, iToIncludeSoundDevice, currentEvoVoipDeviceGuid.c_str(), oldCurrentOutDeviceName.c_str());
		}
		if( requestedRoomName.IsEmpty() && requestedUserName.IsEmpty()) //serverIP = "";
			Connect( requestedRoomName, SKIP_REALITY_ID, requestedUserName, requestedUserPassword, requestedAvatarID);
		else
			ConnectImpl( false);

		MP_DELETE( pDeviceInfo);
		return 0;
	}
    outputDeviceWasChanged = false;
	if(pDeviceInfo->bInitVoipSystem)
		SetVoipSystemInitialized( pDeviceInfo->bInitVoipSystem);
	CONNECTION_STATUS	currentConnectionStatus = connectionStatus;
	/*if( currentConnectionStatus == CS_CONNECTED ||
		currentConnectionStatus == CS_CONNECTING)
	{*/
		/*if( !bToIncludeSoundDevice)
		{
			CComString str;
			str.Format("[VOIP]: OnInitVoipSystem no reconnect and bToIncludeSoundDevice = %d", (int)bToIncludeSoundDevice);
			WriteLogLine( str.GetBuffer());
		}
		else
		{*/
			if( requestedRoomName.IsEmpty() && requestedUserName.IsEmpty()) //serverIP = "";
				Connect( requestedRoomName, SKIP_REALITY_ID, requestedUserName, requestedUserPassword, requestedAvatarID);
			else
				ConnectImpl( false);
		//}
	//}

	CComString str;
	USES_CONVERSION;
	str.Format("[VOIP]: OnInitVoipSystem pDeviceInfo->iVasDevice = %d, pDeviceInfo->sDeviceName = %s", (int)pDeviceInfo->iVasDevice, W2A(pDeviceInfo->sDeviceName.c_str()));
	WriteLogLine( str.GetBuffer());

	if( pDeviceInfo->iVasDevice == VAS_MICROPHONE)
	{
		OnSetInputDeviceMode( pDeviceInfo->sDeviceName, currentDeviceLine, iToIncludeSoundDevice, pDeviceInfo->iVasDevice);
	}
	else if( pDeviceInfo->iVasDevice == VAS_MIXER)
	{
		OnSetInputMixerMode( pDeviceInfo->sDeviceName, currentMixerGuid, iToIncludeSoundDevice, pDeviceInfo->iVasDevice);
	}
	/*else if( pDeviceInfo->iVasDevice == VAS_VIRTUALCABLE)
	{
		OnSetInputVirtualAudioCableMode( bToIncludeSoundDevice);		
	}*/
	if( pDeviceInfo->iVasDevice == -1 || pDeviceInfo->sOutDeviceName != currentEvoVoipOutDeviceGuid)
	{
		OnSetOutputDeviceMode(  pDeviceInfo->sOutDeviceName, currentEvoVoipOutDeviceLine, iToIncludeSoundDevice);
	}
	MP_DELETE( pDeviceInfo);
	return 0;	
}

bool CVoipManagerConaito::RebuildIfEmptyDevicesList()
{
	if( inputDevicesInfo.size() != 0)
		return true;

	m_voipManagerPipe->SendGetDevices( true, false);
	return false;
}

void CVoipManagerConaito::DestroyVoipSystem()
{
	Disconnect();
}

//! Удаление окна и деинициализации библиотеки EVOVOIP
void CVoipManagerConaito::DestroyEvoSystem()
{
	MP_DELETE( m_voipManagerPipe);
	m_voipManagerPipe = NULL;
	if( voipWindowClient.IsWindow())
		voipWindowClient.DestroyWindow();
}

////////////////////////////////////////////////////////////////////////////////
// Events handling
////////////////////////////////////////////////////////////////////////////////
//! Вызывается после установки свойств
void CVoipManagerConaito::OnDirectSoundUsedChanged()
{
	//??
}

void CVoipManagerConaito::OnGainLevelChanged()
{
	m_voipManagerPipe->SendSetVoiceGainLevel(micGainLevel);
}

void CVoipManagerConaito::OnActivationLevelChanged()
{
	float val = GET_RANGED_VAL(1, 20, micActivationLevel);
	m_voipManagerPipe->SendSetVoiceActivationLevel( val);
}

void CVoipManagerConaito::OnVoiceActivatedChanged()
{
	HandleTransmissionStateChanged();

	OnActivationLevelChanged();
	OnAutoVolumeChanged();
}

void CVoipManagerConaito::OnGetMyUserID(int aUserID)
{
	m_userID = aUserID;

	if( microphoneEnabled)
	{
		if(!voiceActivated)		
		{		
			CALLBACK_TWO_PARAMS( onUserTalking, userName.GetBuffer(), 0);
		}
	}
	else
	{
		if( !voiceActivated)
		{
			CALLBACK_TWO_PARAMS( onUserStoppedTalking, userName.GetBuffer(), 0);
		}
	}
}

void CVoipManagerConaito::OnCommunicationAreaChanged()
{
	if( recordServerConnection->IsRecordingInProgress())
		recordServerConnection->ForceSynchronization( m_userID);
}

void CVoipManagerConaito::HandleTransmissionStateChanged()
{
	m_voipManagerPipe->SendHandleTransmissionStateChanged( microphoneEnabled, voiceActivated);
	/*if( !clientInstance)
		return;*/	
}

void CVoipManagerConaito::OnVoiceQualityChanged()
{
	if( connectionStatus == CS_DISCONNECTED ||
		connectionStatus == CS_DISCONNECTING)
		return;
	Disconnect();
	ConnectImpl( false);
}

void CVoipManagerConaito::OnConnectionStatus( int aConnectionStatus)
{	
	switch ( aConnectionStatus)
	{
	case CS_CONNECTED:
		{
			SendMessageToWnd( WM_EVOVOIP_CONNECTSUCCESS);			
			break;
		}		
	case CS_CONNECTFAILED:
		{
			SendMessageToWnd( WM_EVOVOIP_CONNECTFAILED);
			/*connectionStatus = CS_DISCONNECTED;
			CALLBACK_NO_PARAMS(onConnectFailed);*/
			break;
		}		
	case CS_LOSTCONNECTION:
		{
			SendMessageToWnd( WM_EVOVOIP_CONNECTIONLOST);
			/*connectionStatus = CS_DISCONNECTED;
			CALLBACK_NO_PARAMS(onConnectionLost);*/
			break;
		}
	}
}

//! Вызывается при невозможности соединения с сервером
LRESULT CVoipManagerConaito::OnConnectFailed()
{
	connectionStatus = CS_DISCONNECTED;
	CALLBACK_ONE_PARAM(onConnectFailed, serverIP.GetBuffer());
	return 0;
}

//! Вызывается при успешном соединении с сервером
LRESULT CVoipManagerConaito::OnConnectSuccess()
{
	connectionStatus = CS_CONNECTED;	
	m_voipManagerPipe->SendDoLogin( userName.AllocSysString(), userPassword.AllocSysString(), roomName.AllocSysString());
	
	if( context->mpLogWriter != NULL)
	{
		CComString str;
		str.Format("[VOIP]: OnConnectSuccess::SendDoLogin::userName = %s, bRestore %d", userName.GetBuffer(),  (int)m_voipManagerPipe->IsRestoringConnect());
		WriteLogLine( str.GetBuffer());
	}
	if( m_voipManagerPipe->IsRestoringConnect())
	{
		ChangeLocationImpl( roomName);
		OnConnectToEVoipServer();		
	}
	if (userName != "")
		m_voipManagerPipe->SetRestoredConnect();	
	return 0;
}

//! Вызывается при потери связи с сервером
LRESULT CVoipManagerConaito::OnConnectionLost()
{
	connectionStatus = CS_DISCONNECTED;
	CALLBACK_NO_PARAMS(onConnectionLost);
	if( context->mpLogWriter != NULL)
	{
		CComString str;
		str.Format("[VOIP]: OnConnectionLost");
		WriteLogLine( str.GetBuffer());
	}
	return 0;
}

//! Вызывается при выходе пользователя из сеанса
/*LRESULT CVoipManagerConaito::OnLoggedOut()
{
	return 0;
}*/

//! Вызывается при отказе сервера в обслуживании пользователя
LRESULT CVoipManagerConaito::OnKicked()
{
	CALLBACK_NO_PARAMS(onKicked);
	return 0;
}

//! Вызывается при обновлении данных с сервера
/*LRESULT CVoipManagerConaito::OnServerUpdate()
{
	return 0;
}*/

/*//! Вызывается 
LRESULT CVoipManagerConaito::OnCommandError(WPARAM wParam)
{
	return 0;
}*/

//! Вызывается при появлении нового пользователя в канале
LRESULT CVoipManagerConaito::OnAddUser(WPARAM wParam, LPARAM lParam)
{
	/*int aUserID = wParam;
	wchar_t *p = (wchar_t *)lParam;
	CComString sUserName = (BSTR)p;
	RegisterVoipAvatar( aUserID, sUserName.GetBuffer());*/
	//m_voipManagerPipe->SendSetUserVolume(aUserID, 255);
	return true;
}

//! Вызывается при обновлении данных по конкретному пользователю
/*LRESULT CVoipManagerConaito::OnUpdateUser(WPARAM wParam)
{
	return 0;
}*/

//! Вызывается при удалении пользователя из канала
LRESULT CVoipManagerConaito::OnRemoveUser(WPARAM wParam)
{
	//UnregisterVoipAvatar( wParam);
	return 0;
}

//! Вызывается при удалении подключении к каналу на сервере
LRESULT CVoipManagerConaito::OnChannelJoined(WPARAM wParam, LPARAM lParam)
{
	OnChannelJoined( (wchar_t *)lParam);
	return 0;
}

//! Вызывается при удалении подключении к каналу на сервере
void CVoipManagerConaito::OnChannelJoined(wchar_t *szChannelPath)
{
	channelConnectionStatus = CS_CONNECTED;
	CALLBACK_NO_PARAMS(onConnectSuccess);

	USES_CONVERSION;
	CVoipManager::OnChannelJoined( (char*)W2A(szChannelPath));
} 

//! Вызывается при удалении отключении от канала на сервере
LRESULT CVoipManagerConaito::OnChannelLeft( WPARAM wParam)
{
	channelConnectionStatus = CS_DISCONNECTED;
	if( context->mpLogWriter != NULL)
	{
		CComString str;
		str.Format("[VOIP]: OnChannelLeft");
		WriteLogLine( str.GetBuffer());
	}
	return 0;
}

LRESULT CVoipManagerConaito::OnUserTalking(WPARAM wParam, LPARAM lParam)
{
	int aUserID = wParam;
	wchar_t *p = (wchar_t *)lParam;
	CComString sUserName = (BSTR)p;

	CALLBACK_TWO_PARAMS( onUserTalking, sUserName.GetBuffer(), 0);
	return 0;
}

/*LRESULT CVoipManagerConaito::OnUserStoppedTalking(WPARAM wParam)
{
	User user = {0};
	BOOL b = GetEvoUserImpl( 3, wParam, &user);
	return 0;
}*/

LRESULT CVoipManagerConaito::OnUserStoppedTalking( WPARAM wParam, LPARAM lParam)
{	
	int aUserID = wParam;
	wchar_t *p = (wchar_t *)lParam;
	CComString sUserName = (BSTR)p;
	CALLBACK_TWO_PARAMS( onUserStoppedTalking, sUserName.GetBuffer(), 0);
	return 0;
}

void CVoipManagerConaito::SendConnectRequest( LPCSTR alpcVoipServerAddress, unsigned int aiVoipPort, LPCSTR alpcRecordServerAddress
								, unsigned int aiRecordPort, LPCSTR alpcRoomName)
{
	connectRequestParams.voipServerAddress = alpcVoipServerAddress;
	connectRequestParams.voipPort = aiVoipPort;
	connectRequestParams.recordServerAddress = alpcRecordServerAddress;
	connectRequestParams.recordPort = aiRecordPort;
	connectRequestParams.roomName = alpcRoomName;
	//HandleOnConnectRequest();
	PostMessage( voipWindowClient.m_hWnd, WM_VOIP_CONNECT_REQUEST, 0, 0);
}

void CVoipManagerConaito::HandleOnConnectRequest()
{
	if (context->mpComMan && !isSubscribedToNotify) 
	{
		isSubscribedToNotify = true;
		context->mpComMan->GetConnectionChanger()->SubscribeToNotify(this);
	}

	ConnectToVoipServer( connectRequestParams.voipServerAddress
						, connectRequestParams.voipPort
						, requestedRoomName
						, requestedUserName
						, requestedUserPassword
						, requestedAvatarID
						, connectRequestParams.recordServerAddress
						, connectRequestParams.recordPort);
}

//! Обработка изменения автоматической регулировки громкости
void CVoipManagerConaito::OnAutoVolumeChanged()
{
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
		// восстановим коэффициент усиления голоса
		OnGainLevelChanged();
		if( GetAudioSubsystemType() == AUDIO_SUBSYSTEM_XP)
		{
			CWTMicContoller::CDeviceName microphoneName = microphoneCtrl.GetDeviceMicrophoneName( currentEvoVoipDeviceGuid);
			microphoneCtrl.SetDeviceLineVolume( currentEvoVoipDeviceGuid, microphoneName, 0xFFFF);	//100%
		}
		else
		{
			//??
		}
	}
	else
	{
		if( GetAudioSubsystemType() == AUDIO_SUBSYSTEM_XP)
		{
			CWTMicContoller::CDeviceName mixerName = microphoneCtrl.GetDeviceMixerName( currentEvoVoipDeviceGuid);
			microphoneCtrl.SetDeviceLineVolume( currentEvoVoipDeviceGuid, mixerName, 0x3FFF);	//25%
		}
		else
		{
			//??
		}
	}
}

bool CVoipManagerConaito::SetUserVolume( const wchar_t* alwpcUserLogin, double adUserVolume)
{
	/*if( !clientInstance)
		return false;*/

	if( !alwpcUserLogin)
		return false;
	

	if( context->mpLogWriter)
	{	
		USES_CONVERSION;
		std::string sUser = W2A(alwpcUserLogin);
		CComString str;
		str.Format("[VOIP]: SetUserVolume userLogin=%s, volume=%d", sUser.c_str(), (int)(255*adUserVolume));
		WriteLogLine( str.GetBuffer());
	}

	m_voipManagerPipe->SendSetUserVolume( alwpcUserLogin, (int)adUserVolume);
	return true;
}

bool CVoipManagerConaito::SetIncomingVoiceVolume( double adUserVolume)
{
	m_voipManagerPipe->SendIncomingVoiceVolume((int)adUserVolume);
	return true;
}

bool CVoipManagerConaito::SetEchoCancellation(bool anEnabled)
{
	m_voipManagerPipe->SendEchoCancellation(anEnabled);
	return true;
}

bool CVoipManagerConaito::SetMixerEchoCancellation(bool anEnabled)
{
	m_voipManagerPipe->SendMixerEchoCancellation(anEnabled);
	return true;
}

bool CVoipManagerConaito::SetUserGainLevel( const wchar_t* alwpcUserLogin, double adUserGainLevel)
{
	/*if( !clientInstance)
		return false;*/ 

	if( !alwpcUserLogin)
		return false;	

	m_voipManagerPipe->SendSetUserGainLevel(alwpcUserLogin, adUserGainLevel*1000);
	return true;
	//EVO_SetUserGainLevel( clientInstance, evoUserID, adUserGainLevel*1000);
}

//! Автоматическое управление громкостью
bool CVoipManagerConaito::IsAutoGaneControlEnabled()
{	
	return agcEnabled;
}
bool CVoipManagerConaito::SetAutoGaneControlEnabled(bool abEnabled)
{
	agcEnabled = abEnabled;
	m_voipManagerPipe->SendSetAutoGaneControlEnabled( abEnabled);
	return true;
}
//! Автоматическое подавление шумов
bool CVoipManagerConaito::IsDenoisingEnabled()
{
	return denoisingEnabled;
}

bool CVoipManagerConaito::SetDenoisingEnabled(bool abEnabled)
{
	denoisingEnabled = abEnabled;

	m_voipManagerPipe->SendEnableDenoising( abEnabled);
	return true;
}

//! 
void CVoipManagerConaito::HandleVoiceData( int nUserID, int nSampleRate, const short* pRawAudio, int nSamples)
{
	if( m_userID == nUserID)
	{
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

				mutexCurrentInputLevel.lock();
				m_currentInputLevel = (m_currentInputLevel + ((double)calculatedDisperse - (double)calculatedMedian/nSamples) / m_currentModeCurrentTopInputLevel) / 2;
				mutexCurrentInputLevel.unlock();
			}
		}
		else
		{
			mutexCurrentInputLevel.lock();
			m_currentInputLevel /= 2;
			mutexCurrentInputLevel.unlock();
		}
	}

	CVoipManager::HandleVoiceData( nUserID, nSampleRate, pRawAudio, nSamples);
}

bool CVoipManagerConaito::IsVoipSystemInitialized()
{
	return bVoipSystemInitialized;
}

bool CVoipManagerConaito::IsGetInputDevices()
{ 
	if(inputDevicesInfo.size() > 0 /* && outputDevicesInfo.size()>0 */)
		return true;
	return false;
}

void CVoipManagerConaito::SetInputDevices( vecDeviceInfo *apInputDevicesInfo)
{
	inputDevicesInfo.clear();
	int nCount = apInputDevicesInfo->size();
	if( nCount > 0)
	{
		inputDevicesInfo.reserve( nCount);
		for( int i=0; i<nCount; i++)
		{
			inputDevicesInfo.push_back( apInputDevicesInfo->at(i));
		}
	}	
}

void CVoipManagerConaito::SetOutputDevices( vecDeviceInfo *apOutputDevicesInfo)
{
	outputDevicesInfo.clear();
	int nCount = apOutputDevicesInfo->size();
	if( nCount > 0)
	{
		outputDevicesInfo.reserve( nCount);
		for( int i=0; i<nCount; i++)
		{
			outputDevicesInfo.push_back( apOutputDevicesInfo->at(i));
		}
	}	
}

/* затычка через один два выпущенных релиза убрать*/
void CVoipManagerConaito::CorrectOldSettings()
{
	if( !bVoipSettingCorrect && context != NULL && context->mpRM != NULL)
	{
		/*
		CUserSettings uSettings;
		uSettings.LoadAll();
		std::string sGuid;
		std::string sVal = uSettings.GetSetting( "inputDeviceName");		
		USES_CONVERSION;
		if( sVal.size() > 2)
		{
			sGuid = W2A(GetDeviceGuidByName( A2W(sVal.c_str()), inputDevicesInfo).c_str());
			if ( !sGuid.empty())
			{			
				uSettings.SaveSetting( "inputDeviceGuid", sGuid);
			}
		}
		sVal = uSettings.GetSetting( "inputMixDeviceName");
		if(sVal.size() > 2)
		{
			sGuid = W2A(GetDeviceGuidByName( A2W(sVal.c_str()), inputDevicesInfo).c_str());
			if ( !sGuid.empty())
			{			
				uSettings.SaveSetting( "inputMixDeviceGuid", sGuid);
			}
		}
		uSettings.GetSetting( "outputDeviceName");
		if( sVal.size() < 2)
			return;
		sGuid = W2A(GetDeviceGuidByName( A2W(sVal.c_str()), outputDevicesInfo).c_str());
		if ( !sGuid.empty())
		{			
			uSettings.SaveSetting( "outputDeviceGuid", sGuid);
		}
		uSettings.SaveAllExludeGlobalSettings();
		*/
		std::wstring sGuid;
		char inputDeviceName[255];
		context->mpRM->GetUserSetting( L"inputDeviceName", inputDeviceName, 255);	
		USES_CONVERSION;
		if( strlen(inputDeviceName)>2)
		{
			sGuid = GetDeviceGuidByName( A2W(inputDeviceName), inputDevicesInfo);
			if ( !sGuid.empty())
			{			
				context->mpRM->SetUserSetting( L"inputDeviceGuid", (wchar_t*)sGuid.c_str());
				context->mpRM->SetUserSetting( L"inputDeviceName", L"");
			}
		}
		char inputMixDeviceName[255];
		context->mpRM->GetUserSetting( L"inputMixDeviceName", inputMixDeviceName, 255);
		if( strlen(inputMixDeviceName)>2)
		{
			sGuid = GetDeviceGuidByName( A2W(inputMixDeviceName), inputDevicesInfo);
			if ( !sGuid.empty())
			{			
				context->mpRM->SetUserSetting( L"inputMixDeviceGuid", (wchar_t*)sGuid.c_str());
				context->mpRM->SetUserSetting( L"inputMixDeviceName", L"");
			}
			else if( !strcmp(inputMixDeviceName, "Аудио Микшер") || !strcmp(inputMixDeviceName, "Audio Mixer"))
			{
				context->mpRM->SetUserSetting( L"inputMixDeviceGuid", A2W(inputMixDeviceName));
				context->mpRM->SetUserSetting( L"inputMixDeviceName", L"");
			}
		}
		char outputDeviceName[255];
		context->mpRM->GetUserSetting( L"outputDeviceName", outputDeviceName, 255);
		if( strlen(outputDeviceName)<2)
			return;
		sGuid = GetDeviceGuidByName( A2W(outputDeviceName), outputDevicesInfo);
		if ( !sGuid.empty())
		{			
			context->mpRM->SetUserSetting( L"outputDeviceGuid", (wchar_t*)sGuid.c_str());
			context->mpRM->SetUserSetting( L"outputDeviceName", L"");
		}
		else if( !strcmp(outputDeviceName, "Устройство Windows по умолчанию") || !strcmp(outputDeviceName, "Default Windows device"))
		{
			context->mpRM->SetUserSetting( L"outputDeviceGuid", L"Default device");
			context->mpRM->SetUserSetting( L"outputDeviceName", L"");
		}
		bVoipSettingCorrect = true;
	}
}

void CVoipManagerConaito::SetVoipSystemInitialized( bool abVoipSystemInitialized)
{
	bVoipSystemInitialized = abVoipSystemInitialized;
}

void CVoipManagerConaito::SendMessageToWnd( unsigned int auMSG, unsigned int data1, const wchar_t *data2)
{
	SendMessage( voipWindowClient.m_hWnd, auMSG, (WPARAM)data1, (LPARAM)data2);
}

oms::omsContext *CVoipManagerConaito::GetOMSContext()
{
	return context;
}

// Возвращает имя устройства по его EVO_VOIP номеру. В случае, если такого устройства нет в системе - возвращает ""
std::wstring CVoipManagerConaito::GetDeviceGuidByEvoVoidID( int aiDeviceID)
{	
	vecDeviceInfoIt it = inputDevicesInfo.begin(), end = inputDevicesInfo.end();
	for (; it!=end; ++it)
	{
		if( aiDeviceID == it->nDeviceID)
			return it->sGUID;
	}

	return L"";
}

std::wstring CVoipManagerConaito::GetDeviceNameByGuid( const wchar_t* alpcDeviceGuid)
{	
	if( alpcDeviceGuid == NULL || wcslen(alpcDeviceGuid) == NULL)
		return L"";
	vecDeviceInfoIt it = inputDevicesInfo.begin(), end = inputDevicesInfo.end();
	for (; it!=end; ++it)
	{
		if( wcslen(it->sGUID) > 0 && _wcsnicmp( alpcDeviceGuid, it->sGUID, min(wcslen(alpcDeviceGuid), wcslen(it->sGUID))) == 0)
			return it->szDeviceName;
	}

	return L"";
}

std::wstring CVoipManagerConaito::GetDeviceGuidByName( const wchar_t* alpcDeviceName, std::vector<SoundDevice> &aInputDevicesInfo)
{	
	if( alpcDeviceName == NULL || wcslen(alpcDeviceName) == NULL)
		return L"";
	vecDeviceInfoIt it = aInputDevicesInfo.begin(), end = aInputDevicesInfo.end();
	for (; it!=end; ++it)
	{
		if( wcslen(it->szDeviceName) > 0 && _wcsnicmp( alpcDeviceName, it->szDeviceName, min(wcslen(alpcDeviceName), wcslen(it->szDeviceName))) == 0)
			return it->sGUID;
	}

	return L"";
}

int CVoipManagerConaito::GetEvoVoipDeviceIDByGuid( const wchar_t* alpcDeviceGuid)
{	
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

bool CVoipManagerConaito::CheckEnableCurrentDevice()
{
	if( GetEvoVoipDeviceIDByGuid( currentEvoVoipDeviceGuid.c_str()) == -1)
		return false;
	return true;
}

bool CVoipManagerConaito::IsEqualProxySettings(std::vector<IProxySettings*>* aProxyInfoList1, std::vector<IProxySettings*>* aProxyInfoList2)
{
	if (aProxyInfoList1->size() != aProxyInfoList2->size())
		return false;

	std::vector<IProxySettings*>::iterator it1 = aProxyInfoList1->begin();
	std::vector<IProxySettings*>::iterator itEnd = aProxyInfoList1->end();
	std::vector<IProxySettings*>::iterator it2 = aProxyInfoList2->begin();

	for (; it1 != itEnd; it1++, it2++)
	{
		if ((*it1)->getType() != (*it2)->getType())
			return false;
		if ((*it1)->getPort() != (*it2)->getPort())
			return false;
		if (wcscmp((*it1)->getIP(), (*it2)->getIP()))
			return false;
		if (wcscmp((*it1)->getName(), (*it2)->getName()))
			return false;
		if (wcscmp((*it1)->getUser(), (*it2)->getUser()))
			return false;
		if (wcscmp((*it1)->getPassword(), (*it2)->getPassword()))
			return false;
	}

	return true;
}

void CVoipManagerConaito::OnChangeConnectionType(void* aProxyInfo)
{
	if( context->mpLogWriter != NULL)
	{
		CComString str;
		str.Format("[VOIP]: OnChangeConnectionType 1 - Proxy");
		WriteLogLine( str.GetBuffer());
	}
	if (!aProxyInfo)
		return;

	bool changedProxySettings = false;

	std::vector<IProxySettings*>* proxyInfoList = (std::vector<IProxySettings*>*)aProxyInfo;
	std::vector<IProxySettings*>::iterator it = proxyInfoList->begin();
	if (it != proxyInfoList->end())
	{
		changeProxyListMutex.lock();
		if ((*it)->getType() == PT_HTTP || (*it)->getType() == PT_HTTPS) {
			changedProxySettings = !IsEqualProxySettings(&httpsProxySettingsVec, proxyInfoList);
			ProxySettingsList::clearProxySettingsList(&httpsProxySettingsVec);
			ProxySettingsList::copyProxySettingsList(proxyInfoList, &httpsProxySettingsVec);
		}
		else if ((*it)->getType() == PT_SOCKS4 || (*it)->getType() == PT_SOCKS5) {
			changedProxySettings = !IsEqualProxySettings(&socksProxySettingsVec, proxyInfoList);
			ProxySettingsList::clearProxySettingsList(&socksProxySettingsVec);
			ProxySettingsList::copyProxySettingsList(proxyInfoList, &socksProxySettingsVec);
		}
		changeProxyListMutex.unlock();
	}	
	else
		return;

	if (changedProxySettings) {
		if( context->mpLogWriter != NULL)
		{
			CComString str;
			str.Format("[VOIP]: OnChangeConnectionType reconnect");
			WriteLogLine( str.GetBuffer());
		}
//need reconnect only voipman
		DisconnectImpl();
		ConnectImpl(false);
	}
}

std::wstring CVoipManagerConaito::TryGetCurrentInDeviceGuid( bool aMicrophone/*, SoundDevice &sndDevice*/)
{
	CWTMicContoller micController;
	std::wstring currentDevice;
	
	int audioSystem = GetAudioSubsystemType();

	if( audioSystem == AUDIO_SUBSYSTEM_XP)
		currentDevice = micController.GetCurrentInDeviceGuid( true);

	if ((currentDevice.empty() || !_wcsicmp(currentDevice.c_str(), L"default device")) && inputDevicesInfo.size() > 0)
	{
		if( audioSystem > AUDIO_SUBSYSTEM_XP)
		{
			wchar_t alpcBuffer[1024] = {0};
			mutexAudioDevice.lock();
			if( m_pDefaultAudioDeviceManager == NULL)
				m_pDefaultAudioDeviceManager = MP_NEW( CDefaultAudioDeviceManager);
			m_pDefaultAudioDeviceManager->GetDefaultInDeviceGuid( alpcBuffer, countof( alpcBuffer));
			mutexAudioDevice.unlock();
			//CDefaultAudioDeviceManager defaultAudioDeviceManager; 
			//VOIPAC_ERROR errorCode = ACERROR_NOERROR;			
			if( aMicrophone)
			{
				std::vector<std::wstring> micNames;
				mutexAudioDevice.lock();
				m_pDefaultAudioDeviceManager->GetMicrophones( micNames);
				mutexAudioDevice.unlock();
				if( !DeviceIsIntoArrayName( alpcBuffer, micNames))
				{	
					if( micNames.size() > 0)
					{
						ZeroMemory(alpcBuffer, 1024* sizeof(wchar_t));
						wcscpy_s( alpcBuffer, 1024, micNames.begin()->c_str());
					}					
					else 
					{
						WriteLogLine( "[VOIP]: TryGetCurrentInDeviceGuid::microphone is not found");
						std::vector<CWTMicContoller::CDeviceName> av;
						mutexAudioDevice.lock();
						m_pDefaultAudioDeviceManager->GetInDevices( av);
						mutexAudioDevice.unlock();
						std::vector<CWTMicContoller::CDeviceName>::iterator iter = av.begin();
						for( ; iter != av.end(); iter++)
						{
							if( !micController.IsMixerName( iter->c_str()))
							{
								WriteLogLine( "[VOIP]: TryGetCurrentInDeviceGuid:: set first is not mixer InDevice");
								//errorCode = ACERROR_NOERROR;
								ZeroMemory(alpcBuffer, 1024 * sizeof(wchar_t));
								wcscpy_s( alpcBuffer, 1024, iter->c_str());
							}
						}
					}
				}
			}
			else
			{
				if( !micController.IsMixerName( alpcBuffer))
				{
					std::vector<std::wstring> mixNames;
					mutexAudioDevice.lock();
					m_pDefaultAudioDeviceManager->GetNoMicInDevices( mixNames);
					mutexAudioDevice.unlock();
					std::vector<std::wstring>::iterator iter = mixNames.begin();
					for( ; iter != mixNames.end(); iter++)
					{
						if( micController.IsMixerName( iter->c_str()))
						{
							ZeroMemory(alpcBuffer, 1024 * sizeof(wchar_t));
							wcscpy_s( alpcBuffer, 1024, iter->c_str());
							break;
						}
					}
					if(  mixNames.size() > 0 && wcslen( alpcBuffer) == 0)
					{
						WriteLogLine( "[VOIP]: TryGetCurrentInDeviceGuid::mixer is not found");
						ZeroMemory(alpcBuffer, 1024 * sizeof(wchar_t));
						wcscpy_s( alpcBuffer, 1024, mixNames.begin()->c_str());
					}
				}			
			}
			/*if( errorCode == ACERROR_NOERROR)
			{*/
				currentDevice = alpcBuffer;
			//}
		}

		if ((currentDevice.empty() || !_wcsicmp(currentDevice.c_str(), L"default device")) && inputDevicesInfo.size() > 0)
		{
			if ( inputDevicesInfo.size() > 1)
				currentDevice = inputDevicesInfo[1].sGUID;		
			else
				currentDevice = inputDevicesInfo[0].sGUID;
		}
	}
	
	CComString str;
	USES_CONVERSION;
	str.Format("[VOIP]: TryGetCurrentInDeviceGuid::currentDevice = %s", W2A(currentDevice.c_str()));
	WriteLogLine( str.GetBuffer());
	return currentDevice;
}

bool CVoipManagerConaito::IsMixerSet()
{
	CWTMicContoller micController;
	int audioSystem = GetAudioSubsystemType();
	if( audioSystem == AUDIO_SUBSYSTEM_XP)
	{
		return micController.IsMixerName( currentEvoVoipMixLineName.c_str());
	}
	return true;
	//return micController.IsMixerName( currentMixDeviceGuid.c_str());
}

std::string CVoipManagerConaito::ReadRegKey(std::string path, std::string key)
{	 
	HKEY rKey;
	char str[1024];	
	DWORD strSize = sizeof(str);
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, path.c_str(), 0, KEY_QUERY_VALUE, &rKey) != ERROR_SUCCESS)
	{		
		return "";
	}
	if (RegQueryValueEx(rKey, key.c_str(), NULL, NULL, (LPBYTE)&str[0], &strSize) != ERROR_SUCCESS)
	{		
		return "";
	}
	if (RegCloseKey(rKey) != ERROR_SUCCESS)
	{		
		return "";
	}

	std::string s = str;

	return s;
}

bool CVoipManagerConaito::LogSystemDrivers()
{
	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA DeviceInfoData;
	DWORD i;


	if( context->mpLogWriter != NULL)
	{
		CComString str;
		str.Format("List of drivers :");
		WriteLogLine( str.GetBuffer());
	}

	// Create a HDEVINFO with all present devices.
	hDevInfo = SetupDiGetClassDevs(NULL,
		0, // Enumerator
		0,
		DIGCF_PRESENT | DIGCF_ALLCLASSES );

	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	// Enumerate through all devices in Set.
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	for (i=0;SetupDiEnumDeviceInfo(hDevInfo,i,
		&DeviceInfoData);i++)
	{
		DWORD DataT;
		char* buffer = NULL;
		DWORD buffersize = 0;

		while (!SetupDiGetDeviceRegistryProperty(
			hDevInfo,
			&DeviceInfoData,
			SPDRP_DRIVER,
			&DataT,
			(PBYTE)buffer,
			buffersize,
			&buffersize))
		{
			if (GetLastError() == 
				ERROR_INSUFFICIENT_BUFFER)
			{
				// Change the buffer size.
				if (buffer) 
					 MP_DELETE_ARR(buffer);
					//LocalFree(buffer);
				// Double the size to avoid problems on 
				// W2k MBCS systems per KB 888609. 
				buffer = MP_NEW_ARR(char, buffersize * 2);
				//buffer = (LPTSTR)LocalAlloc(LPTR,buffersize * 2);
			}
			else
			{
				break;
			}
		}
		if (buffer != NULL)
		{
			//path from msdn
			std::string regPath = "SYSTEM\\CurrentControlSet\\Control\\Class\\";
			regPath += buffer;
			
			std::string ver = ReadRegKey(regPath, "DriverVersion");
			std::string desc = ReadRegKey(regPath, "DriverDesc");

			if( context->mpLogWriter != NULL)
			{
				if (ver != "" && desc != "")
				{
					CComString str;
					str.Format("Device = %s, Driver version %s", desc.c_str(), ver.c_str());
					WriteLogLine( str.GetBuffer());
				}
			}	
		}

		if (buffer) 
			MP_DELETE_ARR(buffer);
	}

	if ( GetLastError()!=NO_ERROR &&
		GetLastError()!=ERROR_NO_MORE_ITEMS )
	{
		return false;
	}

	//  Cleanup
	SetupDiDestroyDeviceInfoList(hDevInfo);

	return true;
}

DWORD WINAPI ThreadCheckAudioDevices( void *lpvParam)
{
	((CVoipManagerConaito*)lpvParam)->CheckAudioDevices();	
	return 0;
}

void CVoipManagerConaito::StartCheckAudioDevices()
{	
	if( !m_bCheckAudioDevices || m_eventCheckAudioDevices != INVALID_HANDLE_VALUE)
		return;
	m_eventCheckAudioDevices = CreateEvent( NULL, FALSE, FALSE, "eventCheckAudioDevices");
	CSimpleThreadStarter checkAudioDevicesThreadStarter(__FUNCTION__);
	checkAudioDevicesThreadStarter.SetRoutine(ThreadCheckAudioDevices);
	checkAudioDevicesThreadStarter.SetParameter(this);
	m_hCheckAudioDevicesThread = checkAudioDevicesThreadStarter.Start();
	if (m_hCheckAudioDevicesThread == NULL)
	{
		CComString sLog;
		sLog.Format( "[VOIP] Critical error. CreateThread failed. Error code is %u", GetLastError());
		WriteLogLine( sLog.GetBuffer());
	}
	else
		m_checkAudioDevicesThreadId = checkAudioDevicesThreadStarter.GetThreadID();	
}

void CVoipManagerConaito::CheckAudioDevices()
{
	int winNum = -1;	
	while( m_bCheckAudioDevices)
	{
		winNum = GetAudioSubsystemType();
		if(winNum == -1)
		{
			Sleep(1000);
			continue;
		}
		if( winNum < AUDIO_SUBSYSTEM_XP)
			return;
		break;	
	}
	int sizeInPrev = -1;
	int sizeOutPrev = -1;
	CDSMixer2 *pdsMixer2 = NULL;
	if( winNum == AUDIO_SUBSYSTEM_XP)
	{
		CDSMixer2 *pdsMixer2 = new CDSMixer2();
		sizeInPrev = pdsMixer2->GetInDevices().size();
		sizeOutPrev = pdsMixer2->GetOutDevices().size();
		delete pdsMixer2;
	}
	else
	{
		mutexAudioDevice.lock();
		if( m_pDefaultAudioDeviceManager != NULL)
			MP_DELETE( m_pDefaultAudioDeviceManager);
		m_pDefaultAudioDeviceManager = MP_NEW( CDefaultAudioDeviceManager);
		mutexAudioDevice.unlock();
		sizeInPrev = m_pDefaultAudioDeviceManager->GetCountInDevices();
		sizeOutPrev = m_pDefaultAudioDeviceManager->GetCountOutDevices();
	}		

	int sizeIn = 0;
	int sizeOut = 0;
	std::wstring sNull = L"";
	std::wstring sDefaultDevice = L"Default device";
	bool bSetInputMicDevice = false;
	bool bSetInputMixDevice = false;
	bool bSetOutDevice = false;

	while( m_bCheckAudioDevices)
	{		
		WaitForSingleObject( m_eventCheckAudioDevices, 15000);		
		bSetInputMicDevice = false;
		bSetInputMixDevice = false;
		bSetOutDevice = false;
		if( winNum == AUDIO_SUBSYSTEM_XP)
		{
			pdsMixer2 = new CDSMixer2();
			sizeIn = pdsMixer2->GetInDevices().size();
			sizeOut = pdsMixer2->GetOutDevices().size();
			delete pdsMixer2;
		}
		else
		{
			mutexAudioDevice.lock();
			if( m_pDefaultAudioDeviceManager != NULL)
				MP_DELETE( m_pDefaultAudioDeviceManager);
			m_pDefaultAudioDeviceManager = MP_NEW( CDefaultAudioDeviceManager);
			mutexAudioDevice.unlock();
			sizeIn = m_pDefaultAudioDeviceManager->GetCountInDevices();
			sizeOut = m_pDefaultAudioDeviceManager->GetCountOutDevices();
			CWTMicContoller::CDevicesNamesVector inDeviceNames;
			CWTMicContoller::CDevicesNamesVector outDeviceNames;
			m_pDefaultAudioDeviceManager->GetInDevices( inDeviceNames);
			m_pDefaultAudioDeviceManager->GetOutDevices( outDeviceNames);
			if( !DeviceIsIntoArrayName( currentMicrophoneDeviceGuid, inDeviceNames))
			{
				currentMicrophoneDeviceGuid = L"";
				bSetInputMicDevice = true;
			}
			if( !DeviceIsIntoArrayName( currentMixDeviceGuid, inDeviceNames))
			{
				currentMixDeviceGuid = L"";
				bSetInputMixDevice = true;
			}
			if( currentEvoVoipOutDeviceGuid != L"Default device" && !DeviceIsIntoArrayName( currentEvoVoipOutDeviceGuid, outDeviceNames))
			{
				outputDeviceWasChanged = true;
				currentEvoVoipOutDeviceGuid = L"";
				if( !bSetInputMicDevice && !bSetInputMixDevice)
					bSetOutDevice = true;
			}
		}		
		if( context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLn( "[VOIP] CheckAudioDevices::sizeInPrev==", sizeInPrev, "  sizeOutPrev==", sizeOutPrev, "  sizeIn==", sizeIn, "  sizeOut==", sizeOut);
		if( sizeIn != sizeInPrev || sizeOutPrev != sizeOut)
		{
			if( sizeIn != sizeInPrev)
				WriteLogLine( "[VOIP] CheckAudioDevices::change count input device");
			if( sizeOutPrev != sizeOut)
				WriteLogLine( "[VO IP] CheckAudioDevices::change count output device");
			m_voipManagerPipe->SendGetDevices( true, true);			
		}		
		if( sizeInPrev == 0 && sizeIn > 0)
		{
			WriteLogLine( "[VOIP] CheckAudioDevices::appear input audio device");			
			if( audioSource == VAS_MIXER)
			{
				SetInputMixerMode( L"", L"", 0);	
			}
			else //if( audioSource == VAS_MICROPHONE)
			{
				SetInputDeviceMode( L"", L"", 0);
			}
		}
		else
		{
			if( bSetInputMicDevice || bSetInputMixDevice)
			{
				WriteLogLine( "[VOIP] CheckAudioDevices::change input audio device");			
				if( audioSource == VAS_MIXER)
				{
					if( bSetInputMixDevice)
						SetInputMixerMode( L"", L"", 0);
					if( bSetInputMicDevice && winNum != AUDIO_SUBSYSTEM_XP)
						OnSetInputDeviceMode( sDefaultDevice, sNull, 0, 0);
				}
				else //if( audioSource == VAS_MICROPHONE)
				{
					if( bSetInputMicDevice)
						SetInputDeviceMode( L"", L"", 0);
					if( bSetInputMixDevice && winNum != AUDIO_SUBSYSTEM_XP)
						OnSetInputMixerMode( sDefaultDevice, sNull, 0, 0);
				}
			}			
			if( bSetOutDevice)
			{
				WriteLogLine( "[VOIP] CheckAudioDevices::change output audio device");
				if( audioSource == VAS_MIXER)
					SetInputMixerMode( currentMixDeviceGuid.c_str(), currentEvoVoipMixLineName.c_str(), 0);
				else
					SetInputDeviceMode( currentMicrophoneDeviceGuid.c_str(), currentEvoVoipLineName.c_str(), 0);
			}
			if( bSetInputMicDevice || bSetInputMixDevice ||  bSetOutDevice)
				m_voipManagerPipe->SendGetDevices( true, false);
		}
		sizeInPrev = sizeIn;
		sizeOutPrev = sizeOut;
	}
}

bool CVoipManagerConaito::StartFindRealMicrophon()
{
	m_voipManagerPipe->SendStartFindRealMicrophon();
	return true;
}

bool CVoipManagerConaito::EndFindRealMicrophon()
{
	m_voipManagerPipe->SendEndFindRealMicrophon();
	return true;
}