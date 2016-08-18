// HTTPReader.h: interface for the CHTTPReader class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __HTTPREADER_H__
#define __HTTPREADER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WININET_
# ifndef _WINDOWS_
#  include <windows.h>
# endif
# include <wininet.h>
#endif

#include "ProxySettings.h"

class CHTTPReader
{
public:
	CHTTPReader(LPCTSTR lpszServerName=NULL,bool bUseSSL=false);
	~CHTTPReader();

	bool  OpenInternet     (ProxySettings* proxySettings, LPCTSTR lpszAgent=TEXT("RSDN HTTP Reader"));
	void  CloseInternet    ();
	bool  OpenConnection   (ProxySettings* proxySettings, LPCTSTR lpszServerName=NULL);
	void  CloseConnection  ();

	bool  Get              (ProxySettings* proxySettings, LPCTSTR lpszAction,LPCTSTR lpszReferer=NULL);
	bool  Post             (ProxySettings* proxySettings, LPCTSTR lpszAction,LPCTSTR lpszData,LPCTSTR lpszReferer=NULL);
	void  CloseRequest     ();

	char *GetData          (char *lpszBuffer,DWORD dwSize,DWORD *lpdwBytesRead=NULL);
	char *GetData          (DWORD *lpdwBytesRead=NULL);
	DWORD GetDataSize      ();
	void  SetDataBuffer    (DWORD dwBufferSize);

	void  SetDefaultHeader (LPCTSTR lpszDefaultHeader);
	DWORD GetError         () const { return m_dwLastError; }

protected:
	bool CheckError (bool bTest);
	void StrDup     (LPTSTR& lpszDest,LPCTSTR lpszSource);
	bool SendRequest(ProxySettings* proxySettings, LPCTSTR lpszVerb,LPCTSTR lpszAction,LPCTSTR lpszData,LPCTSTR lpszReferer);

	HINTERNET m_hInternet;
	HINTERNET m_hConnection;
	HINTERNET m_hRequest;

	DWORD     m_dwLastError;
	bool      m_bUseSSL;
	LPTSTR    m_lpszServerName;
	LPTSTR    m_lpszDefaultHeader;
	char     *m_lpszDataBuffer;
	DWORD     m_dwBufferSize;
};

#endif // __HTTPREADER_H__
