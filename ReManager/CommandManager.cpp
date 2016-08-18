#include "stdafx.h"
#include "commandmanager.h"

CCommandManager::CCommandManager(void)
{
	InitializeCriticalSection(&m_InCommandCS);
	InitializeCriticalSection(&m_OutCommandCS);
}

CCommandManager::~CCommandManager(void)
{
	DeleteCriticalSection(&m_InCommandCS);
	DeleteCriticalSection(&m_OutCommandCS);
}

void CCommandManager::AddInCommand(Command* command)
{
	EnterCriticalSection(&m_InCommandCS);
	m_inCommands.push_back(command);
	LeaveCriticalSection(&m_InCommandCS);
}

void CCommandManager::AddOutCommand(Command * command)
{
	EnterCriticalSection(&m_OutCommandCS);
	m_outCommands.push_back(command);
	LeaveCriticalSection(&m_OutCommandCS);
}

Command* CCommandManager::GetInCommand()
{
	Command* command = NULL;
	EnterCriticalSection(&m_InCommandCS);
	if (!m_inCommands.empty())
	{
		std::vector<Command*>::iterator it = m_inCommands.begin();
		command = *it;
		m_inCommands.erase(it);
	}
	LeaveCriticalSection(&m_InCommandCS);
	return command;
}

Command* CCommandManager::GetOutCommand()
{
	Command* command = NULL;
	EnterCriticalSection(&m_OutCommandCS);
	if (!m_outCommands.empty())
	{
		std::vector<Command*>::iterator it = m_outCommands.begin();
		command = *it;
		m_outCommands.erase(it);
	}
	LeaveCriticalSection(&m_OutCommandCS);
//	if (command == NULL)
//		return GetIdleCommand();
	return command;
}

Command* CCommandManager::GetIdleCommand()
{
	return new Command(CMD_IDLE);
}

void CCommandManager::ClearAllCommands()
{
	EnterCriticalSection(&m_OutCommandCS);
	m_outCommands.clear();
	LeaveCriticalSection(&m_OutCommandCS);

	EnterCriticalSection(&m_InCommandCS);
	m_inCommands.clear();
	LeaveCriticalSection(&m_InCommandCS);

}

