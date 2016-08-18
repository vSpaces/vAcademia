#include "stdafx.h"
#include "UrlParser.h"

CUrlParser::CUrlParser():
MP_WSTRING_INIT(wsFullURL),
MP_WSTRING_INIT(wsProtocol),
MP_WSTRING_INIT(wsHost),
MP_WSTRING_INIT(wsResource),
MP_WSTRING_INIT(wsBookmark),
MP_WSTRING_INIT(wsProtocolDefault),
MP_WSTRING_INIT(wsHostDefault),
MP_WSTRING_INIT(wsResourceDefault)
{
	wsFullURL = L"";
	nPort = 0;
	pParamMap = MP_NEW( String2StringMap);
}

CUrlParser::CUrlParser( const wchar_t* aURL):
MP_WSTRING_INIT(wsFullURL),
MP_WSTRING_INIT(wsProtocol),
MP_WSTRING_INIT(wsHost),
MP_WSTRING_INIT(wsResource),
MP_WSTRING_INIT(wsBookmark),
MP_WSTRING_INIT(wsProtocolDefault),
MP_WSTRING_INIT(wsHostDefault),
MP_WSTRING_INIT(wsResourceDefault)
{
	nPort = 0;
	pParamMap = MP_NEW( String2StringMap);
	ParseURL( aURL);
}

CUrlParser::~CUrlParser()
{
	Clear();
	if (pParamMap != NULL)
		MP_DELETE(pParamMap);
}

void CUrlParser::Clear()
{
	wsFullURL.clear();
	wsProtocol.clear();
	wsHost.clear();
	wsResource.clear();
	wsBookmark.clear();

	wsProtocol = wsProtocolDefault;
	wsProtocol = wsProtocolDefault;
	wsHost = wsHostDefault;
	nPort = nPortDefault;
	wsResource = wsResourceDefault;

	pParamMap->clear();
}

/*void CUrlParser::SetDefault( const wchar_t* aDefaultURL)
{
}*/

wchar_t* CreateWStr( const wchar_t* aBegin, const wchar_t*& aEnd)
{
	size_t size;
	if (aEnd == NULL)
		aEnd = aBegin + wcslen( aBegin);
	size = aEnd - aBegin;

	wchar_t* wsRes = MP_NEW_ARR( wchar_t, size + 1);
	rtl_wcsncpy( wsRes, size + 1, aBegin, size);
	wsRes[size] = 0;
	return wsRes;
}

wchar_t* CreateEmptyWStr()
{
	wchar_t* wsRes = MP_NEW_ARR( wchar_t, 1);
	wsRes[0] = 0;
	return wsRes;
}

wstring	trimQuotes( const wchar_t* aURL)
{
	if ((aURL == NULL) || (wcslen(aURL) == 0))
		return L"";

	wstring ret = aURL;
	while( ret.length() > 0)
	{
		if( ret[0] == L'\"' || ret[0] == L'\'')
			ret = ret.substr(1, ret.length() - 1);
		else
			break;
	}

	while( ret.length() > 0)
	{
		if( ret[ret.length() - 1] == L'\"' || ret[ret.length() - 1] == L'\'')
			ret = ret.substr(0, ret.length() - 1);
		else
			break;
	}

	return ret;
}

void CUrlParser::ParseURL( const wchar_t* apURL)
{
	Clear();
	if ((apURL == NULL) || (wcslen(apURL) == 0)) {
		return;
	}

//	int size;
	const wchar_t* p = NULL;

	wstring	sUrl = trimQuotes( apURL);

	const wchar_t* aURL = sUrl.c_str();
	//
	// wsProtocol (mandatory)
	//
	const wchar_t* pProtocol = aURL;
	const wchar_t* pProtocolEnd = wcsstr(aURL, L"://");
	if (pProtocolEnd != NULL) {
		wsProtocol.assign( pProtocol, pProtocolEnd - pProtocol);
		pProtocolEnd += 3;
		wsHost.clear();
		wsResource.clear();
		wsBookmark.clear();
	}
	else
		pProtocolEnd = pProtocol;


	//
	// server (mandatory)
	// and
	// nPort (optional)
	//
	const wchar_t* pHost = pProtocolEnd;
	const wchar_t* pHostEnd = wcsstr( pHost, L"/");
	wsHost.assign( pHost, pHostEnd - pHost);
	wstring::size_type pos = wsHost.find( L':');
	if (pos != wstring::npos)
	{
		nPort = (unsigned short)wcstoul( wsHost.substr( pos + 1).c_str(), NULL, 10);
		wsHost.resize( pos);
	}

	//
	// wsResource (optional)
	//
	const wchar_t* pRes = pHostEnd;
	const wchar_t* pResEnd = wcsstr( pRes, L"#");
	if (pResEnd == NULL)
		pResEnd = wcsstr( pRes, L"?");
	wsResource = CreateWStr( pRes, (const wchar_t *&) pResEnd);

	//
	// wsBookmark (optional)
	//
	if (*pResEnd == L'#')
	{
		const wchar_t* pBookmark = pResEnd + 1;
		const wchar_t* pBookmarkEnd = wcsstr( pBookmark, L"?");
		wsBookmark = CreateWStr( pBookmark, (const wchar_t *&) pBookmarkEnd);
	}

	//
	// pParamMap (optional)
	//
	const wchar_t* pParams = wcsstr( pResEnd, L"?");
	while (pParams != NULL && *pParams != 0)
	{
		pParams += 1;
		const wchar_t* pParamName = pParams;
		pParams = wcsstr( pParamName, L"=");
		if (pParams == NULL)
			break;
		size_t paramNameSize = pParams - pParamName;

		pParams += 1;
		const wchar_t* pValue = pParams;
		p = wcsstr( pParams, L"&");
		size_t valSize;
		if (p == NULL)
			valSize = wcslen( pParams);
		else
			valSize = p - pValue;
		pParams = p;

		wchar_t* wsParamName = MP_NEW_ARR( wchar_t, paramNameSize + 1);
		rtl_wcsncpy( wsParamName, paramNameSize + 1, pParamName, paramNameSize);
		wsParamName[paramNameSize] = 0;

		wchar_t* wsValue = MP_NEW_ARR( wchar_t, valSize + 1);
		rtl_wcsncpy( wsValue, valSize + 1, pValue, valSize);
		wsValue[valSize] = 0;

		pParamMap->insert( String2String( wsParamName, wsValue));
	}

	CreateFullURL();
}

size_t GetLength( const wstring& aWStr, int anAddon)
{
	size_t size = aWStr.size();
	if (size == 0)
		return 0;
	else
		return size + anAddon;
}

void CUrlParser::CreateFullURL()
{
	int fullSize =
		GetLength( wsProtocol, 3) + GetLength( wsHost, 1) +
		GetLength( wsResource, 1) + GetLength( wsBookmark, 1);
	if (pParamMap->size() > 0)
	{
		String2StringMap::iterator it = pParamMap->begin();
		while (it != pParamMap->end())
		{
			fullSize += GetLength( it->first.c_str(), 1) + GetLength( it->second.c_str(), 1);
			it++;
		}
	}
	wsFullURL.clear();
	wsFullURL += wsProtocol;
	wsFullURL += L"://";
	wsFullURL += wsHost;
	wsFullURL += wsResource;
	if (!wsBookmark.empty())
	{
		wsFullURL += L"#";
		wsFullURL += wsBookmark;
	}
	if (pParamMap->size() > 0)
	{
		String2StringMap::iterator it = pParamMap->begin();
		while (it != pParamMap->end())
		{
			if (it == pParamMap->begin())
				wsFullURL += L"?";
			else
				wsFullURL += L"&";
			wsFullURL += it->first.c_str();
			wsFullURL += L"=";
			wsFullURL += it->second.c_str();
			it++;
		}
	}
}

const wchar_t* CUrlParser::GetParam(const wchar_t* aParamName, const wchar_t* aDefault)
{
	wstring wsParamName( aParamName);
	String2StringMap::iterator it = pParamMap->find( wsParamName);
	if (it == pParamMap->end())
		return aDefault;
	else
		return it->second.c_str();
}

bool CUrlParser::AddParam(const wchar_t* aParamName, const wchar_t* aValue)
{
	wstring wsParamName( aParamName);
	wstring wsValue( aValue);
	String2StringMap::iterator it = pParamMap->find( wsParamName);
	if (it != pParamMap->end())
		pParamMap->erase( it);
	pParamMap->insert( String2String( wsParamName, wsValue));
	CreateFullURL();
	return true;
}

bool CUrlParser::RemoveParam(const wchar_t* aParamName)
{
	wstring wsParamName( aParamName);
	String2StringMap::iterator it = pParamMap->find( wsParamName);
	if (it == pParamMap->end())
		return false;
	pParamMap->erase( it);
	CreateFullURL();
	return true;
}