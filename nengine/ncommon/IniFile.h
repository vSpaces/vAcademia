#pragma once

#ifdef LAUNCHER_SLN
#define MP_STRING std::string
#define MP_WSTRING std::wstring
#else
#include "CommonCommonHeader.h"
#endif


#include <string>

// Class for representation INI-file
class CIniFile
{
public:
	CIniFile(void);
	CIniFile(std::wstring name);
	~CIniFile(void);

	// Set Ini-file name
	void SetName(std::wstring name);

	// Return string from ini-file from given section and with given key
	std::string GetString(std::string section, std::string key);
	std::wstring GetString(std::wstring section, std::wstring key);
	std::string GetStringWithDefault(std::string section, std::string key, std::string defaultValue);

#ifndef LAUNCHER_SLN
	// Return int from ini-file from given section and with given key
	int GetInt(std::string section, std::string key);

	// Return float from ini-file from given section and with given key
	float GetFloat(std::string section, std::string key);

	// Return bool from ini-file from given section and with given key
	bool GetBool(std::string section, std::string key);
#endif

	// Set string for ini-value from given section and with given key
	void SetString(std::string section, std::string key, std::string value);

#ifndef LAUNCHER_SLN
	// Set int for ini-value from given section and with given key
	void SetInt(std::string section, std::string key, int value);

	// Set float for ini-value from given section and with given key
	void SetFloat(std::string section, std::string key, float value);

	// Set bool for ini-value from given section and with given key
	void SetBool(std::string section, std::string key, bool value);
#endif

private:
	// Ini-file name
	MP_WSTRING m_fileName;
};
