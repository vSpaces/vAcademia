#pragma once

#include "DataStorage.h"

class CCommandQueue
{
public:
	CCommandQueue(void);
	~CCommandQueue(void);

	void AddOutCommand(CDataStorage* outCmdData);
	CDataStorage* GetOutCommand();

	void AddInCommand(CDataStorage* inCmdData);
	CDataStorage* GetInCommand();
	int GetInCommandSize();

	void ClearInCommands();
	void ClearOutCommands();

private:
	MP_VECTOR<CDataStorage*> m_outCmdQueue;
	MP_VECTOR<CDataStorage*> m_inCmdQueue;

	CRITICAL_SECTION m_outCs, m_inCs;
};