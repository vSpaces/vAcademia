
#include "StdAfx.h"
#include "IniFile.h"
#include "windows.h"
#include "HelperFunctions.h"
#include <atlconv.h>

CIniFile::CIniFile(void)
#ifndef LAUNCHER_SLN
:	MP_WSTRING_INIT(m_fileName)
#endif
{
}

CIniFile::CIniFile(std::wstring name)
#ifndef LAUNCHER_SLN
:	MP_WSTRING_INIT(m_fileName)
#endif
{
	m_fileName = name;
}

void CIniFile::SetName(std::wstring name)
{
	m_fileName = name;
}

std::string CIniFile::GetStringWithDefault(std::string section, std::string key, std::string defaultValue)
{
	wchar_t tmp[102];

	USES_CONVERSION;
	GetPrivateProfileStringW( A2W(section.data()), A2W(key.data()), A2W(defaultValue.data()), (LPWSTR)&tmp, 100, m_fileName.data());

	return W2A(tmp);
}

std::string CIniFile::GetString(std::string section, std::string key)
{
	wchar_t tmp[102];
	USES_CONVERSION;
	GetPrivateProfileStringW(  A2W(section.data()),  A2W(key.data()), L"", (LPWSTR)&tmp, 100, m_fileName.data());

	return W2A(tmp);
}

std::wstring CIniFile::GetString(std::wstring section, std::wstring key)
{
	wchar_t tmp[102];
	USES_CONVERSION;
	GetPrivateProfileStringW(  section.data(),  key.data(), L"", (LPWSTR)&tmp, 100, m_fileName.data());

	return std::wstring(tmp);
}

#ifndef LAUNCHER_SLN
int CIniFile::GetInt(std::string section, std::string key)
{
	return rtl_atoi(GetString(section, key).data());
}

float CIniFile::GetFloat(std::string section, std::string key)
{
	return StringToFloat(GetString(section, key).data());
}

bool CIniFile::GetBool(std::string section, std::string key)
{
	std::string tmp = GetString(section, key).data();

	bool res = (tmp == "true");

	return res;
}
#endif

void CIniFile::SetString(std::string section, std::string key, std::string value)
{
	USES_CONVERSION;
	WritePrivateProfileStringW(  A2W(section.data()),  A2W(key.data()),  A2W(value.data()), m_fileName.data());	 
}

#ifndef LAUNCHER_SLN
void CIniFile::SetInt(std::string section, std::string key, int value)
{
	SetString(section, key, IntToStr(value));
}

void CIniFile::SetFloat(std::string section, std::string key, float value)
{
	SetString(section, key, FloatToStr(value));
}

void CIniFile::SetBool(std::string section, std::string key, bool value)
{
	SetString(section, key, value ? "true" : "false");
}
#endif

CIniFile::~CIniFile(void)
{
}