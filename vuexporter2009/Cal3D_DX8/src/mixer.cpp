//****************************************************************************//
// mixer.cpp                                                                  //
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

#include <atlbase.h>
#if _MSC_VER > 1200
	#include <atltrace.h>
#endif
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
#include "calanimationmorph.h"
#include "stepctrl.h"
#include "corekeyframe.h"
#include "coreskeleton.h"
#include "animsequencer.h"
#include "Mpeg4FDP.h"
#include "coreanimationflags.h"
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

CalMixer::CalMixer()
{
	pSetAnimationAction = NULL;
	attach_parent = NULL;
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
  assert(m_listStepCycle.empty());
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

bool CalMixer::blendCycle(int id, float weight, float delay, bool asynch)
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
    CalAnimationCycle *pAnimationCycle;
    pAnimationCycle = new CalAnimationCycle();
    if(pAnimationCycle == 0)
    {
      CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
      return false;
    }

    // create the new animation instance
    if(!pAnimationCycle->create(pCoreAnimation))
    {
      delete pAnimationCycle;
      return false;
    }

	pAnimationCycle->setCoreID( id);

    // insert new animation into the tables
    m_vectorAnimation[id] = pAnimationCycle;
    m_listAnimationCycle.push_front(pAnimationCycle);

    // blend the animation
	pAnimationCycle->initStates( m_pModel->getSkeleton());
	pAnimationCycle->FallOnSkel( m_pModel->getSkeleton());
	//if( asynch)	pAnimationCycle->setAsync(m_animationTime, m_animationDuration);
	if( asynch)	pAnimationCycle->setAsync(0, pAnimationCycle->getDuration());
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
  if( asynch)	pAnimationCycle->setAsync(0, pAnimationCycle->getDuration());
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

  // set animation cycle to async state
  pAnimationCycle->setAsync(m_animationTime, m_animationDuration);

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
		  std::vector<VOIDPAIR>*	tracks = new std::vector<VOIDPAIR>;
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
    delete pAnimationAction;
  }

  // destroy all active animation cycles
  while(!m_listAnimationCycle.empty())
  {
    CalAnimationCycle *pAnimationCycle;
    pAnimationCycle = m_listAnimationCycle.front();
    m_listAnimationCycle.pop_front();

    pAnimationCycle->destroy();
    delete pAnimationCycle;
  }

  // destroy all active morph animation
  while(!m_listAnimationMorph.empty())
  {
    CalAnimationMorph *pAnimationMorph;
    pAnimationMorph = m_listAnimationMorph.front();
    m_listAnimationMorph.pop_front();

    pAnimationMorph->destroy();
    delete pAnimationMorph;
  }

  // destroy all active animation cycles
  while (!m_listStepCycle.empty())
  {
    CalAnimationCycle *pAnimationCycle;
    pAnimationCycle = m_listStepCycle.front();
    m_listStepCycle.pop_front();

    pAnimationCycle->destroy();
    delete pAnimationCycle;
  }

  // clear the animation table
  m_vectorAnimation.clear();

  if( pSetAnimationAction != NULL){
	  delete pSetAnimationAction;
	  pSetAnimationAction = NULL;
  }

  // clear bones tracks
  if( m_pModel->getSkeleton()){
	  for( int c = 0; c < m_pModel->getSkeleton()->getCoreSkeleton()->getVectorCoreBone().size(); c++)
	  {
		  std::vector<VOIDPAIR>*	tracks = bonesTracks[c];
		  tracks->clear();
		  delete tracks;
	  }
  }
  bonesTracks.clear();

  std::vector<CalAnimSequencer *>::iterator	sit;
  for (sit=m_vectorSequencers.begin(); sit!=m_vectorSequencers.end(); sit++)
  {
	  CalAnimSequencer*	seq = (CalAnimSequencer*)*sit;
	  delete seq;
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

bool CalMixer::executeAction(int id, float delayIn, float delayOut, CalModel* psmodel, DWORD dwsid, bool am_bReverse)
{
  if((id < 0) || (id >= (int)m_vectorAnimation.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  /*ATLTRACE("EXECUTE ACTION");
  char tmp[20];
  itoa(id, tmp, 10);
  ATLTRACE(tmp);*/

  // get the core animation
  CalCoreAnimation *pCoreAnimation;
  pCoreAnimation = m_pModel->getCoreModel()->getCoreAnimation(id);
  if(pCoreAnimation == 0)
  {
    return false;
  }

  pCoreAnimation->initialize( m_pModel->getSkeleton());

  // allocate a new animation action instance
  CalAnimationAction *pAnimationAction;
  pAnimationAction = new CalAnimationAction();
  pAnimationAction->setCoreID( id);
  pAnimationAction->m_bReverse = am_bReverse;

  // если необходима синхронизаци€
  if( psmodel != NULL)
		pAnimationAction->setSynch( psmodel, dwsid);
  if(pAnimationAction == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__); 
    return false;
  }

  // create the new animation instance
  if(!pAnimationAction->create(pCoreAnimation))
  {
    delete pAnimationAction;
    return false;
  }

  /*if( pCoreAnimation->m_bRelative){
	  pAnimationAction->initRelAnimation( m_pModel->getSkeleton());
  }*/

  /*if( !am_bReverse)
	pCoreAnimation->m_bSaveFirst = true;
  else
	pCoreAnimation->m_bSaveLast = true;*/

  pAnimationAction->initStates( m_pModel->getSkeleton());
  pAnimationAction->FallOnSkel( m_pModel->getSkeleton());


  /*CalVector		translation;
  CalQuaternion	rotation;
  int			rootID;
  rootID = m_pModel->getSkeleton()->getRootID();
  //if(( pAnimationAction->getRootState(rootID, translation, rotation))&&(!(memcmp(&m_pModel->vTranslation,&CalVector(0, 0, 0), sizeof(CalVector)) == 0))){
  if(( pAnimationAction->getBoneStartState(rootID, translation, rotation)||
	  (pAnimationAction->getCoreAnimation()->m_strLock.compare("") != 0)*//*&&
	  ((!(memcmp(&m_pModel->vTranslation,&CalVector(0, 0, 0), sizeof(CalVector)) == 0))||
	  (!(memcmp(&m_pModel->qRotation,&CalQuaternion(CalVector(0, 0, 1), 0), sizeof(CalQuaternion)) == 0)))||
	  (pAnimationAction->getCoreAnimation()->m_strLock.compare("") != 0)
	  *///){
	CheckAnimPlace( pAnimationAction);
	//checkBonesGrow( pAnimationAction);
  //}
  if( !m_pModel->WasMoved())	m_pModel->vTranslation = CalVector(0.001f, 0, 0);
  // insert new animation into the table
  if( pCoreAnimation->getFlags() & CAL3D_COREANIM_PRIMARY)
  {
	  m_listAnimationAction.push_back(pAnimationAction);
  }
  else
  {
	  m_listAnimationAction.push_front(pAnimationAction);
  }

  // проверить: есть ли на текущий момент движущиес€ кости, присутствующие в данной анимации
  //  если есть, то плавно их погасить, а новые включить
  coverCurrent(pAnimationAction);

#ifdef DEBUG
  if( pAnimationAction->getDuration() > 10)
	  {
	  float iii=pAnimationAction->getDuration();
	  int ii=0;
	  }
#endif

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

  StepCtrl*	stepControl = m_pModel->m_pStepCtrl;
  if(stepControl != NULL){
	  bActiveAnimationPresent = true;
	  stepControl->stepIt( deltaTime);
	  if(stepControl->done()){
		  m_pModel->clearStep();
	  }
  }

  // јнимаци€ дл€ команды "SET"
  if( pSetAnimationAction != NULL)	{
	  bActiveAnimationPresent = true;
	  pSetAnimationAction->update( 0);
	  dwLastSetAccTime = dwSetAccTime;
	  dwSetAccTime += deltaTime;
	  if( dwSetAccTime >= dwSetDuration){
		//m_pModel->setDoneFlag(AT_SET, pSetAnimationAction->coreAnimID);
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
		if( (*iteratorAnimationAction)->isDone())
		{
		  m_pModel->setDoneFlag(AT_ACTION, (*iteratorAnimationAction)->coreAnimID);
		  (*iteratorAnimationAction)->destroy();
		  delete (*iteratorAnimationAction);
		  iteratorAnimationAction = m_listAnimationAction.erase(iteratorAnimationAction);
		}else
		{
			(*iteratorAnimationAction)->Done();
			if(stepControl != NULL)
				stepControl->animDone( (*iteratorAnimationAction)->coreAnimID);
			++iteratorAnimationAction;
		}
    }
  }

  // todo: update all active animation poses of this model

  // update the weight of all active animation cycles of this model
  std::list<CalAnimationCycle *>::iterator iteratorAnimationCycle;
  iteratorAnimationCycle = m_listAnimationCycle.begin();

  float accumulatedWeight, accumulatedDuration;
  accumulatedWeight = 0.0f;
  accumulatedDuration = 0.0f;

  while(iteratorAnimationCycle != m_listAnimationCycle.end())
  {
	bActiveAnimationPresent = true;
    // update and check if animation cycle is still active
    if((*iteratorAnimationCycle)->update(deltaTime))
    {
	  float animationTime;
      if((*iteratorAnimationCycle)->getState() == CalAnimation::STATE_SYNC)
	  {
        if(m_animationDuration == 0.0f)
		{
          animationTime = 0.0f;
		}
        else
		{
          animationTime = m_animationTime * (*iteratorAnimationCycle)->getDuration() / m_animationDuration;
		}
	  }
      else
	  {
        animationTime = (*iteratorAnimationCycle)->getTime();
	  }
	  int id=-1;
	  if( (*iteratorAnimationCycle)->getDuration() != 0)
	  {
		float ph = animationTime/(*iteratorAnimationCycle)->getDuration();
		while( (id = (*iteratorAnimationCycle)->getNotify(ph)) != -1)
		  m_pModel->setDoneFlag( (ACTIONTYPE)id, 0);
	  }

      // check if it is in sync. if yes, update accumulated weight and duration
      if((*iteratorAnimationCycle)->getState() == CalAnimation::STATE_SYNC)
      {
        accumulatedWeight += (*iteratorAnimationCycle)->getWeight();
        accumulatedDuration += (*iteratorAnimationCycle)->getWeight() * (*iteratorAnimationCycle)->getDuration();
      }

      ++iteratorAnimationCycle;
    }
    else
    {
      // animation cycle has ended, destroy and remove it from the animation list
      (*iteratorAnimationCycle)->destroy();
      delete (*iteratorAnimationCycle);
      iteratorAnimationCycle = m_listAnimationCycle.erase(iteratorAnimationCycle);
    }
  }

  // update the weight of all active animation step cycles of this model
  //std::list<CalAnimationCycle *>::iterator iteratorAnimationCycle;
  iteratorAnimationCycle = m_listStepCycle.begin();

  //float accumulatedWeight, accumulatedDuration;
//  accumulatedWeight = 0.0f;
//  accumulatedDuration = 0.0f;

  while(iteratorAnimationCycle != m_listStepCycle.end())
  {
	bActiveAnimationPresent = true;
    // update and check if animation cycle is still active
    if((*iteratorAnimationCycle)->update(deltaTime))
    {
      // check if it is in sync. if yes, update accumulated weight and duration
      if((*iteratorAnimationCycle)->getState() == CalAnimation::STATE_SYNC)
      {
        accumulatedWeight += (*iteratorAnimationCycle)->getWeight();
        accumulatedDuration += (*iteratorAnimationCycle)->getWeight() * (*iteratorAnimationCycle)->getDuration();
      }
	  
      float animationTime;
      if((*iteratorAnimationCycle)->getState() == CalAnimation::STATE_SYNC)
	  {
        if(m_animationDuration == 0.0f)
		{
          animationTime = 0.0f;
		}
        else
		{
          animationTime = m_animationTime * (*iteratorAnimationCycle)->getDuration() / m_animationDuration;
		}
	  }
      else
	  {
        animationTime = (*iteratorAnimationCycle)->getTime();
	  }
	  int id = 0;
	  while( (id = (*iteratorAnimationCycle)->getNotify(animationTime/(*iteratorAnimationCycle)->getDuration())) != -1)
	  {
		  m_pModel->setDoneFlag( (ACTIONTYPE)id, 0);
	  }
      ++iteratorAnimationCycle;
    }
    else
    {
      // animation cycle has ended, destroy and remove it from the animation list
	  (*iteratorAnimationCycle)->Done();
      (*iteratorAnimationCycle)->destroy();
      delete (*iteratorAnimationCycle);
      iteratorAnimationCycle = m_listStepCycle.erase(iteratorAnimationCycle);
    }
  }

   // adjust the global animation cycle duration
  if(accumulatedWeight > 0.0f)
  {
    m_animationDuration = accumulatedDuration / accumulatedWeight;
  }
  else
  {
    m_animationDuration = 0.0f;
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

  bool affectAnimMoving = false;
  bool m_bSaveRootPos = false;
  bool m_bSetDelete  = false;

  // get the skeleton we need to update
  CalSkeleton *pSkeleton;
  pSkeleton = m_pModel->getSkeleton();
  if(pSkeleton == 0) return;

  // clear the skeleton state
  pSkeleton->clearState();

  // get the bone vector of the skeleton
  //std::vector<CalBone *>& vectorBone = pSkeleton->getVectorBone();

  if( pSetAnimationAction != NULL)	{
    // get the core animation instance
    CalCoreAnimation *pCoreAnimation;
    pCoreAnimation = pSetAnimationAction->getCoreAnimation();

    // get the duration of the core animation
    float duration;
    duration = pCoreAnimation->getDuration();

    // get the list of core tracks of above core animation
    std::list<CalCoreTrack *>& listCoreTrack = pSetAnimationAction->getListCoreTrack();

    // loop through all core tracks of the core animation
    std::list<CalCoreTrack *>::iterator iteratorCoreTrack;
    for(iteratorCoreTrack = listCoreTrack.begin(); iteratorCoreTrack != listCoreTrack.end(); ++iteratorCoreTrack)
    {
      // get the appropriate bone of the track
      CalBone *pBone;
      pBone = pSkeleton->getBone((*iteratorCoreTrack)->getCoreBoneId());
//#ifdef DEBUG
//if( (*iteratorCoreTrack)->getCoreBoneId() == Cal::PROJECTION_BONE_ID)
//int projection = 0;
//#endif
	  if( !pBone)	continue;

      // get the current translation and rotation
	  bool bClear = true;
      CalVector translation;
      CalQuaternion rotation;
      bClear = (*iteratorCoreTrack)->getState(0, duration, translation, rotation);

      // blend the bone state with the new state
	  if(dwSetAccTime > dwSetDuration)
		  dwSetAccTime = dwSetDuration;
	  float setFactor = 1.0;
	  if(( dwSetAccTime == 0)&&( dwSetDuration != 0)) setFactor = 0.0;
	  if( dwSetAccTime - dwLastSetAccTime != 0.0)
		  setFactor = (dwSetAccTime - dwLastSetAccTime)/(dwSetDuration - dwLastSetAccTime);
		  //setFactor = (dwSetAccTime - dwLastSetAccTime) / dwSetDuration;
		  //setFactor = dwSetAccTime / dwSetDuration;
	  if( setFactor > 1) setFactor = 1.0;

	  CalVector		tr = pBone->getTranslation();
	  CalQuaternion	rt = pBone->getRotation();

	  rt.blend( setFactor, rotation);
	  tr.blend( setFactor, translation);
	  if( (*iteratorCoreTrack)->getCoreBoneId() > 0)	
		  tr = pBone->getCoreBone()->getTranslation();
	  pBone->blendState(1.0, tr, rt);
    }
	if(dwSetAccTime >= dwSetDuration){
		m_bSetDelete = true;
	}
  }

  /*ATLTRACE("ANIMATION ACTION COUNT = ");
  int kk = m_listAnimationAction.size();
  char tmp[20];
  itoa(kk, tmp, 10);
  ATLTRACE(tmp);*/

  bool	slakeFound = false;
  // loop through all animation actions
  std::list<CalAnimationAction *>::iterator iteratorAnimationAction;
  for(iteratorAnimationAction = m_listAnimationAction.begin(); iteratorAnimationAction != m_listAnimationAction.end(); ++iteratorAnimationAction)
  {
    // get the core animation instance
    CalCoreAnimation *pCoreAnimation;
    pCoreAnimation = (*iteratorAnimationAction)->getCoreAnimation();

    // get the duration of the core animation
    float duration;
    duration = (*iteratorAnimationAction)->getDuration();

    // get the list of core tracks of above core animation
    std::list<CalCoreTrack *>& listCoreTrack = (*iteratorAnimationAction)->getListCoreTrack();//pCoreAnimation->getListCoreTrack();

    // loop through all core tracks of the core animation
    std::list<CalCoreTrack *>::iterator iteratorCoreTrack;
    for(iteratorCoreTrack = listCoreTrack.begin(); iteratorCoreTrack != listCoreTrack.end(); ++iteratorCoreTrack)
    {
	  int boneId = (*iteratorCoreTrack)->getCoreBoneId();
	  if(boneId == -1)
		  int ksdfj=0;
	  // check for slake/groing
	  if( boneId != Cal::PROJECTION_BONE_ID)
	  {
		  int	trackCount = bonesTracks.size();
		  if( boneId < trackCount)
		  {
			  if((*iteratorCoreTrack)->growing() || (*iteratorCoreTrack)->slaking())
			  {
				  slakeFound = true;
				  VOIDPAIR	pair;
				  pair.first = *iteratorCoreTrack;
				  pair.second = *iteratorAnimationAction;
				  if( boneId != Cal::PROJECTION_BONE_ID)
					bonesTracks[boneId]->push_back( pair);
				  continue;
			  }
		  }
	  }
	  if( (*iteratorCoreTrack)->getWeight() == 0.0)
		  continue;

      // get the appropriate bone of the track
      CalBone *pBone;
      pBone = pSkeleton->getBone((*iteratorCoreTrack)->getCoreBoneId());
	  if( !pBone)	continue;

      // get the current translation and rotation
      CalVector translation;
      CalQuaternion rotation;
      bool bClear = true;
	  bClear = (*iteratorCoreTrack)->getState((*iteratorAnimationAction)->getTime(), duration, translation, rotation);

	  if( (*iteratorCoreTrack)->getCoreBoneId() > 0)	
		  translation = pBone->getCoreBone()->getTranslation();
	  else
	  {
		  int jehrfwerf=0;
	  }
      pBone->blendState((*iteratorAnimationAction)->getWeight(), translation, rotation);

	  if( boneId == -1)
	  {
		CalQuaternion quat = pBone->getRotationAbsolute();
		CalVector v2(1, 0, 0);
		v2 *= quat;
		if( v2.dot(CalVector(0, -1, 0)) > 0.99)
		{
			int ii=0;
		}
	  }
    }
  }

  /*if( slakeFound)
  {
	  for( int c = 0; c < bonesTracks.size(); c++)
	  {
		  if( bonesTracks[c]->size() > 0)
		  {
			  for( int j = bonesTracks[c]->size()-1; j >= 0; j--)
			  {
				VOIDPAIR	pair = (*bonesTracks[c])[j];
				CalCoreTrack* ptrack = (CalCoreTrack*)pair.first;
				CalAnimation* panim	 = (CalAnimation*)pair.second;
				CalBone *pBone = pSkeleton->getBone(ptrack->getCoreBoneId());
				if( !pBone)	continue;

				// get the current translation and rotation
				CalVector translation;
				CalQuaternion rotation;
				bool bClear = true;
				bClear = ptrack->getState(panim->getTime(), panim->getDuration(), translation, rotation);

				float m_weight = ptrack->getWeight();
				if( (m_weight = ptrack->getWeight()) != 0.0)
					pBone->blendState(m_weight, translation, rotation);
			  }
			  bonesTracks[c]->clear();
		  }
	  }
  }*/

  // lock the skeleton state
  pSkeleton->lockState();

  /*ATLTRACE("ANIMATION CYCLE COUNT = ");
  kk = m_listAnimationCycle.size();
  itoa(kk, tmp, 10);
  ATLTRACE(tmp);*/

  // loop through all animation cycles
  std::list<CalAnimationCycle *>::iterator iteratorAnimationCycle;
  for(iteratorAnimationCycle = m_listAnimationCycle.begin(); iteratorAnimationCycle != m_listAnimationCycle.end(); ++iteratorAnimationCycle)
  {
    // get the core animation instance
    CalCoreAnimation *pCoreAnimation;
    pCoreAnimation = (*iteratorAnimationCycle)->getCoreAnimation();

    // calculate adjusted time
    float animationTime;
    if((*iteratorAnimationCycle)->getState() == CalAnimation::STATE_SYNC)
    {
      if(m_animationDuration == 0.0f)
      {
        animationTime = 0.0f;
      }
      else
      {
        animationTime = m_animationTime * (*iteratorAnimationCycle)->getDuration() / m_animationDuration;
      }
    }
    else
    {
      animationTime = (*iteratorAnimationCycle)->getTime();
    }

    // get the duration of the core animation
    float duration;
    duration = (*iteratorAnimationCycle)->getDuration();

    // get the list of core tracks of above core animation
    std::list<CalCoreTrack *>& listCoreTrack = (*iteratorAnimationCycle)->getListCoreTrack();//pCoreAnimation->getListCoreTrack();

    // loop through all core tracks of the core animation
    std::list<CalCoreTrack *>::iterator iteratorCoreTrack;
    for(iteratorCoreTrack = listCoreTrack.begin(); iteratorCoreTrack != listCoreTrack.end(); ++iteratorCoreTrack)
    {
      // get the appropriate bone of the track
      CalBone *pBone;
      pBone = pSkeleton->getBone((*iteratorCoreTrack)->getCoreBoneId());
//#ifdef DEBUG
//if( (*iteratorCoreTrack)->getCoreBoneId() == Cal::PROJECTION_BONE_ID)
//int projection = 0;
//#endif
	  if( !pBone)	continue;

      // get the current translation and rotation
      CalVector translation;
      CalQuaternion rotation;
	  bool bClear = true;
	  bClear = (*iteratorCoreTrack)->getState(animationTime, duration, translation, rotation);
//translation = pBone->getCoreBone()->getTranslation();
      pBone->blendState((*iteratorAnimationCycle)->getWeight(), translation, rotation);
    }
  }
  // lock the skeleton state
  pSkeleton->lockState();


  // проверим на вертикальность скелета
  //
  //pSkeleton->checkBipVertical();
  
  // let the skeleton calculate its final state
  pSkeleton->calculateState();

  if( !m_pModel->WasMoved()){
	  m_pModel->vTranslation = CalVector(0.001f, 0, 0);
	  m_pModel->setFloor( pSkeleton->getFloor());
  }

  if( m_bSaveRootPos){
	  m_pModel->saveRotPoint();
  }

  //
  // ѕроверка на блокировку костей
  if( pSetAnimationAction != NULL && !pSetAnimationAction->getCoreAnimation()->m_strLock.IsEmpty()){
	  /*CalVector	curPos = pSkeleton->getBone(77f9f9df   INT         3
	  
		pSkeleton->getCoreSkeleton()->getCoreBoneIdSubName( 
								pSetAnimationAction->getCoreAnimation()->m_strLock
							))->getTranslationAbsolute();
	  curPos = m_pModel->transformVector( curPos);
	  CalVector	needPos = pSetAnimationAction->m_vLock;
  
	  CalVector delta = needPos; delta -= curPos;

	  m_pModel->vTranslation += delta;*/
	  m_pModel->adjustBoneLock(pSetAnimationAction);
  }

  {
	  FORALLMEMBERS(AnimationAction)
		  if( !(*iteratorAnimationAction)->getCoreAnimation()->m_strLock.IsEmpty())
			  m_pModel->adjustBoneLock((*iteratorAnimationAction));
	  ENDFOR(AnimationAction)
  }

  {
	  FORALLMEMBERS2T(AnimationCycle, StepCycle)
		  if( !(*iteratorStepCycle)->getCoreAnimation()->m_strLock.IsEmpty())
			  m_pModel->adjustBoneLock((*iteratorStepCycle));
	  ENDFOR(StepCycle)
  }
  m_pModel->stockFloor();

  if( m_bSetDelete){
		m_pModel->setDoneFlag(AT_SET, pSetAnimationAction->coreAnimID);
	  	delete pSetAnimationAction;
		pSetAnimationAction = NULL;
  }

  // check for attachments
  CalBone*	m_pFrame=NULL;
  if(attach_parent && (m_pFrame=m_pModel->getSkeleton()->getBone(Cal::PROJECTION_BONE_ID))!=NULL)
  {
		CalVector		trn = attach_translation;
		trn *= attach_parent->getRotationAbsolute();
		trn += attach_parent->getTranslationAbsolute();
		CalQuaternion	rtn = attach_rotation;
		rtn *= attach_parent->getRotationAbsolute();
		m_pFrame->clearState();
		m_pFrame->blendState(1.0, trn, rtn);
  }
}


bool CalMixer::executeStep(int id, float weight, float delay)
{
  // get the animation for the given id
  CalAnimation *pAnimation = getAnimation( id, ANIMTYPE_STEPCYCLE);

  // blend the animation cycle
  m_pModel->m_pStepCtrl->setDuration( pAnimation->getDuration());
 
  //if( pAnimation->getBoneStartState(m_pModel->getSkeleton()->getRootID(), translation, rotation))
  CheckAnimPlace( pAnimation);
  //checkBonesGrow( pAnimation);

  //pAnimation->decreaseAngle( 0.8, &(m_pModel->getSkeleton()->getCoreSkeleton()->getVectorCoreBone()));
  pAnimation->addNotification( 0.067f, (int)AT_STEPSOUNDR);
  pAnimation->addNotification( 0.53f, (int)AT_STEPSOUNDL);
  pAnimation->setCoreID( id);
  return ((CalAnimationCycle*)pAnimation)->blend(weight, delay);
}


//****************************************************************************//
bool CalMixer::setAction(int id, DWORD dur, bool bPose, bool inPlace)
{
	if( pSetAnimationAction != NULL){
	  //return false;
	  delete pSetAnimationAction;
	}

  if((id < 0) || (id >= (int)m_vectorAnimation.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  /*ATLTRACE("SET ACTION");
  char tmp[20];
  itoa(id, tmp, 10);
  ATLTRACE(tmp);*/

  // get the core animation
  CalCoreAnimation *pCoreAnimation;
  pCoreAnimation = m_pModel->getCoreModel()->getCoreAnimation(id);
  if(pCoreAnimation == 0)
  {
    return false;
  }

  pCoreAnimation->initialize( m_pModel->getSkeleton());

  // allocate a new animation action instance
  //CalAnimationAction *pAnimationAction;
  pSetAnimationAction = new CalAnimationAction();
  dwSetDuration = dur/1000.0f;
  dwSetAccTime = 0;

  pSetAnimationAction->setCoreID( id);
  if(pSetAnimationAction == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return false;
  }

  // create the new animation instance
  if(!pSetAnimationAction->create(pCoreAnimation))
  {
    delete pSetAnimationAction;
    return false;
  }

  pSetAnimationAction->initStates( m_pModel->getSkeleton());
  pSetAnimationAction->FallOnSkel( m_pModel->getSkeleton());

  CheckAnimPlace( pSetAnimationAction, inPlace);
  //checkBonesGrow( pSetAnimationAction);

  // execute the animation
  return pSetAnimationAction->execute(0, 0);
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
  pCoreAnimation->setDuration( duration);
}

void CalMixer::clearAction(int id, float fadeOut)
{
  CalAnimation*	pAnimation;
  DWORD	type = findAnimationById(id, &pAnimation);
  if( pAnimation == NULL)	return;

  /*ATLTRACE("CLEAR ACTION");
  char tmp[20];
  itoa(id, tmp, 10);
  ATLTRACE(tmp);*/

  switch( type)
  {
	case	ANIMTYPE_POSE:
	  delete pSetAnimationAction;
	  pSetAnimationAction = NULL;
	  break;
	case ANIMTYPE_ACTION:
	  ((CalAnimationAction*)pAnimation)->fadeOut(fadeOut);
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
/* inanimPlace - установить модель на позицию, вз€тую из анимации
/************************************************************************/
void CalMixer::CheckAnimPlace(CalAnimation* pAnim, bool inanimPlace)
{
	if( !pAnim)	return;

	int			rootID = m_pModel->getSkeleton()->getRootID();
	CalVector		translation;
	CalQuaternion	rotation;
	bool	frameStateCaptured = pAnim->getBoneStartState(Cal::PROJECTION_BONE_ID, translation, rotation);

	if( frameStateCaptured &&	// Added 5.01.06 дл€ того, чтобы ¬ — с жуками работала. Ќо вроде правильна€ затычка ;)
		pAnim->getCoreAnimation() && pAnim->getCoreAnimation()->m_version>1005 && !pAnim->getCoreAnimation()->m_bRelative)
	{
		m_pModel->relative_pos_changed = false;
		m_pModel->getSkeleton()->set_frame_move(CalVector(0,0,0), CalQuaternion(0,0,0,1));
		return;
	}

	if( !pAnim->getCoreAnimation()->m_bRelative)
	{
		if( (m_pModel->WasMoved() || inanimPlace) && frameStateCaptured)
		{
			CalVector	pos(0,0,0);
			CalQuaternion quat(0,0,0,1);
			if( !inanimPlace)	// оставить модель на месте. ѕереносим анимацию в текущее положение
			{
				if( m_pModel->m_pfExternTransform)
				{
					m_pModel->m_pfExternTransform(pos, quat, m_pModel->m_pExternClass);
					m_pModel->getSkeleton()->set_frame_move(pos, quat);
					CalVector	dv;
					pAnim->m_vRootTranslation = pos - translation;
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
	m_pModel->getSkeleton()->set_frame_move(translation, CalQuaternion(0,0,0,1));
					translation.z = bipTrn.z - bh;
					pAnim->m_vRootTranslation = translation;
					m_pModel->relative_pos_changed = false;
				}else
				{
					// frame translation
					m_pModel->vTranslation = translation;
					translation.invert();
					translation.z += bh;
					m_pModel->vTranslation.z -= bh;
					pAnim->m_vRootTranslation = translation;
				}
			}

			if( !inanimPlace)	// оставить модель на месте. ѕовернем анимацию так, чтобы она совпала с текущим положением
			{
				if( m_pModel->m_pfExternTransform)
				{
					CalVector xmodel(1,0,0);
					CalVector xanim(1,0,0);
					xmodel *= quat;
					xanim *= rotation;
			 		if( xanim.dot(xmodel) > 0.99)
					{
						pAnim->m_vRootRotation = CalQuaternion(0,0,0,1);
					}
					else	if( xanim.dot(xmodel) < -0.99)
					{
						pAnim->m_vRootRotation = CalQuaternion(CalVector(0,0,1),3.141692);
					}
					else	
					{
						pAnim->m_vRootRotation = CalQuaternion(xanim.getNormal(xmodel), -xanim.getAngle(xmodel));
					}
				}
			
			}else
			{
				pAnim->m_vRootRotation = CalQuaternion(0,0,0,1);
			}
			pAnim->fitPosition( pAnim->m_vRootTranslation, pAnim->m_vRootRotation);
		}

		if( !pAnim->bLocked && !pAnim->getCoreAnimation()->m_strLock.IsEmpty()){
			  pAnim->m_vLock = m_pModel->getSkeleton()->getBone(
									m_pModel->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName( 
										pAnim->getCoreAnimation()->m_strLock
									))->getTranslationAbsolute();
			  pAnim->m_vLock = m_pModel->transformVector( pAnim->m_vLock);
			  pAnim->bLocked = true;
		}
	}
	else
	{
		pAnim->getBoneStartState(Cal::PROJECTION_BONE_ID, translation, rotation);
		CalVector	pos(0,0,0);
		CalQuaternion quat(0,0,0,1);
		if( !inanimPlace)	// оставить модель на месте. ѕереносим анимацию в текущее положение
		{
			if( m_pModel->m_pfExternTransform)
			{
				m_pModel->m_pfExternTransform(pos, quat, m_pModel->m_pExternClass);
				m_pModel->getSkeleton()->set_frame_move(pos, quat);
				CalVector	dv;
				pAnim->m_vRootTranslation = pos - translation;
				pAnim->fitPosition( pAnim->m_vRootTranslation, CalQuaternion(CalVector(0,0,1),0));
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
	  if( (*iteratorAnimationAction)->coreAnimID == id)
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
  CalAnimationMorph* pAnimationMorph;
  pAnimationMorph = new CalAnimationMorph();
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
    delete pAnimationMorph;
    return false;
  }

  // destroy all active morph animation
  while(!m_listAnimationMorph.empty())
  {
    CalAnimationMorph *pAnimationMorph;
    pAnimationMorph = m_listAnimationMorph.front();
    m_listAnimationMorph.pop_front();

    pAnimationMorph->destroy();
    delete pAnimationMorph;
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

bool CalMixer::containSameBones(int id1, int id2, CalCoreSkeleton*	skeleton)
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

CalAnimation* CalMixer::getAnimation(int id, int type)
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
		case ANIMTYPE_POSE:
			break;
		case ANIMTYPE_CYCLE:
			panim = new CalAnimationCycle();
			m_listAnimationCycle.push_front((CalAnimationCycle*)panim);
			break;
		case ANIMTYPE_STEPCYCLE:
			panim = new CalAnimationCycle();
			m_listStepCycle.push_front((CalAnimationCycle*)panim);
			break;
	}

    // create the new animation instance
    if(!panim->create(pCoreAnimation))
    {
      	switch( type){
			case ANIMTYPE_ACTION:
				delete (CalAnimationAction*)panim;
				break;
			case ANIMTYPE_CYCLE:
			case ANIMTYPE_STEPCYCLE:
				delete (CalAnimationCycle*)panim;
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
}

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

					pair.first->grow( action->getDuration()>0.5?0.5:action->getDuration());
					pair.second->slake( (*iteratorAnimationAction)->getDuration()>0.5?0.5:(*iteratorAnimationAction)->getDuration());
				}
				
			}
		}
	ENDFOR(AnimationAction)
	//
/*	std::list<TRACKPAIR>	tracks;
	if( pSetAnimationAction && (action!=pSetAnimationAction) && action->containBoneFrom( pSetAnimationAction, &tracks))
	{
		foundIntersection = true;
		while( !tracks.empty())
		{
			// go OUT from animation
			TRACKPAIR	pair = tracks.front();
			tracks.pop_front();

			pair.first->grow( 0.5);
			pair.second->slake( 0.5);
		}
	}*/
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

		if( boneId >= vectorBone.size())
		{
			CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__, "Error animation BoneID!");
		}
		CalVector	pos = vectorBone[boneId]->getCoreBone()->getTranslation();
		int boneId2 = (*iteratorCoreTrack)->getCoreBoneId();
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

void CalMixer::set_face_expression(Cal3DComString pack1, int id1, Cal3DComString pack2, int id2, float weight)
{
	std::map<Cal3DComString, EM_PACK>&	map_emotions = m_pModel->get_mapEmotions();
	std::map<Cal3DComString, EM_PACK>::iterator	it;
	EM_PACK	em1, em2;
	
	if ((it=map_emotions.find(pack1)) == map_emotions.end())	return;
	em1 = (EM_PACK)it->second;
	if ((it=map_emotions.find(pack2)) == map_emotions.end())	return;
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
	CalAnimSequencer*	seq = new CalAnimSequencer();
	m_vectorSequencers.push_back(seq);
	seq->create(this,sid,mid,eid);
	seq->m_seqID = m_vectorSequencers.size()-1;
	if( pf)
	{	seq->pfAccomlished = pf;	seq->pdAccomlished = pdata;	}
	return seq->m_seqID;
}

bool CalMixer::playSequencer(int sId)
{
	if( sId >= m_vectorSequencers.size())	return false;
	CalAnimSequencer*	seq = m_vectorSequencers[sId];
	seq->play();
	return true;
}

bool CalMixer::stopSequencer(int sId)
{
	if( sId >= m_vectorSequencers.size())	return false;
	CalAnimSequencer*	seq = m_vectorSequencers[sId];
	seq->stop();
	return true;
}

bool CalMixer::resumeSequencer(int sId)
{
	if( sId >= m_vectorSequencers.size())	return false;
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

	if( pSetAnimationAction && pSetAnimationAction->getCoreAnimation() == pCoreAnimation)
	{
	  *ppAnimation = pSetAnimationAction;
	  return	ANIMTYPE_POSE;
	}

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
