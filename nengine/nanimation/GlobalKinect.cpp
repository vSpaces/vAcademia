
#include "StdAfx.h"
#include "GlobalKinect.h"

CGlobalKinect::CGlobalKinect()
{
	m_leftCorner.Set(0, 0, 0);
	m_rightCorner.Set(0, 0, 0);
}

void CGlobalKinect::SetDeskLeftCorner(CVector3D corner)
{
	m_leftCorner = corner;
}

void CGlobalKinect::SetDeskRightCorner(CVector3D corner)
{
	m_rightCorner = corner;
}

CVector3D CGlobalKinect::GetDeskLeftCorner()
{
	return m_leftCorner;
}

CVector3D CGlobalKinect::GetDeskRightCorner()
{
	return m_rightCorner;
}

CGlobalKinect* CGlobalKinect::GetInstance()
{
	static CGlobalKinect* obj = NULL;

	if (obj == NULL)
	{
		obj = MP_NEW(CGlobalKinect);
	}

	return obj;
}

CGlobalKinect::~CGlobalKinect()
{
}