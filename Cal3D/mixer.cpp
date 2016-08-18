//****************************************************************************//
// mixer.cpp                                                                  //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#include "StdAfx.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include <assert.h>
#include "error.h"
#include "mixer.h"
#include "coremodel.h"
#include "corebone.h"
#include "coreanimation.h"
#include "coretrack.h"
#include "model.h"
#include "skeleton.h"
#include "bone.h"
#include "animation.h"
#include "animation_action.h"
#include "animation_cycle.h"
#include "animation_set.h"
#include "calanimationmorph.h"
//#include "stepctrl.h"
#include "corekeyframe.h"
#include "coreskeleton.h"
#include "animsequencer.h"
#include "Mpeg4FDP.h"
#include "coreanimationflags.h"
#include "memory_leak.h"
 /*****************************************************************************/
/** Constructs the mixer instance.
  *
  * This function is the default constructor of the mixer instance.
  *****************************************************************************/
#define	FORALLMEMBERS(type)		std::list<Cal##type *>::iterator iterator##type;	\
	iterator##type = m_list##type.begin();											\
	while(iterator##type != m_list##type.end())	{									\

#define	FORALLMEMBERS2T(ittype, listtype)		std::list<Cal##ittype *>::iterator iterator##listtype;	\
	iterator##listtype = m_list##listtype.begin();											\
	while(iterator##listtype != m_list##listtype.end())	{									\

#define	ENDFOR(type)		++iterator##type;}

std::string getString(int a)
{
	char tmp[20];
	_itoa_s(a, tmp, 10);
	return tmp;
}

CalMixer::CalMixer():
	MP_VECTOR_INIT(m_vectorAnimation),
	MP_LIST_INIT(m_listAnimationAction),
	MP_LIST_INIT(m_listAnimationMorph),
	MP_LIST_INIT(m_listAnimationCycle),
	MP_LIST_INIT(m_listFaceExpression),
	MP_VECTOR_INIT(m_vectorSequencers),
	MP_VECTOR_INIT(vAfterMotions),
	MP_VECTOR_INIT(bonesTracks)
{
	attach_parent = NULL;
	animation_basis_used = true;
	basis_animation = NULL;
}

 /*****************************************************************************/
/** Destructs the mixer instance.
  *
  * This function is the destructor of the mixer instance.
  *****************************************************************************/

CalMixer::~CalMixer()
{
  assert(m_vectorAnimation.empty());
  assert(m_listAnimationCycle.empty());
  assert(m_listAnimationMorph.empty());
  assert(m_listAnimationAction.empty());
  //assert(m_listStepCycle.empty());
}

 /*****************************************************************************/
/** Interpolates the weight of an animation cycle.
  *
  * This function interpolates the weight of an animation cycle to a new value
  * in a given amount of time. If the specified animation cycle is not active
  * yet, it is activated.
  *
  * @param id The ID of the animation cycle that should be blended.
  * @param weight The weight to interpolate the animation cycle to.
  * @param delay The time in seconds until the new weight should be reached.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalMixer::blendCycle(int id, float weight, float delay, bool /*asynch*/, float duration)
{
  if((id < 0) || (id >= (int)m_vectorAnimation.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  // get the animation for the given id
  CalAnimation *pAnimation;
  pAnimation = m_vectorAnimation[id];

  // create a new animation instance if it is not active yet
  if(pAnimation == 0)
  {
    // take the fast way out if we are trying to clear an inactive animation
    if(weight == 0.0f) return true;

    // get the core animation
    CalCoreAnimation *pCoreAnimation;
    pCoreAnimation = m_pModel->getCoreModel()->getCoreAnimation(id);
    if(pCoreAnimation == 0) return false;

	pCoreAnimation->initialize( m_pModel->getSkeleton());

    // allocate a new animation cycle instance
    CalAnimationCycle *pAnimationCycle = MP_NEW( CalAnimationCycle);
    if(pAnimationCycle == 0)
    {
      CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
      return false;
    }

    // create the new animation instance
    if(!pAnimationCycle->create(pCoreAnimation))
    {
      MP_DELETE( pAnimationCycle);
      return false;
    }

	pAnimationCycle->setCoreID( id);
	pAnimationCycle->setDuration( duration==0.0?pCoreAnimation->getDuration() : duration);

    // insert new animation into the tables
    m_vectorAnimation[id] = pAnimationCycle;
    m_listAnimationCycle.push_front(pAnimationCycle);

    // blend the animation
	pAnimationCycle->initStates( m_pModel->getSkeleton());
	pAnimationCycle->FallOnSkel( m_pModel->getSkeleton());
	
	CheckAnimPlace(pAnimationCycle);
    return pAnimationCycle->blend(weight, delay);
  }

  // check if this is really a animation cycle instance
  if(pAnimation->getType() != CalAnimation::TYPE_CYCLE)
  {
      CalError::setLastError(CalError::INVALID_ANIMATION_TYPE, __FILE__, __LINE__);
      return false;
  }

  // clear the animation cycle from the active vector if the target weight is zero
  if(weight == 0.0f)
  {
      m_vectorAnimation[id] = 0;
  }

  // cast it to an animation cycle
  CalAnimationCycle *pAnimationCycle;
  pAnimationCycle = (CalAnimationCycle *)pAnimation;
  pAnimationCycle->setCoreID( id);
  pAnimationCycle->initStates( m_pModel->getSkeleton());
  pAnimationCycle->FallOnSkel( m_pModel->getSkeleton());

  // blend the animation cycle  
  CheckAnimPlace(pAnimationCycle);
  return pAnimationCycle->blend(weight, delay);
}

 /*****************************************************************************/
/** Fades an animation cycle out.
  *
  * This function fades an animation cycle out in a given amount of time.
  *
  * @param id The ID of the animation cycle that should be faded out.
  * @param delay The time in seconds until the the animation cycle is
  *              completely removed.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalMixer::clearCycle(int id, float delay)
{
  // check if the animation id is valid
  if((id < 0) || (id >= (int)m_vectorAnimation.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  // get the animation for the given id
  CalAnimation *pAnimation;
  pAnimation = m_vectorAnimation[id];

  // we can only clear cycles that are active
  if(pAnimation == 0) return true;

  // check if this is really a animation cycle instance
  if(pAnimation->getType() != CalAnimation::TYPE_CYCLE)
  {
      CalError::setLastError(CalError::INVALID_ANIMATION_TYPE, __FILE__, __LINE__);
      return false;
  }

  // clear the animation cycle from the active vector
  m_vectorAnimation[id] = 0;

  // cast it to an animation cycle
  CalAnimationCycle *pAnimationCycle;
  pAnimationCycle = (CalAnimationCycle *)pAnimation;

  // blend the animation cycle
  return pAnimationCycle->blend(0.0f, delay);
}

 /*****************************************************************************/
/** Creates the mixer instance.
  *
  * This function creates the mixer instance.
  *
  * @param pModel A pointer to the model that should be managed with this mixer
  *               instance.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalMixer::create(CalModel *pModel)
{
  if(pModel == 0)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  m_pModel = pModel;

  CalCoreModel *pCoreModel;
  pCoreModel = m_pModel->getCoreModel();
  if(pCoreModel == 0)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  // build the animation table
  int coreAnimationCount;
  coreAnimationCount = m_pModel->getCoreModel()->getCoreAnimationCount();

  m_vectorAnimation.reserve(coreAnimationCount);
  m_vectorAnimation.insert(m_vectorAnimation.begin(), (std::vector<CalAnimation *>::size_type)coreAnimationCount, 0);

  // set the animation time/duration values to default
  m_animationTime = 0.0f;
  m_animationDuration = 0.0f;

  //
  if( pModel->getSkeleton())
  {
	  int bonecount = pModel->getSkeleton()->getCoreSkeleton()->getVectorCoreBone().size();
	  bonesTracks.reserve(bonecount);
	  for( int c = 0; c < bonecount; c++)
	  {
		  vecVOIDPAIR* tracks = MP_NEW(vecVOIDPAIR);	
		  MP_VECTOR_DC_INIT((*tracks));
		  bonesTracks.push_back( tracks);
	  }
  }

  return true;
}

 /*****************************************************************************/
/** Destroys the mixer instance.
  *
  * This function destroys all data stored in the mixer instance and frees all
  * allocated memory.
  *****************************************************************************/

void CalMixer::destroy()
{
  // destroy all active animation actions
  while(!m_listAnimationAction.empty())
  {
    CalAnimationAction *pAnimationAction;
    pAnimationAction = m_listAnimationAction.front();
    m_listAnimationAction.pop_front();

    pAnimationAction->destroy();
    pAnimationAction->release();
  }

  // destroy all active animation cycles
  while(!m_listAnimationCycle.empty())
  {
    CalAnimationCycle *pAnimationCycle;
    pAnimationCycle = m_listAnimationCycle.front();
    m_listAnimationCycle.pop_front();

    pAnimationCycle->destroy();
    MP_DELETE( pAnimationCycle);
  }

  // destroy all active morph animation
  while(!m_listAnimationMorph.empty())
  {
    CalAnimationMorph *pAnimationMorph;
    pAnimationMorph = m_listAnimationMorph.front();
    m_listAnimationMorph.pop_front();

    pAnimationMorph->destroy();
    MP_DELETE( pAnimationMorph);
  }

  // clear the animation table
  m_vectorAnimation.clear();

  // clear bones tracks
  if (!m_pModel->IsSharedSkeleton())
  if( m_pModel->getSkeleton()){
	  for( unsigned int c = 0; c < m_pModel->getSkeleton()->getCoreSkeleton()->getVectorCoreBone().size(); c++)
	  {
		  vecVOIDPAIR*	tracks = bonesTracks[c];
		  tracks->clear();
		  MP_DELETE( tracks);
	  }
  }
  bonesTracks.clear();

  std::vector<CalAnimSequencer *>::iterator	sit;
  for (sit=m_vectorSequencers.begin(); sit!=m_vectorSequencers.end(); sit++)
  {
	  CalAnimSequencer*	seq = (CalAnimSequencer*)*sit;
	  MP_DELETE( seq);
  }
  m_vectorSequencers.clear();

  m_pModel = 0;
}

 /*****************************************************************************/
/** Executes an animation action.
  *
  * This function executes an animation action.
  *
  * @param id The ID of the animation cycle that should be blended.
  * @param delayIn The time in seconds until the animation action reaches the
  *                full weight from the beginning of its execution.
  * @param delayOut The time in seconds in which the animation action reaches
  *                 zero weight at the end of its execution.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalMixer::executeAction(int id, float delayIn, float delayOut, float duration)
{
  if((id < 0) || (id >= (int)m_vectorAnimation.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  // get the core animation
  CalCoreAnimation *pCoreAnimation;
  pCoreAnimation = m_pModel->getCoreModel()->getCoreAnimation(id);
  if(pCoreAnimation == 0)
  {
	return false;
  }

  pCoreAnimation->initialize( m_pModel->getSkeleton());

  // allocate a new animation action instance
  CalAnimationAction *pAnimationAction = MP_NEW( CalAnimationAction);
  pAnimationAction->setCoreID( id);

  if(pAnimationAction == 0)
  {
	  CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__); 
	  return false;
  }

  // create the new animation instance
  if(!pAnimationAction->create(pCoreAnimation))
  {
    MP_DELETE( pAnimationAction);
    return false;
  }

  pAnimationAction->setDuration( duration==0.0 ? pCoreAnimation->getDuration() : duration);

  pAnimationAction->initStates( m_pModel->getSkeleton());
  pAnimationAction->FallOnSkel( m_pModel->getSkeleton());

  CheckAnimPlace( pAnimationAction);

  // insert new animation into the table
  if( pCoreAnimation->getFlags() & CAL3D_COREANIM_PRIMARY)
	  m_listAnimationAction.push_back(pAnimationAction);
  else
	  m_listAnimationAction.push_front(pAnimationAction);

  // execute the animation
  return pAnimationAction->execute(delayIn, delayOut);
}

 /*****************************************************************************/
/** Updates all active animations.
  *
  * This function updates all active animations of the mixer instance for a
  * given amount of time.
  *
  * @param deltaTime The elapsed time in seconds since the last update.
  *****************************************************************************/

bool CalMixer::updateAnimation(float deltaTime)
{
  bool	bActiveAnimationPresent = false;
  // update the current animation time
  if(m_animationDuration == 0.0f)
  {
    m_animationTime = 0.0f;
  }
  else
  {
    m_animationTime += deltaTime;
    if(m_animationTime >= m_animationDuration)
    {
      m_animationTime = fmod(m_animationTime, m_animationDuration);
    }
  }

  // update all active animation actions of this model
  std::list<CalAnimationAction *>::iterator iteratorAnimationAction;
  iteratorAnimationAction = m_listAnimationAction.begin();

  while(iteratorAnimationAction != m_listAnimationAction.end())
  {
	bActiveAnimationPresent = true;
    // update and check if animation action is still active
    if((*iteratorAnimationAction)->update(deltaTime))
    {
      ++iteratorAnimationAction;
    }
    else
    {
		  (*iteratorAnimationAction)->completeCallbacks(m_pModel);
		  (*iteratorAnimationAction)->release();
		  iteratorAnimationAction = m_listAnimationAction.erase(iteratorAnimationAction);
    }
  }

  // todo: update all active animation poses of this model
  // update the weight of all active animation cycles of this model
  std::list<CalAnimationCycle *>::iterator iteratorAnimationCycle;
  iteratorAnimationCycle = m_listAnimationCycle.begin();

  while(iteratorAnimationCycle != m_listAnimationCycle.end())
  {
	  bActiveAnimationPresent = true;
	  // update and check if animation action is still active
	  if((*iteratorAnimationCycle)->update(deltaTime))
	  {
		  ++iteratorAnimationCycle;
	  }
	  else
	  {
		  (*iteratorAnimationCycle)->completeCallbacks(m_pModel);
		  MP_DELETE( *iteratorAnimationCycle);
		  iteratorAnimationCycle = m_listAnimationCycle.erase(iteratorAnimationCycle);
	  }
  }

  if( m_pModel->m_pFAPU)
	  m_pModel->m_pFAPU->update(deltaTime);

  return bActiveAnimationPresent;
}

 /*****************************************************************************/
/** Updates the skeleton.
  *
  * This function updates the skeleton to match the current animation state.
  *****************************************************************************/
void CalMixer::updateSkeleton()
{
  // get the skeleton we need to update
  CalSkeleton *pSkeleton;
  pSkeleton = m_pModel->getSkeleton();
  if(pSkeleton == 0) return;

  // clear the skeleton state
  pSkeleton->clearState();

  // loop through all animation actions
  std::list<CalAnimationAction *>::iterator iteratorAnimationAction;
  for(iteratorAnimationAction = m_listAnimationAction.begin(); iteratorAnimationAction != m_listAnimationAction.end(); ++iteratorAnimationAction)
  {
	// get the animation instance
	CalAnimation *pAnimation;
	pAnimation = (*iteratorAnimationAction);

	// get the core animation instance
    CalCoreAnimation *pCoreAnimation;
    pCoreAnimation = pAnimation->getCoreAnimation();

    // get the duration of the core animation
    float coreDuration = pCoreAnimation->getDuration();
	float animDuration = pAnimation->getDuration();
	float durationFactor = coreDuration;
	if( animDuration != 0)
		durationFactor /= animDuration;

	float time = pAnimation->getTime() * durationFactor;
	float weight = pAnimation->getWeight();

    // get the list of core tracks of above core animation
    std::list<CalCoreTrack *>& listCoreTrack = pAnimation->getListCoreTrack();

	CalVector		applyTrackTranslation;
	CalQuaternion	applyTrackRotation;
    // loop through all core tracks of the core animation
    std::list<CalCoreTrack *>::iterator iteratorCoreTrack;
    for(iteratorCoreTrack = listCoreTrack.begin(); iteratorCoreTrack != listCoreTrack.end(); ++iteratorCoreTrack)
    {
		CalCoreTrack* apCoreTrack = (*iteratorCoreTrack);

		int boneId = apCoreTrack->getCoreBoneId();

		// если сдвигом занимается другая анимация
		if( boneId == Cal::PROJECTION_BONE_ID && (*iteratorAnimationAction) != basis_animation)
			continue;

		if( apCoreTrack->getWeight() == 0.0)
			return;

		// get the appropriate bone of the track
		CalBone *pBone = pSkeleton->getBone( boneId);
		if( !pBone)
			return;

		// get the current translation and rotation
		apCoreTrack->getState( time, coreDuration, applyTrackTranslation, applyTrackRotation);

		if( boneId == Cal::PROJECTION_BONE_ID)
		{
			pAnimation->applyProjectionBoneTransformation( applyTrackTranslation, applyTrackRotation);
			pSkeleton->m_bFrameMoved = true;
		}

		// PRIMARY анимации должны "перекрывать" все остальные. Их ни с чем не блендим
		if( pAnimation->getFlags() & CAL3D_COREANIM_PRIMARY) 
			pBone->clearState();

		pBone->blendState( weight, applyTrackTranslation, applyTrackRotation);
    }
  }
  // lock the skeleton state
  pSkeleton->lockState();

  // loop through all animation cycles
  std::list<CalAnimationCycle *>::iterator iteratorAnimationCycle;
  for(iteratorAnimationCycle = m_listAnimationCycle.begin(); iteratorAnimationCycle != m_listAnimationCycle.end(); ++iteratorAnimationCycle)
  {
	CalAnimationCycle* animationCycle = (*iteratorAnimationCycle);

    // get the core animation instance
    CalCoreAnimation *pCoreAnimation;
    pCoreAnimation = animationCycle->getCoreAnimation();

	// get the duration of the core animation
	float coreDuration = pCoreAnimation->getDuration();
	float animDuration = animationCycle->getDuration();
	float durationFactor = coreDuration;
	if( animDuration != 0)
		durationFactor /= animDuration;

    // calculate adjusted time
    float animationTime;
    animationTime = animationCycle->getTime();
    animationTime *= durationFactor;

	float weight = animationCycle->getWeight();

    // get the list of core tracks of above core animation
    std::list<CalCoreTrack *>& listCoreTrack = animationCycle->getListCoreTrack();//pCoreAnimation->getListCoreTrack();

    // loop through all core tracks of the core animation
    std::list<CalCoreTrack *>::iterator iteratorCoreTrack;
    for(iteratorCoreTrack = listCoreTrack.begin(); iteratorCoreTrack != listCoreTrack.end(); ++iteratorCoreTrack)
    {
      // get the appropriate bone of the track
	  int boneId = (*iteratorCoreTrack)->getCoreBoneId();

	  // если сдвигом занимается другая анимация
	  if( boneId == Cal::PROJECTION_BONE_ID && animationCycle != basis_animation)
		  continue;
      CalBone* pBone = pSkeleton->getBone( boneId);
	  if( !pBone)
		  continue;

      // get the current translation and rotation
      CalVector translation;
      CalQuaternion rotation;
	  bool bClear = true;
	  bClear = (*iteratorCoreTrack)->getState(animationTime, coreDuration, translation, rotation);

	  if( boneId == Cal::PROJECTION_BONE_ID)
	  {
		  animationCycle->applyProjectionBoneTransformation( translation, rotation);
		  pSkeleton->m_bFrameMoved = true;
	  }

      pBone->blendState(weight, translation, rotation);
    }
  }
  // lock the skeleton state
  pSkeleton->lockState();
  
  // let the skeleton calculate its final state
  pSkeleton->calculateState();

  // check for attachments
  CalBone*	m_pFrame=NULL;
  if(attach_parent && (m_pFrame=m_pModel->getSkeleton()->getBone(Cal::PROJECTION_BONE_ID))!=NULL)
  {
	  assert(false);
		CalVector		trn = attach_translation;
		trn *= attach_parent->getRotationAbsolute();
		trn += attach_parent->getTranslationAbsolute();
		CalQuaternion	rtn = attach_rotation;
		rtn *= attach_parent->getRotationAbsolute();
		m_pFrame->clearState();
		m_pFrame->blendState(1.0, trn, rtn);
  }
}


void CalMixer::removeAllSetActionsImmediately()
{
	std::list<CalAnimationAction *>::iterator it = m_listAnimationAction.begin(), 
													end = m_listAnimationAction.end();
	for(; it!=end; it++)
	{
		if( (*it)->getType() == CalAnimation::TYPE_SET)
		{
			(*it)->release();
			m_listAnimationAction.erase( it);
			break;
		}
	}
}

void CalMixer::insertSetActionIntoList( CalAnimationAction* pAnimation)
{
	std::list<CalAnimationAction *>::iterator firstPrimaryMotionIterator = m_listAnimationAction.end();
	std::list<CalAnimationAction *>::iterator it = m_listAnimationAction.begin(), 
												end = m_listAnimationAction.end();
	for(; it!=end; it++)
	{
		if( (*it)->getFlags())
		{
			firstPrimaryMotionIterator = it;
			break;
		}
	}

	if( firstPrimaryMotionIterator != m_listAnimationAction.end())
	{
		CalAnimationAction* pb = *m_listAnimationAction.begin();
		// The container is extended by inserting new elements before the element at the specified position
		m_listAnimationAction.insert( firstPrimaryMotionIterator, pAnimation);
		pb = *m_listAnimationAction.begin();
	}
	else
	{
		// insert new animation into the table
		m_listAnimationAction.push_back(pAnimation);
	}
}
//****************************************************************************//
bool CalMixer::setAction(int id, DWORD dur, bool /*bPose*/, bool inPlace)
{
  if((id < 0) || (id >= (int)m_vectorAnimation.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  // Ticket #2663 (closed defect: fixed)
  if( m_pModel->isFrozen())
	  dur = 0;

  // get the core animation
  CalCoreAnimation *pCoreAnimation;
  pCoreAnimation = m_pModel->getCoreModel()->getCoreAnimation(id);
  if(pCoreAnimation == 0)
  {
    return false;
  }

  pCoreAnimation->initialize( m_pModel->getSkeleton());

  // allocate a new animation action instance
  CalAnimationSet *pAnimationAction = MP_NEW( CalAnimationSet);
  dwSetDuration = dur/1000.0f;
  dwSetAccTime = 0;

  pAnimationAction->setCoreID( id);
  if(pAnimationAction == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return false;
  }

  // create the new animation instance
  if(!pAnimationAction->create(pCoreAnimation))
  {
    MP_DELETE( pAnimationAction);
    return false;
  }

  pAnimationAction->initStates( m_pModel->getSkeleton());
  pAnimationAction->FallOnSkel( m_pModel->getSkeleton());

  CheckAnimPlace( pAnimationAction, inPlace);
  // переведем анимацию в вид
  // 1. Кадр - положение костей из скелета
  // 2. Кадр - положение костей из 0 кадра анимации
  pAnimationAction->createSetAnimation( m_pModel->getSkeleton(), dwSetDuration);

  removeAllSetActionsImmediately();

  insertSetActionIntoList( pAnimationAction);

  // execute the animation
  return pAnimationAction->execute( 0, 0);
}

void CalMixer::setAnimDuration(int id, float duration)
{
  if((id < 0) || (id >= (int)m_vectorAnimation.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return;
  }

  CalCoreAnimation *pCoreAnimation;
  pCoreAnimation = m_pModel->getCoreModel()->getCoreAnimation(id);
  if (pCoreAnimation)
  {
	pCoreAnimation->setDuration( duration);
  }
}

void CalMixer::clearAction(int id, float fadeOut)
{
  CalAnimation*	pAnimation = NULL;
  DWORD	type = findAnimationById(id, &pAnimation);
  if( pAnimation == NULL)	return;

  switch( type)
  {
	case ANIMTYPE_ACTION:
	  ((CalAnimationAction*)pAnimation)->fadeOut(fadeOut);
	  break;
	case ANIMTYPE_SET:
		((CalAnimationSet*)pAnimation)->fadeOut(fadeOut);
		break;
  }
}

void CalMixer::clearActionFadeOnLastFrame(int id, float fadeOut)
{
  CalAnimation*	pAnimation = NULL;
  DWORD	type = findAnimationById(id, &pAnimation);
  if( pAnimation == NULL)	return;

  /*ATLTRACE("CLEAR ACTION FADE");
  char tmp[20];
  itoa(id, tmp, 10);
  ATLTRACE(tmp);*/

  switch( type)
  {
	case ANIMTYPE_ACTION:
	  pAnimation->fadeOnLastFrame(m_pModel->getSkeleton(), fadeOut);
	  break;
	default:
		//??
	  break;
  }
}

/************************************************************************/
/* inanimPlace - установить модель на позицию, взятую из анимации
/************************************************************************/
void CalMixer::CheckAnimPlace(CalAnimation* pAnim, bool inanimPlace)
{
	return;
	assert( false);
	if( !pAnim)	return;

	int			rootID = m_pModel->getSkeleton()->getRootID();
	CalVector		translation;
	CalQuaternion	rotation;
	CalVector		m_vRootTranslation;
	CalQuaternion	m_vRootRotation;
	bool	frameStateCaptured = pAnim->getBoneStartState(Cal::PROJECTION_BONE_ID, translation, rotation);

	if( frameStateCaptured &&	// Added 5.01.06 для того, чтобы ВКС с жуками работала. Но вроде правильная затычка ;)
		pAnim->getCoreAnimation() && pAnim->getCoreAnimation()->m_version>1005 && !pAnim->getCoreAnimation()->m_bRelative)
	{
		m_pModel->relative_pos_changed = false;
		CalVector emptyVec(0, 0, 0);
		CalQuaternion emptyQuat(0, 0, 0, 1);
		m_pModel->getSkeleton()->set_frame_move(emptyVec, emptyQuat);
		return;
	}

	if( !pAnim->getCoreAnimation()->m_bRelative)
	{
		if( (true/*m_pModel->WasMoved()*/ || inanimPlace) && frameStateCaptured)
		{
			CalVector	pos(0,0,0);
			CalQuaternion quat(0,0,0,1);
			if( !inanimPlace)	// оставить модель на месте. Переносим анимацию в текущее положение
			{
				if( m_pModel->m_pfExternTransform)
				{
					m_pModel->m_pfExternTransform(pos, quat, m_pModel->m_pExternClass);
					m_pModel->getSkeleton()->set_frame_move(pos, quat);
					CalVector	dv;
					m_vRootTranslation = pos - translation;
					//
					float bh = m_pModel->get_bip_height();
					CalVector		bipTrn;
					CalQuaternion	bipRtn;
					if(pAnim->getBoneStartState(rootID, bipTrn, bipRtn))
					{
						CalVector	bipAddon(0, 0, bh-bipTrn.z);
						pAnim->translateBip(bipAddon);
					}
				}
			}else
			{	// переместить модель на новое место.
				float bh = m_pModel->get_bip_height();
				CalVector		bipTrn;
				CalQuaternion	bipRtn;
				if(pAnim->getBoneStartState(rootID, bipTrn, bipRtn))
				{
					CalVector	bipAddon(0, 0, -bipTrn.z + bh);
					pAnim->translateBip(bipAddon);
					translation.x = translation.y = 0;
	//				m_pModel->getSkeleton()->set_frame_move(translation, rotation);
					CalQuaternion emptyQuat(0,0,0,1);
					m_pModel->getSkeleton()->set_frame_move(translation, emptyQuat);
					translation.z = bipTrn.z - bh;
					m_vRootTranslation = translation;
					m_pModel->relative_pos_changed = false;
				}else
				{
					// frame translation
					//m_pModel->vTranslation = translation;
					translation.invert();
					translation.z += bh;
					//m_pModel->vTranslation.z -= bh;
					m_vRootTranslation = translation;
				}
			}

			if( !inanimPlace)	// оставить модель на месте. Повернем анимацию так, чтобы она совпала с текущим положением
			{
				if( m_pModel->m_pfExternTransform)
				{
					CalVector xmodel(1,0,0);
					CalVector xanim(1,0,0);
					xmodel *= quat;
					xanim *= rotation;
			 		if( xanim.dot(xmodel) > 0.99)
					{
						m_vRootRotation = CalQuaternion(0,0,0,1);
					}
					else	if( xanim.dot(xmodel) < -0.99)
					{
						m_vRootRotation = CalQuaternion(CalVector(0,0,1),3.141692f);
					}
					else	
					{
						m_vRootRotation = CalQuaternion(xanim.getNormal(xmodel), -xanim.getAngle(xmodel));
					}
				}
			
			}else
			{
				m_vRootRotation = CalQuaternion(0,0,0,1);
			}
			pAnim->setProjectionBoneTransformation( m_vRootTranslation, m_vRootRotation);
		}

		/*if( !pAnim->bLocked && !pAnim->getCoreAnimation()->m_strLock.IsEmpty()){
			  pAnim->m_vLock = m_pModel->getSkeleton()->getBone(
									m_pModel->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName( 
										pAnim->getCoreAnimation()->m_strLock
									))->getTranslationAbsolute();
			  //pAnim->m_vLock = m_pModel->transformVector( pAnim->m_vLock);
			  pAnim->bLocked = true;
		}*/
	}
	else
	{
		pAnim->getBoneStartState(Cal::PROJECTION_BONE_ID, translation, rotation);
		CalVector	pos(0,0,0);
		CalQuaternion quat(0,0,0,1);
		if( !inanimPlace)	// оставить модель на месте. Переносим анимацию в текущее положение
		{
			if( m_pModel->m_pfExternTransform)
			{
				m_pModel->m_pfExternTransform(pos, quat, m_pModel->m_pExternClass);
				m_pModel->getSkeleton()->set_frame_move(pos, quat);
				CalVector	dv;
				m_vRootTranslation = pos - translation;
				pAnim->setProjectionBoneTransformation( m_vRootTranslation, CalQuaternion(CalVector(0,0,1),0));
			}
		}
	}
}

float CalMixer::getAnimActionRelTime(DWORD id)
{
  std::list<CalAnimationAction *>::iterator iteratorAnimationAction;
  iteratorAnimationAction = m_listAnimationAction.begin();
  while(iteratorAnimationAction != m_listAnimationAction.end())
  {
	  if( (*iteratorAnimationAction)->coreAnimID == (int)id)
		  return (*iteratorAnimationAction)->getTime()/(*iteratorAnimationAction)->getDuration();
	  iteratorAnimationAction++;
  }
  return -1;
}

float CalMixer::getAnimTime(int id)
{
  if( id < 0) return 0.0f;

  std::list<CalAnimationAction *>::iterator iteratorAnimationAction;
  iteratorAnimationAction = m_listAnimationAction.begin();
  while(iteratorAnimationAction != m_listAnimationAction.end())
  {
	  if( (*iteratorAnimationAction)->coreAnimID == id)
		   return (*iteratorAnimationAction)->getTime();
	  iteratorAnimationAction++;
  }
  return 0;
}

bool CalMixer::morph(int id, bool ab_Reverse)
{
  if((id < 0) || (id >= (int)m_vectorAnimation.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  // get the core animation
  CalCoreAnimation *pCoreAnimation;
  pCoreAnimation = m_pModel->getCoreModel()->getCoreAnimation(id);
  if(pCoreAnimation == 0)
  {
    return false;
  }

  // allocate a new animation action instance
  CalAnimationMorph* pAnimationMorph = MP_NEW( CalAnimationMorph);
  pAnimationMorph->setCoreID( id);
  pAnimationMorph->m_bReverse = ab_Reverse;

  if(pAnimationMorph == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__); 
    return false;
  }

  // create the new animation instance
  if(!pAnimationMorph->create(pCoreAnimation))
  {
    MP_DELETE( pAnimationMorph);
    return false;
  }

  // destroy all active morph animation
  while(!m_listAnimationMorph.empty())
  {
    CalAnimationMorph *pAnimationMorph;
    pAnimationMorph = m_listAnimationMorph.front();
    m_listAnimationMorph.pop_front();

    pAnimationMorph->destroy();
    MP_DELETE( pAnimationMorph);
  }

  // insert new animation into the table
  m_listAnimationMorph.push_front(pAnimationMorph);

  // execute the animation
  return pAnimationMorph->execute();
}

void CalMixer::updateMorph()
{
  // морфинг модели
  std::list<CalAnimationMorph *>::iterator iteratorAnimationMorph;
  iteratorAnimationMorph = m_listAnimationMorph.begin();

  while(iteratorAnimationMorph != m_listAnimationMorph.end())
  {
	  m_pModel->updateSkin((*iteratorAnimationMorph));
	  ++iteratorAnimationMorph;
  }
}

bool CalMixer::containSameBones(int id1, int id2, CalCoreSkeleton*	/*skeleton*/)
{
	CalCoreAnimation*	pAnim1 = m_pModel->getCoreModel()->getCoreAnimation( id1);
	CalCoreAnimation*	pAnim2 = m_pModel->getCoreModel()->getCoreAnimation( id2);
	//if( skeleton)
	//{
		//if( pAnim1->IsCommon()) pAnim1->FallOnSkel(skeleton);
		//if( pAnim2->IsCommon()) pAnim2->FallOnSkel(skeleton);
	//
	if( !pAnim2 || !pAnim1)	return false;
	return pAnim1->containBoneFrom( pAnim2);
}

void CalMixer::updateLookCtrl()
{

}

/*CalAnimation* CalMixer::getAnimation(int id, int type)
{
  if((id < 0) || (id >= (int)m_vectorAnimation.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return NULL;
  }

  // get the animation for the given id
  CalAnimation *pAnimation;
  pAnimation = m_vectorAnimation[id];

  // create a new animation instance if it is not active yet
  if(pAnimation == 0)
  {
    // get the core animation
    CalCoreAnimation *pCoreAnimation;
    pCoreAnimation = m_pModel->getCoreModel()->getCoreAnimation(id);
    if(pCoreAnimation == 0) return NULL;

	CalAnimation*	panim;

	switch( type){
		case ANIMTYPE_ACTION:
			panim = new CalAnimationAction();
			m_listAnimationAction.push_front((CalAnimationAction*)panim);
			break;
		case ANIMTYPE_ACTION:
			panim = new CalAnimationAction();
			m_listAnimationAction.push_front((CalAnimationAction*)panim);
			break;
		case ANIMTYPE_POSE:
			break;
		case ANIMTYPE_CYCLE:
			panim = new CalAnimationCycle();
			m_listAnimationCycle.push_front((CalAnimationCycle*)panim);
			break;
	}

    // create the new animation instance
    if(!panim->create(pCoreAnimation))
    {
      	switch( type){
			case ANIMTYPE_ACTION:
				MP_DELETE( CalAnimationAction*)panim;
				break;
			case ANIMTYPE_CYCLE:
			case ANIMTYPE_STEPCYCLE:
				MP_DELETE( CalAnimationCycle*)panim;
				break;
		}
      return NULL;
    }

    // insert new animation into the tables
    m_vectorAnimation[id] = panim;

    // blend the animation
    return panim;
  }

  // check if this is really a animation cycle instance
  if(pAnimation->getType() != type)
  {
      CalError::setLastError(CalError::INVALID_ANIMATION_TYPE, __FILE__, __LINE__);
      return NULL;
  }

  return pAnimation;
}*/

void CalMixer::coverCurrent(CalAnimationAction *action)
{
	bool	foundIntersection = false;

	FORALLMEMBERS(AnimationAction)
		if((*iteratorAnimationAction) != action)
		{
			std::list<TRACKPAIR>	tracks;
			if( action->containBoneFrom( (*iteratorAnimationAction), &tracks))
			{
				foundIntersection = true;
				while( !tracks.empty())
				{
					// go OUT from animation
					TRACKPAIR	pair = tracks.front();
					tracks.pop_front();

					pair.first->grow( action->getDuration()>0.5f?0.5f:action->getDuration());
					pair.second->slake( (*iteratorAnimationAction)->getDuration()>0.5f?0.5f:(*iteratorAnimationAction)->getDuration());
				}
				
			}
		}
	ENDFOR(AnimationAction)
}

void CalMixer::checkBonesGrow(CalAnimation *panim)
{
	std::list<CalCoreTrack *> listCoreTrack = panim->getListCoreTrack();

	std::vector<CalBone *>& vectorBone = m_pModel->getSkeleton()->getVectorBone();

	std::list<CalCoreTrack *>::iterator iteratorCoreTrack;
    for(iteratorCoreTrack = listCoreTrack.begin(); iteratorCoreTrack != listCoreTrack.end(); ++iteratorCoreTrack)
    {
		int boneId = (*iteratorCoreTrack)->getCoreBoneId();
		if( boneId < 1)	continue;
		(*iteratorCoreTrack)->checkTrns();

		if( boneId >= (int)vectorBone.size())
		{
			CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__, "Error animation BoneID!");
		}
		CalVector	pos = vectorBone[boneId]->getCoreBone()->getTranslation();
		(*iteratorCoreTrack)->setTranslationAllKeys( pos);
	}
}

void CalMixer::executeEmotion(FACEExpression *fe, float dur, float dweight)
{
	if( !m_pModel->m_pFAPU)	return;
	m_pModel->m_pFAPU->execute(fe,dur,dweight);
}

void CalMixer::removeEmotion(FACEExpression *fe, float dur)
{
	if( !m_pModel->m_pFAPU)	return;
	m_pModel->m_pFAPU->remove(fe,dur);
}

void CalMixer::set_face_expression(std::string pack1, int id1, std::string pack2, int id2, float weight)
{
	std::map<MP_STRING, EM_PACK>&	map_emotions = m_pModel->get_mapEmotions();
	std::map<MP_STRING, EM_PACK>::iterator	it;
	EM_PACK	em1, em2;
	
	if ((it=map_emotions.find(MAKE_MP_STRING(pack1))) == map_emotions.end())	return;
	em1 = (EM_PACK)it->second;
	if ((it=map_emotions.find(MAKE_MP_STRING(pack2))) == map_emotions.end())	return;
	em2 = (EM_PACK)it->second;
	assert(em1.count > id1);
	assert(em2.count > id2);

	set_face_expression( em1.emotions[id1], em2.emotions[id2], weight);
}

/************************************************************************/
/* Set face expression between <e1> & <e2> with koefficient <weight>
/************************************************************************/
void CalMixer::remove_face_expressions()
{
	if( m_pModel->m_pFAPU)
		m_pModel->m_pFAPU->remove_all_emotions();
}

void CalMixer::set_face_expression(FACEExpression* e1, FACEExpression* e2, float weight)
{
	if( !m_pModel->m_pFAPU)	return;
	if( e1 && e2)
	{
		m_pModel->m_pFAPU->execute(e1, 0.01f, 1.0f - weight);
		m_pModel->m_pFAPU->execute(e2, 0.01f, weight);
	}else
	{
		if( e1)	m_pModel->m_pFAPU->execute(e1, 0.01f, weight);
		if( e2)	m_pModel->m_pFAPU->execute(e2, 0.01f, weight);
	}
}


int CalMixer::createSequencer(int sid, int mid, int eid, pcbfSeqAccomplished pf, LPVOID pdata)
{
	int count=0;
	std::vector<CalAnimSequencer *>::iterator	it;
	for (it=m_vectorSequencers.begin(); it!=m_vectorSequencers.end(); it++, count++)
	{
		CalAnimSequencer*	seq = *it;
		if(seq->m_startID==sid && seq->m_cycleID==mid && seq->m_endID==eid)
		{
			seq->create(this,sid,mid,eid);
			if( pf)
			{	seq->pfAccomlished = pf;	seq->pdAccomlished = pdata;	}
			return count;
		}
	}
	CalAnimSequencer*	seq = MP_NEW( CalAnimSequencer);
	m_vectorSequencers.push_back(seq);
	seq->create(this,sid,mid,eid);
	seq->m_seqID = m_vectorSequencers.size()-1;
	if( pf)
	{	seq->pfAccomlished = pf;	seq->pdAccomlished = pdata;	}
	return seq->m_seqID;
}

bool CalMixer::playSequencer(int sId)
{
	if( sId >= (int)m_vectorSequencers.size())	return false;
	CalAnimSequencer*	seq = m_vectorSequencers[sId];
	seq->play();
	return true;
}

bool CalMixer::stopSequencer(int sId)
{
	if( sId >= (int)m_vectorSequencers.size())	return false;
	CalAnimSequencer*	seq = m_vectorSequencers[sId];
	seq->stop();
	return true;
}

bool CalMixer::resumeSequencer(int sId)
{
	if( sId >= (int)m_vectorSequencers.size())	return false;
	CalAnimSequencer*	seq = m_vectorSequencers[sId];
	seq->resume();
	return true;
}

float CalMixer::getDuration(int animID)
{
	if( m_pModel->getCoreModel()->getCoreAnimation(animID))
		return m_pModel->getCoreModel()->getCoreAnimation(animID)->getDuration();
	return 0.0f;
}

void CalMixer::attachTo(CalBone* parentBone, CalBone* childBone)
{
	if( !parentBone)
	{
		if( !attach_parent)	return;
		CalVector parentTrn = attach_parent->getTranslationAbsolute();
		CalQuaternion parentRtn = attach_parent->getRotationAbsolute();
		attach_rotation *= parentRtn;
		attach_translation *= parentRtn;
		attach_translation += parentTrn;
		childBone->clearState();
		childBone->blendState(1.0f, attach_translation, attach_rotation);
		attach_parent = NULL;
		return;
	}
	//
	attach_parent = parentBone;
	CalVector parentTrn = parentBone->getTranslationAbsolute();
	CalQuaternion parentRtn = parentBone->getRotationAbsolute();
	CalVector childTrn = childBone->getTranslationAbsolute();
	CalQuaternion  childRtn = childBone->getRotationAbsolute();
 
	// attach_rotation * parentRtn = childRtn
	//	attach_rotation = childRtn * parentRtn`

	parentRtn.conjugate();
	attach_rotation = childRtn;
	attach_rotation *= parentRtn;
	
	// childTrn * localTrn = parentTrn
	attach_translation = childTrn - parentTrn;
	attach_translation *= parentRtn;
}

DWORD	CalMixer::findAnimationById(int id, CalAnimation** ppAnimation)
{
	CalCoreAnimation *pCoreAnimation;
	pCoreAnimation = m_pModel->getCoreModel()->getCoreAnimation(id);
	if( pCoreAnimation == NULL)	return	ANIMTYPE_NONE;

	std::list<CalAnimationAction *>::iterator iteratorAnimationAction;
	iteratorAnimationAction = m_listAnimationAction.begin();
	while(iteratorAnimationAction != m_listAnimationAction.end())
	{
	if((*iteratorAnimationAction)->getCoreAnimation() == pCoreAnimation)
	{
	  *ppAnimation = (*iteratorAnimationAction);
	  return	ANIMTYPE_ACTION;
	} 
	++iteratorAnimationAction;
	}

	std::list<CalAnimationCycle *>::iterator iteratorAnimationCycle;
	iteratorAnimationCycle= m_listAnimationCycle.begin();
	while(iteratorAnimationCycle != m_listAnimationCycle.end())
	{
	if((*iteratorAnimationCycle)->getCoreAnimation() == pCoreAnimation)
	{
	  *ppAnimation = (*iteratorAnimationCycle);
	  return	ANIMTYPE_CYCLE;
	} 
	++iteratorAnimationCycle;
	}
	return	ANIMTYPE_NONE;
}

void CalMixer::setAnimationBasisMode()
{
	animation_basis_used = true;
	basis_position.set(0,0,0);
	basis_rotation = CalQuaternion(0,0,1,0);
}

void CalMixer::setPredefinedBasisMode( const CalVector& pos, const CalQuaternion& rtn)
{
	animation_basis_used = false;
	basis_position = pos;
	basis_rotation = rtn;
}

std::vector<std::string> CalMixer::getMotionsInfo()
{
	std::vector<std::string> info;

	{
		std::list<CalAnimationAction *>::iterator it = m_listAnimationAction.begin();
		std::list<CalAnimationAction *>::iterator itEnd = m_listAnimationAction.end();

		for ( ; it != itEnd; it++)
		{
			std::string str = "[action] delayIn:";
			str += getString((int)(*it)->getDelayIn());
			str += " delayOut:";
			str += getString((int)(*it)->getDelayOut());
			str += " delayTarget:";
			str += getString((int)(*it)->getDelayTarget());
			str += " animationName:";
			str += (*it)->getCoreAnimation()->getName();			

			info.push_back(str);
		}		
	}

	{
		std::list<CalAnimationMorph *>::iterator it = m_listAnimationMorph.begin();
		std::list<CalAnimationMorph *>::iterator itEnd = m_listAnimationMorph.end();

		for ( ; it != itEnd; it++)
		{
			std::string str = "[morph] phaze:";
			str += getString((int)(*it)->getPhaze());
			str += " animationName:";
			str += (*it)->getCoreAnimation()->getName();			

			info.push_back(str);
		}		
	}

	{
		std::list<CalAnimationCycle *>::iterator it = m_listAnimationCycle.begin();
		std::list<CalAnimationCycle *>::iterator itEnd = m_listAnimationCycle.end();

		for ( ; it != itEnd; it++)
		{
			std::string str = "[cycle] targetDelay:";
			str += getString((int)(*it)->getTargetDelay());
			str += " targetWeight:";
			str += getString((int)(*it)->getTargetWeight());
			str += " animationName:";
			str += (*it)->getCoreAnimation()->getName();			

			info.push_back(str);
		}	
	}
	
	return info;
}
