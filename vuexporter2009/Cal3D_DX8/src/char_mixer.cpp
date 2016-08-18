//****************************************************************************//
// CharMixer.cpp                                                                  //
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

#include "error.h"
#include "char_mixer.h"
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


CalCharMixer::CalCharMixer()
{
}

CalCharMixer::~CalCharMixer()
{
}

/************************************************************************/
/* inanimPlace - установить модель на позицию, вз€тую из анимации
/************************************************************************/
void CalCharMixer::CheckAnimPlace(CalAnimation* pAnim, bool inanimPlace)
{
	int			rootID = m_pModel->getSkeleton()->getRootID();
	CalVector		translation, externPosition;
	CalQuaternion	rotation, externRotation;

	if( m_pModel->m_pfExternTransform)
	{
		m_pModel->m_pfExternTransform(externPosition, externRotation, m_pModel->m_pExternClass);
		m_pModel->getSkeleton()->set_frame_move(externPosition, externRotation);
	}
	
	if( (m_pModel->WasMoved() || inanimPlace) && 
		pAnim->getBoneStartState(Cal::PROJECTION_BONE_ID, translation, rotation))
	{
		CalVector	pos(0,0,0);
		CalQuaternion quat(0,0,0,1);

		if( !inanimPlace)	// оставить модель на месте. ѕереносим анимацию в текущее положение
		{
			if( m_pModel->m_pfExternTransform)
			{
				// ѕеренос новой анимации в новое положение
				externPosition = m_pModel->getSkeleton()->get_frame()->getTranslationAbsolute();
				moveAnimationToPosition(pAnim, externPosition, externRotation);
				//
				// ѕеренос текущих анимации в новое положение
				std::list<CalAnimationAction *>::iterator iteratorAnimationAction;
				for(iteratorAnimationAction = m_listAnimationAction.begin(); iteratorAnimationAction != m_listAnimationAction.end(); ++iteratorAnimationAction)
				{
					// get the core animation instance
					CalAnimation *pAnimation = (*iteratorAnimationAction);
					if( pAnimation->getBoneStartState(Cal::PROJECTION_BONE_ID, translation, rotation))
					{
						moveAnimationToPosition(pAnimation, externPosition, externRotation);
					}
				}
				// ѕеренос текущих циклов в новое положение
				std::list<CalAnimationCycle *>::iterator iteratorAnimationCycle;
				for(iteratorAnimationCycle = m_listAnimationCycle.begin(); iteratorAnimationCycle != m_listAnimationCycle.end(); ++iteratorAnimationCycle)
				{
					// get the core animation instance
					CalAnimation *pAnimation = (*iteratorAnimationCycle);
					if( pAnimation->getBoneStartState(Cal::PROJECTION_BONE_ID, translation, rotation))
					{
						moveAnimationToPosition(pAnimation, externPosition, externRotation);
					}
				}
			}
		}
		else
		{	
			// переместить модель на новое место.
			m_pModel->relative_pos_changed = false;
			pAnim->m_vRootRotation = CalQuaternion(0,0,0,1);
		}
	}

	/*
	if( !pAnim->bLocked && !pAnim->getCoreAnimation()->m_strLock.IsEmpty())
	{
		  pAnim->m_vLock = m_pModel->getSkeleton()->getBone(
								m_pModel->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName( 
									pAnim->getCoreAnimation()->m_strLock
								))->getTranslationAbsolute();
		  pAnim->m_vLock = m_pModel->transformVector( pAnim->m_vLock);
		  pAnim->bLocked = true;
	}
	*/
}

void CalCharMixer::moveAnimationToPosition(CalAnimation* pAnim, const CalVector& externPosition, const CalQuaternion& externRotation)
{
	CalVector translation;
	CalQuaternion rotation;
	pAnim->getBoneStartState(Cal::PROJECTION_BONE_ID, translation, rotation);
	int		rootID = m_pModel->getSkeleton()->getRootID();

	// ѕеренос
	pAnim->m_vRootTranslation = externPosition - translation;
	//
	if( pAnim->version() < 1008)
	{
		assert(false);
		CalVector		bipTrn;
		CalQuaternion	bipRtn;
		float bh = m_pModel->get_bip_height();
		if(pAnim->getBoneStartState(rootID, bipTrn, bipRtn))
		{
			CalVector	bipAddon(0, 0, bh-bipTrn.z);
			pAnim->translateBip(bipAddon);
		}
	}

	// ѕоворот
	CalVector xmodel(1,0,0);
	CalVector xanim(1,0,0);
	xmodel *= externRotation;
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
	/*CalVector vtemp;
	vtemp.set(1,0,0);
	vtemp *= quat;
	vtemp.set(1,0,0);
	vtemp *= pAnim->m_vRootRotation;*/
	pAnim->fitPosition( pAnim->m_vRootTranslation, pAnim->m_vRootRotation);
}