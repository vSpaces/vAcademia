//****************************************************************************//
// CharMixer.h                                                                    //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_CharMixer_H
#define CAL_CharMixer_H

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//
#include "global.h"
#include "mixer.h"


//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//
typedef std::pair<LPVOID, LPVOID> VOIDPAIR;
 /*****************************************************************************/
/** The CharMixer class.
  *****************************************************************************/

class CAL3D_API CalCharMixer : public CalMixer
{
// constructors/destructor
public:
    CalCharMixer();
	virtual ~CalCharMixer();

// member functions	
public:
	virtual void CheckAnimPlace(CalAnimation* pAnim, bool inanimPlace=false);

protected:
	void moveAnimationToPosition(CalAnimation* pAnim, const CalVector& externPosition, const CalQuaternion& externRotation);
};

#endif

//****************************************************************************//
