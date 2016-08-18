#include "stdafx.h"
#include "Destroyableconnection.h"

CDestroyableConnection::CDestroyableConnection(unsigned int aDestroyCallCount)
{
	m_destroyCount = aDestroyCallCount;
}

CDestroyableConnection::~CDestroyableConnection(void)
{
}

void CDestroyableConnection::destroy()
{
	bool needDestroy = false;

	m_lock.lock();
	m_destroyCount--;
	if (m_destroyCount <= 0)
		needDestroy = true;
	m_lock.unlock();

	if (needDestroy)
		_destroy();
}
