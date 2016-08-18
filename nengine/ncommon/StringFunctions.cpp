
#include "StdAfx.h"
#include <math.h>
#include "StringFunctions.h"
#include <atlconv.h>

std::string ULongToStr(unsigned int a)
{
	char tmp[20];

	_ultoa_s(a, tmp, 20, 10);

	return tmp;
}

std::string IntToStr(int a)
{
	char tmp[20];

	_itoa_s(a, tmp, 20, 10);

	return tmp;
}

std::wstring IntToWStr(int a)
{
	wchar_t tmp[20];

	_itow_s(a, tmp, 20, 10);

	return tmp;
}

std::string Int64ToStr(__int64 a)
{
	char tmp[40];

	_i64toa_s(a, tmp, 40, 10);

	return tmp;
}

std::string FloatToStr(float a)
{
	char buffer[70];	
	strcpy_s(buffer, "");
	sprintf_s(buffer, 70, "%15.5f", a);
	return buffer;	
}

void CharReplace(std::string& str, char src, char dest)
{
	for (unsigned int i = 0; i < str.size(); i++)
	if (str[i] == src)
	{
		str[i] = dest;
	}
}

void StringReplace(std::string& str, std::string from, std::string to)
{
	int p = str.find(from);
	
	while (p != std::string::npos) 
	{ 
		str.replace(p, from.length(), to); 
		p += to.length();
		p = str.find(from, p);
	}
}

void StringReplaceW(std::wstring& str, std::wstring from, std::wstring to)
{
	int p = str.find(from);

	while (p != std::wstring::npos) 
	{ 
		str.replace(p, from.length(), to); 
		p += to.length();
		p = str.find(from, p);
	}
}

wchar_t* CopyWide(const wchar_t* str)
{
	int size = wcslen(str) + 1;

	wchar_t* result = (wchar_t*)MP_NEW_ARR(unsigned short, size);
	wcscpy_s(result, size, str);

	return result;
}

wchar_t* CharToWide(const char* str)
{
	int size = strlen(str) + 1;

	wchar_t* result = (wchar_t*)MP_NEW_ARR(unsigned short, size);
	mbstowcs_s((size_t*)&size, result, size, str, size);

	return result;
}

wchar_t* CharToWide(char* str)
{
	int size = strlen(str) + 1;

	wchar_t* result = (wchar_t*)MP_NEW_ARR(unsigned short, size);
	mbstowcs_s((size_t*)&size, result, size, str, size);

	return result;
}

std::string WCharStringToString(wchar_t* str)
{
	USES_CONVERSION;
	return W2A(str);
}

std::string WStringToString(std::wstring& str)
{
	return WCharStringToString((wchar_t*)str.c_str());
}

std::wstring CharStringToWString(char* str)
{
	USES_CONVERSION;
	return A2W(str);
}

std::wstring StringToWString(std::string& str)
{
	return CharStringToWString((char*)str.c_str());
}

char* WideToChar(wchar_t* str)
{
	_wsetlocale(LC_ALL, L"Russian"); // setlocale(LC_ALL, "Russian");

	int size = wcslen(str) + 1;

	char* result = MP_NEW_ARR(char, size);
	int resultSize = 0;
	wcstombs_s((size_t*)&resultSize, result, size, str, size);
	result[size - 1] = '\0';	

	return result;
}

char* WideToChar(const wchar_t* str)
{
	_wsetlocale(LC_ALL, L"Russian"); // setlocale(LC_ALL, "Russian");
	
	int size = wcslen(str) + 1;

	char* result = MP_NEW_ARR(char, size);	
	int resultSize = 0;
	wcstombs_s((size_t*)&resultSize, result, size, str, size);

	result[size - 1] = '\0';

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

unsigned char HexDigitToDec(wchar_t p)
{
	if ((p >= L'0') && (p <= L'9'))
	{
		return (unsigned char)(p - L'0');
	}

	if ((p >= L'a') && (p <= L'f'))
	{
		return (unsigned char)(p - L'a' + 10);
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

unsigned int HexStringToDec(std::wstring hex)
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

float StringToFloat(const char *p)
{
#define white_space(c) ((c) == ' ' || (c) == '\t')
#define valid_digit(c) ((c) >= '0' && (c) <= '9')

    int frac = 0;
    float sign, value, scale;

    // Skip leading white space, if any.

    while (white_space(*p) ) {
        p += 1;
    }

    // Get sign, if any.

    sign = 1.0;
    if (*p == '-') {
        sign = -1.0;
        p += 1;

    } else if (*p == '+') {
        p += 1;
    }

    // Get digits before decimal point or exponent, if any.

    value = 0.0f;
    while (valid_digit(*p)) {
        value = value * 10.0f + (*p - '0');
        p += 1;
    }

    // Get digits after decimal point, if any.

    if ((*p == '.') || (*p == ',')) {
        float pow10 = 10.0f;
        p += 1;

        while (valid_digit(*p)) {
            value += (*p - '0') / pow10;
            pow10 *= 10.0f;
            p += 1;
        }
    }

    // Handle exponent, if any.

    scale = 1.0f;
    if ((*p == 'e') || (*p == 'E')) {
        unsigned int expon;
        p += 1;

        // Get sign of exponent, if any.

        frac = 0;
        if (*p == '-') {
            frac = 1;
            p += 1;

        } else if (*p == '+') {
            p += 1;
        }

        // Get digits of exponent, if any.

        expon = 0;
        while (valid_digit(*p)) {
            expon = expon * 10 + (*p - '0');
            p += 1;
        }
        if (expon > 308) expon = 308;

        // Calculate scaling factor.

        while (expon >= 25) { scale *= (float)1E25; expon -= 25; }
        while (expon >=  8) { scale *= 1E8;  expon -=  8; }
        while (expon >   0) { scale *= 10.0; expon -=  1; }
    }

    // Return signed and scaled floating point result.

    return sign * (frac ? (value / scale) : (value * scale));
}

void StringTrimRight(std::string& str)
{
	while ((str.size() > 0) && (str[str.size() - 1] == ' '))
	{
		str.erase(str.size() - 1, 1);
	}
}

void StringTrimLeft(std::string& str)
{
	while ((str.size() > 0) && (str[0] == ' '))
	{
		str.erase(0, 1);
	}
}

void StringTrimRight(std::wstring& str)
{
	while ((str.size() > 0) && (str[str.size() - 1] == L' '))
	{
		str.erase(str.size() - 1, 1);
	}
}

void StringTrimLeft(std::wstring& str)
{
	while ((str.size() > 0) && (str[0] == L' '))
	{
		str.erase(0, 1);
	}
}