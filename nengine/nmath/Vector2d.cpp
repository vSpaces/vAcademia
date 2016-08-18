
#include "StdAfx.h"
#include <math.h>
#include "Vector2d.h"
#include "CommonFunctions.h"

CVector2D::CVector2D(float xx, float yy)
{
	x = xx;
	y = yy;
}

CVector2D::CVector2D()
{
	x = y = 0;
}

void CVector2D::Set(float dx, float dy)
{
	x = dx;
	y = dy;
}

void CVector2D::Set(CVector2D& p)
{
	x = p.x;
	y = p.y;
}

void CVector2D::Normalize()
{
	float r = float(sqrt(x * x + y * y));

	x = x / r;
	y = y / r;
}

float CVector2D::Dot(CVector2D b)
{
	return (x * b.x + y * b.y);
}

CVector2D CVector2D::operator -(CVector2D a)
{
	CVector2D b;

	b.x = x - a.x;
	b.y = y - a.y;

	return b;
}

CVector2D CVector2D::operator +(CVector2D a)
{
	CVector2D b;

	b.x = x + a.x;
	b.y = y + a.y;

	return b;
}

void CVector2D::Rotate(float angle)
{
	float cs = (float)cos(GradToRadians(angle));
	float sn = (float)sin(GradToRadians(angle));

	float xx = x * cs - y * sn;
	y = x * sn + y * cs;
	x = xx;
}

float CVector2D::GetLength()
{
	return sqrtf(x * x + y * y);
}

CVector2D::~CVector2D()
{
}