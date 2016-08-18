// CalAnimationMorph.cpp: implementation of the CalAnimationMorph class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CalAnimationMorph.h"
#include "memory_leak.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CalAnimationMorph::CalAnimationMorph()
{
	m_pFace = NULL;
	m_iPhase1 = -1;
	m_iPhase2 = -1;
	m_dwCurrentTime = 0;
	bSendAlready = false;
}

CalAnimationMorph::~CalAnimationMorph()
{

}

bool CalAnimationMorph::create(CalCoreAnimation *pCoreAnimation)
{
	if(pCoreAnimation == 0)
	{
	//CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
	return false;
	}

	m_pCoreAnimation = pCoreAnimation;

	m_pFace = (CFace*)pCoreAnimation;
	return true;
}

bool CalAnimationMorph::execute()
{
	m_dwLastUpdateTime = 0;
	m_state = STATE_IN;
	return true;
}

void CalAnimationMorph::setCoreID( int id)
{
	coreAnimID = id;
}

float CalAnimationMorph::getPhaze()const
{
	return m_fPhase;
}


bool CalAnimationMorph::update(float deltaTime)
{
	bool b_Ret = true;

	float	stayTime = getCoreAnimation()->getKeepLastTime();
	float	durTime = m_pCoreAnimation->getDuration();
	float	fullDuration = durTime + stayTime + (stayTime>0.0?durTime:0);

	float   m_dwAbsTime = 0.0;

	m_dwLastUpdateTime = m_dwCurrentTime;
	m_dwCurrentTime += deltaTime;

	
	if( m_dwCurrentTime > fullDuration)
	{
		m_dwCurrentTime = fullDuration;
		b_Ret = false;
	}
	m_dwAbsTime = m_dwCurrentTime;

	if( m_dwCurrentTime < durTime)
		m_state = STATE_IN;
	else
		if( m_dwCurrentTime>=durTime && m_dwCurrentTime<durTime+stayTime)
		{
			m_state = STATE_STEADY;
			m_dwAbsTime = durTime;
			m_fPhase = 1.0;
		}
		else
			if( stayTime != 0)
			{
				m_state = STATE_OUT;
				m_dwAbsTime = m_dwCurrentTime - stayTime - durTime;
			}


	float fMorphPhase = m_dwAbsTime/durTime;
	m_iPhase1 = (int)(m_pFace->header.lNum*fMorphPhase) - 1;
	m_iPhase2 = m_iPhase1 + 1;
	
	if( fMorphPhase != m_pCoreAnimation->getDuration() && m_state != STATE_STEADY)
		m_fPhase = m_pFace->header.lNum*fMorphPhase - (int)(m_pFace->header.lNum*fMorphPhase);
	else
		m_fPhase = 1.0f;

	if( fMorphPhase == 1.0)
		m_iPhase2 = m_iPhase1;
		
	if( (m_bReverse && m_state != STATE_OUT) || (!m_bReverse && m_state == STATE_OUT)){
	//if( m_bReverse ^ state != STATE_OUT){
		if(m_iPhase1 != -1){
			m_iPhase1 = m_pFace->header.lNum - m_iPhase1 - 1;
		}
		m_iPhase2 = m_pFace->header.lNum - m_iPhase2 - 1;
		m_fPhase = 1.0f - m_fPhase;
		if( m_dwAbsTime == m_pCoreAnimation->getDuration()){
			m_iPhase1 = -1;
			m_fPhase = 0.0f;
			b_Ret = false;
		}
	}
//char buf[255]; memset( &buf, 0, 255);
//sprintf( buf, "m_fPhase %f, %i, %i, %f, %f, %f\n", m_fPhase, m_iPhase1, m_iPhase2, fMorphPhase,m_dwAbsTime,m_dwCurrentTime);OutputDebugString( buf);
	return b_Ret;
}