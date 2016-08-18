//****************************************************************************//
// plane.h                                                                   //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_plane_H
#define CAL_plane_H

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "global.h"

//****************************************************************************//
// Forward declarations                                                       //
//****************************************************************************//
#include "vector.h"
class CalQuaternion;


//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//

 /*****************************************************************************/
/** The plane class.
  *****************************************************************************/
class CAL3D_API CalPlane
{
	CalVector normal;	//!< Plane normal
	float d0;			//!< Perpendicular distance from the plane to the origin

	//! Default constructor
	CalPlane() : d0(0) 
	{ }
	
	//! Constructor from components
	CalPlane(CalVector n, float dist) : 
		normal(n),d0(dist)
	{ }

	//! Constructor from components
	CalPlane(CalVector v1, CalVector v2, CalVector v3);

	//! Copy-constructor
	CalPlane(const CalPlane& in) : normal(in.normal), d0(in.d0) 
	{ }

	//! Atribuition operator
	void operator=(const CalPlane& in) 
	{ 
		normal = in.normal;
		d0 = in.d0;
	}

	//! Compute the perpendicular distance from a point to the plane
	float distance(CalVector& v);

	//! Intersect a ray (ro,rd) with the plane and return intersection point (ip) and distance to intersection (dist)
	//int ray_intersect(CalVector& ro,CalVector& rd,CalVector& ip,float& dist);

	bool contain(CalPlane&	pl);
	

	bool coplanar(CalPlane& p);

	CalVector	project(CalVector v);
	
};

#endif

//****************************************************************************//
