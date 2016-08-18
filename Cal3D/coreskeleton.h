//****************************************************************************//
// coreskeleton.h                                                             //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_CORESKELETON_H
#define CAL_CORESKELETON_H

#include "CommonCal3DHeader.h"

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "global.h"
#include "vector.h"	// Added by ClassView

//****************************************************************************//
// Forward declarations                                                       //
//****************************************************************************//

class CalCoreBone;

//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//

 /*****************************************************************************/
/** The core skeleton class.
  *****************************************************************************/

class CAL3D_API CalCoreSkeleton
{
// member variables
protected:
  MP_VECTOR<CalCoreBone *> m_vectorCoreBone;
  MP_LIST<int> m_listRootCoreBoneId;

// constructors/destructor
public:
	CalCoreSkeleton();
	virtual ~CalCoreSkeleton();
	
// member functions
public:
	float getStayedBipHeight();
	bool IsBoneRoot(int id);
	bool containBone(const std::string& strName);
  int addCoreBone(CalCoreBone *pCoreBone);
  void calculateState();
  bool create();
  void destroy();
  CalCoreBone *getCoreBone(int coreBoneId);
  int getCoreBoneId(LPCTSTR strName);
  int getCoreBoneIdSubName(LPCTSTR strName);
  std::list<int>& getListRootCoreBoneId();
  std::vector<CalCoreBone *>& getVectorCoreBone();
};

#endif

//****************************************************************************//
