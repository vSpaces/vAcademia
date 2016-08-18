// HTTP.cpp: implementation of the CHTTP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>
#include "HTTP.h"
#include "ThreadAffinity.h"
#include "rm.h"
#include "CrashHandlerInThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define BUFFER_SIZE 65000
#define QT_GET	_T("GET")
#define QT_POST	_T("POST")
#define QM_BOUNDARY _T("----OOOOOOOOOOOEEEEEEEEECCCCCCCCCCCCC")

/*#define CODI	"\r\nContent-Disposition: form-data; name=\""
#define FINA	"\"; filename=\""
#define ENDCO	"\"\r\n\r\n"
#define COTY	"Content-Type: application/octet-stream"*/

CHTTP::CHTTP( oms::omsContext* apContext, ILogWriter* lw) : m_pLW(lw), nPort( 80), nProxyUsing( puPreconfig)
{
	put_sessionID( 0);
	m_pLW = lw;
	pContext = apContext;
	curl_global_init(CURL_GLOBAL_ALL); 
}

CHTTP::~CHTTP()
{
	pContext = NULL;
}

struct CHttpQuery
{
	CDataBuffer oData;
	CURL *mpcurl;
	bool bSent;
	DWORD dwStatusCode;
	// для ведения лога:
	char sQuery[3124];
	//
	CHttpQuery()
	{
		ZeroMemory( this, sizeof( *this));
	}
};

static int GetServerReply(void *buffer, size_t size, size_t nmemb, void *stream)
{
	CDataBuffer *pData = (CDataBuffer*)stream;
	if(size*nmemb<1){
		return -1;
	}
	int len = pData->getLength();
	pData->addData(size*nmemb, (BYTE*)buffer);
	return pData->getLength()-len;
}
EQueryError CHTTP::openQuery(BSTR aQueryStr, BSTR aParams, CHttpQuery*& aQuery)
{
	if(sServerName.IsEmpty()) return qeServerError;
	CComString sQuery = formatQueryString( aQueryStr);
	CComString sParams = formatParamsString(aParams);

	CHttpQuery* pQuery = aQuery = MP_NEW(CHttpQuery);

	EQueryError iResult = qeNoError;

	if (nProxyUsing == puProxy && sProxy.GetLength() > 0)
	{
		curl_easy_setopt(pQuery->mpcurl, CURLOPT_PROXY, sProxy.GetString());
		if(!sProxyUser.IsEmpty())
		{
			CComString sProxyUserPass = sProxyUser + ":" + sProxyPassword;
			curl_easy_setopt(pQuery->mpcurl, CURLOPT_PROXYUSERPWD, sProxyUserPass.GetString());
		}
	}
	CComString sUrl = sServerName + "/"+ sQuery+"?"+sParams; 
	curl_easy_setopt(pQuery->mpcurl, CURLOPT_URL, sUrl.GetString());
	curl_easy_setopt(pQuery->mpcurl, CURLOPT_PORT, nPort);
	curl_easy_setopt(pQuery->mpcurl, CURLOPT_FOLLOWLOCATION, 1);

	curl_easy_setopt(pQuery->mpcurl, CURLOPT_WRITEFUNCTION, GetServerReply);
	curl_easy_setopt(pQuery->mpcurl, CURLOPT_WRITEDATA, &pQuery->oData);
	rtl_strcpy(pQuery->sQuery, va_arrsize_in_bytes(pQuery->sQuery), sUrl.GetString());
	return iResult;
}

EQueryError CHTTP::closeQuery(CHttpQuery*& aQuery)
{
	CHttpQuery* pQuery = (aQuery);
	if (pQuery != NULL && pQuery->mpcurl!=NULL)
	{
		curl_easy_cleanup(pQuery->mpcurl);
		pQuery->mpcurl = NULL;
	}
	aQuery = NULL;
	return qeNoError;
}

EQueryError CHTTP::readQueryData( CHttpQuery& aQuery, DWORD aBufferSize, BYTE aBuffer[], DWORD& aReadSize)
{
	aReadSize = 0;
	if (aQuery.mpcurl == NULL)
		return qeServerError;

	EQueryError iResult = qeNoError;

	CURLcode res;
	if (!aQuery.bSent)
	{
		curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "Accept: */*\r\n");
		curl_easy_setopt(aQuery.mpcurl, CURLOPT_HTTPHEADER, headers); 
		res = curl_easy_perform(aQuery.mpcurl);
		if(res!=CURLE_OK){
			//int errorCode = GetLastError();
			iResult = qeServerError;
		}
		else
		{
			aQuery.bSent = true;
			curl_easy_getinfo(aQuery.mpcurl,CURLINFO_RESPONSE_CODE,aQuery.dwStatusCode); 
		}
	}

	if (iResult == qeNoError && aQuery.dwStatusCode >= 200 && aQuery.dwStatusCode < 300)
	{
		rtl_memcpy(aBuffer, aBufferSize, (BYTE*)aQuery.oData, aBufferSize);
		aReadSize = aQuery.oData.getLength();
		if(aReadSize>aBufferSize){
			aQuery.oData.growFromBegin(aReadSize-aBufferSize);
		}
	}
	else if (aQuery.dwStatusCode >= 400 && aQuery.dwStatusCode < 500)
		iResult = qeQueryNotFound;
	else
		iResult = qeScriptError;

	if (iResult != qeNoError && m_pLW != NULL)
	{
		CComString sErrMessage;
		if (m_pLW != NULL)
		{
			CLogValue log("[ERROR] ", getErrorStr( iResult, &sErrMessage), " - query: ", aQuery.sQuery, "aQuery.dwStatusCode = ", aQuery.dwStatusCode);
			m_pLW->WriteLnLPCSTR(log.GetData());
		}
	}
	return iResult;
}

EQueryError CHTTP::sendQuery(BSTR aQuery, BSTR aParams, CComString &aResult, EQueryType qType)
{
	BYTE* pData = NULL;
	DWORD dwDataSize = 0;
	EQueryError iResult = sendQueryData( aQuery, aParams, &dwDataSize, &pData, qType);
	if (iResult == qeNoError)
	{		
		aResult.CreateString( (LPSTR)pData, dwDataSize);
		if (pData != NULL)
			MP_DELETE_ARR( pData);
	}
	return iResult;
}

EQueryError CHTTP::sendQuerySync(BSTR aQuery, BSTR aParams, CComString &aResult, EQueryType qType)
{
	BYTE* pData = NULL;
	DWORD dwDataSize = 0;
	EQueryError iResult = sendQueryData( aQuery, aParams, &dwDataSize, &pData, qType, NULL, 0, false);
	if (iResult == qeNoError)
	{		
		aResult.CreateString( (LPSTR)pData, dwDataSize);
		if (pData != NULL)
			MP_DELETE_ARR( pData);
	}
	return iResult;
}

struct CQueryInfo {
	BSTR bsQuery;
	BSTR bsParams;
	DWORD* pDataSize;
	BYTE** pData;
	EQueryType qType;
	LPVOID mpData;
	DWORD mpSize;
	CHTTP* pHTTP;
	EQueryError nResult;
	// будем сохранять хэндлы для того, чтобы можно было их закрыть, если поток подвиснет
	CURL *mpcurl;
	//CRequestHandles mRequestHandles; 
};

DWORD WINAPI queryThread( LPVOID aQueryInfo)
{
	CQueryInfo* pQuery = (CQueryInfo*) aQueryInfo;
	if (pQuery != NULL && pQuery->pHTTP != NULL)
		pQuery->nResult = pQuery->pHTTP->sendQueryDataInThread( pQuery->bsQuery, pQuery->bsParams, pQuery->mpcurl,
			pQuery->pDataSize, pQuery->pData, pQuery->qType, pQuery->mpData, pQuery->mpSize);
	return 0;
}

EQueryError CHTTP::sendQueryData(BSTR aQuery, BSTR aParams, 
								 DWORD* aDataSize, BYTE** aData, EQueryType qType, 
								 LPVOID mpData, DWORD mpSize, bool aThread)
{
	if(sServerName.IsEmpty())
		return qeServerError;

	if (aThread)
	{
		CQueryInfo oQuery;
		oQuery.mpcurl = NULL;
		oQuery.bsQuery = aQuery;
		oQuery.bsParams = aParams;
		oQuery.pDataSize = aDataSize;
		oQuery.pData = aData;
		oQuery.qType = qType;
		oQuery.mpData = mpData;
		oQuery.mpSize = mpSize;
		oQuery.pHTTP = this;
		oQuery.nResult = qeNotImplError;

		CSimpleThreadStarter queryThreadStarter(__FUNCTION__);		
		queryThreadStarter.SetRoutine(queryThread);
		queryThreadStarter.SetParameter(&oQuery);
		HANDLE hQueryThread = queryThreadStarter.Start();

		// будем ждать только hQueryThread
		WaitForSingleObject( hQueryThread, INFINITE);
		CloseHandle(hQueryThread);
		return oQuery.nResult;
	}
	else
	{
		CURL *pcurl = NULL;
		return sendQueryDataInThread(aQuery, aParams, pcurl, aDataSize, aData, qType, mpData, mpSize);
	}
}

void FormatErrorMessage(){
	LPVOID lpMsgBuf;
	if (!FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL ))
	{
		// Handle the error.
		return;
	}

	// Process any inserts in lpMsgBuf.
	// ...

	// Display the string.
	//MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );

	// Free the buffer.
	LocalFree( lpMsgBuf );
}

EQueryError CHTTP::sendQueryDataInThread(BSTR aQuery, BSTR aParams, CURL *pculr, 
										 DWORD* aDataSize, BYTE** aData, EQueryType qType, 
										 LPVOID mpData, DWORD mpSize)
{
	*aDataSize = 0;
	*aData = NULL;
	EQueryError iResult = qeNoError;

	CURL *curl = pculr;
	CURLcode res = CURLE_OK;

	if(sServerName.IsEmpty()) return qeServerError;
	
	curl = curl_easy_init();

	if(curl) {
		CURLM *multi_handle = NULL;
		CComString sUrl;
		CComString sQueryPostData;
		CComString sQuery;
		if(qType == qtReadFile)
		{
			sUrl = aQuery; 
		}
		else
		{
			sQuery = formatQueryString( aQuery);
			sQueryPostData = formatParamsString(aParams);
			sUrl = sServerName + "/"+ sQuery; 
			if(qType == qtStandart || qType == qtGet /*|| qType == qtMultipart*/){
				sUrl += "?"; 
				sUrl += sQueryPostData; 
			}
		}
		if (nProxyUsing == puProxy && sProxy.GetLength() > 0)
		{
			curl_easy_setopt(curl, CURLOPT_PROXY, sProxy.GetString());
			if(!sProxyUser.IsEmpty())
			{
				CComString sProxyUserPass = sProxyUser + ":" + sProxyPassword;
				curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, sProxyUserPass.GetString());
			}
		}
		curl_easy_setopt(curl, CURLOPT_URL, sUrl.GetString());
		//if(qType != qtReadFile)
		curl_easy_setopt(curl, CURLOPT_PORT, nPort);
		curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 65536);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20);
		
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		res = curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);

		if( sUser.IsEmpty()==false){
			CComString sUserPass = sUser + ":" + sPassword;
			curl_easy_setopt(curl, CURLOPT_USERPWD, sUserPass.GetBuffer());	
			curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
		}
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
	
		curl_slist *headers = NULL;
		struct curl_httppost *formpost=NULL;
		struct curl_httppost *lastptr=NULL;
		if(qType == qtPost && sQueryPostData.IsEmpty()==false){
			res = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, sQueryPostData.GetString());
			//curl_formadd(&formpost, &lastptr, CURLFORM_BUFFER, (LPVOID)sQueryPostData.GetString(), CURLFORM_END);
			//TCHAR tsz[ 1024 ];
			//wsprintf( tsz, _T( "Accept: */*\r\nContent-Type: application/x-www-form-urlencoded\r\n"
			//	"Content-Length: %d\r\n\r\n"), sQueryPostData.GetLength());
			//headers = curl_slist_append(headers, tsz);
			//curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
		}
		else if(qType==qtMultipart){
			multi_handle = curl_multi_init();
			TCHAR tsz[ 128 ];
			wsprintf( tsz, _T( "Content-Type: multipart/form-data; boundary=\"%s\"\r\n" ), QM_BOUNDARY );
			headers = curl_slist_append(headers, tsz);
			CDataBuffer oData;
			oData.addData(mpSize, (BYTE*)mpData);
			SendMultipartRequest(curl, &oData);
			curl_formadd(&formpost, &lastptr, CURLFORM_BUFFER, (LPVOID)mpData, CURLFORM_END);
			curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
			// &???  headers = curl_slist_append(headers, "Accept: */*\r\n");
		}
		else{
			headers = curl_slist_append(headers, "Accept: */*\r\n");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); 
		}
		
		CDataBuffer oData;
		res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, GetServerReply);
		res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &oData);
		//char *sRequest = new char(3124);
		//curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, GetHTTPRequest);
		//curl_easy_setopt(curl, CURLOPT_WRITEHEADER, sRequest); 
		curl_easy_setopt(curl, CURLOPT_FAILONERROR);	

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		
		DWORD dwStatusCode = 0;
 		curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&dwStatusCode); 

		/* always cleanup */
		curl_slist_free_all(headers); 
		curl_easy_cleanup(curl);
		curl = NULL;

		if (res == CURLE_OPERATION_TIMEDOUT)
		{
			return qeTimeOut;
		}

		if (dwStatusCode >= 200 && dwStatusCode < 300)
		{
		}
		else if (dwStatusCode >= 400 && dwStatusCode < 500)
		{
			iResult = qeQueryNotFound;
		}
		else
		{
			iResult = qeScriptError;
		}

		if (dwStatusCode >= 400)
		{
			CComString sTypeError = "Server is not access";
			if (dwStatusCode >= 500)
				sTypeError =  "Host is not access";
			if (nProxyUsing == puProxy && sProxy.GetLength() > 0)
			{
				if(sProxyUser.IsEmpty())
				{
					if (m_pLW != NULL)
					{
						CLogValue log("ERROR[", sTypeError.GetString(), "]: Cannot connect to Server ", sUrl.GetString(), ", port=", (int)nPort, ", Proxy: ", sProxy.GetString());
						m_pLW->WriteLnLPCSTR(log.GetData());
					}
				}
				else
				{
					CComString sProxyUserPass = sProxyUser + ":" + sProxyPassword;
					if (m_pLW != NULL)
					{
						CLogValue log("ERROR[", sTypeError.GetString(), "]: Cannot connect to Server ", sUrl.GetString(), ", port=", (int)nPort, ", Proxy: ", sProxy.GetString(), ", proxyuser: ", sProxyUserPass.GetString());
						m_pLW->WriteLnLPCSTR(log.GetData());
					}
				}
				if( pContext != NULL && pContext->mpSM != NULL && pContext->mpMapMan != NULL 
					&& (dwStatusCode == 407 || dwStatusCode == 401))
				{
					const wchar_t* pwcLogin = pContext->mpComMan->GetClientStringID();
					bool bRender = pContext->mpMapMan->IsRender3DNeeded();
					if( bRender && pwcLogin != NULL && *pwcLogin != L'\0')
					{
						CComString sProxyServer = sProxy;
						int pos = sProxyServer.Find(":", 0);
						if( pos > -1)
							sProxyServer = sProxyServer.Left( pos);
						USES_CONVERSION;
						if( pContext->mpSM != NULL)
							pContext->mpSM->OnProxyAuthError( L"user_settings", PT_HTTP, A2W( sProxyServer.GetBuffer()));
						::SysFreeString( (BSTR)pwcLogin);
					}
				}				
			}
			else
			{
				if (m_pLW != NULL)
				{
					CLogValue log("ERROR[", sTypeError.GetString(), "]: Cannot connect to Server ", sUrl.GetString(), ", port=", nPort);
					m_pLW->WriteLnLPCSTR(log.GetData());
				}
			}			
		}

		*aDataSize = oData.getLength();
		if (res==0 && (*aDataSize)>0)
		{
			//*aData = (BYTE*)GlobalAlloc( GPTR, *aDataSize);
			*aData = MP_NEW_ARR( BYTE, *aDataSize);
			rtl_memcpy( *aData, *aDataSize, (BYTE*)oData, *aDataSize);
		}
		else
		{
			*aData = NULL;
			*aDataSize = 0;
		}
		if (res==23)
			iResult = qeReadError;

		if (iResult != qeNoError && m_pLW != NULL)
		{
			CComString sErrMessage;
			if (m_pLW != NULL)
			{
				CLogValue log("[ERROR] ", getErrorStr( iResult, &sErrMessage), " - query: ", sQuery.GetBuffer(0), ", dwStatusCode = ", dwStatusCode);
				m_pLW->WriteLnLPCSTR(log.GetData());
			}
		}
	}
	return iResult;
}

CComString CHTTP::formatQueryString(BSTR aQuery)
{
	CComString sQuery( aQuery);
	sQuery = sQueryBase + sQuery;
	return sQuery;
}

CComString CHTTP::formatParamsString(BSTR aParams)
{
	CComString sParams( (aParams == NULL ? L"" : aParams));
	CComString sParamsRes="";
	if (sParams.GetLength() > 0)
		sParams = sSession + "&" + sParams;
	else
		sParams = sSession;
	if (sParams.GetLength() > 0)
	{
		BYTE ch;
		char sDigits[ 3];
		for (int i = 0;  i < sParams.GetLength();  i++) {
			ch = sParams[ i];
			if (ch > 32 && ch < 128)
				sParamsRes += ch;
			else
			{
				sParamsRes += '%';
				if (ch < 16)
					sParamsRes += '0';
				sParamsRes += (char*)_itoa_s( ch, sDigits, 3, 16);
			}
		}
	}

	// Add random appendix
	CComString sRandomParamName = "rnd_http_param=";
	int sRandomParamNameIndex = 0;
	while( sParams.Find( sRandomParamName) != -1)
	{
		sRandomParamName.Format("rnd_http_param%u=", sRandomParamNameIndex++);
	}

	CComString sRandomParamValue;

	/* initialize random seed: */
	srand ( (int)this + (int)time(NULL));
	int randValue = rand();
	sRandomParamValue.Format("%u%u", timeGetTime(), randValue);

	if( sParams.IsEmpty())
		sParams = sRandomParamName + sRandomParamValue;
	else
		sParams = sParams + "&" + sRandomParamName + sRandomParamValue;

	return sParams;
}

static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{


	if(size*nmemb < 1)
		return 0;
	if( ptr == NULL || userp == NULL)
		return size;
	return size;

	//int len = pooh->getLength();
	//CDataBuffer *byteptr = (CDataBuffer *)ptr;
	/*CDataBuffer *byteptr = NULL;//(CDataBuffer *)ptr;
	byteptr = new CDataBuffer();
	byteptr->addData(pooh->getLength(),(BYTE*(pooh));


	ptr = (void*((BYTE*(byteptr));*/
	//return byteptr.getLength();                         /* no more data left to deliver */
} 

/*EQueryError*/ void CHTTP::SendMultipartRequest( CURL *curl, CDataBuffer *pData)
{
	//EQueryError nResult = qeNoError;
	//BOOL	bres=FALSE;
	//DWORD dwCnt = dwSize;

	//curl_easy_setopt(curl, CURLOPT_UPLOAD, 1); 

	/* we want to use our own read function */
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);

	/* pointer to pass to our read function */
	/*struct WriteThis pooh;

	pooh.readptr = (char*)lpData;
	pooh.sizeleft = dwSize; */
	curl_easy_setopt(curl, CURLOPT_READDATA, &pData);
	//return nResult;
}

void CHTTP::resetSession()
{
	DWORD dwSize = 0;
	BYTE* pData = NULL;
	sendQueryData( L"session_reset.php", L"", &dwSize, &pData);
	if (pData != NULL)
		MP_DELETE_ARR( pData);
}

LPTSTR CHTTP::getErrorStr( EQueryError aError, CComString *aErrorStr)
{
	switch (aError) {
	case qeNoError:       *aErrorStr = ""; break;
	case qeServerError:   *aErrorStr = "Ошибка сервера"; break;
	case qeQueryNotFound: *aErrorStr = "Ошибка запрос не найден"; break;
	case qeScriptError:   *aErrorStr = "Ошибка выполения скрипта"; break;
	case qeReadError:     *aErrorStr = "Ошибка чтения (сети)"; break;
	case qeServerNotResolved: *aErrorStr = "Не удалось получить IP-адрес сервера по его имени"; break;
	case qeTimeOut:		  *aErrorStr = "Превышено время ожидания ответа"; break;
	default: *aErrorStr = "";
	}
	return (LPTSTR)aErrorStr->GetString();
}

void CHTTP::applyProxySettings(IProxySettings* aProxySettings)
{
	if (aProxySettings == NULL)
		return;

	USES_CONVERSION;
	string proxy = W2A(aProxySettings->getIP());
	proxy += ":";
	char buff[50];
	_itoa_s((int)aProxySettings->getPort(), buff, 50, 10);
	proxy += buff;
	put_proxy(proxy.c_str());
	put_proxyUser(W2A(aProxySettings->getUser()));
	put_proxyPassword(W2A(aProxySettings->getPassword()));
}

void CHTTP::OnChangeConnectionSettings(void* aProxyMode, void* aProxySettings)
{
	IProxySettings* pProxySettings = (IProxySettings*)aProxySettings;
	ProxyMode* pProxyMode = (ProxyMode*)aProxyMode;

	if (*pProxyMode == PM_NO_PROXY)
	{
		put_proxyUsing(puDirect);
	}
	else if (*pProxyMode == PM_MANUAL)
	{
		put_proxyUsing(puProxy);
		applyProxySettings(pProxySettings);
	}
	else if (*pProxyMode == PM_SYSTEM)
	{
		if (pProxySettings != NULL)
		{
			put_proxyUsing(puProxy);
			applyProxySettings(pProxySettings);
		}
		else
			put_proxyUsing(puPreconfig);
	}
}
 