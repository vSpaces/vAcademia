//****************************************************************************//
// Plane.cpp                                                                 //
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
#include "Plane.h"
#include "quaternion.h"
#include "vector.h"
#include "memory_leak.h"

 /*****************************************************************************/
/** Constructs the Plane instance.
  *
  * This function is the default constructor of the Plane instance.
  *****************************************************************************/

CalPlane::CalPlane(CalVector v1, CalVector v2, CalVector v3)
{ 
	CalVector	e1, e2;
	e1 = v2 - v1;
	e2 = v3 - v1;
	normal = e1.cross(e2);
	normal.normalize();
	d0 = v1.dot(normal);
}

float CalPlane::distance(CalVector& v)
{ 
	return normal.dot(v)-d0; 
}

bool CalPlane::contain(CalPlane&	pl)
{
	return coplanar(pl);
}

bool CalPlane::coplanar(CalPlane& p)
{
	float dot = normal.dot(p.normal);
	if( dot>0.99f && (float)fabs(d0-p.d0) < 0.01f)
		return true;
	return false;
}

CalVector	CalPlane::project(CalVector v)
{
	float d = distance(v);
	CalVector	in = normal;
	v -= in*d;
	return v;
}