#pragma once

#include <map>
#include <vector>
#include "Mutex.h"
#include "PipeClient.h"
#include "CommandQueue.h"
#include "Commands.h"
#include "include\ConaitoSDK\VoipManagerConaitoServer.h"


class CVoipManagerPipeEVO
{
	friend class CVoipManagerConaitoServer;
public:
	CVoipManagerPipeEVO(oms::omsContext* aContext, CVoipManagerConaitoServer *aVoipManagerConaitoServer, LPCSTR apWritePipe, LPCSTR apReadPipe);
	~CVoipManagerPipeEVO(void);

	//bool IsConnected();
	void Run( bool abTwoApplication);
protected:
	void ReleaseThread( HANDLE &aThread);
	void DestroyPipes();
	//CDataStorage* GetIdleData();

	static DWORD WINAPI ThreadWritePipeRunner(LPVOID);
	static DWORD WINAPI ThreadReadPipeRunner(LPVOID);
	static DWORD WINAPI ThreadProcessCommand(LPVOID);
	void WritePipeClientThread();
	void ReadPipeClientThread();
	void ProcessCommand();
	void ProcessCommandById(int cmdID, CDataStorage* commandData);
	void SendDestroyMsg();
protected:
	void OnAliveMsg( CDataStorage* data);
	void OnIdle(CDataStorage* data);
	void OnInitVoipSystem( CDataStorage* data);
	void OnSetVoiceGainLevel( CDataStorage* data);
	void OnHandleTransmissionStateChanged(CDataStorage* data);
	void OnSetVoiceActivationLevel( CDataStorage* data);
	void OnSetAutoGaneControlEnabled( CDataStorage* data);
	void OnSetEnableDenoising( CDataStorage* data);
	void OnGetDevices( CDataStorage* data);
	void OnGetInputDevices( CDataStorage* data);
	void OnGetOutputDevices( CDataStorage* data);
	void OnSetUserGainLevel( CDataStorage* data);
	void OnConnectToEVoip( CDataStorage* data);
	void OnJoinChannel( CDataStorage* data);
	void OnDisconnectEVoip( CDataStorage* data);
	void OnDoLogin( CDataStorage* data);
	void OnGetUser( CDataStorage* data);
	void OnSetUserVolume( CDataStorage* data);
	void OnCloseVoipManServer( CDataStorage* data);
	void OnSetMasterVolume( CDataStorage* data);
	void OnSetEchoCancellation( CDataStorage* data);
	void OnSetMixerEchoCancellation( CDataStorage* data);
	void OnStartFindMicDevice(CDataStorage* data);
	void OnEndFindMicDevice(CDataStorage* data);

	void SendCommit();
	void SendConnectionStatus( int aConnectionStatus);
	void SendChannelJoined( wchar_t *aChannelPath);
	void SendChannelLeft();
	void SendUserTalking( unsigned int aUserID, wchar_t *aUserNick);
	void SendUserStoppedTalking( unsigned int aUserID, wchar_t *aUserNick);
	void SendKicked();
	void SendAddUser( unsigned int aUserID, wchar_t *aUserNick);
	void SendRemoveUser( unsigned int aUserID);
	void SendCurrentInputLevel();
	void SendLog( const char* asLog);
	//void SendLog( const wchar_t* asLog);
	void SendMyUserID( int anUserID);
	void SendDeviceErrorCode( int aDeviceCode);
	void SendAutoSelectedMic( wchar_t *aDeviceGuid, wchar_t *aDeviceName, wchar_t *aDeviceLineName);

protected:	

	oms::omsContext* context;
	CPipeClient	m_readPipeClient;
	CPipeClient	m_writePipeClient;
	//bool m_needCommandProcessing;
	CCommandQueue m_commandsData;
	DWORD m_writePipeThreadId; 
	HANDLE m_hWritePipeThread;
	DWORD m_readPipeThreadId; 
	HANDLE m_hReadPipeThread;
	DWORD m_processThreadId; 
	HANDLE m_hProcessThread;
	volatile bool bRunning;
	CVoipManagerConaitoServer *pVoipManagerConaitoServer;
	HANDLE mutexNeedProcessCommand;
	CMutex mutexClose;
	int bFilesIsNotFind;
	bool bFirstSended;
	HWND hWndMainServer;
	float lastInputLevel;
	bool bTwoApplication;
	CComString sPipeReadFromEVOIP;
	CComString sPipeWriteToEVOIP;
	int lastTickSendMsg;
public:
	void SetHWND( HWND ahWndMainServer){ hWndMainServer = ahWndMainServer;};
};