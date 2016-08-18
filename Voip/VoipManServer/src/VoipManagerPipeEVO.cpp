#include "StdAfx.h"
#include "..\include\voipManagerPipeEVO.h"
#include "include/PipeProtocol/ConfirmationOut.h"
#include "include/PipeProtocol/ConnectionStatusOut.h"
#include "include/PipeProtocol/ChannelJoinedOut.h"
#include "include/PipeProtocol/ChannelLeftOut.h"
#include "include/PipeProtocol/UserTalkingOut.h"
#include "include/PipeProtocol/KickedOut.h"
#include "include/PipeProtocol/AddUserOut.h"
#include "include/PipeProtocol/RemoveUserOut.h"
#include "include/PipeProtocol/CurrentInputLevelOut.h"
#include "include/PipeProtocol/LogOut.h"
#include "include/PipeProtocol/GetMyUserIDOut.h"
#include "include/PipeProtocol/WizardFindedDeviceOut.h"

CVoipManagerPipeEVO::CVoipManagerPipeEVO(oms::omsContext* aContext, CVoipManagerConaitoServer *aVoipManagerConaitoServer, LPCSTR apWritePipe, LPCSTR apReadPipe)
{
	context = aContext;
	mutexNeedProcessCommand = NULL;
	/*if( !aNoPlayer)
		mutexNeedProcessCommand = CreateEvent( NULL, FALSE, FALSE, "mutexNeedProcessCommandEVO");*/
	bRunning = false;
	//m_needCommandProcessing = false;
	pVoipManagerConaitoServer = aVoipManagerConaitoServer;
	pVoipManagerConaitoServer->SetManagerPipe( this);
	bFilesIsNotFind = 0;
	lastInputLevel = -10;
	bTwoApplication = false;
	sPipeWriteToEVOIP = apReadPipe;
	sPipeReadFromEVOIP = apWritePipe;
	lastTickSendMsg = 0;

	m_writePipeClient.SetContext(aContext);
	m_readPipeClient.SetContext(aContext);
}

void CVoipManagerPipeEVO::ReleaseThread( HANDLE &aThread)
{
	if( aThread == NULL)
		return;
	if( WaitForSingleObject( aThread, 2000) == WAIT_TIMEOUT)
	{
		mutexClose.lock();
		if(aThread!=NULL)
		{
			//09.04.2014 лишь при закрытии приложения - иначе риск повисаний
			::TerminateThread( aThread, 0);
			CloseHandle(aThread);
			aThread = NULL;
		}
		mutexClose.unlock();
	}
}

CVoipManagerPipeEVO::~CVoipManagerPipeEVO(void)
{	
	bRunning = false;
	//DestroyPipes(); - может повиснуть на закрытии пайпа если поднимался внутри академии
	context = NULL;
	ReleaseThread( m_hWritePipeThread);	
	ReleaseThread( m_hReadPipeThread);
	ReleaseThread( m_hProcessThread);
}

/*bool CVoipManagerPipeEVO::IsConnected()
{
	return m_writePipeClient.IsConnected() && m_readPipeClient.IsConnected();
}*/

void CVoipManagerPipeEVO::DestroyPipes()
{
	CComString sLog;
	if( context->mpLogWriter != NULL)
		context->mpLogWriter->WriteLn( "[VOIP_PIPE] Destroy pipes");

	// Удаляем пишуший пайп
	int errorCode = 0;
	if( m_writePipeClient.IsConnected())
	{
		m_writePipeClient.Disconnect();
		sLog.Format("[VOIP_PIPE] m_writePipeServer was disconnected. Disconnect result of %s is %u", sPipeWriteToEVOIP.GetBuffer(), errorCode);
	}
	else
	{
		sLog.Format("[VOIP_PIPE] m_writePipeServer is not connected. Destroy is not needed.");
	}
	if( context->mpLogWriter != NULL)
		context->mpLogWriter->WriteLn( sLog.GetBuffer());

	// Удаляем читающий пайп
	if( m_readPipeClient.IsConnected())
	{
		m_readPipeClient.Disconnect();
		sLog.Format("[VOIP_PIPE] m_readPipeServer was disconnected. Disconnect result of %s is %u", sPipeReadFromEVOIP.GetBuffer(), errorCode);
	}
	else
	{
		sLog.Format("[VOIP_PIPE] m_readPipeServer is not connected. Destroy is not needed.");
	}
	if( context->mpLogWriter != NULL)
		context->mpLogWriter->WriteLn( sLog.GetBuffer());

	if( context->mpLogWriter != NULL)
		context->mpLogWriter->WriteLn( "[VOIP_PIPE] Pipes was destroyed");
}


void CVoipManagerPipeEVO::Run( bool abTwoApplication)
{
	bTwoApplication = abTwoApplication;
	bRunning = true;
	//MessageBox(NULL, sPipeWriteToEVOIP.GetBuffer(), "\\\\.\\pipe\\voipmanpipetoEVOIP", MB_OK);
	//m_readPipeClient.SetPipeName( "\\\\.\\pipe\\voipmanpipetoEVOIP");
	m_readPipeClient.SetPipeName( sPipeWriteToEVOIP.GetBuffer());
	int iReadConnectError = m_readPipeClient.TryConnect();	

	CSimpleThreadStarter pipeThreadStarter(__FUNCTION__);
	pipeThreadStarter.SetRoutine(ThreadReadPipeRunner);
	pipeThreadStarter.SetParameter(this);
	m_hReadPipeThread = pipeThreadStarter.Start();
	m_readPipeThreadId = pipeThreadStarter.GetThreadID();

	if (m_hReadPipeThread == NULL)
	{
		if( context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLn("[VOIPSERVER] ThreadReadPipeRunner is not runner errorCode="+GetLastError());
	}

	CSimpleThreadStarter commandThreadStarter(__FUNCTION__);
	commandThreadStarter.SetRoutine(ThreadProcessCommand);
	commandThreadStarter.SetParameter(this);
	m_hProcessThread = commandThreadStarter.Start();
	m_processThreadId = commandThreadStarter.GetThreadID();

	if (m_hProcessThread == NULL)
	{
		if( context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLn("[VOIPSERVER] ThreadProcessCommand is not runner errorCode="+GetLastError());
	}
	m_writePipeClient.SetPipeName( sPipeReadFromEVOIP.GetBuffer());
	//m_writePipeClient.SetPipeName( "\\\\.\\pipe\\voipmanpipefromEVOIP");
	int iWriteConnectError = m_writePipeClient.TryConnect();
	CComString sLog; sLog.Format( "[VOIPSERVER] CVoipManagerPipeEVO::Run  iReadConnectError==%d,  iWriteConnectError==%d", iReadConnectError, iWriteConnectError);
	pVoipManagerConaitoServer->WriteLogLine( sLog.GetBuffer());
//.Format("[VOIP_PIPE] m_readPipeServer is connected.");

	CSimpleThreadStarter writeThreadStarter(__FUNCTION__);
	writeThreadStarter.SetRoutine(ThreadWritePipeRunner);
	writeThreadStarter.SetParameter(this);
	m_hWritePipeThread = writeThreadStarter.Start();
	m_writePipeThreadId = writeThreadStarter.GetThreadID();

	if (m_hWritePipeThread == NULL)
	{
		if( context->mpLogWriter != NULL)
			context->mpLogWriter->WriteLn("[VOIPSERVER] ThreadWritePipeRunner is not runner errorCode="+GetLastError());
	}
}

DWORD WINAPI CVoipManagerPipeEVO::ThreadWritePipeRunner(LPVOID lpvParam)
{
	((CVoipManagerPipeEVO*)lpvParam)->WritePipeClientThread();
	return 0;
}

void CVoipManagerPipeEVO::SendDestroyMsg()
{
	SendMessage( hWndMainServer, WM_CLOSE, 0, 0);
	SendMessage( hWndMainServer, WM_DESTROY, 0, 0);
}

void CVoipManagerPipeEVO::WritePipeClientThread()
{	
	while ( bRunning)
	{
		if( !m_writePipeClient.IsConnected())
		{
			int errorCode = m_writePipeClient.TryConnect();			
			if( bTwoApplication && (errorCode == ERROR_FILE_NOT_FOUND /*|| errorCode == ERROR_PATH_NOT_FOUND*/))
			{
				bFilesIsNotFind++;
				if( bFilesIsNotFind > 1)
				{
					bRunning = false;
					if( hWndMainServer != NULL)
					{
						if( pVoipManagerConaitoServer != NULL)
							pVoipManagerConaitoServer->WriteLogLine("[VOIPSERVER] i am close");
						SendDestroyMsg();
						break;
					}
				}
			}
			Sleep(500);
			continue;
		}

		/*mutex.lock();
		if( bFilesIsNotFind > 0)
			bFilesIsNotFind &= ~1;
		mutex.unlock();*/

		std::vector<CDataStorage*> commands;
		CDataStorage* outCommandData = m_commandsData.GetOutCommand();
		if( outCommandData == NULL)
		{			
			//if(pVoipManagerConaitoServer->IsVoipSystemInitialized())
			SendCurrentInputLevel();
			/*else
				Sleep(500);*/
		}
		else while (outCommandData != NULL)
		{
			commands.push_back(outCommandData);
			outCommandData = m_commandsData.GetOutCommand();
		}
		
		bool bDisConnected = false;
		if(commands.size() > 0)
		{
			m_writePipeClient.WriteData(commands, bDisConnected); // отправка команды		
			if(bDisConnected)
			{
				Sleep(100);
				m_writePipeClient.TryConnect();
				continue;
			}
		}					
		Sleep(50);
	}	
	mutexClose.lock();
	m_hWritePipeThread = NULL;
	mutexClose.unlock();
}

DWORD WINAPI CVoipManagerPipeEVO::ThreadReadPipeRunner(LPVOID lpvParam)
{
	((CVoipManagerPipeEVO*)lpvParam)->ReadPipeClientThread();
	return 0;
}

void CVoipManagerPipeEVO::ReadPipeClientThread()
{
	while ( bRunning)
	{
		if( !m_readPipeClient.IsConnected())
		{
			int errorCode = m_readPipeClient.TryConnect();
			if( bTwoApplication && (errorCode == ERROR_FILE_NOT_FOUND /*|| errorCode == ERROR_PATH_NOT_FOUND*/))
			{
				//mutex.lock();
				bFilesIsNotFind++;
				//mutex.unlock();
				if( bFilesIsNotFind > 1)
				{
					bRunning = false;
					if( hWndMainServer != NULL)
					{
						if( pVoipManagerConaitoServer != NULL)
							pVoipManagerConaitoServer->WriteLogLine("[VOIPSERVER] i am close");
						SendDestroyMsg();
						break;
					}
				}
			}
			Sleep(500);
			continue;
		}
		/*if ( m_needCommandProcessing)
		{
			Sleep(100);
			continue;
		}*/
		//mutex.lock();
		//if( bFilesIsNotFind > 0)
		bFilesIsNotFind = 0;
		//mutex.unlock();

		bool bDisConnected = false;		
		CDataStorage *data = NULL;
		if(!bDisConnected)
		{
			int count = m_readPipeClient.ReadCount( bDisConnected);
			for (int packetNum = 0; packetNum < count; packetNum++)
			{
				CDataStorage* inCommandData = m_readPipeClient.ReadData( bDisConnected);
				if(bDisConnected)
				{
					break;
				}
				if( inCommandData == NULL)
				{
					packetNum--;
					Sleep(50);
					continue;
				}
				m_commandsData.AddInCommand( inCommandData); // чтение команды
			}
			if(bDisConnected)
			{
				Sleep(100);
				m_readPipeClient.TryConnect();
				continue;
			}
			if( mutexNeedProcessCommand != NULL)
				SetEvent( mutexNeedProcessCommand);
			//m_needCommandProcessing = true;
		}							

		Sleep(50);
	}	
	mutexClose.lock();
	m_hReadPipeThread = NULL;
	mutexClose.unlock();
}

DWORD WINAPI  CVoipManagerPipeEVO::ThreadProcessCommand(LPVOID lpvParam)
{
	((CVoipManagerPipeEVO*)lpvParam)->ProcessCommand();
	return 0;
}

// чтение данных от сервера менеджера и передача их на исполнению клиент менеджера
void CVoipManagerPipeEVO::ProcessCommand()
{	
	while ( bRunning)
	{
		if( mutexNeedProcessCommand != NULL)
			WaitForSingleObject( mutexNeedProcessCommand, INFINITE);
		if (!m_readPipeClient.IsConnected())
		{
			Sleep(500);
			continue;
		}

		/*if (!m_needCommandProcessing)
		{
			Sleep(100);
			continue;
		}*/		

		byte cmdID = 0;
		CDataStorage* commandData = m_commandsData.GetInCommand();

		while (commandData != NULL)
		{
			if ((commandData->Read(cmdID) && cmdID != 0 /*&& cmdID != 1*/))
			{
				break;
			}
			if( pVoipManagerConaitoServer != NULL)
				pVoipManagerConaitoServer->WriteLogLine("analyse number protocol message is false");
			MP_DELETE(commandData);
			commandData = m_commandsData.GetInCommand();
		}

		if (commandData != NULL)
		{			
			if (cmdID == CMD_IDLE)
			{
				MP_DELETE(commandData);
				/*CDataStorage *outCommandData = GetIdleData();
				//printf("ProcessCommand::CMD_IDLE\n");
				m_commandsData.AddOutCommand( outCommandData);*/
				//m_needCommandProcessing = false;
				continue;
			}
			//if( pVoipManagerConaitoServer != NULL)
			//	pVoipManagerConaitoServer->SendMessageToWnd( cmdID, commandData);
			ProcessCommandById(cmdID, commandData);
			MP_DELETE(commandData);
		}
		//m_needCommandProcessing = false;
		if (m_commandsData.GetInCommandSize() == 0)
			Sleep(50);
		else
			Sleep(1);
	}
	mutexClose.lock();
	m_hProcessThread = NULL;
	mutexClose.unlock();
}

/*CDataStorage* CVoipManagerPipeEVO::GetIdleData()
{
	MP_NEW_P(idleData, CDataStorage, 4);
	BYTE idleByte = CMD_IDLE;
	idleData->Add(idleByte);

	return idleData;
}*/

void CVoipManagerPipeEVO::SendCommit()
{
	// закомментрировано так как посылается CURRENTINPUTLEVEL
	//MP_NEW_P( packetOut, CConfirmationOut, CMD_CONFIRMATION);
	//m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipeEVO::SendConnectionStatus( int aConnectionStatus)
{
	CConnectionStatusOut packetOut( CMD_CONNECTIONSTATUS, aConnectionStatus);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipeEVO::SendChannelJoined( wchar_t *aChannelPath)
{
	CChannelJoinedOut packetOut( CMD_CHANNELJOINED, aChannelPath);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipeEVO::SendAutoSelectedMic( wchar_t *aDeviceGuid, wchar_t *aDeviceName, wchar_t *aDeviceLineName)
{
	CWizardFindedDeviceOut packetOut( CMD_AUTOFINDEDMIC, aDeviceGuid, aDeviceName, aDeviceLineName);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipeEVO::SendChannelLeft()
{
	CChannelLeftOut packetOut( CMD_CHANNELLEFT);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipeEVO::SendUserTalking( unsigned int aUserID, wchar_t *aUserNick)
{
	byte btTalk = 1;
	CUserTalkingOut packetOut( CMD_USERTALKINGSTATUS, btTalk, aUserID, aUserNick);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipeEVO::SendUserStoppedTalking( unsigned int aUserID, wchar_t *aUserNick)
{
	byte btTalk = 0;
	CUserTalkingOut packetOut( CMD_USERTALKINGSTATUS, btTalk, aUserID, aUserNick);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipeEVO::SendKicked()
{
	CKickedOut packetOut( CMD_KICKED);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipeEVO::SendAddUser( unsigned int aUserID, wchar_t *aUserNick)
{
	/*CAddUserOut packetOut( CMD_ADDUSER, aUserID, aUserNick);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());*/
}

void CVoipManagerPipeEVO::SendRemoveUser( unsigned int aUserID)
{
	/*CRemoveUserOut packetOut( CMD_REMOVEUSER, aUserID);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());*/
}

void CVoipManagerPipeEVO::SendCurrentInputLevel()
{
	int connectStatus = pVoipManagerConaitoServer->GetConnectionStatus();
	float currentInputLevel = 0;
	if( connectStatus == CS_CONNECTED)
		currentInputLevel = pVoipManagerConaitoServer->GetCurrentInputLevel();		
	/*else
		if( lastInputLevel < 0.05)
			return;*/
		
	int curTickSendMsg = GetTickCount();
	if( lastTickSendMsg + 500 < curTickSendMsg || currentInputLevel > lastInputLevel + 0.0001 || currentInputLevel < lastInputLevel - 0.0001)
	{
		lastTickSendMsg = curTickSendMsg;
		CCurrentInputLevelOut packetOut( CMD_CURRENTINPUTLEVEL, currentInputLevel);
		m_commandsData.AddOutCommand( packetOut.GetDataStorage());
		lastInputLevel = currentInputLevel;
	}
}

void CVoipManagerPipeEVO::SendMyUserID( int anUserID)
{
	CGetMyUserIDOut packetOut( CMD_GETMYUSERID, anUserID, true);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

void CVoipManagerPipeEVO::SendDeviceErrorCode( int aDeviceCode)
{
	CConnectionStatusOut packetOut( CMD_DEVICE_ERROR_CODE, aDeviceCode);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}

/*void CVoipManagerPipeEVO::SendLog( const wchar_t* asLog)
{
	USES_CONVERSION;
	SendLog( W2A( asLog));
}*/

void CVoipManagerPipeEVO::SendLog( const char* asLog)
{
	CLogOut packetOut( CMD_LOG, asLog);
	m_commandsData.AddOutCommand( packetOut.GetDataStorage());
}
