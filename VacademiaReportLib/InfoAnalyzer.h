#pragma once

class CDataBuffer;

class CInfoAnalyzer
{
public:
	CInfoAnalyzer();
	virtual ~CInfoAnalyzer();

protected:
	BOOL IsValid();
	CComString GetParam( TCHAR *pattern, TCHAR *endPattern, BOOL bEncoding = TRUE);

public:
	CComString GetAppName();
	CComString GetModuleName();	
	CComString GetModuleParam( TCHAR *moduleName, TCHAR *pattern, TCHAR *endPattern);
	CComString GetProjectPath();
	CComString GetUserNameInAcademia();
	CComString GetUserName();
	CComString GetComputerName();
	CComString GetSessionID();
	CComString GetUUID();
	CComString GetIPAddress();
	CComString GetAppVersion();
	CComString GetServerName();
	CComString GetLanguage();
	CComString GetErrorType();

public:
	BOOL LoadFromFile( WCHAR *sFileName);
	BOOL LoadFromData( char *sBuffer, int length);
	CComString GetStringData();
	void DisableEncoding();
	void EnableEncoding();
	BOOL GetData( CDataBuffer *buffer);

private:
	CComString m_sBuffer;
	BOOL m_bEncoding;
};
