#pragma once

#include <string>

class CDumpParamsGetter
{
public:
	CDumpParamsGetter(void);
	virtual ~CDumpParamsGetter(void);
	BOOL get( TCHAR *keyName, wchar_t **sValue, TCHAR *sDefaultValue);

private:
	std::wstring m_sFileName;
};
