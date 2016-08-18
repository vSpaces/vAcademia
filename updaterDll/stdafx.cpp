// stdafx.cpp : source file that includes just the standard includes
// updater.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

WCHAR lpcIniFullPathName[MAX_PATH*2+2];
WCHAR glpcRootPath[MAX_PATH*2+2];
WCHAR glpcRepositoryPath[MAX_PATH*2+2];
WCHAR glpcUserPath[MAX_PATH*2+2];
TCHAR glpcUIPath[MAX_PATH*2+2];
TCHAR glpcDownloadManager[MAX_PATH*2+2];
TCHAR glpcComponentsPath[MAX_PATH*2+2];
CComString m_NewNersion;
CComString ucCurentVersion;
CComString m_Lang;
CComString m_LanguageIni;
CWComString m_ClientPath;
BOOL  m_IsRequiredUpdate= FALSE;
BOOL m_IsNeedDownloadSetupOnly = TRUE;
BOOL m_IsBrokenClient = FALSE;

/*HMODULE loadDLL( LPCSTR alpcRelPath, LPCSTR alpcName)
{
	CComString sModuleDirectory;
	GetModuleFileName( NULL, sModuleDirectory.GetBufferSetLength(MAX_PATH2), MAX_PATH2);

	CComString sCurrentDirectory;
	GetCurrentDirectory( MAX_PATH2, sCurrentDirectory.GetBufferSetLength(MAX_PATH2));

	//SECFileSystem fs;
	//SetCurrentDirectory( fs.GetCurrentPath(sModuleDirectory) + alpcRelPath);
	sModuleDirectory.Replace("\\", "/");
	sModuleDirectory.Replace("//", "/");
	int pos = sModuleDirectory.ReverseFind('//');
	if(pos>-1)
	{
		sModuleDirectory = sModuleDirectory.Left(pos);
	}
	CComString sPath = sModuleDirectory + "/" + alpcName;
	sPath.Replace("\\", "/");
	sPath.Replace("//", "/");
	HMODULE handle = ::LoadLibrary( sPath);
	SetCurrentDirectory( sCurrentDirectory);
	return handle;
}*/