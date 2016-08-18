// stdafx.cpp : source file that includes just the standard includes
// updater.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

TCHAR lpcIniFullPathName[MAX_PATH*2+2];
TCHAR glpcRootPath[MAX_PATH*2+2];
TCHAR glpcRepositoryPath[MAX_PATH*2+2];
TCHAR glpcUserPath[MAX_PATH*2+2];
TCHAR glpcUIPath[MAX_PATH*2+2];
TCHAR glpcDownloadManager[MAX_PATH*2+2];
TCHAR glpcComponentsPath[MAX_PATH*2+2];
CComString m_NewNersion;
CComString ucCurentVersion;

HMODULE loadDLL( LPCSTR alpcRelPath, LPCSTR alpcName)
{
	CComString sModuleDirectory;
	GetModuleFileName( NULL, sModuleDirectory.GetBufferSetLength(MAX_PATH2), MAX_PATH2);

	CComString sCurrentDirectory;
	GetCurrentDirectory( MAX_PATH2, sCurrentDirectory.GetBufferSetLength(MAX_PATH2));

	//SECFileSystem fs;
	//SetCurrentDirectory( fs.GetPath(sModuleDirectory) + alpcRelPath);
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
}

