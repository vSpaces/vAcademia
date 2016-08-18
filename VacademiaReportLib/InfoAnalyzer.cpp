#include "StdAfx.h"
#include "InfoAnalyzer.h"
#include <DataBuffer.h>
#include <io.h>
#include <assert.h>
#include <curl.h>

#define MAX_STRING_LEN 150

CInfoAnalyzer::CInfoAnalyzer()
{
	m_bEncoding = TRUE;
}

CInfoAnalyzer::~CInfoAnalyzer()
{
	
}

//////////////////////////////////////////////////////////////////////////

BOOL CInfoAnalyzer::IsValid()
{
	assert( m_sBuffer.GetLength() > 0);
	return m_sBuffer.GetLength() > 0;
}

CComString CInfoAnalyzer::GetParam( TCHAR *pattern, TCHAR *endPattern, BOOL bEncoding /* = TRUE */)
{
	if ( !IsValid())
		return "";

	int pos = m_sBuffer.Find( pattern);
	if ( pos == -1)
		return "";
	int pos2 = m_sBuffer.Find( endPattern, pos + strlen( pattern));
	if ( pos2 == -1)
		return "";
	CComString sBuffer1 = m_sBuffer.Right( m_sBuffer.GetLength() - pos - strlen( pattern));
	CComString sParam = sBuffer1.Left( pos2 - pos - strlen( pattern));	
	if ( m_bEncoding && bEncoding)
	{
		sParam = curl_escape( sParam.GetBuffer(), sParam.GetLength());
	}

	if ( sParam.GetLength() > MAX_STRING_LEN)
	{
		// если длина строки больше MAX_STRING_LEN, то укорачиваем ее
		sParam = sParam.Left( MAX_STRING_LEN);
	}

	return sParam;
}

//////////////////////////////////////////////////////////////////////////

CComString CInfoAnalyzer::GetAppName()
{
	if ( !IsValid())
		return "";

	int pos = m_sBuffer.Find( " ");
	if ( pos == -1)
		return "";
	CComString appName = m_sBuffer.Left( pos);
	return appName;
}

CComString CInfoAnalyzer::GetModuleName()
{
	if ( !IsValid())
		return "";
	return GetParam( "in module ", " at ");
}

CComString CInfoAnalyzer::GetModuleParam( TCHAR *moduleName, TCHAR *pattern, TCHAR *endPattern)
{
	if ( !IsValid())
		return "";
	return GetParam( pattern, endPattern);
}

CComString CInfoAnalyzer::GetProjectPath()
{
	if ( !IsValid())
		return "";
	CComString sPath = GetParam( "Process:  ", ";", FALSE);
	int ipos = sPath.ReverseFind( '\\');
	if ( ipos > -1)
	{
		sPath = sPath.Left( ipos);
	}
	if ( m_bEncoding)
		sPath = curl_escape( sPath.GetBuffer(), sPath.GetLength());
	return sPath;
}

CComString CInfoAnalyzer::GetUserNameInAcademia()
{
	if ( !IsValid())
		return "";
	return GetParam( "UserNameInAcademia=\"", "\"");
}

CComString CInfoAnalyzer::GetUserName()
{
	if ( !IsValid())
		return "";
	return GetParam( ", run by \"", "\"");
}

CComString CInfoAnalyzer::GetComputerName()
{
	if ( !IsValid())
		return "";
	return GetParam( "on computer ", ".");
}

CComString CInfoAnalyzer::GetSessionID()
{
	if ( !IsValid())
		return "";
	return GetParam( "SessionID=\"", "\"");
}

CComString CInfoAnalyzer::GetUUID()
{
	if ( !IsValid())
		return "";
	return GetParam( "UUID=\"", "\"");
}

CComString CInfoAnalyzer::GetIPAddress()
{
	if ( !IsValid())
		return "";
	return GetParam( "IP address of computer ", ";");
}

CComString CInfoAnalyzer::GetAppVersion()
{
	if ( !IsValid())
		return "";
	CComString res = GetParam( "AppVersion=\"", "\"", FALSE);
	res.TrimLeft();
	res.TrimRight();
	if ( res.GetLength() == 0)
		"vacademia_0.0.0.0_beta";
	return res;
}

CComString CInfoAnalyzer::GetServerName()
{
	if ( !IsValid())
		return "";
	CComString res = GetParam( "ServerName=\"", "\"", FALSE);
	res.TrimLeft();
	res.TrimRight();
	if ( res.GetLength() == 0)
		SERVER_NAME;
	return res;
}

CComString CInfoAnalyzer::GetLanguage()
{
	if ( !IsValid())
		return "";
	CComString res = GetParam( "Language=\"", "\"", FALSE);
	res.TrimLeft();
	res.TrimRight();
	if ( res.GetLength() == 0)
		"eng";
	return res;
}

CComString CInfoAnalyzer::GetErrorType()
{
	if ( !IsValid())
		return "";
	CComString res = GetModuleParam( GetAppName().GetBuffer(), "caused an ", "(");
	return res;
}

//////////////////////////////////////////////////////////////////////////

BOOL CInfoAnalyzer::LoadFromFile( WCHAR *sFileName)
{
	FILE *fp = _wfopen( sFileName, L"r");
	if ( fp == NULL)
		return FALSE;
	int size = _filelength( fp->_file);
	char *sBuffer = new char[ size];
	int readed = fread( sBuffer, 1, size, fp);
	/*if ( readed != size)
	{
		delete[] sBuffer;
		fclose( fp);
		return FALSE;
	}*/
	BOOL bSucc = LoadFromData( sBuffer, readed);
	delete[] sBuffer;
	fclose( fp);
	return bSucc;
}

BOOL CInfoAnalyzer::LoadFromData( char *sBuffer, int length)
{
	m_sBuffer = CComString( sBuffer);
	return TRUE;
}

CComString CInfoAnalyzer::GetStringData()
{
	CComString sData;	
	sData.Format( "uuid=%s&appName=%s&moduleName=%s&path=%s&userNameInAcademia=%s&userName=%s&computerName=%s&sessionID=%s&IPAddr=%s",
					GetUUID().GetBuffer(), GetAppName().GetBuffer(), GetModuleName().GetBuffer(), GetProjectPath().GetBuffer(), GetUserNameInAcademia().GetBuffer(),
					GetUserName().GetBuffer(), GetComputerName().GetBuffer(), GetSessionID().GetBuffer(), GetIPAddress().GetBuffer());

	CComString sErrorType = GetErrorType();//GetModuleParam( GetAppName().GetBuffer(), "caused an ", "(");
	sErrorType.Replace( ' ', '_');

	sData += ( CComString( "&errorType=") + sErrorType);

	return sData;
}

void CInfoAnalyzer::DisableEncoding()
{
	m_bEncoding = FALSE;
}

void CInfoAnalyzer::EnableEncoding()
{
	m_bEncoding = TRUE;
}

BOOL CInfoAnalyzer::GetData( CDataBuffer *buffer)
{
	BOOL bEncoding = TRUE;
	if ( m_bEncoding)
		DisableEncoding();
	else 
		bEncoding = FALSE;

	CComString sUuid = GetUUID();
	unsigned short len = sUuid.GetLength();
	buffer->addData( sizeof(unsigned short), (BYTE*) &len);
	buffer->addData( len, (BYTE*) sUuid.GetBuffer());

	CComString sAppName = GetAppName();
	len = sAppName.GetLength();
	buffer->addData( sizeof(unsigned short), (BYTE*) &len);
	buffer->addData( len, (BYTE*) sAppName.GetBuffer());

	CComString sModuleName = GetModuleName();
	len = sModuleName.GetLength();
	buffer->addData( sizeof(unsigned short), (BYTE*) &len);
	buffer->addData( len, (BYTE*) sModuleName.GetBuffer());

	CComString sProjectName = GetProjectPath();
	sProjectName.Replace( "\\", "\\\\");
	len = sProjectName.GetLength();
	buffer->addData( sizeof(unsigned short), (BYTE*) &len);
	buffer->addData( len, (BYTE*) sProjectName.GetBuffer());

	CComString sUserNameInAcademia = GetUserNameInAcademia();
	len = sUserNameInAcademia.GetLength();
	buffer->addData( sizeof(unsigned short), (BYTE*) &len);
	buffer->addData( len, (BYTE*) sUserNameInAcademia.GetBuffer());

	CComString sUserName = GetUserName();
	len = sUserName.GetLength();
	buffer->addData( sizeof(unsigned short), (BYTE*) &len);
	buffer->addData( len, (BYTE*) sUserName.GetBuffer());

	CComString sComputerName = GetComputerName();
	len = sComputerName.GetLength();
	buffer->addData( sizeof(unsigned short), (BYTE*) &len);
	buffer->addData( len, (BYTE*) sComputerName.GetBuffer());

	CComString sSessionID = GetSessionID();
	len = sSessionID.GetLength();
	buffer->addData( sizeof(unsigned short), (BYTE*) &len);
	buffer->addData( len, (BYTE*) sSessionID.GetBuffer());

	CComString sIPAddress = GetIPAddress();
	len = sIPAddress.GetLength();
	buffer->addData( sizeof(unsigned short), (BYTE*) &len);
	buffer->addData( len, (BYTE*) sIPAddress.GetBuffer());

	CComString sErrorType = GetErrorType();
	sErrorType.Replace( ' ', '_');
	len = sErrorType.GetLength();
	buffer->addData( sizeof(unsigned short), (BYTE*) &len);
	buffer->addData( len, (BYTE*) sErrorType.GetBuffer());

	CComString sVersion = GetAppVersion();
	len = sVersion.GetLength();
	buffer->addData( sizeof(unsigned short), (BYTE*) &len);
	buffer->addData( len, (BYTE*) sVersion.GetBuffer());

	if ( bEncoding)
		EnableEncoding();

	return TRUE;
}
