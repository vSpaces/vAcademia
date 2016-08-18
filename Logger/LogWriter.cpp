
#include "StdAfx.h"
#include "LogWriter.h"
//#include "PlatformDependent.h"
#include <time.h>

CLogWriter::CLogWriter():
	m_file(NULL),
	m_isWorking(true),
	m_appName("NonameApp")
{
	InitializeCriticalSection(&m_cs);

	/*struct tm   *tmTime;
	time_t      szClock;
	time( &szClock);
	tmTime = localtime( &szClock);
	char sNow[MAX_INT_LENGTH * 8];
	sNow[ 0] = 0;
	sprintf( sNow, "%4d-%02d-%02d %02d.%02d.%02d ",
		1900+tmTime->tm_year, tmTime->tm_mon+1, tmTime->tm_mday,
		tmTime->tm_hour, tmTime->tm_min, tmTime->tm_sec);
	m_logName = sNow;
	m_logName += MAIN_LOG_FILENAME;*/
}

void CLogWriter::SetMode(bool isWorking)
{
	m_isWorking = isWorking;
}

void CLogWriter::SetApplicationName(std::string appName)
{
	m_appName = appName;
}

void CLogWriter::WriteLPCSTR(const char * str)
{
	if (!m_isWorking)
	{
		return;
	}

	EnterCriticalSection(&m_cs);

	Send(str);

	LeaveCriticalSection(&m_cs);
}


void CLogWriter::WriteLnLPCSTR(const char * str)
{
	if (!m_isWorking)
	{
		return;
	}

	EnterCriticalSection(&m_cs);

	send();

	LeaveCriticalSection(&m_cs);
}

void CLogWriter::PrintLPCSTR(const char * str)
{
	WriteLPCSTR(str);
}

void CLogWriter::PrintLnLPCSTR(const char * str)
{
	WriteLnLPCSTR(str);
}

void CLogWriter::WriteLn(CLogValue obj)
{
	if (!m_isWorking)
	{
		return;
	}

	WriteLnLPCSTR(obj.GetData());
}

void CLogWriter::WriteLn(CLogValue obj1, CLogValue obj2)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString();
	WriteLnLPCSTR(str.c_str());
}

void CLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString();
	WriteLnLPCSTR(str.c_str());
}

void CLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString();
	WriteLnLPCSTR(str.c_str());
}

void CLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString();
	WriteLnLPCSTR(str.c_str());
}

void CLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString();
	WriteLnLPCSTR(str.c_str());
}

void CLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString();
	WriteLnLPCSTR(str.c_str());
}

void CLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString();
	WriteLnLPCSTR(str.c_str());
}

void CLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString() + obj9.GetString();
	WriteLnLPCSTR(str.c_str());
}

void CLogWriter::WriteLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString() + obj9.GetString() + obj10.GetString();
	WriteLnLPCSTR(str.c_str());
}

void CLogWriter::Write(CLogValue obj)
{
	if (!m_isWorking)
	{
		return;
	}

	WriteLPCSTR(obj.GetData());
}

void CLogWriter::Write(CLogValue obj1, CLogValue obj2)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString();
	WriteLPCSTR(str.c_str());
}

void CLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString();
	WriteLPCSTR(str.c_str());
}

void CLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString();
	WriteLPCSTR(str.c_str());
}

void CLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString();
	WriteLPCSTR(str.c_str());
}

void CLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString();
	WriteLPCSTR(str.c_str());
}

void CLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString();
	WriteLPCSTR(str.c_str());
}

void CLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString();
	WriteLPCSTR(str.c_str());
}

void CLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString() + obj9.GetString();
	WriteLPCSTR(str.c_str());
}

void CLogWriter::Write(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString() + obj9.GetString() + obj10.GetString();
	WriteLPCSTR(str.c_str());
}

void CLogWriter::PrintLn(CLogValue obj)
{
	if (!m_isWorking)
	{
		return;
	}

	PrintLnLPCSTR(obj.GetData());
}

void CLogWriter::PrintLn(CLogValue obj1, CLogValue obj2)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString();
	PrintLnLPCSTR(str.c_str());
}

void CLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString();
	PrintLnLPCSTR(str.c_str());
}

void CLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString();
	PrintLnLPCSTR(str.c_str());
}

void CLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString();
	PrintLnLPCSTR(str.c_str());
}

void CLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString();
	PrintLnLPCSTR(str.c_str());
}

void CLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString();
	PrintLnLPCSTR(str.c_str());
}

void CLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString();
	PrintLnLPCSTR(str.c_str());
}

void CLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString() + obj9.GetString();
	PrintLnLPCSTR(str.c_str());
}

void CLogWriter::PrintLn(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString() + obj9.GetString() + obj10.GetString();
	PrintLnLPCSTR(str.c_str());
}

void CLogWriter::Print(CLogValue obj)
{
	if (!m_isWorking)
	{
		return;
	}

	PrintLPCSTR(obj.GetData());
}

void CLogWriter::Print(CLogValue obj1, CLogValue obj2)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString();
	PrintLPCSTR(str.c_str());
}

void CLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString();
	PrintLPCSTR(str.c_str());
}

void CLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString();
	PrintLPCSTR(str.c_str());
}

void CLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString();
	PrintLPCSTR(str.c_str());
}

void CLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString();
	PrintLPCSTR(str.c_str());
}

void CLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString();
	PrintLPCSTR(str.c_str());
}

void CLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString();
	PrintLPCSTR(str.c_str());
}

void CLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString() + obj9.GetString();
	PrintLPCSTR(str.c_str());
}

void CLogWriter::Print(CLogValue obj1, CLogValue obj2, CLogValue obj3, CLogValue obj4, CLogValue obj5, CLogValue obj6, CLogValue obj7, CLogValue obj8, CLogValue obj9, CLogValue obj10)
{
	if (!m_isWorking)
	{
		return;
	}

	std::string str = obj1.GetString() + obj2.GetString() + obj3.GetString() + obj4.GetString() + obj5.GetString() + obj6.GetString() + obj7.GetString() + obj8.GetString() + obj9.GetString() + obj10.GetString();
	PrintLPCSTR(str.c_str());
}

CLogWriter::~CLogWriter()
{
	fclose(m_file);
	DeleteCriticalSection( &m_cs);
}
