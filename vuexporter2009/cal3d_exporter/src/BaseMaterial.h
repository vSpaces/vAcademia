//----------------------------------------------------------------------------//
// BaseMaterial.h                                                             //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

#ifndef BASE_MATERIAL_H
#define BASE_MATERIAL_H

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//

class CBaseMaterial
{
// member variables
protected:

// constructors/destructor
protected:
	CBaseMaterial();
public:
	virtual ~CBaseMaterial();

// member functions
public:
#ifdef MAX3DS_BUILD
	virtual int GetMaxMapType(int mapId) = 0;
#endif
	virtual void GetAmbientColor(float *pColor) = 0;
	virtual void GetDiffuseColor(float *pColor) = 0;
	virtual int GetMapCount() = 0;
	virtual std::string GetMapFilename(int mapId) = 0;
	virtual std::string GetName() = 0;
	virtual void GetSpecularColor(float *pColor) = 0;
#ifdef MAX3DS_BUILD
	virtual void GetEmissiveColor(float *pColor) = 0;
#endif
	virtual float GetShininess() = 0;
};

#endif

//----------------------------------------------------------------------------//
