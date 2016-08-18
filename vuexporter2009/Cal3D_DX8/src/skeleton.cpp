//****************************************************************************//
// calskeleton.cpp                                                            //
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

#include "skeleton.h"
#include "error.h"
#include "bone.h"
#include "coreskeleton.h"
#include "corebone.h" // DEBUG

 /*****************************************************************************/
/** Constructs the skeleton instance.
  *
  * This function is the default constructor of the skeleton instance.
  *****************************************************************************/

CalSkeleton::CalSkeleton()
{
  m_pCoreSkeleton = 0;
  //qrtn = CalQuaternion(CalVector(1, 0, 0), 0);
  m_bFrameMoved = false;
  m_pOwnerSkel = NULL;
  m_pSlaverSkel = NULL;
  m_pFrameBone = NULL;
  //
  m_vFrameTranslation.set(0,0,0);
  m_vFrameRotation.x = m_vFrameRotation.y = m_vFrameRotation.z = 0.0f;
  m_vFrameRotation.w = 1.0f;
}

 /*****************************************************************************/
/** Destructs the skeleton instance.
  *
  * This function is the destructor of the skeleton instance.
  *****************************************************************************/

CalSkeleton::~CalSkeleton()
{
  assert(m_vectorBone.empty());
}

 /*****************************************************************************/
/** Calculates the state of the skeleton instance.
  *
  * This function calculates the state of the skeleton instance by recursively
  * calculating the states of its bones.
  *****************************************************************************/

void CalSkeleton::calculateState()
{
  // calculate all bone states of the skeleton
  std::list<int>& listRootCoreBoneId = m_pCoreSkeleton->getListRootCoreBoneId();

  if( m_pFrameBone)
	  m_pFrameBone->clearState();

  std::list<int>::iterator iteratorRootBoneId;
  for(iteratorRootBoneId = listRootCoreBoneId.begin(); iteratorRootBoneId != listRootCoreBoneId.end(); ++iteratorRootBoneId)
	  m_vectorBone[*iteratorRootBoneId]->calculateState();
}

 /*****************************************************************************/
/** Clears the state of the skeleton instance.
  *
  * This function clears the state of the skeleton instance by recursively
  * clearing the states of its bones.
  *****************************************************************************/

void CalSkeleton::clearState()
{
  // clear all bone states of the skeleton
  std::vector<CalBone *>::iterator iteratorBone;
  for(iteratorBone = m_vectorBone.begin(); iteratorBone != m_vectorBone.end(); ++iteratorBone)
  {
    (*iteratorBone)->clearState();
  }
  if( m_pFrameBone)
	  m_pFrameBone->clearState();
}

/*****************************************************************************/
/** Creates the skeleton instance.
  *
  * This function creates the skeleton instance based on a core skeleton.
  *
  * @param pCoreSkeleton A pointer to the core skeleton on which this skeleton
  *                      instance should be based on.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalSkeleton::create(CalCoreSkeleton *pCoreSkeleton)
{
  if(pCoreSkeleton == 0)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  m_pCoreSkeleton = pCoreSkeleton;

  // clone the skeleton structure of the core skeleton
  std::vector<CalCoreBone *>& vectorCoreBone = pCoreSkeleton->getVectorCoreBone();
  // get the number of bones
  int boneCount;
  boneCount = vectorCoreBone.size();

  // reserve space in the bone vector
  m_vectorBone.reserve(boneCount);

  // clone every core bone
  int boneId;
  for(boneId = 0; boneId < boneCount; boneId++)
  {
    CalBone *pBone;
    pBone = new CalBone();
    if(pBone == 0)
    {
      CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
      return false;
    }

    // create a bone for every core bone
    if(!pBone->create(vectorCoreBone[boneId]))
    {
      delete pBone;
      return false;
    }

    // set skeleton in the bone instance
    pBone->setSkeleton(this);

    // insert bone into bone vector
    m_vectorBone.push_back(pBone);
  }

  m_pFrameBone = new CalBone();

  return true;
}

 /*****************************************************************************/
/** Destroys the skeleton instance.
  *
  * This function destroys all data stored in the skeleton instance and frees
  * all allocated memory.
  *****************************************************************************/

void CalSkeleton::destroy()
{
  // destroy all bones
  std::vector<CalBone *>::iterator iteratorBone;
  for(iteratorBone = m_vectorBone.begin(); iteratorBone != m_vectorBone.end(); ++iteratorBone)
  {
    (*iteratorBone)->destroy();
    delete (*iteratorBone);
  }
  m_vectorBone.clear();

  if(m_pFrameBone)
	  delete m_pFrameBone;

  m_pCoreSkeleton = 0;
  m_pFrameBone = 0;
}

 /*****************************************************************************/
/** Provides access to a bone.
  *
  * This function returns the bone with the given ID.
  *
  * @param boneId The ID of the bone that should be returned.
  *
  * @return One of the following values:
  *         \li a pointer to the bone
  *         \li \b 0 if an error happend
  *****************************************************************************/
CalBone *CalSkeleton::getBone(LPCTSTR	name)
{
	return getBone(getCoreSkeleton()->getCoreBoneIdSubName( name));
}

 /*****************************************************************************/
/** Provides access to the core skeleton.
  *
  * This function returns the core skeleton on which this skeleton instance is
  * based on.
  *
  * @return One of the following values:
  *         \li a pointer to the core skeleton
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreSkeleton *CalSkeleton::getCoreSkeleton()
{
  return m_pCoreSkeleton;
}

 /*****************************************************************************/
/** Returns the bone vector.
  *
  * This function returns the vector that contains all bones of the skeleton
  * instance.
  *
  * @return A reference to the bone vector.
  *****************************************************************************/

std::vector<CalBone *>& CalSkeleton::getVectorBone()
{
  return m_vectorBone;
}

 /*****************************************************************************/
/** Locks the state of the skeleton instance.
  *
  * This function locks the state of the skeleton instance by recursively
  * locking the states of its bones.
  *****************************************************************************/

void CalSkeleton::lockState()
{
  if( m_pFrameBone)
    m_pFrameBone->lockState();
  // lock all bone states of the skeleton
  std::vector<CalBone *>::iterator iteratorBone;
  for(iteratorBone = m_vectorBone.begin(); iteratorBone != m_vectorBone.end(); ++iteratorBone)
  {
    (*iteratorBone)->lockState();
  }
}


// Функции для запоминания позы скелета
void CalSkeleton::saveState()
{
  // lock all bone states of the skeleton
  std::vector<CalBone *>::iterator iteratorBone;
  for(iteratorBone = m_vectorBone.begin(); iteratorBone != m_vectorBone.end(); ++iteratorBone)
  {
    (*iteratorBone)->pushState();
  }
}

void CalSkeleton::restoreState()
{
  // lock all bone states of the skeleton
  std::vector<CalBone *>::iterator iteratorBone;
  for(iteratorBone = m_vectorBone.begin(); iteratorBone != m_vectorBone.end(); ++iteratorBone)
  {
    (*iteratorBone)->popState();
  }
}

//****************************************************************************//



//****************************************************************************//
//****************************************************************************//
//****************************************************************************//
// DEBUG-/TEST-CODE                                                           //
//****************************************************************************//
//****************************************************************************//
//****************************************************************************//

int CalSkeleton::getBonePoints(float *pPoints)
{
  int nrPoints;
  nrPoints = 0;

  std::vector<CalBone *>::iterator iteratorBone;
  for(iteratorBone = m_vectorBone.begin(); iteratorBone != m_vectorBone.end(); ++iteratorBone)
  {
    const CalVector& translation = (*iteratorBone)->getTranslationAbsolute();

    *pPoints++ = translation[0];
    *pPoints++ = translation[1];
    *pPoints++ = translation[2];

    nrPoints++;
  }

  return nrPoints;
}

int CalSkeleton::getBonePointsStatic(float *pPoints)
{
  int nrPoints;
  nrPoints = 0;

  std::vector<CalBone *>::iterator iteratorBone;
  for(iteratorBone = m_vectorBone.begin(); iteratorBone != m_vectorBone.end(); ++iteratorBone)
  {
    const CalVector& translation = (*iteratorBone)->getCoreBone()->getTranslationAbsolute();

    *pPoints++ = translation[0];
    *pPoints++ = translation[1];
    *pPoints++ = translation[2];

    nrPoints++;
  }

  return nrPoints;
}

int CalSkeleton::getBoneLines(float *pLines)
{
  int nrLines;
  nrLines = 0;

  std::vector<CalBone *>::iterator iteratorBone;
  for(iteratorBone = m_vectorBone.begin(); iteratorBone != m_vectorBone.end(); ++iteratorBone)
  {
    int parentId;
    parentId = (*iteratorBone)->getCoreBone()->getParentId();

    if(parentId != -1)
    {
      CalBone *pParent;
      pParent = m_vectorBone[parentId];

      const CalVector& translation = (*iteratorBone)->getTranslationAbsolute();
      const CalVector& translationParent = pParent->getTranslationAbsolute();

      *pLines++ = translationParent[0];
      *pLines++ = translationParent[1];
      *pLines++ = translationParent[2];

      *pLines++ = translation[0];
      *pLines++ = translation[1];
      *pLines++ = translation[2];

      nrLines++;
    }
  }

  return nrLines;
}

int CalSkeleton::getBoneLinesStatic(float *pLines)
{
  int nrLines;
  nrLines = 0;

  std::vector<CalBone *>::iterator iteratorBone;
  for(iteratorBone = m_vectorBone.begin(); iteratorBone != m_vectorBone.end(); ++iteratorBone)
  {
    int parentId;
    parentId = (*iteratorBone)->getCoreBone()->getParentId();

    if(parentId != -1)
    {
      CalBone *pParent;
      pParent = m_vectorBone[parentId];

      const CalVector& translation = (*iteratorBone)->getCoreBone()->getTranslationAbsolute();
      const CalVector& translationParent = pParent->getCoreBone()->getTranslationAbsolute();

      *pLines++ = translationParent[0];
      *pLines++ = translationParent[1];
      *pLines++ = translationParent[2];

      *pLines++ = translation[0];
      *pLines++ = translation[1];
      *pLines++ = translation[2];

      nrLines++;
    }
  }

  return nrLines;
}

//****************************************************************************//
//****************************************************************************//
//****************************************************************************//
// END DEBUG-/TEST-CODE                                                       //
//****************************************************************************//
//****************************************************************************//
//****************************************************************************//

CalVector CalSkeleton::getRootPoint( bool abAbsolute)
{
  // calculate all bone states of the skeleton
  std::list<int>& listRootCoreBoneId = m_pCoreSkeleton->getListRootCoreBoneId();

  std::list<int>::iterator iteratorRootBoneId;
  iteratorRootBoneId = listRootCoreBoneId.begin();

  //может быть не этот метод нужно вызывать
  //( см. getTranslationAbsolute() или getTranslationBoneSpace()
  if( abAbsolute)
	return m_vectorBone[*iteratorRootBoneId]->getTranslationAbsolute();
  else
	return m_vectorBone[*iteratorRootBoneId]->getTranslation();
}

// name - имя кости к которой аттачится скелет
// pOwner - указатель на скелета хозяина для данного скелета
void CalSkeleton::attachToSkel(LPCTSTR aname, CalSkeleton *pOwner, CalVector trn, CalQuaternion rtn)
{
	Cal3DComString name;
	if( aname);
		name = aname;
	// unlink the skeleton
	if( pOwner == NULL){
		//if( m_pSlaverSkel != NULL){
		/*m_pSlaverSkel->*/m_pOwnerBone = NULL;
		/*m_pSlaverSkel->*/m_pOwnerSkel = NULL;
		//}
		return;
	}
	//m_pSlaverSkel = pSlaver;

	/*pSlaver->*/m_pOwnerBone = pOwner->getBone( pOwner->m_pCoreSkeleton->getCoreBoneId( name));
	/*pSlaver->*/m_pOwnerSkel = (CalSkeleton*)pOwner;

	// Рассчитываем новый начальный поворот для кости
	// поворачиваем и сдвигаем кость потомок для согласования
	// разницы углов с костью родителем
	std::list<int>& listRootCoreBoneId = /*pSlaver->*/m_pCoreSkeleton->getListRootCoreBoneId();
	std::list<int>::iterator iteratorRootBoneId = listRootCoreBoneId.begin();
	CalCoreBone*		pSlaverRootCoreBone = /*pSlaver->*/m_vectorBone[*iteratorRootBoneId]->getCoreBone();

	pSlaverRootCoreBone->setRotation( rtn);
	pSlaverRootCoreBone->setTranslation( trn);
	m_vectorBone[*iteratorRootBoneId]->m_rotation = rtn;
	m_vectorBone[*iteratorRootBoneId]->m_translation = trn;
}

bool CalSkeleton::haveOwner()
{
	if( m_pOwnerSkel != NULL) return true;
	return false;
}

void CalSkeleton::setUp()
{
  // clear all bone states of the skeleton
  std::vector<CalBone *>::iterator iteratorBone;
  for(iteratorBone = m_vectorBone.begin(); iteratorBone != m_vectorBone.end(); ++iteratorBone)
  {
    (*iteratorBone)->setUpState();
  }
}

CalVector CalSkeleton::getRootDir()
{
  std::list<int>& listRootCoreBoneId = m_pCoreSkeleton->getListRootCoreBoneId();

  std::list<int>::iterator iteratorRootBoneId;
  iteratorRootBoneId = listRootCoreBoneId.begin();

  CalQuaternion rtn = m_vectorBone[*iteratorRootBoneId]->getRotation();
  CalVector ret(1, 0, 0);
  ret *= rtn;
  ret.normalize();
  return ret;
}

int CalSkeleton::getRootID()
{
  std::list<int>& listRootCoreBoneId = m_pCoreSkeleton->getListRootCoreBoneId();
  std::list<int>::iterator iteratorRootBoneId = listRootCoreBoneId.begin();
  return m_pCoreSkeleton->getCoreBoneId(m_vectorBone[*iteratorRootBoneId]->getCoreBone()->getName());
}

CalQuaternion CalSkeleton::getRootRotation()
{
  std::list<int>& listRootCoreBoneId = m_pCoreSkeleton->getListRootCoreBoneId();

  std::list<int>::iterator iteratorRootBoneId;
  iteratorRootBoneId = listRootCoreBoneId.begin();

  //может быть не этот метод нужно вызывать
  //( см. getTranslationAbsolute() или getTranslationBoneSpace()
  return m_vectorBone[*iteratorRootBoneId]->getRotation();
}

CalVector CalSkeleton::getDirection(int boneID, CalVector axes)
{
  CalQuaternion rtn = m_vectorBone[boneID]->getRotationAbsolute();
  CalVector ret(axes);
  ret *= rtn;
  ret.normalize();
  return ret;
}

CalQuaternion CalSkeleton::getBoneRotation(int id, bool abAbs)
{
	if( abAbs)
		return m_vectorBone[id]->getRotationAbsolute();
	else
		return m_vectorBone[id]->getRotation();
}

void CalSkeleton::shiftBone(int id, CalVector delta)
{
	m_vectorBone[id]->m_translation += delta;
	m_vectorBone[id]->m_translationAbsolute += delta;
}

 /***************************************************************************************/
/** moveBone - меняет поворот и/или сдвиг кости											
 *   если установлен bUpdateChilds, то вычисляются абсолютные координаты				
 *   у всех потомков этой кости															
 ****************************************************************************************/
void CalSkeleton::moveBone(DWORD id, CalVector trn, CalQuaternion rtn, bool bUpdateChilds)
{
	CalBone* bone = getBone( id);
	bone->m_translation = trn;
	bone->m_rotation = rtn;

	if( bUpdateChilds) bone->calculateState();
}

 /***************************************************************************************/
/** checkBipVertical - для "живых" персонажей отслеживает постоянно 
 *   вертикальное положение корневой кости скелета.
 *   Если вертикаль нарушается, то может возникнуть перекашивание скелета.
 *   При восстановлении вертикали восстанавливаем прежнее положении для кости Pelvis
 ****************************************************************************************/
void CalSkeleton::checkBipVertical()
{
	//
	float		bipBoneUpToVertAngle = 0.0;
	CalVector	bipBoneUp;
	CalVector	bipBoneUpNormal;
	CalBone*	pBipBone = NULL;
	CalBone*	pBipBoneSon = NULL;
	//
	// проверим, является ли персонаж "живым" (корневая кость содержит имя bip)
	{	// если кость не найдена, выходим из процедуры
		if( !getCoreSkeleton()->containBone("Bip"))	return;
	}
	//
	// получим указатель на корневую кость
	pBipBone = getBone( getRootID());
	//
	// если корневая кость имеет более одного потомка, то скелет неправильный
	if( pBipBone->getCoreBone()->getListChildId().size() != 1) return;
	//
	// получим указатель на "сына" корневой кости
	pBipBoneSon = getBone((*pBipBone->getCoreBone()->getListChildId().begin()));
	//
	// вычислим вектор базовой кости
	bipBoneUp = getDirection( getRootID(), CalVector( 0, 0, 1));
	//
	// вычислим угол поворота до вертикали
	bipBoneUp.normalize();
	bipBoneUpToVertAngle = bipBoneUp.getAngle( CalVector( 0, 0, 1));
	//
	// повернем базовую кость
	bipBoneUpNormal = bipBoneUp.getNormal( CalVector( 0, 0, 1));
	bipBoneUpNormal.normalize();
	CalQuaternion	bipBoneRotationAbsolute = pBipBone->getRotationAbsolute();
	CalQuaternion	bipBoneUpToVertRtn = CalQuaternion( bipBoneUpNormal, -bipBoneUpToVertAngle);
	CalQuaternion	bipBoneUpToVertRtnInverse = CalQuaternion( bipBoneUpNormal, bipBoneUpToVertAngle);
	//qrtn = bipBoneRotationAbsolute;	//????
	//
	// установим новое значение для потомка
	CalQuaternion	_a = pBipBoneSon->m_rotation;
	CalQuaternion	_b = pBipBone->m_rotationAbsolute;
	CalQuaternion	_c = _a;	_c *= _b;
	//
	// установим новое значение
	pBipBone->m_rotationAbsolute *= bipBoneUpToVertRtn;
	pBipBone->m_rotationAbsolute.Normalize();
	pBipBone->saveState();
	//
	//
	CalQuaternion	_by = pBipBone->m_rotationAbsolute;
	_by.conjugate();

	CalQuaternion	_x1 = _c; _x1 *= _by;
	CalQuaternion	_x = _a; _x.conjugate();
	_x *= _x1;
	// установим новое значение для потомка
	pBipBoneSon->m_rotation *= _x;
	pBipBoneSon->m_rotation.Normalize();
}

float CalSkeleton::getFloor()
{
	float floor = FLT_MAX;
	std::vector<CalBone *>::iterator iteratorBone;

	for(iteratorBone = m_vectorBone.begin(); iteratorBone != m_vectorBone.end(); ++iteratorBone)
	{
		Cal3DComString boneName = (*iteratorBone)->getCoreBone()->getName();
		if( boneName.Find("Foot", 0) == -1 && boneName.Find("foot", 0) == -1)	continue;
		float	boneZ = (*iteratorBone)->getTranslationAbsolute().z;
		floor = (boneZ<floor)?boneZ:floor;
	}
	return floor;
}

bool CalSkeleton::get_frame_move(CalVector &trn, CalQuaternion &rtn, bool abRelativeToPrevious)
{
	if( !m_pFrameBone)	return false;

	if( abRelativeToPrevious)
	{
		// translation
		trn = m_pFrameBone->getTranslationAbsolute() - m_vFrameTranslation;
//if( trn.length() > 30)
//int iouerger=0;
		//m_vFrameTranslation = m_pFrameBone->getTranslationAbsolute();

		// rotation
		rtn=m_vFrameRotation;
		rtn.conjugate();
		rtn *= m_pFrameBone->getRotationAbsolute();
		//m_vFrameRotation = m_pFrameBone->getRotationAbsolute();
		rtn.Normalize();
	}
	else
	{
		trn = m_pFrameBone->getTranslationAbsolute();
		rtn = m_pFrameBone->getRotationAbsolute();
	}
	// Save last transformations
	m_vFrameTranslation = m_pFrameBone->getTranslationAbsolute();
	m_vFrameRotation = m_pFrameBone->getRotationAbsolute();
	return true;
}

bool CalSkeleton::set_frame_move(CalVector &trn, CalQuaternion &rtn)
{
	m_vFrameTranslation = trn;
	m_vFrameRotation = rtn;
	m_pFrameBone->m_translationAbsolute = trn;
	m_pFrameBone->m_rotationAbsolute = rtn;
	m_pFrameBone->m_translation = trn;
	m_pFrameBone->m_rotation = rtn;
	return true;
}

int	CalSkeleton::getBoneId(CalBone *apBone)
{
	int index = 0;
	std::vector<CalBone *>::iterator iteratorBone;
	for(iteratorBone = m_vectorBone.begin(); iteratorBone != m_vectorBone.end(); ++iteratorBone, ++index)
	{
		if(*iteratorBone == apBone)	return index;
	}
	return -2;
}

void CalSkeleton::relaxToCoreState()
{
	std::vector<CalBone *>::iterator iteratorBone;
	for(iteratorBone = m_vectorBone.begin(); iteratorBone != m_vectorBone.end(); ++iteratorBone)
	{
		if((*iteratorBone)->getParentId() != -1)
		{
			(*iteratorBone)->setRotation( (*iteratorBone)->getCoreBone()->getRotation());
			(*iteratorBone)->setTranslation( (*iteratorBone)->getCoreBone()->getTranslation());
		}
	}
	calculateState();
}