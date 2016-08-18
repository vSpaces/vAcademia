// LogFile.h: interface for the CLogFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGFILE_H__41CD24FA_AA42_4991_989D_DAE421AA46A1__INCLUDED_)
#define AFX_LOGFILE_H__41CD24FA_AA42_4991_989D_DAE421AA46A1__INCLUDED_

#include "ComString.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLogFile  
{
public:
	CLogFile();
	virtual ~CLogFile();
	void writeLogV( LPCSTR aMessage, va_list vl);
	void writeLog( LPCSTR aMessage, ...);
	void writeLogTimeV( LPCSTR aMessage, va_list vl);
	void writeLogTime( LPCSTR aMessage, ...);
	inline void setLogFile( LPCSTR aLogFile) {
		EnterCriticalSection( &cs);
		sLogFile = aLogFile;
		HANDLE hLog = CreateFile( sLogFile, GENERIC_WRITE, FILE_SHARE_READ, 
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hLog != INVALID_HANDLE_VALUE)
		{
			CloseHandle( hLog);
		}
		LeaveCriticalSection( &cs);
	}
	inline LPCSTR getLogFile() { return sLogFile.GetString();}

protected:
	CRITICAL_SECTION cs;
	CComString sLogFile;
};

#endif // !defined(AFX_LOGFILE_H__41CD24FA_AA42_4991_989D_DAE421AA46A1__INCLUDED_)
