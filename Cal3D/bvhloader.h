//****************************************************************************//
// loader.h                                                                   //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CALBVH_LOADER_H
#define CALBVH_LOADER_H

#include "CommonCal3DHeader.h"

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//
#include "global.h"
#include "bvhFigure.h"

//****************************************************************************//
// Forward declarations                                                       //
//****************************************************************************//

//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//
class CalCoreAnimation;
class CalCoreTrack;

 /*****************************************************************************/
/** The loader class.
  *****************************************************************************/


class CAL3D_API CalBvhLoader
{
// constructors/destructor
public:
	CalBvhLoader();
	virtual ~CalBvhLoader();

// member functions
public:
	CalCoreAnimation*	loadBVH(void *apData, DWORD adwSize, int& errorCode);

protected:
	//void	addCoreTracks(ArticulatedFigure::Node* pNode, ArticulatedFigure* figure, CalCoreAnimation* pCoreAnimation, CalCoreTrack* pParentCalTrack);
	void	addCoreTracks(ArticulatedFigure::Node* pNode, ArticulatedFigure* figure, CalCoreAnimation* pCoreAnimation, ArticulatedFigure::Node* pParentNode);

private:
	bool m_angleError;
	static const float m_pi;
	static __forceinline float to_rad( float x){ return x*m_pi/180.0f;}
};

#endif

//****************************************************************************//
