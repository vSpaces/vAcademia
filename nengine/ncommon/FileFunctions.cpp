
#include "StdAfx.h"
#include "FileFunctions.h"

FILE* FileOpen(const std::wstring& path, const std::wstring& mode)
{
	FILE* fl = NULL;
	if (_wfopen_s(&fl, path.c_str(), mode.c_str()) == 0)
	{
		return fl;
	}
	else
	{
		return NULL;
	}
}

FILE* FileOpenA(const std::string& path, const std::string& mode)
{
	FILE* fl = NULL;
	if (fopen_s(&fl, path.c_str(), mode.c_str()) == 0)
	{
		return fl;
	}
	else
	{
		return NULL;
	}
}

bool IsFileExists(const std::wstring& path)
{
	FILE* fl = FileOpen(path, L"r");
	if (fl)
	{
		fclose(fl);
		return true;
	}

	return false;
}