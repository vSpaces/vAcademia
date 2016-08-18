
#include "StdAfx.h"
#include "LogSender.h"
//#include "PlatformDependent.h"
#include <time.h>
#include <sys/timeb.h>
#include "ClientLogOut.h"

CLogSender::CLogSender(oms::omsContext* aContext):
	m_isWorking(true),
	pClientSession(NULL),
	MP_STRING_INIT(m_logName),
	MP_STRING_INIT(sMessages)

{
	InitializeCriticalSection(&m_cs);
	if (aContext != NULL)
		aContext->mpLogger = this;
}

void CLogSender::SetMode(bool isWorking)
{
	m_isWorking = isWorking;
	if(!m_isWorking)
		sMessages = "";
}

/*void CLogSender::SetApplicationName(std::string appName)
{
	m_appName = appName;
}*/
void CLogSender::SetClientSession(cs::iclientsession *apClientSession)
{
	if( apClientSession == NULL)
	{
		pClientSession = apClientSession;
		return;
	}
	apClientSession->SetMaxQueueSize( MAX_SESSION_QUEUE_SIZE);
	while ( sMessages.size()>0)
	{
		EnterCriticalSection(&m_cs);
		while(sMessages.size()>0)
		{
			if( sMessages.size() > MAX_STRING_IN_PACKET_SIZE)
			{
				std::string sMessage = sMessages.substr(0, MAX_STRING_IN_PACKET_SIZE);
				sMessages.erase(0, MAX_STRING_IN_PACKET_SIZE);
				CClientLogOut queryOut(sMessage.c_str(), true);
				apClientSession->Send(ST_ClientLog, queryOut.GetData(), queryOut.GetDataSize());
			}
			else
			{
				CClientLogOut queryOut(sMessages.c_str(), true);
				apClientSession->Send(ST_ClientLog, queryOut.GetData(), queryOut.GetDataSize());
				sMessages = "";
			}
		}
		LeaveCriticalSection(&m_cs);
		Sleep(1);
	}	
	pClientSession = apClientSession;
}

void CLogSender::WriteLPCSTR(const char * str)
{
	if (!m_isWorking)
	{
		return;
	}

	char time[4 * MAX_INT_LENGTH];
	_strtime_s(&time[0], 4 * MAX_INT_LENGTH);
	WriteLPCSTR(&time[0], str, NULL);
}

void CLogSender::WriteLPCSTR(const char* time, const char * str)
{
	WriteLPCSTR(&time[0], str, NULL);
}

void CLogSender::WriteLnLPCSTR(const char *str)
{
	if (!m_isWorking)
	{
		return;
	}

	char time[4 * MAX_INT_LENGTH];
	_strtime_s(&time[0], 4 * MAX_INT_LENGTH);
	WriteLPCSTR(&time[0], str, "\n");
}

void CLogSender::WriteLnLPCSTR(const char* time, const char * str)
{
	WriteLPCSTR(&time[0], str, "\n");
}

void CLogSender::WriteLPCSTR(const char* time, const char * str, const char *delimiter)
{
	if (!m_isWorking)
	{
		return;
	}
	
	std::string sLog = time;
	sLog += " - ";
	sLog += str;
	if(delimiter != NULL)
		sLog += delimiter;
	
	if(pClientSession!=NULL)
	{
		CClientLogOut queryOut(sLog.c_str(), true);
		pClientSession->Send(ST_ClientLog, queryOut.GetData(), queryOut.GetDataSize());
	}
	else
	{
		EnterCriticalSection(&m_cs);
		sMessages += sLog;
		LeaveCriticalSection(&m_cs);
	}	
}

void CLogSender::PrintLPCSTR(const char * str)
{
	PrintLPCSTR(str, NULL);
}

void CLogSender::PrintLnLPCSTR(const char * str)
{
	PrintLPCSTR(str, "\n");
}

void CLogSender::PrintLPCSTR(const char * str, const char *delimiter)
{
	if (!m_isWorking)
	{
		return;
	}
	
	std::string sLog = str;
	if(delimiter != NULL)
		sLog += delimiter;

	if(pClientSession!=NULL)
	{
		CClientLogOut queryOut(sLog.c_str(), false);
		pClientSession->Send(ST_ClientLog, queryOut.GetData(), queryOut.GetDataSize());
	}
	else
	{
		EnterCriticalSection(&m_cs);
		sMessages += sLog;
		LeaveCriticalSection(&m_cs);
	}
}

CLogSender::~CLogSender()
{
	pClientSession = NULL;
	DeleteCriticalSection( &m_cs);
}
