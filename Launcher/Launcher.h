#pragma once

#include "resource.h"

__forceinline static std::string StringToLower(std::string& str)
{
	std::string res = str;
	for (unsigned int i = 0; i < str.length(); i++)
		if ((res[i] >= 'A') && (res[i] <= 'Z'))
		{
			res[i] -= ('A' - 'a');
		}

		return res;
}
