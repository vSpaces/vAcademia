//----------------------------------------------------------------------------//
// MaxLight.cpp                                                                //
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
#include "MaxLight.h"
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

CMaxLight::CMaxLight()
{

}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

CMaxLight::~CMaxLight()
{

}

void CMaxLight::Create(INode *m_pNode)
{
	m_pINode = m_pNode;
	m_pILight = (LightObject*)(m_pINode->EvalWorldState(0.0).obj);
}

Point3 CMaxLight::GetDiffuse()
{
	Point3	res(0,0,0);
	if( !m_pILight)	return res;
	return m_pILight->GetRGBColor(0.0);
}

Point3 CMaxLight::GetSpecular()
{
	Point3	res(0,0,0);
	if( !m_pILight)	return res;
	return m_pILight->GetRGBColor(0.0);
}

BOOL CMaxLight::GetUseAtten()
{
	if( !m_pILight)	return FALSE;
	return m_pILight->GetUseAtten();
}

BOOL CMaxLight::GetEnable()
{
	if( !m_pILight)	return FALSE;
	return m_pILight->GetUseLight();
}

float CMaxLight::GetStartAtten()
{
	if( !m_pILight)	return 0.0;
	return m_pILight->GetAtten(0.0, LIGHT_ATTEN_START);
}

float CMaxLight::GetEndAtten()
{
	if( !m_pILight)	return 0.0;
	return m_pILight->GetAtten(0.0, LIGHT_ATTEN_END);
}

float CMaxLight::GetIntensity()
{
	if( !m_pILight)	return 0.0;
	return m_pILight->GetIntensity(0.0);
}

int CMaxLight::GetShadowMethod()
{
	if( !m_pILight)	return 0;
	return m_pILight->GetShadowMethod();
}

int	CMaxLight::GetLightType()
{
	if( !m_pILight)	return 0;
	Class_ID	classID = m_pILight->ClassID();
	if( classID == Class_ID(OMNI_LIGHT_CLASS_ID, 0)) return 1;
}