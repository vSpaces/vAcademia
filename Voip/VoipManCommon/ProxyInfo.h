#pragma once

struct ProxyInfo
{
	MP_WSTRING sProxyUserName;
	MP_WSTRING sProxyPassword;
	MP_WSTRING sProxyIP;
	unsigned short proxyType;

	ProxyInfo():
	MP_WSTRING_INIT(sProxyUserName)
		,MP_WSTRING_INIT(sProxyPassword)
		,MP_WSTRING_INIT(sProxyIP)
	{

	}
};