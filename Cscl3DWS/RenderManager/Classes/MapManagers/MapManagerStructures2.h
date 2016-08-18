#pragma once

#include "MapManagerStructures.h"
#include "vector.h"
#include "quaternion.h"

inline CalVector& ToCalVector( CalVector& left, const CMapObjectVector& right)
{
	left.x = right.x;
	left.y = right.y;
	left.z = right.z;

	return left;
}

inline CalVector ToCalVector( const CMapObjectVector& val)
{
	CalVector v;
	return ToCalVector( v, val);
}

inline CMapObjectQuaternion& operator*=(CMapObjectQuaternion& q, const CalVector& v)
{
	float qx, qy, qz, qw; 
	qx = q.x; qy = q.y; qz = q.z; qw = q.w;

	q.x = qw * v.x            + qy * v.z - qz * v.y;
	q.y = qw * v.y - qx * v.z            + qz * v.x;
	q.z = qw * v.z + qx * v.y - qy * v.x;
	q.w =          - qx * v.x - qy * v.y - qz * v.z;

	return q;
}

inline CalVector& operator*=( CalVector& v, const CMapObjectQuaternion& q)
{
	CMapObjectQuaternion temp(-q.x, -q.y, -q.z, q.w);
	temp *= v;
	temp *= q;

	v.x = temp.x;
	v.y = temp.y;
	v.z = temp.z;

	return v;
}
