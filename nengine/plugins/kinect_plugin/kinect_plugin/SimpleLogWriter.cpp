
#include "StdAfx.h"
#include "SimpleLogWriter.h"
#include <windows.h>
#include <winbase.h>
#include <winuser.h>
#include "shlobj.h"
#include <time.h>
#include <io.h> 

std::string GetApplicationDataDirectory()
{
	CHAR tmp[MAX_PATH];
	SHGetSpecialFolderPathA(NULL, tmp, CSIDL_APPDATA, true);
	return tmp;
}

CSimpleLogWriter::CSimpleLogWriter()	
{
	m_logPath = GetApplicationDataDirectory();

	struct tm   *tmTime;
	time_t      szClock;
	time( &szClock);
	tmTime = localtime( &szClock);
	char sNow[30 * 8];
	sNow[ 0] = 0;
	sprintf( sNow, "%4d-%02d-%02d %02d.%02d.%02d",
		1900+tmTime->tm_year, tmTime->tm_mon+1, tmTime->tm_mday,
		tmTime->tm_hour, tmTime->tm_min, tmTime->tm_sec);
	m_logPath += "\\Vacademia";

	::CreateDirectoryA(m_logPath.c_str(), NULL);

	m_logPath += "\\Kinect";

	::CreateDirectoryA(m_logPath.c_str(), NULL);

	m_logPath += "\\";
	m_logPath += sNow;
	m_logPath += ".log";
}

void CSimpleLogWriter::Write(std::string s)
{
	FILE* fl = fopen(m_logPath.c_str(), "a+");
	if (fl)
	{
		fprintf(fl, "%s\n", s.c_str());
		fclose(fl);
	}
}

void CSimpleLogWriter::Write(std::string s, int a)
{
	FILE* fl = fopen(m_logPath.c_str(), "a+");
	if (fl)
	{
		fprintf(fl, "%s %d\n", s.c_str(), a);
		fclose(fl);
	}
}

CSimpleLogWriter::~CSimpleLogWriter()
{
}