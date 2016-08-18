//****************************************************************************//
// coretrack.h                                                                //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_CORETRACK_H
#define CAL_CORETRACK_H

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
class CalCoreKeyframe;

//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//

 /*****************************************************************************/
/** The core track class.
  *****************************************************************************/

class CAL3D_API CalCoreTrack
{
// misc
public:
  static const unsigned int STATE_IN;
  static const unsigned int STATE_STEADY;
  static const unsigned int STATE_OUT;

// member variables
protected:
  int m_coreBoneId;
  Cal3DComString m_coreBoneName;
  bool m_bClearTrans;
  std::map<float, CalCoreKeyframe *> m_mapCoreKeyframe;

// constructors/destructor
public:
  CalCoreTrack();
	virtual ~CalCoreTrack();

// member functions	
public:
	float m_coreCommonBoneLength;
	float getAverageTranslationLength();
	void adjustKeysTranslation(float k);
	void setKeysTranslation(CalVector tr);
	float getFloor();
	void setTranslationAllKeys(CalVector trn);
	float getWeight(){ return m_weight;}
	bool slaking(){ return m_state == STATE_OUT;}
	bool growing(){ return m_state == STATE_IN;}
	bool update(float deltaTime);
	void slake(float dur);
	void grow(float dur);
	CalCoreKeyframe* getBegPointer();
	bool checkTrns();
	void fitPosition(CalVector trn, CalQuaternion rtn);
	void decreaseAngle(float dec, void* data);
	void addKeyframe(CalVector tr, CalQuaternion rtn, float time);
	void copyData(CalCoreTrack* pTrack, float trackDur, CalCoreKeyframe* apFirstKeyframe=NULL, CalCoreKeyframe* apLastKeyframe=NULL, float a_dwKeepLast=0, bool bRootTrack=false, bool bReverse=false);
	void initRelKeys(CalVector v, CalQuaternion q, bool bFirst, bool bLast, bool bReverseInitialization=false);
	void setDuration(float newDuration, float oldDuration);
	bool addCoreKeyframe(CalCoreKeyframe *pCoreKeyframe);
	bool create();
	void destroy();
	int getCoreBoneId();
	std::map<float, CalCoreKeyframe *>& getMapCoreKeyframe();
	bool getState(float time, float duration, CalVector& translation, CalQuaternion& rotation);
	bool setCoreBoneId(int coreBoneId);
	bool setCoreBoneName(LPSTR coreBoneName);
	Cal3DComString getCoreBoneName(){ return m_coreBoneName;}
  
protected:
	unsigned int	m_state;
	float			m_weight;
	float			m_outtime;
	float			m_time;
};

#endif

//****************************************************************************//
