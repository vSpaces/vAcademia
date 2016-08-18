
#pragma once

class CColor3
{
public:
	unsigned char r, g, b;

	CColor3():
		r(255),
		g(255),
		b(255)
	{
	}

	CColor3(unsigned char _r, unsigned char _g, unsigned char _b):
		r(_r),
		g(_g),
		b(_b)
	{
	}

	CColor3(const CColor3& clr):
		r(clr.r),
		g(clr.g),
		b(clr.b)
	{
	}

	__forceinline CColor3 operator -(CColor3 a)
	{
		CColor3 v;

		v.r = r - a.r;
		v.g = g - a.g;
		v.b = b - a.b;

		return v;
	}

	__forceinline CColor3 operator +(CColor3 a)
	{
		CColor3 v;

		v.r = r + a.r;
		v.g = g + a.g;
		v.b = b + a.b;

		return v;
	}

	__forceinline void operator -=(CColor3 a)
	{
		r = r - a.r;
		g = g - a.g;
		b = b - a.b;
	}

	__forceinline void operator +=(CColor3 a)
	{
		r = r + a.r;
		g = g + a.g;
		b = b + a.b;
	}

	__forceinline void operator *=(float a)
	{
		r = (unsigned char)(r * a);
		g = (unsigned char)(g * a);
		b = (unsigned char)(b * a);
	}

	__forceinline void operator /=(float a)
	{
		r = (unsigned char)(r / a);
		g = (unsigned char)(g / a);
		b = (unsigned char)(b / a);
	}

	__forceinline CColor3 operator *(float a)
	{
		CColor3 v;

		v.r = (unsigned char)(r * a);
		v.g = (unsigned char)(g * a);
		v.b = (unsigned char)(b * a);

		return v;
	}

	__forceinline CColor3& operator =(const CColor3& v)
	{
		r = v.r;
		g = v.g;
		b = v.b;

		return *this;
	}

	~CColor3()
	{
	}
};
