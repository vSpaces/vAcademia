//****************************************************************************//
// bone.h                                                                     //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_BONE_H
#define CAL_BONE_H

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "global.h"
#include "vector.h"
#include "quaternion.h"

//****************************************************************************//
// Forward declarations                                                       //
//****************************************************************************//

class CalCoreBone;
class CalSkeleton;

//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//

 /*****************************************************************************/
/** The bone class.
  *****************************************************************************/

class CAL3D_API CalBone
{
	friend class CalSkeleton;
// member variables
protected:
  CalCoreBone *m_pCoreBone;
  CalSkeleton *m_pSkeleton;
  float m_accumulatedWeight;
  float m_accumulatedWeightAbsolute;
  CalVector m_translation;
  CalQuaternion m_rotation;
  CalVector m_translationAbsolute;
  CalQuaternion m_rotationAbsolute;
  CalVector m_translationBoneSpace;
  CalQuaternion m_rotationBoneSpace;

  CalVector m_savetranslation;
  CalQuaternion m_saverotation;

// constructors/destructor
public:
	CalBone();
	virtual ~CalBone();
	
// member functions
public:
	void saveState();
	void setUpState();
  void blendState(float weight, const CalVector& translation, const CalQuaternion& rotation);
  //void calculateState(CalVector vRotation=CalVector(0, 0, 0), float angle = 0, CalVector vTranslation=CalVector(0, 0, 0));
//  void calculateState(CalQuaternion qRotation=CalQuaternion(CalVector(0, 0, 0), 0), CalVector vTranslation=CalVector(0, 0, 0));
  void calculateState();
  void clearState();
  bool create(CalCoreBone *pCoreBone);
  void destroy();
  CalCoreBone *getCoreBone();
  const CalQuaternion& getRotation();
  const CalQuaternion& getRotationAbsolute();
  const CalQuaternion& getRotationBoneSpace();
  const CalVector& getTranslation();
  const CalVector& getTranslationAbsolute();
  const CalVector& getTranslationBoneSpace();
  void lockState();
  void pushState();
  void popState();
  void setSkeleton(CalSkeleton *pSkeleton);
  void setRotation(CalQuaternion qrtn);
  void setTranslation(CalVector atrn);
  int getParentId();
};

#endif

//****************************************************************************//
