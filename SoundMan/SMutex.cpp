#include "stdafx.h"
#include ".\smutex.h"

CSMutex::CSMutex()
{
	InitializeCriticalSection( &cs); 
}

CSMutex::~CSMutex()
{
	DeleteCriticalSection( &cs);
}

void CSMutex::lock()
{
	EnterCriticalSection( &cs);
}

void CSMutex::unlock()
{
	LeaveCriticalSection( &cs);
}