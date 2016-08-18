#pragma once

#include <memory.h>
#include <windows.h>
#include "Command.h"
#include <string>
#include <vector>
#include "CommandManager.h"
#include "imessagehandler.h"
#include "IRecordEditor.h"

#include "Protocol\RecordEditorProtocolIn.h"
#include "Protocol\GetRecordListOut.h"

using namespace std;

struct IRecordEditor;

class CPipeConnector
{
public:
	CPipeConnector(const char * asPipeName, cs::imessagehandler * pMessageHandler, CCommandManager * commandManager);
	~CPipeConnector(void);
	
public:

	bool Start();
	void Stop();

	bool isConnected();
	bool isHandshaked();

	void PrepairToDestroy();

private:

	bool isReadPipeConnected();
	bool isWritePipeConnected();

	void ReconnectReadPipe();
	void ReconnectWritePipe();

	void Disconnect();

	bool ReadCommand(Command * pCommand);
	bool WriteCommand(Command * pCommand);

	static void _PipeConnectorReadThreadRunner(LPVOID lpvParam);
	static void _PipeConnectorWriteThreadRunner(LPVOID lpvParam);
	static void _HandleCommandsThreadRunner(LPVOID lpvParam);

	void _PipeConnectorReadThread();
	void _PipeConnectorWriteThread();
	void _HandleCommandsThread();

private:
	DWORD	m_pipeReadThreadId; 
	DWORD	m_pipeWriteThreadId; 
	DWORD	m_handleCommandThreadId; 

	HANDLE	m_hPipeReadThread; 
	HANDLE	m_hPipeWriteThread; 
	HANDLE	m_hHandleCommandThread;

	HANDLE m_hReadPipe;
	HANDLE m_hWritePipe;

	std::string m_readPipeName;
	std::string m_writePipeName;

	bool m_isReadPipeConnected;
	bool m_isWritePipeConnected;

	bool m_isWorking;
	bool m_isHandshaked;

	CRITICAL_SECTION WriteCommandCS;

	cs::imessagehandler * m_pMessageHandler;
	CCommandManager * m_pCommandManager;
};
