#pragma once

#include "ViskoeThread.h"

class CSocketConnectionTester : public ViskoeThread::CThreadImpl<CSocketConnectionTester>
{
public:
	CSocketConnectionTester(void);
	~CSocketConnectionTester(void);

public:
	void	StartSocketConnection();
	void	StopSocketConnection();

	// CThreadImpl<CSocketConnectionTester>
public:
	DWORD Run();
};
