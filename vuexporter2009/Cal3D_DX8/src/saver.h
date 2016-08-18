//****************************************************************************//
// saver.h                                                                    //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_SAVER_H
#define CAL_SAVER_H

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "global.h"
#include "Mpeg4FDP.h"

//****************************************************************************//
// Forward declarations                                                       //
//****************************************************************************//

class CalCoreModel;
class CalCoreSkeleton;
class CalCoreBone;
class CalCoreAnimation;
class CalCoreTrack;
class CalCoreKeyframe;
class CalCoreMesh;
class CalCoreSubmesh;
class CalCoreMaterial;
class CFaceDesc;

//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//

 /*****************************************************************************/
/** The loader class.
  *****************************************************************************/

class CAL3D_API CalSaver
{
// constructors/destructor
public:
	CalSaver();
	virtual ~CalSaver();
	
// member functions
public:
  bool saveConfig(const std::string& strFilename, CalCoreMesh *pCoreMesh);
  void saveFace(const std::string& strFilename, CFaceDesc *face);
  bool saveCoreAnimation(const std::string& strFilename, CalCoreAnimation *pCoreAnimation);
  bool saveCoreMaterial(const std::string& strFilename, CalCoreMaterial *pCoreMaterial);
  bool saveCoreMaterialOld(const std::string& strFilename, CalCoreMaterial *pCoreMaterial);
  bool saveCoreMaterialXML(const std::string& strFilename, CalCoreMaterial *pCoreMaterial);
  bool saveFAP(const std::string& strFilename, FACEExpression** pexprs, int count);
  bool saveFAPU(const std::string& strFilename, CMpeg4FDP *pFace);
  bool saveCoreMesh(const std::string& strFilename, CalCoreMesh *pCoreMesh);
  bool saveCoreSkeleton(const std::string& strFilename, CalCoreSkeleton *pCoreSkeleton);

protected:
  bool saveCoreBones(std::ofstream& file, const std::string& strFilename, CalCoreBone *pCoreBone);
  bool saveCoreKeyframe(std::ofstream& file, const std::string& strFilename, CalCoreKeyframe *pCoreKeyframe);
  bool saveCoreSubmesh(std::ofstream& file, const std::string& strFilename, CalCoreSubmesh *pCoreSubmesh);
  bool saveCoreTrack(std::ofstream& file, const std::string& strFilename, CalCoreTrack *pCoreTrack);
};

#endif

//****************************************************************************//
