#pragma once 

#include <windows.h>

class CCpuUsage
{
public:
	CCpuUsage();
	virtual ~CCpuUsage();
 
    double GetCpuUsage(DWORD dwProcessID);

    BOOL EnablePerformaceCounters(BOOL bEnable = TRUE);
 
private:
	bool            m_bFirstTime;
	LONGLONG        m_lnOldValue;
	LARGE_INTEGER   m_OldPerfTime100nSec;
};