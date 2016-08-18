//****************************************************************************//
// animation.h                                                                //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_ANIMATION_H
#define CAL_ANIMATION_H

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "global.h"
#include "vector.h"
#include "quaternion.h"
#include "coreanimation.h"
//****************************************************************************//
// Forward declarations                                                       //
//****************************************************************************//

class CalCoreAnimation;
class CalCoreTrack;
class CalModel;
class CalSkeleton;
class CalCoreSkeleton;
class CalAnimSequencer;

//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//

 /*****************************************************************************/
/** The animation class.
  *****************************************************************************/

class CAL3D_API CalAnimation
{
// misc
public:
  static const unsigned int TYPE_NONE;
  static const unsigned int TYPE_CYCLE;
  static const unsigned int TYPE_POSE;
  static const unsigned int TYPE_ACTION;

  static const unsigned int STATE_NONE;
  static const unsigned int STATE_SYNC;
  static const unsigned int STATE_ASYNC;
  static const unsigned int STATE_IN;
  static const unsigned int STATE_STEADY;
  static const unsigned int STATE_OUT;

// member variables
protected:
  CalCoreAnimation *m_pCoreAnimation;
  float m_duration;
  unsigned int m_type;
  unsigned int m_state;
  float m_time;
  float m_timeFactor;
  float m_weight;
  CalAnimSequencer*	m_pSequencer;

// constructors/destructor
protected:
  CalAnimation();
  virtual ~CalAnimation();

// member functions	
public:
	void setSequencer(CalAnimSequencer* pseq);
	void FallOnSkel(CalSkeleton* pSkel);
	void FallOnSkel(CalCoreSkeleton *pCoreSkel);
	bool b_was_sinchronized;
	int coreAnimID;
	void setCoreID( int id);
	float getFloor(CalSkeleton *pSkel);
	virtual void clearOnNextStep(){};
	int getNotify(float time);
	void addNotification(float time, int id);
	void fitPosition(CalVector trn, CalQuaternion rtn);
	void translateBip(CalVector trn);
	void decreaseAngle(float dec, void* data);
	void slakeTrack(int id, float dur);
	bool bPose;
	bool bLocked;
	void getState(int id, float time, CalVector& translation, CalQuaternion& rotation);
	DWORD getKFNUM();
	float getDuration();
	void copyCoreAnimation(CalSkeleton *pSkel);
	float getMaxDiffers(CalSkeleton* pSkel, bool bFirst=true);
	void initStates(CalSkeleton* apSkeleton);
	void fadeOnLastFrame(CalSkeleton *apSkeleton, float	fadeTime);
	bool rootActive();
	// Relative motions
	bool bSelfMaster;
	std::list<CalCoreTrack *> m_listSelfCoreTrack;
	///////////////////
	void initRelAnimation(CalSkeleton* pSkel, std::vector<int>* ap_vecBonesIds=NULL);
	bool m_bReverse;
	void setSynch(CalModel* pmodel, DWORD dwid);
	std::list<CalCoreTrack *>& getListCoreTrack();
	void seek(float seekTime);
	bool			m_bRootMoving;
	bool			m_bRootActive;
	CalVector		m_vRootTranslation;
	CalQuaternion	m_vRootRotation;
	bool rootMoving();
	bool getBoneStartState(int RootID, CalVector& translation, CalQuaternion &rotation, float time = -1.0);
	bool containBoneFrom(CalAnimation* pCheckAnim, std::list<TRACKPAIR>*	tracksout = NULL);
	void setDuration(float duration);
	void setFlags(DWORD val){	dwFlags = val;}
	void setFlag(DWORD val){	dwFlags |= val;}
	void removeFlag(DWORD val){	dwFlags &= ~val;}
	DWORD getFlags(){	return dwFlags;}
	inline	bool isDone(){	return bDone;}
	void	Done();
	inline	DWORD version(){ return m_pCoreAnimation->m_version;}
	
	float getPhase();
	virtual bool create(CalCoreAnimation *pCoreAnimation) = 0;
	virtual void destroy();
	CalCoreAnimation *getCoreAnimation();
	unsigned int getState();
	float getTime();
	unsigned int getType();
	float getWeight();

	// Синхронизация
	CalModel*		m_pSynchModel;
	DWORD			dw_SynchID;
	CalVector		m_vLock;

	// map of TIMES and apropriate NOTIFICATIONS
	std::map<float, int>	mapNotifications;
	float					lastntf;
	DWORD				dwFlags;
	float				m_fadeOutTime;

protected:
	bool bDone;
};

#endif

//****************************************************************************//
