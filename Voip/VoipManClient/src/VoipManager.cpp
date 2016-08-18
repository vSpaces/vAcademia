#include "StdAfx.h"
#include <algorithm>
#include <atldef.h>
#include "..\include\voipmanager.h"
#include "soundLog\MP3Encoder.h"
#include "..\include\Protocol\VoipServersQueryIn.h"
#include "..\include\Protocol\VoipServerRequestQueryOut.h"
#include "..\include\Protocol\RegisterRecordQueryOut.h"
#include "..\include\RecordServer\Protocol\FileCreatedQueryIn.h"
#include "..\include\RecordServer\Protocol\RecordStartedQueryIn.h"
#include "..\include\RecordServer\Protocol\RecordPlayFinishedIn.h"
#include "..\include\RecordServer\Protocol\RecordFinishedIn.h"
#include "..\include\RecordServer\Protocol\AudioPlayedQueryIn.h"
#include "..\include\RecordServer\Protocol\MixedAudioCreatedQueryIn.h"
#include "..\include\RecordServer\Protocol\PlaySessionConnectedQueryIn.h"
#include "..\include\RecordServer\Protocol\AudioFilesInfoQueryIn.h"
#include "..\include\RecordServer\Protocol\EvoClientWorkStateChangedQueryIn.h"
#include "..\include\RecordServer\Protocol\NotifiedQueryIn.h"

#include "..\include\Win7EmptyDevicesStub.h"
#include "ServiceMan.h"
#include "ILogWriter.h"

#include "DeviceList\WTMicContoller.h"

#include "..\micRecorder\micRecorder.h"
#include "..\include\Voip3DSoundCalcManager.h"
#include "DataBuffer2.h"

#include "rm.h"

/*
#ifdef BUILD_H

HMODULE LoadDLL( LPCSTR alpcRelPath, LPCSTR alpcName)
{
	CComString sModuleDirectory;
	GetModuleFileName( NULL, sModuleDirectory.GetBufferSetLength(MAX_PATH), MAX_PATH);

	CComString sCurrentDirectory;
	GetCurrentDirectory( MAX_PATH, sCurrentDirectory.GetBufferSetLength(MAX_PATH));
	
	sModuleDirectory.Replace("\\", "/");
	sModuleDirectory.Replace("//", "/");
	int pos = sModuleDirectory.ReverseFind('//');
	if(pos>-1)
	{
		sModuleDirectory = sModuleDirectory.Left(pos);
	}
	CComString sPath = sModuleDirectory + "/" + alpcName;
	sPath.Replace("\\", "/");
	sPath.Replace("//", "/");
	HMODULE handle = ::LoadLibrary( sPath);
	SetCurrentDirectory( sCurrentDirectory);
	return handle;
}

#endif
*/

CVoipManager::CVoipManager(oms::omsContext* aContext) :
	mUseDS( false)
	, micGainLevel( 0.0f)
	, micActivationLevel( 0.2f)
	, recFreq( 8000)
	, voiceQuality( 0.3f)
	, m_iLogMode( 1)
	, voiceActivated( false)
	, autoVolume( true)
	, audioSource( VAS_MICROPHONE)
	, MP_VECTOR_INIT(vecCallbacks)
	,MP_VECTOR_INIT(httpsProxySettingsVec)
	,MP_VECTOR_INIT(socksProxySettingsVec)
#if VOIPMAN_BUILD_L
	, m_pUser3DPositioning( NULL)
	, MP_MAP_INIT(mapVoip3DSoundCalcManagerID)
#endif
{
	MP_NEW_V3(recordServerConnection, CRecordServerConnection, aContext, this, this);
	MP_NEW_V3(playServerConnection, CPlayServerConnection, aContext, this, this);
	MP_NEW_V3(selfRecordingServerConnection, CSelfRecordServerConnection, aContext, this, this);
	csVecCallbacks.Init();
	context = aContext;	
	bTerminating = FALSE;
	//recordIsOnGoing = false;
	selfRecordingIsOnGoing = VSR_DISCONNECTED;
	selfRecordingChannelConnected = false;

	m_pSndData2lex = MP_NEW( CDataBuffer2);
	CWin7EmptyDevicesStub::MakeExceptOneDeviceEnabled( aContext->mpLogWriter);
	voipServerPort = 0;
	m_currentRecordID = 0;

	requestedUserPassword = "vacadem";
}

CVoipManager::~CVoipManager(void)
{
#if VOIPMAN_BUILD_L
	MapVoip3DSoundCalcManagerID::iterator it = mapVoip3DSoundCalcManagerID.begin(), end = mapVoip3DSoundCalcManagerID.end();
	for ( ; it != end; it++)
	{
		CVoip3DSoundCalcManager *pVoip3DSoundCalcManager = (CVoip3DSoundCalcManager*) it->second;
		if (pVoip3DSoundCalcManager)
			MP_DELETE(pVoip3DSoundCalcManager);
	}
	mapVoip3DSoundCalcManagerID.clear();
	m_pUser3DPositioning = NULL;
#endif	
	if( m_pSndData2lex != NULL)
		MP_DELETE( m_pSndData2lex);

	MP_DELETE(recordServerConnection);
	MP_DELETE(playServerConnection);
	MP_DELETE(selfRecordingServerConnection);
	csVecCallbacks.Term();
}

/*
Вычисляет имя локации. Если auRealityID == -1, то имя канала принимаем за имя локации
*/
CComString	CVoipManager::EvaluateVoiceChannelName( LPCSTR alpcLocationName, unsigned int auRealityID)
{
	if( !alpcLocationName)
		return "Island0";

	if( auRealityID == SKIP_REALITY_ID)
		return alpcLocationName;

	CComString voiceChannelName;
	if( *alpcLocationName == 0)	// пустая строка
		voiceChannelName.Format("NonLocation%u", auRealityID);
	else
		voiceChannelName.Format("%s%u", alpcLocationName, auRealityID);

	return voiceChannelName;
}

//! Подключиться к группе общения голосовой связи
voipError	CVoipManager::Connect( LPCSTR alpcLocationName, unsigned int auRealityID, LPCSTR alpcUserName, LPCSTR alpcPassword, long long alAvatarID)
{
	if( !context)	
		return ERROR_CONTEXT_MISSED;

	if( context->mpLogWriter)
	{
		CComString str;
		str.Format("[VOIP]: Connecting to: alpcGroupName = %s, alpcUserName = %s, selfRecordingIsOnGoing=%u", alpcLocationName, alpcUserName, selfRecordingIsOnGoing);
		WriteLogLine( str.GetBuffer());
	}

	
	if( selfRecordingIsOnGoing != VSR_DISCONNECTED)
	{
		if( context->mpLogWriter)
			WriteLogLine("[VOIP]: SelfRecordingMode 51");

		// если мы тестим свой голос - запомним параметры внешнего подключения. 
		// После выхода из SR режима мы их применим
		/*inSRModeRequestedRoomName = alpcGroupName;
		inSRModeRequestedUserName = alpcUserName;
		inSRModeRequestedUserPassword = alpcPassword;
		inSRModeRequestedAvatarID = alAvatarID;
		return ERROR_NOERROR;*/
	}
	else
	{
		if( context->mpLogWriter)
			WriteLogLine("[VOIP]: SelfRecordingMode 52");

		requestedRoomName = EvaluateVoiceChannelName(alpcLocationName, auRealityID);
		requestedUserName = alpcUserName;
		requestedUserPassword = alpcPassword;
		requestedAvatarID = alAvatarID;
	}

	if( !context->mpServiceMan)
		return ERROR_NOTINITIALIZED;

	if( context->mpLogWriter)
		WriteLogLine("[VOIP]: SelfRecordingMode 53");

	context->mpServiceMan->SendVoipServerRequest( EvaluateVoiceChannelName(alpcLocationName, auRealityID).GetBuffer());

	return ERROR_NOERROR;
}

//! Подключиться к определенному серверу голосовой связи
void	CVoipManager::OnConnectToEVoipServer()
{
	// 
	//! В режиме записи записывающий сервер не меняем
	if( context->mpLogWriter)
		WriteLogLine("[VOIP]: SelfRecordingMode 57");
	if( selfRecordingIsOnGoing == VSR_DISCONNECTED)
	{
		if( context->mpLogWriter)
			WriteLogLine("[VOIP]: SelfRecordingMode 58");
		if( !recordServerConnection->IsRecordingInProgress())
		{
			if( context->mpLogWriter)
				WriteLogLine("[VOIP]: SelfRecordingMode 59");
			if( recordServerConnection->Connect( sRecordServer, uiRecordServerPort) == ERROR_NOERROR)
			{
				//??
			}
		}
		else
		{
			//if( context->mpLogWriter)
			//	WriteLogLine("[VOIP]: SelfRecordingMode 60");
			//recordServerConnection->ResumeRecording( serverIP, roomName);
		}
	}
	else
	{
		if( context->mpLogWriter)
			WriteLogLine("[VOIP]: SelfRecordingMode 61");
		if( selfRecordingServerConnection->Connect( sRecordServer, uiRecordServerPort) == ERROR_NOERROR)
		{
			//??
		}
	}

	//! В режиме воспроизведения пока следуем по локациям за пользователем
	/*if( playServerConnection->IsPlayingInProgress())
	{
		if( context->mpLogWriter)
		{
			CComString str;
			str.Format("[VOIP]: ConnectToVoipServer::ChangeLocation Current serverIP = %s, roomName = %s", alpcServer, alpcGroupName);
			WriteLogLine(str.GetBuffer());
		}
		playServerConnection->ChangeLocation( alpcServer, alpcGroupName);
	}*/
}

//! Подключиться к определенному серверу голосовой связи
voipError	CVoipManager::ConnectToVoipServer( LPCSTR alpcServer, unsigned int auiVoipServerPort, LPCSTR alpcGroupName
									, LPCSTR alpcUserName, LPCSTR alpcPassword, long long aiAvatarID
									, LPCSTR alpcRecordServer, unsigned int auiRecordServerPort)
{
	if( context->mpLogWriter)
	{
		CComString str;
		str.Format("[VOIP]: SelfRecordingMode 56 roomName = %s", alpcGroupName);
		WriteLogLine(str.GetBuffer());
	}
	// на это сервере все равно голосовой связи никогда не бывает,
	// а на коннекте подвисает - nap
	/*if (strcmp(alpcServer, "192.168.0.77") == 0)
	{
		return ERROR_NOERROR;
	}

	if (strcmp(alpcServer, "192.168.0.78") == 0)
	{
		return ERROR_NOERROR;
	}*/

	bool	bCheckConnecting = false;
	bool	serverSwitchNeeded = true;
	// уже подключены к серверу
	if( serverIP == alpcServer && userName == alpcUserName && userPassword == alpcPassword)
	{
		bCheckConnecting = (roomName == alpcGroupName);
		if( bCheckConnecting)
		{
			if( connectionStatus != CS_DISCONNECTED
				&& connectionStatus != CS_DISCONNECTING)
			{
				OnChannelJoined( alpcGroupName);
//если по Joined to channel пришло entered not in self channel
//может случаться если задержалась подтверждение с звука на выход из bunkerOf_* канала
				if (selfRecordingIsOnGoing == VSR_CONNECTING)
					OnConnectToEVoipServer();
				return ERROR_NOERROR;
			}
		}
		else
			serverSwitchNeeded = false;
	}

	serverIP = alpcServer;
	roomName = alpcGroupName;
	userName = alpcUserName;
	userPassword = alpcPassword;
	voipServerPort = auiVoipServerPort;

	if( context->mpLogWriter)
	{
		CComString str;
		str.Format("[VOIP]: SelfRecordingMode 56 2 roomName = %s", alpcGroupName);
		WriteLogLine(str.GetBuffer());
	}

	if( serverSwitchNeeded)
	{		
		if(!bCheckConnecting)
			Disconnect();

		sRecordServer = alpcRecordServer;
		uiRecordServerPort = auiRecordServerPort;
		voipError error = ConnectImpl( bCheckConnecting);
		/*if( error != ERROR_NOERROR)
			return error;

		// ! Connection established
		CALLBACK_NO_PARAMS(onConnectStart);*/
	}
	else
	{
		ChangeLocationImpl( roomName);
		OnConnectToEVoipServer();
	}
	

	return ERROR_NOERROR;
}

//! Восстанавливает подключение к текущему серверу голосовой связи
voipError	CVoipManager::RestoreConnection()
{
	if( connectionStatus == CS_CONNECTING ||
		connectionStatus == CS_CONNECTED)
		return ERROR_NOERROR;

	return ConnectImpl( false);
}

//! Отключиться от текущего сервера голосовой связи
voipError	CVoipManager::Disconnect()
{
	// В режиме записи записывающий сервер не отключаем
	if(!recordServerConnection->IsRecordingInProgress())
		recordServerConnection->Disconnect();
	//else
	//	recordServerConnection->PauseRecording();

	selfRecordingServerConnection->Disconnect();

	return DisconnectImpl();
}

//! Создать говорящего бота с номером
voipError	CVoipManager::CreateServerTalkingBot( unsigned int aClientSideObjectID)
{
	return ERROR_NOERROR;
}

//! Удалить говорящего бота с номером
voipError	CVoipManager::DestroyServerTalkingBot( unsigned int aClientSideObjectID)
{
	return ERROR_NOERROR;
}

//! Удалить менеджер голосовой связи
void CVoipManager::Release()
{
	Destroy();
	MP_DELETE_THIS;
}

//! использовать ли DirectSound для воспроизведения звука
//! значение по умолчанию: 1
void CVoipManager::SetDirectSoundUsed( bool val)
{
	if( mUseDS == val)
		return;
	mUseDS = val;
	OnDirectSoundUsedChanged();
}


//! уровень усиления сигнала с микрофона. Диапазон значений 100-4000. 1000 - усиления нет. 4000 - x4, 100-x0.1
//! значение по умолчанию: 1000
void CVoipManager::SetMicGainLevel(float val)
{
	if( micGainLevel == val)
		return;
	micGainLevel = val;
	OnGainLevelChanged();
}

//! уровень начала передачи сигнала. Диапазон значений 0-20. 
//! 0 - сигнал передается полностью, 10 - сигнал передается тольно если он сильнее чем 50% громкости, 
//! 20 - сигнал передаваться не будет
//! значение по умолчанию: 5
void CVoipManager::SetMicActivationLevel(float val)
{
	if( micActivationLevel == val)
		return;
	micActivationLevel = val;
	OnActivationLevelChanged();
}

//! частота дискретизации при записи. Значения: 8000, 16000, 32000
//! значение по умолчанию: 8000
void CVoipManager::SetRecorderFreq(unsigned int  val)
{
	ATLASSERT( FALSE);
	if( recFreq == val)
		return;
	recFreq = val;
}

//! качество звука. Диапазон значений 1-10. 1 - минимальное качество. 10 - максимальное качество
//! значение по умолчанию: 4
void CVoipManager::SetVoiceQuality(float val)
{
	if( voiceQuality == val)
		return;
	voiceQuality = val;
	OnVoiceQualityChanged();
}

void CVoipManager::SetVoiceActivated(bool enabled)
{
	if( voiceActivated == enabled)
		return;

	//voiceActivated = enabled;
	voiceActivated = false;	// отключаем активацию голосом. В этом режиме звук корявится.
	OnVoiceActivatedChanged();
}

//! Получение параметров связи
bool CVoipManager::IsDirectSoundUsed()
{
	return mUseDS;
}

float CVoipManager::GetMicGainLevel()
{
	return micGainLevel;
}

float CVoipManager::GetMicActivationLevel()
{
	return micActivationLevel;
}

unsigned int CVoipManager::GetRecorderFreq()
{
	return recFreq;
}

float CVoipManager::GetVoiceQuality()
{
	return voiceQuality;
}

/*void CVoipManager::RegisterVoipAvatar( int aiAvatarID, const CComString& asName)
{
	mutexLog.lock();

	MapVoipIDName::iterator it = mapVoipIDName.find( aiAvatarID);
	if( it == mapVoipIDName.end())
	{
		if( asName == userName)
			myUserID = aiAvatarID;

		CComString	nameToInsert = asName;
		nameToInsert.MakeLower();
		mapVoipIDName.insert( MapVoipIDName::value_type( aiAvatarID, nameToInsert));		

#if VOIPMAN_BUILD_L
		CVoip3DSoundCalcManager *pVoip3DSoundCalcManager = MP_NEW(CVoip3DSoundCalcManager());
		mapVoip3DSoundCalcManagerID.insert( MapVoip3DSoundCalcManagerID::value_type( aiAvatarID, pVoip3DSoundCalcManager));
#endif

	}

	mutexLog.unlock();
	//GetUserNameByID(aiAvatarID);
}

void CVoipManager::UnregisterVoipAvatar( int aiAvatarID)
{
	mutexLog.Wait();
	MapVoipIDName::iterator it = mapVoipIDName.find( aiAvatarID);
	if ( it != mapVoipIDName.end())
	{
		mapVoipIDName.erase( it);		
#if VOIPMAN_BUILD_L
		MapVoip3DSoundCalcManagerID::iterator it = mapVoip3DSoundCalcManagerID.find( aiAvatarID);
		if ( it != mapVoip3DSoundCalcManagerID.end())
		{
			CVoip3DSoundCalcManager *pVoip3DSoundCalcManager = (CVoip3DSoundCalcManager*) it->second;
			SAFE_DELETE( pVoip3DSoundCalcManager);
			mapVoip3DSoundCalcManagerID.erase( it);
		}
#endif
	}
	mutexLog.Signal();
}*/

void CVoipManager::Destroy()
{

}

#if VOIPMAN_BUILD_L
CVoip3DSoundCalcManager *CVoipManager::GetVoip3DSoundManagerID( int aiAvatarID )
{
	MapVoip3DSoundCalcManagerID::iterator it = mapVoip3DSoundCalcManagerID.find( aiAvatarID );
	if ( it != mapVoip3DSoundCalcManagerID.end())
		return ( CVoip3DSoundCalcManager *)(it->second);
	return NULL;
}
#endif

//! Добавляет обработчик нотификационных событий
bool CVoipManager::AddCallback( voipIVoipManCallbacks* apCallback)
{
	csVecCallbacks.Lock();
	VecCallbacks::iterator it = find( vecCallbacks.begin(), vecCallbacks.end(), apCallback);
	if( it == vecCallbacks.end())
		vecCallbacks.push_back( apCallback);
	csVecCallbacks.Unlock();
	return true;
}

//! Удаляет обработчик нотификационных событий
bool CVoipManager::RemoveCallback( voipIVoipManCallbacks* apCallback)
{
	csVecCallbacks.Lock();
	VecCallbacks::iterator it = find( vecCallbacks.begin(), vecCallbacks.end(), apCallback);
	if( it != vecCallbacks.end())
	{
		vecCallbacks.erase( it);
		csVecCallbacks.Unlock();
		return true;
	}
	csVecCallbacks.Unlock();
	return false;
}

float _x = 2.0f;
float _y = 0.0f;
float _z = 0.0f;
bool bInc = true;
int it_ = 0;
double pi = 3.14;
int alpha = 0;

#include <math.h>
#include <mmsystem.h>

void CVoipManager::HandleVoiceData( int nUserID, int nSampleRate, const short* pRawAudio, int nSamples)
{	
}

bool CVoipManager::SetUserPosition( int nUserID, float x, float y, float z)
{
	return false;
}

//! Вызывается синхронно перед началом записи
bool CVoipManager::OnBeforeStartRecording()
{
	//recordIsOnGoing = true;
	ATLASSERT( FALSE);
	return true;
}

//! Начинает получать событие начала и окончания записи звука
bool CVoipManager::StartListenRecording( unsigned int aRecordID)
{
	SetRecordState(aRecordID, true, false);
	return recordServerConnection->StartListenRecording( aRecordID);
}

//! Заканчивает получать событие начала и окончания записи звука
bool CVoipManager::StopListenRecording( unsigned int aRecordID)
{
	SetRecordState(0, false, false);
	return recordServerConnection->StopListenRecording( aRecordID);
}

//! Начинает запись общения для текущей группы
bool CVoipManager::StartRecording( unsigned int aRecordID, LPCSTR alpcLocationName, unsigned int auRealityID)
{
	if( context->mpLogWriter)
	{
		CComString str;
		str.Format( "[VOIP]: CVoipManager::StartRecording %d", aRecordID);
		WriteLogLine(str.GetBuffer());
	}
	SetRecordState(aRecordID, true, true);

	// в режиме записи своего голоса ничего не делаем
	if( selfRecordingIsOnGoing != VSR_DISCONNECTED)
	{
		if( context->mpLogWriter)
			WriteLogLine("[VOIP]: CVoipManager::StartRecording failed now because of selfRecordingIsOnGoing");
		return false;
	}

	return StartRecordingImpl( aRecordID, alpcLocationName, auRealityID);
}

bool CVoipManager::PrepareSelfRecording( )
{
	if( context->mpLogWriter)
	{
		CComString str;
		str.Format( "[VOIP]: CVoipManager::PrepareSelfRecording");
		WriteLogLine(str.GetBuffer());
	}

	if( !selfRecordingServerConnection->PrepareSelfRecording( serverIP.GetBuffer(), roomName, userName.GetBuffer()))
	{	
		OnSelfRecordingConnectionLost();

		if( context->mpLogWriter)
			WriteLogLine("[VOIP]: CVoipManager::StartSelfRecording failed");

		return false;
	}

	return true;
}

bool CVoipManager::ResumeSelfRecording( )
{
	if( context->mpLogWriter)
	{
		CComString str;
		str.Format( "[VOIP]: CVoipManager::ResumeSelfRecording");
		WriteLogLine(str.GetBuffer());
	}

	if( !selfRecordingServerConnection->ResumeSelfRecording( ))
	{	
		if( context->mpLogWriter)
			WriteLogLine("[VOIP]: CVoipManager::ResumeSelfRecording failed");

		return false;
	}

	return true;
}

bool CVoipManager::StopSelfRecording( )
{
	bool abResult = selfRecordingServerConnection->StopSelfRecording();
	selfRecordingServerConnection->Disconnect();
	return abResult;
}

//! Начинает запись общения для текущей группы
bool CVoipManager::StartRecordingImpl( unsigned int aRecordID, LPCSTR alpcLocationName, unsigned int auRealityID)
{
	if( context->mpLogWriter)
	{
		CComString str;
		str.Format( "[VOIP]: CVoipManager::StartRecordingImpl %d", aRecordID);
		WriteLogLine(str.GetBuffer());
	}

	CComString recordedChannelName = EvaluateVoiceChannelName(alpcLocationName, auRealityID);
	if( recordServerConnection->StartRecording( serverIP.GetBuffer(), recordedChannelName.GetBuffer(), userName.GetBuffer(), aRecordID))
	{
		CopyCurrentPlayingFiles();
		if( context->mpServiceMan->SendVoipRecordRegisterRequest(serverIP.GetBuffer(), recordedChannelName.GetBuffer(), aRecordID) == OMS_OK)
			return true;
	}
	else
	{
		OnRecordConnectionLost(ERROR_CONNECTION_FAILED);
	}

	if( context->mpLogWriter)
		WriteLogLine("[VOIP]: CVoipManager::StartRecordingImpl failed");

	return false;
}

void CVoipManager::CopyCurrentPlayingFiles()
{
	if( playServerConnection->IsPlayingInProgress())
	{
		std::vector<std::string>	playingFiles = playServerConnection->GetPlayingFiles();
		for( unsigned int i=0; i<playingFiles.size(); i++)
			recordServerConnection->CopyAudioFile( playingFiles[i].c_str(), playServerConnection->GetRecordID());
	}
}

//! Заканчивает запись общения для текущей группы
bool CVoipManager::StopRecording(unsigned int aRecordID)
{
	if (m_currentRecordID == aRecordID)
	{
		SetRecordState(0, false, true);
	}
	//recordIsOnGoing = false;
	return StopRecordingImpl(aRecordID);	
}

//! Останавливает запись идентификатором aRecordID на текущем сервере
bool CVoipManager::StopRecordingImpl(unsigned int aRecordID)
{
	if( context->mpLogWriter)
	{
		CComString str;
		str.Format( "[VOIP]: CVoipManager::StopRecordingImpl", aRecordID);
		WriteLogLine(str.GetBuffer());
	}
	return recordServerConnection->StopRecording(aRecordID);
}

bool CVoipManager::DeleteRecord(unsigned int aRecordID)
{
	if( recordServerConnection->IsRecordingInProgress() && recordServerConnection->GetRecordID() == aRecordID)
		StopRecording(aRecordID);
	return recordServerConnection->DeleteRecord( aRecordID);
}

bool CVoipManager::DeleteRecords( unsigned int *pRecIDs, unsigned int count)
{
	return recordServerConnection->DeleteRecords( pRecIDs, count);
}

//! Начинает воспроизведение указанной записи для указанной реальности
bool CVoipManager::StartPlayingRecord(unsigned int aRecordID, unsigned int aRealityID, const wchar_t* apwcLocationID)
{
	if( context->mpLogWriter)
	{
		CComString str;
		str.Format("[VOIP]: StartPlayingRecord received: aRecordID = %u, aRealityID = %u", aRecordID, aRealityID);
		WriteLogLine(str.GetBuffer());
	}

	if( playServerConnection->IsPlayingInProgress())
	{
		if( context->mpLogWriter)
		{
			CComString str;
			str.Format("[VOIP]: StartPlayingRecord IsPlayingInProgress() == true: aRecordID = %u, aRealityID = %u", playServerConnection->GetRecordID(), playServerConnection->GetRealityID());
			WriteLogLine(str.GetBuffer());
		}
		if( playServerConnection->GetRecordID() == aRecordID 
			&& playServerConnection->GetRealityID() == aRealityID)
		{
			return true;
		}
		StopPlayingRecord();
	}
	if( playServerConnection->StartPlayingRecord( aRecordID, aRealityID, apwcLocationID))
	{

		if( context->mpServiceMan->SendVoipGetPlayServerRequest( aRealityID, aRecordID) == OMS_OK)
			return true;
	}
	return false;
}

//! Посылает серверу команду на воспроизведение записанного файла
bool CVoipManager::StartUserAudioPlayback( LPCSTR alpUserName, LPCSTR alpFileName, LPCSTR alpcCommunicationAreaName, unsigned int aiStartPosition, unsigned int auSyncVersion)
{
	if( !playServerConnection->IsPlayingInProgress())
		return false;

	if( playServerConnection->PlayAudioFile( alpUserName, alpFileName, aiStartPosition, auSyncVersion, alpcCommunicationAreaName))
	{
		if( recordServerConnection->IsRecordingInProgress())
			recordServerConnection->CopyAudioFile( alpFileName, playServerConnection->GetRecordID());

		return true;
	}

	return false;
}

//! Посылает серверу команду на остановку записанного файла
bool CVoipManager::StopUserAudioPlayback( LPCSTR alpUserName)
{
	if( !playServerConnection->IsPlayingInProgress())
		return false;

	return playServerConnection->RemoveUserAudio( alpUserName);
}

//! Посылает серверу команду на приостановку воспроизведения речи пользователя
bool CVoipManager::PauseUserAudio( LPCSTR alpUserName)
{
	if( !playServerConnection->IsPlayingInProgress())
		return false;

	return playServerConnection->PauseUserAudio( alpUserName);
}

//! Посылает серверу команду на продолжение воспроизведения речи пользователя
bool CVoipManager::ResumeUserAudio( LPCSTR alpUserName)
{
	if( !playServerConnection->IsPlayingInProgress())
		return false;

	return playServerConnection->ResumeUserAudio( alpUserName);
}

bool CVoipManager::SetUserVolume( const wchar_t* alpwcUserLogin, double adUserVolume)
{
	ATLASSERT( FALSE);
	return false;
}

// общая громкость входящего звука пользователей
bool CVoipManager::SetIncomingVoiceVolume( double adUserVolume)
{
	return false;
}

bool CVoipManager::SetEchoCancellation(bool anEnabled)
{
	return false;
}

bool CVoipManager::SetMixerEchoCancellation(bool anEnabled)
{
	return false;
}

//! Устанавливает уровень усиления громкости от пользователя
bool CVoipManager::SetUserGainLevel( const wchar_t* alwpcUserLogin, double adUserGainLevel)
{
	ATLASSERT( FALSE);
	return false;

}

//! Возвращает тип источника звука - голос или звук с компа
voipAudioSource CVoipManager::GetAudioSource()
{
	return audioSource;
}

//! Возвращает режим настройки громкости. Автоматический или ручной
bool CVoipManager::GetAutoVolume()
{
	return autoVolume;
}

//! Устанавливает режим настройки громкости. Автоматический или ручной
void CVoipManager::SetAutoVolume(bool abAuto)
{
	autoVolume = abAuto;
	OnAutoVolumeChanged();
}

//! Заканчивает воспроизведение определенной записи
bool CVoipManager::StopPlayingRecord()
{
	if( playServerConnection->IsPlayingInProgress())
	{
		if( context->mpLogWriter)
		{
			CComString str;
			str.Format("[VOIP]: StopPlayingRecord. Current aRecordID = %u, aRealityID = %u", playServerConnection->GetRecordID(), playServerConnection->GetRealityID());
			WriteLogLine(str.GetBuffer());
		}
		playServerConnection->StopPlayingRecord( );
	}
	return true;
}

/************************************************************************/
/* Handlers
/************************************************************************/
void CVoipManager::OnFileCreated( BYTE* aData, int aDataSize)
{
	CFileCreatedQueryIn	queryIn( aData, aDataSize);
	if( !queryIn.Analyse())
	{
		if( context->mpLogWriter)
			WriteLogLine("[VOIP]: OnFileCreated received and NOT analyzed");
		ATLASSERT( FALSE);
		return;
	}
	if( context->mpLogWriter)
	{
		CComString str;
		str.Format( "[VOIP]: OnFileCreated received and analyzed: UserName = %s, fileName = %s", queryIn.GetUserName().GetBuffer(), queryIn.GetFileName().GetBuffer());
		WriteLogLine(str.GetBuffer());
	}
	if (m_currentRecordID != queryIn.GetRecordID())
	{
		if( context->mpLogWriter)
		{
			CComString str;
			str.Format( "[VOIP]: OnFileCreated m_currentRecordID != queryIn.GetRecordID(): m_currentRecordID = %u, queryIn.GetRecordID() = %u", m_currentRecordID, queryIn.GetRecordID());
			WriteLogLine(str.GetBuffer());
		}
		return;
	}
	CALLBACK_TWO_PARAMS(onUserFileSaved, queryIn.GetUserName().GetBuffer(), queryIn.GetFileName().GetBuffer())
}

void CVoipManager::OnRecordStartResult( BYTE* aData, int aDataSize)
{
	if( context->mpLogWriter)
		WriteLogLine("[VOIP]: OnRecordStarted received");

	CRecordStartedQueryIn	queryIn( aData, aDataSize);
	if( !queryIn.Analyse())
	{
		if( context->mpLogWriter)
			WriteLogLine("[VOIP]: OnRecordStarted::Analyze FAILED");
		ATLASSERT( FALSE);
		return;
	}

	if( context->mpLogWriter)
	{
		CComString str;
		str.Format("[VOIP]: OnRecordStarted params: queryIn.GetRecordID()=%u, recordServerConnection->GetRecordID()=%u", queryIn.GetRecordID(), recordServerConnection->GetRecordID());
		WriteLogLine(str.GetBuffer());
	}

	if( queryIn.GetRecordID() == recordServerConnection->GetRecordID() ||
		queryIn.GetRecordID() == recordServerConnection->GetListenRecordID())
	{
		if( context->mpLogWriter)
		{
			WriteLogLine("[VOIP]: OnRecordStarted OnRecordConnectionRestored() called");
		}

		CALLBACK_TWO_PARAMS(onRecordingChanged, recordServerConnection->GetRecordID(), true);

		OnRecordConnectionRestored();
	}
};

//! Вызывается, когда воспроизводящий клиент на сервере закрывается
void CVoipManager::OnPlayFinished( BYTE* aData, int aDataSize)
{
	CRecordPlayFinishedIn queryIn( aData, aDataSize);
	if( !queryIn.Analyse())
	{
		if( context->mpLogWriter)
			WriteLogLine("[VOIP]: OnPlayFinished::Analyze FAILED");
		ATLASSERT( FALSE);
		return;
	}

	// Если при получении события прикращения воспроизведения звука на сервере мы все еще
	// находимся в воспроизведении - надо попробовать перезапустить клиента. Этот функционал надо
	// перенести на сервер.
	if( queryIn.GetRecordID() == playServerConnection->GetRecordID())
	{
		OnPlayConnectionLost();
		if( playServerConnection->IsPlayingInProgress())
			playServerConnection->OnConnectRestored();
	}
}

//! Вызывается, когда мы подключились к сессии воспроизведения
void CVoipManager::OnPlaySessionConnected( BYTE* aData, int aDataSize)
{
	CPlaySessionConnectedQueryIn	queryIn( aData, aDataSize);
	if( !queryIn.Analyse())
	{
		if( context->mpLogWriter)
			WriteLogLine("[VOIP]: CPlaySessionConnectedQueryIn::Analyze FAILED");
		ATLASSERT( FALSE);
		return;
	}

	if( playServerConnection->IsPlayingInProgress())
	{
		if( queryIn.GetRecordID() == playServerConnection->GetRecordID() && queryIn.GetRealityID() == playServerConnection->GetRealityID())
		{
			playServerConnection->SetPlayingMode( queryIn.GetPlaySessionMode());
		}
	}
}

//! Вызывается, когда началась запись собственного голоса
void CVoipManager::OnNotified( BYTE* aData, int aDataSize)
{
	CNotifiedQueryIn	queryIn( aData, aDataSize);
	if( !queryIn.Analyse())
	{
		if( context->mpLogWriter)
			WriteLogLine("[VOIP]: CNotifiedQueryIn::Analyze FAILED");
		ATLASSERT( FALSE);
		return;
	}

	// проверим на актуальность сообщения
	switch( queryIn.GetNotificationCode())
	{
		case NC_SelfRecordStarted:
		case NC_SelfRecordStopped:
		case NC_SelfPlayingStarted:
		case NC_SelfPlayingStopped:
		{
			if( selfRecordingIsOnGoing == VSR_DISCONNECTED || selfRecordingServerConnection->GetSessionToken() != queryIn.GetSessionToken())
			{
				return;
			}
		} break;

		default:
			break;
	}

	CALLBACK_TWO_PARAMS(onNotified, queryIn.GetNotificationCode(), 0);
}

void CVoipManager::OnEvoClientWorkStateChanged( BYTE* aData, int aDataSize)
{
	CEvoClientWorkStateChangedQueryIn	queryIn( aData, aDataSize);
	if( !queryIn.Analyse())
	{
		if( context->mpLogWriter)
			WriteLogLine("[VOIP]: CEvoClientWorkStateChangedQueryIn::Analyze FAILED");
		ATLASSERT( FALSE);
		return;
	}

	if( queryIn.GetSessionType() == ECST_RECORDIND)
	{
		// если это записывающий клиент, а мы уже не в записи..
		if( recordServerConnection->GetRecordID() != queryIn.GetRecordID() && recordServerConnection->GetListenRecordID() != queryIn.GetRecordID())
			return;
	}
	else	if( queryIn.GetSessionType() == ECST_PLAYING)
	{
		// если это воспроизводящий клиент, а мы уже не слушаем..
		if( playServerConnection->GetRealityID() != queryIn.GetRealityID() && playServerConnection->GetRecordID() != queryIn.GetRecordID())
			return;
	}
	else	if( queryIn.GetSessionType() == ECST_SELFRECORDING)
	{
		// если это тестовый клиент, а мы уже не тестим..
		if( !selfRecordingChannelConnected && selfRecordingIsOnGoing != VSR_CONNECTING)
			return;

		// по запуску клиента - подтверждаем вход в режим тестирования голоса
		if( queryIn.GetWorkState() == voip::ECWS_CONNECTED_TO_CHANNEL)
			SetSRModeState( VSR_CONNECTED);
	}

	SetEvoVoipClientWorkState( queryIn.GetSessionType(), queryIn.GetWorkState(), queryIn.GetErrorCode());
}

void CVoipManager::SetEvoVoipClientWorkState(voip::voipEvoClientSessionType aSessionType, voip::voipEvoClientWorkState aWorkState, unsigned int auError)
{
	if( aSessionType == ECST_PLAYING)
	{

	}
	else	if( aSessionType == ECST_RECORDIND)
	{
		
	}
	else	if( aSessionType == ECST_SELFRECORDING)
	{

	}
	
	CALLBACK_THREE_PARAMS( onEvoClientWorkStateChanged, aSessionType, aWorkState, auError);
}

//! Вызывается, когда мы получили список файлов по сессии
void CVoipManager::OnAudioFilesInfo( BYTE* aData, int aDataSize)
{
	CAudioFilesInfoQueryIn	queryIn( aData, aDataSize);
	if( !queryIn.Analyse())
	{
		if( context->mpLogWriter)
			WriteLogLine("[VOIP]: CAudioFilesInfoQueryIn::Analyze FAILED");
		ATLASSERT( FALSE);
		return;
	}

	if( playServerConnection->IsPlayingInProgress())
	{
		playServerConnection->SetAudioFilesInfo( queryIn.GetAudioFrames(), queryIn.GetRecordID());
	}
}

//! Вызывается, когда пишуший клиент на сервере закрывается
void CVoipManager::OnRecordFinished( BYTE* aData, int aDataSize)
{
	CRecordFinishedIn	queryIn( aData, aDataSize);
	if( !queryIn.Analyse())
	{
		if( context->mpLogWriter)
			WriteLogLine("[VOIP]: OnRecordFinished::Analyze FAILED");
		ATLASSERT( FALSE);
		return;
	}

	// Если мы все еще пишем эту запись - попробуем переподключиться
	if( queryIn.GetRecordID() == recordServerConnection->GetRecordID())
	{
		OnRecordConnectionLost(queryIn.GetErrorCode());
		if( recordServerConnection->IsRecordingInProgress())
			recordServerConnection->OnConnectRestored();
	}
}

void CVoipManager::OnAudioPlayed( BYTE* aData, int aDataSize)
{
	CAudioPlayedQueryIn	queryIn( aData, aDataSize);
	if( !queryIn.Analyse())
	{
		if( context->mpLogWriter)
			WriteLogLine("[VOIP]: OnAudioPlayed::Analyze FAILED");
		ATLASSERT( FALSE);
		return;
	}
	CALLBACK_TWO_PARAMS(onAudioFilePlayed, queryIn.GetUserName().GetBuffer(), queryIn.GetFileName().GetBuffer())
}

void CVoipManager::OnMixedAudioCreated( BYTE* aData, int aDataSize)
{
	CMixedAudioCreatedQueryIn	queryIn( aData, aDataSize);
	if( !queryIn.Analyse())
	{
		if( context->mpLogWriter)
			WriteLogLine("[VOIP]: OnMixedAudioCreated::Analyze FAILED");
		ATLASSERT( FALSE);
		return;
	}
	if( recordServerConnection->IsRecordingInProgress()
		&& queryIn.GetRecordID() == recordServerConnection->GetRecordID())
	{
		CALLBACK_TWO_PARAMS(onMixedAudioCreated, queryIn.GetRecordID(), queryIn.GetMixedAudioName());
	}
}

//! Приостанавливает воспроизведение определенной записи
void CVoipManager::PauseServerAudio()
{
	playServerConnection->PauseServerAudio();
}

//! Продолжает воспроизведение определенной записи
void CVoipManager::ResumeServerAudio()
{
	playServerConnection->ResumeServerAudio();
}

//! Перематывает воспроизведение определенной записи
void CVoipManager::RewindServerAudio()
{
	playServerConnection->RewindServerAudio();
}

bool	CVoipManager::SetVoipRecordServerAddress(LPCSTR alpcVoipServerAddress, unsigned int aiVoipPort, LPCSTR alpcRecordServerAddress
									 , unsigned int aiRecordPort, LPCSTR alpcRoomName)

{
	if( alpcRoomName != requestedRoomName)
		return false;

	/*alpcVoipServerAddress = "78.46.87.100";
	alpcRecordServerAddress = "192.168.0.104";
	aiVoipPort = 443;
	aiRecordPort = 11194;	// порт VoipServer.exe*/

	if( context->mpLogWriter)
	{
		CComString str;
		str.Format("[VOIP]: SelfRecordingMode 55 roomName = %s", alpcRoomName);
		WriteLogLine(str.GetBuffer());
	}

	SendConnectRequest( alpcVoipServerAddress, aiVoipPort, alpcRecordServerAddress, aiRecordPort, alpcRoomName);
	return true;
}

bool	CVoipManager::SetVoipPlayServerAddress(unsigned int aiRecordID
											   , LPCSTR alpcVoipRecordServerAddress, unsigned int aiRecordPort
											   , LPCSTR alpcVoipPlayServerAddress, unsigned int aiPlayPort)
{
	/*alpcVoipRecordServerAddress = "192.168.0.104";
	aiRecordPort = 11194;	// порт VoipServer.exe

	alpcVoipPlayServerAddress = "78.46.87.100";
	aiPlayPort = 443; 	// порт Mumble*/

	// Если запрос опоздал и мы воспроизводим уже другую запись, то игнорим его
	if( playServerConnection->GetRecordID() != aiRecordID)
		return false;

	// Если сервер для записи не найден
	if( alpcVoipRecordServerAddress == NULL || *alpcVoipRecordServerAddress == 0)
		return false;
	if (alpcVoipPlayServerAddress == NULL || *alpcVoipPlayServerAddress == 0)
		return false;

	// В нулевой реальности тоже сейчас может проигрываться запись, поэтому проверям через режим
	if( !playServerConnection->IsPlayingInProgress())
		return false;

	if( playServerConnection->Connect( alpcVoipRecordServerAddress, aiRecordPort))
	{
		/*if( connectionStatus == CS_CONNECTED
			|| connectionStatus == CS_CONNECTING)
		{*/
			//! В режиме воспроизведения пока следуем по локациям за пользователем
			if( playServerConnection->IsPlayingInProgress())
			{
				if( context->mpLogWriter)
				{
					CComString str;
					str.Format("[VOIP]: SetVoipPlayServerAddress::SetEvoVOIPServer. Current serverIP = %s", serverIP.GetBuffer());
					WriteLogLine(str.GetBuffer());
				}

				playServerConnection->SetEvoVOIPServer( alpcVoipPlayServerAddress);
			}
		//}
	}
	return false;
}

void CVoipManager::OnRecordConnectionLost(unsigned int auErrorCode)
{
	if( context->mpLogWriter)
	{
		CComString str;
		str.Format( "[VOIP]: OnRecordConnectionLost()::Record ID=%u", recordServerConnection->GetRecordID());
		WriteLogLine( str.GetBuffer());
	}
	CALLBACK_THREE_PARAMS(onRecordingChanged, recordServerConnection->GetRecordID(), false, auErrorCode);

	SetEvoVoipClientWorkState( ECST_RECORDIND, ECWS_CLOSED, 0);
}

void CVoipManager::OnRecordConnectionRestored()
{
	// восстановлена связь с сервером записи
	if( context->mpLogWriter)
	{
		CComString str;
		str.Format( "[VOIP]: OnRecordConnectionRestored()::Record ID=%u", recordServerConnection->GetRecordID());
		WriteLogLine( str.GetBuffer());
	}

}

void CVoipManager::OnPlayConnectionLost()
{
	SetEvoVoipClientWorkState( ECST_PLAYING, ECWS_CLOSED, 0);
}

void CVoipManager::OnPlayConnectionRestored()
{
	// ??
}

void CVoipManager::OnSelfRecordingConnectionLost()
{
	if( selfRecordingIsOnGoing != VSR_DISCONNECTED)
	{
		SetEvoVoipClientWorkState( ECST_SELFRECORDING, ECWS_CLOSED, 0);

		// если мы пишем свой голос, то перестанем это делать
		AbortSelfRecordingMode();
	}
}

void CVoipManager::OnSelfRecordingConnectionRestored()
{
	// если мы в канале и подключились к серверу - все ок
	if( selfRecordingIsOnGoing == VSR_CONNECTING)
	{
		PrepareSelfRecording();
	}
}

bool CVoipManager::StartMicRecording()
{
/*
	if ( !lpStartRecording)
		return false;
	return lpStartRecording() == 0;
*/
#if BUILD_H
	return Start_Recording() == 0;
#else
	return false;
#endif
}

bool CVoipManager::StopMicRecording( LPWSTR *fileName, unsigned int aSize)
{
/*
	if ( !lpStopRecording)
		return false;
	return lpStopRecording() == 0;
*/
#if BUILD_H
	return Stop_Recording( fileName, aSize) == 0;
#else
	return false;
#endif

}

float CVoipManager::GetCurrentMicInputLevelNormalized()
{
	return Get_CurrentMicInputLevelNormalized();
}

void CVoipManager::WriteLogLine(LPCSTR alpString)
{
	if( alpString && context->mpLogWriter)
	{
		context->mpLogWriter->WriteLnLPCSTR( alpString, ltDefault);
	}
}

//! Автоматическое управление громкостью
bool CVoipManager::IsAutoGaneControlEnabled()
{
	ATLASSERT( false);
	return false;
}
bool CVoipManager::SetAutoGaneControlEnabled(bool abEnabled)
{
	ATLASSERT( false);
	return false;
}
//! Автоматическое подавление шумов
bool CVoipManager::IsDenoisingEnabled()
{
	ATLASSERT( false);
	return false;
}
bool CVoipManager::SetDenoisingEnabled(bool abEnabled)
{
	ATLASSERT( false);
	return false;
}

//! Вызывается при вхождении в канал
void CVoipManager::OnChannelJoined(const char* alpcChannelName)
{
	selfRecordingChannelConnected = false;

	if( context->mpLogWriter)
	{
		USES_CONVERSION;
		CComString str;
		str.Format("[VOIP]: Joined to channel = %s", alpcChannelName);
		WriteLogLine( str.GetBuffer());
	}

	if( selfRecordingIsOnGoing == VSR_DISCONNECTED)
	{
		if( context->mpLogWriter)
		{
			USES_CONVERSION;
			CComString str;
			str.Format("[VOIP]: selfRecordingIsOnGoing is false");
			WriteLogLine( str.GetBuffer());
		}
		return;
	}



	// вошли не в тот канал
	if( strstr( alpcChannelName, roomName.GetBuffer()) == NULL)
	{
		if( context->mpLogWriter)
		{
			USES_CONVERSION;
			CComString str;
			str.Format("[VOIP]: entered not in self channel %s, %s", alpcChannelName, roomName.GetBuffer());
			WriteLogLine( str.GetBuffer());
		}

		ConnectToSelfVoiceChannel();
		return;
	}

	selfRecordingChannelConnected = true;
}

voipError CVoipManager::ConnectToSelfVoiceChannel()
{
	if( context->mpLogWriter)
		WriteLogLine("[VOIP]: SelfRecordingMode 5");

	CComString	myOwnGroup;
	myOwnGroup.Format("bunkerOf_%s", userName.GetBuffer());

	selfRecordingChannelConnected = false;

	if( context->mpLogWriter)
	{
		CComString str;
		str.Format("[VOIP]: Before connect 1");
		WriteLogLine( str.GetBuffer());
	}

	voipError errorCode = Connect( myOwnGroup, SKIP_REALITY_ID, userName.GetBuffer(), requestedUserPassword, requestedAvatarID);

	if( context->mpLogWriter)
		WriteLogLine("[VOIP]: SelfRecordingMode 6");

	if( errorCode == ERROR_NOERROR)
	{
		if( context->mpLogWriter)
			WriteLogLine("[VOIP]: SelfRecordingMode 7");
		// после вызова Connect - ждем либо OnRecordConnectionLost либо OnRecordConnectionRestored
		SetSRModeState( VSR_CONNECTING);
	}
	else
	{
		int ff = 0;
	}

	return errorCode;
}

//! вход в режим прослушивания своего голоса
int CVoipManager::EnterSelfRecordingMode()
{
	if( context->mpLogWriter)
		WriteLogLine("[VOIP]: SelfRecordingMode 1");

	if( !IsVoipSystemInitialized())
		return ERROR_NOTINITIALIZED;

	if( context->mpLogWriter)
		WriteLogLine("[VOIP]: SelfRecordingMode 2");

	if( selfRecordingIsOnGoing == VSR_CONNECTED || selfRecordingIsOnGoing == VSR_CONNECTING)
		return ERROR_NOERROR;

	if( context->mpLogWriter)
		WriteLogLine("[VOIP]: SelfRecordingMode 3");

	inSRModeRequestedRoomName = requestedRoomName;
	inSRModeRequestedUserName = requestedUserName;
	inSRModeRequestedUserPassword = requestedUserPassword;
	inSRModeRequestedAvatarID = requestedAvatarID;

	return ConnectToSelfVoiceChannel();
}

//! начало записи своего голоса
int CVoipManager::StartSelfRec()
{
	if( selfRecordingIsOnGoing != VSR_CONNECTED)
		return ERROR_NOTINITIALIZED;

	if( !selfRecordingChannelConnected)
		return ERROR_SR_CHANNEL_NOT_CONNECTED;

	if( !ResumeSelfRecording())
		return ERROR_CONNECTION_FAILED;

	return ERROR_NOERROR;
}

//! остановить запись своего голоса
int CVoipManager::StopSelfRec()
{
	if( selfRecordingIsOnGoing != VSR_CONNECTED)
		return ERROR_NOTINITIALIZED;

	if( !selfRecordingServerConnection->IsRecordingInProgress())
		return ERROR_NOTINITIALIZED;

	selfRecordingServerConnection->PauseSelfRecording();

	return ERROR_NOERROR;
}

//! воспроизвести записанный голос
int CVoipManager::PlaySelfRec()
{
	if( selfRecordingIsOnGoing != VSR_CONNECTED)
		return ERROR_SR_MODE_NOT_INITIALIZED;

	if( !selfRecordingServerConnection->IsRecordingInProgress())
		return ERROR_NOTINITIALIZED;

	selfRecordingServerConnection->PlaySelfRecording();

	return ERROR_NOERROR;
}

//! выйти из режима прослушивания своего голоса
int CVoipManager::ExitSelfRecordingMode()
{
	if( context->mpLogWriter)
		WriteLogLine("[VOIP]: SelfRecordingMode ExitSelfRecordingMode");

	if( selfRecordingIsOnGoing != VSR_CONNECTED && selfRecordingIsOnGoing != VSR_CONNECTING)
		return ERROR_NOERROR;

	SetSRModeState( VSR_DISCONNECTING);

	AbortSelfRecordingMode();

	return ERROR_NOERROR;
}

//! Прервать режим прослушивания своего голоса
int CVoipManager::AbortSelfRecordingMode()
{
	StopSelfRecording();

	SetSRModeState( VSR_DISCONNECTED);

	// Тут надо переподключиться к моей группе
	// ??
	if( context->mpLogWriter)
		WriteLogLine("[VOIP]: Before connect 2");
	Connect( inSRModeRequestedRoomName.GetBuffer()
			, SKIP_REALITY_ID
			, inSRModeRequestedUserName.GetBuffer()
			, inSRModeRequestedUserPassword.GetBuffer()
			, inSRModeRequestedAvatarID);


	return ERROR_NOERROR;
}

void CVoipManager::SetSRModeState( voip::voipSRModeStatusChanged	aState)
{
	selfRecordingIsOnGoing = aState;
	CALLBACK_ONE_PARAM(onSRModeStatusChanged, aState);
}

void CVoipManager::SetRecordState(unsigned int aRecordID, bool aRecording, bool anAuthor)
{
	m_currentRecordID = aRecordID;
}