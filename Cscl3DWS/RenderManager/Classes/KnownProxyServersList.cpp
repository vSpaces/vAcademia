
#include "StdAfx.h"
#include "XMLList.h"
#include "ProxyFinder.h"
#include "PlatformDependent.h"
#include "KnownProxyServersList.h"

_SProxySettings::_SProxySettings(const _SProxySettings& other):
	MP_STRING_INIT(host),
	MP_STRING_INIT(port),
	MP_STRING_INIT(user),
	MP_STRING_INIT(pass)
{
	host = other.host;
	port = other.port;
	user = other.user;
	pass = other.pass;
}

_SProxySettings::_SProxySettings(const std::string& _host, const std::string& _port, const std::string& _user,
		const std::string& _pass):
	MP_STRING_INIT(host),
	MP_STRING_INIT(port),
	MP_STRING_INIT(user),
	MP_STRING_INIT(pass)
{
	host = _host;
	port = _port;
	user = _user;
	pass = _pass;
}

CKnownProxyServersList::CKnownProxyServersList():
	MP_VECTOR_INIT(m_servers)
{
	std::vector<std::string> fields;
	fields.push_back("port");
	fields.push_back("user");
	fields.push_back("pass");

	std::wstring fullPath = GetApplicationRootDirectory();
	fullPath += L"known_proxy_servers.xml";

	CXMLList proxyList(fullPath, fields, false, true);
	std::string host, port, user, pass;
	while (proxyList.GetNextValue(host))
	{
		if (!proxyList.GetOptionalFieldValue("port", port))
		{
			continue;
		}

		if (!proxyList.GetOptionalFieldValue("user", user))
		{
			continue;
		}

		if (!proxyList.GetOptionalFieldValue("pass", pass))
		{
			continue;
		}

		SProxySettings proxySettings(host, port, user, pass);

		m_servers.push_back(proxySettings);
	}
}

bool CKnownProxyServersList::IsKnownProxyServer(std::string& userName, std::string& password, std::string& host, std::string& port)
{
	CProxyFinder proxyFinder;
	std::vector<IProxySettings*> httpProxySettingsList;
	proxyFinder.getAllProxySettings(&httpProxySettingsList, PT_HTTP);

	if (httpProxySettingsList.size() > 0)
	{
		IProxySettings* settings = httpProxySettingsList[0];
		char* str = WideToChar(settings->getIP());
		std::string _host = str;
		MP_DELETE_ARR(str);

		MP_VECTOR<SProxySettings>::iterator it = m_servers.begin();
		MP_VECTOR<SProxySettings>::iterator itEnd = m_servers.end();

		for ( ; it != itEnd; it++)
		if (_host == (*it).host)
		{
			host = (*it).host;
			port = (*it).port;
			userName = (*it).user;
			password = (*it).pass;

			return true;
		}
	}

	return false;
}

CKnownProxyServersList::~CKnownProxyServersList()
{
}