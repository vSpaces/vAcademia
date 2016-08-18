#ifndef CURLPARSER_H
#define CURLPARSER_H

#include <map>
#include <string>

using std::wstring;
using std::map;
using std::pair;
typedef map <wstring, wstring> String2StringMap;
typedef pair <wstring, wstring> String2String;


/*
* Creates a new URL object from a string representation of the URL. The URL
* must be in the form:
*
* <protocol>://<host>[:<port>]/<resource>[#<bookmark>][?<parameters>]
*
* If a parsing error occurs, fullURL is set to an empty string ("").
*
* @param url the URL
*/

class CUrlParser
{
public:
	CUrlParser();
	CUrlParser( const wchar_t* aURL);
	virtual ~CUrlParser();

	void Clear();
	//void SetDefault( const wchar_t* aDefaultURL);
	void ParseURL( const wchar_t* apURL);
	const wchar_t* GetFullURL() { return wsFullURL.c_str();};
	const wchar_t* GetProtocol() { return wsProtocol.c_str();};
	const wchar_t* GetHost() { return wsHost.c_str();};
	unsigned short GetPort() { return nPort;};
	const wchar_t* GetResource() { return wsResource.c_str();};
	const wchar_t* GetBookmark() { return wsBookmark.c_str();};
	const wchar_t* GetParam(const wchar_t* aParamName, const wchar_t* aDefault);
	bool AddParam(const wchar_t* aParamName, const wchar_t* aValue);
	bool RemoveParam(const wchar_t* aParamName);

protected:
	void CreateFullURL();

	MP_WSTRING wsFullURL;
	MP_WSTRING wsProtocol;
	MP_WSTRING wsHost;
	unsigned short nPort;
	MP_WSTRING wsResource;
	MP_WSTRING wsBookmark;
	String2StringMap* pParamMap;

	MP_WSTRING wsProtocolDefault;
	MP_WSTRING wsHostDefault;
	unsigned short nPortDefault;
	MP_WSTRING wsResourceDefault;
};
#endif // CURLPARSER_H
