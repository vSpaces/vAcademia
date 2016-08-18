//****************************************************************************//
// model.cpp                                                                  //
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
#include "global.h"
#include "model.h"
#include "skeleton.h"
#include "bone.h"
#include "mixer.h"
#include "char_mixer.h"
#include "renderer.h"
#include "coremodel.h"
#include "coreskeleton.h"
#include "coremesh.h"
#include "coresubmesh.h"
#include "mesh.h"
#include "physique.h"
#include "springsystem.h"
#include "stepctrl.h"
#include "facedesc.h"
//#include "d3dxmath.h"
#include "submesh.h"
#include "coretrack.h"
#include "corekeyframe.h"
#include "coreanimation.h"
#include "corebone.h"
#include "calanimationmorph.h"
//#include "mpeg4fdp.h"

//class CalCoreKeyframe;

 /*****************************************************************************/
/** Constructs the model instance.
  *
  * This function is the default constructor of the model instance.
  *****************************************************************************/
void Cal::caldoutFF(LPSTR str, float w1, float w2)
{
	char buf[255]; memset( &buf, 0, 255);
	if( w1==FLT_MAX && w2==FLT_MAX)
		sprintf( buf, str);
	else
		if( w1==FLT_MAX)
			sprintf( buf, str, w1);
		else
			sprintf( buf, str, w1, w2);
	OutputDebugString( buf);
}


CalModel::CalModel() : lookctrl( this)
{
  edges = false;
  pickmode = PM_FACES;
  m_bNoSkel = false;
  m_pCoreModel = 0;
  m_pSkeleton = 0;
  m_pMixer = 0;
  m_pPhysique = 0;
  m_pSpringSystem = 0;
  m_pRenderer = 0;
  m_userData = 0;
  m_pStepCtrl = NULL;
  m_pFace = NULL;
  dwLex1 = dwLex2 = 0;
  fFacePhase = 0.0f;
  eMoveType = CMT_ABSOLUTE;
  eRotType = CRT_ROOT;
  vLastRotPoint = CalVector(0, 0, 0);
  qRotation = CalQuaternion(CalVector(0, 0, 1), 0);
  vTranslation = CalVector(0.0, 0, 0);
  bLastRotPointDef = false;
  m_bStepDone = false;
  m_Scale = 1;
  m_bClearCycle = false;
  m_pOwner = NULL;
  m_pSlaver = NULL;
  vScale3D = CalVector(1.0, 1.0, 1.0);
  m_bLocalTrans = false;
  m_pFAPU = NULL;
  bboxIsRight = false;
  m_pfExternTransform = false;
  relative_pos_changed = true;
  __is_character = false;

  //lookctrl.mb_Enable = false;
  pmExternTransform	= NULL;
  D3DXMatrixIdentity( &mMoving);
  D3DXMatrixIdentity( &mCoreTransform);

  m_bNoSkel = false;
  fvf = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
  stride = sizeof(D3DVERTEX);

  externBuffers = false;
  m_bPrevUpdateSkin = false;
  m_floorLevel = FLT_MAX;

  unbinded_self_skeleton = NULL;
}

 /*****************************************************************************/
/** Destructs the model instance.
  *
  * This function is the destructor of the model instance.
  *****************************************************************************/

CalModel::~CalModel()
{
//  assert(m_vectorMesh.empty());
  /*if(m_pFAPU!= NULL)
	  delete m_pFAPU;*/
}

 /*****************************************************************************/
/** Attachs a mesh.
  *
  * This function attachs a mesh to the model instance.
  *
  * @param coreMeshId The ID of the mesh that should be attached.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalModel::attachMesh(int coreMeshId)
{
  // check if the id is valid
  if((coreMeshId < 0) ||(coreMeshId >= m_pCoreModel->getCoreMeshCount()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  // get the core mesh
  CalCoreMesh *pCoreMesh;
  pCoreMesh = m_pCoreModel->getCoreMesh(coreMeshId);

  // check if the mesh is already attached
  int meshId;
  for(meshId = 0; meshId < (int)m_vectorMesh.size(); meshId++)
  {
    // check if we found the matching mesh
    if(m_vectorMesh[meshId]->getCoreMesh() == pCoreMesh)
    {
      // mesh is already active -> do nothing
      return true;
    }
  }

  // allocate a new mesh instance
  CalMesh *pMesh;
  pMesh = new CalMesh();
  if(pMesh == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return false;
  }

  // create the new mesh instance
  if(!pMesh->create(pCoreMesh))
  {
    delete pMesh;
    return false;
  }

  // set model in the mesh instance
  pMesh->setModel(this);

  // insert the new mesh into the active list
  m_vectorMesh.push_back(pMesh);

  return true;
}

 /*****************************************************************************/
/** Creates the model instance.
  *
  * This function creates the model instance based on a core model.
  *
  * @param pCoreModel A pointer to the core model on which this model instance
  *                   should be based on.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalModel::create(CalCoreModel *pCoreModel)
{
  bboxIsRight = false;
  m_bPrevUpdateSkin = false;
  if(pCoreModel == 0)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  m_pCoreModel = pCoreModel;

  // allocate a new skeleton instance
  if( m_pCoreModel->getCoreSkeleton() != NULL){
	  CalSkeleton *pSkeleton;
	  pSkeleton = new CalSkeleton();
	  if(pSkeleton == 0)
	  {
		CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
		return false;
	  }

	  // create the skeleton from the core skeleton
	  if(!pSkeleton->create(pCoreModel->getCoreSkeleton()))
	  {
		delete pSkeleton;
		return false;
	  }

	  m_pSkeleton = pSkeleton;
	  m_pSkeleton->setUp();

	  if(pSkeleton->getCoreSkeleton()->containBone("Bip") ||
		pSkeleton->getCoreSkeleton()->containBone("bip"))
			__is_character = true;
  }else NoSkel( true);

  // allocate a new mixer instance
  CalMixer *pMixer;
  if( is_character())
	pMixer = new CalCharMixer();
  else
	pMixer = new CalMixer();
  if(pMixer == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    m_pSkeleton->destroy();
	if( m_pSkeleton) delete m_pSkeleton;
    return false;
  }

  // create the mixer from this model
  if(!pMixer->create(this)) 
  {
	  if( m_pSkeleton){
		m_pSkeleton->destroy();
		delete m_pSkeleton;
	  }
    delete pMixer;
    return false;
  }

  m_pMixer = pMixer;
  //m_pMixer->m_pModel = this;

  // allocate a new physqiue instance
  CalPhysique *pPhysique;
  pPhysique = new CalPhysique();
  if(pPhysique == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    m_pMixer->destroy();
    delete m_pMixer;
	if( m_pSkeleton){
		m_pSkeleton->destroy();
		delete m_pSkeleton;
	}
    return false;
  }

  // create the physique from this model
  if(!pPhysique->create(this))
  {
    m_pMixer->destroy();
    delete m_pMixer;
	if( m_pSkeleton){
		m_pSkeleton->destroy();
		delete m_pSkeleton;
	}
    delete pPhysique;
    return false;
  }

  m_pPhysique = pPhysique;

  // allocate a new spring system instance
  CalSpringSystem *pSpringSystem;
  pSpringSystem = new CalSpringSystem();
  if(pSpringSystem == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    m_pPhysique->destroy();
    delete m_pPhysique;
    m_pMixer->destroy();
    delete m_pMixer;
	if( m_pSkeleton){
		m_pSkeleton->destroy();
		delete m_pSkeleton;
	}
    return false;
  }

  // create the spring system from this model
  if(!pSpringSystem->create(this))
  {
    m_pPhysique->destroy();
    delete m_pPhysique;
    m_pMixer->destroy();
    delete m_pMixer;
	if( m_pSkeleton){
		m_pSkeleton->destroy();
		delete m_pSkeleton;
	}
    delete pSpringSystem;
    return false;
  }

  m_pSpringSystem = pSpringSystem;

  // allocate a new renderer instance
  CalRenderer *pRenderer;
  pRenderer = new CalRenderer();
  if(pRenderer == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    m_pSpringSystem->destroy();
    delete m_pSpringSystem;
    m_pPhysique->destroy();
    delete m_pPhysique;
    m_pMixer->destroy();
    delete m_pMixer;
	if( m_pSkeleton){
		m_pSkeleton->destroy();
		delete m_pSkeleton;	
	}
    return false;
  }

  // create the renderer from this model
  if(!pRenderer->create(this))
  {
    m_pSpringSystem->destroy();
    delete m_pSpringSystem;
    m_pPhysique->destroy();
    delete m_pPhysique;
    m_pMixer->destroy();
    delete m_pMixer;
	if( m_pSkeleton){
		m_pSkeleton->destroy();
		delete m_pSkeleton;
	}
    delete pRenderer;
    return false;
  }

  m_pRenderer = pRenderer;

  // initialize the user data
  m_userData = 0;

  setFace(pCoreModel->m_pCoreFace);

  return true;
}

 /*****************************************************************************/
/** Destroys the model instance.
  *
  * This function destroys all data stored in the model instance and frees all
  * allocated memory.
  *****************************************************************************/

void CalModel::destroy()
{
  unbindSkeleton();

  // destroy all active meshes
  int meshId;
  for(meshId = 0; meshId < (int)m_vectorMesh.size(); meshId++)
  {
    m_vectorMesh[meshId]->destroy();
    delete m_vectorMesh[meshId];
  }
  m_vectorMesh.clear();

  // destroy the renderer instance
  if(m_pRenderer != 0)
  {
    m_pRenderer->destroy();
    delete m_pRenderer;
    m_pRenderer = 0;
  }

  // destroy the spring system instance
  if(m_pSpringSystem != 0)
  {
    m_pSpringSystem->destroy();
    delete m_pSpringSystem;
    m_pSpringSystem = 0;
  }

  // destroy the physique instance
  if(m_pPhysique != 0)
  {
    m_pPhysique->destroy();
    delete m_pPhysique;
    m_pPhysique = 0;
  }

  // destroy the mixer instance
  if(m_pMixer != 0)
  {
    m_pMixer->destroy();
    delete m_pMixer;
    m_pMixer = 0;
  }

  // destroy the skeleton instance
  if(m_pSkeleton != 0)
  {
    m_pSkeleton->destroy();
    delete m_pSkeleton;
    m_pSkeleton = 0;
  }

  if( m_pStepCtrl)  delete m_pStepCtrl;

  m_pCoreModel = 0;
}

void CalModel::destroy_geometry()
{
  // destroy all active meshes
  int meshId;
  for(meshId = 0; meshId < (int)m_vectorMesh.size(); meshId++)
  {
    m_vectorMesh[meshId]->destroy();
    delete m_vectorMesh[meshId];
  }
  m_vectorMesh.clear();
  
  m_pCoreModel = 0;
}

 /*****************************************************************************/
/** Detaches a mesh.
  *
  * This function detaches a mesh from the model instance.
  *
  * @param coreMeshId The ID of the mesh that should be detached.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalModel::detachMesh(int coreMeshId)
{
  // check if the id is valid
  if((coreMeshId < 0) ||(coreMeshId >= m_pCoreModel->getCoreMeshCount()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  // get the core mesh
  CalCoreMesh *pCoreMesh;
  pCoreMesh = m_pCoreModel->getCoreMesh(coreMeshId);

  // find the mesh for the given id
  std::vector<CalMesh *>::iterator iteratorMesh;
  for(iteratorMesh = m_vectorMesh.begin(); iteratorMesh != m_vectorMesh.end(); ++iteratorMesh)
  {
    // get the mesh
    CalMesh *pMesh;
    pMesh = *iteratorMesh;

    // check if we found the matching mesh
    if(pMesh->getCoreMesh() == pCoreMesh)
    {
      // destroy the mesh
      pMesh->destroy();
      delete pMesh;

      // erase the mesh out of the active mesh list
      m_vectorMesh.erase(iteratorMesh);

      return true;
    }
  }

  return false;
}

 /*****************************************************************************/
/** Provides access to the core model.
  *
  * This function returns the core model on which this model instance is based
  * on.
  *
  * @return One of the following values:
  *         \li a pointer to the core model
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreModel *CalModel::getCoreModel()
{
  return m_pCoreModel;
}

 /*****************************************************************************/
/** Provides access to an attached mesh.
  *
  * This function returns the attached mesh with the given core mesh ID.
  *
  * @param coreMeshId The core mesh ID of the mesh that should be returned.
  *
  * @return One of the following values:
  *         \li a pointer to the mesh
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalMesh *CalModel::getMesh(int coreMeshId)
{
  // check if the id is valid
  if((coreMeshId < 0) ||(coreMeshId >= m_pCoreModel->getCoreMeshCount()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return 0;
  }

  // get the core mesh
  CalCoreMesh *pCoreMesh;
  pCoreMesh = m_pCoreModel->getCoreMesh(coreMeshId);

  // search the mesh
  int meshId;
  for(meshId = 0; meshId < (int)m_vectorMesh.size(); meshId++)
  {
    // check if we found the matching mesh
    if(m_vectorMesh[meshId]->getCoreMesh() == pCoreMesh)
    {
      return m_vectorMesh[meshId];
    }
  }

  return 0;
}

 /*****************************************************************************/
/** Sets the LOD level.
  *
  * This function sets the LOD level of all attached meshes.
  *
  * @param lodLevel The LOD level in the range [0.0, 1.0].
  *****************************************************************************/

void CalModel::setLodLevel(float lodLevel)
{
  // set the lod level in all meshes
  std::vector<CalMesh *>::iterator iteratorMesh;
  for(iteratorMesh = m_vectorMesh.begin(); iteratorMesh != m_vectorMesh.end(); ++iteratorMesh)
  {
    // set the lod level in the mesh
    (*iteratorMesh)->setLodLevel(lodLevel);
  }
}

 /*****************************************************************************/
/** Sets the material set.
  *
  * This function sets the material set of all attached meshes.
  *
  * @param setId The ID of the material set.
  *****************************************************************************/

void CalModel::setMaterialSet(int setId)
{
  // set the lod level in all meshes
  std::vector<CalMesh *>::iterator iteratorMesh;
  for(iteratorMesh = m_vectorMesh.begin(); iteratorMesh != m_vectorMesh.end(); ++iteratorMesh)
  {
    // set the material set in the mesh
    (*iteratorMesh)->setMaterialSet(setId);
  }
  //
	int			meshId;
	CalRenderer	*pCalRenderer = getRenderer();
	for(meshId = 0; meshId < pCalRenderer->getMeshCount(); meshId++){
		int submeshCount;
		submeshCount = pCalRenderer->getSubmeshCount(meshId);
		int submeshId;
		for(submeshId = 0; submeshId < submeshCount; submeshId++)
		{
		  // select mesh and submesh for further data access
		  if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
		  {
			  float materialColor[4];
			  unsigned char meshColor[4];

			  // set the material ambient color
			  /*pCalRenderer->getAmbientColor(&meshColor[0]);
			  materialColor[0] = meshColor[0] / 255.0f;  materialColor[1] = meshColor[1] / 255.0f; materialColor[2] = meshColor[2] / 255.0f; materialColor[3] = meshColor[3] / 255.0f;
			  pCalRenderer->m_pSelectedSubmesh->material.Ambient.r = (D3DVALUE)materialColor[0]; 
			  pCalRenderer->m_pSelectedSubmesh->material.Ambient.g = (D3DVALUE)materialColor[1]; 
			  pCalRenderer->m_pSelectedSubmesh->material.Ambient.b = (D3DVALUE)materialColor[2];
			  pCalRenderer->m_pSelectedSubmesh->material.Ambient.a = (D3DVALUE)materialColor[3];
 
			  // set the material diffuse color
			  pCalRenderer->getDiffuseColor(&meshColor[0]);
			  materialColor[0] = meshColor[0] / 255.0f;  materialColor[1] = meshColor[1] / 255.0f; materialColor[2] = meshColor[2] / 255.0f; materialColor[3] = meshColor[3] / 255.0f;
			  pCalRenderer->m_pSelectedSubmesh->material.Diffuse.r = (D3DVALUE)materialColor[0]; 
			  pCalRenderer->m_pSelectedSubmesh->material.Diffuse.g = (D3DVALUE)materialColor[1]; 
			  pCalRenderer->m_pSelectedSubmesh->material.Diffuse.b = (D3DVALUE)materialColor[2]; 
			  pCalRenderer->m_pSelectedSubmesh->material.Diffuse.a = 1.0 - (D3DVALUE)materialColor[3]; 


//pCalRenderer->m_pSelectedSubmesh->material.Ambient.r = pCalRenderer->m_pSelectedSubmesh->material.Diffuse.r; 
//pCalRenderer->m_pSelectedSubmesh->material.Ambient.g = pCalRenderer->m_pSelectedSubmesh->material.Diffuse.g;
//pCalRenderer->m_pSelectedSubmesh->material.Ambient.b = pCalRenderer->m_pSelectedSubmesh->material.Diffuse.b;
//pCalRenderer->m_pSelectedSubmesh->material.Ambient.a = pCalRenderer->m_pSelectedSubmesh->material.Diffuse.a;


			  // set the material specular color
			  pCalRenderer->getSpecularColor(&meshColor[0]);
			  materialColor[0] = meshColor[0] / 255.0f;  materialColor[1] = meshColor[1] / 255.0f; materialColor[2] = meshColor[2] / 255.0f; materialColor[3] = meshColor[3] / 255.0f;
			  pCalRenderer->m_pSelectedSubmesh->material.Specular.r = (D3DVALUE)materialColor[0]; 
			  pCalRenderer->m_pSelectedSubmesh->material.Specular.g = (D3DVALUE)materialColor[1]; 
			  pCalRenderer->m_pSelectedSubmesh->material.Specular.b = (D3DVALUE)materialColor[2]; 
			  pCalRenderer->m_pSelectedSubmesh->material.Specular.a = 1.0;//(D3DVALUE)materialColor[3]; 

			  pCalRenderer->getEmissiveColor(&meshColor[0]);
			  materialColor[0] = meshColor[0] / 255.0f;  materialColor[1] = meshColor[1] / 255.0f; materialColor[2] = meshColor[2] / 255.0f; materialColor[3] = meshColor[3] / 255.0f;
			  pCalRenderer->m_pSelectedSubmesh->material.Emissive.r = (D3DVALUE)materialColor[0]; 
			  pCalRenderer->m_pSelectedSubmesh->material.Emissive.g = (D3DVALUE)materialColor[1]; 
			  pCalRenderer->m_pSelectedSubmesh->material.Emissive.b = (D3DVALUE)materialColor[2]; 
			  pCalRenderer->m_pSelectedSubmesh->material.Emissive.a = 1.0;//(D3DVALUE)materialColor[3]; 

			  // set the material shininess factor
			  float shininess;
			  shininess = pCalRenderer->getShininess();
			  pCalRenderer->m_pSelectedSubmesh->material.Power = (float)shininess;*/

			  //
		  }
		}
	}
	//
}

 /*****************************************************************************/
/** Stores user data.
  *
  * This function stores user data in the model instance.
  *
  * @param userData The user data that should be stored.
  *****************************************************************************/

void CalModel::setUserData(Cal::UserData userData)
{
  m_userData = userData;
}

 /*****************************************************************************/
/** Updates the model instance.
  *
  * This function updates the model instance for a given amount of time.
  *
  * @param deltaTime The elapsed time in seconds since the last update.
  *****************************************************************************/
//bool test = false;
bool CalModel::update(float deltaTime, bool updateSkin)
{
	if( NoSkel()) return false;	//	если без скелета, то не апдейтим модель
	if( !m_pMixer)	return false;

	m_bRecalcRootTM = false;
	bool	bNeedToUpdate = true;
	if( !m_pMixer->updateAnimation(deltaTime)){
		//if( m_bClearCycle)
		//	bNeedToUpdate = false;
		m_bClearCycle = true;
	}else{
		m_bClearCycle = false;
	}
	//if( m_pOwner != NULL)	bNeedToUpdate = true;

	if( m_bPrevUpdateSkin && !updateSkin)
	{
		m_bPrevUpdateSkin = false;
		updateSkin = true;
	}
	else	
	{
		if( !m_bPrevUpdateSkin && updateSkin)
			bNeedToUpdate = true;
		m_bPrevUpdateSkin = updateSkin;
	}
	if( unbinded_self_skeleton)	bNeedToUpdate=true;
	m_pMixer->updateSkeleton();
	if ((updateSkin && (bNeedToUpdate || pointctrl.bEnable || lookctrl.m_bEnabled)) || m_pFAPU)
	{
		updatePointCtrl( deltaTime);
		lookctrl.Update( deltaTime);		
		//if( !test)
		//{
			m_pPhysique->update();
			m_pMixer->updateMorph();
			m_pSpringSystem->update(deltaTime);
		//	test = true;
		//}
		//bboxIsRight = false;
		//getBoundingBox(NULL, NULL);
		return true;
	}
	return bNeedToUpdate;
}

//****************************************************************************//

void CalModel::Rotate(CalVector axes, float angle, CALOPTYPE type)
{
	//точка - центр поворота
	CalVector	pRotPoint;
	//точка - центр модели
	CalVector	pRootPoint;
	//вектор - длина плеча
	CalVector	pVector;
	//кватернион поворота
	CalQuaternion quatRotation(axes, angle); 

	if( eRotType == CRT_ROOT)
	  pRotPoint = vTranslation;//CalVector(0, 0, 0);//getSkeleton()->getRootPoint();
	else
	  pRotPoint = rotPoint;

	//абс. коорд = лок.коорд + перемещение
	pRootPoint = vTranslation;//getSkeleton()->getRootPoint() + vTranslation;
	pVector = pRootPoint - pRotPoint;

	//добавляем или заменяем поворот
	if( type == CAL_AFTER)
		qRotation *= quatRotation;
	else
		qRotation = quatRotation;

	pVector *= quatRotation;

	vTranslation = pRotPoint + pVector;
}

void CalModel::Move(CalVector v, CALOPTYPE type)
{
	//добавляем или заменяем сдвиг
	CalQuaternion stepQuat = qRotation;
	stepQuat.w = -stepQuat.w;
	if( eMoveType == CMT_RELATIVE){
		v *= stepQuat;
		vTranslation += v;
	}else
		vTranslation = v;
}

bool CalModel::step(STEPPARAMS	params)
{
	if( m_pStepCtrl != NULL)	//previous step is playing now
		return false;
	
	m_pStepCtrl = new StepCtrl(this);
	m_pStepCtrl->beginStep( params);
	return true;
}

void CalModel::endStep()
{
	if(m_pStepCtrl == NULL)
		return;
	m_pStepCtrl->endStep();
}

void CalModel::clearStep()
{
	if(m_pStepCtrl == NULL)
		return;
	//if( m_pStepCtrl->canBeCleared()){
	delete m_pStepCtrl;
	m_pStepCtrl = NULL;
	//actiontype = AT_STEP;
	m_bStepDone = true;
	setDoneFlag(AT_STEP, 0);
	//}
}

void CalModel::setDuration(int animID, float duration)
{
	m_pMixer->setAnimDuration(animID, duration);
}

void CalModel::setFace(CFace *fd)
{
	m_pFace = fd;
}

CFace* CalModel::getFace()
{
	return m_pFace;
}

void CalModel::updateSkin(CalAnimationMorph* ap_Face)
{
  CFace*		m_pCalcFace			= NULL;
  CalRenderer*	pCalRenderer	= NULL;

  if( ap_Face)
	  m_pCalcFace = ap_Face->m_pFace;
  else
	  m_pCalcFace = m_pFace;

  pCalRenderer = getRenderer();
  DWORD meshCount = pCalRenderer->getMeshCount();

  if(m_pCalcFace != NULL){
	  if( !m_pCalcFace->init( this))
		  return;
	  m_pCalcFace->calcVectors();
	  for(int meshId = 0; meshId < 1/*meshCount*/; meshId++){
		  for(/*register*/int iSubmeshId=0; iSubmeshId<m_pCalcFace->header.dwSubmeshCount; iSubmeshId++){
			  // select mesh and submesh for further data access
			  if(pCalRenderer->selectMeshSubmesh(meshId, m_pCalcFace->aSubmeshNum[iSubmeshId]))
			  {
				float* meshVertices;
				//pCalRenderer->getVertList(&meshVertices);
				pCalRenderer->m_pSelectedSubmesh->lockVertices();
				meshVertices = pCalRenderer->m_pSelectedSubmesh->getVertices();
				int lex1 = 0;
				if( !ap_Face) lex1 = dwLex1;
				else lex1 = ((CalAnimationMorph*)ap_Face)->m_iPhase1;
				int lex2 = 0;
				if( !ap_Face) lex2 = dwLex2;
				else lex2 = ((CalAnimationMorph*)ap_Face)->m_iPhase2;
				float phase = 0;
				if( !ap_Face) phase = fFacePhase;
				else phase = ((CalAnimationMorph*)ap_Face)->m_fPhase;
				getPhysique()->calculateFace(m_pCalcFace, meshVertices, iSubmeshId, lex1, lex2, phase, pCalRenderer->useVB());
				pCalRenderer->m_pSelectedSubmesh->releaseVertices();
			  }
		  }
	  }
  }
}

void CalModel::setFace(WORD l1, WORD l2, float phase)
{
  dwLex1 = l1;
  dwLex2 = l2;
  fFacePhase = phase;
}

//Функция возвращаем матрицу транформации 
// модели в глобальных координатах
void CalModel::getTransMatrix(D3DMATRIX*	pMatrix){

	//D3DMATRIX	tr, rot, roottr, temp, local, locsc;
	D3DXMATRIX	mCenter, mRestore, mMoving, mTrans, mOut;
	CalQuaternion	rotation;
	CalVector		translation;
	D3DXQUATERNION quat;
	//сдвигаем модель в 0
	CalVector root;

	if( NoSkel()){		// если нет скелета
		*pMatrix = mCoreTransform;
		return;
	}

	if( m_pOwner != NULL){
		m_pOwner->getTransMatrix( pMatrix);
/*if( getCoreModel()->getName().compare("color1") == 0){
char buf[255]; memset( &buf, 0, 255);
CalVector stepVector = m_pSkeleton->getRootPoint();
stepVector += m_pOwner->vTranslation;
sprintf( buf, "Cal position1 (%.2f, %.2f, %.2f)\n", stepVector.x, stepVector.y, stepVector.z);OutputDebugString( buf);
}*/
		return;
	}else{
/*if( getCoreModel()->getName().compare("color1") == 0){
char buf[255]; memset( &buf, 0, 255);
CalVector stepVector = getRootPosition();
sprintf( buf, "Cal position2 (%.2f, %.2f, %.2f)\n", stepVector.x, stepVector.y, stepVector.z);OutputDebugString( buf);
}*/
	}

	getCenterMatrix( &mCenter);
	getRestoreMatrix( &mRestore);
	getMovingMatrix( &mMoving);
	getTransformMatrix( &mTrans);
	
	MULTMATRIX( &mOut, &mCenter, &mTrans);
	MULTMATRIX( &mOut, &mOut,  &mMoving);
	MULTMATRIX( &mOut, &mOut,  &mRestore);

	*pMatrix = mOut;
}

void CalModel::setRotType(CALROTTYPE type)
{
	eRotType = type;
}

void CalModel::setMoveType(CALMOVETYPE type)
{
	eMoveType = type;
}

void CalModel::saveRotPoint ()
{
	if( eRotType == CRT_ROOT)
	  vLastRotPoint = getSkeleton()->getRootPoint();
	else
	  vLastRotPoint = rotPoint;

	bLastRotPointDef = true;
}

void CalModel::setScale(float scale)
{
	m_Scale = scale;
}

void CalModel::checkCoreAnimations()
{
	if( !getCoreModel())	return;

	CalAnimation*	nullAnim = 0;
	int				coreSize = getCoreModel()->getCoreAnimationCount();

	CalMixer *pMixer = getMixer();
	if( pMixer->m_vectorAnimation.size() < coreSize){
		pMixer->m_vectorAnimation.push_back( nullAnim);
	}
}

void CalModel::attachVB(IDirect3DDevice8 *pD3D8)
{
	int		vcount = 0, fcount = 0;
	int		meshId;
	CalRenderer	*pCalRenderer = getRenderer();

	if( pCalRenderer->lockVertices())
	{
		if( pCalRenderer->lockIndices())
		{
			for(meshId = 0; meshId < pCalRenderer->getMeshCount(); meshId++)
			{
				int submeshCount;
				submeshCount = pCalRenderer->getSubmeshCount(meshId);
				int submeshId;
				for(submeshId = 0; submeshId < submeshCount; submeshId++)
				{
					// select mesh and submesh for further data access
					if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
					{
						pCalRenderer->m_pSelectedSubmesh->stride = stride;
						pCalRenderer->m_pSelectedSubmesh->vbStartIndex = vcount;
						pCalRenderer->m_pSelectedSubmesh->ibStartIndex = fcount;
						pCalRenderer->m_pSelectedSubmesh->fill_vertices(pCalRenderer->get_vertices(pCalRenderer->m_pSelectedSubmesh));
						pCalRenderer->m_pSelectedSubmesh->fill_indices((WORD*)pCalRenderer->get_indices(pCalRenderer->m_pSelectedSubmesh));
						if( NoSkel())
						{
							float* pBaseVertexBuffer=(float*)pCalRenderer->get_vertices(pCalRenderer->m_pSelectedSubmesh);
							getPhysique()->calculateNormals(pCalRenderer->m_pSelectedSubmesh, pBaseVertexBuffer);
						}
						float meshTextureCoordinates[3][3];
						int textureCoordinateCount;
						for( int itx=0; itx<pCalRenderer->getMapCount(); itx++)
							textureCoordinateCount = pCalRenderer->getTextureCoordinates(itx, &meshTextureCoordinates[0][0]);
						vcount = vcount + pCalRenderer->m_pSelectedSubmesh->getVertexCount();
						fcount = fcount + pCalRenderer->m_pSelectedSubmesh->getFaceCount()*3;
					}
				}
			}
			pCalRenderer->releaseIndices();
		}
		pCalRenderer->releaseVertices();
	}
	//
}

bool CalModel::doneAction(ACTIONTYPE	*type, int	*value)
{
	if( actiontype.empty()) return false;
	ACTIONTYPE	donetype;
	int			doneid;

	donetype = actiontype[actiontype.size()-1];
	doneid	 = actionid[actionid.size()-1];
	actiontype.pop_back();
	actionid.pop_back();

	*type = donetype;
	*value = doneid;

	if(donetype == AT_STEP){
		if( m_bStepDone) {
			*value = doneid;
			m_bStepDone = false;
		}
	}
	return true;
}

int CalModel::getDoneAnimationID()
{
	int i_temp = 0;//m_iDoneAnimation;
	//m_iDoneAnimation = -1;
	return i_temp;
}

CalVector CalModel::getRootPosition( bool buseextern)
{
	CalVector	rootVector;
	if( !m_pSkeleton)	rootVector = CalVector(0,0,0);
	else	rootVector = m_pSkeleton->getRootPoint();

	rootVector += vTranslation;
	if( buseextern || pmExternTransform){
		D3DXVECTOR4	v4;
		D3DXVec3Transform( &v4, &D3DXVECTOR3(rootVector.x, rootVector.y, rootVector.z), (D3DXMATRIX*)&pmExternTransform);

		rootVector.x = v4.x / v4.w;
		rootVector.y = v4.y / v4.w;
		rootVector.z = v4.z / v4.w;
	}
	return rootVector;
}

CalVector CalModel::getDirection()
{
	CalVector		dir = m_pSkeleton->getRootDir();
	CalQuaternion	dirQ=qRotation;
	dirQ.conjugate();
	dir *= dirQ;
	return	dir;
}

CalVector CalModel::getRootDirection()
{
	CalVector		dir = m_pSkeleton->getRootDir();
	dir.normalize();
	return dir;
}

void CalModel::setDoneFlag(ACTIONTYPE type, int id)
{
	actiontype.push_back( type);
	actionid.push_back( id);
}

//int CalModel::lookAt(DWORD adwDelay, CalModel *m_pModel, float trTime, bool bToCamera, bool bToPoint, CalVector camPos)
int CalModel::lookAt(LOOKATPARAMS params)
{
	if( NoSkel())	return 0;
	lookctrl.Init( params);
	return 0;

//	// Определим задержку
//	float mfDelay=0.0;
//	if( params.adwDelay != 0){
//		mfDelay = ((float)params.adwDelay*rand()/RAND_MAX)/1000;
//	}
//	// Создаем анимацию на поворот головы
//	// 0 кадр : текущий поворот
//	// 1 кадр : поворот по направлению персонажа
//	CalVector			translation;
//	CalQuaternion		rotation;
//	CalCoreAnimation*	pLookAtCoreAnimation=NULL;
//	CalCoreTrack*		pLookAtCoreTrackN=NULL;
//	CalCoreTrack*		pLookAtCoreTrackH=NULL;
//	CalCoreKeyframe*	pLookAtKeyFrame=NULL;
//	int					iNeckID			= -1;
//	int					iHeadID			= -1;
//	int					iLookAtHeadID	= -1;
//	int					iLClavicleID	= -1;
//	int					iRClavicleID	= -1;
//
//	params.trTime = params.trTime > 1.0 ? 1.0 : params.trTime;
//	params.trTime = params.trTime < 0.0 ? 0.0 : params.trTime;
//	// Скелет не содержит кость *NECK*
//	// Скелет не содержит кость HEAD*
//	if(!( m_pSkeleton->getCoreSkeleton()->containBone("Head") || m_pSkeleton->getCoreSkeleton()->containBone("head")))
//		return -1;
//	if(m_pSkeleton->getCoreSkeleton()->containBone("Neck"))
//		iNeckID = m_pSkeleton->getCoreSkeleton()->getCoreBoneIdSubName( "Neck");
//	if(m_pSkeleton->getCoreSkeleton()->containBone("neck"))
//		iNeckID = m_pSkeleton->getCoreSkeleton()->getCoreBoneIdSubName( "neck");
//
//	if(m_pSkeleton->getCoreSkeleton()->containBone("Head"))
//		iHeadID = m_pSkeleton->getCoreSkeleton()->getCoreBoneIdSubName( "Head");
//	if(m_pSkeleton->getCoreSkeleton()->containBone("head"))
//		iHeadID = m_pSkeleton->getCoreSkeleton()->getCoreBoneIdSubName( "head");
//
//	if(m_pSkeleton->getCoreSkeleton()->containBone("L Clavicle"))
//		iLClavicleID = m_pSkeleton->getCoreSkeleton()->getCoreBoneIdSubName( "L Clavicle");
//	if(m_pSkeleton->getCoreSkeleton()->containBone("R Clavicle"))
//		iRClavicleID = m_pSkeleton->getCoreSkeleton()->getCoreBoneIdSubName( "R Clavicle");
//
//		// -------------ВТОРОЙ СКЕЛЕТ---------------------------------------------
//	if( params.m_pModel != NULL){
//		if(params.m_pModel->m_pSkeleton->getCoreSkeleton()->containBone("Neck"))
//			iLookAtHeadID = params.m_pModel->m_pSkeleton->getCoreSkeleton()->getCoreBoneIdSubName( "Neck");
//		if(params.m_pModel->m_pSkeleton->getCoreSkeleton()->containBone("neck"))
//			iLookAtHeadID = params.m_pModel->m_pSkeleton->getCoreSkeleton()->getCoreBoneIdSubName( "neck");
//		if( iLookAtHeadID == -1)
//			iLookAtHeadID = params.m_pModel->m_pSkeleton->getCoreSkeleton()->getCoreBoneIdSubName( "neck");
//	}
//
//	// Первый кадр
//	// ---голова---
//	pLookAtCoreAnimation = new CalCoreAnimation();
//	pLookAtCoreAnimation->create();
//
//	pLookAtCoreTrackH = new CalCoreTrack();
//	pLookAtCoreTrackH->create();
//	pLookAtCoreTrackH->setCoreBoneId(iHeadID);
//	pLookAtCoreAnimation->addCoreTrack( pLookAtCoreTrackH);
//
//	pLookAtKeyFrame = new CalCoreKeyframe();
//	pLookAtKeyFrame->create();
//	translation = m_pSkeleton->getBone(iHeadID)->getTranslation();
//	
//	rotation = m_pSkeleton->getBone(iHeadID)->getRotation();
//	rotation.Normalize();
//	pLookAtKeyFrame->setRotation( rotation);
//	rotation = m_pSkeleton->getBone(iHeadID)->getRotationAbsolute();
//	pLookAtKeyFrame->setTranslation( translation);
//	pLookAtKeyFrame->setTime( 0.0f);
//	pLookAtCoreTrackH->addCoreKeyframe( pLookAtKeyFrame);
//	//----------------------------------------первый кадр
//
//	if( mfDelay != 0.0){
//		pLookAtKeyFrame = new CalCoreKeyframe();
//		pLookAtKeyFrame->create();
//		translation = m_pSkeleton->getBone(iHeadID)->getTranslation();
//		
//		rotation = m_pSkeleton->getBone(iHeadID)->getRotation();
//		rotation.Normalize();
//		pLookAtKeyFrame->setRotation( rotation);
//		rotation = m_pSkeleton->getBone(iHeadID)->getRotationAbsolute();
//		pLookAtKeyFrame->setTranslation( translation);
//		pLookAtKeyFrame->setTime( mfDelay);
//		pLookAtCoreTrackH->addCoreKeyframe( pLookAtKeyFrame);
//	}
//
//	// Второй кадр
//	CalQuaternion		headRotation;
//	CalQuaternion		deltaRotation;
//	CalVector			lookTranslation;
//	CalVector			headDirection;
//	CalVector			normal;
//	float				angle;
//	float				frameTime;
//
//	// голова
//	pLookAtKeyFrame = new CalCoreKeyframe();
//	pLookAtKeyFrame->create();
//	pLookAtKeyFrame->setTranslation( translation);
//
//	// абсолютное 
//	if( params.m_pModel != NULL || params.bToCamera || params.bToPoint){
//		CalVector	headTranslation = m_pSkeleton->getBone(iHeadID)->getTranslationAbsolute();
//		headTranslation += vTranslation;
//		if( !params.bToCamera && !params.bToPoint){
//			// если смотрим не в камеру то вычисляем положение точки наблюдения
//			lookTranslation = params.m_pModel->m_pSkeleton->getBone(iLookAtHeadID)->getTranslationAbsolute();
//			lookTranslation += params.m_pModel->vTranslation;
//
//			D3DXVECTOR3	looktrn3( lookTranslation.x, lookTranslation.y, lookTranslation.z);
//			D3DXVECTOR4	looktrn4;
//			D3DXVec3Transform(&looktrn4, &looktrn3, (D3DXMATRIX*)params.m_TrgExtTrans);
//			looktrn4.x /= looktrn4.w;
//			looktrn4.y /= looktrn4.w;
//			looktrn4.z /= looktrn4.w;
//			lookTranslation = CalVector( looktrn4.x, looktrn4.y, looktrn4.z);
//
//			lookctrl.Enable( true);
//			//lookctrl.pLookBone = params.m_pModel->m_pSkeleton->getBone(iLookAtHeadID);
//			//lookctrl.pHeadBone = m_pSkeleton->getBone(iHeadID);
//			//lookctrl.pLookTrg = params.m_pModel;
//		}else{
//			lookctrl.Enable( false);
//			lookTranslation = params.camPos;
//			CalVector	vNeck = m_pSkeleton->getBone(iNeckID)->getTranslation();
//			lookTranslation.z -= vNeck.length();
//		}
//		// вычислим вектор направления взгляда
//		lookTranslation -= headTranslation;
//
//
//		// вычислим направление перпендикулярное шее
//		CalQuaternion	mRotation;
//		if( params.trTime == 1.0)
//			headDirection = m_pSkeleton->getDirection(iNeckID);	//!!!NEW!!!
//		else
//			headDirection = m_pSkeleton->getDirection(iHeadID);	//!!!NEW!!!
//		mRotation=qRotation;
//		mRotation.conjugate();		//??
//		headDirection *= mRotation;
//
//		// Учет коэффициента поворота
//		CalVector eyeVector = headDirection;
//		eyeVector *= lookTranslation.length()/eyeVector.length();
//		eyeVector.blend( params.trTime, lookTranslation);
//		//if( bToCamera)
//		//	eyeVector.z = lookTranslation.z;
//		lookTranslation = eyeVector;
//
//		headDirection = m_pSkeleton->getDirection(iNeckID);	//!!!NEW!!!
//		headDirection *= mRotation;
//
//		// ограничим поднятие и опускание головы
//		float fLookZ = lookTranslation.z;
//		if( fLookZ < 0) fLookZ *= -1;
//		float fDeltaZAngle = 0.0;
//		if( lookTranslation.length() != 0)
//			fDeltaZAngle = asin( fLookZ/lookTranslation.length());
//
//		// 25 - градусов максимальный угол для подъема головы
//		float fMaxUpAngle = 25.0f*3.14f/180.0f;
//		if( fDeltaZAngle > fMaxUpAngle)
//			lookTranslation.z *= fMaxUpAngle/fDeltaZAngle;
//
//		// 1 - рассчитаем все как-будто у нас голова смотрит прямо, а потом умножим на
//		//     conjugate( текущий поворот)
//
//		// Угол поворота
//		angle = headDirection.getAngle( lookTranslation);
//		//angle *= trTime;
//		//angle *= -1; 
//		frameTime = angle/1.57f*1.5;
//		if( frameTime < 0) frameTime *= -1;
//		if( frameTime < 0.45f)	frameTime = 0.45f;
//
//		// Оси поворота
//		normal = headDirection.getNormal( lookTranslation);
//		CalQuaternion		parentrotation;
//		parentrotation = m_pSkeleton->getBone(iNeckID)->getRotationAbsolute(); //(1)
//		parentrotation *= mRotation;
//		parentrotation.conjugate(); //(1)
//		normal *= parentrotation;
//
//		// переводим deltarotation в локальные координаты
//		// получаем абсолютное вращение родительской для головы кости
//		CalQuaternion	curRotation;
//		deltaRotation = CalQuaternion( normal, angle); //(1)
//		//rotation = m_pSkeleton->getBone(iHeadID)->getRotation(); //(1)
//		rotation = CalQuaternion(0, 0, 0, 1);	//!!!NEW!!!
//		deltaRotation.conjugate(); //(1)
//		curRotation = rotation; //(1)
//		rotation = deltaRotation; //(1)
//		rotation.Normalize(); //(1)
//
//		//rotation = deltaRotation; //(2)
//		//rotation.Normalize(); //(2)
//
//
//		CalVector		baseVector(0, 1, 0);
//		CalVector		scndVector(0, 1, 0);
//		CalVector		crntVector(0, 1, 0);
//
//		CalQuaternion	nullRtn(0, 0, 0, 1);
//		baseVector *= nullRtn;
//		scndVector *= rotation;
//		crntVector *= m_pSkeleton->getBone(iHeadID)->getRotation();
//
//		float fMaxAngle = 70.0f*3.14f/180.0f;
//		if( (baseVector.getAngle( scndVector) > 0 && baseVector.getAngle( scndVector) > fMaxAngle) || 
//			(baseVector.getAngle( scndVector) < 0 && baseVector.getAngle( scndVector) < -fMaxAngle)){
//				//??
//			/*// угол между текущим направлением и направлением прямо перед собой
//			float curAngRotation = crntVector.getAngle( baseVector);
//			// угол между текущим направлением и направлением до цели
//			float fullAngle = crntVector.getAngle( scndVector);*/
//			// угол между направлением прямо перед собой и направлением до цели
//			float addAngle = baseVector.getAngle( scndVector);
//
//			float div = 1.0;
//			// если не поворачиваем справа налево или наоборот
//			/*if( addAngle > fullAngle){
//				int iii=0;
//				//div = (fullAngle - (fMaxAngle - curAngRotation))/fullAngle;
//			}else{
//				int iii=0;
//				//div = (fMaxAngle + curAngRotation)/fullAngle;
//			}*/
//			div = fMaxAngle/addAngle;
//			if( div < 0) div *= -1;
//			rotation.w /= div;
//
//			angle *= div;
//			frameTime = angle/1.57f*1.5;
//			if( frameTime < 0) frameTime *= -1;
//			if( frameTime < 0.1f)
//				 frameTime = 0.1f;
//			deltaRotation = CalQuaternion( normal, angle);
//			deltaRotation.conjugate();
//			rotation = curRotation;
//
//			rotation = deltaRotation;
//			//rotation = deltaRotation;
//			rotation.Normalize();
//		}
//
//		// вычислим направление головы после поворота
//		CalQuaternion	currentRotationAbs =	m_pSkeleton->getBone(iNeckID)->getRotationAbsolute();
//		CalQuaternion	currentRotation =		rotation;
//		CalQuaternion	dirQuat;
//		CalVector dirVector(0, 1, 0);
//		CalVector upVector(1, 0, 0);
//		CalVector normVector;
//		CalVector normVector2d;
//
//		currentRotation *= currentRotationAbs;
//		currentRotation.Normalize();
//
//		dirVector *= currentRotation;
//		dirVector.normalize();
//		dirVector *= mRotation;
//
//		upVector *= currentRotation;
//		upVector.normalize();
//		upVector *= mRotation;
//
//		normVector = dirVector.getNormal( upVector);
//		normVector2d = normVector;
//
//		bool bInverse = false;
//		if( normVector2d.z < 0) bInverse = true;
//		normVector2d.z = 0;
//
//		float fSideAngle = normVector.getAngle( normVector2d);
//		if( bInverse)	fSideAngle *= -1;
//		CalVector		realDirection(0, 1, 0);
//		CalQuaternion	q1 = rotation;
//		realDirection *= q1;
//
//		realDirection.normalize();
//		realDirection.invert();
//		dirQuat = CalQuaternion(realDirection, fSideAngle);
//		dirQuat *= rotation;
//		rotation = dirQuat;
//		////////////////////////////////////////////////
//		rotation.Normalize();
//		pLookAtKeyFrame->setRotation( rotation);
//	}else{
//		CalQuaternion	mRotation;
//		CalVector		curDirection;
//		CalVector		newDirection;
//		curDirection = m_pSkeleton->getDirection(iHeadID);
//		newDirection = CalVector(0, 1, 0);
//		angle = curDirection.getAngle( newDirection);
//		//angle *= trTime;
//		frameTime = angle/1.57f*1.5;
//		if( frameTime < 0) frameTime *= -1;
//		if( frameTime < 0.1f)
//			 frameTime = 0.1f;
//		//pLookAtKeyFrame->setRotation( CalQuaternion(0, 0, 0, 1));
//		pLookAtKeyFrame->setRotation( getSkeleton()->getCoreSkeleton()->getCoreBone(iHeadID)->getRotation());
//	}
//
//	// проверка на заданное время
//	if( params.adwTime != 0)
//		frameTime = (float)params.adwTime/1000.0f;
//
//	pLookAtKeyFrame->setTime( frameTime+mfDelay);
//	pLookAtCoreAnimation->setDuration( frameTime+mfDelay);
//	pLookAtCoreTrackH->addCoreKeyframe( pLookAtKeyFrame);
//
//	//pLookAtCoreAnimation->setRelMode( false);
//
//	// TRACK для шеи, если смотрим перед собой
//	if( params.m_pModel == NULL && !params.bToCamera){
//		pLookAtCoreTrackH = new CalCoreTrack();
//		pLookAtCoreTrackH->create();
//		pLookAtCoreTrackH->setCoreBoneId(iNeckID);
//		pLookAtCoreAnimation->addCoreTrack( pLookAtCoreTrackH);
//
//		pLookAtCoreTrackH->addKeyframe( 
//			m_pSkeleton->getBone(iNeckID)->getTranslation(),
//			m_pSkeleton->getBone(iNeckID)->getRotation(),
//			0.0);
//
//		if( mfDelay != 0){
//			pLookAtCoreTrackH->addKeyframe( 
//				m_pSkeleton->getBone(iNeckID)->getTranslation(),
//				m_pSkeleton->getBone(iNeckID)->getRotation(),
//				mfDelay);
//		}
//
//		pLookAtCoreTrackH->addKeyframe( 
//			m_pSkeleton->getBone(iNeckID)->getTranslation(),
//			getSkeleton()->getCoreSkeleton()->getCoreBone(iNeckID)->getRotation(),
//			frameTime+mfDelay);
//
//		CalQuaternion	curNeckRtn = m_pSkeleton->getBone(iNeckID)->getRotation();
//		CalQuaternion	nextNeckRtn = getSkeleton()->getCoreSkeleton()->getCoreBone(iNeckID)->getRotation();
//		CalQuaternion	deltaNeckRtn = nextNeckRtn;//curNeckRtn;
//		deltaNeckRtn.conjugate();
//		deltaNeckRtn *= curNeckRtn;//nextNeckRtn;
//		//deltaNeckRtn.conjugate();
//
//		if( iLClavicleID != -1){
//			pLookAtCoreTrackH = new CalCoreTrack();
//			pLookAtCoreTrackH->create();
//			pLookAtCoreTrackH->setCoreBoneId(iLClavicleID);
//			pLookAtCoreAnimation->addCoreTrack( pLookAtCoreTrackH);
//
//			pLookAtCoreTrackH->addKeyframe( 
//				m_pSkeleton->getBone(iLClavicleID)->getTranslation(),
//				m_pSkeleton->getBone(iLClavicleID)->getRotation(),
//				0.0);
//
//			if( mfDelay != 0.0){
//				pLookAtCoreTrackH->addKeyframe( 
//					m_pSkeleton->getBone(iLClavicleID)->getTranslation(),
//					m_pSkeleton->getBone(iLClavicleID)->getRotation(),
//					mfDelay);
//			}
//
//			CalQuaternion	clavicleRotation = m_pSkeleton->getBone(iLClavicleID)->getRotation();
//			clavicleRotation *= deltaNeckRtn;
//			pLookAtCoreTrackH->addKeyframe( 
//				m_pSkeleton->getBone(iLClavicleID)->getTranslation(),
//				clavicleRotation,
//				frameTime+mfDelay);
//		}
//		if( iRClavicleID != -1){
//			pLookAtCoreTrackH = new CalCoreTrack();
//			pLookAtCoreTrackH->create();
//			pLookAtCoreTrackH->setCoreBoneId(iRClavicleID);
//			pLookAtCoreAnimation->addCoreTrack( pLookAtCoreTrackH);
//
//			pLookAtCoreTrackH->addKeyframe( 
//				m_pSkeleton->getBone(iRClavicleID)->getTranslation(),
//				m_pSkeleton->getBone(iRClavicleID)->getRotation(),
//				0.0);
//
//			if( mfDelay != 0.0){
//				pLookAtCoreTrackH->addKeyframe( 
//					m_pSkeleton->getBone(iRClavicleID)->getTranslation(),
//					m_pSkeleton->getBone(iRClavicleID)->getRotation(),
//					mfDelay);
//			}
//
//			CalQuaternion	clavicleRotation = m_pSkeleton->getBone(iRClavicleID)->getRotation();
//			clavicleRotation *= deltaNeckRtn;
//			pLookAtCoreTrackH->addKeyframe( 
//				m_pSkeleton->getBone(iRClavicleID)->getTranslation(),
//				clavicleRotation,
//				frameTime+mfDelay);
//		}
//	}
//
//	int	iCoreAnimID = m_pCoreModel->addCoreAnimation( pLookAtCoreAnimation);
//
//
//	//pLookAtCoreAnimation->setRelMode( true);
//
//	checkCoreAnimations();
//	m_pMixer->executeAction( iCoreAnimID, 0.0, 0.0);
//	return iCoreAnimID;
}

bool CalModel::contain(std::string& strBuffer){
	if( !m_pFace) return false;

	std::string filelist;
	filelist.append( m_pFace->header.pFileName);
	std::string::size_type pos = 0;

	while( true){
		pos = filelist.find_first_not_of(" \t", pos);
		std::string::size_type pos1 = filelist.find_first_of("&", pos);

		if( pos >= filelist.size())
			break;

		std::string strKey;
		strKey = filelist.substr(pos, pos1 - pos);
		pos += strKey.size();

		std::string strTextNameForChange;
		std::string strTextNameToChange;
		std::string::size_type kpos;
		std::string::size_type kpos1;
		kpos = strKey.find_first_not_of(" \t");
		kpos1 = strKey.find_first_of("|", kpos);
		strTextNameForChange = strKey.substr(kpos, kpos1 - kpos);
		kpos = kpos + 1;
		strTextNameToChange = strKey.substr(kpos1, strKey.size() - kpos1);
		// Если нашли имя текстуры
		if( strTextNameForChange.compare(strKey) == 0){
			return true;
		}
		return false;
	}
	return false;
}

/*char* CalModel::GetFaceTextureName()
{
	char* retstr;
	retstr=(char*)malloc(2);
	retstr[0]='\0';
	if( !m_pFace) return retstr;
	if( m_pFace->header.bFileName == 0) return retstr;
	
	std::string textname;
	std::string filelist;
	filelist.append( m_pFace->header.pFileName);
	std::string::size_type pos = 0;

	pos = filelist.find_first_not_of(" \t", pos);
	std::string::size_type pos1 = filelist.find_first_of("&", pos);

	if( pos >= filelist.size())
		return retstr;

	std::string strKey;
	strKey = filelist.substr(pos, pos1 - pos);
	pos += strKey.size();

	std::string strTextNameForChange;
	std::string strTextNameToChange;
	std::string::size_type kpos;
	std::string::size_type kpos1;
	kpos = strKey.find_first_not_of(" \t");
	kpos1 = strKey.find_first_of("|", kpos);
	strTextNameForChange = strKey.substr(kpos, kpos1 - kpos);
	free(retstr);

	retstr = (char*)malloc( strTextNameForChange .size()+1);
	ZeroMemory(retstr, strTextNameForChange .size()+1);
	memcpy( retstr, strTextNameForChange .data(), strTextNameForChange .size());
	return retstr;*/
	/*kpos = kpos + 1;
	strTextNameToChange = strKey.substr(kpos1, strKey.size() - kpos1);

	// Если нашли имя текстуры
	if( strTextNameForChange.compare(strKey) == 0){
		return true;
	}
	return false;*/
/*}

void CalModel::FreeFaceTextureName(char* apName){
	if(apName) free(apName);
}

char* CalModel::GetFaceNewTextureName()
{
	char* retstr;
	retstr=(char*)malloc(2);
	retstr[0]='\0';
	if( !m_pFace) return retstr;
	if( m_pFace->header.bFileName == 0) return retstr;

	std::string textname;
	std::string filelist;
	filelist.append( m_pFace->header.pFileName);
	std::string::size_type pos = 0;

	pos = filelist.find_first_not_of(" \t", pos);
	std::string::size_type pos1 = filelist.find_first_of("&", pos);

	if( pos >= filelist.size())
		return retstr;

	std::string strKey;
	strKey = filelist.substr(pos, pos1 - pos);
	pos += strKey.size();

	std::string strTextNameForChange;
	std::string strTextNameToChange;
	std::string::size_type kpos;
	std::string::size_type kpos1;
	kpos = strKey.find_first_not_of(" \t");
	kpos1 = strKey.find_first_of("|", kpos);
	strTextNameForChange = strKey.substr(kpos, kpos1 - kpos);

	kpos1 = kpos1 + 1;
	strTextNameToChange = strKey.substr(kpos1, strKey.size() - kpos1);
	free(retstr);
	
	retstr = (char*)malloc( strTextNameToChange.size()+1);
	ZeroMemory(retstr, strTextNameToChange.size()+1);
	memcpy( retstr, strTextNameToChange.data(), strTextNameToChange.size());
	return retstr;
}

void CalModel::FreeFaceNewTextureName(char* apName){
	if(apName) free(apName);
}*/

int CalModel::getVertCount()
{
	if( !m_pRenderer)	return 0;
	return m_pRenderer->getVertCountInModel();
}

void CalModel::attachToModel(CalModel *pOwner, LPSTR name, CalVector trn, CalQuaternion rtn)
{
	if( !m_pSkeleton)	return;
	if( !pOwner)
	{
		if( !m_pOwner)	return;
		CalBone*	frame = m_pSkeleton->getBone(Cal::PROJECTION_BONE_ID);
		if( frame)
		{
			m_pMixer->attachTo(NULL, frame);
			if( m_pOwner && m_pOwner->m_pfExternTransform)
			{
				CalVector		parentTrn;
				CalQuaternion	parentRtn;
				m_pOwner->m_pfExternTransform(parentTrn, parentRtn, m_pOwner->m_pExternClass);
				//
				CalVector translation = frame->getTranslationAbsolute();
				CalQuaternion rotation = frame->getRotationAbsolute();
				rotation *= parentRtn;
				translation *= parentRtn;
				translation += parentTrn;
				frame->clearState();
				frame->blendState(1.0f, translation, rotation);
				frame->saveState();
				relative_pos_changed = false;
			}
		}
		m_pOwner = NULL;
		return;
	}
	m_pOwner = pOwner;
	CalSkeleton*	parentSkel = pOwner->getSkeleton();
	Cal3DComString boneName = Cal3DComString(name);
	if( !parentSkel || !parentSkel->getCoreSkeleton()->containBone(boneName))	return;
	int	boneId = parentSkel->getCoreSkeleton()->getCoreBoneIdSubName(boneName);
	CalBone*	bone = parentSkel->getBone(boneId);
	if( !bone)	return;

	CalVector		atrn = bone->getTranslationAbsolute(), trnAbs=atrn;
	CalQuaternion	aquat = bone->getRotationAbsolute(), rtnAbs=aquat;

CalVector	savetrn = atrn;
CalQuaternion	savertn = aquat;

	CalVector		parentTrn;
	CalQuaternion	parentRtn;
	if( pOwner->m_pfExternTransform)
	{
		pOwner->m_pfExternTransform(parentTrn, parentRtn, pOwner->m_pExternClass);
		aquat *= parentRtn;
		atrn *= parentRtn;
		atrn += parentTrn;
	}

	bone->clearState();
	bone->blendState(1, atrn, aquat);

	CalVector tr = m_pSkeleton->get_frame()->getTranslationAbsolute();
	CalQuaternion rt = m_pSkeleton->get_frame()->getRotationAbsolute();
	if( m_pfExternTransform)
	{
		m_pfExternTransform(tr, rt, m_pExternClass);
		m_pSkeleton->set_frame_move(tr, rt);
	}
	m_pMixer->attachTo(bone, m_pSkeleton->get_frame());
	bone->clearState();
	bone->blendState(1, savetrn, savertn);
}

void CalModel::detachModel()
{

}

bool CalModel::getBoundingBox(CalVector *max, CalVector *min)
{
	if( !getRenderer())	return false;

	if (bboxIsRight) {
		*max = bboxMax;
		*min = bboxMin;
		return true;
	}

	LPDIRECT3DVERTEXBUFFER8	puntrVB		= NULL;
	LPDIRECT3DVERTEXBUFFER8	ptrVB		= NULL;
	CalRenderer*			pCalRenderer= NULL;
	float*					unptrVert	= NULL;
	DWORD					vertexCount = 0;
	int						fstride=stride / sizeof(float);


	pCalRenderer = getRenderer();
	//vertexCount = getVertCount();
	if( pCalRenderer->lockVertices())
	{
		int meshId;
		for(meshId = 0; meshId < pCalRenderer->getMeshCount(); meshId++)
		{

			for(int submeshId = 0; submeshId < pCalRenderer->getSubmeshCount(meshId); submeshId++)
			{
				if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
				{
					float*	meshVertices = NULL;
					vertexCount = pCalRenderer->m_pSelectedSubmesh->getVertexCount();//pCalRenderer->getVertList(&meshVertices);
					unptrVert = (float*)pCalRenderer->get_vertices(pCalRenderer->m_pSelectedSubmesh);
					//pCalRenderer->getVBs(&ptrVB, &puntrVB);
					//if( SUCCEEDED( puntrVB->Lock( 0, stride*vertexCount, (BYTE**)&unptrVert, D3DLOCK_READONLY) ) ) {
						D3DVERTEX*	pvert = (D3DVERTEX*)unptrVert;
						if( meshId == 0 && submeshId == 0)
						{
								bboxMax.x = pvert->x;
								bboxMin.x = pvert->x;
								bboxMax.y = pvert->y;
								bboxMin.y = pvert->y;
								bboxMax.z = pvert->z;
								bboxMin.z = pvert->z;
						}
						for( int i=1; i < vertexCount; i++)
						{
							D3DVERTEX*	pvert = (D3DVERTEX*)unptrVert;
							if( bboxMin.x > pvert->x) bboxMin.x = pvert->x;
							if( bboxMin.y > pvert->y) bboxMin.y = pvert->y;
							if( bboxMin.z > pvert->z) bboxMin.z = pvert->z;

							if( bboxMax.x < pvert->x) bboxMax.x = pvert->x;
							if( bboxMax.y < pvert->y) bboxMax.y = pvert->y;
							if( bboxMax.z < pvert->z) bboxMax.z = pvert->z;

							unptrVert += fstride;
						}
					//	puntrVB->Unlock();
					//}
				}
			}
		}
		pCalRenderer->releaseVertices();
	}
	else	return false;

	D3DXVECTOR3			boundBox[8];
	D3DXVECTOR4			boundBoxTransform[8];
	D3DXMATRIX			matrix;
	int i;
	for(i=0; i < 8; i++){
		boundBox[i].x = bboxMin.x;
		boundBox[i].y = bboxMin.y;
		if( i < 4)
			boundBox[i].z = bboxMin.z;
		else
			boundBox[i].z = bboxMax.z;
	}
	boundBox[1].y = bboxMax.y;
	boundBox[2].x = bboxMax.x;	boundBox[2].y = bboxMax.y;
	boundBox[3].x = bboxMax.x;

	boundBox[4].x = bboxMin.x;	boundBox[5].y = bboxMin.y;
	boundBox[5].x = bboxMax.x;
	boundBox[7].y = bboxMin.y;

	getTransMatrix( (D3DMATRIX*)&matrix);
	for( i=0; i < 8; i++){
		D3DXVec3Transform( &boundBoxTransform[i], &boundBox[i], &matrix);
	}

	bboxMin.x = bboxMax.x = boundBoxTransform[0].x;
	bboxMin.y = bboxMax.y = boundBoxTransform[0].y;
	bboxMin.z = bboxMax.z = boundBoxTransform[0].z;

	for( i=1; i < 8; i++){
		if( bboxMin.x > boundBoxTransform[i].x) bboxMin.x = boundBoxTransform[i].x;
		if( bboxMin.y > boundBoxTransform[i].y) bboxMin.y = boundBoxTransform[i].y;
		if( bboxMin.z > boundBoxTransform[i].z) bboxMin.z = boundBoxTransform[i].z;

		if( bboxMax.x < boundBoxTransform[i].x) bboxMax.x = boundBoxTransform[i].x;
		if( bboxMax.y < boundBoxTransform[i].y) bboxMax.y = boundBoxTransform[i].y;
		if( bboxMax.z < boundBoxTransform[i].z) bboxMax.z = boundBoxTransform[i].z;
	}
	//::MessageBox( NULL, "bool CalModel::getBoundingBox(CalVector *max, CalVector *min)", "Not released", MB_OK);
	if(min)	*min = bboxMin;
	if(max)	*max = bboxMax;
	bboxIsRight = true;
	return true;
}

void alignBBox(CalVector& aboxMin, CalVector& aboxMax, D3DMATRIX* matrix)
{
	D3DXVECTOR3			boundBox[8];
	D3DXVECTOR4			boundBoxTransform[8];
	int i;
	for(i=0; i < 8; i++){
		boundBox[i].x = aboxMin.x;
		boundBox[i].y = aboxMin.y;
		if( i < 4)	boundBox[i].z = aboxMin.z;
		else	boundBox[i].z = aboxMax.z;
	}
	boundBox[1].y = aboxMax.y;
	boundBox[2].x = aboxMax.x;	boundBox[2].y = aboxMax.y;
	boundBox[3].x = aboxMax.x;

	boundBox[4].x = aboxMin.x;	boundBox[5].y = aboxMin.y;
	boundBox[5].x = aboxMax.x;
	boundBox[7].y = aboxMin.y;

	for( i=0; i < 8; i++){
		D3DXVec3Transform( &boundBoxTransform[i], &boundBox[i], (D3DXMATRIX*)matrix);
	}

	aboxMin.x = aboxMax.x = boundBoxTransform[0].x;
	aboxMin.y = aboxMax.y = boundBoxTransform[0].y;
	aboxMin.z = aboxMax.z = boundBoxTransform[0].z;

	for( i=1; i < 8; i++){
		if( aboxMin.x > boundBoxTransform[i].x) aboxMin.x = boundBoxTransform[i].x;
		if( aboxMin.y > boundBoxTransform[i].y) aboxMin.y = boundBoxTransform[i].y;
		if( aboxMin.z > boundBoxTransform[i].z) aboxMin.z = boundBoxTransform[i].z;

		if( aboxMax.x < boundBoxTransform[i].x) aboxMax.x = boundBoxTransform[i].x;
		if( aboxMax.y < boundBoxTransform[i].y) aboxMax.y = boundBoxTransform[i].y;
		if( aboxMax.z < boundBoxTransform[i].z) aboxMax.z = boundBoxTransform[i].z;
	}
}

bool CalModel::getBBoxes(std::vector<CalVector>& vmax, std::vector<CalVector>& vmin)
{
	LPDIRECT3DVERTEXBUFFER8	puntrVB		= NULL;
	LPDIRECT3DVERTEXBUFFER8	ptrVB		= NULL;
	CalRenderer*			pCalRenderer= NULL;
	float*					unptrVert	= NULL;
	DWORD					vertexCount = 0;
	int						fstride=stride / sizeof(float);
	CalVector				aboxMax, aboxMin;


	pCalRenderer = getRenderer();
	//vertexCount = getVertCount();
	if( pCalRenderer->lockVertices())
	{
		int meshId;
		for(meshId = 0; meshId < pCalRenderer->getMeshCount(); meshId++)
		{

			for(int submeshId = 0; submeshId < pCalRenderer->getSubmeshCount(meshId); submeshId++)
			{
				if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
				{
					float*	meshVertices = NULL;
					vertexCount = pCalRenderer->m_pSelectedSubmesh->getVertexCount();//pCalRenderer->getVertList(&meshVertices);
					unptrVert = (float*)pCalRenderer->get_vertices(pCalRenderer->m_pSelectedSubmesh);
					D3DVERTEX*	pvert = (D3DVERTEX*)unptrVert;

					aboxMax.x = aboxMin.x = pvert->x;
					aboxMax.y = aboxMin.y = pvert->y;
					aboxMax.z = aboxMin.z = pvert->z;

					for( int i=1; i < vertexCount; i++)
					{
						D3DVERTEX*	pvert = (D3DVERTEX*)unptrVert;
						if( aboxMin.x > pvert->x) aboxMin.x = pvert->x;
						if( aboxMin.y > pvert->y) aboxMin.y = pvert->y;
						if( aboxMin.z > pvert->z) aboxMin.z = pvert->z;

						if( aboxMax.x < pvert->x) aboxMax.x = pvert->x;
						if( aboxMax.y < pvert->y) aboxMax.y = pvert->y;
						if( aboxMax.z < pvert->z) aboxMax.z = pvert->z;

						unptrVert += fstride;
					}
					//
					D3DMATRIX matrix;
					getTransMatrix( &matrix);
					alignBBox(aboxMin, aboxMax, &matrix);
					vmin.push_back(aboxMin);
					vmax.push_back(aboxMax);
				}
			}
		}
		pCalRenderer->releaseVertices();
	}
	else	return false;

	return true;
}

void CalModel::move3d(float x, float y, float z)
{
	CalVector	curPos = getRootPosition();
	CALMOVETYPE eCurrentMoveType = eMoveType;
	eMoveType = CMT_ABSOLUTE;

	vTranslation.x += x-curPos.x;
	vTranslation.y += y-curPos.y;
	vTranslation.z += z-curPos.z;
	eMoveType = eCurrentMoveType;
}

/*int CalModel::getFaceCount()
{
	return m_pRenderer->getFaceCountInModel();
}*/

void CalModel::scale3d(float x, float y, float z)
{
	if( x <= 0 || y <= 0 || z <= 0) return;
	vScale3D = CalVector(x, y, z);
	m_bLocalTrans = true;
}

void CalModel::rotate3d(CalVector axes, float angle, CALOPTYPE type)
{
	Rotate(axes, angle, type);
	m_bLocalTrans = true;
}

CalVector CalModel::transformVector(CalVector v)
{
	D3DMATRIX	matrix;
	D3DXVECTOR3  d3dxv(v.x, v.y, v.z);
	D3DXVECTOR4  d3dxv4;

	getTransMatrix( &matrix);
	D3DXVec3Transform(&d3dxv4, &d3dxv, (D3DXMATRIX*)&matrix);
	d3dxv4.x /= d3dxv4.w; d3dxv4.y /= d3dxv4.w; d3dxv4.z /= d3dxv4.w;

	return CalVector( d3dxv4.x, d3dxv4.y, d3dxv4.z);
}

bool CalModel::NoSkel(bool abSetNoSkel)
{
	if( abSetNoSkel)
		m_bNoSkel = true;
	return m_bNoSkel;
}

//void CalModel::PointAt(float x, float y, float z, DWORD hand, DWORD type)
void CalModel::PointAt(POINTATPARAMS params)
{
	Cal3DComString sprefix;	// префикс имени кости в зависимости от руки

	CalCoreSkeleton	*pcs = m_pSkeleton->getCoreSkeleton();	// базовый скелет

	if( params.m_dwHand == RIGHTHAND){ sprefix = "r ";}else{ sprefix = "l ";}

	if( !( params.m_dwHand == pointctrl.initHand)){	// инициализация номеров

		{Cal3DComString name; name += sprefix; name += "Clavicle";
		if(pcs->containBone(name))
			pointctrl.idClavicle = pcs->getCoreBoneIdSubName( name);
		}
		{Cal3DComString name; name += sprefix; name += "UpperArm";
		if(pcs->containBone(name))
			pointctrl.idUpper = pcs->getCoreBoneIdSubName( name);
		}
		{Cal3DComString name; name += sprefix; name += "Forearm";
		if(pcs->containBone(name))
			pointctrl.idLower = pcs->getCoreBoneIdSubName( name);
		}
		{Cal3DComString name; name += sprefix; name += "Hand";
		if(pcs->containBone(name))
			pointctrl.idHand = pcs->getCoreBoneIdSubName( name);
		}
		pointctrl.initHand = params.m_dwHand;
	}
	if( (params.pointflags == PF_POINT)||( params.pointflags == PF_CAMERA))
		pointctrl.target = params.m_Point;//CalVector( x, y, z);
	else
		if( params.pointflags == PF_TARGET)
			pointctrl.p_target = (CalModel*)params.m_pTarget;

	pointctrl.multiplier = params.multiplier;
	pointctrl.bEnable = true;
}

CalVector CalModel::getDirectionAbsolute(DWORD id, CalVector axes)
{

	CalQuaternion mrotation;
	CalVector	  result;
	result = m_pSkeleton->getDirection(id, axes);	//!!!NEW!!!
	mrotation=qRotation;
	mrotation.conjugate();		//??
	result *= mrotation;

	return result;
}

bool CalModel::updatePointCtrl( float deltaTime)
{
	if( !pointctrl.bEnable) return false;
	//if( pointctrl.bEnable) return false;
	//pointctrl.bEnable = true;

	CalCoreSkeleton	*pcs = m_pSkeleton->getCoreSkeleton();	// базовый скелет
	CalVector		handRoot;				// точка начала плечевой кости, вокруг которой вращается рука
	CalVector		vpoint;					// вектор показа
	CalVector		currentdirection;		// текущее направление руки
	CalVector		uppernormal;			// нормаль для поворота руки
	float			upperangle;				// угол поворота плечевой кости

	handRoot = m_pSkeleton->getBone(pointctrl.idUpper)->getTranslationAbsolute();	// получим точку поворота руки
	handRoot += vTranslation;

	if( pointctrl.p_target!=NULL/* && */)
		vpoint = pointctrl.p_target->getRootPosition( true);
	else		
		vpoint = pointctrl.target;

	// translate vpoint into local coordinates
	if (pmExternTransform)
	{
		D3DXMATRIX	im;
		D3DXMatrixInverse(&im, NULL, (D3DXMATRIX*)pmExternTransform);
		vpoint.transform(&im);
	}

	vpoint -= handRoot;

	// вычислим текущее направление руки
	currentdirection = getDirectionAbsolute( pointctrl.idUpper, CalVector(1, 0, 0));

	float m_fPart = 1.0f;

	// угол и нормаль для поворота руки по направлению
	upperangle = currentdirection.getAngle( vpoint);
	uppernormal = currentdirection.getNormal( vpoint);

	float maxanglebytime = deltaTime*1.57f*pointctrl.multiplier;

	if( upperangle > maxanglebytime + pointctrl.accWeight){
		upperangle = maxanglebytime + pointctrl.accWeight;
		m_fPart = (maxanglebytime + pointctrl.accWeight)/upperangle;
		pointctrl.accWeight += upperangle;
	}else{
		pointctrl.bEnable = false;
	}

	// новый вариант
	/*uppernormal.normalize();
	upperangle *= m_fPart;
	CalQuaternion	_resuprtn(uppernormal, upperangle);
	_resuprtn.Normalize();
	CalQuaternion	_uprtn = m_pSkeleton->getBone(pointctrl.idUpper)->getRotation();
	_uprtn *= _resuprtn;	// вращение руки в родительских координатах


	CalQuaternion qqq = m_pSkeleton->getBone(pointctrl.idClavicle)->getRotation();
	qqq *= qRotation;
	_uprtn.conjugate();
	_uprtn *= qqq;*/
	////

	// переводим вектор нормали из глобальных координат в локальные координаты плеча
	CalQuaternion		parentrotation;
	CalQuaternion		mRotation;
	mRotation = qRotation;
	mRotation.conjugate();
	parentrotation = m_pSkeleton->getBone(pointctrl.idClavicle)->getRotationAbsolute(); //(1)
	parentrotation *= mRotation;
	parentrotation.conjugate();
	uppernormal *= parentrotation;

	uppernormal.normalize();
	upperangle *= m_fPart;
	CalQuaternion	resuprtn(uppernormal, upperangle);
	resuprtn.Normalize();

	CalQuaternion	uprtn = m_pSkeleton->getBone(pointctrl.idUpper)->getRotation();
	uprtn *= resuprtn;	// вращение руки в родительских координатах

	CalQuaternion _res1 = uprtn; _res1.conjugate();
	CalQuaternion _res2 = _res1; _res2 *= m_pSkeleton->getBone(pointctrl.idUpper)->getRotation();
	CalQuaternion _res3 = m_pSkeleton->getBone(pointctrl.idUpper)->getRotation(); _res3 *= _res2;
	uprtn = _res3;

	// x*bone_rotaton = a
	// x - вращение родительской кости
	// bone_rotation*((conjugate a)*bone_rotation)

	CalBone*	pBone = m_pSkeleton->getBone(pointctrl.idUpper);
	pBone->clearState();
	pBone->blendState( 1.0, m_pSkeleton->getBone(pointctrl.idUpper)->getTranslation(), uprtn.Normalize());
	pBone->lockState();
	pBone->calculateState();

	return true;
}

 /*****************************************************************************/
/** Construct matrix to bring model to coord center.
  * 
  *****************************************************************************/
void CalModel::getCenterMatrix(D3DXMATRIX *pm)
{
	CalVector root;

	if( m_pOwner != NULL){
		//assert(false);
		//root = m_pOwner->getSkeleton()->getRootPoint( true);
		root = getSkeleton()->getRootPoint( true);
	}else{
		root = getSkeleton()->getRootPoint( true);
	}

	D3DXMatrixTranslation(pm, -root.x, -root.y, -root.z);
}

 /*****************************************************************************/
/** Construct matrix to bring model to coord center.
  * 
  *****************************************************************************/
void CalModel::getRestoreMatrix(D3DXMATRIX *pm)
{
	CalVector root;

	if( m_pOwner != NULL){
		//assert(false);
		root = getSkeleton()->getRootPoint( true);
	}else{
		root = getSkeleton()->getRootPoint( true);
	}

	D3DXMatrixTranslation(pm, root.x, root.y, root.z);
}

 /*****************************************************************************/
/** Construct matrix to aplly moving transforms to model.
  * 
  *****************************************************************************/
void CalModel::getMovingMatrix(D3DXMATRIX *pm)
{
	D3DXMATRIX	rtn, trn;

	CalQuaternion	qlocalquat	= qRotation;
	D3DXQUATERNION	localrootquat = D3DXQUATERNION(qlocalquat.x, qlocalquat.y, qlocalquat.z, qlocalquat.w);

	D3DXMatrixRotationQuaternion((D3DXMATRIX*)&rtn, &localrootquat);
	D3DXMatrixTranslation((D3DXMATRIX*)&trn, vTranslation.x, vTranslation.y, vTranslation.z);

	MULTMATRIX( pm, &rtn, &trn);
}

 /*****************************************************************************/
/** Construct matrix to aplly transformations
  * 
  *****************************************************************************/
void CalModel::getTransformMatrix(D3DXMATRIX *pm)
{
	D3DXMatrixScaling(pm, vScale3D.x * m_Scale, vScale3D.y * m_Scale, vScale3D.z * m_Scale);
}



bool CalModel::WasMoved()
{
	if( vTranslation != CalVector(0,0,0))	return true;
	CalQuaternion	qZero(CalVector(0, 0, 1), 0);
	if( qRotation.x!=qZero.x||qRotation.y!=qZero.y||qRotation.z!=qZero.z||qRotation.w!=qZero.w)	return true;
	return false;
}

void CalModel::setFloor(float floor)
{
	m_floorLevel = floor;
}

void CalModel::adjustBoneLock(CalAnimation*	anim)
{
	  CalVector	currentPosition = m_pSkeleton->getBone(
							m_pSkeleton->getCoreSkeleton()->getCoreBoneIdSubName( 
								anim->getCoreAnimation()->m_strLock
							))->getTranslationAbsolute();
	  currentPosition = transformVector( currentPosition);
	  CalVector	requiredPosition = anim->m_vLock;

	  requiredPosition -= currentPosition;
	  vTranslation += requiredPosition;
}

void CalModel::stockFloor()
{
	return;
	if( m_floorLevel == FLT_MAX)	return;
	//
	float	bonesFloor = m_pSkeleton->getFloor();

	if( bonesFloor == FLT_MAX)	return;

	vTranslation.z += ( m_floorLevel - (bonesFloor + vTranslation.z));
}

bool CalModel::setFAP(CMpeg4FDP *am_pFAPU)
{
	if( !am_pFAPU)
	{
		m_pFAPU = NULL;
		return true;
	}
	if( am_pFAPU->initialize(this))
	{
		m_pFAPU = am_pFAPU;
		m_pFAPU->calculate_FAPU( this);
	}
	return true;
}

bool CalModel::get_bbox_by_bones(CalVector *max, CalVector *min, std::vector<Cal3DComString>& names)
{
	if ( getPhysique()->pHitBoxes == NULL)	return false;
	getPhysique()->get_bbox_by_hitbs(max, min, names);
	return	true;
}

void CalModel::insert_EmPack(Cal3DComString name, FACEExpression **vector, int count)
{
	EM_PACK	pack;
	pack.emotions = vector;
	pack.count = count;
	m_mapEmotions.insert(std::make_pair(name, pack));
}

void CalModel::generateEdges()
{
	if( edges)	return;
	edges = true;

	CalRenderer*			pCalRenderer= NULL;
	pCalRenderer = getRenderer();
	//vertexCount = getVertCount();
	int meshId;
	for(meshId = 0; meshId < pCalRenderer->getMeshCount(); meshId++)
	{

		for(int submeshId = 0; submeshId < pCalRenderer->getSubmeshCount(meshId); submeshId++)
		{
			if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
			{
				pCalRenderer->m_pSelectedSubmesh->getCoreSubmesh()->generate_edges();
			}
		}
	}
}

bool CalModel::setExtertBuffers(LPVOID vb, LPVOID ib)
{
	if( !externBuffers)	return false;
	getRenderer()->pVertexBuffer = (LPDIRECT3DVERTEXBUFFER8)vb;
	getRenderer()->pIndexBuffer = (LPDIRECT3DINDEXBUFFER8)ib;
	return true;
}

void CalModel::useExternTransform(D3DXMATRIX *pm)
{
	pmExternTransform = pm;
}

bool CalModel::natura3d_pos_changed(CalVector &pos, CalQuaternion &quat, bool& relative)
{
	if (!unbinded_self_skeleton &&
		m_pSkeleton && m_pSkeleton->m_bFrameMoved)
	{
		relative = relative_pos_changed;
		relative_pos_changed = true;
		m_pSkeleton->m_bFrameMoved = false;
		// Absolutely position
		/*
		quat = m_pSkeleton->get_frame()->getRotation();
		pos = m_pSkeleton->get_frame()->getTranslation();
		*/
		// Relatively position
		if( m_pSkeleton->get_frame_move(pos, quat, relative))
			return true;
	}
	return false;
}

float CalModel::get_bip_height()
{
	if(__is_character)
	{
		CalVector	bipPos = m_pSkeleton->getRootPoint();
		return bipPos.z - bboxMin.z;
	}
	return 0.0;//bipPos.z;
}

void	CalModel::get_worldTranslation(CalVector& trn)
{
	getCoreModel()->get_worldTranslation(trn);
}

void	CalModel::get_worldRotation(CalQuaternion& rtn)
{
	getCoreModel()->get_worldRotation(rtn);
}

int CalModel::getFaceCount(){ return m_pRenderer?m_pRenderer->getFaceCountInModel():0;};
int CalModel::getEdgesCount(){ return m_pRenderer?m_pRenderer->getEdgesCountInModel():0;};

void CalModel::bindSkeleton(CalSkeleton* apSkeleton)
{
	unbindSkeleton();
	unbinded_self_skeleton = m_pSkeleton;
	m_pSkeleton = apSkeleton;
}

void CalModel::unbindSkeleton()
{
	if( unbinded_self_skeleton)
		m_pSkeleton = unbinded_self_skeleton;
	unbinded_self_skeleton = NULL;
}