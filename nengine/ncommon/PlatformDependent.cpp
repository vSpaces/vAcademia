
#include "StdAfx.h"
#include "shlobj.h"
#include <windows.h>
#include "PlatformDependent.h"

#ifndef LAUNCHER
#ifndef COMMAN
#ifndef VC_SETTINGS
#include "GlobalSingletonStorage.h"
#endif
#endif
#endif

std::wstring GetCurrentDirectory()
{
	wchar_t tmp[MAX_PATH + 1];
	GetCurrentDirectoryW(MAX_PATH, &tmp[0]);
	return tmp;
}

std::string GetFontsDirectory()
{
	char tmp[MAX_PATH];
	SHGetSpecialFolderPath(NULL, tmp, CSIDL_FONTS, true);
	return tmp;
}

std::wstring GetApplicationDataDirectory()
{
	WCHAR tmp[MAX_PATH];
	SHGetSpecialFolderPathW(NULL, tmp, CSIDL_APPDATA, true);
	return tmp;
}

#ifndef LAUNCHER
#ifndef COMMAN
#ifndef VC_SETTINGS
std::wstring GetApplicationRootDirectory()
{
	return g->sApplicationRootDirectory;
}

void SetApplicationRootDirectory( const std::wstring& asDirectory)
{
	g->sApplicationRootDirectory = asDirectory;
}

void CreateDirectory(std::wstring dir)
{
	::CreateDirectoryW(dir.c_str(), NULL);
}

void MakeFullPathForUI(std::wstring& path)
{
	path = GetApplicationRootDirectory() + path;
	StringReplaceW(path, L"ui:", L"\\ui\\");
}
#endif
#endif
#endif