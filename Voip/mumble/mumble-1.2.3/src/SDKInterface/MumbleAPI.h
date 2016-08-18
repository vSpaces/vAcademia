#if !defined(MUMBLEDLL_H)
#define MUMBLEDLL_H

#define MUMBLE_EXPORTS

#ifdef MUMBLE_EXPORTS
#ifndef MUMBLE_API 
#define MUMBLE_API extern "C" __declspec(dllexport) 
#endif
#else
#define MUMBLE_API __declspec(dllimport)
#endif

#define MUMBLE_SoundSystemType SoundSystem

#define MUMBLE_STRLEN 512
#define MUMBLE_LOG_STRLEN 10240

#define EVO_WRAPPER_SoundSystemType			SoundSystem
#define EVO_STRLEN							MUMBLE_STRLEN

#define SOUND_VOLUME_MAX 255

#include "MumbleWndMessage.h"
#include "MumbleErrorCodes.h"

#define WM_EVOVOIP_CONNECTSUCCESS WM_MUMBLE_CONNECTSUCCESS
#define WM_EVOVOIP_CONNECTFAILED WM_MUMBLE_CONNECTFAILED
#define WM_EVOVOIP_CONNECTIONLOST WM_MUMBLE_CONNECTIONLOST
#define WM_EVOVOIP_ACCEPTED WM_MUMBLE_ACCEPTED
#define WM_EVOVOIP_LOGGEDOUT WM_MUMBLE_LOGGEDOUT
#define WM_EVOVOIP_KICKED WM_MUMBLE_KICKED
#define WM_EVOVOIP_SERVERUPDATE WM_MUMBLE_SERVERUPDATE
#define WM_EVOVOIP_ERROR WM_MUMBLE_ERROR

#define WM_EVOVOIP_ADDUSER WM_MUMBLE_ADDUSER
#define WM_EVOVOIP_REMOVEUSER WM_MUMBLE_REMOVEUSER
#define WM_EVOVOIP_JOINEDCHANNEL WM_MUMBLE_JOINEDCHANNEL
#define WM_EVOVOIP_LEFTCHANNEL WM_MUMBLE_LEFTCHANNEL
#define WM_EVOVOIP_USERTALKING WM_MUMBLE_USERTALKING
#define WM_EVOVOIP_USERSTOPPEDTALKING WM_MUMBLE_USERSTOPPEDTALKING

//unused for compatible
#define WM_EVOVOIP_ADDFILE WM_USER + 473
#define WM_EVOVOIP_REMOVEFILE WM_USER + 474
#define WM_EVOVOIP_CHANNELMESSAGE WM_USER + 462
#define WM_EVOVOIP_TRANSFER_BEGIN WM_USER + 475
#define WM_EVOVOIP_TRANSFER_COMPLETED WM_USER + 476
#define WM_EVOVOIP_TRANSFER_FAILED WM_USER + 477
#define WM_EVOVOIP_USERMESSAGE WM_USER + 461
#define WM_EVOVOIP_CHANNELMESSAGE WM_USER + 462




namespace api {
	typedef wchar_t TTCHAR;

	typedef enum _SoundSystem
	{
		SOUNDSYSTEM_NOSOUND = 0,
		SOUNDSYSTEM_DSOUND = 2,
		SOUNDSYSTEM_WASAPI = 3,
		SOUNDSYSTEM_WINMM = 4, //unsupported
		SOUNDSYSTEM_STREAM = 5
	} SoundSystem;	

	typedef enum _CurrentInputLevelCalculation
	{
		AMPLITUDE = 0,
		SIGNAL_TO_NOISE = 1,
		VACADEM = 2
	} CurrentInputLevelCalculation;

	typedef struct _SoundDevice
	{
		INT32 nDeviceID;
		SoundSystem nSoundSystem;
		TTCHAR szDeviceName[MUMBLE_STRLEN];
		BOOL bSupports3D;
		INT32 nMaxInputChannels;
		INT32 nMaxOutputChannels;
		TTCHAR sGUID[MUMBLE_STRLEN];
	} SoundDevice;

	typedef struct _User
	{
		INT32 nUserID;
		TTCHAR szNickName[MUMBLE_STRLEN];
		INT32 nStatusMode;
		TTCHAR szStatusMsg[MUMBLE_STRLEN];
		INT32 nChannelID;
		TTCHAR szIPAddress[MUMBLE_STRLEN];
		INT32 nUdpPort;
		TTCHAR szVersion[MUMBLE_STRLEN];
		INT32 nFrequency;
		INT32 nPacketsLost;
		INT32 nPacketsCount;
		INT32 nPlayingPacket;
		BOOL bForwarding;
		BOOL bTalking;
		BOOL bAdmin;
	} User;

	
	MUMBLE_API BOOL MUMBLE_IsVoiceActivated(IN LPVOID pClientInstance);
	MUMBLE_API BOOL MUMBLE_EnableVoiceActivation(IN LPVOID pClientInstance, BOOL bEnable);
	MUMBLE_API BOOL MUMBLE_SetVoiceActivationLevel(IN LPVOID pClientInstance, INT32 nLevel);
	MUMBLE_API BOOL MUMBLE_ShutdownSoundSystem(IN LPVOID pClientInstance);
	MUMBLE_API BOOL MUMBLE_LaunchSoundSystem(IN LPVOID pClientInstance, 
		IN SoundSystem nSoundSystem, 
		IN INT32 nInputDeviceID, 
		IN INT32 nOutputDeviceID,
		IN INT32 nRecorderFreq,
		IN INT32 nQuality,
		IN BOOL bStereoPlayback);
	MUMBLE_API BOOL MUMBLE_SetMasterVolume(IN LPVOID pClientInstance, IN INT32 nVolume);
	MUMBLE_API BOOL MUMBLE_SetUserVolume(IN LPVOID pClientInstance,
		IN INT32 nUserID, IN INT32 nVolume);
	MUMBLE_API INT32 MUMBLE_GetMyUserID(IN LPVOID pClientInstance);
	MUMBLE_API BOOL MUMBLE_SetEncoderComplexity(IN LPVOID pClientInstance, 
		IN INT32 nComplexity);
	MUMBLE_API BOOL MUMBLE_IsConnecting(IN LPVOID pClientInstance);
	MUMBLE_API BOOL MUMBLE_Connect(IN LPVOID pClientInstance,
		IN const TTCHAR* lpszHostAddress, 
		IN INT32 nTcpPort, 
		IN INT32 nUdpPort, 
		IN INT32 nLocalTcpPort, 
		IN INT32 nLocalUdpPort);
	MUMBLE_API BOOL MUMBLE_StartTransmitting(IN LPVOID pClientInstance);
	MUMBLE_API BOOL MUMBLE_StopTransmitting(IN LPVOID pClientInstance);
	MUMBLE_API BOOL MUMBLE_SetAGCSettings(IN LPVOID pClientInstance, 
		IN INT32 nGainLevel, 
		IN INT32 nMaxIncrement,
		IN INT32 nMaxDecrement,
		IN INT32 nMaxGain);
	MUMBLE_API BOOL MUMBLE_IsDenoising(IN LPVOID pClientInstance);
	MUMBLE_API BOOL MUMBLE_EnableDenoising(IN LPVOID pClientInstance, 
		IN BOOL bEnable);
	MUMBLE_API BOOL MUMBLE_SetUserPosition(IN LPVOID pClientInstance,
		IN INT32 nUserID, 
		IN float x,
		IN float y, 
		IN float z);
	MUMBLE_API BOOL MUMBLE_SetVoiceGainLevel(IN LPVOID pClientInstance, IN float nLevel);
	MUMBLE_API BOOL MUMBLE_IsAuthorized(IN LPVOID pClientInstance);
	MUMBLE_API BOOL MUMBLE_DoJoinChannel(IN LPVOID pClientInstance,
		IN const TTCHAR* lpszChannelPath, 
		IN const TTCHAR* lpszPassword, 
		IN const TTCHAR* lpszTopic,
		IN const TTCHAR* lpszOpPassword);
	MUMBLE_API BOOL MUMBLE_IsSoundSystemInitialized(IN LPVOID pClientInstance);
	MUMBLE_API BOOL MUMBLE_InitVoIP(IN HWND hWnd, IN BOOL bLLMouseHook, OUT LPVOID* pNewClientInstance, IN SoundSystem nSoundSystem);
	MUMBLE_API BOOL MUMBLE_CloseVoIP(IN LPVOID pClientInstance);
	MUMBLE_API BOOL MUMBLE_DoLeaveChannel(IN LPVOID pClientInstance);
	//MUMBLE_API INT32 MUMBLE_GetCurrentInputLevel(IN LPVOID pClientInstance);
	MUMBLE_API INT32 MUMBLE_GetInputDevicesCount(IN LPVOID pClientInstance, 
		IN SoundSystem nSoundSystem);
	MUMBLE_API BOOL MUMBLE_GetInputDevice(IN LPVOID pClientInstance, 
		IN SoundSystem nSoundSystem, 
		IN INT32 nIndex, 
		OUT SoundDevice* pSoundDevice);
	MUMBLE_API INT32 MUMBLE_GetOutputDevicesCount(IN LPVOID pClientInstance, 
		IN SoundSystem nSoundSystem);
	MUMBLE_API BOOL MUMBLE_GetOutputDevice(IN LPVOID pClientInstance, 
		IN SoundSystem nSoundSystem, 
		IN INT32 nIndex, 
		OUT SoundDevice* pSoundDevice);
	MUMBLE_API BOOL MUMBLE_GetChannelPath(IN LPVOID pClientInstance,
		IN INT32 nChannelID, 
		OUT TTCHAR szChannelPath[MUMBLE_STRLEN]);
	MUMBLE_API BOOL MUMBLE_IsConnected(IN LPVOID pClientInstance);	
	MUMBLE_API BOOL MUMBLE_Disconnect(IN LPVOID pClientInstance);
	MUMBLE_API BOOL MUMBLE_SetUserGainLevel(IN LPVOID pClientInstance,
		IN INT32 nUserID, IN INT32 nGainLevel);
	MUMBLE_API BOOL MUMBLE_GetUser(IN LPVOID pClientInstance,
		IN INT32 nUserID, OUT api::User* lpUser);
	MUMBLE_API BOOL MUMBLE_DoLogin(IN LPVOID pClientInstance,
		IN const TTCHAR* lpszNickName, 
		IN const TTCHAR* lpszServerPassword);
	MUMBLE_API BOOL MUMBLE_RegisterTalkingCallback(IN LPVOID pClientInstance,
                                                     IN TalkingCallback* talkCallback, 
                                                     IN PVOID pUserData);
	MUMBLE_API INT32 MUMBLE_GetVoiceGainLevel(IN LPVOID pClientInstance);
	MUMBLE_API BOOL MUMBLE_GetAGCSettings(IN LPVOID pClientInstance, 
		IN PINT32 lpnGainLevel, 
		OUT PINT32 lpnMaxIncrement,
		OUT PINT32 lpnMaxDecrement,
		OUT PINT32 lpnMaxGain);
	MUMBLE_API INT32 MUMBLE_GetMasterVolume(IN LPVOID pClientInstance);
	MUMBLE_API INT32 MUMBLE_GetVoiceActivationLevel(IN LPVOID pClientInstance);
	MUMBLE_API INT32 MUMBLE_GetEncoderComplexity(IN LPVOID pClientInstance);
	MUMBLE_API BOOL MUMBLE_EnableAGC(IN LPVOID pClientInstance, 
		IN BOOL bEnable);
	MUMBLE_API BOOL MUMBLE_IsForwardingToAll(IN LPVOID pClientInstance);
	MUMBLE_API DOUBLE MUMBLE_GetCurrentInputLevel(IN LPVOID pClientInstance, IN CurrentInputLevelCalculation nInputLevelCalculation=VACADEM);

	MUMBLE_API BOOL MUMBLE_SetProxySettings(IN LPVOID pClientInstance,
		IN const TTCHAR* lpszProxyHostAddress, 
		IN const TTCHAR* lpszProxyUserName, 
		IN const TTCHAR* lpszProxyPassword, 
		IN INT32 nProxyType);

	MUMBLE_API void MUMBLE_StartInputFromDataStream();
	MUMBLE_API void MUMBLE_CreateDataStream(IN const TTCHAR* lpszStreamID);
	MUMBLE_API void MUMBLE_RemoveDataStream(IN const TTCHAR* lpszStreamID);
	MUMBLE_API BOOL MUMBLE_SetDataForDataStream(IN LPVOID pData, IN INT32 aDataSize, IN const TTCHAR* lpszStreamID);

	MUMBLE_API void MUMBLE_RegisterOnConnectSuccessCallback(IN LPVOID pClientInstance, 
		IN EmptyCallback* aCallback);
	MUMBLE_API void MUMBLE_RegisterOnConnectFailedCallback(IN LPVOID pClientInstance, 
		IN EmptyCallback* aCallback);
	MUMBLE_API void MUMBLE_RegisterOnConnectionLostCallback(IN LPVOID pClientInstance, 
		IN EmptyCallback* aCallback);
	MUMBLE_API void MUMBLE_RegisterOnInvalideChannelNameCallback(IN LPVOID pClientInstance, 
		IN EmptyCallback* aCallback);

	MUMBLE_API void MUMBLE_RegisterOnAcceptedCallback(IN LPVOID pClientInstance, 
		IN IntParamCallback* aCallback);
	MUMBLE_API void MUMBLE_RegisterOnUserTalkingCallback(IN LPVOID pClientInstance, 
		IN IntParamCallback* aCallback);
	MUMBLE_API void MUMBLE_RegisterOnUserStoppedTalkingCallback(IN LPVOID pClientInstance, 
		IN IntParamCallback* aCallback);
	MUMBLE_API void MUMBLE_RegisterOnJoinedChannelCallback(IN LPVOID pClientInstance, 
		IN IntParamCallback* aCallback);
	MUMBLE_API void MUMBLE_RegisterOnLeftChannelCallback(IN LPVOID pClientInstance, 
		IN IntParamCallback* aCallback);

	MUMBLE_API void MUMBLE_RegisterOnAddUserCallback(IN LPVOID pClientInstance, 
		IN TwoIntParamCallback* aCallback);

	MUMBLE_API BOOL MUMBLE_RegisterTalkingCallback2(IN LPVOID pClientInstance,
		IN TalkingCallback2* talkCallback, 
		IN PVOID pUserData);

	MUMBLE_API void MUMBLE_RegisterOnRemoveUserCallback(IN LPVOID pClientInstance, 
		IN TwoIntParamCallback* aCallback);

	MUMBLE_API BOOL MUMBLE_SetCommandForDataStream(IN INT32 aCommandID, IN const TTCHAR* lpszStreamID);

	MUMBLE_API int MUMBLE_GetUserChannelInfo(IN INT32 anUserID,  OUT TTCHAR asLog[MUMBLE_LOG_STRLEN], IN INT32 aDataSize, OUT BOOL &aHangAudioThreadDetected);

	MUMBLE_API BOOL MUMBLE_SetEchoCancellation(IN LPVOID pClientInstance, IN BOOL anEnabled);
	MUMBLE_API BOOL MUMBLE_SetMixerEchoCancellation(IN LPVOID pClientInstance, IN BOOL anEnabled, IN SoundSystem nSoundSystem);
	
	MUMBLE_API BOOL MUMBLE_StartFindRealMic(IN LPVOID pClientInstance, IN SoundSystem nSoundSystem);
	MUMBLE_API BOOL MUMBLE_EndFindRealMic(IN LPVOID pClientInstance, OUT TTCHAR asFindedDevice[MUMBLE_STRLEN], OUT TTCHAR asFindedDeviceName[MUMBLE_STRLEN], OUT TTCHAR asFindedDeviceLine[MUMBLE_STRLEN]);
}

typedef std::vector<api::SoundDevice> vecDeviceInfo;
typedef vecDeviceInfo::iterator vecDeviceInfoIt;

#endif //MUMBLEDLL_H