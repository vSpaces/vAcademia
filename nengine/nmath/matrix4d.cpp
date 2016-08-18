
#include "StdAfx.h"
#include <math.h>
#include <memory.h>
#include "Matrix4D.h"

CMatrix4D::CMatrix4D()
{

}

CMatrix4D::CMatrix4D(float a)
{
	m[0][1] = m[0][2] = m[0][3] = m[1][0] = m[1][2] = m[1][3] =
	m[2][0] = m[2][1] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0;
	m[0][0] = m[1][1] = m[2][2] = a;
	m[3][3] = 1;
}

CMatrix4D::CMatrix4D(const CMatrix4D& a)
{
	memcpy(m, &a.m, sizeof(m));
}

CMatrix4D CMatrix4D::Inverse() const
{
	CMatrix4D m(*this);

	m.Invert();

	return m;
}

CVector3D CMatrix4D::TransformPoint(const CVector3D& b) const
{
	return CVector3D(m[0][0]*b.x + m[0][1]*b.y + m[0][2]*b.z + m[0][3],
		             m[1][0]*b.x + m[1][1]*b.y + m[1][2]*b.z + m[1][3],
					 m[2][0]*b.x + m[2][1]*b.y + m[2][2]*b.z + m[2][3]);
}

CVector4D CMatrix4D::TransformPoint(const CVector4D& b) const
{
	return CVector4D(m[0][0]*b.x + m[0][1]*b.y + m[0][2]*b.z + m[0][3]*b.w,
		             m[1][0]*b.x + m[1][1]*b.y + m[1][2]*b.z + m[1][3]*b.w,
					 m[2][0]*b.x + m[2][1]*b.y + m[2][2]*b.z + m[2][3]*b.w,
					 m[3][0]*b.x + m[3][1]*b.y + m[3][2]*b.z + m[3][3]*b.w);
}

CVector3D CMatrix4D::TransformDirection(const CVector3D& b) const
{
	return CVector3D(m[0][0]*b.x + m[0][1]*b.y + m[0][2]*b.z,
		             m[1][0]*b.x + m[1][1]*b.y + m[1][2]*b.z,
	                 m[2][0]*b.x + m[2][1]*b.y + m[2][2]*b.z);
}

CMatrix4D& CMatrix4D::operator =(const CMatrix4D& a)
{
	memcpy(m, &a.m, sizeof(m));

	return *this;
}

CMatrix4D& CMatrix4D::operator +=(const CMatrix4D& a)
{
	for (register int i = 0; i < 4; i++)
	for (register int j = 0; j < 4; j++)
	{
		m[i][j] += a.m[i][j];
	}
	
	return *this;
}

CMatrix4D& CMatrix4D::operator -=( const CMatrix4D& a )
{
	for (register int i = 0; i < 4; i++)
	for (register int j = 0; j < 4; j++)
	{
		m[i][j] -= a.m[i][j];
	}

	return *this;
}

CMatrix4D& CMatrix4D::operator *=(const CMatrix4D& a)
{
	float t[4][4];

	for (register int i = 0; i < 4; i++)
	for (register int j = 0; j < 4; j++)
	{
		t[i][j] = m[i][0] * a.m[0][j] + m[i][1] * a.m[1][j] + m[i][2] * a.m[2][j] + m[i][3] * a.m[3][j];
	}

	memcpy(m, t, sizeof(m));

	return *this;
}

CMatrix4D& CMatrix4D::operator *=(float a)
{
	for (register int i = 0; i < 4; i++)
	for (register int j = 0; j < 4; j++)
	{
		m[i][j] *= a;
	}

	return *this;
}

CMatrix4D& CMatrix4D::operator /=(float a)
{
	for (register int i = 0; i < 4; i++)
	for (register int j = 0; j < 4; j++)
	{
		m[i][j] /= a;
	}

	return *this;
};

CMatrix4D&	CMatrix4D::Transpose()
{
	CMatrix4D a;

	for (register int i = 0; i < 4; i++)
	for (register int j = 0; j < 4; j++)
	{
		a.m [i][j] = m [j][i];
	}

	return (*this = a);
}

CMatrix4D operator +(const CMatrix4D& a, const CMatrix4D& b)
{
	CMatrix4D c;

	for (register int i = 0; i < 4; i++)
	for (register int j = 0; j < 4; j++)
	{
		c.m[i][j] = a.m[i][j] + b.m[i][j];
	}

	return c;
}

CMatrix4D operator -(const CMatrix4D& a, const CMatrix4D& b)
{
	CMatrix4D c;

	for (register int i = 0; i < 4; i++)
	for (register int j = 0; j < 4; j++)
	{
		c.m[i][j] = a.m[i][j] - b.m[i][j];
	}

	return c;
}

CMatrix4D operator *(const CMatrix4D& a, const CMatrix4D& b)
{
	CMatrix4D c;

	for (register int i = 0; i < 4; i++)
	for (register int j = 0; j < 4; j++)
	{
		c.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + 
			a.m[i][3] * b.m[3][j];
	}

	return c;
}


CVector3D operator *(const CMatrix4D& a, const CVector3D& b)
{
	return CVector3D(
		a.m[0][0] * b.x + a.m[0][1] * b.y + a.m[0][2] * b.z,
		a.m[1][0] * b.x + a.m[1][1] * b.y + a.m[1][2] * b.z,
		a.m[2][0] * b.x + a.m[2][1] * b.y + a.m[2][2] * b.z
	);
}

CMatrix4D operator *(const CMatrix4D& a, float b)
{
	CMatrix4D c(a);

	return (c *= b);
}

CMatrix4D operator * (float b, const CMatrix4D& a)
{
	CMatrix4D c(a);

	return (c *= b);
}

CMatrix4D	CMatrix4D::Identity()
{
	return CMatrix4D(1);
}

CMatrix4D CMatrix4D::Scale(const CVector3D& s, float d)
{
	CMatrix4D a(1);

	a.m[0][0] = s.x;
	a.m[1][1] = s.y;
	a.m[2][2] = s.z;
	a.m[3][3] = d;

	return a;
}

void CMatrix4D::Translate(const CVector3D& s)
{
	CMatrix4D a(1);

	a.m[3][0] = s.x;
	a.m[3][1] = s.y;
	a.m[3][2] = s.z;

	(*this) *= a;
}

CMatrix4D CMatrix4D ::RotateX(float angle)
{
	CMatrix4D a(1);
	float cs = (float)cos(angle);
	float sn = (float)sin(angle);

	a.m[1][1] = cs;
	a.m[1][2] = sn;
	a.m[2][1] = -sn;
	a.m[2][2] = cs;

	return a;
}

CMatrix4D CMatrix4D::RotateY(float angle)
{
	CMatrix4D a(1);
	float cs = (float)cos(angle);
	float sn = (float)sin(angle);

	a.m[0][0] = cs;
	a.m[0][2] = -sn;
	a.m[2][0] = sn;
	a.m[2][2] = cs;

	return a;
}

CMatrix4D CMatrix4D::RotateZ(float angle)
{
	CMatrix4D a(1);
	float cs = (float)cos(angle);
	float sn = (float)sin(angle);

	a.m[0][0] = cs;
	a.m[0][1] = sn;
	a.m[1][0] = -sn;
	a.m[1][1] = cs;

	return a;
}

CMatrix4D CMatrix4D::Rotate(const CVector3D& vv, float angle)
{
	CVector3D v = vv;
	v.Normalize();
	float half_angle = angle*0.5f;
    float sin_a = (float)sin(half_angle);
	float x = v.x * sin_a;
	float y = v.y * sin_a;
	float z = v.z * sin_a;
    float w = (float)cos(half_angle);

	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
    float s  = 2.0f/ (x * x + y * y + z * z + w * w);
    x2 = x * s;    y2 = y * s;    z2 = z * s;
    xx = x * x2;   xy = x * y2;   xz = x * z2;
    yy = y * y2;   yz = y * z2;   zz = z * z2;
    wx = w * x2;   wy = w * y2;   wz = w * z2;

	CMatrix4D a;

    a.m[0][0] = 1.0f - (yy + zz);
    a.m[1][0] = xy - wz;
    a.m[2][0] = xz + wy;
	

	a.m[0][1] = xy + wz;
	a.m[1][1] = 1.0f - (xx + zz);
	a.m[2][1] = yz - wx;

	a.m[0][2] = xz - wy;
	a.m[1][2] = yz + wx;
	a.m[2][2] = 1.0f - (xx + yy);

	a.m [0][3] = 0;
	a.m [1][3] = 0;
	a.m [2][3] = 0;

	a.m [3][0] =
	a.m [3][1] =
	a.m [3][2] = 0;
	a.m [3][3] = 1;

	/*
	float cs = (float)cos(angle);
	float sn = (float)sin(angle);

	a.m [0][0] = v.x * v.x + (1 - v.x * v.x) * cs;
	a.m [0][1] = v.x * v.y * (1 - cs) + v.z * sn;
	a.m [0][2] = v.x * v.z * (1 - cs) - v.y * sn;
	a.m [0][3] = 0;
	a.m [1][0] = v.x * v.y * (1 - cs) - v.z * sn;
	a.m [1][1] = v.y * v.y + (1 - v.y * v.y) * cs;
	a.m [1][2] = v.y * v.z * (1 - cs) + v.x * sn;
	a.m [1][3] = 0;
	a.m [2][0] = v.x * v.z * (1 - cs) + v.y * sn;
	a.m [2][1] = v.y * v.z * (1 - cs) - v.x * sn;
	a.m [2][2] = v.z * v.z + (1 - v.z * v.z) * cs;
	a.m [2][3] = 0;
	a.m [3][0] =
	a.m [3][1] =
	a.m [3][2] = 0;
	a.m [3][3] = 1;*/

	return a;
}

CMatrix4D CMatrix4D::Rotate(float yaw, float pitch, float roll)
{
    return RotateY(yaw) * RotateZ(roll) * RotateX(pitch);
}

CMatrix4D CMatrix4D::MirrorX()
{
	CMatrix4D a(1);

	a.m[0][0] = -1;

	return a;
}

CMatrix4D CMatrix4D::MirrorY()
{
	CMatrix4D a(1);

	a.m[1][1] = -1;

	return a;
}

CMatrix4D CMatrix4D::MirrorZ()
{
	CMatrix4D a(1);

	a.m [2][2] = -1;

	return a;
}


bool CMatrix4D::Invert()
{
	#define SWAP_ROWS(a, b)	{float * _tmp = a; (a)=(b); (b)=_tmp; }
	#define MAT(m, r, c) m[r][c]

	float	wtmp [4][8];
	float	m0, m1, m2, m3, s;
	float	*r0, *r1, *r2, *r3;

	r0 = wtmp[0];
	r1 = wtmp[1];
	r2 = wtmp[2];
	r3 = wtmp[3];

	r0[0] = MAT(m, 0, 0);
	r0[1] = MAT(m, 0, 1);
	r0[2] = MAT(m, 0, 2);
	r0[3] = MAT(m, 0, 3);
	r0[4] = 1;
	r0[5] =	r0[6] =	r0[7] = 0;

	r1[0] = MAT(m, 1, 0);
	r1[1] = MAT(m, 1, 1);
	r1[2] = MAT(m, 1, 2);
	r1[3] = MAT(m, 1, 3);
	r1[5] = 1;
	r1[4] =	r1[6] =	r1[7] = 0,

	r2[0] = MAT(m, 2, 0);
	r2[1] = MAT(m, 2, 1);
	r2[2] = MAT(m, 2, 2);
	r2[3] = MAT(m, 2, 3);
	r2[6] = 1;
	r2[4] =	r2[5] =	r2[7] = 0;

	r3[0] = MAT(m, 3, 0);
	r3[1] = MAT(m, 3, 1);
	r3[2] = MAT(m, 3, 2);
	r3[3] = MAT(m, 3, 3);
	r3[7] = 1;
	r3[4] = r3[5] = r3[6] = 0;

											// choose pivot - or die
	if (fabs(r3[0]) > fabs(r2[0]))
	{
		SWAP_ROWS(r3, r2);
	}

	if (fabs(r2[0]) > fabs(r1[0]))
	{
		SWAP_ROWS(r2, r1);
	}

	if (fabs(r1[0]) > fabs(r0[0]))
	{
		SWAP_ROWS(r1, r0);
	}

	if (r0 [0] == 0)
	{
		return false;
	}

	m1 = r1[0] / r0[0];
	m2 = r2[0] / r0[0];
	m3 = r3[0] / r0[0];

	s = r0[1];
	r1[1] -= m1 * s;
	r2[1] -= m2 * s;
	r3[1] -= m3 * s;

	s = r0[2];
	r1[2] -= m1 * s;
	r2[2] -= m2 * s;
	r3[2] -= m3 * s;

	s = r0[3];
	r1[3] -= m1 * s;
	r2[3] -= m2 * s;
	r3[3] -= m3 * s;

	s = r0[4];

	if (s != 0)
	{
		r1[4] -= m1 * s; 
		r2[4] -= m2 * s; 
		r3[4] -= m3 * s;
	}

	s = r0[5];

	if (s != 0)
	{
		r1[5] -= m1 * s; 
		r2[5] -= m2 * s; 
		r3[5] -= m3 * s;
	}

	s = r0[6];

	if (s != 0)
	{
		r1[6] -= m1 * s; 
		r2[6] -= m2 * s; 
		r3[6] -= m3 * s;
	}

	s = r0[7];

	if (s != 0)
	{
		r1[7] -= m1 * s; 
		r2[7] -= m2 * s; 
		r3[7] -= m3 * s;
	}
									
	if (fabs(r3[1]) > fabs(r2[1]))
	{
		SWAP_ROWS(r3, r2);
	}

	if (fabs(r2[1]) > fabs(r1[1]))
	{
		SWAP_ROWS(r2, r1);
	}

	if (r1[1] == 0)
	{
		return false;
	}

	m2 = r2[1] / r1[1]; 
	m3 = r3[1] / r1[1];
	r2[2] -= m2 * r1[2];  
	r3[2] -= m3 * r1[2];
	r2[3] -= m2 * r1[3];  
	r3[3] -= m3 * r1[3];

	s = r1[4];

	if (0 != s)
	{
		r2[4] -= m2 * s; 
		r3[4] -= m3 * s;
	}

	s = r1[5];

	if (0 != s)
	{
		r2[5] -= m2 * s; 
		r3[5] -= m3 * s;
	}

	s = r1[6];

	if ( 0 != s )
	{
		r2[6] -= m2 * s; 
		r3[6] -= m3 * s;
	}

	s = r1[7];

	if (0 != s)
	{
		r2[7] -= m2 * s; 
		r3[7] -= m3 * s;
	}

	if (fabs(r3[2]) > fabs(r2[2]))
	{
		SWAP_ROWS(r3, r2);
	}

	if (r2[2] == 0)
	{
	    return false;
	}

	m3 = r3[2] / r2[2];
	r3[3] -= m3 * r2[3];
	r3[4] -= m3 * r2[4];
	r3[5] -= m3 * r2[5]; 
	r3[6] -= m3 * r2[6];
	r3[7] -= m3 * r2[7];

	if (r3[3] == 0)
	{
		return false;
	}


	s = 1 / r3[3];
	r3[4] *= s; 
	r3[5] *= s; 
	r3[6] *= s; 
	r3[7] *= s;

	m2 = r2[3];
	s = 1 / r2[2];
	r2[4] = s * (r2[4] - r3[4] * m2); 
	r2[5] = s * (r2[5] - r3[5] * m2);
	r2[6] = s * (r2[6] - r3[6] * m2); 
	r2[7] = s * (r2[7] - r3[7] * m2);

	m1 = r1[3];
	r1[4] -= r3[4] * m1; 
	r1[5] -= r3[5] * m1;
	r1[6] -= r3[6] * m1; 
	r1[7] -= r3[7] * m1;

	m0 = r0[3];
	r0[4] -= r3[4] * m0; 
	r0[5] -= r3[5] * m0,
	r0[6] -= r3[6] * m0; 
	r0[7] -= r3[7] * m0;

	m1 = r1[2];
	s = 1 / r1[1];
	r1[4] = s * (r1[4] - r2[4] * m1);
	r1[5] = s * (r1[5] - r2[5] * m1);
	r1[6] = s * (r1[6] - r2[6] * m1);
	r1[7] = s * (r1[7] - r2[7] * m1);

	m0 = r0[2];
	r0[4] -= r2[4] * m0; 
	r0[5] -= r2[5] * m0,
	r0[6] -= r2[6] * m0; 
	r0[7] -= r2[7] * m0;

	m0 = r0[1];
	s = 1 / r0[0];
	r0[4] = s * (r0[4] - r1[4] * m0); 
	r0[5] = s * (r0[5] - r1[5] * m0);
	r0[6] = s * (r0[6] - r1[6] * m0); 
	r0[7] = s * (r0[7] - r1[7] * m0);

	MAT(m, 0, 0) = r0[4]; 
	MAT(m, 0, 1) = r0[5],
	MAT(m, 0, 2) = r0[6]; 
	MAT(m, 0, 3) = r0[7],
	MAT(m, 1, 0) = r1[4]; 
	MAT(m, 1, 1) = r1[5],
	MAT(m, 1, 2) = r1[6]; 
	MAT(m, 1, 3) = r1[7],
	MAT(m, 2, 0) = r2[4]; 
	MAT(m, 2, 1) = r2[5],
	MAT(m, 2, 2) = r2[6]; 
	MAT(m, 2, 3) = r2[7],
	MAT(m, 3, 0) = r3[4]; 
	MAT(m, 3, 1) = r3[5],
	MAT(m, 3, 2) = r3[6]; 
	MAT(m, 3, 3) = r3[7];

#undef MAT
#undef SWAP_ROWS

	return true;
}

