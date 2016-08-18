
#pragma once

#include <Math.h>

class CQuaternion;

class CVector3D
{
public:
	float x, y, z;

public:
	__forceinline CVector3D(float xx, float yy, float zz)
	{
		x = xx;
		y = yy;
		z = zz;
	}

	__forceinline CVector3D()
	{
		x = y = z = 0;
	}

	inline CVector3D Cross(CVector3D const& b) const
	{
		return (CVector3D(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x));
	}

	__forceinline void Cross(const CVector3D& a, const CVector3D& b)
	{
		x = a.y * b.z - a.z * b.y;
		y = a.z * b.x - a.x * b.z;
		z = a.x * b.y - a.y * b.x;
	}

	__forceinline void Set(float dx, float dy, float dz)
	{
		x = dx;
		y = dy;
		z = dz;
	}

	__forceinline void Set(const CVector3D& p)
	{
		x = p.x;
		y = p.y;
		z = p.z;
	}

	__forceinline void Normalize()
	{
		float r = sqrtf(x * x + y * y + z * z);

		if (r != 0.0f)
		{
			x = x / r;
			y = y / r;
			z = z / r;
		}
	}

	__forceinline float Dot(const CVector3D& b) const
	{
		return x * b.x + y * b.y + z * b.z; 
	}

	__forceinline static float Dot(const CVector3D& a, const CVector3D& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z; 
	}

	inline CVector3D operator -(CVector3D const& a) const
	{
		CVector3D b;

		b.x = x - a.x;
		b.y = y - a.y;
		b.z = z - a.z;

		return b;
	}

	inline CVector3D operator +(CVector3D const& a) const
	{
		CVector3D b;

		b.x = x + a.x;
		b.y = y + a.y;
		b.z = z + a.z;

		return b;
	}

	void operator -=(CVector3D const& a)
	{
		x -= a.x;
		y -= a.y;
		z -= a.z;
	}

	__forceinline void operator +=(CVector3D const& a)
	{
		x += a.x;
		y += a.y;
		z += a.z;
	}

	__forceinline void operator *=(float const& a)
	{
		x *= a;
		y *= a;
		z *= a;
	}

	void operator *=(CQuaternion a);
	CVector3D operator *(CQuaternion q);

	__forceinline void operator /= (float a)
	{
		x /= a;
		y /= a;
		z /= a;
	}

	inline CVector3D operator *(float a) const
	{
		CVector3D tmp;

		tmp.x = x * a;
		tmp.y = y * a;
		tmp.z = z * a;

		return tmp;
	}

	void RotateX(float angle);
	void RotateY(float angle);
	void RotateZ(float angle);

	float GetAngleY(CVector3D vec);
	float GetAngleZ(CVector3D vec);

	__forceinline float GetLength() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	__forceinline float GetLengthSq() const
	{
		return x * x + y * y + z * z;
	}

	operator float*()
	{
		return &x;
	}

	operator const float*()const
	{
		return &x;
	}

	CVector3D& operator =(const CVector3D& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;

		return *this;
	}

	__forceinline friend CVector3D operator %(const CVector3D& v, const CVector3D& u);	

	inline CVector3D GetNormal(CVector3D& second) const
	{
		CVector3D ret;
		ret.x = y * second.z - z * second.y;
		ret.y = z * second.x - x * second.z;
		ret.z = x * second.y - y * second.x;
		ret.Normalize();

		return ret;
	}

	__forceinline float GetAngle(CVector3D& second) const
	{
		if ((GetLength() == 0.0f) || (second.GetLength() == 0.0f))
		{
			return 0.0f;
		}

		float arg = (x * second.x + y * second.y + z * second.z) / (GetLength() * second.GetLength());
		if( arg >= 1)
		{
			return 0.0f;
		}

		if( arg <= -1)
		{
			return 3.141592f;
		}

		return acosf(arg);
	}

	float GetAngleSigned(CVector3D const& second, CVector3D const& n = CVector3D(0, 0, 1)) const
	{
		CVector3D vCross = Cross(second);
		return atan2(n.Dot(vCross), Dot(second));
	}

	__forceinline bool operator==(const CVector3D& o) const
	{
		return (x == o.x && y == o.y && z == o.z);
	}

	__forceinline bool operator!=(const CVector3D& o) const
	{
		return ((x != o.x) || (y != o.y) || (z != o.z));
	}

	__forceinline ~CVector3D()
	{
	}
};

CVector3D operator %(const CVector3D& v, const CVector3D& u)
{
	return CVector3D(v.y * u.z - v.z * u.y, v.z * u.x - v.x * u.z, v.x * u.y - v.y * u.x);
}