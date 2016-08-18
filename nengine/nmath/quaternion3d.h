
#pragma once

#include "Vector3D.h"
#define _USE_MATH_DEFINES
#include <Math.h>

#define QUAT_EPS	0.0001

class CQuaternion
{
public:
	float x;
	float y;
	float z;
	float w;
	
public:
	__forceinline CQuaternion()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 1.0f;
	}

	__forceinline CQuaternion(const CQuaternion& q)
	{
		x = q.x;
		y = q.y;
		z = q.z;
		w = q.w;
	}

	__forceinline CQuaternion(float qx, float qy, float qz, float qw)
	{
		x = qx;
		y = qy;
		z = qz;
		w = qw;
	}

	__forceinline CQuaternion::CQuaternion(CVector3D v, float angle)
	{
		float sinA = sin(angle / 2);
		float cosA = cos(angle / 2);

		x = v.x * sinA;
		y = v.y * sinA;
		z = v.z * sinA;
		w = cosA;

		Normalize();
	}

	__forceinline CQuaternion(CVector3D vsource, CVector3D vdest)
	{
		float	dot = vsource.Dot(vdest);

		if (dot > 0.9999999999)	
		{
			x = y = w = 0;
			z = 1;
			return;	
		}

		if (dot < -0.9999999999)
		{
			CVector3D axe;
			axe.Set(0,0,1);
			*this = CQuaternion(axe, 3.14f);
			return;
		}

		*this = CQuaternion(vsource.GetNormal(vdest), -vsource.GetAngle(vdest));
	}

	__forceinline CQuaternion(CVector3D vsource, CVector3D vdest, float mulKoef)
	{
		float	dot = vsource.Dot(vdest);

		if (dot > 0.9999999999)	
		{
			x = y = w = 0;
			z = 1;
			return;	
		}

		if (dot < -0.9999999999)
		{
			CVector3D axe;
			axe.Set(0,0,1);
			*this = CQuaternion(axe, 3.14f);
			return;
		}

		*this = CQuaternion(vsource.GetNormal(vdest), -vsource.GetAngle(vdest) * mulKoef);
	}
	
	__forceinline float& operator[](unsigned int index)
	{
		return (&x)[index];
	}
	
	__forceinline const float& operator[](unsigned int index) const
	{
		return (&x)[index];
	}	
	
	__forceinline void operator=(const CQuaternion& q)
		{
		x = q.x;
		y = q.y;
		z = q.z;
		w = q.w;
	}
	
	__forceinline void operator*=(const CQuaternion& q)
	{
		float mx( x ), my( y ), mz( z ), mw( w );
		float qx(q.x), qy(q.y), qz(q.z), qw(q.w);
		
		x = mw * qx + mx * qw + my * qz - mz * qy;
		y = mw * qy - mx * qz + my * qw + mz * qx;
		z = mw * qz + mx * qy - my * qx + mz * qw;
		w = mw * qw - mx * qx - my * qy - mz * qz;
	}
	
	__forceinline void operator+=(const CQuaternion& q)
	{
		x += q.x;
		y += q.y;
		z += q.z;
		w += q.w;
	}
	
	__forceinline void operator*=(const CVector3D& v)
	{
		float qx, qy, qz, qw;
		qx = x;
		qy = y;
		qz = z;
		qw = w;
		
		x = qw * v.x            + qy * v.z - qz * v.y;
		y = qw * v.y - qx * v.z            + qz * v.x;
		z = qw * v.z + qx * v.y - qy * v.x;
		w =          - qx * v.x - qy * v.y - qz * v.z;
	}

	__forceinline void operator*= (float s)
	{
		x *= s;
		y *= s;
		z *= s;
		w *= s;
	}

	__forceinline bool operator==(const CQuaternion& rhs) const
	{
		return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
	}

	__forceinline bool operator!=(const CQuaternion& rhs) const
	{
		return !operator==(rhs);
	}

	__forceinline void Extract(CVector3D &axes, float &angle)
	{
		Normalize();
   
		angle = acosf(w) * 2;
		float sin_angle = sqrtf(1.0f - w * w);

		if (fabs(sin_angle) < 0.0005)
		{
			sin_angle = 1.0f;
		}

		axes.x = x / sin_angle;
		axes.y = y / sin_angle;
		axes.z = z / sin_angle;
	}

	inline CQuaternion Normalize()
	{
		CQuaternion	rq;
		
		float norm = sqrt(x*x+y*y+z*z+w*w);
		if (0.0f == norm)
		{
			rq.x = x;
			rq.y = y;
			rq.z = z;
			rq.w = w;
			return rq;
		}

		x /= norm;
		y /= norm;
		z /= norm;
		w /= norm;

		rq.x = x;
		rq.y = y;
		rq.z = z;
		rq.w = w;

		return rq;
	}

	__forceinline void Blend(float d, const CQuaternion& q)
	{
		float norm = x * q.x + y * q.y + z * q.z + w * q.w;
		
		bool isFlip = false;
		
		if (norm < 0.0f)
		{
			norm = -norm;
			isFlip = true;
		}
		
		float invD;

		if (1.0f - norm < 0.000001f)
		{
			invD = 1.0f - d;
		}
		else
		{
			float theta = acosf(norm);
			
			float s = 1.0f / sinf(theta);
			
			invD = sinf((1.0f - d) * theta) * s;
			d = sinf(d * theta) * s;
		}
		
		if (isFlip)
		{
			d = -d;
		}
		
		x = invD * x + d * q.x;
		y = invD * y + d * q.y;
		z = invD * z + d * q.z;
		w = invD * w + d * q.w;
	}
	
	__forceinline void Clear()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 1.0f;
	}

	__forceinline void Conjugate()
	{
		x = -x;
		y = -y;
		z = -z;
	}
	
	__forceinline void Invert()
	{
		Conjugate();

		const float norm = (x*x) + (y*y) + (z*z) + (w*w);
		if (norm == 0.0f) 
		{
			return;
		}
		
		const float invNorm = 1.0f / norm;
		*this *= invNorm;
	}
	
	__forceinline void Set(float qx, float qy, float qz, float qw)
	{
		x = qx;
		y = qy;
		z = qz;
		w = qw;
	}

	__forceinline bool IsEmptyRotation() const
	{
		if ((fabsf(x) < QUAT_EPS) && (fabsf(y) < QUAT_EPS))
		{
			if ((fabsf(z) < QUAT_EPS) && (w == 1.0f))
			{
				return true;
			}

			if ((fabsf(w) < QUAT_EPS) && (z == 1.0f))
			{
				return true;
			}
		}

		return false;
	}

	void __forceinline GetEulerAngles(float& heading, float& attitude, float& bank)
	{
		heading = atan2(2.0f * y * w - 2.0f * x * z, 1.0f - 2.0f * y * y - 2.0f * z * z);
		attitude = asin(2.0f * x * y + 2.0f * z * w);
		bank = atan2(2.0f * x * w - 2.0f * y * z , 1 - 2.0f * x * x - 2.0f * z * z);
	}

	void __forceinline GetEulerAnglesInDegrees(float& heading, float& attitude, float& bank)
	{
		static const float mulKoef = (float)(180.0f / M_PI);
		heading = atan2(2.0f * y * w - 2.0f * x * z, 1.0f - 2.0f * y * y - 2.0f * z * z) * mulKoef;
		attitude = asin(2.0f * x * y + 2.0f * z * w) * mulKoef;
		bank = atan2(2.0f * x * w - 2.0f * y * z , 1 - 2.0f * x * x - 2.0f * z * z) * mulKoef;
	}


	__forceinline ~CQuaternion() 
	{
	}
};

static __forceinline CQuaternion operator*(const CQuaternion& q, const CQuaternion& r)
{
	return CQuaternion(
		r.w * q.x + r.x * q.w + r.y * q.z - r.z * q.y,
		r.w * q.y - r.x * q.z + r.y * q.w + r.z * q.x,
		r.w * q.z + r.x * q.y - r.y * q.x + r.z * q.w,
		r.w * q.w - r.x * q.x - r.y * q.y - r.z * q.z
		);
}

static __forceinline float dot( const CQuaternion& q, const CQuaternion& r )
{
	return	q.x * r.x +
			q.y * r.y +
			q.z * r.z +
			q.w * r.w;
}


static __forceinline CQuaternion shortestArc(const CVector3D& from, const CVector3D& to)
{
	CVector3D cross;
	cross.Cross(from, to);

	float dot = CVector3D::Dot(from, to);      
	dot = sqrtf(2.0f * (dot + 1.0f)) ; 
	
	cross /= dot; 
	
	return CQuaternion(cross[0], cross[1], cross[2], -dot/2) ; 

}