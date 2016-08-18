#pragma once

//////////////////////////////////////////////////////////////////////////
// http://www.conaito.com/show.asp
// R:\VU2008\Alex\VOIP\conaitovoipevoenterprisesdk.zip 
// R:\VU2008\Alex\VOIP\conaitovoipevosdk.zip 
#ifdef USE_CONAITO_SDK_LIB

	#if CONAITO_SDK_LIB_VERSION == 3
		#include "ConaitoSDK\Evo3.0\EvoVoIP.h"
		#pragma comment(lib, "..\\VoipManCommon\\lib\\ConaitoSDK\\Evo3.0\\EvoClient.lib")
	#else
		#include "ConaitoSDK\EvoVoIP.h"
		#pragma comment(lib, "..\\VoipManCommon\\lib\\ConaitoSDK\\evovoip.lib")
	#endif

#if CONAITO_SDK_LIB_VERSION == 3
	#define EVO_WRAPPER_SoundSystemType			SoundSystem
	#define	EVO_WRAPPER_STRLEN					EVO_STRLEN

	#define EVO_WRAPPER_Connect                 EVO_Connect                  
	#define EVO_WRAPPER_IsAuthorized            EVO_IsAuthorized             
	#define EVO_WRAPPER_IsConnected             EVO_IsConnected              
	#define EVO_WRAPPER_IsConnecting            EVO_IsConnecting             
	#define EVO_WRAPPER_Disconnect              EVO_Disconnect               
	#define EVO_WRAPPER_IsVoiceActivated        EVO_IsVoiceActivated         
	#define EVO_WRAPPER_StartTransmitting       EVO_StartTransmitting        
	#define EVO_WRAPPER_StopTransmitting        EVO_StopTransmitting         
	#define EVO_WRAPPER_GetCurrentInputLevel    EVO_GetCurrentInputLevel     
	#define EVO_WRAPPER_InitVoIP				EVO_InitVoIP                 
	#define EVO_WRAPPER_EnableVoiceActivation   EVO_EnableVoiceActivation    
	#define EVO_WRAPPER_SetVoiceActivationLevel EVO_SetVoiceActivationLevel  
	#define EVO_WRAPPER_LaunchSoundSystem       EVO_LaunchSoundSystem        
	#define EVO_WRAPPER_StartSoundLoopbackTest  EVO_StartSoundLoopbackTest        
	#define EVO_WRAPPER_EnableVoiceActivation   EVO_EnableVoiceActivation    
	#define EVO_WRAPPER_SetVoiceActivationLevel EVO_SetVoiceActivationLevel  
	#define EVO_WRAPPER_SetVoiceGainLevel       EVO_SetVoiceGainLevel        
	#define EVO_WRAPPER_EnableDenoising         EVO_EnableDenoising          
	#define EVO_WRAPPER_SetMasterVolume         EVO_SetMasterVolume          
	#define EVO_WRAPPER_SetUserVolume           EVO_SetUserVolume            
	#define EVO_WRAPPER_GetMyUserID             EVO_GetMyUserID              
	#define EVO_WRAPPER_DoLeaveChannel          EVO_DoLeaveChannel           
	#define EVO_WRAPPER_ShutdownSoundSystem     EVO_ShutdownSoundSystem      
	#define EVO_WRAPPER_CloseVoIP				EVO_CloseVoIP                
	#define EVO_WRAPPER_SetVoiceGainLevel       EVO_SetVoiceGainLevel        
	#define EVO_WRAPPER_SetVoiceActivationLevel EVO_SetVoiceActivationLevel  
	#define EVO_WRAPPER_EnableVoiceActivation   EVO_EnableVoiceActivation    
	#define EVO_WRAPPER_GetCurrentInputLevel    EVO_GetCurrentInputLevel     
	#define EVO_WRAPPER_DoLogin                 EVO_DoLogin                  
	#define EVO_WRAPPER_DoJoinChannel           EVO_DoJoinChannel            
	#define EVO_WRAPPER_GetUser                 EVO_GetUser                  
	#define EVO_WRAPPER_GetInputDevicesCount    EVO_GetInputDevicesCount     
	#define EVO_WRAPPER_GetInputDevice          EVO_GetInputDevice           
	#define EVO_WRAPPER_GetOutputDevicesCount   EVO_GetOutputDevicesCount    
	#define EVO_WRAPPER_GetOutputDevice         EVO_GetOutputDevice   
	#define EVO_WRAPPER_GetMyUserID				EVO_GetMyUserID
	#define EVO_WRAPPER_RegisterTalkingCallback		EVO_RegisterTalkingCallback
	#define EVO_WRAPPER_EnableAutoPositioning	ATLASSERT(FALSE);//EVO_EnableAutoPositioning - на SigmaTel Audio - вырубает звук
	#define EVO_WRAPPER_IsAutoPositioning		EVO_IsAutoPositioning
	#define EVO_WRAPPER_AutoPositionUsers		ATLASSERT(FALSE);//EVO_AutoPositionUsers - на SigmaTel Audio - вырубает звук
	#define EVO_WRAPPER_SetUserPosition			EVO_SetUserPosition
	#define EVO_WRAPPER_GetUserPosition			EVO_GetUserPosition
	#define EVO_WRAPPER_SetUserStereo			EVO_SetUserStereo
	#define EVO_WRAPPER_GetUserStereo			EVO_GetUserStereo
	#define EVO_WRAPPER_IsUserTalking			EVO_IsUserTalking
	#define EVO_WRAPPER_SetMixerWaveInSelected	EVO_Mixer_SetWaveInSelected
	#define EVO_WRAPPER_GetMixerCount			EVO_Mixer_GetMixerCount
	#define EVO_WRAPPER_IsSoundSystemInitialized			EVO_IsSoundSystemInitialized

#else
	#define EVO_WRAPPER_SoundSystemType			unsigned int
	#define	EVO_WRAPPER_STRLEN					VOIP_STRLEN

	#define EVO_WRAPPER_Connect                 VOIP_Connect                  
	#define EVO_WRAPPER_IsAuthorized            VOIP_IsAuthorized             
	#define EVO_WRAPPER_IsConnected             VOIP_IsConnected              
	#define EVO_WRAPPER_IsConnecting            VOIP_IsConnecting             
	#define EVO_WRAPPER_Disconnect              VOIP_Disconnect               
	#define EVO_WRAPPER_IsVoiceActivated        VOIP_IsVoiceActivated         
	#define EVO_WRAPPER_StartTransmitting       VOIP_StartTransmitting        
	#define EVO_WRAPPER_StopTransmitting        VOIP_StopTransmitting         
	#define EVO_WRAPPER_GetCurrentInputLevel    VOIP_GetCurrentInputLevel     
	#define EVO_WRAPPER_InitVoIP				VOIP_InitVoIP                 
	#define EVO_WRAPPER_EnableVoiceActivation   VOIP_EnableVoiceActivation    
	#define EVO_WRAPPER_SetVoiceActivationLevel VOIP_SetVoiceActivationLevel  
	#define EVO_WRAPPER_LaunchSoundSystem       VOIP_LaunchSoundSystem        
	#define EVO_WRAPPER_StartSoundLoopbackTest  VOIP_StartSoundLoopbackTest        
	#define EVO_WRAPPER_SetVoiceGainLevel       VOIP_SetVoiceGainLevel        
	#define EVO_WRAPPER_EnableDenoising         VOIP_EnableDenoising          
	#define EVO_WRAPPER_SetMasterVolume         VOIP_SetMasterVolume          
	#define EVO_WRAPPER_SetUserVolume           VOIP_SetUserVolume            
	#define EVO_WRAPPER_GetMyUserID             VOIP_GetMyUserID              
	#define EVO_WRAPPER_DoLeaveChannel          VOIP_DoLeaveChannel           
	#define EVO_WRAPPER_ShutdownSoundSystem     VOIP_ShutdownSoundSystem      
	#define EVO_WRAPPER_CloseVoIP				VOIP_CloseVoIP                
	#define EVO_WRAPPER_SetVoiceGainLevel       VOIP_SetVoiceGainLevel        
	#define EVO_WRAPPER_SetVoiceActivationLevel VOIP_SetVoiceActivationLevel  
	#define EVO_WRAPPER_EnableVoiceActivation   VOIP_EnableVoiceActivation    
	#define EVO_WRAPPER_GetCurrentInputLevel    VOIP_GetCurrentInputLevel     
	#define EVO_WRAPPER_DoLogin                 VOIP_DoLogin                  
	#define EVO_WRAPPER_DoJoinChannel           VOIP_DoJoinChannel            
	#define EVO_WRAPPER_GetUser                 VOIP_GetUser                  
	#define EVO_WRAPPER_GetInputDevicesCount    VOIP_GetInputDevicesCount     
	#define EVO_WRAPPER_GetInputDevice          VOIP_GetInputDevice           
	#define EVO_WRAPPER_GetOutputDevicesCount   VOIP_GetOutputDevicesCount    
	#define EVO_WRAPPER_GetOutputDevice         VOIP_GetOutputDevice
	#define EVO_WRAPPER_GetMyUserID             VOIP_GetMyUserID
	#define EVO_WRAPPER_RegisterTalkingCallback		VOIP_RegisterTalkingCallback
	#define EVO_WRAPPER_EnableAutoPositioning	ATLASSERT(FALSE);//VOIP_EnableAutoPositioning - на SigmaTel Audio - вырубает звук
	#define EVO_WRAPPER_IsAutoPositioning		VOIP_IsAutoPositioning
	#define EVO_WRAPPER_AutoPositionUsers		ATLASSERT(FALSE);//VOIP_AutoPositionUsers - на SigmaTel Audio - вырубает звук
	#define EVO_WRAPPER_SetUserPosition			VOIP_SetUserPosition
	#define EVO_WRAPPER_GetUserPosition			VOIP_GetUserPosition
	#define EVO_WRAPPER_SetUserStereo			VOIP_SetUserStereo
	#define EVO_WRAPPER_GetUserStereo			VOIP_GetUserStereo
	#define EVO_WRAPPER_IsUserTalking			VOIP_IsUserTalking
	//#define EVO_WRAPPER_SetMixerWaveInSelected	VOIP_Mixer_SetWaveInSelected
	#define EVO_WRAPPER_GetMixerCount			VOIP_Mixer_GetMixerCount
#endif

typedef std::vector<SoundDevice> vecDeviceInfo;
typedef vecDeviceInfo::iterator vecDeviceInfoIt;

#endif	// USE_CONAITO_SDK_LIB