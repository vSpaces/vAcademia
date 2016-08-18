// HTTPReader.cpp: implementation of the CHTTPReader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HTTPReader.h"

#include <string.h>
#include <tchar.h>

#pragma comment(lib,"wininet")


CHTTPReader::CHTTPReader(LPCTSTR lpszServerName,bool bUseSSL)
: m_hInternet(NULL),
  m_hConnection(NULL),
  m_hRequest(NULL),
  m_lpszServerName(NULL),
  m_lpszDefaultHeader(NULL),
  m_lpszDataBuffer(NULL),
  m_dwBufferSize(0),
  m_bUseSSL(bUseSSL),
  m_dwLastError(0)
{
	SetDefaultHeader(TEXT(
		"Content-Type: application/x-www-form-urlencoded\r\n"
	    "Accept-Language:ru\r\n"
	    "Accept-Encoding:gzip, deflate"));

	StrDup(m_lpszServerName,lpszServerName);
}

CHTTPReader::~CHTTPReader()
{
	delete m_lpszDataBuffer;
	delete m_lpszServerName;
	SetDefaultHeader(NULL);
}

bool CHTTPReader::CheckError(bool bTest)
{
	if (bTest == false) {
		m_dwLastError = ::GetLastError();
	}
	return bTest;
}

bool CHTTPReader::OpenInternet(ProxySettings* proxySettings, LPCTSTR lpszAgent)
{
	if (m_hInternet == NULL)
		m_hInternet = ::InternetOpen(
			lpszAgent,
			INTERNET_OPEN_TYPE_PROXY,
			proxySettings->toString().c_str(),
			NULL,
			0);

	return CheckError(m_hInternet != NULL);
}

void CHTTPReader::CloseInternet ()
{
	CloseConnection();
	
	if (m_hInternet) 
		::InternetCloseHandle(m_hInternet);
	m_hInternet = NULL;
}

bool CHTTPReader::OpenConnection (ProxySettings* proxySettings, LPCTSTR lpszServerName)
{
	if (OpenInternet(proxySettings) && m_hConnection == NULL)
		m_hConnection = ::InternetConnect(
			m_hInternet,
			lpszServerName? 
				lpszServerName:
				m_lpszServerName? 
					m_lpszServerName: 
					TEXT("localhost"),
			m_bUseSSL? INTERNET_DEFAULT_HTTPS_PORT: INTERNET_DEFAULT_HTTP_PORT,
			NULL,
			NULL,
			INTERNET_SERVICE_HTTP,
			0,
			1u);
	
	return CheckError(m_hConnection != NULL);
}

void CHTTPReader::CloseConnection ()
{
	CloseRequest();

	if (m_hConnection)
		::InternetCloseHandle(m_hConnection);
	m_hConnection = NULL;
}

bool CHTTPReader::SendRequest (ProxySettings* proxySettings, 
							   LPCTSTR lpszVerb,
							   LPCTSTR lpszAction,
							   LPCTSTR lpszData,
							   LPCTSTR lpszReferer)
{
	if (OpenConnection(proxySettings)) {
		CloseRequest();

		LPCTSTR AcceptTypes[] = { TEXT("*/*"), NULL}; 
		m_hRequest = ::HttpOpenRequest(
			m_hConnection,
			lpszVerb,
			lpszAction,
			NULL,
			lpszReferer,
			AcceptTypes,
			(m_bUseSSL? INTERNET_FLAG_SECURE|INTERNET_FLAG_IGNORE_CERT_CN_INVALID: 0) 
				| INTERNET_FLAG_KEEP_CONNECTION,
			1);

		if (m_hRequest != NULL) {
			if (::HttpSendRequest(
					m_hRequest, 
					m_lpszDefaultHeader, 
					-1,
					(LPVOID)lpszData,
					lpszData? _tcslen(lpszData): 0) == FALSE) {
				CheckError(false);
				CloseRequest();
				return false;
			}
		}
	}

	return CheckError(m_hRequest != NULL);
}

bool CHTTPReader::Get(ProxySettings* proxySettings, LPCTSTR lpszAction,LPCTSTR lpszReferer)
{
	return SendRequest(proxySettings, TEXT("GET"),lpszAction,NULL,lpszReferer);
}

bool CHTTPReader::Post(ProxySettings* proxySettings, LPCTSTR lpszAction,LPCTSTR lpszData,LPCTSTR lpszReferer)
{
	return SendRequest(proxySettings, TEXT("POST"),lpszAction,lpszData,lpszReferer);
}

void CHTTPReader::CloseRequest()
{
	if (m_hRequest)
		::InternetCloseHandle(m_hRequest);
	m_hRequest = NULL;
}

void CHTTPReader::StrDup (LPTSTR& lpszDest,LPCTSTR lpszSource)
{
	delete lpszDest;

	if (lpszSource == NULL) {
		lpszDest = NULL;
	} else {
		lpszDest = new TCHAR[_tcslen(lpszSource)+1];
		_tcscpy(lpszDest,lpszSource);
	}
}
																			    
void CHTTPReader::SetDefaultHeader (LPCTSTR lpszDefaultHeader)
{
	StrDup(m_lpszDefaultHeader,lpszDefaultHeader);
}

char *CHTTPReader::GetData (char *lpszBuffer,DWORD dwSize,DWORD *lpdwBytesRead)
{
	DWORD dwBytesRead;
	if (lpdwBytesRead == NULL)
		lpdwBytesRead = &dwBytesRead;
	*lpdwBytesRead = 0;

	if (m_hRequest) {
		bool bRead = ::InternetReadFile(
			m_hRequest,
			lpszBuffer,
			dwSize,
			lpdwBytesRead) != FALSE;
		lpszBuffer[*lpdwBytesRead] = 0;

		return CheckError(bRead) && *lpdwBytesRead? lpszBuffer: NULL;
	}

	return NULL;
}

DWORD CHTTPReader::GetDataSize ()
{
	if (m_hRequest) {
		DWORD dwDataSize = 0;
		DWORD dwLengthDataSize = sizeof(dwDataSize);

		BOOL bQuery = ::HttpQueryInfo(
			m_hRequest,
			HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
			&dwDataSize, 
			&dwLengthDataSize,
			NULL);

		return bQuery? dwDataSize: 0;
	}

	return 0;
}

char *CHTTPReader::GetData (DWORD *lpdwBytesRead)
{
	DWORD dwDataSize = GetDataSize();
	SetDataBuffer(dwDataSize);	
	return GetData(
		m_lpszDataBuffer, 
		dwDataSize? dwDataSize: m_dwBufferSize, 
		lpdwBytesRead);
}

void CHTTPReader::SetDataBuffer (DWORD dwBufferSize)
{
	if (dwBufferSize > m_dwBufferSize) {
		delete m_lpszDataBuffer;
		m_lpszDataBuffer = new char[(m_dwBufferSize = dwBufferSize) + 1];
	}
}
