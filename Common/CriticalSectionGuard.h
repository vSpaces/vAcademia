#pragma once

class CCriticalSectionGuard
{
public:
	CCriticalSectionGuard( CRITICAL_SECTION* aCriticalSection)
	{
		criticalSection = NULL;
		lock( aCriticalSection);
	}

	~CCriticalSectionGuard()
	{
		unlock();
	}

	void lock(CRITICAL_SECTION* aCriticalSection)
	{
		unlock();
		criticalSection = aCriticalSection;
		if (criticalSection == NULL)
			return;
		EnterCriticalSection( criticalSection);
	}

	void unlock()
	{
		if (criticalSection == NULL)
			return;
		LeaveCriticalSection( criticalSection);
		criticalSection = NULL;
	}

private:
	CRITICAL_SECTION* criticalSection;
};
