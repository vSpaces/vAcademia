//****************************************************************************//
// vector.cpp                                                                 //
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
#include "vector.h"
#include "quaternion.h"
#include "memory_leak.h"

 /*****************************************************************************/
/** Constructs the vector instance.
  *
  * This function is the default constructor of the vector instance.
  *****************************************************************************/

CalVector::CalVector() : x(0.0f), y(0.0f), z(0.0f)
{
}

 /*****************************************************************************/
/** Constructs the vector instance.
  *
  * This function is a constructor of the vector instance.
  *
  * @param v The vector to construct this vector instance from.
  *****************************************************************************/

CalVector::CalVector(const CalVector& v) : x(v.x), y(v.y), z(v.z)
{
}

 /*****************************************************************************/
/** Constructs the vector instance.
  *
  * This function is a constructor of the vector instance.
  *
  * @param vx The x component.
  * @param vy The y component.
  * @param vz The z component.
  *****************************************************************************/

CalVector::CalVector(float vx, float vy, float vz) : x(vx), y(vy), z(vz)
{
}

CalVector::CalVector(D3DVECTOR v) : x(v.x), y(v.y), z(v.z)
{
}

 /*****************************************************************************/
/** Destructs the vector instance.
  *
  * This function is the destructor of the vector instance.
  *****************************************************************************/

CalVector::~CalVector()
{
}

 /*****************************************************************************/
/** Provides access to the components of the vector instance.
  *
  * This function provides read and write access to the three components of the
  * vector instance.
  *
  * @param i The index to the specific component.
  *
  * @return A reference to the specific component.
  *****************************************************************************/

float& CalVector::operator[](unsigned int i)
{
  return (&x)[i];
}

 /*****************************************************************************/
/** Provides access to the components of the vector instance.
  *
  * This function provides read access to the three components of the vector
  * instance.
  *
  * @param i The index to the specific component.
  *
  * @return A constant reference to the specific component.
  *****************************************************************************/

const float& CalVector::operator[](unsigned int i) const
{
  return (&x)[i];
}

 /*****************************************************************************/
/** Equates the vector instance with another vector.
  *
  * This operator equates the vector instance with another vector.
  *
  * @param v The vector to equate the vector instance with.
  *****************************************************************************/

void CalVector::operator=(const CalVector& v)
{
  x = v.x;
  y = v.y;
  z = v.z;
}

/*void CalVector::operator=(const D3DXVECTOR3& v)
{
  x = v.x;
  y = v.y;
  z = v.z;
}*/

int CalVector::operator==(const CalVector& v)
{
  return !(x!=v.x || y!=v.y || z!=v.z);
}

int CalVector::operator!=(const CalVector& v)
{
  return (x!=v.x || y!=v.y || z!=v.z);
}

/*bool CalVector::operator==(const CalVector& v1, const CalVector& v2)
{
  if( v1.x!=v2.x || v1.y!=v2.y || v1.z!=v2.z) return false;
  return true;
}*/

 /*****************************************************************************/
/** Adds another vector to the vector instance.
  *
  * This operator adds another vector to the vector instance.
  *
  * @param v The vector to be added.
  *****************************************************************************/

void CalVector::operator+=(const CalVector& v)
{
  x += v.x;
  y += v.y;
  z += v.z;
}

void CalVector::operator+=(const float v)
{
  x += v;
  y += v;
  z += v;
}
/*CalVector CalVector::operator+(const CalVector& v, const float f)
{
  x = v.x + f;
  y = v.y + f;
  z = v.z + f;
}*/

 /*****************************************************************************/
/** Subtracts another vector from the vector instance.
  *
  * This operator subtracts another vector from the vector instance.
  *
  * @param v The vector to be subtracted.
  *****************************************************************************/

void CalVector::operator-=(const CalVector& v)
{
  x -= v.x;
  y -= v.y;
  z -= v.z;
}
void CalVector::operator-=(const float v)
{
  x -= v;
  y -= v;
  z -= v;
}
/*CalVector CalVector::operator-(const CalVector& v, const float f)
{
  x = v.x - f;
  y = v.y - f;
  z = v.z - f;
}*/
 /*****************************************************************************/
/** Scales the vector instance.
  *
  * This operator scales the vector instance by multiplying its components by
  * a specific factor.
  *
  * @param d The factor to multiply the vector components by.
  *****************************************************************************/

void CalVector::operator*=(const float d)
{
  x *= d;
  y *= d;
  z *= d;
}

 /*****************************************************************************/
/** Transforms the vector instance by a quaternion.
  *
  * This function transforms the vector instance by a given quaternion.
  *
  * @param q The quaternion to be used for the transformation.
  *****************************************************************************/
CalVector operator*(const CalVector& v, const CalQuaternion& q)
{
	CalVector res = v;
	res *= q;
	return res;
}

void CalVector::operator*=(const CalQuaternion& q)
{
  CalQuaternion temp(-q.x, -q.y, -q.z, q.w);
  temp *= *this;
  temp *= q;

  x = temp.x;
  y = temp.y;
  z = temp.z;
}

 /*****************************************************************************/
/** Scales the vector instance.
  *
  * This operator scales the vector instance by dividing its components by a
  * specific factor.
  *
  * @param d The factor to divide the vector components by.
  *****************************************************************************/

void CalVector::operator/=(const float d)
{
  x /= d;
  y /= d;
  z /= d;
}

 /*****************************************************************************/
/** Calculates the sum of two vectors.
  *
  * This operator calculates the sum of two vectors.
  *
  * @param v The first vector to be added.
  * @param u The second vector to be added.
  *
  * @return The sum of the two vectors.
  *****************************************************************************/

CAL3D_API CalVector operator+(const CalVector& v, const CalVector& u)
{
  return CalVector(v.x + u.x, v.y + u.y, v.z + u.z);
}

 /*****************************************************************************/
/** Calculates the difference of two vectors.
  *
  * This operator calculates the difference of two vectors.
  *
  * @param v The first vector to be added.
  * @param u The second vector to be subtracted.
  *
  * @return The difference of the two vectors.
  *****************************************************************************/

CAL3D_API CalVector operator-(const CalVector& v, const CalVector& u)
{
   return CalVector(v.x - u.x, v.y - u.y, v.z - u.z);
}

 /*****************************************************************************/
/** Calculates a scaled vector.
  *
  * This operator calculates the vector multiplied by a factor.
  *
  * @param v The vector to be scaled.
  * @param d The factor to multiply the vector with.
  *
  * @return The scaled vector.
  *****************************************************************************/

CAL3D_API CalVector operator*(const CalVector& v, float d)
{
  return CalVector(v.x * d, v.y * d, v.z * d);
}

CAL3D_API CalVector operator+(const CalVector& v, float d)
{
  return CalVector(v.x + d, v.y + d, v.z + d);
}

CAL3D_API CalVector operator-(const CalVector& v, float d)
{
  return CalVector(v.x - d, v.y - d, v.z - d);
}

 /*****************************************************************************/
/** Calculates a scaled vector.
  *
  * This operator calculates the vector multiplied by a factor.
  *
  * @param d The factor to multiply the vector with.
  * @param v The vector to be scaled.
  *
  * @return The scaled vector.
  *****************************************************************************/

CAL3D_API CalVector operator*(float d, const CalVector& v)
{
  return CalVector(v.x * d, v.y * d, v.z * d);
}

 /*****************************************************************************/
/** Calculates a scaled vector.
  *
  * This operator calculates the vector divided by a factor.
  *
  * @param v The vector to be scaled.
  * @param d The factor to divide the vector with.
  *
  * @return The scaled vector.
  *****************************************************************************/

CAL3D_API CalVector operator/(const CalVector& v, float d)
{
  return CalVector(v.x / d, v.y / d, v.z / d);
}

 /*****************************************************************************/
/** Calculates the dot product of two vectors.
  *
  * This operator calculates the dot product of two vectors.
  *
  * @param v The first vector.
  * @param u The second vector.
  *
  * @return The dot product of the two vectors.
  *****************************************************************************/

CAL3D_API float operator*(const CalVector& v, const CalVector& u)
{
  return v.x * u.x + v.y * u.y + v.z * u.z;
}

 /*****************************************************************************/
/** Calculates the vector product of two vectors.
  *
  * This operator calculates the vector product of two vectors.
  *
  * @param v The first vector.
  * @param u The second vector.
  *
  * @return The vector product of the two vectors.
  *****************************************************************************/

CAL3D_API CalVector operator%(const CalVector& v, const CalVector& u)
{
  return CalVector(v.y * u.z - v.z * u.y, v.z * u.x - v.x * u.z, v.x * u.y - v.y * u.x);
}

 /*****************************************************************************/
/** Interpolates the vector instance to another vector.
  *
  * This function interpolates the vector instance to another vector by a given
  * factor.
  *
  * @param d The blending factor in the range [0.0, 1.0].
  * @param v The vector to be interpolated to.
  *****************************************************************************/

void CalVector::blend(float d, const CalVector& v)
{
  x += d * (v.x - x);
  y += d * (v.y - y);
  z += d * (v.z - z);
}

 /*****************************************************************************/
/** Clears the vector instance.
  *
  * This function clears the vector instance.
  *****************************************************************************/

void CalVector::clear()
{
  x = 0.0f;
  y = 0.0f;
  z = 0.0f;
}

 /*****************************************************************************/
/** Returns the length of the vector instance.
  *
  * This function returns the length of the vector instance.
  *
  * @return The length of the vector instance.
  *****************************************************************************/

float CalVector::length()
{
  return sqrtf(x * x + y * y + z * z);
}

float CalVector::lengthsq()
{
  return x * x + y * y + z * z;
}

 /*****************************************************************************/
/** Normalizes the vector instance.
  *
  * This function normalizes the vector instance and returns its former length.
  *
  * @return The length of the vector instance before normalizing.
  *****************************************************************************/

float CalVector::normalize()
{
  // calculate the length of the vector
  float length = sqrtf(x * x + y * y + z * z);

  if( length == 0) return 0;

  // normalize the vector
  x /= length;
  y /= length;
  z /= length;

  return length;
}

CalVector CalVector::normalized()
{
  // calculate the length of the vector
  float length =sqrtf(x * x + y * y + z * z);

  if( length == 0) return CalVector(0,0,0);

  // normalize the vector
  x /= length;
  y /= length;
  z /= length;

  return *this;
}

 /*****************************************************************************/
/** Sets new values.
  *
  * This function sets new values in the vector instance.
  *
  * @param x The x component.
  * @param y The y component.
  * @param z The z component.
  *****************************************************************************/

void CalVector::set(float vx, float vy, float vz)
{
  x = vx;
  y = vy;
  z = vz;
}

//****************************************************************************//

CalVector CalVector::getNormal(CalVector second)
{
	CalVector	ret;
	ret.x = y*second.z - z*second.y;
	ret.y = z*second.x - x*second.z;
	ret.z = x*second.y - y*second.x;
	ret.normalize();
	return ret;
}

float CalVector::getAngle( CalVector second)
{
	float	arg = (x*second.x+y*second.y+z*second.z)/(length()*second.length());
	if( arg >= 1)
		return 0.0;
	if( arg <= -1)
		return 3.141692f;	
	return acosf( arg);
}

void CalVector::invert()
{
	x *= -1;
	y *= -1;
	z *= -1;
}

void CalVector::project(CALPLANES plane)
{
	if( plane == YOZ){ x = 0; return;}
	if( plane == XOZ){ y = 0; return;}
	if( plane == XOY){ z = 0; return;}
}

float CalVector::dot(const CalVector &v) const
{
	return x*v.x+y*v.y+z*v.z;
}

CalVector CalVector::cross(const CalVector &v) const
{
	CalVector ret;
	ret.x = y * v.z - z * v.y;
	ret.y = z * v.x - x * v.z;
	ret.z = x * v.y - y * v.x;
	return ret;
}

CalVector CalVector::cross(const CalVector &v1, const CalVector &v2)
{
	x = v1.y * v2.z - v1.z * v2.y;
	y = v1.z * v2.x - v1.x * v2.z;
	z = v1.x * v2.y - v1.y * v2.x;
	return *this;
}

bool CalVector::coplanar(const CalVector &v)
{
	return (getAngle(v)==0) ? true:false;
}

bool CalVector::undef()
{
	if( x==FLT_MAX || y==FLT_MAX || z==FLT_MAX) return true;
	return false;
}

CalVector CalVector::transform(D3DMATRIX* m)
{
	float xr,yr,zr,wr;
	xr = x*m->_11+y*m->_21+z*m->_31+m->_41;
	yr = x*m->_12+y*m->_22+z*m->_32+m->_42;
	zr = x*m->_13+y*m->_23+z*m->_33+m->_43;
	wr = x*m->_14+y*m->_24+z*m->_34+m->_44;
	x=xr/wr;	y=yr/wr;	z=zr/wr;
	return *this;
}

float CalVector::clockwise_dot(CalVector &v, CalVector &n)
{
	float dt=dot(v),f;
	CalVector v1;
	v1 = cross(v);
	f=v1.dot(n);

	if(f<0)	dt=-dt-2;

	return dt;
}

CalVector CalVector::project(const CalVector& v)
{
	CalVector	vc(*this);	vc.normalize();
	float len = vc.dot(v);
	return vc*len;
}

CalVector CalVector::project(const CalVector& p1, const CalVector& p2)
{
	CalVector	rd = (p2-p1);	rd.normalize();
	return p1 + rd*rd.dot(*this - p1);
}
