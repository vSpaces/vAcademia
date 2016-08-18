//****************************************************************************//
// mixer.h                                                                    //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_MIXER_H
#define CAL_MIXER_H

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//
#include "global.h"
#include "vector.h"
#include "mpeg4fdp.h"

//****************************************************************************//
// Forward declarations                                                       //
//****************************************************************************//

class CalModel;
class CalAnimation;
class CalAnimationAction;
class CalAnimationCycle;
class CalAnimationMorph;
class CalAnimSequencer;
class CalBone;
class StepCtrl;

#include "AnimSequencer.h"

//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//
typedef std::pair<LPVOID, LPVOID> VOIDPAIR;
 /*****************************************************************************/
/** The mixer class.
  *****************************************************************************/

class CAL3D_API CalMixer
{
// member variables
public:
  CalModel *m_pModel;
  std::vector<CalAnimation *>		m_vectorAnimation;
  std::list<CalAnimationAction *>	m_listAnimationAction;
  std::list<CalAnimationMorph *>	m_listAnimationMorph;
  std::list<CalAnimationCycle *>	m_listAnimationCycle;
  std::list<CalAnimationCycle *>	m_listStepCycle;
  std::list<FACEExpression *>		m_listFaceExpression;
  std::vector<CalAnimSequencer *>	m_vectorSequencers;
  // Движение на установку модели
  CalAnimationAction*				pSetAnimationAction;
  float								dwSetDuration;
  float								dwSetAccTime;
  float								dwLastSetAccTime;

  float m_animationTime;
  float m_animationDuration;

// constructors/destructor
public:
  CalMixer();
	virtual ~CalMixer();

// member functions	
public:
	float getDuration(int animID);
	bool resumeSequencer(int sId);
	bool stopSequencer(int sId);
	bool playSequencer(int sId);
	int createSequencer(int sid, int mid, int eid, pcbfSeqAccomplished pf=NULL, LPVOID pdata=NULL);
	void set_face_expression(Cal3DComString pack1, int id1, Cal3DComString pack2, int id2, float weight);
	void set_face_expression(FACEExpression* e1, FACEExpression* e2, float weight);
	void remove_face_expressions();
	void removeEmotion(FACEExpression* fe, float dur=1.0);
	void executeEmotion(FACEExpression* fe, float dur=1.0, float dweight=1.0);
	void checkBonesGrow(CalAnimation* panim);
	void coverCurrent(CalAnimationAction* action);
	CalAnimation* getAnimation(int id, int type);
	void updateLookCtrl();
	bool containSameBones(int id1, int id2, CalCoreSkeleton*	skeleton=NULL);
	void updateMorph();
	float getAnimTime(int id);
	float getAnimActionRelTime( DWORD id);
	virtual void CheckAnimPlace(CalAnimation* pAnim, bool inanimPlace=false);
	void clearAction(int id, float fadeOut=0.0);
	void clearActionFadeOnLastFrame(int id, float fadeOut=0.0);
	bool setAction(int id, DWORD dur, bool bPose = false, bool inPlace = false);
	void setAnimDuration(int id, float duration);
	void setDuration(int animID, float duration);
	bool executeStep(int, float, float);
	bool blendCycle(int id, float weight, float delay, bool asynch=false);
	bool clearCycle(int id, float delay);
	bool create(CalModel *pModel);
	void destroy();
	bool executeAction(int id, float delayIn, float delayOut, CalModel* psmodel=NULL, DWORD dwsid=0, bool am_bReverse = false);
	bool morph(int id, bool ab_Reverse=false);
	bool updateAnimation(float deltaTime);
	void updateSkeleton();
	void attachTo(CalBone* parentBone, CalBone* childBone);
	DWORD findAnimationById(int id, CalAnimation** ppAnimation);

	std::vector<DWORD >		vAfterMotions;	// id1, id2.....
	std::vector<std::vector<VOIDPAIR>*>	bonesTracks;

protected:
	// attach
	CalBone*					attach_parent;
	CalVector					attach_translation;
	CalQuaternion				attach_rotation;
};

#endif

//****************************************************************************//
