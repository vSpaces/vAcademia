//****************************************************************************//
// vector.h                                                                   //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_VECTOR_H
#define CAL_VECTOR_H

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "global.h"

//****************************************************************************//
// Forward declarations                                                       //
//****************************************************************************//

class CalQuaternion;

//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//

 /*****************************************************************************/
/** The vector class.
  *****************************************************************************/
class CAL3D_API CalVector
{
// member variables
public:
  float x;
  float y;
  float z;

// constructors/destructor
public:
	CalVector();
	CalVector(const CalVector& v);
	CalVector(float vx, float vy, float vz);
	CalVector(D3DVECTOR v);
	~CalVector();

// member functions
public:
	CalVector project(const CalVector& p1, const CalVector& p2);
	CalVector project(const CalVector& v);
	float clockwise_dot(CalVector &v, CalVector &n);
	CalVector transform(D3DMATRIX* m);
	bool undef();
	bool coplanar(const CalVector& v);
	float dot(const CalVector &v) const;
	CalVector cross(const CalVector &v) const;
	CalVector cross(const CalVector &v1, const CalVector &v2);
	void project(CALPLANES plane);
	void invert();
	CalVector getNormal( CalVector second);
	float getAngle( CalVector second);
	float& operator[](unsigned int i);
	const float& operator[](unsigned int i) const;
	int operator==(const CalVector& v);
	int operator!=(const CalVector& v);
	void operator=(const CalVector& v);
	void operator+=(const CalVector& v);
	void operator-=(const CalVector& v);
	void operator+=(const float v);
	void operator-=(const float v);
	void operator*=(const float d);
	void operator*=(const CalQuaternion& q);
	void operator/=(const float d);

	friend CAL3D_API CalVector operator+(const CalVector& v, const CalVector& u);
	friend CAL3D_API CalVector operator-(const CalVector& v, const CalVector& u);
	friend CAL3D_API CalVector operator+(const CalVector& v, const float u);
	friend CAL3D_API CalVector operator-(const CalVector& v, const float u);
	friend CAL3D_API CalVector operator*(const CalVector& v, const float d);
	friend CAL3D_API CalVector operator*(const CalVector& v, const CalQuaternion& q);
	friend CAL3D_API CalVector operator*(const float d, const CalVector& v);
	friend CAL3D_API CalVector operator/(const CalVector& v, const float d);
	friend CAL3D_API float operator*(const CalVector& v, const CalVector& u);
	friend CAL3D_API CalVector operator%(const CalVector& v, const CalVector& u);

	void	minimize(const CalVector& v){ if(x>v.x) x=v.x;	if(y>v.y) y=v.y;	if(z>v.z) z=v.z;}
	void	maximize(const CalVector& v){ if(x<v.x) x=v.x;	if(y<v.y) y=v.y;	if(z<v.z) z=v.z;}
	void blend(float d, const CalVector& v);
	void clear();
	float length();
	float lengthsq();
	float normalize();
	CalVector normalized();
	void set(float vx, float vy, float vz);
};

#endif

//****************************************************************************//
