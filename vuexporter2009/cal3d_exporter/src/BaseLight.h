//----------------------------------------------------------------------------//
// BaseLight.h                                                                 //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

#ifndef BASE_LIGHT_H
#define BASE_LIGHT_H

//----------------------------------------------------------------------------//
// Forward declarations                                                       //
//----------------------------------------------------------------------------//

class CSkeletonCandidate;
class CVertexCandidate;

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//

class CBaseLight
{
// member variables
protected:

// constructors/destructor
protected:
	CBaseLight();
public:
	virtual ~CBaseLight();

// member functions
public:
	virtual Point3 GetDiffuse() = 0;
	virtual Point3 GetSpecular() = 0;
	virtual BOOL GetUseAtten() = 0;
	virtual BOOL GetEnable() = 0;
	virtual float GetStartAtten() = 0;
	virtual float GetEndAtten() = 0;
	virtual float GetIntensity() = 0;
	virtual int GetShadowMethod() = 0;
};

#endif

//----------------------------------------------------------------------------//
