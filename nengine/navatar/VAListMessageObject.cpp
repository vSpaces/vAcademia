
#include "StdAfx.h"
#include "HelperFunctions.h"
#include "VAListMessageObject.h"

CVAListMessageObject::CVAListMessageObject(va_list list, int argCount):
	m_list(list),
	m_argCount(argCount)
{
}

int CVAListMessageObject::GetArgumentCount()
{
	return m_argCount;
}

std::string CVAListMessageObject::GetNextString()
{
	wchar_t* tmp = va_arg(m_list, wchar_t*);	
	if (tmp)
	{
		char* tmp2 = WideToChar(tmp);
		std::string res = tmp2;
		MP_DELETE_ARR(tmp2);
		return res;
	}
	else
	{
		return "";
	}
}

CVAListMessageObject::~CVAListMessageObject()
{
}