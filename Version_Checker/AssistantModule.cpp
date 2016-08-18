#include "stdafx.h"
#include "AssistantModule.h"
#include <Shlwapi.h>

wstring GetRootDir()
{
	const unsigned int maxLen = 512;
	wchar_t buffer[maxLen];
	GetModuleFileNameW(NULL, buffer, sizeof(buffer));
	PathRemoveFileSpecW((LPWSTR)buffer);
	return (wstring(buffer));
}