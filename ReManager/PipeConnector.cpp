#include "stdafx.h"
#include "pipeconnector.h"

CPipeConnector::CPipeConnector(const char * asPipeName, cs::imessagehandler * pMessageHandler, CCommandManager * commandManager)
{
	m_pMessageHandler = pMessageHandler;
	m_pCommandManager = commandManager;

	m_isReadPipeConnected = true;
	m_isWritePipeConnected = true;

	m_isWorking = false;
	m_isHandshaked = false;

	m_readPipeName = std::string(asPipeName) + "_write";
	m_writePipeName = std::string(asPipeName) + "_read";

	m_hReadPipe = CreateFile(m_readPipeName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, NULL, NULL);
	if (m_hReadPipe == NULL || m_hReadPipe == INVALID_HANDLE_VALUE)
		m_isReadPipeConnected = false;

	m_hWritePipe = CreateFile(m_writePipeName.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, NULL, NULL);
	if (m_hWritePipe == NULL || m_hWritePipe == INVALID_HANDLE_VALUE)
		m_isWritePipeConnected = false;

	m_hPipeReadThread = NULL;
	m_hPipeWriteThread = NULL;

	InitializeCriticalSection(&WriteCommandCS);
}

CPipeConnector::~CPipeConnector(void)
{
	if (m_isWorking)
		Stop();
	DeleteCriticalSection(&WriteCommandCS);
}

bool CPipeConnector::Start()
{
//	if (!isConnected())
//		return false;
	m_isWorking = true;

	m_hHandleCommandThread = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE) _HandleCommandsThreadRunner,
		this,
		0,
		&m_handleCommandThreadId);

	if (m_handleCommandThreadId == NULL || m_hHandleCommandThread == INVALID_HANDLE_VALUE)
		m_isWorking = false;

	m_hPipeReadThread = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE) _PipeConnectorReadThreadRunner,
		this,
		0,
		&m_pipeReadThreadId);

	if (m_hPipeReadThread == NULL || m_hPipeReadThread == INVALID_HANDLE_VALUE)
		m_isWorking = false;

	m_hPipeWriteThread = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE) _PipeConnectorWriteThreadRunner,
		this,
		0,
		&m_pipeWriteThreadId);

	if (m_hPipeWriteThread == NULL || m_hPipeWriteThread == INVALID_HANDLE_VALUE)
		m_isWorking = false;

	if (!m_isWorking)
		Stop();

	return m_isWorking;
}

void CPipeConnector::Stop()
{
	m_isWorking = false;
	m_isHandshaked = false;

	Disconnect();

	WaitForSingleObject(m_hPipeReadThread, INFINITE);
	CloseHandle( m_hPipeReadThread);
	m_hPipeReadThread = NULL;

	WaitForSingleObject(m_hPipeWriteThread, INFINITE);
	CloseHandle( m_hPipeWriteThread);
	m_hPipeWriteThread = NULL;

	WaitForSingleObject(m_hHandleCommandThread, INFINITE);
	CloseHandle( m_hHandleCommandThread);
	m_hHandleCommandThread = NULL;
}


bool CPipeConnector::isConnected()
{
	if (m_hReadPipe == NULL || m_hReadPipe == INVALID_HANDLE_VALUE 
		|| m_hWritePipe == NULL || m_hWritePipe == INVALID_HANDLE_VALUE)
		return false;

	return m_isReadPipeConnected && m_isWritePipeConnected;
}

bool CPipeConnector::isHandshaked()
{
	return m_isHandshaked;
}

bool CPipeConnector::isReadPipeConnected()
{
	if (m_hReadPipe == NULL || m_hReadPipe == INVALID_HANDLE_VALUE)
		return false;

	return m_isReadPipeConnected;
}

bool CPipeConnector::isWritePipeConnected()
{
	if (m_hWritePipe == NULL || m_hWritePipe == INVALID_HANDLE_VALUE)
		return false;

	return m_isWritePipeConnected;
}

void CPipeConnector::ReconnectReadPipe()
{
	m_isReadPipeConnected = false;
	if (m_hReadPipe != NULL && m_hReadPipe != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hReadPipe);
	}
	m_hReadPipe = CreateFile(m_readPipeName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (m_hReadPipe != NULL && m_hReadPipe != INVALID_HANDLE_VALUE)
		m_isReadPipeConnected = true;

	m_pCommandManager->AddOutCommand(new Command(CMD_IDLE));
}

void CPipeConnector::ReconnectWritePipe()
{
	m_isHandshaked = false;
	m_isWritePipeConnected = false;
	if (m_hWritePipe != NULL && m_hWritePipe != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hWritePipe);
	}
	m_hWritePipe = CreateFile(m_writePipeName.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (m_hWritePipe != NULL && m_hWritePipe != INVALID_HANDLE_VALUE)
		m_isWritePipeConnected = true;
}

void CPipeConnector::Disconnect()
{
	if (m_hReadPipe != NULL && m_hReadPipe != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hReadPipe);
		m_hReadPipe = NULL;
	}
	m_isReadPipeConnected = false;

	if (m_hWritePipe != NULL && m_hWritePipe != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hWritePipe);
		m_hWritePipe = NULL;
	}
	m_isWritePipeConnected = false;
}

bool CPipeConnector::ReadCommand(Command * pCommand)
{
	DWORD bytesReaded;
	DWORD dataSize;

	if ( !ReadFile(m_hReadPipe, &dataSize, sizeof(DWORD), &bytesReaded, NULL))
	{
		m_isReadPipeConnected = false;
		return false;
	}

	pCommand->ResetSize(dataSize);

	if ( !ReadFile(m_hReadPipe, pCommand->GetData(), dataSize, &bytesReaded, NULL))
	{
		m_isReadPipeConnected = false;
		return false;
	}

	return bytesReaded == dataSize;
}

bool CPipeConnector::WriteCommand(Command * pCommand)
{
	DWORD bytesWrited;
	DWORD dataSize = pCommand->GetDataSize();

	EnterCriticalSection(&WriteCommandCS);

	if ( !WriteFile(m_hWritePipe, &dataSize, sizeof(unsigned int), &bytesWrited, NULL))
	{
		m_isWritePipeConnected = false;
		return false;
	}

	if ( !WriteFile(m_hWritePipe, pCommand->GetData(), dataSize, &bytesWrited, NULL))
	{
		m_isWritePipeConnected = false;
		return false;
	}

	LeaveCriticalSection(&WriteCommandCS);

	return bytesWrited == dataSize;
}

void CPipeConnector::_PipeConnectorReadThreadRunner(LPVOID lpvParam)
{
	((CPipeConnector*)lpvParam)->_PipeConnectorReadThread();
}

void CPipeConnector::_PipeConnectorWriteThreadRunner(LPVOID lpvParam)
{
	((CPipeConnector*)lpvParam)->_PipeConnectorWriteThread();
}

void CPipeConnector::_HandleCommandsThreadRunner(LPVOID lpvParam)
{
	((CPipeConnector*)lpvParam)->_HandleCommandsThread();
}

void CPipeConnector::_PipeConnectorReadThread()
{
	// чтение комманд редактора
	while (m_isWorking)
	{
		if ( !isReadPipeConnected())
			ReconnectReadPipe();

		while ( isConnected())
		{
			Command * inCommand = new Command(CMD_UNKNOWN);
			if ( !ReadCommand(inCommand))
			{
				delete inCommand;
				break;
			}
			m_pCommandManager->AddInCommand(inCommand);
		}	
		Sleep(20);
	}

}

void CPipeConnector::_PipeConnectorWriteThread()
{
	// посылка комманд редактору
	while (m_isWorking)
	{
		if ( !isWritePipeConnected())
			ReconnectWritePipe();

		while ( isConnected())
		{
			Command * outCommand = m_pCommandManager->GetOutCommand();
			if (outCommand != NULL)
			{
				if ( !WriteCommand(outCommand))
				{
					delete outCommand;
					break;
				}
				delete outCommand;
			}
		}	
		Sleep(20);
	}

}

void CPipeConnector::_HandleCommandsThread()
{
	BYTE type = 0;
	// обработка входящих комманд
	while (m_isWorking)
	{
		Sleep(20);

		Command * inCommand =  m_pCommandManager->GetInCommand();

		if (inCommand != NULL)
		{
			type = inCommand->GetType();
			m_pMessageHandler->HandleMessage( type, inCommand->GetData() + 1, inCommand->GetDataSize() - 1, 0);

			if (type == CMD_HANDSHAKE)
				m_isHandshaked = true;
			delete inCommand;
		}
	}	
}

void CPipeConnector::PrepairToDestroy()
{
	m_pCommandManager->ClearAllCommands();
	Command * exitCommand = new Command(CMD_EXIT);
	WriteCommand(exitCommand);
	delete exitCommand;
}

