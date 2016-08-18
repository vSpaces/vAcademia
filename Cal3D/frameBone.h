//****************************************************************************//
// frameBone.h                                                                //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //

#ifndef CAL_FRAME_BONE_H
#define CAL_FRAME_BONE_H


#include "global.h"
#include "vector.h"
#include "quaternion.h"
#include "bone.h"
#include "corebone.h"

class CAL3D_API CalFrameBone : public CalBone
{
public:
  CalFrameBone();
  ~CalFrameBone() { }

  void blendState(float weight, const CalVector& translation, const CalQuaternion& rotation);
};

#endif

//****************************************************************************//
