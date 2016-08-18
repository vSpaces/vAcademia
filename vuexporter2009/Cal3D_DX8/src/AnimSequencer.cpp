// AnimSequencer.cpp: implementation of the CalAnimSequencer class.
//
//////////////////////////////////////////////////////////////////////

#include "cal3d.h"
#include "AnimSequencer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CalAnimSequencer::CalAnimSequencer()
{
	pfAccomlished = NULL;
	m_fadeDelay = 0.0;
	m_startID = m_cycleID = m_endID = -1;
	mode = -1;	// not defined
}

CalAnimSequencer::~CalAnimSequencer()
{

}

/************************************************************************/
/* Start sequence from start
/************************************************************************/
void CalAnimSequencer::play()
{
	mode = -1;
	resume();
}

void CalAnimSequencer::stop()
{
	if (mode == 1) 
	{
		m_pMixer->clearAction(m_startID);
		if(m_endID != -1)
		{
			mode = 3;
			executeAction(m_endID);
		}
		else
		{
			mode = -1;
			if( pfAccomlished)	pfAccomlished( m_seqID, -1, pdAccomlished);
		}
	}
	else	if (mode == 2) 
	{
		
		if(m_endID != -1)
		{
			m_pMixer->clearCycle(m_cycleID, m_pMixer->getDuration(m_endID));
			executeAction(m_endID, m_pMixer->getDuration(m_endID));
			mode = 3;
		}
		else
		{
			m_pMixer->clearCycle(m_cycleID, 0.0f);
			mode = -1;
			if( pfAccomlished)	pfAccomlished( m_seqID, -1, pdAccomlished);
		}
	}
	else	if (mode == 3) 
	{

	}
	/*else	if (mode == -1)
	{
		if( pfAccomlished)	pfAccomlished( m_seqID, -1, pdAccomlished);
	}*/
}

bool CalAnimSequencer::create(CalMixer* pm, int sid, int mid, int eid)
{
	m_pMixer = pm;
	m_startID = sid;
	m_cycleID = mid;
	m_endID = eid;
	return true;
}

void CalAnimSequencer::animExecuted(int mId)
{
	if( mId == m_startID)
	{
		if( mode == 1)
		{
			mode = 2;
			resume();
		}
	}else	if( mId == m_cycleID)
	{
		if( mode == 2)
		{
			mode = 3;
			resume();
		}
	}else	if( mId == m_endID)
	{
		if( /*mode == 2 || */mode == 3)
		{
			mode = 4;
			resume();
		}
		//mode = 4;
	}
}

/************************************************************************/
/* Start sequencer depends on current state
/************************************************************************/
void CalAnimSequencer::resume()
{
	int ev_mode = -2;
	if(mode == -1)
	{
		mode = 1;
		if(m_startID != -1)
		{
			executeAction(m_startID, 0.0f);
			ev_mode = 1;
		}
		else
			mode = 2;
	}
	if( mode == 2)
	{
		if(m_cycleID != -1)
		{
			blendCycle(m_cycleID);
			ev_mode = 2;
		}
		else
			mode = 3;
	}
	if(mode == 3)
	{
		if(m_endID != -1)
		{
			//executeAction(m_endID, m_fadeDelay);
			ev_mode = 3;
		}
		else
			ev_mode = -1;
	}
	if(mode == 4)
	{
		ev_mode = -1;
		mode = -1;
	}
	if( pfAccomlished && ev_mode != -2)	pfAccomlished( m_seqID, ev_mode, pdAccomlished);
}

void CalAnimSequencer::executeAction(int id, float delay)
{
	if( m_pMixer->executeAction(id, delay, 0.0))
	{
		std::list<CalAnimationAction*>::iterator	it;
		it = m_pMixer->m_listAnimationAction.begin();
		(*it)->setSequencer(this);
	}
}

void CalAnimSequencer::blendCycle(int id, float delay)
{
	if( m_pMixer->blendCycle(id, 1.0, delay))
	{
		std::list<CalAnimationCycle*>::iterator	it;
		it = m_pMixer->m_listAnimationCycle.begin();

		CalAnimation*	pAnimation = *it;
		pAnimation->addNotification( 0.067f, (int)AT_STEPSOUNDR);
		pAnimation->addNotification( 0.53f, (int)AT_STEPSOUNDL);
//		(*it)->setSequencer(this);
	}
}
