
#include "StdAfx.h"
#include <math.h>
#include "locale.h"
#ifndef LAUNCHER_SLN
#include "Vector3D.h"
#endif
#include "HelperFunctions.h"

float RadiansToDegrees(float rad)
{
	return (rad * 180.0f / 3.14159265358f);
}

#ifndef LAUNCHER_SLN
CVector3D StrToVector3D(std::string str)
{
	CVector3D tmp;
	sscanf_s(str.c_str(), "%f;%f;%f", &tmp.x, &tmp.y, &tmp.z);
	return tmp;
}
#endif

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

unsigned int GetMinBigger2st(unsigned int value)
{
	unsigned int res = 2;

	while (value > res)
	{
		res *= 2; 
	}

	return res;
}

unsigned int Get2st(char degree)
{
	int k = 1;

	for (int i = 0; i < degree; i++)
	{
		k *= 2;
	}

	return k;
}

bool CheckVarName(std::string& name)
{
	int size = name.size();

	for (int i = 0; i < size; i++)
	{
		unsigned char p = name[i];
		if (!(((p >= 'a') && (p <= 'z')) || ((p >= 'A') && (p <= 'Z'))
			 || ((p >= '0') && (p <= '9')) || (p == '_')))
		{
			return false;
		}
	}

	return true;
}

bool IsPowerOf2(int a)
{
	return ((a & (a - 1)) == 0);
}


#ifndef LAUNCHER_SLN

std::string CorrectPath(std::string path)
{
	if (path.find("tp://") != std::string::npos)
	{
		return path;
	}

	std::string::iterator it= path.begin();
	std::string::iterator itEnd= path.end();

	std::string tempPath = "";
	bool isNeedToAddSlash = true;
	char ch[2];

	ch[1] = '\0';
	//удалим повтор€ющиес€ слэши и заменим все слэши на обратные
	for (; it!=itEnd; it++)
	{
		if ((((*it) == '/') || ((*it) == '\\' ))
			&& isNeedToAddSlash)
		{
			tempPath += "\\";
			isNeedToAddSlash = false;
		}
		else if (((*it) != '/') && ((*it)!= '\\'))
		{
			ch[0] = (*it);		
			tempPath += ch;
			isNeedToAddSlash = true;
		}

	}

	path = tempPath;

	//пути вида test/test1/../test2/1.jpg превращаем в пути вида test/test2/1.jpg.
	//предусмотрев, что выходов (..) из папки может быть сколько угодно.

	int str1Pos = path.find("\\..\\");

	int str2Pos = path.rfind("\\..\\") + 3;

	if ((str1Pos != std::string::npos) && (str2Pos != std::string::npos))
	{
		std::string beginPath = path.substr(0,str1Pos); //  test/test1
		std::string endPath = path.substr(str2Pos); //      /test2/1.jpg

		int pos = -1;

		while( path.find("\\..\\") != std::string::npos)
		{
			pos = beginPath.rfind("\\");

			if (pos == -1)
			{
				path = tempPath;
				break;
			}

			beginPath = beginPath.substr(0,pos);

			// убираем из пути 1 выход (..)
			str2Pos -=3;
			path = path.substr(0,str2Pos);
			path += endPath;
		}

		path = beginPath;
		path += endPath;
	}

	return StringToLower(path);
}

std::wstring CorrectPath(std::wstring path)
{
	if (path.find(L"tp://") != std::wstring::npos)
	{
		return path;
	}	

	std::wstring::iterator it= path.begin();
	std::wstring::iterator itEnd= path.end();

	std::wstring tempPath = L"";
	bool isNeedToAddSlash = true;
	wchar_t ch[2];

	ch[1] = '\0';
	//удалим повтор€ющиес€ слэши и заменим все слэши на обратные
	for (; it!=itEnd; it++)
	{
		if ((((*it) == L'/') || ((*it) == L'\\' ))
			&& isNeedToAddSlash)
		{
			tempPath += L"\\";
			isNeedToAddSlash = false;
		}
		else if (((*it) != L'/') && ((*it)!= L'\\'))
		{
			ch[0] = (*it);	
			tempPath += ch;
 			isNeedToAddSlash = true;
		}

	}

	path = tempPath;

	//пути вида test/test1/../test2/1.jpg превращаем в пути вида test/test2/1.jpg.
	//предусмотрев, что выходов (..) из папки может быть сколько угодно.

	int str1Pos = path.find(L"\\..\\");

	int str2Pos = path.rfind(L"\\..\\") + 3;

	if ((str1Pos != std::wstring::npos) && (str2Pos != std::wstring::npos))
	{
		std::wstring beginPath = path.substr(0,str1Pos); //  test/test1
		std::wstring endPath = path.substr(str2Pos); //      /test2/1.jpg

		int pos = -1;

		while( path.find(L"\\..\\") != std::wstring::npos)
		{
			pos = beginPath.rfind(L"\\");

			if (pos == -1)
			{
				path = tempPath;
				break;
			}

			beginPath = beginPath.substr(0,pos);

			// убираем из пути 1 выход (..)
			str2Pos -=3;
			path = path.substr(0,str2Pos);
			path += endPath;
		}

		path = beginPath;
		path += endPath;
	}

	return StringToLower(path);
}

#endif