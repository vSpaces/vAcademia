#pragma once

#include "Vector2d.h"

int WildCompare(const char *wild, const char *string);
float GetTriangleSquare(CVector2D p1, CVector2D p2, CVector2D p3);

template<class Type>
int GetSign(Type& val)
{
	if(val >= 0)
	{
		return 1;
	}

	return -1;
}