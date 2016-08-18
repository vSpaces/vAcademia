#pragma once

#include <vector>
#include "Command.h"

class CCommandManager
{
public:
	CCommandManager(void);
	~CCommandManager(void);

	void AddInCommand(Command * command);
	void AddOutCommand(Command * command);
	Command* GetInCommand();
	Command* GetOutCommand();

	void ClearAllCommands();

private:
	Command* GetIdleCommand();

private:
	std::vector<Command *> m_inCommands;
	std::vector<Command *> m_outCommands;

	CRITICAL_SECTION m_InCommandCS;
	CRITICAL_SECTION m_OutCommandCS;
};
