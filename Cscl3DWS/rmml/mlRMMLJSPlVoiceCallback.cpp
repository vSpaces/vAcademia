#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"
#include "mlRMMLJSPlVoice.h"

namespace rmml {

	mlVoipCallbackManager::mlVoipCallbackManager(mlSceneManager* apSM)
	{
		mpSM = apSM;
		mbCallbacksIsSet = false;
	}

	void mlVoipCallbackManager::SetCallbacks(JSContext *cx)
	{
		if(mbCallbacksIsSet) return;
		omsContext* pOMSContext = mpSM->GetContext();
		if(pOMSContext == NULL) return;
		voip::IVoipManager* pVoipMan = pOMSContext->mpVoipMan;
		if(pVoipMan == NULL)
			return;
		mbCallbacksIsSet = pVoipMan->AddCallback(this);
	}

	void mlVoipCallbackManager::onConnectSuccessInternal()
	{
		mpSM->NotifyVoiceOnConnectSuccess();
	}

	void mlVoipCallbackManager::onEventEntered( unsigned int uiRealityId,  unsigned int uiPlayingRecordID, unsigned int uiBaseRecordId, const wchar_t* apwcLocationID)
	{
		omsContext* pOMSContext = mpSM->GetContext();
		if(pOMSContext == NULL) return;

		// Записи для воспроизведения нет
		if( uiPlayingRecordID == 0 || uiPlayingRecordID == -1)
			return;

		unsigned int playingRecID = uiPlayingRecordID;
		if (uiBaseRecordId != 0 && uiBaseRecordId != INCORRECT_RECORD_ID)
			playingRecID = uiBaseRecordId;

		if( pOMSContext->mpVoipMan && pOMSContext->mpMapMan)
		{
			pOMSContext->mpVoipMan->StartPlayingRecord( playingRecID, uiRealityId, apwcLocationID);
		}
	}

	void mlVoipCallbackManager::onEventLeaved()
	{
		omsContext* pOMSContext = mpSM->GetContext();
		if(pOMSContext == NULL) return;
		if( pOMSContext->mpVoipMan)
			pOMSContext->mpVoipMan->StopPlayingRecord();
	}

	void mlVoipCallbackManager::onConnectStartInternal()
	{
		mpSM->NotifyVoiceOnConnectStart();
	}

	void mlVoipCallbackManager::onConnectFailedInternal( mlSynchData &aData)
	{
		char* voipServerIP;
		aData >> voipServerIP;
		mpSM->NotifyVoiceOnConnectFailed( voipServerIP);
	}

	void mlVoipCallbackManager::onConnectionLostInternal()
	{
		mpSM->NotifyVoiceOnConnectLost();
	}

	void mlVoipCallbackManager::onDisconnectedInternal()
	{
		mpSM->NotifyVoiceOnDisconnected();
	}

	void mlVoipCallbackManager::onKickedInternal()
	{
		mpSM->NotifyVoiceOnKicked();
	}

	void mlVoipCallbackManager::onAddRoomInternal(mlSynchData& aData)
	{
		char* roomName;
		aData >> roomName;
		mpSM->NotifyVoiceOnAddRoom( roomName);
	}

	void mlVoipCallbackManager::onRemoveRoomInternal(mlSynchData& aData)
	{
		char* roomName;
		aData >> roomName;
		mpSM->NotifyVoiceOnRemoveRoom( roomName);
	}

	void mlVoipCallbackManager::onUserTalkingInternal(mlSynchData& aData)
	{
		char* userName;
		aData >> userName;
		mpSM->NotifyVoiceOnUserTalking( userName);
	}

	void mlVoipCallbackManager::onUserStoppedTalkingInternal(mlSynchData& aData)
	{
		char* userName;
		aData >> userName;
		mpSM->NotifyVoiceOnUserStoppedTalking( userName);
	}

	void mlVoipCallbackManager::onUserFileSavedInternal(mlSynchData& aData)
	{
		char* userName;
		aData >> userName;
		//! Первую строку придется сохранить, так как получение второй ее перетрет..
		int iStrLen = strlen(userName);
		char* un = MP_NEW_ARR( char, iStrLen + 1);
		memcpy( un, userName, iStrLen + 1/*for last 0*/);
		char* fileName;
		aData >> fileName;
		mpSM->NotifyVoiceOnUserFileSaved( un, fileName);
		//! .. и почистить
		MP_DELETE_ARR( un);
	}

	void mlVoipCallbackManager::onAudioFilePlayedInternal(mlSynchData& aData)
	{
		char* userName;
		aData >> userName;
		//! Первую строку придется сохранить, так как получение второй ее перетрет..
		char* un = MP_NEW_ARR( char, strlen(userName) + 1);
		memcpy( un, userName, strlen(userName) + 1/*for last 0*/);
		char* fileName;
		aData >> fileName;
		mpSM->NotifyVoiceOnAudioFilePlayed( un, fileName);
		//! .. и почистить
		MP_DELETE_ARR( un);
	}

	void mlVoipCallbackManager::onSRModeStatusChangedInternal(mlSynchData& aData)
	{
		unsigned int status;
		aData >> status;
		mpSM->NotifyVoiceOnSRModeStatusChanged( status);
	}

	void mlVoipCallbackManager::onNotifiedInternal(mlSynchData& aData)
	{
		unsigned int code;
		unsigned int data;
		aData >> code;
		aData >> data;
		mpSM->NotifyVoiceOnNotified( code, data);
	}

	void mlVoipCallbackManager::onSetInputDeviceInternal(mlSynchData& aData)
	{
		unsigned int code;
		aData >> code;
		int iToIncludeSoundDevice;
		aData >> iToIncludeSoundDevice;
		mpSM->NotifySetInputDevice( code, iToIncludeSoundDevice);
	}

	void mlVoipCallbackManager::onSetOutputDeviceInternal(mlSynchData& aData)
	{
		unsigned int code;
		aData >> code;
		int iToIncludeSoundDevice;
		aData >> iToIncludeSoundDevice;
		mpSM->NotifySetOutputDevice( code, iToIncludeSoundDevice);
	}

	void mlVoipCallbackManager::onGetInputDevicesInternal(mlSynchData& aData)
	{
		wchar_t *pMicDeviceNames;
		aData >> pMicDeviceNames;
		mlString sMicDeviceNames = pMicDeviceNames;
		wchar_t *pMixDeviceNames;
		aData >> pMixDeviceNames;
		mpSM->NotifyGetInputDevices( sMicDeviceNames.c_str(), pMixDeviceNames);
	}

	void mlVoipCallbackManager::onGetOutputDevicesInternal(mlSynchData& aData)
	{
		wchar_t *sDeviceNames;
		aData >> sDeviceNames;
		mpSM->NotifyGetOutputDevices( sDeviceNames);
	}

	void mlVoipCallbackManager::OnRecievedFindedMicNameInternal( mlSynchData& aData)
	{
		wchar_t *pDeviceGuid;
		wchar_t *pDeviceName;
		wchar_t *sDeviceLineName;
		
		aData >> pDeviceGuid;
		mlString sDeviceGuid = pDeviceGuid;
		aData >> pDeviceName;
		mlString sDeviceName = pDeviceName;
		aData >> sDeviceLineName;

		mpSM->NotifyAutoFindedMicName( sDeviceGuid.c_str(), sDeviceName.c_str(), (const wchar_t*)sDeviceLineName);
	}

	void mlVoipCallbackManager::onDeviceErrorCodeInternal( mlSynchData& aData)
	{
		int errorCode;
		aData >> errorCode;
		mpSM->NotifyDeviceErrorCode( errorCode);
	}

	void mlVoipCallbackManager::onReInitializeVoipSystemInternal( )
	{
		mpSM->NotifyReInitializeVoipSystem( );
	}
	

	void mlVoipCallbackManager::onEvoClientWorkStateChangedInternal(mlSynchData& aData)
	{
		unsigned int aSessionType;
		unsigned int aWorkState;
		unsigned int auErrorCode;
		aData >> (unsigned int)aSessionType;
		aData >> (unsigned int)aWorkState;
		aData >> (unsigned int)auErrorCode;
		mpSM->NotifyVoiceOnEvoClientWorkStateChanged( aSessionType, aWorkState, auErrorCode);
	}

	void mlVoipCallbackManager::onRecordingChangedInternal(mlSynchData& aData)
	{
		bool recordingInProgress;
		unsigned int errorCode;
		aData >> recordingInProgress;
		aData >> errorCode;
		if( recordingInProgress && errorCode == 0)
			mpSM->NotifyVoiceOnRecordStarted();
		else
			mpSM->NotifyVoiceOnRecordStopped(errorCode);
	}

	//! Real calls to RMML

	void mlVoipCallbackManager::onConnectSuccess()
	{
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnConnectSuccess, NULL, 0);
	}

	void mlVoipCallbackManager::onConnectStart()
	{
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnConnectStart, NULL, 0);
	}

	void mlVoipCallbackManager::onConnectFailed( const char* aVoipServerIP)
	{
		mlSynchData LogData;
		LogData << aVoipServerIP;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnConnectFailed, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlVoipCallbackManager::onConnectionLost()
	{
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnConnectLost, NULL, 0);
	}

	void mlVoipCallbackManager::onDisconnected()
	{
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnDisconnected, NULL, 0);
	}

	void mlVoipCallbackManager::onKicked()
	{
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnKicked, NULL, 0);
	}

	void mlVoipCallbackManager::onAddRoom( const char* alpcRoomName)
	{
		mlSynchData LogData;
		LogData << alpcRoomName;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnAddRoom, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlVoipCallbackManager::onRemoveRoom( const char* alpcRoomName)
	{
		mlSynchData LogData;
		LogData << alpcRoomName;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnRemoveRoom, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlVoipCallbackManager::onUserTalking( const char* alpcUserName, long long alRmmlID)
	{
		mlSynchData LogData;
		//long lhigh = (long)(alRmmlID >> 32);
		//long llow = (long)(alRmmlID & 0x00000000FFFFFFFF);
		LogData << alpcUserName;
		//LogData << lhigh;
		//LogData << llow;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnUserTalking, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlVoipCallbackManager::onUserStoppedTalking( const char* alpcUserName, long long alRmmlID)
	{
		mlSynchData LogData;
		LogData << alpcUserName;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnUserStoppedTalking, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlVoipCallbackManager::onUserFileSaved( const char* alpcUserName, const char* alpcFileName)
	{
		mlSynchData LogData;
		LogData << alpcUserName;
		LogData << alpcFileName;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnUserFileSaved, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlVoipCallbackManager::onAudioFilePlayed( const char* alpcUserName, const char* alpcFileName)
	{
		mlSynchData LogData;
		LogData << alpcUserName;
		LogData << alpcFileName;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnAudioFilePlayed, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlVoipCallbackManager::onRecordingChanged( unsigned int auRecordID, bool abRecording, unsigned int auErrorCode)
	{
		mlSynchData LogData;
		LogData << abRecording;
		LogData << auErrorCode;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnRecordingChanged, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlVoipCallbackManager::onMixedAudioCreated( unsigned int auRecordID, const char* alpcFileName)
	{
		mpSM->GetContext()->mpPreviewRecorder->AddPreviewAudio( auRecordID, alpcFileName);
	}

	void mlVoipCallbackManager::onEvoClientWorkStateChanged( voip::voipEvoClientSessionType aSessionType, voip::voipEvoClientWorkState aWorkState, unsigned int auErrorCode)
	{
		mlSynchData LogData;
		LogData << (unsigned int)aSessionType;
		LogData << (unsigned int)aWorkState;
		LogData << (unsigned int)auErrorCode;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnEvoClientWorkStateChanged, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlVoipCallbackManager::onSRModeStatusChanged( voip::voipSRModeStatusChanged aStatus)
	{
		mlSynchData LogData;
		LogData << (unsigned int)aStatus;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnSRModeStatusChanged, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlVoipCallbackManager::onNotified(unsigned int auNotifyCode, unsigned int auNotifiedData)
	{
		mlSynchData LogData;
		LogData << (unsigned int)auNotifyCode;
		LogData << (unsigned int)auNotifiedData;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnNotified, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlVoipCallbackManager::onSetInputDevice(unsigned int auErrorCode, int aiToIncludeSoundDevice)
	{
		mlSynchData LogData;
		LogData << (unsigned int)auErrorCode;
		LogData << aiToIncludeSoundDevice;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnSetInputDevice, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlVoipCallbackManager::onSetOutputDevice(unsigned int auErrorCode, int aiToIncludeSoundDevice)
	{
		mlSynchData LogData;
		LogData << (unsigned int)auErrorCode;
		LogData << aiToIncludeSoundDevice;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnSetOutputDevice, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlVoipCallbackManager::onGetInputDevices(  const wchar_t *apMicDeviceNames, const wchar_t *apMixDeviceNames)
	{
		mlSynchData LogData;		
		LogData << apMicDeviceNames;
		LogData << apMixDeviceNames;
		//LogData << (unsigned int)auNotifiedData;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnGetInputDevices, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlVoipCallbackManager::onGetOutputDevices( const wchar_t *apDevicesNames)
	{
		mlSynchData LogData;
		LogData << apDevicesNames;
		//LogData << (unsigned int)auNotifiedData;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnGetOutputDevices, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlVoipCallbackManager::onRecievedFindedMicName(const wchar_t * aDeviceGuid, const wchar_t * aDeviceName, const wchar_t * aDeviceLineName)
	{
		mlSynchData LogData;
		LogData << aDeviceGuid;
		LogData << aDeviceName;
		LogData << aDeviceLineName;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnRecievedFindedMicName, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlVoipCallbackManager::onRecievedDeviceErrorCode(int anErrorCode)
	{
		mlSynchData LogData;
		LogData << anErrorCode;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnDeviceErrorCode, (unsigned char*)LogData.data(), LogData.size());
	}

	void mlVoipCallbackManager::onReInitializeVoipSystem()
	{
		mlSynchData LogData;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_voipOnReInitializeVoipSystem, (unsigned char*)LogData.data(), LogData.size());		
	}
}

