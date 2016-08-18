#pragma once

#include "std_define.h"

class CMutex
{
public:
	CMutex();
	virtual ~CMutex();

public:
	void lock();
	bool tryToLock();
	void unlock();

	void Wait() { lock();};
	void Signal() { unlock();};

private:
	CRITICAL_SECTION cs;
};

class CWaitAndSignal
{
public:
	CWaitAndSignal( CMutex &mutex);
	~CWaitAndSignal();

private:
	CMutex &pMutex;
};
