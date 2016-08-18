#pragma once

#include "ProxySettings.h"
#include <ShellAPI.h>
#include <shlobj.h>
#include "IniFile.h"
#include <fstream>
#include <vector>
#include "StringFunctions.h"

using namespace std;

class CProxyFinder
{
public:
	CProxyFinder(void);
	~CProxyFinder(void);

public:
	bool getIEProxySettings(IProxySettings* proxySettings, ProxyType aType);
	bool getOperaProxySettings(IProxySettings* proxySettings, ProxyType aType);
	bool getFirefoxProxySettings(IProxySettings* proxySettings, ProxyType aType);

	void getAllProxySettings(std::vector<IProxySettings*>* proxySettingsList, ProxyType aType);

private:
	bool getFirefoxProxySettingsForAllProfiles(std::vector<IProxySettings*>* proxySettingsList, ProxyType aType);
	bool addProxySettingsToList(std::vector<IProxySettings*>* aProxySettingsList, IProxySettings* aProxySettings);
};
