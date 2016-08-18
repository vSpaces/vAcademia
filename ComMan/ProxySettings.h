#pragma once

#include "MemoryProfilerCreate.h"
#include <stdio.h>
#include <string>
#include <vector>
#include "atlconv.h"

using namespace std;

typedef enum ProxyType
{
	PT_UNKNOWN = 0,
	PT_HTTP,
	PT_HTTPS,
	PT_SOCKS4,
	PT_SOCKS5
};

typedef enum ProxyMode
{
	PM_NO_PROXY = 0,
	PM_SYSTEM,
	PM_MANUAL
	
};

struct ProxySettingsExternal
{
	unsigned short	port;
	unsigned short	type;
	bool			enabled;
	bool			useUser;
	const wchar_t*		name;
	const wchar_t*		ip;
	const wchar_t*		user;
	const wchar_t*		password;

	ProxySettingsExternal()
	{
		name = NULL;
		ip = NULL;
		user = NULL;
		password = NULL;

		port = 0;
		type = 0;
		enabled = false;
		useUser = false;
	}
};

struct IProxySettings
{
	virtual unsigned short	getPort() = 0;
	virtual unsigned short	getType() = 0;
	virtual bool			isEnabled() = 0;
	virtual bool			isUseUser() = 0;
	virtual wchar_t*		getName() = 0;
	virtual wchar_t*		getIP() = 0;
	virtual wchar_t*		getUser() = 0;
	virtual wchar_t*		getPassword() = 0;
	virtual bool			isTunnel() = 0;

	virtual void			setPort(unsigned short aPort) = 0;
	virtual void			setType(unsigned short aType) = 0;
	virtual void			setEnabled(bool aEnabled) = 0;
	virtual void			setUseUser(bool aUseUser) = 0;
	virtual void			setName(wchar_t* aName) = 0;
	virtual void			setIP(wchar_t* aIP) = 0;
	virtual void			setUser(wchar_t* aUser) = 0;
	virtual void			setPassword(wchar_t* aPassword) = 0;
	virtual void			setTunnel(bool aTunnel) = 0;

	virtual string			toString() = 0;
	virtual void			clear() = 0;
	virtual void			destroy() = 0;
	virtual bool			isEqual(IProxySettings* aProxySettings) = 0;
};

struct ProxySettings: public IProxySettings
{
	unsigned short	port;
	unsigned short	type;
	bool			enabled;
	bool			useUser;
	MP_WSTRING		name;
	MP_WSTRING		ip;
	MP_WSTRING		user;
	MP_WSTRING		password;
	bool			tunnel;

	ProxySettings() :
		MP_WSTRING_INIT(name),
		MP_WSTRING_INIT(ip),
		MP_WSTRING_INIT(user),
		MP_WSTRING_INIT(password)
	{
		clear();
	}

	ProxySettings(IProxySettings* aProxySettings) :
	MP_WSTRING_INIT(name),
		MP_WSTRING_INIT(ip),
		MP_WSTRING_INIT(user),
		MP_WSTRING_INIT(password)
	{
		this->port = aProxySettings->getPort();
		this->type = aProxySettings->getType();
		this->enabled = aProxySettings->isEnabled();
		this->useUser = aProxySettings->isUseUser();
		this->name = aProxySettings->getName();
		this->ip = aProxySettings->getIP();
		this->user = aProxySettings->getUser();
		this->password = aProxySettings->getPassword();
		this->tunnel = aProxySettings->isTunnel();
	}

	virtual unsigned short getPort()
	{
		return port;
	}

	virtual unsigned short getType()
	{
        return type;
	}

	bool isEnabled()
	{
		return enabled;
	}

	virtual bool isUseUser()
	{
		return useUser;
	}

	virtual wchar_t* getName()
	{
		return (wchar_t*)name.c_str();
	}

	virtual wchar_t* getIP()
	{
		return (wchar_t*)ip.c_str();
	}

	virtual wchar_t* getUser()
	{
		return (wchar_t*)user.c_str();
	}

	virtual wchar_t* getPassword()
	{
		return (wchar_t*)password.c_str();
	}

	virtual bool isTunnel()
	{
        return tunnel;
	}

	virtual void setPort(unsigned short aPort)
	{
		port = aPort;
	}

	virtual void setType(unsigned short aType)
	{
        type = aType;
	}

	virtual void setEnabled(bool aEnabled)
	{
		enabled = aEnabled;
	}

	virtual void setUseUser(bool aUseUser)
	{
		useUser = aUseUser;
	}

	virtual void setName(wchar_t* aName)
	{
        name = aName;
	}

	virtual void setIP(wchar_t* aIP)
	{
		ip = aIP;
	}

	virtual void setUser(wchar_t* aUser)
	{
		user = aUser;
	}
	
	virtual void setPassword(wchar_t* aPassword)
	{
		password = aPassword;
	}

	virtual void setTunnel(bool aTunnel)
	{
		tunnel = aTunnel;
	}

	virtual bool isEqual(IProxySettings* aProxySettings)
	{
		return (!wcscmp( this->getIP(), aProxySettings->getIP())) && this->getPort() == aProxySettings->getPort();
	}

	virtual void ProxySettings::clear()
	{
		port = 0;
		type = PT_UNKNOWN;
		enabled = false;
		useUser = false;
		name = L"no_proxy";
		ip = L"";
		user = L"";
		password = L"";
		tunnel = false;
	}

	virtual string ProxySettings::toString()
	{
		USES_CONVERSION;
		char buf[6];
		sprintf_s(&buf[0], 6, "%hu", port);
		string result = W2A(name.c_str());
		result += " ";
		result += W2A(ip.c_str());
		result += ":";
		result += buf;
		result += " used:";
		result += (enabled ? "true" : "false");
		result += " user:";
		result += W2A(user.c_str());
		result += " password: ";
		result += (password.length() > 0 ? "exist" : "not exist");
		result += " tunnel:";
		result += (tunnel ? "true" : "false");

		return result;
	}

	virtual void destroy()
	{
		MP_DELETE_THIS;
	}
};

typedef MP_VECTOR<IProxySettings*> ProxySettingsVec;

struct ProxySettingsList
{
	static void ProxySettingsList::clearProxySettingsList(std::vector<IProxySettings*>* aProxySettingsList)
	{
		ProxySettingsVec::iterator it = aProxySettingsList->begin();
		ProxySettingsVec::iterator end = aProxySettingsList->end();
		for ( ; it != end; ++it)
		{
			ProxySettings* proxySettings = (ProxySettings*)(*it);
			proxySettings->destroy();
		}

		aProxySettingsList->clear();
	}

	static void ProxySettingsList::copyProxySettingsList(std::vector<IProxySettings*>* aSourceList, std::vector<IProxySettings*>* aDestList)
	{
		clearProxySettingsList(aDestList);

		ProxySettingsVec::iterator it = aSourceList->begin();
		ProxySettingsVec::iterator end = aSourceList->end();
		for ( ; it != end; ++it)
		{
			ProxySettings* copyProxySettings;
			MP_NEW_V(copyProxySettings, ProxySettings, (ProxySettings*)(*it));
			aDestList->push_back(copyProxySettings);
		}
	}
};