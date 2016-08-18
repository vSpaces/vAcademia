#include "StdAfx.h"
#include "ProxyFinder.h"

CProxyFinder::CProxyFinder(void)
{
}

CProxyFinder::~CProxyFinder(void)
{
}

bool CProxyFinder::getIEProxySettings(IProxySettings* proxySettings, ProxyType aType)
{
	if (proxySettings == NULL)
		return false;

	proxySettings->setType( (unsigned short)aType);
	proxySettings->setName(L"system");
	proxySettings->setEnabled(true);

	USES_CONVERSION;

	HKEY hKey;
	if( RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, KEY_READ, &hKey)==ERROR_SUCCESS && hKey!=0)
	{
		DWORD dwEnabledType = REG_DWORD;
		DWORD dwEnabledSize = 4;

		char* lpEnabledData = MP_NEW_ARR(char, dwEnabledSize);// Declare the buffer

		LONG lResEnable = RegQueryValueEx( hKey, "ProxyEnable", 0, &dwEnabledType, (BYTE*)lpEnabledData, &dwEnabledSize);
		if(lResEnable != ERROR_SUCCESS)
			return false;

		//proxySettings->enabled = (DWORD)(*lpEnabledData); // - игнорируем используется он в системе или нет
		MP_DELETE_ARR(lpEnabledData);

		DWORD dwType = REG_SZ;
		DWORD dwSize = 1024;

		char* lpData = MP_NEW_ARR(char, dwSize);// Declare the buffer


		LONG lResProxy = RegQueryValueEx( hKey, "ProxyServer", 0, &dwType, (BYTE*)lpData, &dwSize);
		if(lResProxy != ERROR_SUCCESS)
			return false;

		string proxy = lpData;
		MP_DELETE_ARR(lpData);

		if (proxy == "")
			return false;

		int pos = proxy.find("=");
		if (pos == std::string::npos) // прокси один на все протоколы
		{
			int pos = proxy.find(":");
			if (pos < 0)
				return false;

			proxySettings->setIP(A2W(proxy.substr(0, pos).c_str()));
			string port = proxy.substr(pos + 1, proxy.length() - pos - 1);

			unsigned short iPort;
			sscanf_s(port.c_str(), "%hu", &iPort);
			proxySettings->setPort(iPort);
		}
		else
		{
			pos = std::string::npos;

			if (aType == PT_HTTP)
				pos = proxy.find("http=");
			else if (aType == PT_HTTPS)
				pos = proxy.find("https=");
			else if (aType == PT_SOCKS4 || aType == PT_SOCKS5)
				pos = proxy.find("socks=");

			if (pos == std::string::npos)
				return false;

			if (aType == PT_HTTP)
				pos += 5;
			else if (aType == PT_HTTPS)
				pos += 6;
			else if (aType == PT_SOCKS4 || aType == PT_SOCKS5)
				pos += 6;


			int pos2 = proxy.find(":", pos);
			if (pos2 == std::string::npos)
				return false;

			proxySettings->setIP( A2W(proxy.substr(pos, pos2 - pos).c_str()));
			pos = pos2;

			pos2 = proxy.find(";", pos);
			if (pos2 == std::string::npos)
				pos2 = proxy.length();

			string port = proxy.substr(pos + 1, pos2 - pos - 1);

			unsigned short iPort;
			sscanf_s(port.c_str(), "%hu", &iPort);
			proxySettings->setPort(iPort);
		}

		RegCloseKey( hKey);

		proxySettings->setType( (unsigned short)aType);
	}

	return true;
}

bool CProxyFinder::getFirefoxProxySettings(IProxySettings* proxySettings, ProxyType aType)
{
	std::vector<IProxySettings*> proxySettingProfileList;
	getFirefoxProxySettingsForAllProfiles(&proxySettingProfileList, aType);
	if (proxySettingProfileList.empty())
		return false;

	*proxySettings = *(proxySettingProfileList[0]);
	return true;
}

bool CProxyFinder::getFirefoxProxySettingsForAllProfiles(std::vector<IProxySettings*>* proxySettingList, ProxyType aType)
{
	if (proxySettingList == NULL)
		return false;

	USES_CONVERSION;

	DWORD dwSize = 1024;
	wchar_t* lpData = MP_NEW_ARR(wchar_t, dwSize);// Declare the buffer
	
	if (!SHGetSpecialFolderPathW(NULL, lpData, CSIDL_APPDATA, NULL))
		return false;

	wstring firefoxAppDataPath = lpData;
	firefoxAppDataPath += L"\\Mozilla\\Firefox\\";

	MP_DELETE_ARR(lpData);

	wstring iniFilePath = firefoxAppDataPath + L"profiles.ini";

	CIniFile iniFile(iniFilePath.c_str());

	string sType = "";
	if (aType == PT_HTTP)
		sType = "http";
	else if ( aType == PT_HTTPS)
		sType = "ssl";
	else if ( aType == PT_SOCKS4 || aType == PT_SOCKS5)
		sType = "socks";


	for (int i = 0; i < 256; ++i)
	{
		ProxySettings proxySettings;
		proxySettings.type = (unsigned short)aType;
		proxySettings.enabled = true;

		proxySettings.name = L"firefox";
		wchar_t buf[50];
        _itow_s(i, buf, 50, 10);
		proxySettings.name += buf;

		wchar_t num[50];
		_itow_s(i,num,50,10);
		wstring profile = L"Profile";
		profile += num;
		wstring profilePath = iniFile.GetString(profile.c_str(), L"Path");
		if (profilePath != L"")
		{
			int ps = 0;
			while((ps = profilePath.find(L"/",ps)) != std::wstring::npos){
				profilePath.replace(ps, 1, L"\\");
			}

			USES_CONVERSION;
			wstring configFilePath = firefoxAppDataPath + profilePath.c_str() + L"\\prefs.js";

			ifstream configFileStream;
			configFileStream.open(configFilePath.c_str(), std::ios::in);
			if (!configFileStream.good())
			{
				configFileStream.close();
				continue;
			}

			string line = "";
			int pos = 0;
			while (!configFileStream.eof())
			{
				if (!getline(configFileStream, line))
				{
					configFileStream.close();
					break;
				}

				// http host
				pos = line.find("user_pref(\"network.proxy." + sType + "\"");
				if (pos!=-1)
				{
					pos = line.find(", \"") + 3;
					int pos2 = line.find("\"", pos);
					//proxySettings.ip = A2W(line.substr(pos, pos2 - pos).c_str()); 
					proxySettings.ip = CharToWide(line.substr(pos, pos2 - pos).c_str());
				}

				// http port
				pos = line.find("user_pref(\"network.proxy." + sType + "_port\"");
				if (pos!=-1)
				{
					pos = line.find(", ") + 2;
					int pos2 = line.find(")", pos);
					string port = line.substr(pos, pos2 - pos);
					proxySettings.port = (unsigned short)rtl_atoi(port.c_str());
				}

			}
		}
		else
			break;

		if (proxySettings.ip != L"" && proxySettings.port != 0)
		{
			ProxySettings * pProxySettings;
			MP_NEW_V(pProxySettings, ProxySettings, (IProxySettings*)&proxySettings);
			proxySettings.type = (unsigned short)aType;
			if ( !addProxySettingsToList(proxySettingList, pProxySettings))
				pProxySettings->destroy();
		}
	}

	if (!proxySettingList->empty())
		return true;

	return false;
}

bool CProxyFinder::getOperaProxySettings(IProxySettings* proxySettings, ProxyType aType)
{
	if (proxySettings == NULL)
		return false;

	proxySettings->setType((unsigned short)aType);
	proxySettings->setName(L"opera");
	proxySettings->setEnabled(true);

	USES_CONVERSION;

	DWORD dwSize = 1024;
	wchar_t* lpData = MP_NEW_ARR(wchar_t, dwSize);// Declare the buffer

	if (!SHGetSpecialFolderPathW(NULL, lpData, CSIDL_APPDATA, NULL))
		return false;

	wstring operaAppDataPath = lpData;
	operaAppDataPath += L"\\Opera\\Opera\\";

	MP_DELETE_ARR(lpData);

	wstring iniFilePath = operaAppDataPath + L"operaprefs.ini";

	CIniFile iniFile(iniFilePath.c_str());

	string proxy = "";
	if (aType == PT_HTTP)
		proxy = iniFile.GetString("Proxy", "HTTP server");
	else if (aType == PT_HTTPS)
		proxy = iniFile.GetString("Proxy", "HTTPS server");
	else if (aType == PT_SOCKS4 || aType == PT_SOCKS5)
		proxy = iniFile.GetString("Proxy", "SOCKS server");

	if (proxy != "")
	{
		int pos = proxy.find(":");
		if (pos <= 0)
			return false;

		proxySettings->setIP( A2W(proxy.substr(0, pos).c_str()));
		string port = proxy.substr(pos + 1, proxy.length() - pos - 1);

		unsigned short iPort;
		sscanf_s(port.c_str(), "%hu", &iPort);
		proxySettings->setPort(iPort);

		proxySettings->setType( (unsigned short)aType);
		return true;
	}

	return false;
}

void CProxyFinder::getAllProxySettings(std::vector<IProxySettings*>* proxySettingsList, ProxyType aType)
{
	if (proxySettingsList == NULL)
		return;

	getFirefoxProxySettingsForAllProfiles(proxySettingsList, aType);

	IProxySettings* operaProxySettings = MP_NEW(ProxySettings);
	if (getOperaProxySettings(operaProxySettings, aType))
		addProxySettingsToList(proxySettingsList, operaProxySettings);
	else
		operaProxySettings->destroy();

	IProxySettings* ieProxySettings = MP_NEW(ProxySettings);
	if (getIEProxySettings(ieProxySettings, aType))
		addProxySettingsToList(proxySettingsList, ieProxySettings);
	else
		ieProxySettings->destroy();
}

bool CProxyFinder::addProxySettingsToList(std::vector<IProxySettings*>* aProxySettingsList, IProxySettings* aProxySettings)
{
	if (aProxySettings == NULL || aProxySettingsList == NULL)
		return false;

	bool isExist = false;
	std::vector<IProxySettings*>::iterator it = aProxySettingsList->begin();
	std::vector<IProxySettings*>::iterator end = aProxySettingsList->end();
	for (; it != end; ++it)
	{
		if ((IProxySettings*)(*it)->isEqual(aProxySettings))
		{
			isExist = true;
			break;
		}
	}

	if (!isExist)
	{
		aProxySettingsList->push_back(aProxySettings);
		return true;
	}

	return false;
}
