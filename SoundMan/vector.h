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
// Class declaration                                                          //
//****************************************************************************//

 /*****************************************************************************/
/** The vector class.
  *****************************************************************************/
class CalVector
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
  //CalVector(D3DVECTOR v);
  ~CalVector();

// member functions
public:
	CalVector project(CalVector p1, CalVector p2);
	CalVector project(CalVector v);
	float clockwise_dot(CalVector &v, CalVector &n);
	//void transform(D3DMATRIX& m, float* fl);
	bool undef();
	bool coplanar(CalVector& v);
	float dot(CalVector &v);
	CalVector cross(CalVector &v);
	CalVector cross(CalVector &v1, CalVector &v2);
//	CalVector(D3DXVECTOR3 v);
	void invert();
	CalVector getNormal( CalVector second);
	float getAngle( CalVector second);
  float& operator[](unsigned int i);
  const float& operator[](unsigned int i) const;
  int operator==(const CalVector& v);
  int operator!=(const CalVector& v);
  void operator=(const CalVector& v);
  //void operator=(const D3DXVECTOR3& v);
  void operator+=(const CalVector& v);
  void operator-=(const CalVector& v);
  void operator+=(const float v);
  void operator-=(const float v);
  void operator*=(const float d);
  void operator/=(const float d);
  friend CalVector operator+(const CalVector& v, const CalVector& u);
  friend CalVector operator-(const CalVector& v, const CalVector& u);
  friend CalVector operator+(const CalVector& v, const float u);
  friend CalVector operator-(const CalVector& v, const float u);
  friend CalVector operator*(const CalVector& v, const float d);
  friend CalVector operator*(const float d, const CalVector& v);
  friend CalVector operator/(const CalVector& v, const float d);
  friend float operator*(const CalVector& v, const CalVector& u);
  friend CalVector operator%(const CalVector& v, const CalVector& u);
  void	minimize(CalVector v){ if(x>v.x) x=v.x;	if(y>v.y) y=v.y;	if(z>v.z) z=v.z;}
  void	maximize(CalVector v){ if(x<v.x) x=v.x;	if(y<v.y) y=v.y;	if(z<v.z) z=v.z;}
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
