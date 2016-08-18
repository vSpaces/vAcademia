//****************************************************************************//
// quaternion.cpp                                                             //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#include "StdAfx.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//
#include "stdafx.h"	
#include "quaternion.h"
#include "vector.h"
#include "memory_leak.h"

 /*****************************************************************************/
/** Constructs the quaternion instance.
  *
  * This function is the default constructor of the quaternion instance.
  *****************************************************************************/

CalQuaternion::CalQuaternion() : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{
}

 /*****************************************************************************/
/** Constructs the quaternion instance.
  *
  * This function is a constructor of the quaternion instance.
  *
  * @param q The quaternion to construct this quaternion instance from.
  *****************************************************************************/

CalQuaternion::CalQuaternion(const CalQuaternion& q) : x(q.x), y(q.y), z(q.z), w(q.w)
{
}

 /*****************************************************************************/
/** Constructs the quaternion instance.
  *
  * This function is a constructor of the quaternion instance.
  *
  * @param qx The x component.
  * @param qy The y component.
  * @param qz The z component.
  * @param qw The w component.
  *****************************************************************************/

CalQuaternion::CalQuaternion(float qx, float qy, float qz, float qw) : x(qx), y(qy), z(qz), w(qw)
{
}

/** Constructs the quaternion instance.
  *
  * This function is a constructor of the quaternion instance.
  *
  * @param qx The x component.
  * @param qy The y component.
  * @param qz The z component.
  * @param qw The w component.
  *****************************************************************************/

CalQuaternion::CalQuaternion(CalVector	v, float angle)
{
	float sin_a = sin( angle / 2 );
    float cos_a = cos( angle / 2 );

    x = v.x * sin_a;
    y = v.y * sin_a;
    z = v.z * sin_a;
    w = cos_a;
    Normalize();
}

CalQuaternion::CalQuaternion(CalVector	vsource, CalVector vdest)
{
	float	dot = vsource.dot(vdest);
	if( dot > 0.9999)	
	{
		x = y = w = 0;
		z = 1;
	};
	if( dot < -0.9999)
	{
		CalVector axe;
		axe.set(0,0,1);
		/*if( vsource.x==0){	axe.set(1,0,0);}
		else	if( vsource.y==0){	axe.set(0,1,0);}
		else	if( vsource.z==0){	axe.set(0,0,1);}
		else	{ axe.set(-vsource.y,vsource.x,0);}*/
		*this = CalQuaternion(axe, 3.14f);
		return;
	}
	*this = CalQuaternion(vsource.getNormal(vdest), -vsource.getAngle(vdest));
}

CalQuaternion::CalQuaternion(const float m[4][4])
{
  float  tr, s, q[4];
  int    i, j, k;

  int nxt[3] = {1, 2, 0};

  tr = m[0][0] + m[1][1] + m[2][2];

  if (tr > 0.0f)
  {
    s = sqrt (tr + 1.0f);
    w = s / 2.0f;
    s = 0.5f / s;
    x = (m[1][2] - m[2][1]) * s;
    y = (m[2][0] - m[0][2]) * s;
    z = (m[0][1] - m[1][0]) * s;
  }
  else
  {
    i = 0;
    if (m[1][1] > m[0][0]) i = 1;
    if (m[2][2] > m[i][i]) i = 2;
    j = nxt[i];
    k = nxt[j];

    s = sqrtf((m[i][i] - (m[j][j] + m[k][k])) + 1.0f);

    q[i] = s * 0.5f;

    if (s != 0.0f) s = 0.5f / s;

    q[3] = (m[j][k] - m[k][j]) * s;
    q[j] = (m[i][j] + m[j][i]) * s;
    q[k] = (m[i][k] + m[k][i]) * s;

    x = q[0];
    y = q[1];
    z = q[2];
    w = q[3];
  }
}

 /*****************************************************************************/
/** Destructs the quaternion instance.
  *
  * This function is the destructor of the quaternion instance.
  *****************************************************************************/

CalQuaternion::~CalQuaternion()
{
}

 /*****************************************************************************/
/** Provides access to the components of the quaternion instance.
  *
  * This function provides read and write access to the three components of the
  * quaternion instance.
  *
  * @param index The index to the specific component.
  *
  * @return A reference to the specific component.
  *****************************************************************************/

float& CalQuaternion::operator[](unsigned int index)
{
  return (&x)[index];
}

 /*****************************************************************************/
/** Provides access to the components of the quaternion instance.
  *
  * This function provides read access to the three components of the quaternion
  * instance.
  *
  * @param index The index to the specific component.
  *
  * @return A constant reference to the specific component.
  *****************************************************************************/

const float& CalQuaternion::operator[](unsigned int index) const
{
  return (&x)[index];
}

 /*****************************************************************************/
/** Equates the quaternion instance with another quaternion.
  *
  * This operator equates the quaternion instance with another quaternion.
  *
  * @param q The quaternion to equate the quaternion instance with.
  *****************************************************************************/

void CalQuaternion::operator=(const CalQuaternion& q)
{
  x = q.x;
  y = q.y;
  z = q.z;
  w = q.w;
}



 /*****************************************************************************/
/** Calculates the product of two quaternions.
  *
  * This operator calculates the product of two quaternions.
  *
  * @param q The first quaternion.
  * @param r The second quaternion.
  *
  * @return The product of the two quaternions.
  *****************************************************************************/

CAL3D_API CalQuaternion operator*(const CalQuaternion& q, const CalQuaternion& r)
{
  return CalQuaternion(
    r.w * q.x + r.x * q.w + r.y * q.z - r.z * q.y,
    r.w * q.y - r.x * q.z + r.y * q.w + r.z * q.x,
    r.w * q.z + r.x * q.y - r.y * q.x + r.z * q.w,
    r.w * q.w - r.x * q.x - r.y * q.y - r.z * q.z
  );
}

 /*****************************************************************************/
/** Multiplies a vector to the quaternion instance.
  *
  * This operator multiplies a vector to the quaternion instance.
  *
  * @param v The vector to be multiplied.
  *****************************************************************************/

__forceinline void CalQuaternion::operator*=(const CalVector& v)
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


 /*****************************************************************************/
/** Interpolates the quaternion instance to another quaternion.
  *
  * This function interpolates the quaternion instance to another quaternion by
  * a given factor.
  *
  * @param d The blending factor in the range [0.0, 1.0].
  * @param v The quaternion to be interpolated to.
  *****************************************************************************/

void CalQuaternion::blend(float d, const CalQuaternion& q)
{
  float norm;
  norm = x * q.x + y * q.y + z * q.z + w * q.w;

  bool bFlip;
  bFlip = false;

  if(norm < 0.0f)
  {
    norm = -norm;
    bFlip = true;
  }

  float inv_d;
  if(1.0f - norm < 0.000001f)
  {
    inv_d = 1.0f - d;
  }
  else
  {
    float theta;
    theta = acos(norm);

    float s;
    s = 1.0f / sin(theta);

    inv_d = sin((1.0f - d) * theta) * s;
    d = sin(d * theta) * s;
  }

  if(bFlip)
  {
    d = -d;
  }

  x = inv_d * x + d * q.x;
  y = inv_d * y + d * q.y;
  z = inv_d * z + d * q.z;
  w = inv_d * w + d * q.w;
}

 /*****************************************************************************/
/** Conjugates the quaternion instance.
  *
  * This function conjugates the quaternion instance.
  *****************************************************************************/

void CalQuaternion::conjugate()
{
  x = -x;
  y = -y;
  z = -z;
}

 /*****************************************************************************/
/** Sets new values.
  *
  * This function sets new values in the quaternion instance.
  *
  * @param qx The x component.
  * @param qy The y component.
  * @param qz The z component.
  * @param qw The w component.
  *****************************************************************************/

void CalQuaternion::set(float qx, float qy, float qz, float qw)
{
  x = qx;
  y = qy;
  z = qz;
  w = qw;
}

//****************************************************************************//

CalQuaternion CalQuaternion::Normalize()
{
	CalQuaternion	rq;
	rq.x = x;
	rq.y = y;
	rq.z = z;
	rq.w = w;

	float norm = sqrt(x*x+y*y+z*z+w*w);
	if( norm == 0)
		return rq;
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

void CalQuaternion::extract(CalVector &axes, float &angle)
{
	Normalize();

    //float	cos_angle  = w;
    angle      = acos( w ) * 2/* * RADIANS*/;
    float	sin_angle  = sqrtf( 1.0f - w * w );


	//float sa;
    if ( fabs( sin_angle ) < 0.0005 )
      sin_angle = 1;

    axes.x = x/sin_angle;
    axes.y = y/sin_angle;
    axes.z = z/sin_angle;
    
}

void CalQuaternion::extractMatrix(D3DMATRIX &m, CalVector& v)
{
	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
	x2 = x + x;
	y2 = y + y;
	z2 = z + z;
	xx = x * x2;   xy = x * y2;   xz = x * z2;
	yy = y * y2;   yz = y * z2;   zz = z * z2;
	wx = w * x2;   wy = w * y2;   wz = w * z2;

	m._11=1.0f-(yy+zz); m._12=xy-wz;			m._13=xz+wy;
	m._21=xy+wz;        m._22=1.0f-(xx+zz);		m._23=yz-wx;
	m._31=xz-wy;        m._32=yz+wx;			m._33=1.0f-(xx+yy);

	m._14 = m._24 = m._34 = 0;
	m._41 = v.x;
	m._42 = v.y;
	m._43 = v.z;
	m._44 = 1;
}

void CalQuaternion::extractMatrix(D3DMATRIX &m)
{
	CalVector v;
	extractMatrix(m, v);
}

void CalQuaternion::transform(const D3DMATRIX &m)
{
	float vals[4][4] = {
		{m._11, m._12, m._13, m._14},
		{m._21, m._22, m._23, m._24},
		{m._31, m._32, m._33, m._34},
		{m._41, m._42, m._43, m._44}
	};
	CalQuaternion	mquat(vals);
	CalQuaternion	mcurrent;
	mcurrent.x = x;
	mcurrent.y = y;
	mcurrent.z = z;
	mcurrent.w = w;
	mcurrent *= mquat;
	x = mcurrent.x;
	y = mcurrent.y;
	z = mcurrent.z;
	w = mcurrent.w;
}

void CalQuaternion::clear()
{
	x = 0;
	y = 0;
	z = 0;
	w = 1;
}