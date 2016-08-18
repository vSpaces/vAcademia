
#pragma once

#include "../CommonRenderManagerHeader.h"

#include "IDefaultSettingValueProvider.h"
#include "ILogWriter.h"
#include <string>
#include <map>
#include <vector>

typedef struct _SSettingWithDelay
{
	unsigned int settingID;
	MP_STRING setting;
	unsigned int delay;
	unsigned int startTime;

	_SSettingWithDelay(unsigned int _settingID, std::string _setting, unsigned int _delay, unsigned int _startTime);	
} SSettingWithDelay;

class CUserSettings
{
public:
	CUserSettings();
	~CUserSettings();

	void SetUserStringID(const wchar_t* apwcUserStringID);
	void SetLogWriter(ILogWriter* apLogWriter);
	void SetIsOfficial(bool abIsOfficial);

	void LoadAll();
	void LoadGlobal();

	std::string GetSetting(const unsigned int settingID)const;
	std::string GetSetting(const std::string& key)const;
	//std::string GetPassSetting(const std::string& key);

	bool SaveSetting(const unsigned int settingID, const std::string& setting, bool abSaveToFile = false);
	bool SaveSetting(const std::string& name, const std::string& setting, bool abSaveToFile = false);
	//bool SavePassSetting(const std::string& name, std::string& setting, bool abSaveToFile);

	void SaveSettingWithDelay(const unsigned int settingID, const std::string& setting, unsigned int delay);
	void SaveSettingWithDelay(const std::string& name,const std::string& setting, unsigned int delay);

	void AddDefaultSettingValueProvider(IDefaultSettingValueProvider* valueProvider);

	void Update();

	void SaveDelayedSettingsImmediately();
	bool SaveGlobalSettings();
	bool SaveAllExludeGlobalSettings();
	bool SaveAll();

private:
	bool SaveAll( bool abGlobalSave);
	bool SaveAll(const std::wstring& fileName, bool bGlobal = false);

	void InitProperties();

	std::wstring GetUserSettingsPath();
	std::wstring GetDefaultUserSettingsPath();
	std::wstring GetGlobalUserSettingsPath();
	int GetSettingID(const std::string& key)const;
	void xorString( std::string &sString);

	std::string GetDefaultSettingValue(const unsigned int settingID)const;

	MP_STRING_DC m_settingsName[SETTING_COUNT + SETTING_GLOBAL_COUNT];
	MP_STRING_DC m_settings[SETTING_COUNT + SETTING_GLOBAL_COUNT];
	MP_WSTRING m_userSettingsPath;
	MP_WSTRING m_globalUserSettingsPath;
	MP_WSTRING m_defaultUserSettingsPath;
	MP_WSTRING m_userStringID;
	MP_WSTRING m_appRootDir;
	MP_MAP<MP_STRING, int>	settingsMap;
	ILogWriter* mpLogWriter;
	bool		m_bOfficial;

	MP_VECTOR<SSettingWithDelay *> m_settingWithDelay;

	MP_VECTOR<IDefaultSettingValueProvider *> m_defaultValueProviders;
	int xorKey;
};