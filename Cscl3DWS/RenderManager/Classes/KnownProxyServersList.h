
#pragma once

typedef struct _SProxySettings
{
	MP_STRING host;
	MP_STRING port;
	MP_STRING user;
	MP_STRING pass;

	_SProxySettings(const _SProxySettings& other);
	_SProxySettings(const std::string& host, const std::string& port, const std::string& user,
		const std::string& pass);	
} SProxySettings;

class CKnownProxyServersList
{
public:
	CKnownProxyServersList();
	~CKnownProxyServersList();

	bool IsKnownProxyServer(std::string& userName, std::string& password, std::string& host, std::string& port);

private:
	MP_VECTOR<SProxySettings> m_servers;
};
