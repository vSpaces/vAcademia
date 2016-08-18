
#include "StdAfx.h"
#include <math.h>
#include "HelperFunctions.h"

std::string ULongToStr(unsigned int a)
{
	char tmp[20];

	ultoa(a, tmp, 10);

	return tmp;
}

std::string IntToStr(int a)
{
	char tmp[20];

	itoa(a, tmp, 10);

	return tmp;
}

float RadiansToDegrees(float rad)
{
	return (rad * 180.0f / 3.14159265358f);
}

std::string FloatToStr(float a)
{
	char tmp[20], tmp2[20];

	itoa((int)a, tmp, 10);
	itoa(((int)(fabsf(a) * 100000.0f)%100000), tmp2, 10);

	std::string fn = tmp;
	fn += ".";
	fn += tmp2;

	return fn;
}


void StringReplace(std::string& str, std::string from, std::string to)
{
	int p = str.find(from);
	
	while (p != -1) 
	{ 
		str.replace(p, from.length(), to); 
		p = str.find(from);
	}
}

std::string StringToLower(std::string& str)
{
	std::string res = str;
	for (unsigned int i = 0; i < str.length(); i++)
	if ((res[i] >= 'A') && (res[i] <= 'Z'))
	{
		res[i] -= ('A' - 'a');
	}

	return res;
}

std::string StringToUpper(std::string& str)
{
	std::string res = str;
	for (unsigned int i = 0; i < str.length(); i++)
	if ((res[i] >= 'a') && (res[i] <= 'z'))
	{
		res[i] += ('A' - 'a');
	}

	return res;
}

bool BoolFromUInt(unsigned int value)
{
	if (value == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool BoolFromUShort(unsigned short value)
{
	if (value == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

wchar_t* CharToWide(const char* str)
{
	int size = strlen(str) + 1;

	wchar_t* result = (wchar_t*)new unsigned short[size];
	mbstowcs(result, str, size);

	return result;
}

wchar_t* CharToWide(char* str)
{
	int size = strlen(str) + 1;

	wchar_t* result = (wchar_t*)new unsigned short[size];
	mbstowcs(result, str, size);

	return result;
}

char* WideToChar(wchar_t* str)
{
	int size = wcslen(str) + 1;

	char* result = new char[size];
	wcstombs(result, str, size);

	return result;
}

char* WideToChar(const wchar_t* str)
{
	_wsetlocale(LC_ALL, L"Russian"); // setlocale(LC_ALL, "Russian");
	
	//_locale_t rus = _create_locale(LC_ALL, "Russian");
	int size = wcslen(str) + 1;

	char* result = new char[size];
	wcstombs(result, str, size);

	result[size - 1] = '\0';

	//_free_locale(rus);

	return result;
}

unsigned char HexDigitToDec(char p)
{
	if ((p >= '0') && (p <= '9'))
	{
		return (p - '0');
	}

	if ((p >= 'a') && (p <= 'f'))
	{
		return (p - 'a' + 10);
	}

	return 255;
}

unsigned int HexStringToDec(std::string hex)
{
	int mult = 1;
	hex = StringToLower(hex);

	unsigned int res = 0;

	for (int i = hex.size() - 1; i >= 0; i--)
	{
		res += HexDigitToDec(hex[i]) * mult;

		mult *= 16;
	}

	return res;
}