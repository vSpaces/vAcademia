#include "StdAfx.h"
#include "SimpleThreadStarter.h"
#include "LogWriterLastMsgAsync.h"
#include <time.h>

void CLogMessage::SetMessage( const char *apText, bool bTime, bool abNewStr)
{
	text = apText;
	if( bTime)
		_strtime_s(&time[0], 4 * MAX_INT_LENGTH);
	else
		time[0] ='\0';
	bNewStr = abNewStr;
}

CLogWriterLastMsgAsync::CLogWriterLastMsgAsync()	
{
	m_file = NULL;
	m_isWorking = true;
	m_appName = "ScriptLog";
	mpInfoMan = NULL;
	mpLoggerSender = NULL;
	bThread = true;
	InitializeCriticalSection(&m_cs);

	struct tm   tmTime;
	time_t      szClock;
	time( &szClock);
	localtime_s(&tmTime, &szClock);
	char sNow[MAX_INT_LENGTH * 8];
	sNow[ 0] = 0;
	sprintf_s( sNow, MAX_INT_LENGTH * 8, "%4d-%02d-%02d %02d.%02d.%02d ",
		1900+tmTime.tm_year, tmTime.tm_mon+1, tmTime.tm_mday,
		tmTime.tm_hour, tmTime.tm_min, tmTime.tm_sec);
	m_logName = sNow;
	m_logName += MAIN_LOG_SC_FILENAME;
	hEvent = CreateEvent( NULL, TRUE, FALSE, NULL);

	CSimpleThreadStarter logThreadStarter(__FUNCTION__);	
	logThreadStarter.SetRoutine(writeThread_);
	logThreadStarter.SetParameter(this);
	hThread = logThreadStarter.Start();
}

CLogWriterLastMsgAsync::~CLogWriterLastMsgAsync()
{
	bThread = false;
	DWORD dwExitCode = 0;	

	EnterCriticalSection(&m_cs);
	SetEvent( hEvent);
	LeaveCriticalSection(&m_cs);

	WaitForSingleObject( hThread, 2000);
	::CloseHandle( hEvent);
	hEvent = NULL;
	mpLoggerSender = NULL;

	EnterCriticalSection(&m_cs);
	if( m_file != NULL)
		fclose(m_file);
	LeaveCriticalSection(&m_cs);

	DeleteCriticalSection(&m_cs);
}

void CLogWriterLastMsgAsync::WriteLPCSTR(const char * str, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	EnterCriticalSection(&m_cs);

	message.SetMessage( str, true, false);
	
	LeaveCriticalSection(&m_cs);
}


void CLogWriterLastMsgAsync::WriteLnLPCSTR(const char * str, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	EnterCriticalSection(&m_cs);

	message.SetMessage( str, true, true);

	LeaveCriticalSection(&m_cs);
}

void CLogWriterLastMsgAsync::PrintLPCSTR(const char * str, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	EnterCriticalSection(&m_cs);

	message.SetMessage( str, false, false);

	LeaveCriticalSection(&m_cs);
}

void CLogWriterLastMsgAsync::PrintLnLPCSTR(const char * str, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	EnterCriticalSection(&m_cs);

	message.SetMessage( str, false, true);

	LeaveCriticalSection(&m_cs);
}

DWORD WINAPI CLogWriterLastMsgAsync::writeThread_(LPVOID param)
{
	CLogWriterLastMsgAsync* logWriter = (CLogWriterLastMsgAsync*)param;
	return logWriter->writeThread();
}

int CLogWriterLastMsgAsync::writeThread()
{
	while( true)
	{
		DWORD dwRes = WaitForSingleObject( hEvent, 10000);
		if( dwRes != WAIT_TIMEOUT)
			break;

		EnterCriticalSection(&m_cs);
		
		if (message.IsEmpty())
		{
			LeaveCriticalSection(&m_cs);
			continue;
		}		

		if (!m_file)
		{
			std::wstring path = GetLogFilePath();			
			m_file = _wfsopen(path.c_str(), L"wb", _SH_DENYWR);
		}

		if( m_file != 0)
		{
			if( message.time[0] != '\0')
				fprintf(m_file, "%s\t", message.time);

			fprintf(m_file, message.text.c_str());

			if(message.bNewStr)
				fprintf(m_file, "\n");

			fflush(m_file);
		}

		LeaveCriticalSection(&m_cs);
	}

	return 0;
}
