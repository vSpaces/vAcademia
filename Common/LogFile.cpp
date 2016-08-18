// LogFile.cpp: implementation of the CLogFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool makeDirectory( LPCSTR aDirPath)
{
	bool bRes = true;
	LPSTR sDir = new char[ strlen( aDirPath) + 1];
	strcpy( sDir, aDirPath);
	LPSTR pEnd = sDir + strlen( sDir);
	LPSTR pNullMark = sDir + 1;
	while ((pNullMark < pEnd) && bRes)
	{
		while ((*pNullMark != '\\') || (*(pNullMark-1) == ':'))
			pNullMark++;
		*pNullMark = 0;
		CreateDirectory( sDir, NULL);
		*pNullMark = '\\';
		pNullMark++;
	}
	delete[] sDir;
	return bRes;
}

CLogFile::CLogFile()
{
	InitializeCriticalSection( &cs);
}

CLogFile::~CLogFile()
{
	DeleteCriticalSection( &cs);
}

BOOL _BuildStorage(TCHAR* localBase, TCHAR*	dirTree)
{
	TCHAR directory[MAX_PATH];
	memset(&directory, 0, MAX_PATH);
	GetCurrentDirectory(MAX_PATH, directory);

	TCHAR temp[MAX_PATH];
	memset(&temp, 0, MAX_PATH);
	_tcscat(temp, dirTree);

	if(SetCurrentDirectory(localBase) == FALSE)
		return FALSE;

	LPTSTR start;
	LPTSTR point;
	for(start = point = temp; *point; point++)
	{
		if(*point == _T('\\'))
		{
			*point = _T('\0'); 
			CreateDirectory(start, NULL);
			SetCurrentDirectory(start);
			start = point + 1;
		}
	}

	SetCurrentDirectory(directory);
	return TRUE;
}

void CLogFile::writeLogV(LPCSTR aMessage, va_list vl)
{
	EnterCriticalSection( &cs);
	if (!sLogFile.IsEmpty())
	{
		CComString sLogMessage;
		sLogMessage.FormatV( aMessage, vl);

		if (GetFileAttributes( sLogFile) == 0xFFFFFFFF)
		{
			CComString sDirPath( sLogFile.GetString(), sLogFile.ReverseFind( '\\') + 1);
			makeDirectory( sDirPath);
		}
		
		HANDLE hLog = CreateFile( sLogFile, GENERIC_WRITE, FILE_SHARE_READ, 
			NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hLog != INVALID_HANDLE_VALUE)
		{
			SetFilePointer( hLog, 0, 0, FILE_END);
			DWORD dwWriten;
			WriteFile( hLog, sLogMessage.GetString(), sLogMessage.GetLength(), &dwWriten, NULL);
			CloseHandle( hLog);
		}
	}
	LeaveCriticalSection( &cs);
}

void CLogFile::writeLog(LPCSTR aMessage, ...)
{
	va_list vl;
	va_start(vl, aMessage);
	writeLogV( aMessage, vl);
	va_end(vl);
}

void CLogFile::writeLogTimeV(LPCSTR aMessage, va_list vl)
{
	SYSTEMTIME tm;
	char sTime[ 26];
	GetLocalTime( &tm);
	GetTimeFormat( LOCALE_SYSTEM_DEFAULT, TIME_FORCE24HOURFORMAT, &tm, NULL, sTime, 25);
	CComString sLogMessage( sTime);
	sLogMessage += " ";
	sLogMessage += aMessage;

	writeLogV( sLogMessage, vl);
}

void CLogFile::writeLogTime(LPCSTR aMessage, ...)
{
	va_list vl;
	va_start(vl, aMessage);
	writeLogTimeV( aMessage, vl);
	va_end(vl);
}
