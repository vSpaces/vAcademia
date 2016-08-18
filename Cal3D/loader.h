//****************************************************************************//
// loader.h                                                                   //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_LOADER_H
#define CAL_LOADER_H

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "global.h"
#include "mpeg4fdp.h"

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
class CalMemFile;
class CFace;

//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//

 /*****************************************************************************/
/** The loader class.
  *****************************************************************************/

class CAL3D_API CalLoader
{
// constructors/destructor
public:
	CalLoader();
	virtual ~CalLoader();
	
// member functions
public:
	CFace* loadFace(void* apData, DWORD adwSize);
	DWORD nBytesRead;
  int loadFAP(void *apData, DWORD adwSize, FACEExpression**);
  FACEExpressionsPack* loadFAP(void *apData, DWORD adwSize);
  CalCoreAnimation *loadCoreAnimation(const std::string& strFilename);
  CalCoreMaterial *loadCoreMaterial(const std::string& strFilename);
  CalCoreMesh *loadCoreMesh(const std::string& strFilename);
  CalCoreSkeleton *loadCoreSkeleton(const std::string& strFilename);
  CFace* loadFace(const std::string& strFilename);

  CalCoreAnimation *loadCoreAnimation(HANDLE hFile);
  CalCoreMaterial *loadCoreMaterial(HANDLE hFile);
  CalCoreMesh *loadCoreMesh(HANDLE hFile);
  CalCoreSkeleton *loadCoreSkeleton(HANDLE hFile);
  CFace* loadFace(HANDLE hFile);

  CalCoreKeyframe* loadCoreKeyframe(CalMemFile &mf);
  CalCoreTrack* loadCoreTrack(CalMemFile &mf);
  CalCoreAnimation* loadCoreAnimation(void *apData, DWORD adwSize);
  CalCoreBone* loadCoreBones(CalMemFile &mf);
  CalCoreSkeleton* loadCoreSkeleton(void *apData, DWORD adwSize);
  CalCoreSubmesh* loadCoreSubmesh(CalMemFile &mf, bool abNoSkel);
  CalCoreMesh* loadCoreMesh(void *apData, DWORD adwSize);
  CalCoreMaterial* loadCoreMaterial(void *apData, DWORD adwSize);
  CMpeg4FDP* loadFAPU(void *apData, DWORD adwSize);

  CalCoreMaterial* loadFakeMaterial(const char* textureName);

  void deleteCoreAnimation(CalCoreAnimation* animation);
  void deleteFAPU(CMpeg4FDP* pf);
  void deleteFAP(FACEExpressionsPack* pack);

protected:
  CalCoreBone *loadCoreBones(std::ifstream& file, const std::string& strFilename);
  CalCoreKeyframe *loadCoreKeyframe(std::ifstream& file, const std::string& strFilename);
  CalCoreSubmesh *loadCoreSubmesh(std::ifstream& file, const std::string& strFilename, bool abNoSkel=false);
  CalCoreTrack *loadCoreTrack(std::ifstream& file, const std::string& strFilename);

  CalCoreBone *loadCoreBones(HANDLE hFile);
  CalCoreKeyframe *loadCoreKeyframe(HANDLE hFile);
  CalCoreSubmesh *loadCoreSubmesh(HANDLE hFile, bool abNoSkel=false);
  CalCoreTrack *loadCoreTrack(HANDLE hFile);

  int	file_version;	// version of the current file
};

#endif

//****************************************************************************//
