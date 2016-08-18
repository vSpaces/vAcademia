
#include "StdAfx.h"
#include "InFile.h"
#include "windows.h"
#include <string>
#include "HelperFunctions.h"

using namespace std;

CIniFile::CIniFile(void)
{
}

CIniFile::CIniFile(std::string name)
{
	m_fileName = name;
}

void CIniFile::SetName(std::string name)
{
	m_fileName = name;
}

std::string CIniFile::GetString(std::string section, std::string key)
{
	char tmp[102];

	GetPrivateProfileString((LPCSTR)section.data(), (LPCSTR)key.data(), "", (LPSTR)&tmp, 100, (LPCSTR)m_fileName.data());

	return tmp;
}

int CIniFile::GetInt(std::string section, std::string key)
{
	return atoi(GetString(section, key).data());
}

float CIniFile::GetFloat(std::string section, std::string key)
{
	return (float)atof(GetString(section, key).data());
}

bool CIniFile::GetBool(std::string section, std::string key)
{
	std::string tmp = GetString(section, key).data();

	bool res = (tmp == "true");

	return res;
}

void CIniFile::SetString(std::string section, std::string key, std::string value)
{
	WritePrivateProfileString((LPCSTR)section.data(), (LPCSTR)key.data(), (LPCSTR)value.data(), (LPCSTR)m_fileName.data());	 
}

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

CIniFile::~CIniFile(void)
{
}