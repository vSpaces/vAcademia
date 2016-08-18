#pragma once
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <wincrypt.h>
#include <string.h>
#include <xstring>
class CheckSum
{
public:
	CheckSum(void);
	~CheckSum(void);
	static DWORD md5Counter(LPCWSTR);
    static CHAR md5string[34];
	static void enumerateFilesHashes(wchar_t*);
};

