#pragma once

#include "IDesktopMan.h"
#include "selectors/IDesktopSelector.h"
#include "SharingSessionBase.h"
#include "RectAwarenessWnd.h"
#include "sharingwm_handler.h"

class VASEEncoder;
class CSharingViewSession;
class VASWaveletEncoder;

#define	ST_StartSession			1
#define	ST_KeyFrameOut			2
#define	ST_FrameOut				3
#define	ST_MissedSessionName	4
#define	ST_StartViewSession		5
#define	ST_StopSession			6
#define ST_StartRecording		7
#define ST_StopRecording		8
#define	ST_StopSessionView		9
#define ST_StartFilePlay		10
#define ST_ResumeRecord			11
#define ST_PauseRecord			12
#define ST_RewindRecord			13
#define ST_StartSendFrames		14
#define ST_OnlyKeyFrames		15
#define ST_AllFrames			16
#define ST_RemoteControl		17
#define ST_AllowRemoteControl	18
#define ST_DenyRemoteControl	19
#define ST_DeleteRecord			20
#define ST_HideSession			21


#define	RT_SessionStarted		1	// sharing session started (from this computer)
#define	RT_ViewSessionStarted	2	// view sharing session started (from this computer)
#define	RT_SessionNotFound		3	// sharing session was not found due to StartSession request
#define	RT_SessionAlreadyExists	4	// sharing session with name sended with StartSession request already exists on server
#define	RT_FramesPackReceived	5	// received frames pack
#define	RT_SessionClosed		6	// session was closed already
#define RT_StartRecording		7   // Сервер в ответ присылает сообщие с именем сессии и файла, в который она записывается 
#define RT_WaitOwner			8   // need reconnect, because owner don't create SS
#define RT_RemoteCommand		9   // комманда для воспроизведения действий, совершённых на шаринге на удалённом компьютере
#define RT_AllowRemoteControl	10  // разрешить удаленное управление  через экран (в случае если пользователь не владелец шаринга на экране)
#define RT_DenyRemoteControl	11  // запретить удаленное управление  через экран (в случае если пользователь не владелец шаринга на экране)

//////////////////////////////////////////////////////////////////////////
class DESKTOPMAN_API CSharingSession : public CSharingSessionBase
						, public IDesktopSharingSession
						, public CEventSource<IDesktopSharingSessionCallback>
						, public ViskoeThread::CThreadImpl<CSharingSession>
{
	
public:
	CSharingSession( SHARING_CONTEXT* apContext);
	virtual ~CSharingSession();

	void	SetViewSession(CSharingViewSession* apSession);

	void	HandleMessage( BYTE aType, BYTE* aData, int aDataSize);

	// overrides CSharingSessionBase
public:
	virtual void OnSessionStarted();
	virtual void OnSessionStopped();
	virtual void SendTerminateSessionRequest();

	// реализация CThreadImpl<CSharingSession>
public:
	DWORD Run();

	// реализация IDesktopSharingSession
public:
	void SetUseNewCodec();
	//! Установить качество изображения (по сути параметр сжатия для JPEG)
	void SetQuality( unsigned int auQuality);
	//! Установить максимальный FPS
	void SetMaxFPS( unsigned int auFPS);
	//! Подписка на события
	void Subscribe( IDesktopSharingSessionCallback* apCallback);
	void Unsubscribe( IDesktopSharingSessionCallback* apCallback);

	void SetSelector( IDesktopSelector* apSelector);
	void NeedKeyFrame();
	IDesktopSelector* GetSelector();

	// реализация ISharingConnectionEvents
public:
	void OnConnectLost();
	void OnConnectRestored();
	void SendStartSessionRequest();

public:
	void ShowAwarenessWnd();
	void HideAwarenessWnd();
	
	bool Destroy();
	bool IsLive();

	void SetRemoteControlMode(int aFullControl);

private:
	void ThreadEnded();
	bool CanDelete(bool aWillBeDeleteAfterThis = true);
	void CleanUp();

private:
	
	bool				useNewCodec;
	bool				needKeyFrame;
	bool				selectorChanged;
	bool				m_isDestroy; // запрос на удаление из sessionfactory
	bool				m_isThreadEnd;// запрос на удаление из потока захвата
	bool				m_deleteResultGiven;
	bool				m_isDeleted; // можно удалять
	IDesktopSelector*	desktopSelector;
	unsigned int fpsSleepDelay;
	unsigned int fps;
	unsigned int quality;
	IDesktopFrame*	previousFrame;
	CSharingViewSession* viewSession;
	VASEEncoder*	encoder;
	VASWaveletEncoder* waveletEncoder;
	CRectAwarenessWnd* awarenessWnd;
	CCriticalSection csViewSession;
	CCriticalSection csDestroy;
private:
	
	DWORD m_createThreadID;
	SharingWM_Handler m_wmMessageHandler;
};