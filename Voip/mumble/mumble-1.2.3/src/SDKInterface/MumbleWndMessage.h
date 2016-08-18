typedef void TalkingCallback(INT32 nUserID, INT32 nSampleRate, 
							 const short* pRawAudio, 
							 INT32 nSamples, 
							 PVOID pUserData);
typedef void TalkingCallback2(INT32 nUserID, INT32 nSampleRate, 
							 PVOID pRawAudio, 
							 INT32 nSamples, 
							 PVOID pUserData);

typedef void EmptyCallback();
typedef void IntParamCallback(INT32 nUserID);
typedef void TwoIntParamCallback(INT32 nUserID, INT32 nChannelID);

#define WM_MUMBLE_INVALIDCHANNELNAME WM_USER + 490

#define WM_MUMBLE_CONNECTSUCCESS WM_USER + 451
#define WM_MUMBLE_CONNECTFAILED WM_USER + 452
#define WM_MUMBLE_CONNECTIONLOST WM_USER + 453
/// \def WM_EVOVOIP_ACCEPTED
/// The call to #EVO_DoLogin was successful.
/// @param wParam Your user ID
/// @param lParam Ignored
/// @see EVO_DoLogin
#define WM_MUMBLE_ACCEPTED WM_USER + 454
#define WM_MUMBLE_LOGGEDOUT WM_USER + 472
#define WM_MUMBLE_KICKED WM_USER + 463
#define WM_MUMBLE_SERVERUPDATE WM_USER + 464
#define WM_MUMBLE_ERROR WM_USER + 466

/// \def WM_MUMBLE_ADDUSER
/// A new user has joined the channel.
/// @param wParam User's ID
/// @param lParam Channel ID
#define WM_MUMBLE_ADDUSER WM_USER + 455
/// \def WM_MUMBLE_REMOVEUSER
/// User has exited the current channel.
/// @param wParam User's ID
/// @param lParam Channel ID
#define WM_MUMBLE_REMOVEUSER WM_USER + 457
/// \def WM_MUMBLE_JOINEDCHANNEL
/// You have joined a new channel. You'll afterwards receive the
/// WM_MUMBLE_ADDUSER message for each of the users in the channel
/// Note that WM_MUMBLE_REMOVEUSER will not be called in order 
///	to remove the users from the channel you are currently 
/// participating in.
/// @param wParam Channel's ID
/// @param lParam Ignored
/// @see WM_MUMBLE_ADDUSER
/// @see WM_MUMBLE_REMOVEUSER
#define WM_MUMBLE_JOINEDCHANNEL WM_USER + 465
/// \def WM_MUMBLE_LEFTCHANNEL
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
#define WM_MUMBLE_LEFTCHANNEL WM_USER + 471
/// \def WM_MUMBLE_USERTALKING
/// A user is talking.
/// @param wParam User's ID
/// @param lParam Ignored
#define WM_MUMBLE_USERTALKING WM_USER + 467
/// \def WM_MUMBLE_USERSTOPPEDTALKING
/// A user has stopped talking.
/// @param wParam User's ID
/// @param lParam Ignored
#define WM_MUMBLE_USERSTOPPEDTALKING WM_USER + 468
/// \def WM_MUMBLE_MYUSERIDRECIEVED
/// A user has recived id.
/// @param wParam User's ID
/// @param lParam Ignored
#define WM_MUMBLE_MYUSERIDRECIEVED WM_USER + 488
#define WM_MUMBLE_ERRORCODE WM_USER + 489
