//****************************************************************************//
// skeleton.h                                                                 //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_SKELETON_H
#define CAL_SKELETON_H

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "global.h"
#include "vector.h"
#include "quaternion.h"

//****************************************************************************//
// Forward declarations                                                       //
//****************************************************************************//

class CalCoreSkeleton;
class CalBone;

//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//

 /*****************************************************************************/
/** The skeleton class.
  *****************************************************************************/

class CAL3D_API CalSkeleton
{

// member variables
protected:
  CalCoreSkeleton *m_pCoreSkeleton;
  std::vector<CalBone *> m_vectorBone;
  CalBone *	m_pFrameBone;

// constructors/destructor
public:
	CalSkeleton();
	virtual ~CalSkeleton();
	
// member functions
public:
	bool set_frame_move(CalVector& trn, CalQuaternion& rtn);
	bool get_frame_move(CalVector& trn, CalQuaternion& rtn, bool abRelativeToPrevious = false);
	float getFloor();
	//CalQuaternion qrtn;
	void checkBipVertical();
	void moveBone(DWORD id, CalVector trn, CalQuaternion rtn, bool bUpdateChilds = false);
	void shiftBone(int id, CalVector delta);
	CalQuaternion getBoneRotation(int id, bool abAbs=false);
	CalVector getDirection( int boneID, CalVector axes=CalVector(0, 1, 0));
	CalQuaternion getRootRotation();
	int getRootID();
	CalVector getRootDir();
	void setUp();			// установить скелет в начальное положение
	bool haveOwner();
	CalVector getRootPoint( bool abAbsolute=false);
	bool			m_bFrameMoved;
	float	fAngle;
	void relaxToCoreState();
	void calculateState();
	void clearState();
	bool create(CalCoreSkeleton *pCoreSkeleton);
	void destroy();
	inline CalBone *getBone(int boneId)
	{
		if( boneId == Cal::PROJECTION_BONE_ID){	m_bFrameMoved=m_pFrameBone?true:false;	return m_pFrameBone;}
		else  if( boneId < m_vectorBone.size())	return m_vectorBone[boneId];
		else	return NULL;
	}

  int	getBoneId(CalBone *apBone);
  inline CalBone* get_frame(){ return m_pFrameBone;}
  
  CalBone *getBone(LPCTSTR	name);
  CalCoreSkeleton *getCoreSkeleton();
  std::vector<CalBone *>& getVectorBone();
  void lockState();
  void saveState();
  void restoreState();

  int getBonePoints(float *pPoints); // DEBUG-CODE
  int getBonePointsStatic(float *pPoints); // DEBUG-CODE
  int getBoneLines(float *pLines); // DEBUG-CODE
  int getBoneLinesStatic(float *pLines); // DEBUG-CODE

// Переменные и методы для приаттачивания одного скелета к другому
  CalSkeleton	*m_pSlaverSkel;	// скелет ??х.з.
  CalSkeleton	*m_pOwnerSkel;	// скелет хозяин
  CalBone		*m_pOwnerBone;	// кость родитель
  void			attachToSkel(LPCTSTR name, CalSkeleton *pOwner=NULL, CalVector trn=CalVector(0, 0, 0), CalQuaternion rtn=CalQuaternion(0, 0, 0, 1));

private:
	CalVector		m_vFrameTranslation;
	CalQuaternion	m_vFrameRotation;
};

#endif

//****************************************************************************//
