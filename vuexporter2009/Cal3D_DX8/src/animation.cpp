
//****************************************************************************//
// animation.cpp                                                              //
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

#include "animation.h"
#include "error.h"
#include "coretrack.h"
#include "skeleton.h"
#include "model.h"
#include "mixer.h"
#include "coretrack.h"
#include "corebone.h"
#include "bone.h"
#include "corekeyframe.h"
#include "coreskeleton.h"
#include "animsequencer.h"

//****************************************************************************//
// Static member variables initialization                                     //
//****************************************************************************//

const unsigned int CalAnimation::TYPE_NONE = 0;
const unsigned int CalAnimation::TYPE_CYCLE = 1;
const unsigned int CalAnimation::TYPE_POSE = 2;
const unsigned int CalAnimation::TYPE_ACTION = 3;

const unsigned int CalAnimation::STATE_NONE = 0;
const unsigned int CalAnimation::STATE_SYNC = 1;
const unsigned int CalAnimation::STATE_ASYNC = 2;
const unsigned int CalAnimation::STATE_IN = 3;
const unsigned int CalAnimation::STATE_STEADY = 4;
const unsigned int CalAnimation::STATE_OUT = 5;

 /*****************************************************************************/
/** Constructs the animation instance.
  *
  * This function is the default constructor of the animation instance.
  *****************************************************************************/

CalAnimation::CalAnimation()
{
  m_pCoreAnimation = 0;
  m_type = TYPE_NONE;
  m_state = STATE_NONE;
  m_time = 0.0f;
  m_timeFactor = 1.0f;
  m_weight = 0.0f;
  bDone = false;
  m_bRootMoving = false;
  m_bRootActive = false;
  m_bReverse = false;
  m_pSynchModel = NULL;
  m_pSequencer = NULL;
  bSelfMaster = false;
  bLocked = false;
  bPose=false;
  lastntf=0;
}

 /*****************************************************************************/
/** Destructs the animation instance.
  *
  * This function is the destructor of the animation instance.
  *****************************************************************************/

CalAnimation::~CalAnimation()
{
	destroy();
}

 /*****************************************************************************/
/** Destroys the animation instance.
  *
  * This function destroys all data stored in the animation instance and frees
  * all allocated memory.
  *****************************************************************************/

void CalAnimation::destroy()
{
  m_pCoreAnimation = 0;
  // destroy all core tracks
  while(!m_listSelfCoreTrack.empty() && bSelfMaster)
  {
    CalCoreTrack *pCoreTrack;
    pCoreTrack = m_listSelfCoreTrack.front();
    m_listSelfCoreTrack.pop_front();

    pCoreTrack->destroy();
    delete pCoreTrack;
  }

  mapNotifications.clear();
}

 /*****************************************************************************/
/** Provides access to the core animation.
  *
  * This function returns the core animation on which this animation instance
  * is based on.
  *
  * @return One of the following values:
  *         \li a pointer to the core animation
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreAnimation *CalAnimation::getCoreAnimation()
{
  return m_pCoreAnimation;
}

 /*****************************************************************************/
/** Returns the state.
  *
  * This function returns the state of the animation instance.
  *
  * @return One of the following values:
  *         \li \b STATE_NONE
  *         \li \b STATE_SYNC
  *         \li \b STATE_ASYNC
  *         \li \b STATE_IN
  *         \li \b STATE_STEADY
  *         \li \b STATE_OUT
  *****************************************************************************/

unsigned int CalAnimation::getState()
{
  return m_state;
}

 /*****************************************************************************/
/** Returns the time.
  *
  * This function returns the time of the animation instance.
  *
  * @return The time in seconds.
  *****************************************************************************/

float CalAnimation::getTime()
{
/*	if( m_pSynchModel == NULL){
		if( bDone) {
		  if( !m_bReverse)
			return getDuration();
		  else
			return 0.0;
		}else{
		  if( !m_bReverse)
			  return m_time;
		  else
			  if( getDuration() - m_time > 0)
				return getDuration() - m_time;
			  else
				return 0;
		}
	}*/
	if( m_pSynchModel == NULL){		// GET_OUT m_bReverse variable
		if( bDone) {
			return getDuration();
		}else{
			return m_time;
		}
	}
	// Если нужно синхронизировать, то вернем время анимации синхронизатора
	float	factor = 0.0;
	factor = m_pSynchModel->getMixer()->getAnimActionRelTime( dw_SynchID);
	if( factor == -1){
		if( b_was_sinchronized)
		{
			m_time = getDuration();
			m_pSynchModel = NULL;
			return m_time;
		}else{
			return 0;
		}
	}else{
		b_was_sinchronized=true;
	}
	return getDuration()*factor;
}

 /*****************************************************************************/
/** Returns the type.
  *
  * This function returns the type of the animation instance.
  *
  * @return One of the following values:
  *         \li \b TYPE_NONE
  *         \li \b TYPE_CYCLE
  *         \li \b TYPE_POSE
  *         \li \b TYPE_ACTION
  *****************************************************************************/

unsigned int CalAnimation::getType()
{
  return m_type;
}

 /*****************************************************************************/
/** Returns the weight.
  *
  * This function returns the weight of the animation instance.
  *
  * @return The weight.
  *****************************************************************************/

float CalAnimation::getWeight()
{
  /*if( bDone) 
	  return 1.0;*/
  return m_weight;
}

float CalAnimation::getPhase()
{
	if(m_state == STATE_ASYNC){
		return m_time/getDuration();
	}
	if(m_state == STATE_SYNC){
		return m_time/getDuration();
	}
	return 0.0f;
}
//****************************************************************************//

void CalAnimation::setDuration(float duration)
 {
     // get the core animation instance
     CalCoreAnimation *pCoreAnimation;
     pCoreAnimation = getCoreAnimation();
	 if( !bSelfMaster)
 		pCoreAnimation->setDuration( duration);
	 else
		 m_duration = duration;
	 m_fadeOutTime = duration;
}

bool CalAnimation::getBoneStartState(int RootID, CalVector &translation, CalQuaternion &rotation, float time)
{
	std::list<CalCoreTrack *>* listCoreTrack = &m_pCoreAnimation->getListCoreTrack();
	if( bSelfMaster) 
		listCoreTrack = &m_listSelfCoreTrack;

	std::list<CalCoreTrack *>::iterator iteratorCoreTrack;
    for(iteratorCoreTrack = listCoreTrack->begin(); iteratorCoreTrack != listCoreTrack->end(); ++iteratorCoreTrack)
    {
		int boneID = 0;
		boneID = (*iteratorCoreTrack)->getCoreBoneId();
		if( boneID == RootID){
			(*iteratorCoreTrack)->getState((time==-1)?0:time, getDuration(), translation, rotation);
			return true;
		}
	}
	return false;
}


bool CalAnimation::rootMoving()
{
	if (m_bRootMoving) {
		m_bRootMoving = false;
		return true;
	}
	return false;
}

void CalAnimation::seek(float seekTime)
{
	m_time = seekTime;
}

void CalAnimation::setSynch(CalModel *pmodel, DWORD dwid)
{
	m_pSynchModel = pmodel;
	dw_SynchID = dwid;
	b_was_sinchronized=false;
}

void CalAnimation::initRelAnimation(CalSkeleton *pSkel, std::vector<int>* ap_vecBonesIds)
{
    std::list<CalCoreTrack *>::iterator iteratorCoreTrack;
	std::vector<CalBone *>& vectorBone = pSkel->getVectorBone();

	copyCoreAnimation( pSkel);

    for(iteratorCoreTrack = m_listSelfCoreTrack.begin(); iteratorCoreTrack != m_listSelfCoreTrack.end(); ++iteratorCoreTrack)
    {
		bool bf=false;
		if( ap_vecBonesIds != NULL)
			for( int i=0; i < ap_vecBonesIds->size(); i++)
				if( (*ap_vecBonesIds)[i] == (*iteratorCoreTrack)->getCoreBoneId()) bf=true;
		if( (ap_vecBonesIds == NULL) || ( bf)){
			CalBone *pBone = NULL;
			int	boneId = (*iteratorCoreTrack)->getCoreBoneId();
			int	sz = vectorBone.size();
			if( boneId>=sz)
				continue;
			if( boneId == Cal::PROJECTION_BONE_ID)
				pBone = pSkel->get_frame();
			else
				pBone = vectorBone[boneId];

			CalVector		curtranslation;
			CalQuaternion currotation;
			curtranslation = pBone->getTranslation();
			currotation = pBone->getRotation();

			// если анимация относительная и обратная, то возьмем данные с последнего кадра
			(*iteratorCoreTrack)->initRelKeys(curtranslation, currotation, (getCoreAnimation()->m_bSaveFirst && !getCoreAnimation()->m_bRelative) ? true : false, getCoreAnimation()->m_bSaveLast/*, m_bReverse*/);
		}
    }
}

std::list<CalCoreTrack *>& CalAnimation::getListCoreTrack()
{
  if( bSelfMaster)
	return m_listSelfCoreTrack;
  else
	return m_pCoreAnimation->getListCoreTrack();
}

bool CalAnimation::rootActive()
{
	return m_bRootActive;
}

void CalAnimation::initStates(CalSkeleton *apSkeleton)
{
	if( getCoreAnimation()->m_bRelative){
	  initRelAnimation( apSkeleton);
	}else{
		/*if( getCoreAnimation()->m_bSaveFirst || getCoreAnimation()->m_bSaveLast)
			copyCoreAnimation( apSkeleton);*/
		//getCoreAnimation()->m_bSaveFirst = true;
		copyCoreAnimation( apSkeleton);
	}
}

void CalAnimation::fadeOnLastFrame(CalSkeleton *apSkeleton, float	fadeTime)
{
	if( !apSkeleton)	return;
	if( !bSelfMaster)
	{
		//fadeOut(fadeTime);
		return;
	}
	/************************************************************************/
	/* 1. Убрать все ключи из трэков, кроме последнего
	/* 2. Изменить время последнего ключа на [fadeTime]
	/* 3. Добавить первый трэк - текущее положение скелета
	/* 4. Выставить текущее время и состояние в 0 и STATE_IN
	/* 5. Выставить duration анимации в [fadeTime]
	/************************************************************************/
	// 1.
	std::list<CalCoreTrack *>::iterator iteratorCoreTrack;
	for(iteratorCoreTrack=m_listSelfCoreTrack.begin(); iteratorCoreTrack!=m_listSelfCoreTrack.end(); ++iteratorCoreTrack)
	{
		int	trackBoneId = (*iteratorCoreTrack)->getCoreBoneId();
		CalBone* pBone = apSkeleton->getBone(trackBoneId);
		if( !pBone)
		{
			CalCoreTrack*	track = *iteratorCoreTrack;
			if( track)
			{
				track->destroy();
				delete track;
			}
			iteratorCoreTrack = m_listSelfCoreTrack.erase( iteratorCoreTrack);
			iteratorCoreTrack--;
			continue;
		}
		std::map<float, CalCoreKeyframe *>& m_mapCoreKeyframe = (*iteratorCoreTrack)->getMapCoreKeyframe();
		while( m_mapCoreKeyframe.size() > 1)
		{
			CalCoreKeyframe *	begKeyFrame = m_mapCoreKeyframe.begin()->second;
			delete	begKeyFrame;
			m_mapCoreKeyframe.erase(m_mapCoreKeyframe.begin());
		}
		// 2.
		CalCoreKeyframe*	destKeyFrame = m_mapCoreKeyframe.begin()->second;
		destKeyFrame->setTime(fadeTime);
		m_mapCoreKeyframe.clear();
		m_mapCoreKeyframe.insert(std::make_pair(destKeyFrame->getTime(), destKeyFrame));
		// 3.
		CalCoreKeyframe*	srcKeyFrame = new CalCoreKeyframe();
		srcKeyFrame->create();
		srcKeyFrame->setRotation( pBone->getRotation());
		srcKeyFrame->setTranslation( pBone->getTranslation());
		srcKeyFrame->setTime( 0.0f);
		m_mapCoreKeyframe.insert(std::make_pair(srcKeyFrame->getTime(), srcKeyFrame));
	}
	// 4.
	m_state = STATE_IN;
	m_time = 0.0;
	// 5.
	setDuration(fadeTime);
}

/*
 Копирует все треки из Core- анимации в текущую.
 Если анимаци сохраняет текущее положение скелета, либо начинается
 с текущего положения, то в начало копируются все ключи.
*/
void CalAnimation::copyCoreAnimation(CalSkeleton *pSkel)
{
    std::list<CalCoreTrack *>::iterator iteratorCoreTrack;
	std::vector<CalBone *>& vectorBone = pSkel->getVectorBone();

	std::list<CalCoreTrack *>::iterator testTrack;
	testTrack = m_pCoreAnimation->getListCoreTrack().begin();
	CalCoreTrack* pTrack;
	pTrack = (*testTrack);

	float	dw_KeepWithRnd=0.0f;

	float fAddBefore=0.0;
	float fAddAfter=0.0;

	if( getCoreAnimation()->m_bSaveFirst && !getCoreAnimation()->m_bRelative)
		fAddBefore = getMaxDiffers( pSkel);

	if( getCoreAnimation()->m_bSaveLast){
		fAddAfter = getMaxDiffers( pSkel, false);
		dw_KeepWithRnd = getCoreAnimation()->getKeepLastTime();
	}

	m_listSelfCoreTrack.clear();
	for(iteratorCoreTrack = m_pCoreAnimation->getListCoreTrack().begin(); iteratorCoreTrack != m_pCoreAnimation->getListCoreTrack().end(); ++iteratorCoreTrack){
		if( (*iteratorCoreTrack)->getCoreBoneId() == Cal::NO_SUCH_BONE_ID)	continue;
		CalCoreTrack*	pSelfTrack = new CalCoreTrack();

		pSelfTrack->setCoreBoneName((*iteratorCoreTrack)->getCoreBoneName().GetBuffer());
		
		CalCoreKeyframe*	apFirstKeyframe = NULL;
		CalCoreKeyframe*	apLastKeyframe = NULL;
		if( getCoreAnimation()->m_bSaveFirst && !getCoreAnimation()->m_bRelative){
			apFirstKeyframe = new CalCoreKeyframe();
			apFirstKeyframe->create();
			apFirstKeyframe->setRotation( pSkel->getBone((*iteratorCoreTrack)->getCoreBoneId())->getRotation());
			apFirstKeyframe->setTranslation( pSkel->getBone((*iteratorCoreTrack)->getCoreBoneId())->getTranslation());
			apFirstKeyframe->setTime( fAddBefore);
		}
		if( getCoreAnimation()->m_bSaveLast){
			apLastKeyframe = new CalCoreKeyframe();
			apLastKeyframe->create();
			apLastKeyframe->setRotation( pSkel->getBone((*iteratorCoreTrack)->getCoreBoneId())->getRotation());
			apLastKeyframe->setTranslation( pSkel->getBone((*iteratorCoreTrack)->getCoreBoneId())->getTranslation());
			apLastKeyframe->setTime( fAddAfter);
		}
		
		CalCoreKeyframe* pk=(*iteratorCoreTrack)->getBegPointer();

		pSelfTrack->copyData( 
			(*iteratorCoreTrack),
			getCoreAnimation()->getDuration(),
			apFirstKeyframe, 
			apLastKeyframe, 
			dw_KeepWithRnd,
			pSkel->getRootID() == (*iteratorCoreTrack)->getCoreBoneId() ? true : false,
			m_bReverse
			);
		m_listSelfCoreTrack.push_back(pSelfTrack);
	}
	float duration = getCoreAnimation()->getDuration();
	duration += fAddBefore + fAddAfter;
	/*if( getCoreAnimation()->m_bSaveLast)
		duration += 0.5f;
	if( getCoreAnimation()->m_bSaveFirst)
		duration += 0.5f;*/

	duration += dw_KeepWithRnd;
	bSelfMaster = true;
	setDuration( duration);
}

float CalAnimation::getDuration()
{
	if( !bSelfMaster)
		return m_pCoreAnimation->getDuration();
	else
		return m_duration;
}

float CalAnimation::getMaxDiffers(CalSkeleton *pSkel, bool bFirst)
{
	std::list<CalCoreTrack *>::iterator pTrackPntr;
	std::list<CalCoreTrack *>::iterator pEndPntr;
	CalCoreTrack*						pTrack		= NULL;

	if( !bSelfMaster){
		pTrackPntr = m_pCoreAnimation->getListCoreTrack().begin();
		pEndPntr = m_pCoreAnimation->getListCoreTrack().end();
	}
	else
	{
		pTrackPntr = m_listSelfCoreTrack.begin();
		pTrackPntr = m_listSelfCoreTrack.end();
	}

	pTrack = (*pTrackPntr);

	float fMaxDifferAngle = 0.0;

	for(pTrackPntr; pTrackPntr != pEndPntr; ++pTrackPntr){
		CalVector		tTrans, bTrans, tTransP, bTransP;
		CalQuaternion	tRot, bRot;

		float	fMaxDifferAngleSimpl, fMaxDifferAnglePerp;
	
		pTrack = (*pTrackPntr);

		if( bFirst)
			pTrack->getState(0.0, getCoreAnimation()->getDuration(), tTrans, tRot);
		else
			pTrack->getState(getCoreAnimation()->getDuration(), getCoreAnimation()->getDuration(), tTrans, tRot);

		CalBone*	pBone = pSkel->getBone( pTrack->getCoreBoneId());
		if( !pBone)
		{
			continue;
		}
		bRot = pBone->getRotation();

		CalVector	v1(1, 0, 0);
		CalVector	v2(1, 0, 0);
		v1 *= bRot;	v2 *= tRot;
		fMaxDifferAngleSimpl = v1.getAngle(v2); if(fMaxDifferAngleSimpl<0) fMaxDifferAngleSimpl*=-1;

		v1 = CalVector(0, 1, 0);
		v2 = CalVector(0, 1, 0);
		v1 *= bRot;	v2 *= tRot;
		fMaxDifferAnglePerp = v1.getAngle(v2); if(fMaxDifferAngleSimpl<0) fMaxDifferAngleSimpl*=-1;

		if( fMaxDifferAngleSimpl > fMaxDifferAngle) fMaxDifferAngle = fMaxDifferAngleSimpl;
		if( fMaxDifferAnglePerp > fMaxDifferAngle) fMaxDifferAngle = fMaxDifferAnglePerp;
	}

	if( fMaxDifferAngle < 0.43f)	fMaxDifferAngle = 0.43f;
	return fMaxDifferAngle*(3.0f/3.14f);
}

void CalAnimation::getState(int id, float time, CalVector &translation, CalQuaternion &rotation)
{
    // get the list of core tracks of above core animation
    std::list<CalCoreTrack *>& listCoreTrack = getListCoreTrack();

    // loop through all core tracks of the core animation
    std::list<CalCoreTrack *>::iterator iteratorCoreTrack;
    for(iteratorCoreTrack = listCoreTrack.begin(); iteratorCoreTrack != listCoreTrack.end(); ++iteratorCoreTrack)
    {
		if( (*iteratorCoreTrack)->getCoreBoneId() == id){
			(*iteratorCoreTrack)->getState(0, m_duration, translation, rotation);
			return;
		}
	}
}

void CalAnimation::slakeTrack(int id, float dur)
{
	if( !bSelfMaster)	assert(false);
	//
    std::list<CalCoreTrack *>& listCoreTrack = getListCoreTrack();
    std::list<CalCoreTrack *>::iterator iteratorCoreTrack;
    for(iteratorCoreTrack = listCoreTrack.begin(); iteratorCoreTrack != listCoreTrack.end(); ++iteratorCoreTrack)
    {
		if( (*iteratorCoreTrack)->getCoreBoneId() == id){
			(*iteratorCoreTrack)->slake(dur);
			return;
		}
	}
}

bool CalAnimation::containBoneFrom(CalAnimation* pCheckAnim, std::list<TRACKPAIR>*	tracksout)
{
    std::list<CalCoreTrack *>& listCoreTrack1 = getListCoreTrack();
	std::list<CalCoreTrack *>& listCoreTrack2 = pCheckAnim->getListCoreTrack();

    std::list<CalCoreTrack *>::iterator iteratorCoreTrack1;
	std::list<CalCoreTrack *>::iterator iteratorCoreTrack2;

    for(iteratorCoreTrack1 = listCoreTrack1.begin(); iteratorCoreTrack1 != listCoreTrack1.end(); ++iteratorCoreTrack1)
    {
		for(iteratorCoreTrack2 = listCoreTrack2.begin(); iteratorCoreTrack2 != listCoreTrack2.end(); ++iteratorCoreTrack2)
		{
			if( (*iteratorCoreTrack1)->getCoreBoneId() == (*iteratorCoreTrack2)->getCoreBoneId())
			{
				if(!tracksout)
					return true;
				else
				{
					std::pair<CalCoreTrack *, CalCoreTrack *>	tpair;
					tpair.first = *iteratorCoreTrack1;
					tpair.second = *iteratorCoreTrack2;
					tracksout->push_back(tpair);
				}
			}
		}
	}
	if( !tracksout)	return false;
	if(	tracksout->empty())
		return false;
	else
		return true;
}

void CalAnimation::fitPosition(CalVector trn, CalQuaternion rtn)
{
	CalCoreTrack*	ptrack = *(getListCoreTrack().begin());
	if( Cal::CURRENT_FILE_VERSION < 1003)
		ptrack->fitPosition( trn, rtn);
	else
	{
		if( ptrack->getCoreBoneId() == Cal::PROJECTION_BONE_ID)
			ptrack->fitPosition( trn, rtn);
	}
}

void CalAnimation::decreaseAngle(float dec, void* data)
{
    std::list<CalCoreTrack *>& listCoreTrack = getListCoreTrack();
    std::list<CalCoreTrack *>::iterator iteratorCoreTrack;
    for(iteratorCoreTrack = listCoreTrack.begin(); iteratorCoreTrack != listCoreTrack.end(); ++iteratorCoreTrack)
		(*iteratorCoreTrack)->decreaseAngle( dec, data);
}

void CalAnimation::addNotification(float time, int id)
{
	mapNotifications.insert(std::make_pair(time,id));
}



int CalAnimation::getNotify(float time)
{
	std::map<float, int>::iterator itNotify;

	for(itNotify = mapNotifications.begin(); itNotify != mapNotifications.end(); ++itNotify)
	{
		float	maptime = itNotify->first;
		float	mapid   = itNotify->second;

		if( time > lastntf )
		{
			if( maptime > lastntf && maptime <= time)
			{
				lastntf = maptime;
				return mapid;
			}
		}
		else
		{
			if( time < lastntf && (maptime > lastntf || maptime <= time))
			{
				lastntf = maptime;
				return mapid;
			}
		}
	}
	lastntf = time;
	return -1;
}

float CalAnimation::getFloor(CalSkeleton *pSkel)
{
	float	floor = FLT_MAX;
    std::list<CalCoreTrack *>& listCoreTrack = getListCoreTrack();
    std::list<CalCoreTrack *>::iterator iteratorCoreTrack;
	
    for(iteratorCoreTrack = listCoreTrack.begin(); iteratorCoreTrack != listCoreTrack.end(); ++iteratorCoreTrack)
    {
		Cal3DComString	boneName = pSkel->getBone((*iteratorCoreTrack)->getCoreBoneId())->getCoreBone()->getName();
		if( boneName.Find((LPCTSTR)"Foot") == -1)	continue;
		float	trackFloor = (*iteratorCoreTrack)->getFloor();
		floor = (trackFloor<floor)?trackFloor:floor;
	}

	return floor;
}

//****************************************************************************//


void CalAnimation::setCoreID( int id)
{
	coreAnimID = id;
}

void CalAnimation::FallOnSkel(CalSkeleton *pSkel)
{
	FallOnSkel( pSkel->getCoreSkeleton());
}

void CalAnimation::FallOnSkel(CalCoreSkeleton *pCoreSkel)
{
//	if( !getCoreAnimation()->FallOnSkel(pCoreSkel))	return;
	if (!getCoreAnimation()->IsCommon()) return;

	std::list<CalCoreTrack *>::iterator iteratorCoreTrack;
    for(iteratorCoreTrack = getListCoreTrack().begin(); iteratorCoreTrack != getListCoreTrack().end(); ++iteratorCoreTrack)
    {
		int id = pCoreSkel->getCoreBoneIdSubName((*iteratorCoreTrack)->getCoreBoneName());
		(*iteratorCoreTrack)->setCoreBoneId( id);
		if( id == Cal::PROJECTION_BONE_ID)
		{
			// ?? 
			//(*iteratorCoreTrack)->alignFrameTranslation( CalVector);
		}
		else	if (id == 0) {
			float	skelBipHeight = pCoreSkel->getStayedBipHeight();
			if( skelBipHeight != 0.0f)
			{
				float	animBipHeight = getCoreAnimation()->getStayedBipHeight(pCoreSkel);
				if( animBipHeight != 0.0f)
				{
					(*iteratorCoreTrack)->adjustKeysTranslation(skelBipHeight/animBipHeight);
				}
			}
		}
		else	if (id > 0) {
			if( getCoreAnimation()->m_version<1009 || (*iteratorCoreTrack)->m_coreCommonBoneLength == 0.0f)
			{
				CalCoreBone*	pBone = pCoreSkel->getCoreBone(id);
				(*iteratorCoreTrack)->setKeysTranslation( pBone->getTranslation());
			}
			else
			{
				CalVector	skelBoneTranslation = pCoreSkel->getCoreBone(id)->getTranslation();
				float	skelBoneLength = skelBoneTranslation.length();
				float	animBoneLength = (*iteratorCoreTrack)->m_coreCommonBoneLength;
				if( animBoneLength > 0.1)
					(*iteratorCoreTrack)->adjustKeysTranslation( skelBoneLength/animBoneLength);
			}
		}
	}
	
//	std::list<CalCoreTrack *>::iterator iteratorCoreTrack;
    for(iteratorCoreTrack = getListCoreTrack().begin(); iteratorCoreTrack != getListCoreTrack().end(); ++iteratorCoreTrack)
    {
		if ((*iteratorCoreTrack)->getCoreBoneId() == Cal::NO_SUCH_BONE_ID) {
			(*iteratorCoreTrack)->destroy();
			delete	(*iteratorCoreTrack);
			iteratorCoreTrack = getListCoreTrack().erase(iteratorCoreTrack);
			if (iteratorCoreTrack != getListCoreTrack().end())	iteratorCoreTrack--;
		}
	}
}

void CalAnimation::translateBip(CalVector trn)
{
	if(getListCoreTrack().size() < 2)	return;
	if( Cal::CURRENT_FILE_VERSION < 1003)	return;

	std::list<CalCoreTrack *>::iterator	it = getListCoreTrack().begin();
	it++;

	CalCoreTrack*	ptrack = *(it);
	ptrack->fitPosition( trn, CalQuaternion(0,0,0,1));
}

void CalAnimation::setSequencer(CalAnimSequencer *pseq)
{
	m_pSequencer = pseq;
}

void CalAnimation::Done()
{
	bDone = true;
	if (m_pSequencer) m_pSequencer->animExecuted(coreAnimID);
}