#pragma once

class CSMutex
{
public:
	CSMutex();
	virtual ~CSMutex();

public:
	void lock();
	void unlock();

private:
	CRITICAL_SECTION cs;
};
