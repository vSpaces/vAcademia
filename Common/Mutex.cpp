#include "stdafx.h"
#include "mutex.h"

/*#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x400
#endif*/

#include "Windows.h"
#include "winbase.h"

CMutex::CMutex()
{
	InitializeCriticalSection( &cs); 
}

CMutex::~CMutex()
{
	DeleteCriticalSection( &cs);
}

void CMutex::lock()
{
	EnterCriticalSection( &cs);
}

void CMutex::unlock()
{
	LeaveCriticalSection( &cs);
}

bool CMutex::tryToLock()
{
	return TryEnterCriticalSection(&cs)!=0;
}

//////////////////////////////////////////////////////////////////////////

CWaitAndSignal::CWaitAndSignal( CMutex &mutex):
pMutex( ( CMutex &)mutex)
{	
	pMutex.lock();
}

CWaitAndSignal::~CWaitAndSignal()
{
	pMutex.unlock();
}