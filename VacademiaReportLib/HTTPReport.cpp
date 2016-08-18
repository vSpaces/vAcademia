#include "StdAfx.h"
#include "HTTPReport.h"
#include "func.h"
#include <DataBuffer.h>
#include "curl.h"

CHTTPReport::CHTTPReport()
{
	m_dwStatusCode = -1;
}

CHTTPReport::~CHTTPReport()
{

}

#define QM_BOUNDARY _T("----OOOOOOOOOOOEEEEEEEEECCCCCCCCCCCCC")

#define MAX_GET_QUERY_LEN (3 * MAX_PATH) + 220

static size_t ReadCallback(void *ptr, size_t size, size_t nmemb, void *userp)
{
	CDataBuffer *pooh = (CDataBuffer *)userp;

	if ( size * nmemb < 1)
		return 0;
	//int len = pooh->getLength();
	//CDataBuffer *byteptr = (CDataBuffer *)ptr;
	/*CDataBuffer *byteptr = NULL;//(CDataBuffer *)ptr;
	byteptr = new CDataBuffer();
	byteptr->addData(pooh->getLength(),(BYTE*(pooh));


	ptr = (void*((BYTE*(byteptr));*/
	//return byteptr.getLength();                         /* no more data left to deliver */
} 

static int GetServerReply(void *buffer, size_t size, size_t nmemb, void *stream)
{
	CDataBuffer *pData = (CDataBuffer*)stream;
	int dwDataSize = 0;
	if( size * nmemb < 1)
	{
		return -1;
	}
	int len = pData->getLength();
	pData->addData( size * nmemb, (BYTE*)buffer);
	return pData->getLength() - len;	
}

BOOL CHTTPReport::sendData( TCHAR *server, TCHAR* query, TCHAR *sData, int len, CDataBuffer *pDataBuffer /* = NULL */)
{
	if ( strlen( server) < 1)
	{
		CComString s;
		s.Format( "ERROR: length of server less than 1\n");
		WriteLog__( s.GetBuffer());
		return FALSE;
	}

	if ( strlen( query) < 1)
	{
		CComString s;
		s.Format( "ERROR: length of query less than 1\n");
		WriteLog__( s.GetBuffer());
		return FALSE;
	}

	if ( strlen( sData) < 16)
	{
		// корректные данные будут иметь длину не меньше 16
		CComString s;
		s.Format( "ERROR: length of data less than 16\n");
		WriteLog__( s.GetBuffer());
		return FALSE;
	}

	if ( strlen( server) + strlen( query) + strlen( sData) > MAX_GET_QUERY_LEN - 1)
	{
		CComString s;
		s.Format( "ERROR: length of data more than MAX_GET_QUERY_LEN\n");
		WriteLog__( s.GetBuffer());
		return FALSE;
	}

	BOOL bNoError = FALSE;
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	int nPort = 80;
	if ( curl) 
	{
		CURLM *multi_handle = NULL;		
		struct curl_slist *headerlist = NULL;
		TCHAR url[ MAX_GET_QUERY_LEN];
		//_tcscpy( url, server);
		if ( rtl_strcpy( url, MAX_GET_QUERY_LEN, server) == RTLERROR_CONVERTERROR)
		{
			CComString s;
			s.Format( "ERROR: rtl_strcpy string '%s' is failed\n", server);
			WriteLog__( s.GetBuffer());
			return FALSE;
		}
		if ( rtl_strcat( url, MAX_GET_QUERY_LEN - strlen(url), query) == RTLERROR_CONVERTERROR)
		{
			CComString s;
			s.Format( "ERROR: rtl_strcat string '%s' is failed\n", query);
			WriteLog__( s.GetBuffer());
			return FALSE;
		}
		if ( rtl_strcat( url, MAX_GET_QUERY_LEN, "?") == RTLERROR_CONVERTERROR)
		{
			CComString s;
			s.Format( "ERROR: rtl_strcat string '?' is failed\n");
			WriteLog__( s.GetBuffer());
			return FALSE;
		}
		if ( rtl_strcat( url, MAX_GET_QUERY_LEN, sData) == RTLERROR_CONVERTERROR)
		{
			CComString s;
			s.Format( "ERROR: rtl_strcat string '%s' is failed\n", sData);
			WriteLog__( s.GetBuffer());
			return FALSE;
		}
		//_tcscat( url, query);
		//_tcscat( url, "?");
		//_tcscat( url, sData);

		//int len = strlen( sData);
		//sData = curl_easy_escape(curl, sData, len);
		//len = strlen( url);		

		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_PORT, nPort);
		curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 65536);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		res = curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);
		headerlist = curl_slist_append(headerlist, "Accept: */*\r\n");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist); 

		CDataBuffer oData;
		res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, GetServerReply);
		res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &oData);		
		curl_easy_setopt(curl, CURLOPT_FAILONERROR);

		std::vector<char> errorBuffer( CURL_ERROR_SIZE + 1 );
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, &errorBuffer[0]);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		DWORD dwStatusCode = 0;
		int result = curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&m_dwStatusCode); 

		/* always cleanup */
		curl_easy_cleanup(curl);		
		/* free slist */
		//curl_slist_free_all (headerlist);
		curl = NULL;
		if ( m_dwStatusCode >= 200 && m_dwStatusCode < 300)
		{
			if ( pDataBuffer != NULL)
				pDataBuffer->addData( oData.getLength(), (BYTE*) oData);
			bNoError = TRUE;
		}
		else if ( m_dwStatusCode >= 400 && m_dwStatusCode < 500)
		{			
			bNoError = FALSE;
		}
		else if ( m_dwStatusCode >= 500 || m_dwStatusCode == 0)
		{
			bNoError = FALSE;				
		}
		CComString s;
		s.Format( "server = %s\n queryphp = %s\n url = %s\n dwStatusCode = %d\n", server, query, url, m_dwStatusCode);
		WriteLog__( s.GetBuffer());
		if( !bNoError)
		{
			s.Format(" curl errorBuffer==%s\n", &errorBuffer[0]);
			WriteLog__( s.GetBuffer());
		}
	}
	return bNoError;
}

BOOL CHTTPReport::sendFileData(TCHAR *server, TCHAR* query, WCHAR *fullPathSendFile, BYTE *buffer, int len, TCHAR *fileNameToServer,
							  DWORD *aDataSize, BYTE **aData)
{
	CURL *curl;
	CURLcode res;
	BOOL bNoError = FALSE;

	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;
	struct curl_slist *headerlist = NULL;
	static const char buf1[] = "Expect:";
	static const char buf2[] = "Content-Transfer-Encoding: binary";

	curl_global_init(CURL_GLOBAL_ALL);

	/* Fill in the file upload field */
	/*USES_CONVERSION;
	CComString s = W2A(fullPathSendFile);
	if( s.Find("?") == -1)
	{
		curl_formadd(&formpost,
			&lastptr,
			CURLFORM_COPYNAME, "uploadedfiledata",
			CURLFORM_FILE, fullPathSendFile,
			//CURLFORM_PTRCONTENTS, buffer,
			//CURLFORM_CONTENTSLENGTH, len,
			CURLFORM_CONTENTTYPE, "application/x-octet-stream",  
			CURLFORM_END);
	}
	else*/	
		curl_formadd(&formpost,
			&lastptr,
			CURLFORM_COPYNAME, "uploadedfiledata",
			//CURLFORM_FILE, fullPathSendFile,
			//CURLFORM_PTRCONTENTS, buffer,
			//CURLFORM_CONTENTSLENGTH, len,		
			CURLFORM_BUFFER, "uploadedfiledatabuffer",
			CURLFORM_BUFFERPTR, buffer,
			CURLFORM_BUFFERLENGTH, len,
			CURLFORM_CONTENTTYPE, "application/x-octet-stream",  
			CURLFORM_END);
	/*}*/
	/* Fill in the filename field */
	curl_formadd(&formpost,
		&lastptr,
		CURLFORM_COPYNAME, "filename",
		CURLFORM_COPYCONTENTS, fileNameToServer,
		CURLFORM_END);


	/* Fill in the submit field too, even if this is rarely needed */
	curl_formadd(&formpost,
		&lastptr,
		CURLFORM_COPYNAME, "submit",
		CURLFORM_COPYCONTENTS, "send",
		CURLFORM_END);

	curl = curl_easy_init();
	/* initalize custom header list (stating that Expect: 100-continue is not
	wanted */
	headerlist = curl_slist_append(headerlist, buf1);
	headerlist = curl_slist_append(headerlist, buf2);
	if ( curl) 
	{
		TCHAR url[ MAX_PATH];
		_tcscpy( url, server);
		_tcscat( url, query);
		/* what URL that receives this POST */
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); 		
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		//curl_easy_setopt(curl, CURL_VERSION_CONV, 1);
		//curl_easy_setopt(curl, CURLOPT_CONV_TO_NETWORK_FUNCTION, 1);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
		CDataBuffer oData;
		res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, GetServerReply);
		res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &oData);
		curl_easy_setopt(curl, CURLOPT_FAILONERROR);

		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		
		std::vector<char> errorBuffer( CURL_ERROR_SIZE + 1 );
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, &errorBuffer[0]);

		res = curl_easy_perform(curl);

		m_dwStatusCode = 0;
		int result = curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE, &m_dwStatusCode); 

		/* always cleanup */
		curl_easy_cleanup(curl);
		/* then cleanup the formpost chain */
		curl_formfree(formpost);
		/* free slist */
		curl_slist_free_all (headerlist);
		curl = NULL;
		if ( m_dwStatusCode >= 200 && m_dwStatusCode < 300)
		{
			if ( oData != NULL)
				*aDataSize = oData.getLength();
			bNoError = TRUE;
		}
		else if ( m_dwStatusCode >= 400 && m_dwStatusCode < 500)
		{
			bNoError = FALSE;
		}
		else if ( m_dwStatusCode >= 500 || m_dwStatusCode == 0)
			bNoError = FALSE;				
		CComString s;
		s.Format( "server = %s\n queryphp = %s\n url = %s\n dwStatusCode = %d\n", server, query, url, m_dwStatusCode);
		WriteLog__( s.GetBuffer());
		if( !bNoError)
		{
			s.Format(" curl errorBuffer==%s\n", &errorBuffer[0]);
			WriteLog__( s.GetBuffer());
		}
		if ( res == 0 && ( *aDataSize) > 0)
		{
			*aData = (BYTE*)GlobalAlloc( GPTR, *aDataSize);
			memcpy( *aData, (BYTE*)oData, *aDataSize);
		}
		else
		{
			*aData = NULL;
			*aDataSize = 0;
			// CURLE_COULDNT_RESOLVE_PROXY, CURLE_COULDNT_RESOLVE_HOST, CURLE_COULDNT_CONNECT = 7
		}
	}
	return bNoError;
}

BOOL CHTTPReport::sendBufferData(TCHAR *server, BYTE *buffer, int len)
{	
	return FALSE;
}

DWORD CHTTPReport::getStatusCode()
{
	return m_dwStatusCode;
}
