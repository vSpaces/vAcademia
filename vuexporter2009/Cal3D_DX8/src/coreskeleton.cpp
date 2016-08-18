//****************************************************************************//
// coreskeleton.cpp                                                           //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "error.h"
#include "coreskeleton.h"
#include "corebone.h"

 /*****************************************************************************/
/** Constructs the core skeleton instance.
  *
  * This function is the default constructor of the core skeleton instance.
  *****************************************************************************/

CalCoreSkeleton::CalCoreSkeleton()
{
}

 /*****************************************************************************/
/** Destructs the core skeleton instance.
  *
  * This function is the destructor of the core skeleton instance.
  *****************************************************************************/

CalCoreSkeleton::~CalCoreSkeleton()
{
	destroy();
  assert(m_listRootCoreBoneId.empty());
  assert(m_vectorCoreBone.empty());
}

 /*****************************************************************************/
/** Adds a core bone.
  *
  * This function adds a core bone to the core skeleton instance.
  *
  * @param pCoreBone A pointer to the core bone that should be added.
  *
  * @return One of the following values:
  *         \li the assigned bone \b ID of the added core bone
  *         \li \b -1 if an error happend
  *****************************************************************************/

int CalCoreSkeleton::addCoreBone(CalCoreBone *pCoreBone)
{
  // get next bone id
  int boneId;
  boneId = m_vectorCoreBone.size();

  m_vectorCoreBone.push_back(pCoreBone);

  // if necessary, add the core bone to the root bone list
  if(pCoreBone->getParentId() == -1)
  {
    m_listRootCoreBoneId.push_back(boneId);
  }

  return boneId;
}

 /*****************************************************************************/
/** Calculates the current state.
  *
  * This function calculates the current state of the core skeleton instance by
  * calculating all the core bone states.
  *****************************************************************************/

void CalCoreSkeleton::calculateState()
{
  // calculate all bone states of the skeleton
  std::list<int>::iterator iteratorRootCoreBoneId;
  for(iteratorRootCoreBoneId = m_listRootCoreBoneId.begin(); iteratorRootCoreBoneId != m_listRootCoreBoneId.end(); ++iteratorRootCoreBoneId)
  {
    m_vectorCoreBone[*iteratorRootCoreBoneId]->calculateState();
  }
}

 /*****************************************************************************/
/** Creates the core skeleton instance.
  *
  * This function creates the core skeleton instance.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreSkeleton::create()
{
  return true;
}

 /*****************************************************************************/
/** Destroys the core skeleton instance.
  *
  * This function destroys all data stored in the core skeleton instance and
  * frees all allocated memory.
  *****************************************************************************/

void CalCoreSkeleton::destroy()
{
  // destroy all core animations
  std::vector<CalCoreBone *>::iterator iteratorCoreBone;
  for(iteratorCoreBone = m_vectorCoreBone.begin(); iteratorCoreBone != m_vectorCoreBone.end(); ++iteratorCoreBone)
  {
    (*iteratorCoreBone)->destroy();
    delete (*iteratorCoreBone);
  }

  m_vectorCoreBone.clear();

  // clear root bone id list
  m_listRootCoreBoneId.clear();
}

 /*****************************************************************************/
/** Provides access to a core bone.
  *
  * This function returns the core bone with the given ID.
  *
  * @param coreBoneId The ID of the core bone that should be returned.
  *
  * @return One of the following values:
  *         \li a pointer to the core bone
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreBone *CalCoreSkeleton::getCoreBone(int coreBoneId)
{
  if((coreBoneId < 0) || (coreBoneId >= (int)m_vectorCoreBone.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return 0;
  }

  return m_vectorCoreBone[coreBoneId];
}

 /*****************************************************************************/
/** Returns the ID of a specified core bone.
  *
  * This function returns the ID of a specified core bone.
  *
  * @param strName The name of the core bone that should be returned.
  *
  * @return One of the following values:
  *         \li the \b ID of the core bone
  *         \li \b -1 if an error happend
  *****************************************************************************/

int CalCoreSkeleton::getCoreBoneId(LPCTSTR strName)
{
	if( !strName)	return -1;
  int boneId;
  for(boneId = 0; boneId < (int)m_vectorCoreBone.size(); boneId++)
  {
    /*LPSTR	bonename = (char*)m_vectorCoreBone[boneId]->getName().GetBuffer();
    bonename = _strlwr( bonename);
    LPSTR	paramname = (char*)strName.c_str();
    paramname = _strlwr( paramname);

	std::string _paramname( paramname);
	std::string _bonename( bonename);

    if( _paramname == _bonename) return boneId;*/
	  if ( m_vectorCoreBone[boneId]->getName() == strName) return boneId;
	  //if ( _tcsnicmp(m_vectorCoreBone[boneId]->getName().GetBuffer(), strName) == 0) return boneId;
  }

  return -1;
}

int CalCoreSkeleton::getCoreBoneIdSubName(LPCTSTR strName)
{
	if( !strName)	return -1;
	Cal3DComString boneName( strName);
	boneName.MakeLower();
	if(boneName == "")	return	(*m_listRootCoreBoneId.begin());
	if(boneName == "frame")	return	Cal::PROJECTION_BONE_ID;
	int boneId;
	for(boneId = 0; boneId < (int)m_vectorCoreBone.size(); boneId++)
	{
		//LPCTSTR	buf = strName.GetBuffer();
		//if( m_vectorCoreBone[boneId]->getName().Find(buf, 0) != -1) return boneId;
		Cal3DComString	bn = m_vectorCoreBone[boneId]->getName();
		bn.MakeLower();
		if( bn.Find(boneName.GetBuffer(),0) != -1) return boneId;
	}
	return Cal::NO_SUCH_BONE_ID;	//(*m_listRootCoreBoneId.begin());
}


 /*****************************************************************************/
/** Returns the root core bone id list.
  *
  * This function returns the list that contains all root core bone IDs of the
  * core skeleton instance.
  *
  * @return A reference to the root core bone id list.
  *****************************************************************************/

std::list<int>& CalCoreSkeleton::getListRootCoreBoneId()
{
  return m_listRootCoreBoneId;
}

 /*****************************************************************************/
/** Returns the core bone vector.
  *
  * This function returns the vector that contains all core bones of the core
  * skeleton instance.
  *
  * @return A reference to the core bone vector.
  *****************************************************************************/

std::vector<CalCoreBone *>& CalCoreSkeleton::getVectorCoreBone()
{
  return m_vectorCoreBone;
}

//****************************************************************************//

bool CalCoreSkeleton::containBone(const Cal3DComString& strName)
{
  int boneId;
  for(boneId = 0; boneId < (int)m_vectorCoreBone.size(); boneId++)
  {
	  /*LPSTR	bonename = (char*)m_vectorCoreBone[boneId]->getName().c_str();
	  bonename = _strlwr( bonename);
	  LPSTR	paramname = (char*)strName.c_str();
	  paramname = _strlwr( paramname);

	  std::string _paramname( paramname);
	  std::string _bonename( bonename);

	  if(_bonename.find( _paramname) != std::string::npos) return true;*/

	  Cal3DComString	tofind = strName;
	  Cal3DComString	bn = m_vectorCoreBone[boneId]->getName();
	  bn.MakeLower();	tofind.MakeLower();
	  if(bn.Find(tofind.GetBuffer(),0) != -1) return true;
  }
  return false;
}

bool CalCoreSkeleton::IsBoneRoot(int id)
{
  std::list<int>::iterator iteratorRootBoneId;
  for(iteratorRootBoneId = m_listRootCoreBoneId.begin(); iteratorRootBoneId != m_listRootCoreBoneId.end(); ++iteratorRootBoneId)
  {
    if( *iteratorRootBoneId == id)
		return true;
  }
  return false;
}

float CalCoreSkeleton::getStayedBipHeight()
{
	if( !containBone(Cal3DComString("L Foot")))	return 0.0f;
	if( !containBone(Cal3DComString("L Calf")))		return 0.0f;
	CalCoreBone	*thighBone=NULL, *calfBone=NULL;
	thighBone = getCoreBone(getCoreBoneIdSubName(Cal3DComString("L Foot")));
	calfBone = getCoreBone(getCoreBoneIdSubName(Cal3DComString("L Calf")));
	
	float	height=0.0f;
	if( thighBone)
	{
		CalVector	vkeypos = thighBone->getTranslation();
		height += vkeypos.length();
	}
	if( calfBone)
	{
		CalVector	vkeypos = calfBone->getTranslation();
		height += vkeypos.length();
	}
	return height;
}
