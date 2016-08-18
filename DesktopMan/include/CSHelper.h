#pragma once

//////////////////////////////////////////////////////////////////////////
class CFactoryCSHelper
{
	CCriticalSection*	p_cs;
public:
	CFactoryCSHelper(CCriticalSection* cs) : p_cs( cs)
	{
		ATLASSERT( p_cs);
		p_cs->Lock();
	}
	~CFactoryCSHelper()
	{
		p_cs->Unlock();
	}
};