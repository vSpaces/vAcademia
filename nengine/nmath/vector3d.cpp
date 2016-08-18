
#include "StdAfx.h"
#include <math.h>
#include "Vector3d.h"
#include "Quaternion3d.h"
#include "CommonFunctions.h"

void CVector3D::operator *=(CQuaternion a)
{
	CQuaternion temp(-a.x, -a.y, -a.z, a.w);
	temp *= *this;
	temp *= a;

	x = temp.x;
	y = temp.y;
	z = temp.z;
}

CVector3D CVector3D::operator *(CQuaternion q)
{
	CVector3D res(x, y, z);
	res *= q;

	return res;
}

void CVector3D::RotateZ(float angle)
{
	float cs = (float)cos(GradToRadians(angle));
	float sn = (float)sin(GradToRadians(angle));

	float xx = x * cs - y * sn;
	y = x * sn + y * cs;
	x = xx;
}

void CVector3D::RotateX(float angle)
{
	float cs = (float)cos(GradToRadians(angle));
	float sn = (float)sin(GradToRadians(angle));

	float yy = y * cs - z * sn;
	z = y * sn + z * cs;
	y = yy;
}

void CVector3D::RotateY(float angle)
{
	float cs = (float)cos(GradToRadians(-angle));
	float sn = (float)sin(GradToRadians(-angle));

	float xx = x * cs - z * sn;
	z = x * sn + z * cs;
	x = xx;
}

float CVector3D::GetAngleY(CVector3D vec)
{
	float cs = x * vec.x + z * vec.z;
	cs /= sqrtf(x * x + z * z);
	cs /= sqrtf(vec.x * vec.x + vec.z * vec.z);
	return acosf(cs);
}

float CVector3D::GetAngleZ(CVector3D vec)
{
	float cs = x * vec.x + y * vec.y;
	cs /= sqrtf(x * x + y * y);
	cs /= sqrtf(vec.x * vec.x + vec.y * vec.y);
	return acosf(cs);
}