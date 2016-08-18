#include "stdafx.h"
#include "IniValueFinder.h"
#include "AssistantModule.h"

CIniValueFinder::CIniValueFinder(const wstring &iniFileName)
{
	SetPath(iniFileName);
}

CIniValueFinder::~CIniValueFinder(){};

void CIniValueFinder:: SetPath(const wstring &iniFileName)
{
	wstring path = GetRootDir();
	path += L"\\" + iniFileName;
	m_path = path;
}

wstring CIniValueFinder:: GetPath()const
{
	return m_path;
}

string CIniValueFinder:: GetValue(const wchar_t* section, const wchar_t* key)const
{
	const int maxLenData = 255;
	wchar_t buf[255];
	int length = GetPrivateProfileStringW(section, key, L"incorrect value",
		buf, maxLenData, GetPath().c_str());
	wstring tmp = wstring(buf);
	string str;
	str.assign(tmp.begin(), tmp.end());
	return str;
}

void CIniValueFinder::SetValue(const wchar_t* section, const wchar_t* key, const wchar_t* value)
{
	const int maxLenData = 255;
	wchar_t buf[255];
	int length = WritePrivateProfileStringW(section, key, value, GetPath().c_str());	
}