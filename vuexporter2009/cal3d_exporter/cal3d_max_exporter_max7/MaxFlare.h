//----------------------------------------------------------------------------//
// MaxFlare.h                                                                  //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

#ifndef MAX_FLARE_H
#define MAX_FLARE_H

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "BaseLight.h"
#include "MaxNode.h"

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//

#include "maxlight.h"

class CMaxFlare : public CMaxNode
{
// misc
public:
	CalExporter_Flare	flare;

// constructors/destructor
public:
	CMaxFlare();
	virtual ~CMaxFlare();

//
public:
	void		Create(INode* m_pNode);
	CalVector	GetPosition();
};

#endif

//----------------------------------------------------------------------------//
