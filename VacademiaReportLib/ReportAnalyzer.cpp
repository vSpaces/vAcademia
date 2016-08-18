#include "StdAfx.h"
#include "ReportAnalyzer.h"
#include <io.h>
#include <assert.h>
#include "func.h"

CReportAnalyzer::CReportAnalyzer(void)
{
	m_sServerName = SERVER_NAME;
	m_sCrashType = APP_ERROR;
	m_sAppVersion = "vacademia_0.0.0.0.beta";
	m_sLanguage = "eng";
	m_bDumpSaved = FALSE;
	m_bDumpSended = FALSE;
}

CReportAnalyzer::~CReportAnalyzer(void)
{
}

BOOL CReportAnalyzer::GetValueByKey( TCHAR *sKey, TCHAR *sEnd, CComString *sValue, int &ipos)
{
	int pos = m_sBuffer.Find( sKey, ipos);
	if ( pos == -1)	
		return FALSE;	
	int len = strlen( sKey);
	int end_pos = m_sBuffer.Find( sEnd, pos + len);
	if ( end_pos == -1)
		return FALSE;
	CComString sCValue = m_sBuffer.Right( m_sBuffer.GetLength() - pos - len);
	if ( sCValue.GetLength() < 1 && sCValue.GetLength() > 8096)
		return FALSE;
	sCValue = sCValue.Left( end_pos - pos - len);
	sCValue.TrimLeft();
	sCValue.TrimRight();
	*sValue = sCValue;
	ipos = pos;
	return TRUE;
}

BOOL CReportAnalyzer::ParseDumpReport()
{
	int ipos = 0;
	if ( !GetValueByKey( "szCrashType=\"", "\"", &m_sCrashType, ipos))
	{
		CComString fileDumpName;
		m_bDumpSaved = GetValueByKey( "Saved dump file to '", "'", &fileDumpName, ipos);	
		if ( !m_bDumpSaved)
		{
			BOOL b = GetValueByKey( "Failed to save dump file to '", "'", &fileDumpName, ipos);	
			if ( !b)
			{
				BOOL b = GetValueByKey( "Failed to create dump file '", "'", &fileDumpName, ipos);	
				if ( !b)
					return FALSE;
			}
		}

		TCHAR *fileName = GetFilePart( fileDumpName.GetBuffer());
		if ( stricmp( fileName, "player.dmp") == 0)
		{
			m_sCrashType = APP_ERROR;
			return TRUE;
		}
		if ( stricmp( fileName, "player_hang.dmp") == 0)
		{
			m_sCrashType = APP_HANG;
			return TRUE;
		}
		if ( stricmp( fileName, "player_assert.dmp") == 0)
		{
			m_sCrashType = ASSERT_ERROR;
			return TRUE;
		}
		if ( stricmp( fileName, "vacademia.dmp") == 0)
		{
			m_sCrashType = APP_LAUNCHER_ERROR;
			return TRUE;
		}
		
		return FALSE;
	}
	ipos = 0;
	if ( !GetValueByKey( "AppVersion=\"", "\"", &m_sAppVersion, ipos))
	{
		return FALSE;
	}
	ipos = 0;
	if ( !GetValueByKey( "Language=\"", "\"", &m_sLanguage, ipos))
	{
		return FALSE;
	}
	ipos = 0;
	GetValueByKey( "ServerName=\"", "\"", &m_sServerName, ipos);

	CComString fileDumpName;
	m_bDumpSaved = GetValueByKey( "Saved dump file to '", "'", &fileDumpName, ipos);	

	CComString sCommand;
	m_bDumpSended = GetValueByKey( "szCommandLine = ", "VacademiaReport is started!", &sCommand, ipos);
	return m_bDumpSaved && m_bDumpSended;
}

BOOL CReportAnalyzer::ParseSendingReport(  CComString *sCrashType, CComString *sAppVersion, CComString *sLanguage, CComString *sServerName)
{
	int ipos = 0;
	if ( !GetValueByKey( "szCrashType = ", "\n", sCrashType, ipos))
	{
		return FALSE;
	}

	if ( !GetValueByKey( "AppVersion = ", "\n", sAppVersion, ipos))
	{
		return FALSE;
	}

	if ( !GetValueByKey( "Language = ", "\n", sLanguage, ipos))
	{
		return FALSE;
	}

	GetValueByKey( "server = ", "\n", sServerName, ipos);
	return TRUE;
}

int CReportAnalyzer::ParseSendingReport( BOOL bHaveInfo)
{	
	if ( !bHaveInfo)
		return ParseSendingReport( &m_sCrashType, &m_sAppVersion, &m_sLanguage, &m_sServerName) ? DUMP_NOT_SENDED_AND_HAVE_INFO : DUMP_NOT_SENDED_AND_NOT_HAVE_INFO;

	CComString sCrashType;
	CComString sAppVersion;
	CComString sLanguage;
	CComString sServerName;
	BOOL b = ParseSendingReport( &sCrashType, &sAppVersion, &sLanguage, &sServerName);
	if ( !b)
	{
		return DUMP_NOT_SENDED_AND_HAVE_INFO;
	}

	if ( sCrashType.CompareNoCase( APP_PREV_CRASH) != 0 && sCrashType.CompareNoCase( m_sCrashType) == 0)
		return DUMP_NOT_SENDED_AND_HAVE_INFO;

	return DUMP_NOT_SENDED_AND_NOT_HAVE_INFO;
}


//////////////////////////////////////////////////////////////////////////

void CReportAnalyzer::clear()
{
	m_sBuffer = "";
}

BOOL CReportAnalyzer::LoadFromFile( WCHAR *sFileName)
{
	FILE *fp = NULL;
	fp = _wfopen( sFileName, L"r");
	if ( fp == NULL)
		return FALSE;
	int size = _filelength( fp->_file);	
	char *sBuffer = new char[ size];
	int readed = fread( sBuffer, 1, size, fp);	
	BOOL bSucc = LoadFromData( sBuffer, readed);
	delete[] sBuffer;
	fclose( fp);
	return bSucc;
}

BOOL CReportAnalyzer::LoadFromData( char *sBuffer, int length)
{
	m_sBuffer = CComString( sBuffer);	
	return TRUE;
}

int CReportAnalyzer::CheckOnNewCrash()
{
	WCHAR fileName[ MAX_PATH_4];
	getFileNameByProjName( L"mdump", L"txt", fileName);

	BOOL bLogExist = FALSE;
	bLogExist = LoadFromFile( fileName);
	int res_status = DUMP_NOT_SENDED_AND_NOT_HAVE_INFO;

	if ( bLogExist)
	{
		// файл mdump.txt существует
		if ( m_sBuffer.GetLength() > 0)
		{
			res_status = ParseDumpReport() ? DUMP_NOT_SENDED_AND_HAVE_INFO : DUMP_NOT_SENDED_AND_NOT_HAVE_INFO;
		}		
	}	

	if ( res_status == DUMP_NOT_SENDED_AND_NOT_HAVE_INFO)
		return res_status;

	WCHAR *pzFilePart = GetFilePart( fileName);
	rtl_wcscpy( pzFilePart, MAX_PATH_4 - wcslen(fileName) - 1, L"CrashReport.txt.inf");
	
	if ( !LoadFromFile( fileName) && !bLogExist)	
	{
		// файл mdump.txt не существует и файл CrashReport.txt не существует
		return NOT_DUMP;	
	}

	if ( m_sBuffer.GetLength() == 0)
		return res_status;

	BOOL bHaveInfo = res_status == DUMP_NOT_SENDED_AND_HAVE_INFO;
	
	int ipos = 0;
	if ( !GetValueByKey( "dwStatusCode = ", "\n", &m_sCode, ipos))
	{
		return ParseSendingReport( bHaveInfo);
	}
	
	int iCode = rtl_atoi( m_sCode.GetBuffer());
	if ( iCode != 200)
	{
		return ParseSendingReport( bHaveInfo);
	}

	//int ipos = m_sBuffer.Find( "dwStatusCode = ");
	ipos += strlen(  "dwStatusCode = ");
	if ( !GetValueByKey( "dwStatusCode = ", "\n", &m_sCode, ipos))
	{
		return ParseSendingReport( bHaveInfo);
	}

	iCode = rtl_atoi( m_sCode.GetBuffer());
	if ( iCode != 200)
	{
		return ParseSendingReport( bHaveInfo);
	}

	ipos += strlen(  "dwStatusCode = ");
	while( GetValueByKey( "dwStatusCode = ", "\n", &m_sCode, ipos))
	{
		iCode = rtl_atoi( m_sCode.GetBuffer());
		if ( iCode != 200)
		{
			return ParseSendingReport( bHaveInfo);
		}
		ipos += strlen(  "dwStatusCode = ");
	}
	
	return DUMP_SENDED;
}

TCHAR *CReportAnalyzer::GetCrashType()
{
	return m_sCrashType.GetBuffer();
}

TCHAR *CReportAnalyzer::GetAppVersion()
{
	return m_sAppVersion.GetBuffer();
}

TCHAR *CReportAnalyzer::GetLanguage()
{
	return m_sLanguage.GetBuffer();
}

TCHAR *CReportAnalyzer::GetServerName()
{
	return m_sServerName.GetBuffer();
}