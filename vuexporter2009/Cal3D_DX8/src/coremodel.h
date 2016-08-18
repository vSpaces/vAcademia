//****************************************************************************//
// coremodel.h                                                                //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_COREMODEL_H
#define CAL_COREMODEL_H

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "global.h"
#include "vector.h"	// Added by ClassView

//****************************************************************************//
// Forward declarations                                                       //
//****************************************************************************//

class CalCoreSkeleton;
class CalCoreAnimation;
class CalCoreMesh;
class CalCoreMaterial;
class CFace;

//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//

 /*****************************************************************************/
/** The core model class.
  *****************************************************************************/

class CAL3D_API CalCoreModel
{
// member variables
protected:
  Cal3DComString m_strName;
  CalCoreSkeleton *m_pCoreSkeleton;
  std::vector<CalCoreAnimation *> m_vectorCoreAnimation;
  std::vector<CalCoreMesh *> m_vectorCoreMesh;
  std::vector<CalCoreMaterial *> m_vectorCoreMaterial;
  std::map<int, std::map<int, int> > m_mapmapCoreMaterialThread;
  Cal::UserData m_userData;

// constructors/destructor
public:
	CalCoreModel();
	virtual ~CalCoreModel();
	
// member functions
public:
  void destroy_geometry();
  void get_worldTranslation(CalVector& trn);
  void get_worldRotation(CalQuaternion& rtn);
  Cal3DComString& getName(){ return m_strName;}
  int attachCoreAnimation(CalCoreAnimation* pAnim);
  CFace* m_pCoreFace;
  int addCoreAnimation(CalCoreAnimation *pCoreAnimation);
  int addCoreMaterial(CalCoreMaterial *pCoreMaterial);
  int addCoreMesh(CalCoreMesh *pCoreMesh);
  bool create(const Cal3DComString& strName);
  bool createCoreMaterialThread(int coreMaterialThreadId);
  void destroy();
  CalCoreAnimation *getCoreAnimation(int coreAnimationId);
  int getCoreAnimationCount();
  CalCoreMaterial *getCoreMaterial(int coreMaterialId);
  int getCoreMaterialCount();
  int getCoreMaterialId(int coreMaterialThreadId, int coreMaterialSetId);
  CalCoreMesh *getCoreMesh(int coreMeshId);
  int getCoreMeshCount();
  CalCoreSkeleton *getCoreSkeleton();
  Cal::UserData getUserData();
  int loadCoreAnimation(const std::string& strFilename);
  int loadCoreAnimation(void *apData, DWORD adwSize);
  int loadCoreMaterial(const std::string& strFilename);
  int loadCoreMaterial(void *apData, DWORD adwSize);
  int loadCoreMesh(const std::string& strFilename);
  int loadCoreMesh(void *apData, DWORD adwSize);
  bool loadCoreSkeleton(const std::string& strFilename);
  bool loadCoreSkeleton(void *apData, DWORD adwSize);
  bool saveCoreAnimation(const std::string& strFilename, int coreAnimtionId);
  bool saveCoreMaterial(const std::string& strFilename, int coreMaterialId);
  bool saveCoreMesh(const std::string& strFilename, int coreMeshId);
  bool saveCoreSkeleton(const std::string& strFilename);
  bool setCoreMaterialId(int coreMaterialThreadId, int coreMaterialSetId, int coreMaterialId);
  void setUserData(Cal::UserData userData);
  int loadCoreFace(const std::string& strFilename);
  int loadCoreFace(void *apData, DWORD adwSize);
};

#endif

//****************************************************************************//
