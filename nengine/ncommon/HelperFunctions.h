#pragma once

#define MY_PI	3.14159265358979f

#include <string>

#ifndef LAUNCHER_SLN
#include "StringFunctions.h"
#include "Vector3D.h"
#endif

float RadiansToDegrees(float rad);
#ifndef LAUNCHER_SLN
CVector3D StrToVector3D(std::string str);
#endif
bool BoolFromUInt(unsigned int value);
bool BoolFromUShort(unsigned short value);
unsigned int Get2st(char degree);
unsigned int GetMinBigger2st(unsigned int value);
bool CheckVarName(std::string& name);
bool IsPowerOf2(int a);
#ifndef LAUNCHER_SLN
std::string CorrectPath(std::string path);
std::wstring CorrectPath(std::wstring path);
#endif