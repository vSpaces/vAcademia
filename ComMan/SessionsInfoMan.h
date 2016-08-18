// AsyncMan.h: interface for the CAsyncMan class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SESSIONINFOMAN_H__INCLUDED_)
#define AFX_SESSIONINFOMAN_H__INCLUDED_

#include "oms_context.h"
#include "mutex.h"

class CMessageTime
{
public:
	int count;
	int totalTime;
	CMessageTime()
	{
		count = 0;
		totalTime = 0;
	}	
};

class CSessionsInfoMan  
{
public:
	CSessionsInfoMan( oms::omsContext *aContext);
	virtual ~CSessionsInfoMan();

public:
	void AddSessionInfo( int aNum, int aType, int aDeltaTime);	
	bool Log();

protected:
	void Clear();

protected:
	oms::omsContext *pContext;
	int totalDeltaTime;
	CMutex mutex;
};

#endif // !defined(AFX_SESSIONINFOMAN_H__INCLUDED_)
