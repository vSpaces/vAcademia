// HTTP.h: interface for the HTTP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTP_H__14358B89_2945_4450_BC97_FA2B95177016__INCLUDED_)
#define AFX_HTTP_H__14358B89_2945_4450_BC97_FA2B95177016__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wininet.h"
#include "HttpQuery.h"
//#include "LogFile.h"
//#include "curl/curl.h" 
#include "../curl/include/curl/curl.h"
#include "DataBuffer.h"
#include "consts.h"
#include "ILogWriter.h"
#include "oms_context.h"
#include "ProxySettings.h"
#include "iclientsession.h"
#include "ConnectionManager.h"

class CGuardCS{
	LPCRITICAL_SECTION mpcs;
public:
	CGuardCS(LPCRITICAL_SECTION pcs){
		mpcs=pcs;
		EnterCriticalSection(mpcs);
	}
	~CGuardCS(){
		LeaveCriticalSection(mpcs);
	}
};

// Типы запросов
enum EQueryType {
	qtStandart = 0, qtMultipart = 1, qtGet = 2, qtPost = 3, qtReadFile = 4
};

// Использование HTTP прокси
enum EProxyUsing {
	puDirect = 0, puProxy, puPreconfig
};

struct CHttpQuery;

struct CRequestHandles{
	HINTERNET mhINet;
	HINTERNET mhConnection;
	HINTERNET mhData;
	CRequestHandles(){
		mhINet = NULL;
		mhConnection = NULL;
		mhData = NULL;
	}
};

// Класс доступа к HTTP-серверу
class CHTTP:
	public cm::IChangeHttpSettings
{
public:
	CHTTP( oms::omsContext* apContext, ILogWriter* lw);
	virtual ~CHTTP();

	// Открыть запрос
	EQueryError openQuery(BSTR aQueryStr, BSTR aParams, CHttpQuery*& aQuery);
	EQueryError closeQuery(CHttpQuery*& aQuery);
	EQueryError readQueryData( CHttpQuery& aQuery, DWORD aBufferSize, BYTE aBuffer[], DWORD& aReadSize);
	// Послать запрос. Ожидаем текстовый ответ
	EQueryError sendQuery(BSTR aQuery, BSTR aParams, CComString &aResult, EQueryType qType=qtStandart);
	EQueryError sendQuerySync(BSTR aQuery, BSTR aParams, CComString &aResult, EQueryType qType=qtStandart);
	// Послать запрос. Ожидаем бинарный ответ
	EQueryError sendQueryData( BSTR aQuery, BSTR aParams, DWORD* aDataSize, BYTE** aData,
		EQueryType qType=qtStandart, LPVOID mpData=NULL, DWORD mpSize=0, bool aThread = true);
	// Послать запрос в отдельном потоке
	EQueryError sendQueryDataInThread( BSTR aQuery, BSTR aParams, CURL *pcurl, DWORD* aDataSize, BYTE** aData, EQueryType qType=qtStandart, LPVOID mpData=NULL, DWORD mpSize=0);
	// Вернуть идентификатор сесии
	inline long get_sessionID() { return dwSessionID;}
	// Установить идентификатор сесии
	inline void put_sessionID( long aVal) { dwSessionID = aVal; sSession.Format( "sessionID=%lu", dwSessionID);}
	// Вернуть адрес сервера
	inline LPCSTR get_server() { return sServerName;}
	// Установить адрес сервера
	inline void put_server( BSTR newVal) { sServerName = newVal;}
	// Начать сесию
	void resetSession();
	// Вернуть базовый путь запросов
	inline LPCSTR get_queryBase() { return sQueryBase;}
	// Установить базовый путь запросов
	inline void put_queryBase( BSTR newVal) { sQueryBase = newVal;}
	// 
	LPTSTR getErrorStr( EQueryError aError, CComString* aErrorStr);
	// 
	inline EProxyUsing get_proxyUsing() { return nProxyUsing;}
	// 
	inline void put_proxyUsing( EProxyUsing aProxyUsing) { nProxyUsing = aProxyUsing;}
	// 
	inline LPCSTR get_proxy() { return sProxy;}
	// 
	inline void put_proxy( LPCSTR aProxy) { sProxy = aProxy;}
	inline void put_proxy( BSTR aProxy) { sProxy = aProxy;}
	// 
	inline LPCSTR get_proxyUser() { return sProxyUser;}
	// 
	inline void put_proxyUser( LPCSTR aProxyUser) { sProxyUser = aProxyUser;}
	inline void put_proxyUser( BSTR aProxyUser) { sProxyUser = aProxyUser;}
	// 
	inline LPCSTR get_proxyPassword() { return sProxyPassword;}
	// 
	inline void put_proxyPassword( LPCSTR aProxyPassword) { sProxyPassword = aProxyPassword;}
	inline void put_proxyPassword( BSTR aProxyPassword) { sProxyPassword = aProxyPassword;}

	void SetILogWriter(ILogWriter *lw) {m_pLW = lw;}

	void applyProxySettings(IProxySettings* aProxySettings);

// cm::IChangeHttpSettings
public:
	virtual void OnChangeConnectionSettings(void* aProxyMode, void* aProxySettings);

protected:
	// Адрес сервера
	CComString sServerName;
	// Порт сервера
	INTERNET_PORT nPort;
	// Пользователь
	CComString sUser;
	// Пароль
	CComString sPassword;
	// Использование прокси сервера
	EProxyUsing nProxyUsing;
	// Адрес прокси сервера
	CComString sProxy;
	// Пользователь прокси сервера
	CComString sProxyUser;
	// Пароль прокси сервера
	CComString sProxyPassword;
	// Идентификатор сесии
	DWORD dwSessionID;
	// Строка с идентификатором сесии. Добавляется к каждому запросу
	CComString sSession;
	// Путь, отностльно которого делаются все запросы
	CComString sQueryBase;
	//
	ILogWriter * m_pLW;
	//
	CRITICAL_SECTION csSendQuery;
	// Контекс
	oms::omsContext* pContext;

private:
	// Формирование строки запроса с параметрами
	CComString formatQueryString(BSTR aQuery);
	CComString formatParamsString(BSTR aParams);
	// Получает текст ответа сервера и его длину
	//bool GetServerReply( HINTERNET m_hRequest, BYTE** lpData, DWORD* dwSize);
	// Послать данные на сервер multipart запросом
	//EQueryError SendMultipartRequest( HINTERNET m_hRequest, LPVOID lpData, DWORD dwSize);
	void SendMultipartRequest( CURL *curl, CDataBuffer *pData);
};

#endif // !defined(AFX_HTTP_H__14358B89_2945_4450_BC97_FA2B95177016__INCLUDED_)
