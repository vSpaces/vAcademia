//****************************************************************************//
// frameBone.cpp                                                              //
//****************************************************************************//
#include "StdAfx.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "error.h"
#include "framebone.h"
#include "memory_leak.h"

CalFrameBone::CalFrameBone()
{
  // ??
}

void CalFrameBone::blendState(float weight, const CalVector& translation, const CalQuaternion& rotation)
{
	weight = 1.0f;
  CalVector		tr = getTranslationAbsolute();
  CalQuaternion	rt = getRotationAbsolute();

  m_translationAbsolute.blend( weight, translation);
  m_rotationAbsolute.blend( weight, rotation);
  m_accumulatedWeightAbsolute = weight;
}

//****************************************************************************//
