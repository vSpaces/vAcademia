//----------------------------------------------------------------------------//
// MaxLight.h                                                                  //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

#ifndef MAX_LIGHT_H
#define MAX_LIGHT_H

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "BaseLight.h"
#include "MaxNode.h"

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//

typedef struct _CalExporter_Flare
{
	CComString	fname;
	float		size;
	float		dist;
	bool		enabled;
	CalVector	pos;

	_CalExporter_Flare()
	{
		fname="";	
		size=100.0f; 
		dist = 1000.0f;	enabled = false;
		pos.set(0,0,0);
	}

	_CalExporter_Flare(const _CalExporter_Flare& d)
	{
		fname=d.fname;	size=d.size; dist = d.dist;	enabled = d.enabled;
		pos = d.pos;
	}

	void operator=(const _CalExporter_Flare& d)
	{
		fname=d.fname;	size=d.size; dist = d.dist;	enabled = d.enabled;
		pos = d.pos;
	}
} CalExporter_Flare;

class CMaxLight : public CBaseLight, public CMaxNode
{
// misc
public:
	CalExporter_Flare	flare;

// member variables
protected:
	LightObject *m_pILight;

// constructors/destructor
public:
	CMaxLight();
	virtual ~CMaxLight();

//
public:
	void	Create(INode* m_pNode);
	int		GetLightType();
	Point3	GetDiffuse();
	Point3	GetSpecular();
	BOOL	GetUseAtten();
	BOOL	GetEnable();
	float	GetStartAtten();
	float	GetEndAtten();
	float	GetIntensity();
	int		GetShadowMethod();
};

#endif

//----------------------------------------------------------------------------//
