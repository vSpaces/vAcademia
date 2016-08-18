#pragma once

#define PI	3.14159265358979f

#include <string>

std::string IntToStr(int a);
std::string ULongToStr(unsigned int a);
std::string FloatToStr(float a);
float RadiansToDegrees(float rad);
void StringReplace(std::string& str, std::string from, std::string to);
std::string StringToLower(std::string& str);
std::string StringToUpper(std::string& str);
bool BoolFromUInt(unsigned int value);
bool BoolFromUShort(unsigned short value);
wchar_t* CharToWide(char* str);
wchar_t* CharToWide(const char* str);
char* WideToChar(wchar_t* str);
char* WideToChar(const wchar_t* str);
unsigned char HexDigitToDec(char p);
unsigned int HexStringToDec(std::string hex);