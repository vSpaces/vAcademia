#include "StdAfx.h"
#include "LogWriter.h"
#include <time.h>
 #include <io.h> 

CLogWriter::CLogWriter():
	m_file(NULL),
	m_isWorking(true),
	m_appName("Log"),
	mpInfoMan(NULL),
	mpLoggerSender(NULL)
{
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
	m_logName += MAIN_LOG_FILENAME;
}

void CLogWriter::SetMode(bool isWorking)
{
	m_isWorking = isWorking;
}

void CLogWriter::SetApplicationName(std::string appName)
{
	m_appName = appName;
}

void CLogWriter::StartSendingToInfoMan(IExternalLogWriter* aInfoManager)
{
	mpInfoMan = aInfoManager;
}

void CLogWriter::StopSendingToInfoMan()
{
	mpInfoMan = NULL;
}

void CLogWriter::StartSendingToServer( lgs::ILoggerBase* aLoggerSender)
{
	mpLoggerSender = aLoggerSender;
}

void CLogWriter::StopSendingToServer()
{
	mpLoggerSender = NULL;
}

inline void CLogWriter::SendToInfoMan(const std::string& str, LOG_TYPE logType, const char * time)
{
	std::string sendString;
	if (time != NULL)
		sendString = time;
	sendString += "\t";
	sendString += str;
	sendString += "\n";
	mpInfoMan->Trace(sendString, logType);
}

void CLogWriter::StopWriteLog()
{
	EnterCriticalSection(&m_cs);
}

void CLogWriter::ContinueWriteLog()
{
	LeaveCriticalSection(&m_cs);
}

void CLogWriter::SetApplicationDataDirectory(LPCWSTR alpApplicationDataDirectory)
{
	if( alpApplicationDataDirectory)
		m_applicationDataDirectory = alpApplicationDataDirectory;
}

std::wstring CLogWriter::GetApplicationDataDirectory()
{
	return m_applicationDataDirectory;
}

std::wstring CLogWriter::GetLogFilePath()
{
	std::wstring path = GetApplicationDataDirectory();
	path += L"\\Vacademia";

	CreateDirectoryW(path.c_str(), NULL);

	path +=L"\\";
	USES_CONVERSION;
	path += A2W( m_appName.c_str());

	CreateDirectoryW(path.c_str(), NULL);

	path += L"\\";
	path += A2W( m_logName.c_str());
	return path;
}

bool CLogWriter::OpenFileIfNeeded()
{
	if (!m_file)
	{
		std::wstring path = GetLogFilePath();		

		m_file = _wfsopen(path.c_str(), L"wb", _SH_DENYWR);

		return (m_file != NULL);
	}
	
	return true;	
}

void CLogWriter::WriteLPCSTR(const char * str, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	EnterCriticalSection(&m_cs);

	if (!OpenFileIfNeeded())
	{
		LeaveCriticalSection(&m_cs);
		return;
	}
	
	char time[4 * MAX_INT_LENGTH];
	_strtime_s(&time[0], 4 * MAX_INT_LENGTH);

	if( m_file)
	{
		fprintf(m_file, "%s\t", time);

		fprintf(m_file, str);
		fflush(m_file);
	}

	if (mpInfoMan != NULL)
		SendToInfoMan(str, logType, &time[0]);

	if (mpLoggerSender != NULL)
		mpLoggerSender->WriteLPCSTR(time, str);

	LeaveCriticalSection(&m_cs);
}


void CLogWriter::WriteLnLPCSTR(const char * str, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	EnterCriticalSection(&m_cs);

	if (!OpenFileIfNeeded())
	{
		LeaveCriticalSection(&m_cs);
		return;
	}

//	fprintf(m_file, "%d\t", logType);

	char time[4 * MAX_INT_LENGTH];
	_strtime_s(&time[0], 4 * MAX_INT_LENGTH);
	fprintf(m_file, "%s\t", time);

	std::string s = "";
	if( strstr( str, "?") != NULL)
	{
		s = "ff";
	}

	fprintf(m_file, str);
	fprintf(m_file, "\n");
	fflush(m_file);

	if (mpInfoMan != NULL)
		SendToInfoMan(str, logType, &time[0]);

	if (mpLoggerSender != NULL)
		mpLoggerSender->WriteLnLPCSTR(time, str);

	LeaveCriticalSection(&m_cs);
}

void CLogWriter::PrintLPCSTR(const char * str, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	EnterCriticalSection(&m_cs);

	if (!OpenFileIfNeeded())
	{
		LeaveCriticalSection(&m_cs);
		return;
	}

	fprintf(m_file, str);
	fflush(m_file);

	if (mpInfoMan != NULL)
		SendToInfoMan(str, logType);

	if (mpLoggerSender != NULL)
		mpLoggerSender->PrintLPCSTR(str);

	LeaveCriticalSection(&m_cs);
}

void CLogWriter::PrintLnLPCSTR(const char * str, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	EnterCriticalSection(&m_cs);

	if (!OpenFileIfNeeded())
	{
		LeaveCriticalSection(&m_cs);
		return;
	}

	fprintf(m_file, str);
	fprintf(m_file, "\n");
	fflush(m_file);

	if (mpInfoMan != NULL)
		SendToInfoMan(str, logType);

	if (mpLoggerSender != NULL)
		mpLoggerSender->PrintLnLPCSTR(str);

	LeaveCriticalSection(&m_cs);
}

void CLogWriter::WriteLn(CLogValue obj, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	WriteLnLPCSTR(obj.GetData());
}

void CLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString();
	WriteLnLPCSTR(str.c_str());
}

void CLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString();
	WriteLnLPCSTR(str.c_str());
}

void CLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString();
	WriteLnLPCSTR(str.c_str());
}

void CLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString();
	WriteLnLPCSTR(str.c_str());
}

void CLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString();
	WriteLnLPCSTR(str.c_str());
}

void CLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString();
	WriteLnLPCSTR(str.c_str());
}

void CLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString();
	WriteLnLPCSTR(str.c_str());
}

void CLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString() + obj9.GetString();
	WriteLnLPCSTR(str.c_str());
}

void CLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString() + obj9.GetString() + obj10.GetString();
	WriteLnLPCSTR(str.c_str());
}

void CLogWriter::Write(CLogValue obj, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	WriteLPCSTR(obj.GetData());
}

void CLogWriter::Write(CLogValue obj1, CLogValue obj2, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString();
	WriteLPCSTR(str.c_str());
}

void CLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString();
	WriteLPCSTR(str.c_str());
}

void CLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString();
	WriteLPCSTR(str.c_str());
}

void CLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString();
	WriteLPCSTR(str.c_str());
}

void CLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString();
	WriteLPCSTR(str.c_str());
}

void CLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString();
	WriteLPCSTR(str.c_str());
}

void CLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString();
	WriteLPCSTR(str.c_str());
}

void CLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString() + obj9.GetString();
	WriteLPCSTR(str.c_str());
}

void CLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString() + obj9.GetString() + obj10.GetString();
	WriteLPCSTR(str.c_str());
}

void CLogWriter::PrintLn(CLogValue obj, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	PrintLnLPCSTR(obj.GetData());
}

void CLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString();
	PrintLnLPCSTR(str.c_str());
}

void CLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString();
	PrintLnLPCSTR(str.c_str());
}

void CLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString();
	PrintLnLPCSTR(str.c_str());
}

void CLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString();
	PrintLnLPCSTR(str.c_str());
}

void CLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString();
	PrintLnLPCSTR(str.c_str());
}

void CLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString();
	PrintLnLPCSTR(str.c_str());
}

void CLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString();
	PrintLnLPCSTR(str.c_str());
}

void CLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString() + obj9.GetString();
	PrintLnLPCSTR(str.c_str());
}

void CLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString() + obj9.GetString() + obj10.GetString();
	PrintLnLPCSTR(str.c_str());
}

void CLogWriter::Print(CLogValue obj, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	PrintLPCSTR(obj.GetData());
}

void CLogWriter::Print(CLogValue obj1, CLogValue obj2, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString();
	PrintLPCSTR(str.c_str());
}

void CLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString();
	PrintLPCSTR(str.c_str());
}

void CLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString();
	PrintLPCSTR(str.c_str());
}

void CLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString();
	PrintLPCSTR(str.c_str());
}

void CLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString();
	PrintLPCSTR(str.c_str());
}

void CLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString();
	PrintLPCSTR(str.c_str());
}

void CLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString();
	PrintLPCSTR(str.c_str());
}

void CLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString() + obj9.GetString();
	PrintLPCSTR(str.c_str());
}

void CLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10, LOG_TYPE logType)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString() + obj9.GetString() + obj10.GetString();
	PrintLPCSTR(str.c_str());
}

// печать буфера для дальнейшего его использования для отладки
void CLogWriter::WriteBufferAsArray(const unsigned char* buf, unsigned int auSize, LOG_TYPE logType)
{
	char foo[10];
	std::string str = "unsigned char buf[]={";
	for (unsigned int i=0; i<auSize; i++)
	{
		_itoa_s( buf[i], foo, 10, 10);
		str.append(foo);
		if(i<auSize-1)
			str.append(",");
	}
	str.append("}");
	WriteLn(str);
}

CLogWriter::~CLogWriter()
{
	mpLoggerSender = NULL;
	if( m_file)
		fclose(m_file);
	DeleteCriticalSection( &m_cs);
}
