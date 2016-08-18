#pragma once

#include <string>
#include "malloc.h"

__forceinline static std::string StringLeft(std::string& str, const int count)
{
	return str.substr(0, count);
}

__forceinline static std::string StringRight(std::string& str, const int count)
{
	return str.substr(str.size() - count, count);
}

std::string IntToStr(int a);
std::wstring IntToWStr(int a);
std::string Int64ToStr(__int64 a);
std::string ULongToStr(unsigned int a);
std::string FloatToStr(float a);
void StringReplace(std::string& str, std::string from, std::string to);
void StringReplaceW(std::wstring& str, std::wstring from, std::wstring to);
void CharReplace(std::string& str, char src, char dest);

static std::string StringToLower(std::string& str)
{
	std::string res = str;
	for (unsigned int i = 0; i < str.size(); i++)
	if ((res[i] >= 'A') && (res[i] <= 'Z'))
	{
		res[i] -= ('A' - 'a');
	}

	return res;
}

static std::wstring StringToLower(std::wstring& str)
{
	std::wstring res = str;
	for (unsigned int i = 0; i < str.size(); i++)
	if ((res[i] >= L'A') && (res[i] <= L'Z'))
	{
		res[i] -= (wchar_t)(L'A' - L'a');
	}
	return res;
}
	
static std::string StringToUpper(std::string& str)
{		
	std::string res = str;
	for (unsigned int i = 0; i < str.size(); i++)
	if ((res[i] >= 'a') && (res[i] <= 'z'))
	{
		res[i] += ('A' - 'a');
	}

	return res;
}

static std::wstring StringToUpper(std::wstring& str)
{
	std::wstring res = str;
	for (unsigned int i = 0; i < str.size(); i++)
	if ((res[i] >= L'a') && (res[i] <= L'z'))
	{
		res[i] += (wchar_t)(L'A' - L'a');
	}
	return res;
}

void StringTrimRight(std::string& str);
void StringTrimLeft(std::string& str);
void StringTrimRight(std::wstring& str);
void StringTrimLeft(std::wstring& str);
float StringToFloat(const char *p);
wchar_t* CharToWide(char* str);
wchar_t* CharToWide(const char* str);
wchar_t* CopyWide(const wchar_t* str);
char* WideToChar(wchar_t* str);
char* WideToChar(const wchar_t* str);
unsigned char HexDigitToDec(char p);
unsigned char HexDigitToDec(wchar_t p);
unsigned int HexStringToDec(std::string hex);
unsigned int HexStringToDec(std::wstring hex);
std::string WCharStringToString(wchar_t* str);
std::string WStringToString(std::wstring& str);
std::wstring CharStringToWString(char* str);
std::wstring StringToWString(std::string& str);

static std::string StringFormatV(LPCTSTR pszCharSet, va_list vl)
{
	TCHAR* pszTemp = NULL;
	int nBufferSize = 0;
	int nRetVal = -1;

	do {
		nBufferSize += 100;
		pszTemp = (TCHAR*)_alloca(nBufferSize*sizeof(TCHAR));	ZeroMemory( pszTemp, nBufferSize*sizeof(TCHAR));
		nRetVal = _vsnprintf_s(pszTemp, nBufferSize-1, nBufferSize-1, pszCharSet, vl);
	} while (nRetVal < 0 && nBufferSize < (INT_MAX-101));

	std::string res = pszTemp;
	return res;
}

__forceinline static std::string StringFormat(LPCTSTR pszCharSet, ...)
{
	va_list vl;
	va_start(vl, pszCharSet);
	std::string res = StringFormatV( pszCharSet, vl);
	va_end(vl);
	return res;
}