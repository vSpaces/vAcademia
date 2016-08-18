#ifndef __VOIPMAN_H_
#define __VOIPMAN_H_

#ifdef VOIPMAN_EXPORTS
#define VOIPMAN_API //__declspec(dllexport)
#else
#define VOIPMAN_API //__declspec(dllimport)
#endif

#ifdef WIN32
#define VOIPMAN_NO_VTABLE __declspec(novtable)
#else
#define VOIPMAN_NO_VTABLE
#endif

#include "oms_context.h"
#include "rmml.h"
#include "resource.h"

//extern HWND hWndMainServer;

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

	/*struct VOIPMAN_NO_VTABLE IVoipCharacter
	{
		virtual void handleVoiceData( int nSampleRate, const short* pRawAudio, int nSamples) = 0;		
		virtual ml3DPosition getAbsolutePosition() = 0;
		virtual ml3DRotation getAbsoluteRotation() = 0;
		virtual  bool isSayingAudioFile(LPCSTR  alpcFileName) = 0;
	};*/

	struct VOIPMAN_NO_VTABLE IVoip3DSoundCalculator
	{
		virtual bool calculatePosition( voip::IVoipCharacter* pMyIVoipCharacter, voip::IVoipCharacter* pIVoipCharacter, ml3DPosition &userPos) = 0;
	};

	/*struct VOIPMAN_NO_VTABLE voipIVoipManCallbacks
	{
		virtual void onConnectSuccess() = 0;
		virtual void onConnectStart() = 0;
		virtual void onConnectFailed() = 0;
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
	};*/

	VOIPMAN_API omsresult CreateVoipManager( oms::omsContext* aContext);
	VOIPMAN_API void DestroyVoipManager( oms::omsContext* aContext);
}
#endif