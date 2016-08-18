//****************************************************************************//
// coretrack.cpp                                                              //
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

#include "coretrack.h"
#include "error.h"
#include "corekeyframe.h"
#include "corebone.h"

 /*****************************************************************************/
/** Constructs the core track instance.
  *
  * This function is the default constructor of the core track instance.
  *****************************************************************************/

const unsigned int CalCoreTrack::STATE_IN = 3;
const unsigned int CalCoreTrack::STATE_STEADY = 4;
const unsigned int CalCoreTrack::STATE_OUT = 5;

CalCoreTrack::CalCoreTrack()
{
  m_coreBoneId = -1;
  m_state = STATE_STEADY;
  m_weight = 1.0;
  m_bClearTrans = false;
  m_coreCommonBoneLength = 0.0f;
}

 /*****************************************************************************/
/** Destructs the core track instance.
  *
  * This function is the destructor of the core track instance.
  *****************************************************************************/

CalCoreTrack::~CalCoreTrack()
{
  assert(m_mapCoreKeyframe.empty());
}

 /*****************************************************************************/
/** Adds a core keyframe.
  *
  * This function adds a core keyframe to the core track instance.
  *
  * @param pCoreKeyframe A pointer to the core keyframe that should be added.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreTrack::addCoreKeyframe(CalCoreKeyframe *pCoreKeyframe)
{
  m_mapCoreKeyframe.insert(std::make_pair(pCoreKeyframe->getTime(), pCoreKeyframe));
  if( m_mapCoreKeyframe.size() > 135){
	  int iii=0;
  }

  //m_bClearTrans = checkTrns();
  return true;
}

 /*****************************************************************************/
/** Creates the core track instance.
  *
  * This function creates the core track instance.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreTrack::create()
{
  return true;
}

 /*****************************************************************************/
/** Destroys the core track instance.
  *
  * This function destroys all data stored in the core track instance and frees
  * all allocated memory.
  *****************************************************************************/

void CalCoreTrack::destroy()
{
  // destroy all core keyframes
  std::map<float, CalCoreKeyframe *>::iterator iteratorCoreKeyframe;

  for(iteratorCoreKeyframe = m_mapCoreKeyframe.begin(); iteratorCoreKeyframe != m_mapCoreKeyframe.end(); ++iteratorCoreKeyframe)
  {
    CalCoreKeyframe *pCoreKeyframe;
    pCoreKeyframe = iteratorCoreKeyframe->second;

    pCoreKeyframe->destroy();
    delete pCoreKeyframe;
  }

  m_mapCoreKeyframe.clear();

  m_coreBoneId = -1;
}

 /*****************************************************************************/
/** Returns the ID of the core bone.
  *
  * This function returns the ID of the core bone to which the core track
  * instance is attached to.
  *
  * @return One of the following values:
  *         \li the \b ID of the core bone
  *         \li \b -1 if an error happend
  *****************************************************************************/

int CalCoreTrack::getCoreBoneId()
{
  return m_coreBoneId;
}

 /*****************************************************************************/
/** Returns the core keyframe map.
  *
  * This function returns the map that contains all core keyframes of the core
  * track instance.
  *
  * @return A reference to the core keyframe map.
  *****************************************************************************/

std::map<float, CalCoreKeyframe *>& CalCoreTrack::getMapCoreKeyframe()
{
  return m_mapCoreKeyframe;
}

 /*****************************************************************************/
/** Returns a specified state.
  *
  * This function returns the state (translation and rotation of the core bone)
  * for the specified time and duration.
  *
  * @param time The time in seconds at which the state should be returned.
  * @param duration The duration of the animation containing this core track
  *                 instance in seconds.
  * @param translation A reference to the translation reference that will be
  *                    filled with the specified state.
  * @param rotation A reference to the rotation reference that will be filled
  *                 with the specified state.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreTrack::getState(float time, float duration, CalVector& translation, CalQuaternion& rotation)
{
	if( time > duration) {
		int iiii=0;
	}
#pragma message (Error:!! Remove...)
std::map<float, CalCoreKeyframe *>::iterator it = m_mapCoreKeyframe.begin();
for (;it!=m_mapCoreKeyframe.end();it++)
{
	float tt = it->first;
	CalCoreKeyframe* frame = it->second;
	int kjhewrferf=0;
}


  std::map<float, CalCoreKeyframe *>::iterator iteratorCoreKeyframeBefore;
  std::map<float, CalCoreKeyframe *>::iterator iteratorCoreKeyframeAfter;

  // get the one core keyframe before and the one after the requested time
  bool bWrap;
  iteratorCoreKeyframeAfter = m_mapCoreKeyframe.upper_bound(time);

  // check if we have a wrap-around
  if(iteratorCoreKeyframeAfter == m_mapCoreKeyframe.end())
  {
	  /*std::map<float, CalCoreKeyframe *>::iterator	it;
	  for( it=m_mapCoreKeyframe.begin(); it!=m_mapCoreKeyframe.end(); ++it)
	  {
		  float time = it->first;
		  int size = m_mapCoreKeyframe.size();
	  }*/

	  // really wrap ???
	  if( time != duration)	{
		iteratorCoreKeyframeBefore = iteratorCoreKeyframeAfter;
		--iteratorCoreKeyframeBefore;
		iteratorCoreKeyframeAfter = m_mapCoreKeyframe.begin();
		bWrap = true;
	  }else{
		iteratorCoreKeyframeBefore = iteratorCoreKeyframeAfter;
		CalCoreKeyframe* pCoreKeyframeBefore = iteratorCoreKeyframeBefore->second;
		--iteratorCoreKeyframeBefore;
		pCoreKeyframeBefore = iteratorCoreKeyframeBefore->second;
		iteratorCoreKeyframeAfter = iteratorCoreKeyframeBefore;
		--iteratorCoreKeyframeBefore;
		pCoreKeyframeBefore = iteratorCoreKeyframeBefore->second;
		/*iteratorCoreKeyframeBefore = m_mapCoreKeyframe.begin();
		iteratorCoreKeyframeAfter = m_mapCoreKeyframe.begin();*/
		bWrap = false;
	  }
  }
  else
  {
    if(iteratorCoreKeyframeAfter == m_mapCoreKeyframe.begin())
    {
      iteratorCoreKeyframeBefore = m_mapCoreKeyframe.end();
    }
    else
    {
      iteratorCoreKeyframeBefore = iteratorCoreKeyframeAfter;
    }

    --iteratorCoreKeyframeBefore;

    bWrap = false;
  }

  // get the two keyframes
  CalCoreKeyframe *pCoreKeyframeBefore;
  pCoreKeyframeBefore = iteratorCoreKeyframeBefore->second;
  CalCoreKeyframe *pCoreKeyframeAfter;
  pCoreKeyframeAfter = iteratorCoreKeyframeAfter->second;

  // calculate the blending factor between the two keyframe states
  float blendFactor;
  if(bWrap)
  {
	  float divider = duration - pCoreKeyframeBefore->getTime();
	  if( divider == 0) blendFactor = 0.0; else
		blendFactor = (time - pCoreKeyframeBefore->getTime()) / (duration - pCoreKeyframeBefore->getTime());
  }
  else
  {
	  float divider = pCoreKeyframeAfter->getTime() - pCoreKeyframeBefore->getTime();
	  if( divider == 0) blendFactor = 0.0; else
		blendFactor = (time - pCoreKeyframeBefore->getTime()) / divider;
  }

  if( blendFactor > 1)
	  blendFactor = 1;
  // blend between the two keyframes
  translation = pCoreKeyframeBefore->getTranslation();
  translation.blend(blendFactor, pCoreKeyframeAfter->getTranslation());

  rotation = pCoreKeyframeBefore->getRotation();
  rotation.blend(blendFactor, pCoreKeyframeAfter->getRotation());

  //return !m_bClearTrans;
  return m_bClearTrans;
}

 /*****************************************************************************/
/** Sets the ID of the core bone.
  *
  * This function sets the ID of the core bone to which the core track instance
  * is attached to.
  *
  * @param coreBoneId The ID of the bone to which the core track instance should
  *                   be attached to.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreTrack::setCoreBoneId(int coreBoneId)
{
  /*if(coreBoneId < 0)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }*/

  m_coreBoneId = coreBoneId;

  return true;
}

bool CalCoreTrack::setCoreBoneName(LPSTR coreBoneName)
{
  //if(coreBoneName == "")
  //{
//    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    //return false;
  //}

  m_coreBoneName = Cal3DComString(coreBoneName);

  return true;
}

//****************************************************************************//

void CalCoreTrack::setDuration(float newDuration, float oldDuration)
{
  std::map<float, CalCoreKeyframe *> _m_mapCoreKeyframe;
  std::map<float, CalCoreKeyframe *>::iterator iteratorCoreKeyframeBefore;

  for(iteratorCoreKeyframeBefore = m_mapCoreKeyframe.begin(); iteratorCoreKeyframeBefore != m_mapCoreKeyframe.end(); iteratorCoreKeyframeBefore++){
	  CalCoreKeyframe *pCoreKeyframe;
	  pCoreKeyframe = iteratorCoreKeyframeBefore->second;
	  _m_mapCoreKeyframe.insert(std::make_pair(pCoreKeyframe->getTime(), pCoreKeyframe));
	  if( _m_mapCoreKeyframe.size() > 135){
		int iii=0;
		}
  }
  m_mapCoreKeyframe.clear();

  for(iteratorCoreKeyframeBefore = _m_mapCoreKeyframe.begin(); iteratorCoreKeyframeBefore != _m_mapCoreKeyframe.end(); iteratorCoreKeyframeBefore++){
	  CalCoreKeyframe *pCoreKeyframe;
	  pCoreKeyframe = iteratorCoreKeyframeBefore->second;
	  pCoreKeyframe->setTime( pCoreKeyframe->getTime()*(newDuration/oldDuration));

	  m_mapCoreKeyframe.insert(std::make_pair(pCoreKeyframe->getTime(), pCoreKeyframe));
	  if( m_mapCoreKeyframe.size() > 135){
		int iii=0;
		}
  }
}

void CalCoreTrack::initRelKeys(CalVector v, CalQuaternion q, bool bFirst, bool bLast, bool bReverseInitialization)
{
  std::map<float, CalCoreKeyframe *>::iterator iteratorCoreKeyframe;

  CalQuaternion	multRotation;
  CalQuaternion	keyRotation;

  if( !bReverseInitialization){
	  if( !bFirst)
		iteratorCoreKeyframe = m_mapCoreKeyframe.begin();
	  else{
		iteratorCoreKeyframe = m_mapCoreKeyframe.begin();
		++iteratorCoreKeyframe;
	  }
  }else{
		iteratorCoreKeyframe = m_mapCoreKeyframe.end();
		--iteratorCoreKeyframe;
  }

  CalCoreKeyframe *pKeyframe = iteratorCoreKeyframe->second;
  keyRotation = pKeyframe->getRotation();
  keyRotation.conjugate();

  keyRotation *= q;
  multRotation = keyRotation;
  multRotation.Normalize();

  for(iteratorCoreKeyframe = m_mapCoreKeyframe.begin(); iteratorCoreKeyframe != m_mapCoreKeyframe.end(); ++iteratorCoreKeyframe){
	  if( bFirst && iteratorCoreKeyframe == m_mapCoreKeyframe.begin())
		  continue;
	  std::map<float, CalCoreKeyframe *>::iterator titeratorCoreKeyframe;
	  titeratorCoreKeyframe = iteratorCoreKeyframe;
	  titeratorCoreKeyframe++;
	  if( bLast && titeratorCoreKeyframe == m_mapCoreKeyframe.end())
		  continue;
	  CalCoreKeyframe *pCoreKeyframe;
	  pCoreKeyframe = iteratorCoreKeyframe->second;
	  CalQuaternion	curKeyRtn = pCoreKeyframe->getRotation();
	  curKeyRtn *= multRotation;
	  curKeyRtn.Normalize();
	  pCoreKeyframe->setRotation( curKeyRtn);
  }

  /*iteratorCoreKeyframe = m_mapCoreKeyframe.end();
  --iteratorCoreKeyframe;

  CalCoreKeyframe *pCoreKeyframe;
  pCoreKeyframe = iteratorCoreKeyframe->second;

  CalCoreKeyframe *pCoreKeyframeToAdd;
  pCoreKeyframeToAdd = new CalCoreKeyframe();

  pCoreKeyframeToAdd->setRotation( q);
  pCoreKeyframeToAdd->setTime( pCoreKeyframe->getTime()+0.1);

  addCoreKeyframe( pCoreKeyframeToAdd);*/
}

void CalCoreTrack::copyData(CalCoreTrack *pTrack, float trackDur, CalCoreKeyframe* apFirstKeyframe, CalCoreKeyframe* apLastKeyframe, float a_dwKeepLast, bool bRootTrack, bool bReverse)
{
  setCoreBoneId( pTrack->getCoreBoneId());
  m_coreCommonBoneLength = pTrack->m_coreCommonBoneLength;

  float fFirstFrameTime = 0.0;
  float fLastFrameTime = 0.0;
  float fLastBlendTime = 0.0;

  CalQuaternion	multRotation(0, 0, 0, 1);
  CalQuaternion	keyRotation(0, 0, 0, 1);

  CalVector	addTranslation(0, 0, 0);
  CalVector	keyTranslation(0, 0, 0);

  if( apFirstKeyframe != NULL){
	  fFirstFrameTime = apFirstKeyframe->getTime();
	  apFirstKeyframe->setTime( 0.0);

	  // Если интерполяция идет из текущего полодения
	  //  то сделаем всю анимацию относительной
	  std::map<float, CalCoreKeyframe *>::iterator iteratorCoreKeyframe;
	  iteratorCoreKeyframe = pTrack->m_mapCoreKeyframe.begin();
	  CalCoreKeyframe *pKeyframe = iteratorCoreKeyframe->second;
	  keyRotation = pKeyframe->getRotation();
	  keyRotation.conjugate();
	  apFirstKeyframe->getRotation();
	  keyRotation *= apFirstKeyframe->getRotation();
	  multRotation = keyRotation;
	  multRotation.Normalize();

	  multRotation.x = 0;	// вообще-то нужно повернуть скелет вокруг оси Z до текущего вектора поворота

	  keyTranslation = pKeyframe->getTranslation();
	  keyTranslation -= apFirstKeyframe->getTranslation();
	  addTranslation = keyTranslation;

	  addTranslation.z = 0;

	  m_mapCoreKeyframe.insert(std::make_pair(apFirstKeyframe->getTime(), apFirstKeyframe));
  }

  std::map<float, CalCoreKeyframe *>::iterator iteratorCoreKeyframe;
  for(iteratorCoreKeyframe = pTrack->m_mapCoreKeyframe.begin(); iteratorCoreKeyframe != pTrack->m_mapCoreKeyframe.end(); ++iteratorCoreKeyframe){
	  CalCoreKeyframe	*pkf = new CalCoreKeyframe();
	  CalCoreKeyframe	*pkd = (*iteratorCoreKeyframe).second;
	  CalQuaternion		keyRotation(0, 0, 0, 1);
	  CalVector			keyTranslation(0, 0, 0);

	  fLastFrameTime  = pkd->getTime();
	  if( bReverse)	fLastFrameTime = trackDur - fLastFrameTime;
	  keyRotation = pkd->getRotation();
	  if( bRootTrack)
		keyRotation *= multRotation;
	  keyRotation.Normalize();
	  pkf->setRotation( keyRotation);
	  keyTranslation = pkd->getTranslation();
	  if( bRootTrack)
		keyTranslation -= addTranslation;
	  pkf->setTranslation( keyTranslation);
	  pkf->setTime( fLastFrameTime + fFirstFrameTime);

	  m_mapCoreKeyframe.insert(std::make_pair(pkf->getTime(), pkf));
  }

  if( apLastKeyframe != NULL){
	  if( a_dwKeepLast != 0){
		  if( !bReverse)
			--iteratorCoreKeyframe;
		  else
			  iteratorCoreKeyframe = pTrack->m_mapCoreKeyframe.begin();
		CalCoreKeyframe	*pkf = new CalCoreKeyframe();
		pkf->setTranslation( (*iteratorCoreKeyframe).second->getTranslation());
		pkf->setRotation( (*iteratorCoreKeyframe).second->getRotation());
		pkf->setTime( fLastFrameTime + fFirstFrameTime + a_dwKeepLast);
		m_mapCoreKeyframe.insert(std::make_pair(pkf->getTime(), pkf));
	  }
	  fLastBlendTime = apLastKeyframe->getTime();
	  apLastKeyframe->setTime( fLastFrameTime + fFirstFrameTime + fLastBlendTime + (float)a_dwKeepLast);
	  m_mapCoreKeyframe.insert(std::make_pair(apLastKeyframe->getTime(), apLastKeyframe));
  }/*else{
	  if( a_dwKeepLast != 0){
		--iteratorCoreKeyframe;
		CalCoreKeyframe	*pkf = new CalCoreKeyframe();
		pkf->setTranslation( (*iteratorCoreKeyframe).second->getTranslation());
		pkf->setRotation( (*iteratorCoreKeyframe).second->getRotation());
		pkf->setTime( fLastFrameTime + fFirstFrameTime + a_dwKeepLast);
		m_mapCoreKeyframe.insert(std::make_pair(pkf->getTime(), pkf));
	  }
  }*/
}

void CalCoreTrack::addKeyframe(CalVector tr, CalQuaternion rtn, float time)
{
	CalCoreKeyframe*	newkey = NULL;

	newkey = new CalCoreKeyframe();
	newkey->create();
	newkey->setTranslation( tr);
	newkey->setRotation( rtn);
	newkey->setTime( time);
	addCoreKeyframe( newkey);

	//m_bClearTrans = checkTrns();
}

bool CalCoreTrack::checkTrns()
{
  CalVector trn;

  std::map<float, CalCoreKeyframe *>::iterator iteratorCoreKeyframe;
  iteratorCoreKeyframe = m_mapCoreKeyframe.begin();

  trn = (*iteratorCoreKeyframe).second->getTranslation();
  for(iteratorCoreKeyframe = m_mapCoreKeyframe.begin(); iteratorCoreKeyframe != m_mapCoreKeyframe.end(); iteratorCoreKeyframe++){
	  CalVector	trn1 = (*iteratorCoreKeyframe).second->getTranslation();
	  if( memcmp((void*)&trn1, (void*)&trn, sizeof(trn)) != 0)
	  {
		  m_bClearTrans = false;
		  return false;
	  }
  }

  m_bClearTrans = true;
  return true;
}

CalCoreKeyframe* CalCoreTrack::getBegPointer()
{
	std::map<float, CalCoreKeyframe *>::iterator iteratorCoreKeyframe = getMapCoreKeyframe().begin();
	return iteratorCoreKeyframe->second;
}


void CalCoreTrack::slake(float dur)
{
	if( m_state == STATE_OUT)	return;

	m_state = STATE_OUT;
	m_outtime = dur;
	m_time = 0;
}

void CalCoreTrack::grow(float dur)
{
	if( m_state == STATE_IN)	return;

	m_state = STATE_IN;
	m_outtime = dur;
	m_time = 0;
}

bool CalCoreTrack::update(float deltaTime)
{
	if( m_state == STATE_STEADY)	return true;

	if( m_state == STATE_OUT)
	{
		m_time	+= deltaTime;
		if( m_time >= m_outtime)
		{
			if( m_weight == 0.0 )
				m_state = STATE_STEADY;
			else
				m_weight = 0.0;
			//m_state = STATE_STEADY;
		}else
			m_weight = 1.0f - m_time/m_outtime;
	}

	if( m_state == STATE_IN)
	{
		m_time	+= deltaTime;
		if( m_time >= m_outtime)
		{
			if( m_weight == 1.0)
				m_state = STATE_STEADY;
			else
				m_weight = 1.0;
			
		}else
			m_weight = m_time/m_outtime;
	}
	return true;
}

void CalCoreTrack::fitPosition(CalVector trn, CalQuaternion rtn)
{
	std::map<float, CalCoreKeyframe *>::iterator iteratorCoreKeyframe;

	CalVector	frameTranslation;
	frameTranslation = m_mapCoreKeyframe.begin()->second->getTranslation();

	for(iteratorCoreKeyframe = m_mapCoreKeyframe.begin(); iteratorCoreKeyframe != m_mapCoreKeyframe.end(); iteratorCoreKeyframe++){
		CalCoreKeyframe*	pkey = (*iteratorCoreKeyframe).second;
		CalVector		keytrn = pkey->getTranslation();
		CalQuaternion	keyrtn = pkey->getRotation();

		CalVector temp = keytrn - frameTranslation;
		temp *= rtn;
		keytrn = frameTranslation + temp;

CalVector vtemp(1,0,0);
vtemp *= keyrtn;

		keytrn += trn;
		CalQuaternion	qrtn = rtn;
		qrtn *= keyrtn;
		qrtn.Normalize();

vtemp.set(0,-1,0);
vtemp *= qrtn;

		pkey->setTranslation( keytrn);
		pkey->setRotation( qrtn);

vtemp.set(0,-1,0);
vtemp *= rtn;
	}
}

void CalCoreTrack::decreaseAngle(float dec, void* data)
{
	std::vector<CalCoreBone *>* m_vectorCoreBone = (std::vector<CalCoreBone *>*)data;

	std::map<float, CalCoreKeyframe *>::iterator iteratorCoreKeyframe;

	for(iteratorCoreKeyframe = m_mapCoreKeyframe.begin(); iteratorCoreKeyframe != m_mapCoreKeyframe.end(); iteratorCoreKeyframe++){
		CalCoreKeyframe*	pkey = (*iteratorCoreKeyframe).second;
		CalQuaternion	keyrtn = pkey->getRotation();
		CalQuaternion	corertn = (*m_vectorCoreBone)[getCoreBoneId()]->getRotation();

		corertn.blend( dec, keyrtn);
		pkey->setRotation( corertn);
	}
}

void CalCoreTrack::setTranslationAllKeys(CalVector trn)
{
	std::map<float, CalCoreKeyframe *>::iterator iteratorCoreKeyframe;
	if( m_bClearTrans)
	{
		for(iteratorCoreKeyframe = m_mapCoreKeyframe.begin(); iteratorCoreKeyframe != m_mapCoreKeyframe.end(); iteratorCoreKeyframe++)
			iteratorCoreKeyframe->second->setTranslation( trn);
	}
}

float CalCoreTrack::getFloor()
{
	float floor = FLT_MAX;
	std::map<float, CalCoreKeyframe *>::iterator iteratorCoreKeyframe;

	for(iteratorCoreKeyframe = m_mapCoreKeyframe.begin(); iteratorCoreKeyframe != m_mapCoreKeyframe.end(); iteratorCoreKeyframe++)
	{
		CalCoreKeyframe*	pkey = (*iteratorCoreKeyframe).second;
		floor = (pkey->getTranslation().z)<floor?pkey->getTranslation().z:floor;
	}
	return floor;
}

void CalCoreTrack::setKeysTranslation(CalVector tr)
{
	if( m_mapCoreKeyframe.size() == 0)	return;
	float	boneLength = tr.length(), animboneLength = getAverageTranslationLength();
	if( animboneLength != 0)
		boneLength /= animboneLength;
	else
		boneLength = 0.0f;
	adjustKeysTranslation(boneLength);
}

void CalCoreTrack::adjustKeysTranslation(float k)
{
	if( k == 1.0f)	return;
	std::map<float, CalCoreKeyframe *>::iterator iteratorCoreKeyframe;
	for(iteratorCoreKeyframe = m_mapCoreKeyframe.begin(); iteratorCoreKeyframe != m_mapCoreKeyframe.end(); iteratorCoreKeyframe++)
	{
		CalCoreKeyframe*	pkey = (*iteratorCoreKeyframe).second;
		CalVector	keyTranslation = pkey->getTranslation();
		keyTranslation *= k;
		pkey->setTranslation(keyTranslation);
	}
}

float CalCoreTrack::getAverageTranslationLength()
{
	float	trn = 0.0f;
	std::map<float, CalCoreKeyframe *>::iterator iteratorCoreKeyframe;
	for(iteratorCoreKeyframe = m_mapCoreKeyframe.begin(); iteratorCoreKeyframe != m_mapCoreKeyframe.end(); iteratorCoreKeyframe++)
	{
		CalCoreKeyframe*	pkey = (*iteratorCoreKeyframe).second;
		CalVector	keyTranslation = pkey->getTranslation();
		trn += keyTranslation.length();
	}
	trn /= m_mapCoreKeyframe.size();
	return trn;
}
