#include "stdafx.h"
#include "RegKeyFilesDialogs.h"

#define BASE_REG_KEY "SOFTWARE\\VCitiesLabs\\MaxExporter\\"

CRegManager::CRegManager()
{
	HKEY key;//
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,BASE_REG_KEY, 0, KEY_ALL_ACCESS, &key)!= ERROR_SUCCESS)
	{
		RegCreateKey(HKEY_LOCAL_MACHINE, (LPCTSTR)(BASE_REG_KEY), &key);
	}
	RegCloseKey(key);
	
	reglist.clear();
}

CRegManager::~CRegManager()
{
	reglist.clear();
}

int CRegManager::FindInVector(CString Name)
{
	for (int i=0; i< (int)reglist.size(); i++ )
	{
		if ( reglist[i].Name == Name )
			return i;
	}
	return -1;
}

BOOL CRegManager::ReadValue(LPCTSTR pName, CString& ret)
{
	HKEY key;//
	DWORD sizedata=512;//
	unsigned char datakey[512];//

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,BASE_REG_KEY, 0, KEY_ALL_ACCESS, &key)==ERROR_SUCCESS)
	{
		if(RegQueryValueEx(key, pName, NULL, NULL, (LPBYTE)datakey, &sizedata)==ERROR_SUCCESS)
		{
			ret = CString((LPSTR)datakey);
			RegCloseKey(key);
			return TRUE;
		}
		RegCloseKey(key);
	}
	else
	{
		int i;
		i = FindInVector((CString)pName);
		if (i != -1)
		{
			ret = reglist[i].Value;
			return TRUE;
		}
	}
	return FALSE;
}

void CRegManager::WriteValue(LPCTSTR pName, LPCTSTR pValue)
{
	HKEY key;//
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,BASE_REG_KEY, 0, KEY_ALL_ACCESS, &key)==ERROR_SUCCESS)
	{
		RegSetValueEx(key, pName, 0, REG_SZ, (const BYTE*)pValue, strlen(pValue) + 1);
	}
	else
	{
		int i;
		i = FindInVector((CString)pName);
		Sreglist InputLine;
		InputLine.Name  = (CString)pName;
		InputLine.Value = (CString)pValue;
		if (i == -1)
			reglist.push_back( InputLine );
	}
	RegCloseKey(key);
}
