// nrmMotion.cpp: implementation of the nrmMotion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrm3DMoveCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

nrm3DMoveCtrl::nrm3DMoveCtrl()
{
//	m_pObject = NULL;
	m_iDuration = 0;
	m_fCurTime = 0.0f;
	m_bCheckCollisions = FALSE;	
}

nrm3DMoveCtrl::~nrm3DMoveCtrl()
{

}

/*void nrm3DMoveCtrl::init( icollisionobject* aObject, CalVector aPos3D, int aiDuaration, bool abCheckCollisions)
{
	m_pObject = aObject;
	if( m_pObject)
	{
		m_vDestPos3D = aPos3D;
		m_vStartPos3D = m_pObject->get_translation();
		m_iDuration = aiDuaration;
		m_fCurTime = 0.0f;
		m_bCheckCollisions = abCheckCollisions;	
	}
}*/

void nrm3DMoveCtrl::update(int aiDelta)
{
	//if( !m_pObject)	return;
	if( m_iDuration != 0)	m_fCurTime += (float)aiDelta/m_iDuration;
	if( m_fCurTime >= 1)	m_fCurTime = 1;
	if( m_bCheckCollisions)
	{
	}
	else
	{
		CalVector	new_pos = m_vStartPos3D;
		m_vStartPos3D.blend( m_fCurTime, m_vDestPos3D);
		//m_pObject->set_translation( new_pos);
	}
}