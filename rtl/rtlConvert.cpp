#include "StdAfx.h"
#include ".\rtlconvert.h"

namespace rtl
{
	namespace convert
	{
		VA_RTL_API int rtl_atoi(const char * aString)
		{
			int iValue = 0;
			if (aString != NULL)
			{
				//can call invalid parameter handler
				iValue = atoi(aString);	
			}
			return iValue;
		}

		VA_RTL_API int rtl_wtoi(const wchar_t * aString)
		{
			int iValue = 0;
			if (aString != NULL)
			{
				//can call invalid parameter handler
				iValue = _wtoi(aString);	
			}
			return iValue;
		}

		VA_RTL_API __int64 rtl_atoi64(const char * aString)
		{
			__int64 iValue = 0;
			if (aString != NULL)
			{
				//can call invalid parameter handler
				iValue = _atoi64(aString);	
			}
			return iValue;
		}
	}
}