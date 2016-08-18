#pragma once

class CComString;

class CReportAnalyzer
{
public:
	CReportAnalyzer();
	virtual ~CReportAnalyzer();

protected:
	BOOL GetValueByKey( TCHAR *sKey, TCHAR *sEnd, CComString *sValue, int &ipos);	
	BOOL ParseDumpReport();
	BOOL ParseSendingReport( CComString *sCrashType, CComString *sAppVersion, CComString *sLanguage, CComString *sServerName);
	int ParseSendingReport( BOOL bHaveInfo);

public:
	enum{ NOT_DUMP, DUMP_SENDED, DUMP_NOT_SENDED_AND_HAVE_INFO, DUMP_NOT_SENDED_AND_NOT_HAVE_INFO};
	void clear();
	BOOL LoadFromFile( WCHAR *sFileName);
	BOOL LoadFromData( char *sBuffer, int length);
	int CheckOnNewCrash();
	TCHAR *GetCrashType();
	TCHAR *GetAppVersion();
	TCHAR *GetLanguage();
	TCHAR *GetServerName();

private:
	CComString m_sBuffer;
	CComString m_sCode;
	CComString m_sCrashType;
	CComString m_sServerName;
	CComString m_sAppVersion;
	CComString m_sLanguage;
	BOOL m_bDumpSaved;
	BOOL m_bDumpSended;
};
