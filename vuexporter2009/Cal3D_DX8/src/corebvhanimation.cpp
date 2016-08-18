//****************************************************************************//
// CoreBvhAnimation.cpp                                                          //
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

#include "CoreBvhAnimation.h"
#include "coretrack.h"
#include "skeleton.h"
#include "coreskeleton.h"
#include "corekeyframe.h"
#include "corebone.h"
#include "bone.h"
#include "CoreAnimationflags.h"

 /*****************************************************************************/
/** Constructs the core animation instance.
  *
  * This function is the default constructor of the core animation instance.
  *****************************************************************************/

CalCoreBvhAnimation::CalCoreBvhAnimation()
{
	m_bInitialized = false;
	p_InitedSkeleton = NULL;
	m_bvhBipUpPosition = 0.0f;
}

 /*****************************************************************************/
/** Destructs the core animation instance.
  *
  * This function is the destructor of the core animation instance.
  *****************************************************************************/

CalCoreBvhAnimation::~CalCoreBvhAnimation()
{
}

void	CalCoreBvhAnimation::setBipUpPosition( float position)
{
	m_bvhBipUpPosition = position;
}

void CalCoreBvhAnimation::initialize(CalSkeleton* skeleton)
{
	assert( skeleton);
	if( m_bInitialized)
	{
		assert( p_InitedSkeleton == skeleton);
		return;
	}
	p_InitedSkeleton = skeleton;
	// *************************************************************************
	skeleton->saveState();
	int framesCount = 0;

	// определим отношение длины ног персонажа, для правильной анимации
	std::list<CalCoreTrack *> tracks = this->getListCoreTrack();
	std::list<CalCoreTrack *>::iterator iter = tracks.begin();

	float	heightRelation = 1.0f;
	CalBone*	pBipBone = skeleton->getBone(Cal3DComString("bip"));
	if( pBipBone)
	{
		CalCoreBone*	pCoreBipBone = pBipBone->getCoreBone();
		if( pCoreBipBone->getTranslation().z != 0)
		{
			heightRelation = m_bvhBipUpPosition / pCoreBipBone->getTranslation().z;
		}
	}

	//	Надо повернуть скелет на 90 градусов по X
	CalBone*	bone = skeleton->getBone(0);
	CalQuaternion	startBipRotation = bone->getRotation();
	// correctStartBipRotation - лицо обращено вверх по оси Z, голова по направлению оси Y - как в BVH
	CalQuaternion	correctStartBipRotation(0.5f, 0.5f, 0.5f, 0.5f);
	CalQuaternion	deltaBipRotation = startBipRotation;	deltaBipRotation.conjugate();
					deltaBipRotation = deltaBipRotation * correctStartBipRotation;

	bone->setRotation( correctStartBipRotation);
	skeleton->calculateState();
	skeleton->relaxToCoreState();

	// 1. Рассчитаем абсолютные значения поворотов костей
	iter = tracks.begin();
	for (iter; iter != tracks.end(); iter++)
	{
		std::map<float, CalCoreKeyframe*>& keyFrames = (*iter)->getMapCoreKeyframe();

		Cal3DComString	boneName = (*iter)->getCoreBoneName();
		CalBone* pBone = skeleton->getBone(boneName);
		if (pBone)
		{
			int boneId = skeleton->getBoneId( pBone);
			if( boneId == -1)
				int ii=0;
			(*iter)->setCoreBoneId( boneId);
			int parentId = pBone->getParentId();
			if (parentId == -1)
			{
				CalQuaternion	rootRotation = pBone->getRotation();
				std::map<float, CalCoreKeyframe*>::iterator iter = keyFrames.begin();
				for (iter; iter != keyFrames.end(); iter++)
				{
					CalQuaternion bvhRotation = iter->second->getRotation();
					bvhRotation = rootRotation;
					iter->second->setRotation( bvhRotation);
					CalVector	bipTranslation = iter->second->getTranslation();
					if( heightRelation != 0)	bipTranslation.z /= heightRelation;
					iter->second->setTranslation( bipTranslation);
					
				}
				continue;
			}

//CalVector boneDirection(1, 0, 0);
//CalBone* pBone1 = skeleton->getBone(Cal3DComString("spine"));
//boneDirection *= pBone1->getRotationAbsolute();

			std::vector<CalBone *> bones = skeleton->getVectorBone();
			CalBone* pParentBone = bones[parentId];

			CalQuaternion parentQ = pParentBone->getRotationAbsolute();
			parentQ.conjugate();

			std::map<float, CalCoreKeyframe*>::iterator iter2 = keyFrames.begin();
			framesCount = max( framesCount, keyFrames.size());
			int frameIndex = 0;
			for (iter2; iter2 != keyFrames.end(); iter2++, frameIndex++)
			{
				CalVector boneDirection(1, 0, 0);
				boneDirection *= pBone->getRotationAbsolute();
				if( boneName == "l foot" || boneName == "r foot")
				{
					boneDirection.set(0, 0, 1);
				}
				CalVector bvhDirection = iter2->second->getTranslation();
				CalQuaternion bvhRotation = iter2->second->getRotation();

				CalQuaternion absBoneRotation = pBone->getRotationAbsolute();

				if (!((bvhDirection.x == 0.0) && (bvhDirection.y == 0.0) && (bvhDirection.z == 0.0)))
				{
					CalQuaternion q(boneDirection.normalized(), bvhDirection.normalized());
					absBoneRotation *= q;
					q = absBoneRotation;
					CalVector temp(1,0,0);
					temp *= q;

					CalQuaternion	qAnim = q;	qAnim *= bvhRotation;
					iter2->second->setRotation(qAnim);
					if( iter2 == keyFrames.begin())
					{
						CalQuaternion	qSkel = q;	qSkel *= parentQ;
						pBone->setRotation(qSkel);
						pBone->calculateState();
					}
				}
				else
				{
					iter2->second->setRotation(pBone->getRotation());
				}

				iter2->second->setTranslation(pBone->getTranslation());
			}
		}
	}
	// 2. Переведем координаты в локальные координаты родителя
	std::vector<CalBone *> bones = skeleton->getVectorBone();
	//framesCount = 1;
	for( int iframe=0; iframe<framesCount; iframe++)
	{
		iter = tracks.begin();
		for (iter; iter != tracks.end(); iter++)
		{
			Cal3DComString	boneName = (*iter)->getCoreBoneName();
			CalBone* pBone = skeleton->getBone(boneName);
			if (!pBone || pBone->getParentId() == -1)	continue;

			CalBone* pParentBone = bones[pBone->getParentId()];
			CalQuaternion parentQ = pParentBone->getRotationAbsolute();	parentQ.conjugate();

			std::map<float, CalCoreKeyframe*>& keyFrames = (*iter)->getMapCoreKeyframe();
			std::map<float, CalCoreKeyframe*>::iterator iter2 = keyFrames.begin();
			for (int ikey=0; ikey < iframe; ikey++)
			{
				iter2++;
			}

			CalQuaternion	keyRotation = iter2->second->getRotation();
			keyRotation *= parentQ;
			iter2->second->setRotation( keyRotation);
			pBone->setRotation( keyRotation);
			pBone->calculateState();
		}
	}

	// повернем бип обратно
	// startBipRotation = correctStartBipRotation * (conj deltaBipRotation)
	deltaBipRotation.conjugate();

	CalQuaternion	xbackRotation(CalVector(1,0,0), -1.5707963267948966192313216916398);
	CalQuaternion	z90Rotation(CalVector(0,0,1), -1.5707963267948966192313216916398);

	iter = tracks.begin();
	for (iter; iter != tracks.end(); iter++)
	{
		CalBone* pBone = skeleton->getBone((*iter)->getCoreBoneName());
		if (pBone && pBone->getParentId()==-1)
		{
			std::map<float, CalCoreKeyframe*> keyFrames = (*iter)->getMapCoreKeyframe();
			std::map<float, CalCoreKeyframe*>::iterator itKey = keyFrames.begin();
			for ( ; itKey != keyFrames.end(); itKey++)
			{
				//itKey->second->setRotation( itKey->second->getRotation() * deltaBipRotation);
				CalQuaternion	frameRotation = xbackRotation * itKey->second->getRotation();
				frameRotation *= z90Rotation;
				itKey->second->setRotation(  frameRotation);
				CalVector	v1(1,0,0);
				v1 *= itKey->second->getRotation();
			}
			pBone->setRotation( startBipRotation);
			break;
		}
	}
	skeleton->restoreState();

	m_bInitialized = true;
}