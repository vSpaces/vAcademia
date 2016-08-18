/************************************************************************/
/* a * b = c
/*
 *	a = c * b`
	b = a` * c
 */
/************************************************************************/


//****************************************************************************//
// quaternion.h                                                               //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_QUATERNION_H
#define CAL_QUATERNION_H

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "global.h"

//****************************************************************************//
// Forward declarations                                                       //
//****************************************************************************//

class CalVector;

//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//

 /*****************************************************************************/
/** The quaternion class.
  *****************************************************************************/

class CAL3D_API CalQuaternion
{
// member variables
public:
  float x;
  float y;
  float z;
  float w;

// constructors/destructor
public:
  CalQuaternion();
  CalQuaternion(const CalQuaternion& q);
  CalQuaternion(float qx, float qy, float qz, float qw);
  CalQuaternion(CalVector	v, float angle);
  CalQuaternion(CalVector	vsource, CalVector vdest);
  CalQuaternion(const float m[4][4]);
  ~CalQuaternion();

// member functions	
public:
  void extractMatrix(D3DMATRIX& m);
  void extractMatrix(D3DMATRIX &m, CalVector& v_);
  void extract(CalVector& axes, float& angle);
  void transform(const D3DMATRIX &m);
  CalQuaternion Normalize();
  float& operator[](unsigned int index);
  const float& operator[](unsigned int index) const;
  void operator=(const CalQuaternion& q);

   /*****************************************************************************/
/** Multiplies another quaternion to the quaternion instance.
  *
  * This operator multiplies another quaternion to the quaternion instance.
  *
  * @param q The quaternion to be multiplied.
  *****************************************************************************/

__forceinline void operator*=(const CalQuaternion& q)
{
  float qx, qy, qz, qw;
  qx = x;
  qy = y;
  qz = z;
  qw = w;

  x = qw * q.x + qx * q.w + qy * q.z - qz * q.y;
  y = qw * q.y - qx * q.z + qy * q.w + qz * q.x;
  z = qw * q.z + qx * q.y - qy * q.x + qz * q.w;
  w = qw * q.w - qx * q.x - qy * q.y - qz * q.z;
}

void operator*=(const CalVector& v);

  friend CAL3D_API CalQuaternion operator*(const CalQuaternion& q, const CalQuaternion& r);
  void blend(float d, const CalQuaternion& q);
  void conjugate();
  void set(float qx, float qy, float qz, float qw);
  void clear();
};

#endif

//****************************************************************************//
