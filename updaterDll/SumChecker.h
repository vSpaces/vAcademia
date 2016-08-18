#pragma once
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <wincrypt.h>
#include <string.h>
#include "CheckSum.h"
class SumChecker
{
public:
	SumChecker(void);
	~SumChecker(void);
	static DWORD WINAPI CheckAllFiles(std::wstring);
	static char sumStatus[4096]; 
};
