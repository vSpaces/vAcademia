#include "StdAfx.h"
#include "DumpParamsGetter.h"
#include "shlobj.h"
#include <string>

std::wstring GetApplicationDataDirectory2()
{
	wchar_t tmp[MAX_PATH];
	SHGetSpecialFolderPathW(NULL, tmp, CSIDL_APPDATA, true);
	return tmp;
}

CDumpParamsGetter::CDumpParamsGetter(void)
{
	m_sFileName = GetApplicationDataDirectory2().c_str();
	m_sFileName += L"\\Vacademia\\dump_params.ini";
}

CDumpParamsGetter::~CDumpParamsGetter(void)
{
}

BOOL CDumpParamsGetter::get( TCHAR *keyName, wchar_t **sValue, TCHAR *sDefaultValue)
{
	USES_CONVERSION;
	return GetPrivateProfileStringW( L"system", A2W(keyName), A2W(sDefaultValue), *sValue, 255, m_sFileName.c_str());
}
