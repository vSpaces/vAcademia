// CalAnimationMorph.h: interface for the CalAnimationMorph class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALANIMATIONMORPH_H__36DFBAFB_B851_453D_99A2_D604C03F9AD1__INCLUDED_)
#define AFX_CALANIMATIONMORPH_H__36DFBAFB_B851_453D_99A2_D604C03F9AD1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "global.h"
#include "animation.h"
#include "facedesc.h"

class CalMixer;
class CalModel;

class CalAnimationMorph : public CalAnimation
{
public:
	bool bSendAlready;
	friend class CalMixer;
	friend class CalModel;
	bool execute();
	bool update(float deltaTime);
	void setCoreID( int id);
	int coreAnimID;
	CalAnimationMorph();
	virtual ~CalAnimationMorph();
	bool create(CalCoreAnimation *pCoreAnimation);
	virtual void clearOnNextStep();

protected:
	CFace*	m_pFace;
	float	m_dwLastUpdateTime;
	float	m_dwCurrentTime;

	int		m_iPhase1;
	int		m_iPhase2;
	float	m_fPhase;

};

#endif // !defined(AFX_CALANIMATIONMORPH_H__36DFBAFB_B851_453D_99A2_D604C03F9AD1__INCLUDED_)
