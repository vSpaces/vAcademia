#include "StdAfx.h"
#include "AnimationObject.h"
#include "HelperFunctions.h"
#include "GlobalSingletonStorage.h"

CAnimationObject::CAnimationObject():
	m_obj3d(NULL),
	m_model(NULL),
	m_asyncUpdateListener(NULL)
{	
}

void CAnimationObject::SetObject3D(C3DObject* obj)
{
	m_obj3d = obj;
}

CAnimationObject::~CAnimationObject()
{
}