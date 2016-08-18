// AsyncMan.cpp: implementation of the CAsyncMan class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SessionsInfoMan.h"

const int countSession = 12;
const int countType = 255;
CMessageTime sessionInfo[ countSession][ countType];
int needWriteLog[countSession];
CComString sessionName[countSession];

CSessionsInfoMan::CSessionsInfoMan( oms::omsContext *aContext)
{	
	pContext = aContext;
	sessionName[0] = "World";
	sessionName[1] = "Service";
	sessionName[2] = "Res";
	sessionName[3] = "ResXml";
	sessionName[4] = "ResUpl";
	sessionName[5] = "Sync";
	sessionName[6] = "VoipRec";
	sessionName[7] = "VoipPlay";
	sessionName[8] = "VoipSelf";
	sessionName[9] = "Preview";
	sessionName[10] = "UDPSer";
	sessionName[11] = "Sharing";
	Clear();
}

CSessionsInfoMan::~CSessionsInfoMan()
{
	pContext = NULL;
	mutex.lock();
	Clear();
	mutex.unlock();
}

void CSessionsInfoMan::Clear()
{
	
	totalDeltaTime = 0;	
	for( int i = 0;i < 10; i++)
	{
		for( int j = 1; j < 255; j++)
		{
			sessionInfo[i][j].totalTime = 0;
			sessionInfo[i][j].count = 0;
		}
	}
}

bool CSessionsInfoMan::Log()
{
	if( pContext != NULL && pContext->mpLogWriter)
	{
		mutex.lock();
		if( totalDeltaTime < 500)
		{
			Clear();
			mutex.unlock();
			return false;
		}		
		bool bFinded = false;
		for( int i = 0;i < countSession; i++)
		{
			bFinded = false;
			needWriteLog[i] = 0;
			for( int j = 1; !bFinded && j < countType; j++)
			{								
				if( sessionInfo[i][j].count > 0 && sessionInfo[i][j].totalTime > 0)
				{
					needWriteLog[i] = 1;
					bFinded = true;
					break;
				}
			}
		}
		CComString sLog;
		sLog.Format("[Slow handlers] totalDeltaTime==%d", totalDeltaTime);
		for( int i = 0;i < countSession; i++)
		{
			if( needWriteLog[i] == 0)
				continue;
			CComString sLog2;
			sLog += "\n\t\t  " + sessionName[i] + ":";
			for( int j = 1;j < countType; j++)
			{								
				if( sessionInfo[i][j].count > 0 && sessionInfo[i][j].totalTime > 0)
				{
					sLog2.Format("\n\t\t    mesID=%d,  count=%d,  time=%d ms", j, sessionInfo[i][j].count, sessionInfo[i][j].totalTime);
					sLog += sLog2;
				}
			}
		}
		Clear();
		mutex.unlock();
		pContext->mpLogWriter->WriteLnLPCSTR( sLog.GetBuffer());
	}
	return true;
}

void CSessionsInfoMan::AddSessionInfo( int aiSessionInfoIndex, int aType, int aDeltaTime)
{
	mutex.lock();
	sessionInfo[ aiSessionInfoIndex][ aType].totalTime += aDeltaTime;
	sessionInfo[ aiSessionInfoIndex][ aType].count++;
	totalDeltaTime += aDeltaTime;
	mutex.unlock();
	if( aDeltaTime > 100 && pContext != NULL && pContext->mpLogWriter)
	{
		CComString sLog;
		sLog.Format("[Slow handlers] server=%s,  mesID=%d,  time=%d ms", sessionName[ aiSessionInfoIndex].GetBuffer(), aType, aDeltaTime);
		pContext->mpLogWriter->WriteLnLPCSTR( sLog.GetBuffer());
	}	
}