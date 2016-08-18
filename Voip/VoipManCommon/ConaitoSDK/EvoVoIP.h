#if !defined(EVOVOIPDLL_H)
#define EVOVOIPDLL_H

#if !defined(_WINDOWS_)
#include <windows.h>	//need HWND and BOOL
#endif

#define EVOVOIP_VERSION "2.2"

#ifdef EVOVOIPDLL_EXPORTS
#define EVOVOIPDLL_API __declspec(dllexport)
#else
#define EVOVOIPDLL_API __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C" {
#endif


	/// \def WM_EVOVOIP_CONNECTSUCCESS
	/// Connected successfully to the server. VOIP_DoLogin can now be called 
	/// in order to logon to the server.
	/// @param wParam Ignored
	/// @param lParam Ignored
	/// @see VOIP_DoLogin
#define WM_EVOVOIP_CONNECTSUCCESS WM_USER + 451

	/// \def WM_EVOVOIP_CONNECTFAILED
	/// Failed to connect to server.
	/// @param wParam Ignored
	/// @param lParam Ignored
#define WM_EVOVOIP_CONNECTFAILED WM_USER + 452

	/// \def WM_EVOVOIP_CONNECTIONLOST
	/// Connection to server has been lost.
	/// @param wParam Ignored
	/// @param lParam Ignored
#define WM_EVOVOIP_CONNECTIONLOST WM_USER + 453

	/// \def WM_EVOVOIP_ACCEPTED
	/// The call to VOIP_DoLogin was successful.
	/// @param wParam Your user ID
	/// @param lParam Ignored
	/// @see VOIP_DoLogin
#define WM_EVOVOIP_ACCEPTED WM_USER + 454

	/// \def WM_EVOVOIP_ADDUSER
	/// A new user has joined the channel.
	/// @param wParam User's ID
	/// @param lParam Channel ID
#define WM_EVOVOIP_ADDUSER WM_USER + 455

	/// \def WM_EVOVOIP_UPDATEUSER
	/// User information has changed.
	/// @param wParam User's ID
	/// @param lParam Channel ID
#define WM_EVOVOIP_UPDATEUSER WM_USER + 456

	/// \def WM_EVOVOIP_REMOVEUSER
	/// User has exited the current channel.
	/// @param wParam User's ID
	/// @param lParam Channel ID
#define WM_EVOVOIP_REMOVEUSER WM_USER + 457

	/// \def WM_EVOVOIP_ADDCHANNEL
	/// A new channel has been created.
	/// @param wParam Channel's ID
	/// @param lParam Ignored
#define WM_EVOVOIP_ADDCHANNEL WM_USER + 458

	/// \def WM_EVOVOIP_UPDATECHANNEL
	/// A channel has been removed.
	/// wParam Channel's ID
	/// lParam Ignored
#define WM_EVOVOIP_UPDATECHANNEL WM_USER + 459

	/// \def WM_EVOVOIP_REMOVECHANNEL
	/// A channel has been removed.
	/// @param wParam Channel's ID
	/// @param lParam Ignored
#define WM_EVOVOIP_REMOVECHANNEL WM_USER + 460

	/// \def WM_EVOVOIP_USERMESSAGE
	/// A private message has been received.
	/// @param wParam The user's user ID.
	/// @param lParam The message's ID
	/// @see GetUserMessage
#define WM_EVOVOIP_USERMESSAGE WM_USER + 461

	/// \def WM_EVOVOIP_CHANNELMESSAGE
	/// A channel message has been received.
	///
	/// wParam Channel's ID
	/// lParam The message's ID
#define WM_EVOVOIP_CHANNELMESSAGE WM_USER + 462

	/// \def WM_EVOVOIP_KICKED
	/// You have been kicked from the server.
	/// @param wParam Ignored
	/// @param lParam Ignored
#define WM_EVOVOIP_KICKED WM_USER + 463

	/// \def WM_EVOVOIP_SERVERUPDATE
	/// Server has updated its settings (server name, motd, etc.)
	/// @param wParam Ignored
	/// @param lParam Ignored
#define WM_EVOVOIP_SERVERUPDATE WM_USER + 464

	/// \def WM_EVOVOIP_JOINEDCHANNEL
	/// You have joined a new channel. You'll afterwards receive the
	/// WM_EVOVOIP_ADDUSER message for each of the users in the channel
	/// Note that WM_EVOVOIP_REMOVEUSER will not be called in order 
	///	to remove the users from the channel you are currently 
	/// participating in.
	/// @param wParam Channel's ID
	/// @param lParam Ignored
	/// @see WM_EVOVOIP_ADDUSER
	/// @see WM_EVOVOIP_REMOVEUSER
#define WM_EVOVOIP_JOINEDCHANNEL WM_USER + 465

	/// \def WM_EVOVOIP_ERROR
	/// Failed to perform a command. Look up the error number in the
	/// TT3CmdErrors.html file in the Server-directory of the SDK.
	/// Alternatively use VOIP_GetErrorMessage to get the error 
	/// description.
	/// @param wParam Error number
	/// @param lParam Ignored
	/// @see VOIP_GetErrorMessage
#define WM_EVOVOIP_ERROR WM_USER + 466

	/// \def WM_EVOVOIP_USERTALKING
	/// A user is talking.
	/// @param wParam User's ID
	/// @param lParam Ignored
#define WM_EVOVOIP_USERTALKING WM_USER + 467

	/// \def WM_EVOVOIP_USERSTOPPEDTALKING
	/// A user has stopped talking.
	/// @param wParam User's ID
	/// @param lParam Ignored
#define WM_EVOVOIP_USERSTOPPEDTALKING WM_USER + 468

	/// \def WM_EVOVOIP_DIRCONNECTIONFAILED
	/// Establishing direct connection to a user failed and 
	/// voice data must instead be forward through server.
	/// @param wParam User's ID
	/// @param lParam Ignored
#define WM_EVOVOIP_DIRCONNECTIONFAILED WM_USER + 469

	/// \def WM_EVOVOIP_LISTCOMMAND_COMPLETED
	/// A list command issued by TT*_Do* has completed.
	/// @param wParam Ignored
	/// @param lParam Ignored
	/// @see VOIP_DoLogin
	/// @see VOIP_DoJoinChannel
	/// @see VOIP_Admin_DoLogin
	/// @see VOIP_Admin_DoListBans
#define WM_EVOVOIP_LISTCOMMAND_COMPLETED WM_USER + 470

	/// \def WM_EVOVOIP_LEFTCHANNEL
	/// This message is posted when leaving a channel. The WPARAM
	/// contains the channel ID. Note that you may not be able to retrieve
	/// the channel you left if it's destroyed just after you leave it.
	///
	/// This message was added to support DoLeaveChannel. 
	/// The server needs to support TCP protocol 3.4 for this
	/// message to be posted.
	/// @param wParam Channel's ID
	/// @param lParam Ignored
	/// @see VOIP_DoLeaveChannel
	/// @see VOIP_DoJoinChannel
#define WM_EVOVOIP_LEFTCHANNEL WM_USER + 471

	/// \def WM_EVOVOIP_LOGGEDOUT
	/// This message is posted when you log out of a server. In
	/// response to VOIP_DoLogout
	///
	/// The server needs to support TCP protocol 3.4 for this
	/// message to be posted.
	/// @param wParam Ignored
	/// @param lParam Ignored
	/// @see VOIP_DoLogout
#define WM_EVOVOIP_LOGGEDOUT WM_USER + 472

	/// \def WM_EVOVOIP_ADDFILE
	/// This message is posted when a new file is added to the
	/// channel a user is participating in. WPARAM contains
	/// the channel ID and LPARAM contains the file ID. Note
	/// that the file ID is not specific to the channel but
	/// is a global ID.
	///
	/// Use VOIP_Admin_GetChannelFileInfo or VOIP_GetChannelFileInfo to get 
	/// information about the file.
	/// @param wParam Channel ID
	/// @param lParam File ID
	/// @see VOIP_Admin_GetChannelFileInfo
	/// @see VOIP_GetChannelFileInfo
#define WM_EVOVOIP_ADDFILE WM_USER + 473

	/// \def WM_EVOVOIP_REMOVEFILE
	/// This message is posted when a file is removed from the
	/// channel a user is participating in. WPARAM contains
	/// the channel ID and LPARAM contains the file ID.
	///
	/// Note that you may not be able to retrieve information
	/// about the file being removed because it's an async call.
	/// @param wParam Channel ID
	/// @param lParam File ID
#define WM_EVOVOIP_REMOVEFILE WM_USER + 474

	/// \def WM_EVOVOIP_TRANSFER_BEGIN
	/// This message is posted when a new file transfer is
	/// started. WPARAM contains the transfer id which can
	/// be passed to VOIP_Admin_GetFileTransferInfo.
	///
	/// @param wParam Transfer ID
	/// @param lParam Ignored
	/// @see VOIP_Admin_GetFileTransferInfo
#define WM_EVOVOIP_TRANSFER_BEGIN WM_USER + 475

	/// \def WM_EVOVOIP_TRANSFER_COMPLETED
	/// This message is posted when a file transfer is
	/// completed. WPARAM contains the transfer id.
	///
	/// @param wParam Transfer ID
	/// @param lParam Ignored
#define WM_EVOVOIP_TRANSFER_COMPLETED WM_USER + 476

	/// \def WM_EVOVOIP_TRANSFER_FAILED
	/// This message is posted if a file transfer
	/// fails. WPARAM contains the transfer id. A file
	/// transfer which was rejected by the server will
	/// have a transfer id of 0. LPARAM contains the
	/// error number. Use VOIP_GetErrorMessage to get the
	/// error description.
	/// @param wParam Transfer ID
	/// @param lParam Ignored
	/// @see VOIP_GetErrorMessage
#define WM_EVOVOIP_TRANSFER_FAILED WM_USER + 477

	/// \def WM_EVOVOIP_BROADCASTMESSAGE
	/// This message is posted if an admin has sent a broadcast
	/// text message to all users on the server. WPARAM contains
	/// user ID and LPARAM contains message ID.
	///
	/// Broadcast message were introduced in TCP protocol
	/// version 3.4.
	/// @param wParam The user's ID.
	/// @param lParam The message's ID
	/// @see VOIP_GetBroadcastMessage
#define WM_EVOVOIP_BROADCASTMESSAGE WM_USER + 478

	/// \def WM_EVOVOIP_ADMINMESSAGE
	/// This message is posted if an admin has sent you a
	/// text message. The message is extracted in the same
	/// way as regular user messages.
	///
	/// Admin messages were introduced in TCP protocol
	/// version 3.4.
	/// @param wParam The user's ID.
	/// @param lParam The message's ID
	/// @see VOIP_GetUserMessage
#define WM_EVOVOIP_ADMINMESSAGE WM_USER + 479

	/// \def WM_EVOVOIP_USERLOGGEDIN
	/// This message is posted when a client logs on to a
	/// server (successful call to VOIP_DoLogin). Use
	/// VOIP_Admin_GetUser to get information about the user.
	///
	/// @param wParam The user's ID.
	/// @param lParam Unused
	/// @see VOIP_DoLogin
	/// @see VOIP_Admin_GetUser
	/// @see WM_EVOVOIP_USERLOGGEDOUT
#define WM_EVOVOIP_USERLOGGEDIN WM_USER + 480

	/// \def WM_EVOVOIP_USERLOGGEDOUT
	/// This message is posted when a client logs out of a
	/// server (successful call to VOIP_DoLogout). This message
	/// is also called with the user disconnects with
	/// VOIP_Disconnect
	///
	/// @param wParam The user's ID.
	/// @param lParam Unused
	/// @see VOIP_DoLogout
	/// @see VOIP_Disconnect
	/// @see WM_EVOVOIP_USERLOGGEDIN
#define WM_EVOVOIP_USERLOGGEDOUT WM_USER + 481

	/// \def WM_EVOVOIP_HOTKEY
	/// @param wParam = hotkey ID
	/// @param lParam is TRUE when hotkey is active and FALSE when 
	/// it becomes inactive.
	/// @see VOIP_RegisterHotKey
	/// @see VOIP_UnregisterHotKey
#define WM_EVOVOIP_HOTKEY WM_USER + 500

	/// \def WM_EVOVOIP_KEYTEST
	/// @param wParam is the virtual key code.
	/// @param lParam is the TRUE when key is down and 
	/// FALSE when released.
#define WM_EVOVOIP_KEYTEST WM_USER + 501

	/// \def SOUNDSYSTEM_WINMM
	/// Initialize VOIP_LaunchSoundSystem with SOUNDSYSTEM_WINMM 
	/// to use Windows sound system.
	/// @see VOIP_LaunchSoundSystem
#define SOUNDSYSTEM_WINMM 1

	/// \def SOUNDSYSTEM_DSOUND
	/// Initialize VOIP_LaunchSoundSystem with SOUNDSYSTEM_DSOUND 
	/// to use DirectSound.
	/// @see VOIP_LaunchSoundSystem
#define SOUNDSYSTEM_DSOUND 2

	/// \def ENC_QUALITY_MAX
	/// Encoder quality to use. Higher value will increase CPU 
	/// usage and voice quality.
	/// @see VOIP_LaunchSoundSystem
#define ENC_QUALITY_MAX 10

	/// \def ENC_QUALITY_DEFAULT
	/// The default voice quality is 2 which is fine for most 
	/// conversations.
	/// @see VOIP_LaunchSoundSystem
#define ENC_QUALITY_DEFAULT 2

	/// \def ENC_QUALITY_MIN
	/// Encoder quality to use. Higher value will increase CPU 
	/// usage and voice quality.
	/// @see VOIP_LaunchSoundSystem
#define ENC_QUALITY_MIN 1

	/// \def SOUND_VU_MAX
	/// The maximum value of recorded audio.
	/// @see VOIP_GetCurrentInputLevel
	/// @see VOIP_SetVoiceActivationLevel
	/// @see VOIP_GetVoiceActivationLevel
#define SOUND_VU_MAX 20

	/// \def SOUND_VU_MIN
	/// The minimum value of recorded audio.
	/// @see VOIP_GetCurrentInputLevel
	/// @see VOIP_SetVoiceActivationLevel
	/// @see VOIP_GetVoiceActivationLevel
#define SOUND_VU_MIN 0

	/// \def SOUND_VOLUME_MAX
	/// The maximum volume for master volume and user.
	/// @see VOIP_SetMasterVolume
	/// @see VOIP_GetMasterVolume
	/// @see VOIP_SetUserVolume
	/// @see VOIP_GetUserVolume
#define SOUND_VOLUME_MAX 255

	/// \def SOUND_VOLUME_MIN
	/// The minimum volume for master volume and user.
	/// @see VOIP_SetMasterVolume
	/// @see VOIP_GetMasterVolume
	/// @see VOIP_SetUserVolume
	/// @see VOIP_GetUserVolume
#define SOUND_VOLUME_MIN 0

	/// \def GAIN_LEVEL_MAX
	/// The maximum gain level. A gain level of 8000 gains the 
	/// volume by a factor eight.
	/// A gain level of 1000 means no gain.
	/// @see VOIP_SetVoiceGainLevel
	/// @see VOIP_GetVoiceGainLevel
	/// @see VOIP_SetUserGainLevel
	/// @see VOIP_GetUserGainLevel
#define GAIN_LEVEL_MAX 8000

	/// \def GAIN_LEVEL_DEFAULT
	/// The default gain level. A gain level of 1000 means no gain.
	/// Check GAIN_LEVEL_MAX and GAIN_LEVEL_MIN to see how to 
	/// increase and lower gain level.
	/// @see VOIP_SetVoiceGainLevel
	/// @see VOIP_GetVoiceGainLevel
	/// @see VOIP_SetUserGainLevel
	/// @see VOIP_GetUserGainLevel
#define GAIN_LEVEL_DEFAULT 1000


	/// \def GAIN_LEVEL_MIN
	/// The minimum gain level. A gain level of 100 is 1/10 of
	/// of the normal volume (new volume = gain level / 1000).
	/// @see VOIP_SetVoiceGainLevel
	/// @see VOIP_GetVoiceGainLevel
	/// @see VOIP_SetUserGainLevel
	/// @see VOIP_GetUserGainLevel
#define GAIN_LEVEL_MIN 100

	/// \def COMPLEXITY_MAX
	/// The maximum complexity used by the audio encoder.
	/// The complexity setting determines how much time the
	///	the encoder is allowed to use to encode an audio frame.
#define COMPLEXITY_MAX 10

		/// \def COMPLEXITY_DEFAULT
	/// The default complexity used by the audio encoder.
	/// The complexity setting determines how much time the
	///	the encoder is allowed to use to encode an audio frame.
#define COMPLEXITY_DEFAULT 2

	/// \def COMPLEXITY_MIN
	/// The minimum complexity used by the audio encoder.
	/// The complexity setting determines how much time the
	///	the encoder is allowed to use to encode an audio frame.
#define COMPLEXITY_MIN 1

	/// \def USERSTATUS_AVAILABLE
	/// User's status mode is set to available. 
	/// @see VOIP_GetUser
	/// @see VOIP_DoChangeStatus
#define USERSTATUS_AVAILABLE 0

	/// \def USERSTATUS_AWAY
	/// User's status mode is set to away.
	/// @see VOIP_GetUser
	/// @see VOIP_DoChangeStatus
#define USERSTATUS_AWAY 1

	/// \def VOIP_STRLEN
	/// The length of a string (characters, not bytes) which is used 
	/// to extract information from this DLL. To support
	/// Unicode you'll need to convert Unicode strings to UTF-8 on 
	/// Win32. Look up WideCharToMultiByte(.) and
	/// MultiByteToWideChar(.) in the Win32-documentation. On Windows
	/// CE this convertion is done automatically, since it's a Unicode
	/// OS.
#define VOIP_STRLEN 512

	/// A struct to hold a banned user.
	/// This structure is use by VOIP_Admin_GetBannedUsers
	/// @see VOIP_Admin_GetBannedUsers
	typedef struct _BannedUser
	{
		CHAR szIpAddress[VOIP_STRLEN];
		CHAR szChannelPath[VOIP_STRLEN];
		CHAR szTime[VOIP_STRLEN];
		CHAR szNick[VOIP_STRLEN];
	} BannedUser;

	/// A struct for holding a channel.
	/// This structure is used by both the client and admin.
	/// As client the field szPassword will not be filled but
	/// instead the bPassword field.
	/// @see VOIP_Admin_GetChannel
	/// @see VOIP_GetChannel
	typedef struct _Channel
	{
		INT32 nParentID;
		INT32 nChannelID;
		CHAR szName[VOIP_STRLEN];
		CHAR szTopic[VOIP_STRLEN];
		CHAR szPassword[VOIP_STRLEN];	//only set in VOIP_Admin_*
		BOOL bPassword;
		BOOL bStatic;
		INT64 nDiskQuota;
		CHAR szOpPassword[VOIP_STRLEN];	//only set in VOIP_Admin_*
		INT32 nMaxUsers;
	} Channel;

	/// A struct for holding a user's information.
	/// This structure is used by both the client and admin.
	/// @see VOIP_GetUser
	/// @see VOIP_Admin_GetUser
	typedef struct _User
	{
		INT32 nUserID;
		CHAR szNickName[VOIP_STRLEN];
		INT32 nStatusMode;
		CHAR szStatusMsg[VOIP_STRLEN];
		INT32 nChannelID;
		CHAR szAddress[VOIP_STRLEN];
		INT32 nUdpPort;
		CHAR szVersion[VOIP_STRLEN];
		INT32 nFrequency;
		INT32 nPacketsLost;
		INT32 nPacketsCount;
		INT32 nPlayingPacket;
		BOOL bForwarding;
		BOOL bTalking;
		BOOL bAdmin;
	} User;

	/// A struct for holding a file transfer's information.
	/// @see VOIP_GetFileTransferInfo
	/// @see VOIP_Admin_GetFileTransferInfo
	typedef struct _FileTransfer
	{
		INT32 nTransferID;
		INT32 nChannelID;
		CHAR szLocalFilePath[VOIP_STRLEN];
		CHAR szRemoteFileName[VOIP_STRLEN];
		INT64 nFileSize;
		INT64 nTransferred;
		BOOL bInbound; //TRUE if download and FALSE if upload
	} FileTransfer;

	/// A struct for holding information about a file.
	/// @see VOIP_GetChannelFileInfo
	/// @see VOIP_Admin_GetChannelFileInfo
	typedef struct _FileInfo
	{
		INT32 nFileID;
		CHAR szFileName[VOIP_STRLEN];
		INT64 nFileSize;
	} FileInfo;

	/// A struct for holding information about a server. This structure
	/// is only used by the admin.
	/// @see VOIP_Admin_GetServerProperties
	typedef struct _ServerProperties
	{
		CHAR szServerName[VOIP_STRLEN];
		CHAR szServerPasswd[VOIP_STRLEN];
		CHAR szMOTD[VOIP_STRLEN];
		CHAR szOpMePassword[VOIP_STRLEN];
		BOOL bAllowChannels;
		BOOL bAllowOperators;
		INT32 nMaxUsers;
		BOOL bAutoSave;
		BOOL bShowAllUsers;
	} ServerProperties;

	/// An enum for the different playback modes. Experimental!
	typedef enum _PlaybackMode
	{
		PLAYBACKMODE_STABLE = 0,
		PLAYBACKMODE_LOWLATENCY = 1,
		PLAYBACKMODE_VARIABLE = 2,
		PLAYBACKMODE_JITTER = 3,
	} PlaybackMode;

	/// Initialize a new VoIP client. This function must be invoked before any
	/// other of the VOIP_* functions can be called. Call VOIP_CloseVoIP to
	/// shutdown the VoIP client and release its resources.
	///
	/// @param hWnd The window handle which will receive the WM_EVOVOIP_* 
	/// messages.
	/// @param bLLMouseHook Install low-level mouse. When debugging it's best to
	/// to disable it otherwise the mouse cursor will be slow.
	/// @param pNewClientInstance Pointer to new a client instance. To destroy the
	/// new client instance called VOIP_CloseVoIP.
	/// @see VOIP_CloseVoIP
	EVOVOIPDLL_API BOOL VOIP_InitVoIP(IN HWND hWnd, IN BOOL bLLMouseHook, OUT LPVOID* pNewClientInstance);

	/// Close the VoIP client and release its resources.
	/// You'll need to call VOIP_InitVoIP before you call 
	/// any of the other VOIP_* functions.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see VOIP_InitVoIP
	EVOVOIPDLL_API BOOL VOIP_CloseVoIP(IN LPVOID pClientInstance);

	/// Get the number of input devices.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nSoundSystem the sound system to examine.
	/// @see SOUNDSYSTEM_DSOUND
	/// @see SOUNDSYSTEM_WINMM
	/// @see VOIP_GetInputDevice
	EVOVOIPDLL_API INT32 VOIP_GetInputDevicesCount(IN LPVOID pClientInstance, 
		IN INT32 nSoundSystem);

	/// Get information about an input device. Note that DirectSound is 
	/// HIGHLY recommended.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nSoundSystem The sound system to examine. Can be either 
	/// SOUNDSYSTEM_DSOUND or SOUNDSYSTEM_WINMM
	/// @param nIndex The index of the device to extract.
	/// @param szName Will receive the name of the device.
	/// @param lpnDeviceID Will receive the device's ID
	/// @see SOUNDSYSTEM_DSOUND
	/// @see SOUNDSYSTEM_WINMM
	/// @see VOIP_GetInputDevicesCount
	EVOVOIPDLL_API BOOL VOIP_GetInputDevice(IN LPVOID pClientInstance, 
		IN INT32 nSoundSystem, 
		IN INT32 nIndex, 
		OUT CHAR szName[VOIP_STRLEN], 
		OUT INT32* lpnDeviceID);

	/// Get the number of output devices.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nSoundSystem the sound system to examine. 
	/// @see SOUNDSYSTEM_DSOUND
	/// @see SOUNDSYSTEM_WINMM
	/// @see VOIP_GetOutputDevice
	EVOVOIPDLL_API INT32 VOIP_GetOutputDevicesCount(IN LPVOID pClientInstance, 
		IN INT32 nSoundSystem);

	/// Get information about an input device.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nSoundSystem the sound system to examine.
	/// @param nIndex the index of the device to extract.
	/// @param szName will receive the name of the device.
	/// @param lpbSupports3D will receive whether the device supports 3D audio.
	/// @param lpnDeviceID will receive the device's ID
	/// @see SOUNDSYSTEM_DSOUND
	/// @see SOUNDSYSTEM_WINMM
	/// @see VOIP_GetOutputDevicesCount
	EVOVOIPDLL_API BOOL VOIP_GetOutputDevice(IN LPVOID pClientInstance, 
		IN INT32 nSoundSystem, 
		IN INT32 nIndex, 
		OUT CHAR szName[VOIP_STRLEN], 
		OUT BOOL* lpbSupports3D, 
		OUT INT32* lpnDeviceID);

	/// Launch the sound system.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nSoundSystem Choose SOUNDSYSTEM_DSOUND for DirectSound 
	/// and SOUNDSYSTEM_WINMM for Windows sound system. DirectSound
	/// is highly recommended on Win32 whereas WinCE only supports Windows
	/// sound system (SOUNDSYSTEM_WINMM).
	/// @param nInputDeviceID Should be the device ID extracted through 
	/// VOIP_GetInputDevice.
	/// Pass -1 to use the default device.
	/// @param nOutputDeviceID Should be the device ID extracted through 
	/// VOIP_GetOutputDevice.
	/// Pass -1 to use the default device.
	/// @param nRecorderFreq The frequency the client's recorder should use. 
	/// It can either be
	/// 8000 Hz, 16000 Hz or 32000 Hz.
	/// @param nQuality is the quality that the client's encoder should use. 
	/// A value between 1 and 10 is
	/// accepted. 4 gives an acceptable voice quality for most use.
	/// @see SOUNDSYSTEM_DSOUND
	/// @see SOUNDSYSTEM_WINMM
	/// @see VOIP_GetInputDevice
	/// @see VOIP_GetOutputDevice
	/// @see ENC_QUALITY_MAX
	/// @see ENC_QUALITY_MIN
	EVOVOIPDLL_API BOOL VOIP_LaunchSoundSystem(IN LPVOID pClientInstance, 
		IN INT32 nSoundSystem, 
		IN INT32 nInputDeviceID, 
		IN INT32 nOutputDeviceID,
		IN INT32 nRecorderFreq,
		IN INT32 nQuality);

	/// Whether sound system has been initialized successfully.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see VOIP_LaunchSoundSystem
	/// @see VOIP_ShutdownSoundSystem
	EVOVOIPDLL_API BOOL VOIP_IsSoundSystemInitialized(IN LPVOID pClientInstance);

	/// Shutdown the sound system.
	///
	/// Remember to disconnect from the server and shutdown the 
	/// sound system when you quit your application.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see VOIP_LaunchSoundSystem
	EVOVOIPDLL_API BOOL VOIP_ShutdownSoundSystem(IN LPVOID pClientInstance);

	/// Restart the sound system
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	EVOVOIPDLL_API BOOL VOIP_RestartSoundSystem(IN LPVOID pClientInstance, 
		IN INT32 nSoundSystem, 
		IN INT32 nInputDeviceID, 
		IN INT32 nOutputDeviceID);

	/// Experimental
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	EVOVOIPDLL_API BOOL VOIP_SetEncoderComplexity(IN LPVOID pClientInstance, 
		IN INT32 nComplexity);

	/// Experimental
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	EVOVOIPDLL_API INT32 VOIP_GetEncoderComplexity(IN LPVOID pClientInstance);


	/// Perform a record and playback test of specified sound devices.
	///
	/// Note that this test cannot be performed while the sound system 
	/// is launch.
	/// Call VOIP_StopSoundLoopbackTest() to stop the loopback test.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nSoundSystem SOUNDSYSTEM_WINMM for Windows audio, 
	/// SOUNDSYSTEM_DSOUND for DirectSound
	/// @param nInputDeviceID Should be the device ID extracted through 
	/// VOIP_GetInputDevice.
	/// Pass -1 to use the default device.
	/// @param nOutputDeviceID Should be the device ID extracted through 
	/// VOIP_GetOutputDevice.
	/// Pass -1 to use the default device.
	/// @param nRecorderFreq The frequency the client's recorder should 
	/// use. It can either be
	/// 8000 Hz, 16000 Hz or 32000 Hz.
	/// @see VOIP_LaunchSoundSystem
	/// @see VOIP_StopSoundLoopbackTest
	EVOVOIPDLL_API BOOL VOIP_StartSoundLoopbackTest(IN LPVOID pClientInstance, 
		IN INT32 nSoundSystem,
		IN INT32 nInputDeviceID, 
		IN INT32 nOutputDeviceID,
		IN INT32 nRecorderFreq);

	/// Stop recorder and playback test.
	///
	/// Don't call this function if you have initialized the sound
	/// system through VOIP_LaunchSoundSystem.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see VOIP_LaunchSoundSystem
	EVOVOIPDLL_API BOOL VOIP_StopSoundLoopbackTest(IN LPVOID pClientInstance);

	/// Get the number of samples required for VoIP to send
	/// a packet using the specified recorder frequency and quality.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nRecorderFreq Should be either 8000, 16000 or 32000 Hz.
	/// @param nQuality A value between ENC_QUALITY_MIN and ENC_QUALITY_MAX
	/// @see ENC_QUALITY_MIN
	/// @see ENC_QUALITY_MAX
	EVOVOIPDLL_API INT32 VOIP_GetSamplesPerPacket(IN INT32 nRecorderFreq, 
		IN INT32 nQuality);

	/// Get the number of bytes in a packet with the specified settings.
	/// 
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nRecorderFreq Should be either 8000, 16000 or 32000 Hz.
	/// @param nQuality A value between ENC_QUALITY_MIN and ENC_QUALITY_MAX
	/// @see ENC_QUALITY_MIN
	/// @see ENC_QUALITY_MAX
	EVOVOIPDLL_API INT32 VOIP_GetPacketSize(IN INT32 nRecorderFreq, IN INT32 nQuality);

	/// Get the value of the currently recorded audio.
	///
	/// Returns a value between SOUND_VU_MIN and SOUND_VU_MAX
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see SOUND_VU_MIN
	/// @see SOUND_VU_MAX
	EVOVOIPDLL_API INT32 VOIP_GetCurrentInputLevel(IN LPVOID pClientInstance);

	/// Set master volume. Note that it does not affect the Windows mixer.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see SOUND_VOLUME_MAX
	/// @see SOUND_VOLUME_MIN
	EVOVOIPDLL_API BOOL VOIP_SetMasterVolume(IN LPVOID pClientInstance, IN INT32 nVolume);

	/// Get master volume.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see SOUND_VOLUME_MAX
	/// @see SOUND_VOLUME_MIN
	/// @return Returns the master volume.
	EVOVOIPDLL_API INT32 VOIP_GetMasterVolume(IN LPVOID pClientInstance);

	/// Set all users mute.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	EVOVOIPDLL_API BOOL VOIP_MuteAll(IN LPVOID pClientInstance, BOOL bMuteAll);

	/// Is all users set to mute.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	EVOVOIPDLL_API BOOL VOIP_IsAllMute(IN LPVOID pClientInstance);

	/// Enable denoising of outgoing audio
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see VOIP_IsDenoising
	EVOVOIPDLL_API BOOL VOIP_EnableDenoising(IN LPVOID pClientInstance, IN BOOL bEnable);

	/// Is denoising outgoing audio
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see VOIP_EnableDenoising
	EVOVOIPDLL_API BOOL VOIP_IsDenoising(IN LPVOID pClientInstance);

	/// Set voice gaining of recorded audio. 
	/// The gain level ranges from 100 - 4000 where 1000 is no gain. 
	/// So 100 is 1/10 of the original volume and 4000 is 4 times the 
	/// original volume.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see VOIP_GetVoiceGainLevel
	EVOVOIPDLL_API BOOL VOIP_SetVoiceGainLevel(IN LPVOID pClientInstance, IN INT32 nLevel);

	/// Get voice gain level of outgoing audio
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see VOIP_SetVoiceGainLevel
	EVOVOIPDLL_API INT32 VOIP_GetVoiceGainLevel(IN LPVOID pClientInstance);

	/// Enable voice activation.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	EVOVOIPDLL_API BOOL VOIP_EnableVoiceActivation(IN LPVOID pClientInstance, BOOL bEnable);

	/// Whether voice activation is enabled.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	EVOVOIPDLL_API BOOL VOIP_IsVoiceActivated(IN LPVOID pClientInstance);

	/// Set voice activation level.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nLevel must be between SOUND_VU_MIN and SOUND_VU_MAX
	/// @see SOUND_VU_MIN 
	/// @see SOUND_VU_MAX
	EVOVOIPDLL_API BOOL VOIP_SetVoiceActivationLevel(IN LPVOID pClientInstance, INT32 nLevel);

	/// Get voice activation level.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @return Returns a value between SOUND_VU_MIN and  SOUND_VU_MAX
	/// @see SOUND_VU_MIN 
	/// @see SOUND_VU_MAX
	EVOVOIPDLL_API INT32 VOIP_GetVoiceActivationLevel(IN LPVOID pClientInstance);

	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see VOIP_SetUserPosition
	EVOVOIPDLL_API BOOL VOIP_EnableAutoPositioning(IN LPVOID pClientInstance, IN BOOL bEnable);

	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see VOIP_SetUserPosition
	EVOVOIPDLL_API BOOL VOIP_IsAutoPositioning(IN LPVOID pClientInstance);

	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see VOIP_SetUserPosition
	EVOVOIPDLL_API BOOL VOIP_AutoPositionUsers(IN LPVOID pClientInstance);

	/// Set the playback mode used for playing user's sound.
	///
	/// There's four ways to play sound from users:
	/// PLAYBACKMODE_STABLE uses a very defensive approach for playback 
	/// where it tries to play all audio packets which are received from 
	/// a client. 
	/// PLAYBACKMODE_LOWLATENCY uses a very agressive approach for playback 
	/// where it tries to keep the playback buffer empty by skipping packets 
	/// if needed.
	/// PLAYBACKMODE_VARIABLE uses variable playback rate which increases 
	/// and decreases the samplerate of the user's sound to prevent either a
	/// buffer overflow or buffer underflow of incoming packets.
	/// PLAYBACKMODE_JITTER uses a jitter buffer to ensure real-time playback of
	/// user's sound.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param mode See PlaybackMode enumeration for types of playback
	/// @see PlaybackMode
	/// @see VOIP_GetPlaybackMode
	EVOVOIPDLL_API BOOL VOIP_SetPlaybackMode(IN LPVOID pClientInstance, 
		IN PlaybackMode mode);

	/// The currently selected playback mode.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @return See PlaybackMode enumeration for types of playback
	/// @see VOIP_SetPlaybackMode
	/// @see PlaybackMode
	EVOVOIPDLL_API PlaybackMode VOIP_GetPlaybackMode(IN LPVOID pClientInstance);

	/// Enable echo cancellation (experimental). Still not working properly.
	///
	/// Note that it's very CPU intensive.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	EVOVOIPDLL_API BOOL VOIP_EnableEchoCancellation(IN LPVOID pClientInstance, 
		IN BOOL bEnable);

	/// Whether echo cancellation is active. Default is false.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	EVOVOIPDLL_API BOOL VOIP_IsEchoCancelling(IN LPVOID pClientInstance);

	/// Connect to a server. This is a non-blocking call, 
	/// the message WM_EVOVOIP_CONNECTSUCCESS will be sent once 
	/// the connection has been established or WM_EVOVOIP_CONNECTFAILED
	/// if connection could not be established.
	/// Sound system must be initialized before you connect
	/// 
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param lpszHostAddress The IP-address or hostname of the server.
	/// @param nTcpPort The host port of the server (TCP).
	/// @param nUdpPort The sound port of the server (UDP).
	/// @param nLocalTcpPort The local Tcp port which should be used. 
	/// Setting it to 0 makes Windows select a port number (recommended).
	/// @param nLocalUdpPort The local Udp port which should be used. 
	/// Setting it to 0 makes Windows select a port number (recommended).
	/// @see WM_EVOVOIP_CONNECTSUCCESS
	/// @see WM_EVOVOIP_CONNECTFAILED
	EVOVOIPDLL_API BOOL VOIP_Connect(IN LPVOID pClientInstance,
		IN LPCSTR lpszHostAddress, 
		IN INT32 nTcpPort, 
		IN INT32 nUdpPort, 
		IN INT32 nLocalTcpPort, 
		IN INT32 nLocalUdpPort);

	/// Whether the client is connected to a server.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	EVOVOIPDLL_API BOOL VOIP_IsConnected(IN LPVOID pClientInstance);

	/// Whether the client is currently connecting to a server.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	EVOVOIPDLL_API BOOL VOIP_IsConnecting(IN LPVOID pClientInstance);

	/// Disconnect from a server.
	/// 
	/// Remember to disconnect from the server and afterwards 
	/// shutdown the sound system when you quit your application.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	EVOVOIPDLL_API BOOL VOIP_Disconnect(IN LPVOID pClientInstance);

	/// Whether the client is authorized on the server (has received the
	/// WM_EVOVOIP_ACCEPTED message). Use VOIP_DoLogin(..) to log on to
	/// server and VOIP_DoLogout() to log out of the server.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see WM_EVOVOIP_ACCEPTED
	EVOVOIPDLL_API BOOL VOIP_IsAuthorized(IN LPVOID pClientInstance);

	/// Logon to a VoIP server.
	/// 
	/// Once connected to a server call VOIP_DoLogin(..) to logon. If the 
	/// login is successful WM_EVOVOIP_ACCEPTED is posted to your HWND, 
	/// otherwise WM_EVOVOIP_ERROR. Once logged on you're not in a channel. 
	/// Call VOIP_DoJoinChannel to join a channel.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param lpszNickName is the nick name to use.
	/// @param lpszServerPassword is the server's password.
	/// @see VOIP_DoJoinChannel
	/// @see WM_EVOVOIP_ACCEPTED
	/// @see WM_EVOVOIP_ERROR
	EVOVOIPDLL_API BOOL VOIP_DoLogin(IN LPVOID pClientInstance,
		IN LPCSTR lpszNickName, 
		IN LPCSTR lpszServerPassword);

	/// Logout of a server.
	///
	/// If successful the message WM_EVOVOIP_LOGGEDOUT will be posted.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see WM_EVOVOIP_LOGGEDOUT
	EVOVOIPDLL_API BOOL VOIP_DoLogout(IN LPVOID pClientInstance);

	/// VOIP_DoJoinChannel(...) is used for joining a channel but also creating
	/// a new channel if the server allows it. Whether users should be allowed
	/// to create channels can be configured in the server's .xml file. If
	/// VOIP_DoJoinChannel is being used to join a channel the parameters 
	/// "lpszTopic" and "lpszOpPassword" are ignored.
	///
	/// When VOIP_DoJoinChannel(...) is used to create channels it work a bit like
	/// IRC. If you try to join a channel which does not exist it will be 
	/// created as a new channel. If you are the last user to leave a channel
	/// the channel will be removed on the server (unless it's a 
	/// static channel). In the server's configuration file you can see how to set
	/// up static channels and whether users should be allowed to create channels. 
	/// Subchannels are separated by '/' so to create a channel 
	/// called "foo" with the password "bar" and the topic "FooBar" you call 
	/// VOIP_DoJoinChannel in the following way: VOIP_DoJoinChannel("/foo", "bar", "FooBar").
	/// 
	/// If the channel is created successfully the message WM_EVOVOIP_ADDCHANNEL
	/// will be sent, followed by WM_EVOVOIP_JOINEDCHANNEL.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param lpszChannelPath The channel to join. Channels are separated by a 
	/// '/'.
	/// @param lpszPassword The password for the channel to join or for a new 
	/// channel.
	/// @param lpszTopic A topic that can be set for a new channel. Don't include 
	/// a topic if the channel already exists.
	/// @param lpszOpPassword The password a user can specify to become channel 
	/// operator. The password must be sent as a channel message using the prefix 
	/// "/op".
	/// @see VOIP_DoChannelMessage
	/// @see WM_EVOVOIP_ADDCHANNEL
	/// @see WM_EVOVOIP_JOINEDCHANNEL
	EVOVOIPDLL_API BOOL VOIP_DoJoinChannel(IN LPVOID pClientInstance,
		IN LPCSTR lpszChannelPath, 
		IN LPCSTR lpszPassword, 
		IN LPCSTR lpszTopic,
		IN LPCSTR lpszOpPassword);

	/// Leave a channel.
	///
	/// Note that VOIP_DoLeaveChannel() doesn't take any parameters since a user 
	/// can only participate in one channel at the time.
	/// If command is successful the message WM_EVOVOIP_REMOVEUSER will be sent.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see DoJoinChannel
	/// @see WM_EVOVOIP_REMOVEUSER
	/// @see WM_EVOVOIP_LEFTCHANNEL
	EVOVOIPDLL_API BOOL VOIP_DoLeaveChannel(IN LPVOID pClientInstance);

	/// Change your nick name.
	///
	/// The message WM_EVOVOIP_UPDATEUSER will be posted if the update
	/// was successful.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param lpszNewNick is the new nick name to use.
	/// @see WM_EVOVOIP_UPDATEUSER
	EVOVOIPDLL_API BOOL VOIP_DoChangeNick(IN LPVOID pClientInstance, 
		IN LPCSTR lpszNewNick);

	/// Change you currect status
	///
	/// The message WM_EVOVOIP_UPDATEUSER will be posted if the update
	/// was successful.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nStatusMode should be 0 for available and 1 for away.
	/// @param lpszStatusMessage The users message associated with the status 
	/// mode.
	/// @see USERSTATUS_AVAILABLE
	/// @see USERSTATUS_AWAY
	/// @see WM_EVOVOIP_UPDATEUSER
	EVOVOIPDLL_API BOOL VOIP_DoChangeStatus(IN LPVOID pClientInstance,
		IN INT32 nStatusMode, 
		IN LPCSTR lpszStatusMessage);

	/// Send a private message to a user.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nToUserID The user who'll receive the message.
	/// @param lpszContent The message can be multi-line (include EOL) 
	/// but should not be longer than VOIP_STRLEN - 1 characters.
	EVOVOIPDLL_API BOOL VOIP_DoUserMessage(IN LPVOID pClientInstance,
		IN INT32 nToUserID, 
		IN LPCSTR lpszContent);

	/// Send a message to the channel you're currently parcipating in.
	/// Note that the server supports channel commands which are messages
	/// which start with a '/', e.g. /stats.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param lpszContent is the message. The message can be multi line but 
	/// should not be longer than VOIP_STRLEN - 1 characters.
	EVOVOIPDLL_API BOOL VOIP_DoChannelMessage(IN LPVOID pClientInstance,
		IN LPCSTR lpszContent);

	/// Make another user channel operator. Requires that you are channel 
	/// operator.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nUserID is the user who should become channel operator.
	EVOVOIPDLL_API BOOL VOIP_DoChannelOp(IN LPVOID pClientInstance,
		IN INT32 nUserID);

	/// Kick a user from the channel you are currently participating in.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	EVOVOIPDLL_API BOOL VOIP_DoKickUser(IN LPVOID pClientInstance,
		IN INT32 nUserID);

	/// Send a file to the specified channel. It must be the same channel
	/// as the user is currently partipating in. The file being uploaded
	/// must have a file size which is less than the disk quota of the channel, 
	/// minus the sum of all the files in the channel. The disk quota of a
	/// channel can be obtained in the nDiskQuota of the Channel-struct passed 
	/// to VOIP_GetChannel.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param szLocalFilePath The path of the file to upload, e.g. C:\test.txt.
	/// @see Channel
	/// @see VOIP_GetChannel
	EVOVOIPDLL_API BOOL VOIP_DoSendFile(IN LPVOID pClientInstance,
		IN LPCSTR szLocalFilePath);

	/// Download a file from the specified channel. It must be the same channel
	/// as the user is currently partipating in.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nFileID The ID of the file which is passed by WM_EVOVOIP_ADDFILE.
	/// @param szLocalFilePath The path of where to store the file, e.g. 
	/// C:\test.txt.
	/// @see WM_EVOVOIP_ADDFILE
	/// @see VOIP_GetChannelFilesCount
	/// @see VOIP_GetChannelFileID
	EVOVOIPDLL_API BOOL VOIP_DoRecvFile(IN LPVOID pClientInstance,
		IN INT32 nFileID, IN LPCSTR szLocalFilePath);

	/// Delete a file from a channel. The user must be operator of the channel
	/// where the file is located.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nFileID The ID of the file to delete. The ID of the file which 
	/// is passed by WM_EVOVOIP_ADDFILE.
	/// @see WM_EVOVOIP_ADDFILE
	/// @see VOIP_GetChannelFilesCount
	/// @see VOIP_GetChannelFileID
	EVOVOIPDLL_API BOOL VOIP_DoDeleteFile(IN LPVOID pClientInstance,
		IN INT32 nFileID);

	/// Get the server's message of the day (MOTD).
	/// Deprecated, use VOIP_GetServerProperties.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param szMotd will receive the MOTD.
	/// @see VOIP_GetServerProperties
	EVOVOIPDLL_API BOOL VOIP_GetMessageOfTheDay(IN LPVOID pClientInstance,
		OUT CHAR szMotd[VOIP_STRLEN]);

	/// Get the server's name. 
	/// Deprecated, use VOIP_GetServerProperties.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param szName will receive the server's name.
	/// @see VOIP_GetServerProperties
	EVOVOIPDLL_API BOOL VOIP_GetServerName(IN LPVOID pClientInstance,
		OUT CHAR szName[VOIP_STRLEN]);

	/// Get the server's client properties
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	EVOVOIPDLL_API BOOL VOIP_GetServerProperties(IN LPVOID pClientInstance,
		OUT ServerProperties* lpProperties);


	/// Get the root channel's ID
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @return Returns the ID of the root channel. If -1 is returned no root 
	/// channel exists.
	EVOVOIPDLL_API INT32 VOIP_GetRootChannelID(IN LPVOID pClientInstance);

	/// Get the channel current user is participating in.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @return Returns the ID of the current channel. If 0 is returned the 
	/// user is not participating in a channel.
	EVOVOIPDLL_API INT32 VOIP_GetMyChannelID(IN LPVOID pClientInstance);

	/// Get the channel with a specific ID.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nChannelID The ID of the channel to get information about.
	/// @param lpChannel A pointer to a Channel struct which will receive the 
	/// channel's data.
	/// @return FALSE if unable to retrieve channel otherwise TRUE
	EVOVOIPDLL_API BOOL VOIP_GetChannel(IN LPVOID pClientInstance,
		IN INT32 nChannelID, 
		OUT Channel* lpChannel );

	/// Get the ID of the channel's parent.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @return Returns -1 if there's no parent channel, otherwise the
	/// channel's parent ID.
	EVOVOIPDLL_API INT32 VOIP_GetChannelParentID(IN LPVOID pClientInstance,
		IN INT32 nChannelID);

	/// Get the channel's path. Channels are separated by '/'
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nChannelID The channel's ID
	/// @param szChannelPath Will receive the channel's path.
	/// @return Returns TRUE if channel exists.
	EVOVOIPDLL_API BOOL VOIP_GetChannelPath(IN LPVOID pClientInstance,
		IN INT32 nChannelID, 
		OUT CHAR szChannelPath[VOIP_STRLEN]);

	/// Get the number of users in a channel.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nChannelID is the ID of the channel.
	EVOVOIPDLL_API INT32 VOIP_GetChannelUserCount(IN LPVOID pClientInstance,
		IN INT32 nChannelID);

	/// Get a channel message.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nChannelID the ID of the channel to extract the message.
	/// @param nMsgID the ID of the message to extract.
	/// @param lpnFromUserID The ID of the user who sent the message.
	/// @param szContent The content of the message.
	EVOVOIPDLL_API BOOL VOIP_GetChannelMessage(IN LPVOID pClientInstance,
		IN INT32 nChannelID, 
		IN INT32 nMsgID, 
		OUT INT32* lpnFromUserID, 
		OUT CHAR szContent[VOIP_STRLEN]);

	/// Check whether user is operator of a channel
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nUserID the ID of the user to check.
	/// @param nChannelID the ID of the channel to check whether user
	/// is operator of.
	EVOVOIPDLL_API BOOL VOIP_IsChannelOperator(IN LPVOID pClientInstance,
		IN INT32 nUserID, IN INT32 nChannelID);

	/// Get a message which was broadcasted to all users from an administrator.
	/// This function will most likely be called as a result of the message
	/// WM_EVOVOIP_BROADCASTMESSAGE.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nMsgID The ID of the message broadcasted.
	/// @param szContent The content of the message broadcasted.
	/// @see WM_EVOVOIP_BROADCASTMESSAGE
	EVOVOIPDLL_API BOOL VOIP_GetBroadcastMessage(IN LPVOID pClientInstance,
		IN INT32 nMsgID, 
		OUT CHAR szContent[VOIP_STRLEN]);

	/// Get the user's user ID
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @return Returns the user ID assigned to the current user on the server.
	/// -1 is returned if no ID has been assigned to the user.
	EVOVOIPDLL_API INT32 VOIP_GetMyUserID(IN LPVOID pClientInstance);

	/// Get the user with the specified ID.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nUserID the ID of the user to extract.
	/// @param lpUser will receive the user's nick name.
	/// @see USERSTATUS_AVAILABLE
	/// @see USERSTATUS_AWAY
	EVOVOIPDLL_API BOOL VOIP_GetUser(IN LPVOID pClientInstance,
		IN INT32 nUserID, OUT User* lpUser);

	/// Get a private message from a user.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nUserID The ID of the user who sent the message.
	/// @param nMsgID The ID of the message to extract.
	/// @param szContent Will receive the content of the message to extract.
	EVOVOIPDLL_API BOOL VOIP_GetUserMessage(IN LPVOID pClientInstance,
		IN INT32 nUserID, 
		IN INT32 nMsgID, 
		OUT CHAR szContent[VOIP_STRLEN]);

	/// Set the volume of a user. Note that it's a virtual volume
	/// which is being set since the master volume affects the user
	/// volume.
	/// 
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nUserID The user ID of the user whose volume will be changed.
	/// @param nVolume Must be between SOUND_VOLUME_MIN and SOUND_VOLUME_MAX
	/// @see SOUND_VOLUME_MIN
	/// @see SOUND_VOLUME_MAX
	EVOVOIPDLL_API BOOL VOIP_SetUserVolume(IN LPVOID pClientInstance,
		IN INT32 nUserID, IN INT32 nVolume);

	/// Get the volume of a user. Note that it's a virtual volume
	/// which is being set since the master volume affects the user
	/// volume.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see SOUND_VOLUME_MIN
	/// @see SOUND_VOLUME_MAX
	EVOVOIPDLL_API INT32 VOIP_GetUserVolume(IN LPVOID pClientInstance,
		IN INT32 nUserID);

	/// Use software to gain a user's volume.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nUserID The ID of the user who should have sound gained.
	/// @param nGainLevel The gain level for the user. A value from
	/// GAIN_LEVEL_MIN to GAIN_LEVEL_MAX
	/// @see VOIP_GetUserGainLevel
	/// @see GAIN_LEVEL_MIN
	/// @see GAIN_LEVEL_MAX
	/// @see GAIN_LEVEL_DEFAULT
	EVOVOIPDLL_API BOOL VOIP_SetUserGainLevel(IN LPVOID pClientInstance,
		IN INT32 nUserID, IN INT32 nGainLevel);

	/// Get the software gain level for a user.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nUserID The ID of the user whose gain level should be retrieved.
	/// @see VOIP_SetUserGainLevel
	/// @see GAIN_LEVEL_MIN
	/// @see GAIN_LEVEL_MAX
	/// @see GAIN_LEVEL_DEFAULT
	/// @return The gain level for the user. A value from GAIN_LEVEL_MIN to 
	/// GAIN_LEVEL_MAX
	EVOVOIPDLL_API INT32 VOIP_GetUserGainLevel(IN LPVOID pClientInstance,
		IN INT32 nUserID);

	/// Mute a user.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nUserID The user ID of the user to mute (or unmute).
	/// @param bMute TRUE will mute, FALSE will unmute.
	EVOVOIPDLL_API BOOL VOIP_SetUserMute(IN LPVOID pClientInstance,
		IN INT32 nUserID, IN BOOL bMute);

	/// Check if a user is muted.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see VOIP_SetUserMute
	EVOVOIPDLL_API BOOL VOIP_IsUserMute(IN LPVOID pClientInstance,
		IN INT32 nUserID);

	/// Check if a user is talking.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see WM_EVOVOIP_USERTALKING
	/// @see WM_EVOVOIP_USERSTOPPEDTALKING
	EVOVOIPDLL_API BOOL VOIP_IsUserTalking(IN LPVOID pClientInstance,
		IN INT32 nUserID);

	/// Set the position of a user.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	EVOVOIPDLL_API BOOL VOIP_SetUserPosition(IN LPVOID pClientInstance,
		IN INT32 nUserID, 
		IN float x,
		IN float y, 
		IN float z);

	/// Get a user's position.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	EVOVOIPDLL_API BOOL VOIP_GetUserPosition(IN LPVOID pClientInstance,
		IN INT32 nUserID, 
		OUT float* x, 
		OUT float* y, 
		OUT float* z);

	/// When set to true the server will broadcast audio packets 
	/// instead of the client. Forwarding through the server is enabled by 
	/// default.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	EVOVOIPDLL_API BOOL VOIP_ForwardToAll(IN LPVOID pClientInstance,
		IN BOOL bForward);

	/// Whether server is broadcasting packets or the client is doing it.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @return Returns true if the server is forwarding the client's voice 
	/// packets.
	EVOVOIPDLL_API BOOL VOIP_IsForwardingToAll(IN LPVOID pClientInstance);

	/// Get the number of bytes sent. Note that the number of bytes
	/// transferred does not include TCP/IP overhead.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	EVOVOIPDLL_API INT32 VOIP_GetBytesReceived(IN LPVOID pClientInstance);

	/// Get the number of bytes received. Note that the number of bytes
	/// transferred does not include TCP/IP overhead.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	EVOVOIPDLL_API INT32 VOIP_GetBytesSent(IN LPVOID pClientInstance);

	/// Start transmitting voice data.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	EVOVOIPDLL_API BOOL VOIP_StartTransmitting(IN LPVOID pClientInstance);

	/// Stop transmitting voice data.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	EVOVOIPDLL_API BOOL VOIP_StopTransmitting(IN LPVOID pClientInstance);

	/// Register a global hotkey which send WM_EVOVOIP_HOTKEY
	/// to the HWND passed to VOIP_InitVoIP. WPARAM will contain the
	/// hotkey id and LPARAM will be 1 for hotkey active and 0 for hotkey 
	/// inactive. A hotkey can be used as a push-to-talk key combination.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nHotKeyID is the ID of the hotkey to register. It will be
	/// passed as the WPARAM when the hotkey becomes either active or inactive.
	/// @param bCtrl Set to true if Ctrl should be down to activate the hotkey.
	/// @param bAlt Set to true if Alt should be down to activate the hotkey.
	/// @param bShift Set to true if Shift should be down to activate the hotkey.
	/// @param bWin Set to true if Win should be down to activate the hotkey.
	/// @param nVirtualKey is the identifier of a virtual key like e.g. VK_DOWN 
	/// or VK_LBUTTON. Set it to 0 if you only use modifiers.
	/// @see VOIP_InitVoIP
	EVOVOIPDLL_API void VOIP_RegisterHotKey(IN LPVOID pClientInstance,
		IN INT32 nHotKeyID, 
		IN BOOL bCtrl, 
		IN BOOL bAlt, 
		IN BOOL bShift, 
		IN BOOL bWin, 
		IN UINT nVirtualKey);

	/// Unregister a registered hotkey.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see VOIP_RegisterHotKey
	EVOVOIPDLL_API void VOIP_UnregisterHotKey(IN LPVOID pClientInstance,
		IN INT32 nHotKeyID);

	/// Check whether hotkey is active.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nHotKeyID is the ID of the registered hotkey.
	EVOVOIPDLL_API BOOL VOIP_IsHotKeyActive(IN LPVOID pClientInstance,
		IN INT32 nHotKeyID);

	/// Install a test hook so the HWND will be messaged when
	/// a key or mouse button is pressed. Capture the message
	/// WM_EVOVOIP_KEYTEST.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param hWnd is the handle of the window which will be
	/// notified.
	/// @see VOIP_RemoveTestHook
	/// @see WM_EVOVOIP_KEYTEST
	EVOVOIPDLL_API BOOL VOIP_InstallTestHook(IN LPVOID pClientInstance,
		IN HWND hWnd);

	/// Remove the test hook again so the hWnd in VOIP_InstallTestHook
	/// will no longer be notified.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see VOIP_RemoveTestHook
	EVOVOIPDLL_API void VOIP_RemoveTestHook(IN LPVOID pClientInstance);

	/// A function pointer for callbacks when a user is talking. The user's
	/// raw audio can be extracted and stored on disk.
	///
	/// @param nUserID The ID of the user who is talking. Note that 
	/// nUserID will be 0 for yourself while unconnected.
	/// @param nSampleRate The frequency/sample rate of the user's audio.
	/// @param pRawAudio Array pointer to the user's raw audio. If pRawAudio is
	/// NULL it means that the user stopped talking.
	/// @param nSamples The number of samples in the pRawAudio array. If nSamples
	/// is 0 it means that the user stopped talking.
	/// @param pUserData User data.
	typedef void TalkingCallback(INT32 nUserID, INT32 nSampleRate, const short* pRawAudio, INT32 nSamples, PVOID pUserData);

	/// Register for callbacks when a user is talking so you can receive the
	/// user's raw audio.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param talkCallback A function pointer to the function which 
	/// will receive callbacks.
	/// @param pUserData User data.
	/// @see VOIP_UnregisterTalkingCallback
	/// @see TalkingCallback
	EVOVOIPDLL_API BOOL VOIP_RegisterTalkingCallback(IN LPVOID pClientInstance,
		IN TalkingCallback* talkCallback, PVOID pUserData);

	/// Unregister callbacks for when a user is talking.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see VOIP_RegisterTalkingCallback
	EVOVOIPDLL_API BOOL VOIP_UnregisterTalkingCallback(IN LPVOID pClientInstance);

	/// Get the number of files in the specified channel. Use VOIP_GetChannelFileID
	/// to do an index-based sweep of the files in the channel.
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nChannelID The ID of the channel where the file is located.
	/// @param lpnFilesCount A pointer which will be passed the number of files.
	/// @see VOIP_GetChannelFileID
	EVOVOIPDLL_API BOOL VOIP_GetChannelFilesCount(IN LPVOID pClientInstance,
		IN INT32 nChannelID, 
		OUT INT32* lpnFilesCount);

	/// Get information about a file in a channel.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nChannelID The ID of the channel where the file is located.
	/// @param nFileIndex The index of the file (nFileIndex >= 0 && nFileIndex < VOIP_GetChannelFilesCount).
	/// @param lpnFileID A pointer which will received the ID of the file. 
	/// Use VOIP_GetChannelFileInfo to get more info.
	/// @see VOIP_GetChannelFilesCount
	EVOVOIPDLL_API BOOL VOIP_GetChannelFileID(IN LPVOID pClientInstance,
		IN INT32 nChannelID, 
		IN INT32 nFileIndex, OUT INT32* lpnFileID);

	/// Get information about a file which can be downloaded.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nFileID The ID of the file.
	/// @param lpFileInfo A pointer to a FileInfo-struct which will receive 
	/// file information.
	EVOVOIPDLL_API BOOL VOIP_GetChannelFileInfo(IN LPVOID pClientInstance,
		IN INT32 nFileID, 
		OUT FileInfo* lpFileInfo); //checkout WM_EVOVOIP_ADDFILE

	/// Get information about an active file transfer. An active file transfer is
	/// one which has been post through the message WM_EVOVOIP_TRANSFER_BEGIN.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nTransferID The ID of the file transfer to investigate. Transfer 
	/// ID is passed by WM_EVOVOIP_TRANSFER_BEGIN.
	/// @param lpTransfer A pointer to a struct which will receive the file 
	/// transfer information.
	/// @see WM_EVOVOIP_TRANSFER_BEGIN
	EVOVOIPDLL_API BOOL VOIP_GetFileTransferInfo(IN LPVOID pClientInstance,
		IN INT32 nTransferID, 
		OUT FileTransfer* lpTransfer);

	/// Cancel an active file transfer. An active file transfer is
	/// one which has been post through the message WM_EVOVOIP_TRANSFER_BEGIN.
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @param nTransferID The ID of the file transfer to investigate. Transfer 
	/// ID is passed by WM_EVOVOIP_TRANSFER_BEGIN.
	EVOVOIPDLL_API BOOL VOIP_CancelFileTranfer(IN LPVOID pClientInstance,
		IN INT32 nTransferID);

	/// Get a description of an error code posted by either
	/// WM_EVOVOIP_ERROR or WM_EVOVOIP_TRANSFER_FAILED
	///
	/// @param pClientInstance Pointer to client instance created by VOIP_InitVoIP.
	/// @see WM_EVOVOIP_ERROR
	/// @see WM_EVOVOIP_TRANSFER_FAILED
	EVOVOIPDLL_API void VOIP_GetErrorMessage(IN INT32 nError, 
		OUT CHAR szErrorMsg[VOIP_STRLEN]);

	/// Initialize the VoIP administrator module. The administrator module
	/// works in similar was as the client by sending message to the HWND which
	/// is passed to the function.
	/// @param pNewAdminInstance Pointer to a new instance admin instance.
	/// Call VOIP_Admin_Close to destroy the instance and release its resources.
	/// @see VOIP_Admin_Close
	EVOVOIPDLL_API BOOL VOIP_Admin_Init(IN HWND hWnd, OUT LPVOID* pNewAdminInstance);

	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	EVOVOIPDLL_API BOOL VOIP_Admin_Close(IN LPVOID pAdminInstance);


	/// Connect to a VoIP server.
	/// 
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param szHostAddress The IP-address or hostname of the VoIP server.
	/// @param nTcpPort The TCP host port of the VoIP server.
	/// @see WM_EVOVOIP_CONNECTSUCCESS
	/// @see WM_EVOVOIP_CONNECTFAILED
	EVOVOIPDLL_API BOOL VOIP_Admin_Connect(IN LPVOID pAdminInstance, 
		IN LPCSTR szHostAddress, IN INT32 nTcpPort);

	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	EVOVOIPDLL_API BOOL VOIP_Admin_Disconnect(IN LPVOID pAdminInstance);

	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	EVOVOIPDLL_API BOOL VOIP_Admin_IsConnected(IN LPVOID pAdminInstance);

	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	EVOVOIPDLL_API BOOL VOIP_Admin_IsConnecting(IN LPVOID pAdminInstance);

	/// Check if admin has performed login and has been authorized.
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	EVOVOIPDLL_API BOOL VOIP_Admin_IsAuthorized(IN LPVOID pAdminInstance);

	/// Get the User ID assigned to the administrator login.
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @return Returns -1 on error.
	EVOVOIPDLL_API INT32 VOIP_Admin_GetMyUserID(IN LPVOID pAdminInstance);

	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	EVOVOIPDLL_API BOOL VOIP_Admin_GetServerProperties(IN LPVOID pAdminInstance,
		OUT ServerProperties* lpServerInfo);

	/// Log on a connected VoIP server. Make sure server
	/// has remote administration enabled in the server's
	/// config-file.
	///
	/// This function should be called once the WM_EVOVOIP_CONNECTSUCCESS has
	/// been sent to the HWND which was passed to the VOIP_Admin_Init.
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param szUsername The administrator username.
	/// @param szPasswd The administrator password.
	///@see VOIP_Admin_Init
	///@see WM_EVOVOIP_CONNECTSUCCESS
	EVOVOIPDLL_API BOOL VOIP_Admin_DoLogin(IN LPVOID pAdminInstance,
		IN LPCSTR szUsername, 
		IN LPCSTR szPasswd);

	/// Make a new channel. The new channel is static by default.
	///
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param lpChanInfo A Channel-structure containing information about
	/// the channel being created. The Channel's member "nChannelID" is ignored.
	EVOVOIPDLL_API BOOL VOIP_Admin_DoMakeChannel(IN LPVOID pAdminInstance,
		IN const Channel* lpChanInfo);

	/// Update a channel's properties.
	///
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param lpChanInfo A Channel-structure containing information about
	/// the channel being created. The Channel's member "nParentChannelID"
	/// and "szName" are ignored.
	EVOVOIPDLL_API BOOL VOIP_Admin_DoUpdateChannel(IN LPVOID pAdminInstance,
		IN const Channel* lpChanInfo);

	/// Remove a channel from a server. Note that if there's any users in the
	/// channel they will be kicked and subchannels will be deleted as well.
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	EVOVOIPDLL_API BOOL VOIP_Admin_DoRemoveChannel(IN LPVOID pAdminInstance,
		IN INT32 nChannelID);

	/// Issue command to move a user from one channel to another.
	///
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param nUserID User to be moved.
	/// @param szDestChannel Channel path of where user should be put into.
	/// @see VOIP_Admin_DoMoveUserByID
	EVOVOIPDLL_API BOOL VOIP_Admin_DoMoveUser(IN LPVOID pAdminInstance,
		IN INT32 nUserID, 
		IN LPCSTR szDestChannel);

	/// Issue command to move a user from one channel to another.
	///
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param nUserID User to be moved.
	/// @param nDestChannelID ID of channel where user should be move into.
	EVOVOIPDLL_API BOOL VOIP_Admin_DoMoveUserByID(IN LPVOID pAdminInstance,
		IN INT32 nUserID, 
		IN INT32 nDestChannelID);

	/// Update server properties.
	///
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param lpServerInfo A structure holding the information to be set 
	/// on the server.
	EVOVOIPDLL_API BOOL VOIP_Admin_DoUpdateServer(IN LPVOID pAdminInstance,
		IN const ServerProperties* lpServerInfo);

	/// Kick user off a server. To ban a user call VOIP_Admin_DoBanUser.
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	///@see VOIP_Admin_DoBanUser
	EVOVOIPDLL_API BOOL VOIP_Admin_DoKickUser(IN LPVOID pAdminInstance,
		IN INT32 nUserID);

	/// Make user operator of a channel.
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	EVOVOIPDLL_API BOOL VOIP_Admin_DoMakeOperator(IN LPVOID pAdminInstance,
		IN INT32 nUserID, IN INT32 nChannelID);

	/// Issue a ban command on a user in a specific channel. The ban applies 
	/// to the user's IP-address. Call VOIP_Admin_DoKickUser to kick the user off the 
	/// server.
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	///@see VOIP_Admin_DoKickUser
	EVOVOIPDLL_API BOOL VOIP_Admin_DoBanUser(IN LPVOID pAdminInstance,
		IN INT32 nUserID);

	/// Unban the user with the specified IP-address.
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	EVOVOIPDLL_API BOOL VOIP_Admin_DoUnBanUser(IN LPVOID pAdminInstance,
		IN LPCSTR szIpAddress);

	/// Issue a command to the list of users.
	///
	/// This command issues a list command which will notify the HWND passed to
	/// VOIP_Admin_Init when it's completed by sending the message 
	/// WM_EVOVOIP_LISTCOMMAND_COMPLETED. Once completed call
	/// the function VOIP_Admin_GetBannedUsers to get the list of users.
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	///@see VOIP_Admin_Init
	///@see WM_EVOVOIP_LISTCOMMAND_COMPLETED
	///@see VOIP_Admin_GetBannedUsers
	EVOVOIPDLL_API BOOL VOIP_Admin_DoListBans(IN LPVOID pAdminInstance);

	/// Send administrator message to a user. The WM_EVOVOIP_ADMINMESSAGE
	/// will be posted in the client. The user should reply to the admin with
	/// VOIP_DoSendUserMessage(.) and the WM_EVOVOIP_USERMESSAGE will be posted
	/// in the admin.
	///
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param nUserID The user who should receive the message
	/// @param szMessage The content of the message
	/// @see VOIP_DoSendUserMessage
	/// @see VOIP_GetUserMessage
	/// @see VOIP_DoSendChannelMessage
	EVOVOIPDLL_API BOOL VOIP_Admin_DoAdminMessage(IN LPVOID pAdminInstance,
		IN INT32 nUserID, LPCSTR szMessage);

	/// Send a text-message to a channel.
	///
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param nChannelID The ID of the channel
	/// @param szMessage The content of the message
	EVOVOIPDLL_API BOOL VOIP_Admin_DoChannelMessage(IN LPVOID pAdminInstance,
		IN INT32 nChannelID, LPCSTR szMessage);

	/// Broadcast a message to all authorized users on the server.
	///
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param szMessage The content of the message
	EVOVOIPDLL_API BOOL VOIP_Admin_DoBroadcastMessage(IN LPVOID pAdminInstance,
		LPCSTR szMessage);


	/// Send a file to the specified channel. Note that as admin the size of the
	/// disk quota does not apply. VOIP_Admin_UpdateChannel can be used to modify 
	/// the size of the disk quota of a channel.
	///
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param nChannelID ID of the channel to send to.
	/// @param szLocalFilePath The path of the file to upload, e.g. C:\test.txt.
	/// @see VOIP_Admin_UpdateChannel
	EVOVOIPDLL_API BOOL VOIP_Admin_DoSendFile(IN LPVOID pAdminInstance,
		IN INT32 nChannelID, IN LPCSTR szLocalFilePath);

	/// Download a file from the specified channel.
	///
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param nChannelID ID of the channel to download from.
	/// @param nFileID The ID of the file which is passed by WM_EVOVOIP_ADDFILE.
	/// @param szLocalFilePath The path of where to store the file, e.g. C:\test.txt.
	/// @see WM_EVOVOIP_ADDFILE
	/// @see VOIP_Admin_GetChannelFilesCount
	/// @see VOIP_Admin_GetChannelFileID
	EVOVOIPDLL_API BOOL VOIP_Admin_DoRecvFile(IN LPVOID pAdminInstance,
		IN INT32 nChannelID, IN INT32 nFileID, IN LPCSTR szLocalFilePath);

	/// Delete a file from a channel.

	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param nChannelID The ID of the channel where the file is located.
	/// @param nFileID The ID of the file to delete. The ID of the file which
	/// is passed by WM_EVOVOIP_ADDFILE.
	/// @see WM_EVOVOIP_ADDFILE
	/// @see VOIP_Admin_GetChannelFilesCount
	/// @see VOIP_Admin_GetChannelFileID
	EVOVOIPDLL_API BOOL VOIP_Admin_DoDeleteFile(IN LPVOID pAdminInstance,
		IN INT32 nChannelID, IN INT32 nFileID);

	/// Issue command to set a new username and password for the remote 
	/// administrator.
	/// An empty username or password will disable remote logins.
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	EVOVOIPDLL_API BOOL VOIP_Admin_DoUpdateAdmin(IN LPVOID pAdminInstance,
		IN LPCSTR szUsername, IN LPCSTR szPassword);

	/// Save the server's current state to its settings file (typically 
	/// the server's .xml file).
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	EVOVOIPDLL_API BOOL VOIP_Admin_DoSaveConfig(IN LPVOID pAdminInstance);

	/// Quit from server.
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	EVOVOIPDLL_API BOOL VOIP_Admin_DoQuit(IN LPVOID pAdminInstance);


	/// Get information about a user.
	/// 
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param nUserID The ID of the user to get information about.
	/// @param lpUser A User struct to be filled with user information.
	/// @see USERSTATUS_AVAILABLE
	/// @see USERSTATUS_AWAY
	EVOVOIPDLL_API BOOL VOIP_Admin_GetUser(IN LPVOID pAdminInstance,
		IN INT32 nUserID, OUT User* lpUser );

	/// Check wheher a user is operator of a channel.
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	EVOVOIPDLL_API BOOL VOIP_Admin_IsChannelOperator(IN LPVOID pAdminInstance,
		IN INT32 nUserID, IN INT32 nChannelID);

	/// Get the channel with a specific ID.
	///
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param nChannelID The ID of the channel to get information about.
	/// @param lpChannel A pointer to a Channel struct which will receive 
	/// the channel's data.
	/// @return FALSE if unable to retrieve channel otherwise TRUE
	EVOVOIPDLL_API BOOL VOIP_Admin_GetChannel(IN LPVOID pAdminInstance,
		IN INT32 nChannelID, 
		OUT Channel* lpChannel );

	/// Get the channel's path. Channels are separated by '/'
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param nChannelID The channel's ID
	/// @param szChannelPath Will receive the channel's path.
	/// @return Returns TRUE if channel exists.
	EVOVOIPDLL_API BOOL VOIP_Admin_GetChannelPath(IN LPVOID pAdminInstance,
		IN INT32 nChannelID, 
		OUT CHAR szChannelPath[VOIP_STRLEN]);

	/// Get the ID of the specified channel
	///
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param szChannelPath The path of the channel which ID you want
	/// to obtain.
	/// @return The channel ID or -1 if the channel doesn't exist.
	EVOVOIPDLL_API INT32 VOIP_Admin_GetChannelID(IN LPVOID pAdminInstance,
		IN LPCSTR szChannelPath);

	/// Get a private message from a user.
	///
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param nUserID The ID of the user who sent the message.
	/// @param nMsgID The ID of the message to extract.
	/// @param szContent Will receive the content of the message to extract.
	EVOVOIPDLL_API BOOL VOIP_Admin_GetUserMessage(IN LPVOID pAdminInstance,
		IN INT32 nUserID, 
		IN INT32 nMsgID, 
		OUT CHAR szContent[VOIP_STRLEN]);

	/// Get a channel message.
	///
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param nChannelID the ID of the channel to extract the message.
	/// @param nMsgID the ID of the message to extract.
	/// @param lpnFromUserID The ID of the user who sent the message.
	/// @param szContent The content of the message.
	EVOVOIPDLL_API BOOL VOIP_Admin_GetChannelMessage(IN LPVOID pAdminInstance,
		IN INT32 nChannelID, 
		IN INT32 nMsgID, 
		OUT INT32* lpnFromUserID, 
		OUT CHAR szContent[VOIP_STRLEN]);

	/// Get a message which was broadcasted to all users from an administrator.
	/// This function will most likely be called as a result of the message
	/// WM_EVOVOIP_BROADCASTMESSAGE.
	///
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param nMsgID The ID of the message broadcasted.
	/// @param szContent The content of the message broadcasted.
	/// @see WM_EVOVOIP_BROADCASTMESSAGE
	EVOVOIPDLL_API BOOL VOIP_Admin_GetBroadcastMessage(IN LPVOID pAdminInstance,
		IN INT32 nMsgID,
		OUT CHAR szContent[VOIP_STRLEN]);

	/// Get a list of banned users.
	///
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param lpBannedUsers A preallocated array to hold banned users.
	/// @param nCount The max number of banned users the lpBannedUsers array 
	/// can hold.
	/// @param lpnHowMany The number of banned users in the list to extract. This
	/// parameter can be NULL.
	EVOVOIPDLL_API BOOL VOIP_Admin_GetBannedUsers(IN LPVOID pAdminInstance,
		OUT BannedUser* lpBannedUsers, 
		IN INT32 nCount, OUT INT32* lpnHowMany);

	/// Get the number of files in the specified channel. Use 
	/// VOIP_Admin_GetChannelFileID to do an index-based sweep of the files in
	/// the channel.
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param nChannelID The ID of the channel where the file is located.
	/// @param lpnFilesCount A pointer which will be passed the number of files.
	/// @see VOIP_Admin_GetChannelFileID
	EVOVOIPDLL_API BOOL VOIP_Admin_GetChannelFilesCount(IN LPVOID pAdminInstance,
		IN INT32 nChannelID, 
		OUT INT32* lpnFilesCount);

	/// Get information about a file in a channel.
	///
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param nChannelID The ID of the channel where the file is located.
	/// @param nFileIndex The index of the file (nFileIndex >= 0 && nFileIndex < VOIP_Admin_GetChannelFilesCount).
	/// @param lpnFileID A pointer which will received the ID of the file. Use 
	/// VOIP_Admin_GetChannelFileInfo to get more info.
	/// @see VOIP_GetChannelFilesCount
	EVOVOIPDLL_API BOOL VOIP_Admin_GetChannelFileID(IN LPVOID pAdminInstance,
		IN INT32 nChannelID, 
		IN INT32 nFileIndex, OUT INT32* lpnFileID);

	/// Get information about a file which can be downloaded.
	///
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param nFileID The ID of the file.
	/// @param lpFileInfo A pointer to a FileInfo-struct which will receive
	/// file information.
	EVOVOIPDLL_API BOOL VOIP_Admin_GetChannelFileInfo(IN LPVOID pAdminInstance,
		IN INT32 nFileID, 
		OUT FileInfo* lpFileInfo);

	/// Get information about an active file transfer. An active file transfer is
	/// one which has been post through the message WM_EVOVOIP_TRANSFER_BEGIN.
	///
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param nTransferID The ID of the file transfer to investigate. Transfer
	/// ID is passed by WM_EVOVOIP_TRANSFER_BEGIN.
	/// @param lpTransfer A pointer to a struct which will receive the file
	/// transfer information.
	/// @see WM_EVOVOIP_TRANSFER_BEGIN
	EVOVOIPDLL_API BOOL VOIP_Admin_GetFileTransferInfo(IN LPVOID pAdminInstance,
		IN INT32 nTransferID,
		OUT FileTransfer* lpTransfer);

	/// Cancel an active file transfer. An active file transfer is
	/// one which has been post through the message WM_EVOVOIP_TRANSFER_BEGIN.
	///
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param nTransferID The ID of the file transfer to investigate. Transfer
	/// ID is passed by WM_EVOVOIP_TRANSFER_BEGIN.
	EVOVOIPDLL_API BOOL VOIP_Admin_CancelFileTranfer(IN LPVOID pAdminInstance,
		IN INT32 nTransferID);

	/// Set the default disk quota for new channels.
	///
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param nChannelDiskQuota The number of bytes which each channel should
	/// have by default for file storage.
	EVOVOIPDLL_API BOOL VOIP_Admin_DoFileShare(IN LPVOID pAdminInstance,
		IN INT64 nChannelDiskQuota);

	/// Get information the current file share settings.
	/// @param pAdminInstance Pointer to admin instance created by VOIP_Admin_Init.
	/// @param szFilesRoot The location where the server stores its files.
	/// @param lpnChannelDiskQuota The default disk quota of a channel.
	/// @param lpnMaxDiskUsage The server's maximum for file storage.
	EVOVOIPDLL_API BOOL VOIP_Admin_GetFileShare(IN LPVOID pAdminInstance,
		OUT CHAR szFilesRoot[VOIP_STRLEN],
		OUT INT64* lpnChannelDiskQuota, OUT INT64* lpnMaxDiskUsage);

	/// Get the number of Windows Mixers available.
	/// The index from 0 to VOIP_Mixer_GetMixerCount()-1 should be passed to the
	/// VOIP_Mixer_* functions.
	/// @see VOIP_Mixer_GetMixerName
	EVOVOIPDLL_API INT32 VOIP_Mixer_GetMixerCount();

	/// Get the name of a Windows Mixer based on its name.
	/// @param nMixerIndex The index of the mixer. Ranging from 0 to 
	/// VOIP_Mixer_GetMixerCount()-1.
	/// @param szMixerName The output string receiving the name of the device.
	EVOVOIPDLL_API BOOL VOIP_Mixer_GetMixerName(IN INT32 nMixerIndex,
		OUT CHAR szMixerName[VOIP_STRLEN]);

	/// The Windows mixer devices which can be queried by the VOIP_Mixer_* functions.
	/// Wave-In devices which are not in the enum-structure can be accessed by
	/// DEVICE_WAVEIN_BYINDEX which allows the user to query based on an index.
	///
	/// @see VOIP_Mixer_SetWaveOutMute
	/// @see VOIP_Mixer_SetWaveOutVolume
	/// @see VOIP_Mixer_SetWaveInSelected
	/// @see VOIP_Mixer_SetWaveInVolume
	/// @see VOIP_Mixer_GetWaveInDeviceName
	/// @see VOIP_Mixer_SetWaveInDeviceSelected

	enum
	{
		DEVICE_WAVEOUT_MASTER,
		DEVICE_WAVEOUT_WAVE,
		DEVICE_WAVEOUT_MICROPHONE,

		DEVICE_WAVEIN_MICROPHONE,
		DEVICE_WAVEIN_LINEIN,
		DEVICE_WAVEIN_WAVEOUT,
	};

	/// Mute or unmute a Windows Mixer Wave-Out device from the 'enum' of devices.
	/// @param nMixerIndex The index of the mixer. Ranging from 0 to VOIP_Mixer_GetMixerCount()-1.
	/// @param nDevice One of the DEVICE_WAVEOUT_* values from the 'enum' of devices.
	/// @param bMute True if device should be muted, False if it should be unmuted.
	/// @see VOIP_Mixer_GetWaveOutMute
	EVOVOIPDLL_API BOOL VOIP_Mixer_SetWaveOutMute(IN INT32 nMixerIndex, IN INT32 nDevice, BOOL bMute);

	/// Get the mute state of a Windows Mixer Wave-Out device from the 'enum' of devices.
	/// @param nMixerIndex The index of the mixer. Ranging from 0 to VOIP_Mixer_GetMixerCount()-1.
	/// @param nDevice One of the DEVICE_WAVEOUT_* values from the 'enum' of devices.
	/// @see VOIP_Mixer_SetWaveOutMute
	EVOVOIPDLL_API BOOL VOIP_Mixer_GetWaveOutMute(IN INT32 nMixerIndex, IN INT32 nDevice);

	/// Set the volume of a Windows Mixer Wave-Out device from the 'enum' of devices.
	/// @param nMixerIndex The index of the mixer. Ranging from 0 to VOIP_Mixer_GetMixerCount()-1.
	/// @param nDevice One of the DEVICE_WAVEOUT_* values from the 'enum' of devices.
	/// @param nVolume A value ranging from 0 to 65535.
	EVOVOIPDLL_API BOOL VOIP_Mixer_SetWaveOutVolume(IN INT32 nMixerIndex, IN INT32 nDevice, INT32 nVolume);

	/// Get the volume of a Windows Mixer Wave-Out device from the 'enum' of devices.
	/// @param nMixerIndex The index of the mixer. Ranging from 0 to VOIP_Mixer_GetMixerCount()-1.
	/// @param nDevice One of the DEVICE_WAVEOUT_* values from the 'enum' of devices.
	/// @return A value ranging from 0 to 65535.
	EVOVOIPDLL_API INT32 VOIP_Mixer_GetWaveOutVolume(IN INT32 nMixerIndex, IN INT32 nDevice);

	/// Set the selected state of a Windows Mixer Wave-In device from the 'enum' of devices.
	/// @param nDevice One of the DEVICE_WAVEOUT_* values from the 'enum' of devices.
	/// @param nMixerIndex The index of the mixer. Ranging from 0 to VOIP_Mixer_GetMixerCount()-1.
	EVOVOIPDLL_API BOOL VOIP_Mixer_SetWaveInSelected(IN INT32 nMixerIndex, IN INT32 nDevice);

	/// Get the selected state of a Windows Mixer Wave-In device from the 'enum' of devices.
	/// @param nDevice One of the DEVICE_WAVEOUT_* values from the 'enum' of devices.
	/// @param nMixerIndex The index of the mixer. Ranging from 0 to VOIP_Mixer_GetMixerCount()-1.
	EVOVOIPDLL_API BOOL VOIP_Mixer_GetWaveInSelected(IN INT32 nMixerIndex, IN INT32 nDevice);

	/// Set the volume of a Windows Mixer Wave-In device from the 'enum' of devices.
	/// @param nMixerIndex The index of the mixer. Ranging from 0 to VOIP_Mixer_GetMixerCount()-1.
	/// @param nDevice One of the DEVICE_WAVEOUT_* values from the 'enum' of devices.
	/// @param nVolume A value ranging from 0 to 65535.
	EVOVOIPDLL_API BOOL VOIP_Mixer_SetWaveInVolume(IN INT32 nMixerIndex, IN INT32 nDevice, INT32 nVolume);

	/// Get the volume of a Windows Mixer Wave-In device from the 'enum' of devices.
	/// @param nMixerIndex The index of the mixer. Ranging from 0 to VOIP_Mixer_GetMixerCount()-1.
	/// @param nDevice One of the DEVICE_WAVEOUT_* values from the 'enum' of devices.
	/// @return A value ranging from 0 to 65535.
	EVOVOIPDLL_API INT32 VOIP_Mixer_GetWaveInVolume(IN INT32 nMixerIndex, IN INT32 nDevice);

	/// Enable and disable microphone boost
	///
	/// @param nMixerIndex The mixer to access.
	/// @param bEnable True to enable, False to disable.
	EVOVOIPDLL_API BOOL VOIP_Mixer_SetWaveInBoost(IN INT32 nMixerIndex, IN BOOL bEnable);

	/// See if microphone boost is enabled.
	///
	/// @param nMixerIndex The mixer to access.
	EVOVOIPDLL_API BOOL VOIP_Mixer_GetWaveInBoost(IN INT32 nMixerIndex);

	/// Get the number of Windows Mixer Wave-In devices.
	/// @param nMixerIndex The index of the mixer. Ranging from 0 to VOIP_Mixer_GetMixerCount()-1.
	EVOVOIPDLL_API INT32 VOIP_Mixer_GetWaveInDeviceCount(IN INT32 nMixerIndex);

	/// Get the name of the Wave-In device with the specified index.
	/// @param nMixerIndex The index of the mixer. Ranging from 0 to VOIP_Mixer_GetMixerCount()-1.
	/// @param nDeviceIndex The index of the device. Randing from 0 to  VOIP_Mixer_GetWaveInDeviceCount()-1.
	/// @param szDeviceName The output string of the name of the device.
	/// @see VOIP_Mixer_GetWaveInDeviceCount
	EVOVOIPDLL_API BOOL VOIP_Mixer_GetWaveInDeviceName(IN INT32 nMixerIndex, IN INT32 nDeviceIndex, OUT CHAR szDeviceName[VOIP_STRLEN]);

	/// Set the selected state of a Wave-In device in the Windows Mixer.
	/// @param nMixerIndex The index of the mixer. Ranging from 0 to VOIP_Mixer_GetMixerCount()-1.
	/// @param nDeviceIndex The index of the device. Randing from 0 to  VOIP_Mixer_GetWaveInDeviceCount()-1.
	/// @see VOIP_Mixer_GetWaveInDeviceCount
	EVOVOIPDLL_API BOOL VOIP_Mixer_SetWaveInDeviceSelected(IN INT32 nMixerIndex, IN INT32 nDeviceIndex);

	/// Get the selected state of a Wave-In device in the Windows Mixer.
	/// @param nMixerIndex The index of the mixer. Ranging from 0 to VOIP_Mixer_GetMixerCount()-1.
	/// @param nDeviceIndex The index of the device. Randing from 0 to  VOIP_Mixer_GetWaveInDeviceCount()-1.
	/// @see VOIP_Mixer_GetWaveInDeviceCount
	EVOVOIPDLL_API BOOL VOIP_Mixer_GetWaveInDeviceSelected(IN INT32 nMixerIndex, IN INT32 nDeviceIndex);

#ifdef __cplusplus
}
#endif

#endif //EVOVOIPDLL_H
