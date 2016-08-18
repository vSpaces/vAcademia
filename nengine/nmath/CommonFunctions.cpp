#include "StdAfx.h"
#include <math.h>
#include "CommonFunctions.h"

float GradToRadians(float grad)
{
	return (grad / 180.0f * 3.14159265358f);
}

float GetDistSq(float x1, float y1, float x2, float y2)
{
	return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
}

float GetDist(float x1, float y1, float x2, float y2)
{
	return sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

float Min(float a, float b)
{
	if (a > b)
	{
		return b;
	}
	else
	{
		return a;
	}
}

float Min(float a, float b, float c)
{
	float ab = Min(a, b);

	return Min(ab, c);
}

float Min(float a, float b, float c, float d)
{
	float ab = Min(a, b);
	float cd = Min(c, d);

	return Min(ab, cd);
}