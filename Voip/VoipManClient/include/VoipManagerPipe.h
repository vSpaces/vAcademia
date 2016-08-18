#pragma once

#include <map>
#include <vector>
#include "Mutex.h"
#include "PipeServer.h"
#include "CommandQueue.h"
#include "Commands.h"
#include "..\..\include\ConaitoSDK\VoipManagerConaito.h"
#include "include\ConaitoSDK\VoipManagerConaitoServer.h"
#include "include\VoipManagerPipeEVO.h"


class CVoipManagerPipe
{
	friend class CVoipManagerConaito;
public:
	CVoipManagerPipe(oms::omsContext* aContext, CVoipManagerConaito *aVoipManagerConaito);
	~CVoipManagerPipe(void);	

	void Run();
	bool IsRestoringConnect();
	void SetRestoredConnect();

	// пайп-протокол отправка
public:	
	void SendInitVoipSystem( int aiVasDevice, int aiToIncludeSoundDevice, const wchar_t *alpcDeviceName, const wchar_t *alpcOutDeviceNamee);
	void SendSetVoiceGainLevel( float aVal);
	void SendHandleTransmissionStateChanged( bool aMicrophoneEnabled, bool aVoiceActivate);
	void SendSetVoiceActivationLevel( float aVal);
	void SendSetAutoGaneControlEnabled( bool abEnabled);
	void SendEnableDenoising( bool abEnabled);
	void SendGetDevices( bool abNeedAnswerToRmml, bool abForce);
	void SendGetInputDevices(bool abNeedAnswerToRmml, bool abForce);
	void SendGetOutputDevices(bool abNeedAnswerToRmml, bool abForce);
	//void SendGetInputDevices();
	void SendSetUserGainLevel( const wchar_t* alwpcUserLogin, double adUserGainLevel);
	void SendConnectImpl( const char *apwServerIP, std::vector<IProxySettings*>* aProxySettingsVec, int aTcpPort, int anUdpPort, bool aCheckConnecting);
	void SendJoinChannel( const char *aRoomName);
	void SendDisconnect();
	void SendDoLogin( const wchar_t *aUserName, const wchar_t *aUserPassword, const wchar_t *aRoomName);
	//void SendGetUser( int aMsgID, int aUserID);
	void SendSetUserVolume( const wchar_t *aUserName, int aVolume);
	void SendIncomingVoiceVolume( int aVolume);
	void SendEchoCancellation(bool anEnabled);
	void SendMixerEchoCancellation(bool anEnabled);
	void SendStartFindRealMicrophon( );
	void SendEndFindRealMicrophon( );
	
	void CloseVoipManServer();

protected:
	//потоки

	void IncNumPipe( CComString &sPipe, char *aBaseName, int &iNumPipe);
	static DWORD WINAPI ThreadWritePipeRunner(LPVOID);
	static DWORD WINAPI ThreadReadPipeRunner(LPVOID);
	static DWORD WINAPI ThreadProcessCommand(LPVOID);
	static DWORD WINAPI ThreadTimeCommand(LPVOID);
	static DWORD WINAPI ThreadServerRunner(LPVOID);
	static DWORD WINAPI ThreadConnectPipes(LPVOID);
	void WritePipeServerThread();
	void ReadPipeServerThread();
	void ServerRunnerThread();
	void ProcessCommand();
	void CheckTime();
	void ConnectPipesAsync();

protected:
	void ProcessCommandById(int cmdID, CDataStorage* commandData);
	std::wstring GetPathVoipManServer();
	bool KillVoipManServerProcess( int aProcessId);
	int FindVoipManServerProcesses( std::wstring &sPathFile, bool abClose);
	bool RunVoipManagerServer(bool abClose, bool& abIntoPlayer);
	bool CreatePipes();
	void DestroyPipes();
	bool ConnectPipes();
	void ReleaseThread( HANDLE &aThread);

	// пайп-протокол получение
protected:
	void OnDeviceErrorMsg(CDataStorage* data);
	void OnVoipManAliveMsg(CDataStorage* data);
	void OnIdle(CDataStorage* data);
	void OnSendInitVoipSystem(CDataStorage* data);
	void OnSetVoiceGainLevel(CDataStorage* data);
	void OnStartTransmitting(CDataStorage* data);
	void OnStopTransmitting(CDataStorage* data);
	void OnEnableVoiceActivation(CDataStorage* data);
	void OnGetMyUserID(CDataStorage* data);
	void OnGetDevices(CDataStorage* data);
	void OnGetInputDevices( CDataStorage* data);
	void OnGetOutputDevices( CDataStorage* data);
	void OnConnectToEVoipResult( CDataStorage* data);

	void OnConnectionStatus( CDataStorage* data);
	void OnChannelJoined( CDataStorage* data);
	void OnChannelLeft( CDataStorage* data);
	void OnAddUser( CDataStorage* data);
	void OnRemoveUser( CDataStorage* data);
	void OnUserTalking( CDataStorage* data);
	void OnKicked( CDataStorage* data);
	void OnSetCurrentInputLevel( CDataStorage* data);
	void WriteLog( LPCSTR alpcLogData);
	void OnServerLog( CDataStorage* data);
	void OnAutoFindedMic(CDataStorage* data);

protected:	

	oms::omsContext* context;

	// Объекты пайпов
	CPipeServer	m_readPipeServer;
	CPipeServer	m_writePipeServer;

	bool m_needCommandProcessing;
	CCommandQueue m_commandsData;
	DWORD m_writePipeThreadId; 
	HANDLE m_hWritePipeThread;
	DWORD m_readPipeThreadId; 
	HANDLE m_hReadPipeThread;
	DWORD m_processThreadId;
	HANDLE m_hProcessThread;
	DWORD m_timeThreadId;
	HANDLE m_hTimeThread;
	volatile bool bRunning;
	volatile unsigned int lastCommandReceivedTime;
	volatile unsigned int maxDelayBetweenCommands;
	unsigned int pipesID;

	CVoipManagerConaito *pVoipManagerConaito;
	CVoipManagerPipeEVO *pVoipManagerPipeEVO;
	CVoipManagerConaitoServer *pVoipManagerConaitoServer;
	CMutex mutexRun;
	CMutex mutexClose;
	bool bDisConnected;
	bool bRestoringEVO;
	CComString sPipeReadFromEVOIP;
	CComString sPipeWriteToEVOIP;
	PROCESS_INFORMATION pi;
	volatile bool bRunningCheckTime;
	DWORD m_voipManProcessID;
	volatile bool bConnectError;
	volatile bool bReChangePipes;
	HANDLE eventConnectPipes;
public:
};