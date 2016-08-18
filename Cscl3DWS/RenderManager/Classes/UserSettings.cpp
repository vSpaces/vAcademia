
#include "StdAfx.h"
#include "IDefaultSettingValueProvider.h"
#include "UserSettings.h"
#include "XMLPropertiesList.h"
#include "PlatformDependent.h"
#include <atlconv.h>
#include <assert.h>

#ifdef MAP_MANAGER
	#include "GlobalSingletonStorage.h"
#endif

#define DEF_SETTINGS_START	void CUserSettings::InitProperties() { 
#define DEF_SETTINGS_NAME(x, y)		\
					m_settingsName[##x] = ##y ;		\
					settingsMap.insert( MP_MAP<MP_STRING, int>::value_type( MAKE_MP_STRING(##y), ##x));			
#define DEF_SETTINGS_DEFAULT_VALUE(x, y) m_settings[##x] = MAKE_MP_STRING(##y) ;
#define DEF_SETTINGS_END  }

	DEF_SETTINGS_START
		DEF_SETTINGS_NAME(USER_SETTING_QUALITY_MODE, "quality_mode");
		DEF_SETTINGS_NAME(USER_SETTING_VSYNC_ENABLED, "vsync");
		DEF_SETTINGS_NAME(USER_SETTING_VOICE_ENABLED, "voice");
		DEF_SETTINGS_NAME(USER_SETTING_V2DSOUND_ENABLED, "v2dsound");
		DEF_SETTINGS_NAME(USER_SETTING_V3DSOUND_ENABLED, "v3dsound2");
		DEF_SETTINGS_NAME(USER_SETTING_SCREEN_WIDTH, "screen_width");
		DEF_SETTINGS_NAME(USER_SETTING_SCREEN_HEIGHT, "screen_height");
		DEF_SETTINGS_DEFAULT_VALUE(USER_SETTING_VSYNC_ENABLED, "false");
		DEF_SETTINGS_NAME(USER_SETTING_ONLINE_EVENTS_INFO, "online_events_info");
		DEF_SETTINGS_NAME(USER_SETTING_HIHT_WINDOW_SHOW, "hint_window_show");		
		DEF_SETTINGS_NAME(USER_SETTING_AUDIO_AUTO_GAIN_CONTROL_ENABLED, "autoGainControlEnabled");
		DEF_SETTINGS_NAME(USER_SETTING_AUDIO_DENOISING_ENABLED, "denoisingEnabled");
		DEF_SETTINGS_NAME(USER_SETTING_AUDIO_INPUT_DEVICE_GUID, "inputDeviceGuid");
		DEF_SETTINGS_NAME(USER_SETTING_AUDIO_INPUT_DEVICE_NAME, "inputDeviceName");
		DEF_SETTINGS_NAME(USER_SETTING_AUDIO_INPUT_DEVICE_LINE, "inputDeviceLine");
		DEF_SETTINGS_NAME(USER_SETTING_AUDIO_INPUT_MIX_DEVICE_GUID, "inputMixDeviceGuid");
		DEF_SETTINGS_NAME(USER_SETTING_AUDIO_INPUT_MIX_DEVICE_NAME, "inputMixDeviceName");
		DEF_SETTINGS_NAME(USER_SETTING_AUDIO_INPUT_MIX_DEVICE_LINE, "inputMixDeviceLine");
		DEF_SETTINGS_NAME(USER_SETTING_AUDIO_OUTPUT_DEVICE_GUID, "outputDeviceGuid");
		DEF_SETTINGS_NAME(USER_SETTING_AUDIO_OUTPUT_DEVICE_NAME, "outputDeviceName");
		DEF_SETTINGS_NAME(USER_SETTING_AUDIO_OUTPUT_DEVICE_LINE, "outputDeviceLine");
		DEF_SETTINGS_NAME(USER_SETTING_AUDIO_IN_MODE, "inAudioMode");
		DEF_SETTINGS_NAME(USER_SETTING_AUDIO_IN_VOLUME_GAIN, "usersVolumeGain");
		DEF_SETTINGS_NAME(USER_SETTING_AUDIO_AUTO_VOLUME, "autoVolume");
		DEF_SETTINGS_NAME(USER_SETTING_AUDIO_VOICE_GAIN_LEVEL, "voiceGainLevelDb");
		DEF_SETTINGS_NAME(USER_SETTING_AUDIO_VOICE_ACTIVATION_LEVEL, "voiceActivationLevel");
		DEF_SETTINGS_NAME(USER_SETTING_AUDIO_VOICE_QUALITY, "voiceQuality");
		DEF_SETTINGS_NAME(USER_SETTING_AUDIO_HANDS_FREE_ENABLED, "handsFreeEnabled");
		DEF_SETTINGS_NAME(USER_SETTING_DISTANCE_KOEF, "distanceKoef");
		DEF_SETTINGS_NAME(USER_SETTING_AUDIO_BACK_VOLUME, "backMusicVolume");
		DEF_SETTINGS_NAME(USER_SETTING_AUDIO_EFFECTS_VOLUME, "effectsAudioVolume");
		DEF_SETTINGS_NAME(USER_SETTING_NAVIGATION_A_LA_SL_ENABLED, "navigationSL");
		DEF_SETTINGS_NAME(USER_SETTING_MULTISAMPLE_POWER, "multisample_power");	
		DEF_SETTINGS_NAME(USER_SETTING_ALLOW_AVATAR_MOVE_BY_MOUSE, "allow_avatar_move_by_mouse");
		DEF_SETTINGS_DEFAULT_VALUE(USER_SETTING_MULTISAMPLE_POWER, "1");
		DEF_SETTINGS_NAME(USER_SETTING_SHOW_CLOUDS, "show_clouds");
		DEF_SETTINGS_DEFAULT_VALUE(USER_SETTING_SHOW_CLOUDS, "true");
		DEF_SETTINGS_NAME(USER_SETTING_LOGIN, "login");
		DEF_SETTINGS_NAME(USER_SETTING_PASSWORD, "password");
		DEF_SETTINGS_NAME(USER_SETTING_PROXY_MODE, "proxy_mode");
		DEF_SETTINGS_NAME(USER_SETTING_PROXY_ADDRESS, "proxy_address");
		DEF_SETTINGS_NAME(USER_SETTING_PROXY_PORT, "proxy_port");
		DEF_SETTINGS_NAME(USER_SETTING_PROXY_USE_USER, "proxy_use_user");
		DEF_SETTINGS_NAME(USER_SETTING_PROXY_USERNAME, "proxy_username");
		DEF_SETTINGS_NAME(USER_SETTING_PROXY_PASS, "proxy_pass");
		DEF_SETTINGS_NAME(USER_SETTING_SOCKS_MODE, "socks_mode");
		DEF_SETTINGS_NAME(USER_SETTING_SOCKS_ADDRESS, "socks_address");
		DEF_SETTINGS_NAME(USER_SETTING_SOCKS_PORT, "socks_port");
		DEF_SETTINGS_NAME(USER_SETTING_SOCKS_USE_USER, "socks_use_user");
		DEF_SETTINGS_NAME(USER_SETTING_SOCKS_USERNAME, "socks_username");
		DEF_SETTINGS_NAME(USER_SETTING_SOCKS_PASS, "socks_pass");
		DEF_SETTINGS_NAME(USER_SETTING_SOCKS_VER, "socks_ver");
		DEF_SETTINGS_NAME(USER_SETTING_SLIDE_SHOW, "showSlideShow");
		DEF_SETTINGS_NAME(USER_SETTING_DISABLE_BETADRIVER, "disable_betadriver");
		DEF_SETTINGS_NAME(USER_SETTING_DISABLE_OLDDRIVER, "disable_olddriver");
		DEF_SETTINGS_NAME(USER_SETTING_DISABLE_MINSYSREQ, "disable_minsysreq");
		DEF_SETTINGS_NAME(USER_SETTING_HTTPS_MODE, "https_mode");
		DEF_SETTINGS_NAME(USER_SETTING_HTTPS_ADDRESS, "https_address");
		DEF_SETTINGS_NAME(USER_SETTING_HTTPS_PORT, "https_port");
		DEF_SETTINGS_NAME(USER_SETTING_HTTPS_USE_USER, "https_use_user");
		DEF_SETTINGS_NAME(USER_SETTING_HTTPS_USERNAME, "https_username");
		DEF_SETTINGS_NAME(USER_SETTING_HTTPS_PASS, "https_pass");
		DEF_SETTINGS_NAME(USER_SETTING_LS_ADDRESS, "ls_addres");
		DEF_SETTINGS_NAME(USER_SETTING_LS_PORT, "ls_port");
		DEF_SETTINGS_NAME(USER_SETTING_LS_USE_USER, "ls_use_user");
		DEF_SETTINGS_NAME(USER_SETTING_LS_USERNAME, "ls_username");
		DEF_SETTINGS_NAME(USER_SETTING_LS_PASSWORD, "ls_pass");
		DEF_SETTINGS_NAME(USER_SETTING_LS_TYPE, "ls_type");
		DEF_SETTINGS_NAME(USER_SETTING_LS_TUNNEL, "ls_tunnel");
		DEF_SETTINGS_NAME(USER_SETTING_LS_ENABLED, "ls_enabled");
		DEF_SETTINGS_NAME(USER_SETTING_LS_NAME, "ls_name");
		DEF_SETTINGS_NAME(USER_SETTING_INTERACTIVEBOARD_MONITOR, "monitor_for_interactive_board");
		DEF_SETTINGS_NAME(USER_SETTING_KINECT_LEFT_CORNER_X, "kinect_left_corner_x");
		DEF_SETTINGS_DEFAULT_VALUE(USER_SETTING_KINECT_LEFT_CORNER_X, "0");
		DEF_SETTINGS_NAME(USER_SETTING_KINECT_LEFT_CORNER_Y, "kinect_left_corner_y");
		DEF_SETTINGS_DEFAULT_VALUE(USER_SETTING_KINECT_LEFT_CORNER_Y, "0");
		DEF_SETTINGS_NAME(USER_SETTING_KINECT_LEFT_CORNER_Z, "kinect_left_corner_z");
		DEF_SETTINGS_DEFAULT_VALUE(USER_SETTING_KINECT_LEFT_CORNER_Z, "0");
		DEF_SETTINGS_NAME(USER_SETTING_KINECT_RIGHT_CORNER_X, "kinect_right_corner_x");
		DEF_SETTINGS_DEFAULT_VALUE(USER_SETTING_KINECT_RIGHT_CORNER_X, "0");
		DEF_SETTINGS_NAME(USER_SETTING_KINECT_RIGHT_CORNER_Y, "kinect_right_corner_y");
		DEF_SETTINGS_DEFAULT_VALUE(USER_SETTING_KINECT_RIGHT_CORNER_Y, "0");
		DEF_SETTINGS_NAME(USER_SETTING_KINECT_RIGHT_CORNER_Z, "kinect_right_corner_z");
		DEF_SETTINGS_DEFAULT_VALUE(USER_SETTING_KINECT_RIGHT_CORNER_Z, "0");
		DEF_SETTINGS_NAME(USER_SETTING_DISPLAYID_FOR_INTERACTIVE_BOARD, "displayid_for_interactive_board");		
		DEF_SETTINGS_NAME(USER_SETTING_AUDIO_MASTER_VOLUME, "usersVolumeGainVer2");
		DEF_SETTINGS_NAME(USER_SETTING_ECHO_CANCELLATION, "echoCancellation");
	DEF_SETTINGS_END

_SSettingWithDelay::_SSettingWithDelay(unsigned int _settingID, std::string _setting, unsigned int _delay, unsigned int _startTime):
	MP_STRING_INIT(setting)
{
	settingID = _settingID;
	setting = _setting;
	delay = _delay;
	startTime = _startTime;
}

CUserSettings::CUserSettings():
	MP_WSTRING_INIT(m_userSettingsPath),
	MP_WSTRING_INIT(m_globalUserSettingsPath),
	MP_WSTRING_INIT(m_defaultUserSettingsPath),
	MP_WSTRING_INIT(m_userStringID),
	MP_WSTRING_INIT(m_appRootDir),
	MP_MAP_INIT(settingsMap),
	MP_VECTOR_INIT(m_settingWithDelay),
	MP_VECTOR_INIT(m_defaultValueProviders)
{
	for (int i = 0; i < SETTING_COUNT + SETTING_GLOBAL_COUNT; i++)
	{
		MP_STRING_DC_INIT(m_settingsName[i]);
		MP_STRING_DC_INIT(m_settings[i]);
	}

	m_appRootDir = GetApplicationDataDirectory() + L"\\Vacademia";

	CreateDirectoryW((LPCWSTR)(m_appRootDir.c_str()), NULL);

	mpLogWriter = NULL;
	xorKey = 71;
	InitProperties();

}

void CUserSettings::LoadAll()
{
	std::wstring path = GetUserSettingsPath();
#ifndef LAUNCHER
	if( mpLogWriter)
		mpLogWriter->WriteLn(L"load user settings from: ", path);
#endif
	MP_NEW_P2(list, CXMLPropertiesList, path, ERROR_IF_NOT_FOUND);
	if (!list->IsLoaded())
	{
		MP_DELETE(list);
		path = GetDefaultUserSettingsPath();
		MP_NEW_V2(list, CXMLPropertiesList, path, ERROR_IF_NOT_FOUND);		
#ifndef LAUNCHER
		if( mpLogWriter)
			mpLogWriter->WriteLn("load user settings from: ", path);
#endif
	}

	for (int k = 0; k < SETTING_COUNT; k++)
	{
		m_settings[k] = list->GetString(m_settingsName[k]);
	}

	MP_DELETE(list);
}

void CUserSettings::LoadGlobal()
{
	CXMLPropertiesList list(GetGlobalUserSettingsPath(), ERROR_IF_NOT_FOUND);
#ifndef LAUNCHER
	if( mpLogWriter)
		mpLogWriter->WriteLn("load global user settings from: ", GetGlobalUserSettingsPath());
#endif
	if (list.IsLoaded())
	{
		for (int k = 0; k < SETTING_GLOBAL_COUNT; k++)
		{
			m_settings[SETTING_COUNT + k] = list.GetString(m_settingsName[SETTING_COUNT + k]);
		}
	}	
}

std::string CUserSettings::GetDefaultSettingValue(const unsigned int settingID)const
{
	std::vector<IDefaultSettingValueProvider *>::const_iterator it = m_defaultValueProviders.begin();
	std::vector<IDefaultSettingValueProvider *>::const_iterator itEnd = m_defaultValueProviders.end();

	for ( ; it != itEnd; it++)
	{
		std::string value = (*it)->GetDefaultValue(settingID);
		if (value != "")
		{
			return value;
		}
	}

	return "";
}

std::string CUserSettings::GetSetting(const unsigned int settingID) const
{
	if (settingID < SETTING_COUNT + SETTING_GLOBAL_COUNT)
	{
		return (m_settings[settingID] == "") ? GetDefaultSettingValue(settingID) : m_settings[settingID];
	}
	else
	{
		return "";
	}
}

std::string CUserSettings::GetSetting(const std::string& key) const
{
	if (key == "is_official")
	{
		return m_bOfficial?"1":"0";
	}

	int settingID = GetSettingID(key);

	if (settingID != -1)
	{		
		return (m_settings[settingID] == "") ? GetDefaultSettingValue(settingID) : m_settings[settingID];
	}
	else
	{
		return "";
	}
}

bool CUserSettings::SaveSetting(const unsigned int settingID, const std::string& setting, bool abSaveToFile)
{
	if (settingID < SETTING_COUNT + SETTING_GLOBAL_COUNT)
	{
		m_settings[settingID] = setting;
		if( abSaveToFile)
		{
			if ( settingID < SETTING_COUNT)
				return SaveAll( false);
			else
				return SaveAll(GetGlobalUserSettingsPath(), true);
		}
		return true;
	}
	return false;
}

bool CUserSettings::SaveSetting(const std::string& name, const std::string& setting, bool abSaveToFile)
{
	int settingID = GetSettingID(name);

	if (settingID != -1)
	{
		m_settings[settingID] = setting;
		if( abSaveToFile)
		{
			if ( settingID < SETTING_COUNT)
				return SaveAll( false);
			else
				return SaveAll(GetGlobalUserSettingsPath(), true);
		}
		return true;
	}
	return false;
}

int CUserSettings::GetSettingID(const std::string& key) const
{
	MP_MAP<MP_STRING, int>::const_iterator it = settingsMap.find(MAKE_MP_STRING(key));
	if (it == settingsMap.end())
	{
		return -1;
	}

	return it->second;
}

bool CUserSettings::SaveAll( bool abSaveGlobal)
{
	bool saveResult = SaveAll(GetUserSettingsPath());
	saveResult &= SaveAll(GetDefaultUserSettingsPath());
	if( abSaveGlobal)
		saveResult &= SaveAll(GetGlobalUserSettingsPath(), true);

	return saveResult;
}

bool CUserSettings::SaveAll(const std::wstring& fileName, bool bGlobal /* = false */)
{
	if (fileName.size() == 0)
	{
		return false;
	}

	FILE* fl = NULL;
	int res = _wfopen_s(&fl, fileName.c_str(), L"wb");
	if (!fl || res != 0)
	{
		return false;
	}

	fprintf(fl, "<user_settings>\n");

	if ( !bGlobal)
	{
		for (int k = 0; k < SETTING_COUNT; k++)
		{
			fprintf(fl, "\t<user_setting name=\"%s\" value=\"%s\" />\n", m_settingsName[k].c_str(), m_settings[k].c_str());	
		}
	}
	else
	{
		for (int k = 0; k < SETTING_GLOBAL_COUNT; k++)
		{
			fprintf(fl, "\t<user_setting name=\"%s\" value=\"%s\" />\n", m_settingsName[k + SETTING_COUNT].c_str(), m_settings[k + SETTING_COUNT].c_str());	
			
		}
	}

	fprintf(fl, "</user_settings>\n");

	fclose(fl);

	return true;
}

bool CUserSettings::SaveGlobalSettings()
{
	return SaveAll(GetGlobalUserSettingsPath(), true);
}

bool CUserSettings::SaveAllExludeGlobalSettings()
{
	return SaveAll(false);
}

bool CUserSettings::SaveAll()
{
	return SaveAll(true);
}

void CUserSettings::SetIsOfficial(bool abIsOfficial)
{
	m_bOfficial = abIsOfficial;
}

void CUserSettings::SetLogWriter(ILogWriter* apLogWriter)
{
	mpLogWriter = apLogWriter;
}

void CUserSettings::SetUserStringID(const wchar_t* apwcUserStringID)
{
	if( !apwcUserStringID)
		m_userStringID = L"";
	else
	{
		m_userStringID = apwcUserStringID;
	}
}

std::wstring CUserSettings::GetUserSettingsPath()
{
	if (m_userSettingsPath.empty())
	{
		m_userSettingsPath = m_appRootDir;
		m_userSettingsPath += L"\\va_user_settings_";
		m_userSettingsPath += m_userStringID;
		m_userSettingsPath += L".xml";

		if ( m_userStringID.empty())
		{			
			if (mpLogWriter)
			{
				// ÅÑËÈ ÑÐÀÁÎÒÀÅÒ, ÇÂÀÒÜ ÆÅÍÞ ÈËÈ ÂÎÂÓ
				assert(true);
#ifndef LAUNCHER
				mpLogWriter->WriteLn("[ERROR] Login is empty");
#endif
			}
			m_userSettingsPath = L"";
			return L"";
		}
	}

	return m_userSettingsPath;
}

std::wstring CUserSettings::GetDefaultUserSettingsPath()
{
	if (m_defaultUserSettingsPath.empty())
	{
		//USES_CONVERSION;
		m_defaultUserSettingsPath = m_appRootDir;
		m_defaultUserSettingsPath += L"\\va_user_settings.xml";		
	}

	return m_defaultUserSettingsPath;
}

std::wstring CUserSettings::GetGlobalUserSettingsPath()
{
	if (m_globalUserSettingsPath.empty())
	{
		//USES_CONVERSION;
		m_globalUserSettingsPath = m_appRootDir;
		m_globalUserSettingsPath += L"\\va_global_user_settings.xml";		
	}

	return m_globalUserSettingsPath;
}

void CUserSettings::AddDefaultSettingValueProvider(IDefaultSettingValueProvider* valueProvider)
{
	m_defaultValueProviders.push_back(valueProvider);
}

void CUserSettings::SaveSettingWithDelay(const unsigned int settingID, const std::string& setting, unsigned int delay)
{
#ifdef MAP_MANAGER
	MP_NEW_P4(ds, SSettingWithDelay, settingID, setting, delay, g->ne.time);
	m_settingWithDelay.push_back(ds);
#else
	if(delay == 0 || settingID == 0 || setting.empty()) return;
#endif
}

void CUserSettings::SaveSettingWithDelay(const std::string& name,const std::string& setting, unsigned int delay)
{
#ifdef MAP_MANAGER
	SaveSettingWithDelay(GetSettingID(name), setting, delay);
#else
	if(delay == 0 || name.empty() || setting.empty()) return;
#endif
}

void CUserSettings::Update()
{
#ifdef MAP_MANAGER
	std::vector<SSettingWithDelay *>::iterator it = m_settingWithDelay.begin();
	std::vector<SSettingWithDelay *>::iterator itEnd = m_settingWithDelay.end();

	bool bRes = false;
	for ( ; it != itEnd; it++)
	if ((*it)->startTime + (*it)->delay <= g->ne.time)
	{
		bRes = SaveSetting((*it)->settingID, (*it)->setting);
		MP_DELETE(*it);
		m_settingWithDelay.erase(it);
		break;
	}
	if( bRes)
		SaveAllExludeGlobalSettings();
#endif
}

void CUserSettings::SaveDelayedSettingsImmediately()
{
	if (m_settingWithDelay.size() != 0)
	{
		std::vector<SSettingWithDelay *>::iterator it = m_settingWithDelay.begin();
		std::vector<SSettingWithDelay *>::iterator itEnd = m_settingWithDelay.end();

		for ( ; it != itEnd; it++)	
		{
			SaveSetting((*it)->settingID, (*it)->setting);
			MP_DELETE(*it);	
		}

		SaveAllExludeGlobalSettings();
		m_settingWithDelay.clear();
	}
}

void CUserSettings::xorString( std::string &aString)
{
	for (unsigned int i = 0; i < aString.size(); i++)
		aString[i] = aString[i] ^ (char)xorKey;
}

/*std::string CUserSettings::GetPassSetting(const std::string& key)
{	
	std::string sPass = GetSetting( key);
	xorString( sPass);
	return sPass.c_str();
}

bool CUserSettings::SavePassSetting(const std::string& name, std::string& setting, bool abSaveToFile)
{
	xorString( setting);
	return SaveSetting( name, setting, abSaveToFile);
}*/

CUserSettings::~CUserSettings()
{
}
