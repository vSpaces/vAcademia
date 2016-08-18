
#pragma once

#include "Color3.h"

class CColor4
{
public:
	unsigned char r, g, b, a;

	CColor4():
		r(255),
		g(255),
		b(255),
		a(255)
	{
	}

	CColor4(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a):
		r(_r),
		g(_g),
		b(_b),
		a(_a)
	{
	}

	CColor4(const CColor4& clr):
		r(clr.r),
		g(clr.g),
		b(clr.b),
		a(clr.a)
	{
	}

	CColor4(const CColor3& clr):
		r(clr.r),
		g(clr.g),
		b(clr.b),
		a(255)
	{
	}

	__forceinline CColor4 operator -(CColor4 t)
	{
		CColor4 v;

		v.r = r - t.r;
		v.g = g - t.g;
		v.b = b - t.b;
		v.a = a - t.a;

		return v;
	}

	__forceinline void operator -=(CColor4 t)
	{
		r = r - t.r;
		g = g - t.g;
		b = b - t.b;
		a = a - t.a;
	}

	__forceinline void operator +=(CColor4 t)
	{
		r = r + t.r;
		g = g + t.g;
		b = b + t.b;
		a = a + t.a;
	}

	__forceinline void operator *=(float t)
	{
		r = (unsigned char)(r * t);
		g = (unsigned char)(g * t);
		b = (unsigned char)(b * t);
		a = (unsigned char)(a * t);
	}

	__forceinline void operator /=(float t)
	{
		r = (unsigned char)(r / t);
		g = (unsigned char)(g / t);
		b = (unsigned char)(b / t);
		a = (unsigned char)(a / t);
	}

	__forceinline CColor4 operator *(float t)
	{
		CColor4 v;

		v.r = (unsigned char)(r * t);
		v.g = (unsigned char)(g * t);
		v.b = (unsigned char)(b * t);
		v.a = (unsigned char)(a * t);

		return v;
	}

	__forceinline CColor4& operator =(const CColor4& v)
	{
		r = v.r;
		g = v.g;
		b = v.b;
		a = v.a;

		return *this;
	}

	~CColor4()
	{
	}
};
