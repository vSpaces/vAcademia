#include "stdafx.h"
#include "helpfunctions.h"
#include "GlobalSingletonStorage.h"

int WildCompare(const char *wild, const char *string)
{
	const char *cp = NULL, *mp = NULL;

	while ((*string) && (*wild != '*'))
	{
		if ((*wild != *string) && (*wild != '?'))
		{
			return 0;
		}
		wild++;
		string++;
	}

	while (*string) {
		if (*wild == '*') {
			if (!*++wild) {
				return 1;
			}
			mp = wild;
			cp = string+1;
		} else if ((*wild == *string) || (*wild == '?')) {
			wild++;
			string++;
		} else {
			wild = mp;
			string = cp++;
		}
	}

	while (*wild == '*') {
		wild++;
	}
	return !*wild;	
}

float GetTriangleSquare(CVector2D p1, CVector2D p2, CVector2D p3)
{
	float result = 0;

	float a = (p2 - p1).GetLength();
	float b = (p3 - p1).GetLength();
	float c = (p3 - p2).GetLength();

	float p = (a + b + c)/2;

	result = sqrtf(p*(p-a)*(p-b)*(p-c));

	return result;
}
