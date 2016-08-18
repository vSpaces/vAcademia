#include "StdAfx.h"
#include "DumpParamsGetter.h"
#include "shlobj.h"
#include <string>

std::wstring GetApplicationDataDirectory()
{
	WCHAR tmp[MAX_PATH];
	SHGetSpecialFolderPathW(NULL, tmp, CSIDL_APPDATA, true);
	return tmp;
}

CDumpParamsGetter::CDumpParamsGetter(void)
{
	m_sFileName = GetApplicationDataDirectory().c_str();
	m_sFileName += L"\\dump_params.ini";
}

CDumpParamsGetter::~CDumpParamsGetter(void)
{
}

BOOL CDumpParamsGetter::get( WCHAR *keyName, WCHAR **sValue, WCHAR *sDefaultValue)
{
	return GetPrivateProfileStringW( L"system", keyName, sDefaultValue, *sValue, 255, m_sFileName);
}
