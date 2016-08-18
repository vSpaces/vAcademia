
#include "StdAfx.h"
#include "Cal3DModelAccessObject.h"

CCal3DModelAccessObject::CCal3DModelAccessObject(CSkeletonAnimationObject* sao):
	m_sao(sao)
{
	assert(m_sao);
	if (m_sao)
	{
		m_sao->StartCal3DModelAccess();
	}
}
	
CCal3DModelAccessObject::~CCal3DModelAccessObject()
{
	if (m_sao)
	{
		m_sao->EndCal3DModelAccess();
	}
}