//----------------------------------------------------------------------------//
// MaxFlare.cpp                                                                //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "StdAfx.h"
#include "Exporter.h"
#include "MaxFlare.h"
#include "BaseInterface.h"
#include "SkeletonCandidate.h"
#include "VertexCandidate.h"

//----------------------------------------------------------------------------//
// Debug                                                                      //
//----------------------------------------------------------------------------//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//

CMaxFlare::CMaxFlare()
{

}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

CMaxFlare::~CMaxFlare()
{

}

void CMaxFlare::Create(INode *m_pNode)
{
	m_pINode = m_pNode;
}

CalVector	CMaxFlare::GetPosition()
{
	CalVector v(0,0,0);
	if( !m_pINode)	return v;

	CalQuaternion q;
	theExporter.GetInterface()->GetTranslationAndRotation(this, NULL, 0.0, v, q);
	return v;
}
