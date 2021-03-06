//****************************************************************************//
// animation_action.cpp                                                       //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "animation_action.h"
#include "error.h"
#include "coreanimation.h"
#include "coretrack.h"
#include "animsequencer.h"
#include "coreanimationflags.h"

 /*****************************************************************************/
/** Constructs the animation action instance.
  *
  * This function is the default constructor of the animation action instance.
  *****************************************************************************/

CalAnimationAction::CalAnimationAction()
{
  m_type = TYPE_ACTION;
}

 /*****************************************************************************/
/** Destructs the animation action instance.
  *
  * This function is the destructor of the animation action instance.
  *****************************************************************************/

CalAnimationAction::~CalAnimationAction()
{
}

 /*****************************************************************************/
/** Creates the animation action instance.
  *
  * This function creates the animation action instance based on a core
  * animation.
  *
  * @param pCoreAnimation A pointer to the core animation on which this
  *                       animation action instance should be based on.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalAnimationAction::create(CalCoreAnimation *pCoreAnimation)
{
  if(pCoreAnimation == 0)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  setFlags(pCoreAnimation->getFlags());
  m_pCoreAnimation = pCoreAnimation;

  m_fadeOutTime = 0;

  return true;
}

 /*****************************************************************************/
/** Destroys the animation action instance.
  *
  * This function destroys all data stored in the animation action instance and
  * frees all allocated memory.
  *****************************************************************************/

void CalAnimationAction::destroy()
{
  m_pCoreAnimation = 0;
}

 /*****************************************************************************/
/** Executes the animation action instance.
  *
  * This function executes the animation action instance.
  *
  * @param delayIn The time in seconds until the animation action instance
  *                reaches the full weight from the beginning of its execution.
  * @param delayOut The time in seconds in which the animation action instance
  *                 reaches zero weight at the end of its execution.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalAnimationAction::execute(float delayIn, float delayOut)
{
  m_state = STATE_IN;
  m_weight = 0.0f;
  m_delayIn = delayIn;
  m_delayOut = delayOut;
  m_time = 0.0f;
  bDone = false;
  return true;
}

 /*****************************************************************************/
/** Updates the animation action instance.
  *
  * This function updates the animation action instance for a given amount of
  * time.
  *
  * @param deltaTime The elapsed time in seconds since the last update.
  *
  * @return One of the following values:
  *         \li \b true if the animation action instance is still active
  *         \li \b false if the execution of the animation action instance has
  *             ended
  *****************************************************************************/

bool CalAnimationAction::update(float deltaTime)
{
  // update animation action time
  m_time += deltaTime * m_timeFactor;

  // handle IN phase
  if(m_state == STATE_IN)
  {
    // check if we are still in the IN phase
    if(m_time < m_delayIn)
    {
      m_weight = m_time / m_delayIn;
    }
    else
    {
      m_state = STATE_STEADY;
      m_weight = 1.0f;
    }
  }

  // hanbdle STEADY
  if(m_state == STATE_STEADY)
  {
    // cehck if we reached OUT phase
    if(m_time >= getDuration() - m_delayOut)
    {
      m_state = STATE_OUT;
    }
  }

  // handle OUT phase
  if(m_state == STATE_OUT)
  {
    // cehck if we are still in the OUT phase
    if(m_time < m_fadeOutTime)
    {
      m_weight = (m_fadeOutTime - m_time) / m_delayOut;
    }
    else
    {
      // we reached the end of the action animation
	  if( !(getFlags()&CAL3D_COREANIM_STAYATEND))
	  {
		  if(m_delayOut == 0.0f)
			  m_weight = 1.0f;
		  else
			  m_weight = 0.0f;
		  m_time = m_fadeOutTime;
		  return false;
	  }else
	  {
		  return true;
	  }
    }
  }

  if( bSelfMaster)
  {
	std::list<CalCoreTrack *>& listCoreTrack = getListCoreTrack();
	std::list<CalCoreTrack *>::iterator iteratorCoreTrack;
	for(iteratorCoreTrack = listCoreTrack.begin(); iteratorCoreTrack != listCoreTrack.end(); ++iteratorCoreTrack)
		(*iteratorCoreTrack)->update(deltaTime);
  }

  return true;
}

void CalAnimationAction::fadeOut(float duration)
{
	removeFlag(CAL3D_COREANIM_STAYATEND);
	if( duration == 0)
		seek(getDuration());
	else
	{
		//m_weight = (m_fadeOutTime - m_time) / m_delayOut;
		//m_fadeOutTime = m_delayOut*m_weight + m_time;
		m_delayOut = duration;
		m_fadeOutTime = m_delayOut*m_weight + m_time;
		m_state = STATE_OUT;
	}
}
