#ifndef _mlRMMLJSPlVoice_h__
#define _mlRMMLJSPlVoice_h__

#include "VoipManClient.h"

namespace rmml{

	class mlVoipCallbackManager: 
		public voip::voipIVoipManCallbacks
	{
		bool mbCallbacksIsSet;
		mlSceneManager* mpSM;
	public:
		mlVoipCallbackManager(mlSceneManager* apSM);
		void SetCallbacks(JSContext *cx);

		void onEventEntered( unsigned int uiRealityId,  unsigned int uiPlayingRecordID, unsigned int uiBaseRecordID, const wchar_t* apwcLocationID);
		void onEventLeaved();
		void onConnectSuccessInternal();
		void onConnectStartInternal();
		void onConnectFailedInternal(mlSynchData &aData);
		void onConnectionLostInternal();
		void onDisconnectedInternal();
		void onKickedInternal();
		void onAddRoomInternal(mlSynchData& aData);
		void onRemoveRoomInternal(mlSynchData& aData);
		void onUserTalkingInternal(mlSynchData& aData);
		void onUserStoppedTalkingInternal(mlSynchData& aData);
		void onUserFileSavedInternal(mlSynchData& aData);
		void onAudioFilePlayedInternal(mlSynchData& aData);
		void onRecordingChangedInternal(mlSynchData& aData);
		void onEvoClientWorkStateChangedInternal(mlSynchData& aData);
		void onSRModeStatusChangedInternal(mlSynchData& aData);
		void onNotifiedInternal(mlSynchData& aData);
		// для асинхронного режима
		virtual void onSetInputDeviceInternal( mlSynchData& aData);
		virtual void onSetOutputDeviceInternal( mlSynchData& aData);
		virtual void onGetInputDevicesInternal( mlSynchData& aData);
		virtual void onGetOutputDevicesInternal( mlSynchData& aData);
		virtual void onDeviceErrorCodeInternal( mlSynchData& aData);
		virtual void OnRecievedFindedMicNameInternal( mlSynchData& aData);
		virtual void onReInitializeVoipSystemInternal( );

		// реализация voip::voipIVoipManCallbacks
	public:
		virtual void onConnectSuccess();
		virtual void onConnectStart();
		virtual void onConnectFailed(  const char* aVoipServerIP);
		virtual void onConnectionLost();
		virtual void onDisconnected();
		virtual void onKicked();
		virtual void onAddRoom( const char* alpcRoomName);
		virtual void onRemoveRoom( const char* alpcRoomName);
		virtual void onUserTalking( const char* alpcUserName, long long alRmmlID);
		virtual void onUserStoppedTalking( const char* alpcUserName, long long alRmmlID);
		virtual void onUserFileSaved( const char* alpcUserName, const char* alpcFileName);
		virtual void onAudioFilePlayed( const char* alpcUserName, const char* alpcFileName);
		virtual void onRecordingChanged( unsigned int auRecordID, bool abRecording, unsigned int auErrorCode);
		virtual void onMixedAudioCreated( unsigned int auRecordID, const char* alpcFileName);
		virtual void onEvoClientWorkStateChanged( voip::voipEvoClientSessionType aSessionType, voip::voipEvoClientWorkState aWorkState, unsigned int auErrorCode);
		virtual void onSRModeStatusChanged( voip::voipSRModeStatusChanged aStatus);
		virtual void onNotified(unsigned int auNotifyCode, unsigned int auNotifiedData);
		// для асинхронного режима
		virtual void onSetInputDevice( unsigned int auErrorCode, int aiToIncludeSoundDevice);
		virtual void onSetOutputDevice( unsigned int auErrorCode, int aiToIncludeSoundDevice);
		virtual void onGetInputDevices( const wchar_t *apMicDeviceNames, const wchar_t *apMixDeviceNames);
		virtual void onGetOutputDevices( const wchar_t *apDeviceNames);

		virtual void onRecievedDeviceErrorCode(int anErrorCode);
		virtual void onRecievedFindedMicName(const wchar_t * aDeviceGuid, const wchar_t * aDeviceName, const wchar_t * aDeviceLineName);
		virtual void onReInitializeVoipSystem();
	};
}

#endif