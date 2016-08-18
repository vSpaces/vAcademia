#include "stdafx.h"
#include "DumpParamsSaver.h"

#include "shlobj.h"

CDumpParamsSaver::CDumpParamsSaver(void)
{
	m_sFileName = getApplicationDataDirectory().c_str();
	m_sFileName += L"\\Vacademia\\dump_params.ini";
}

CDumpParamsSaver::~CDumpParamsSaver(void)
{
}

BOOL CDumpParamsSaver::set( WCHAR *keyName, WCHAR *sValue)
{
	return WritePrivateProfileStringW( L"system", keyName, sValue, m_sFileName);
}

std::wstring CDumpParamsSaver::getApplicationDataDirectory()
{
	WCHAR tmp[MAX_PATH];
	SHGetSpecialFolderPathW(NULL, tmp, CSIDL_APPDATA, true);
	return tmp;
}
