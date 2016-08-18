//****************************************************************************//
// model.cpp                                                                  //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#include "StdAfx.h"
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
#include "vu_mixer.h"
#include "renderer.h"
#include "coremodel.h"
#include "coreskeleton.h"
#include "coremesh.h"
#include "coresubmesh.h"
#include "mesh.h"
#include "physique.h"
#include "springsystem.h"
//#include "stepctrl.h"
#include "facedesc.h"
//#include "d3dxmath.h"
#include "submesh.h"
#include "coretrack.h"
#include "corekeyframe.h"
#include "coreanimation.h"
#include "corebone.h"
#include "calanimationmorph.h"
#include "animation_action.h"
#include "animation_cycle.h"
#include "memory_leak.h"
#include "PointController.h"
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
		sprintf_s( buf, str);
	else
		if( w1==FLT_MAX)
			sprintf_s( buf, str, w1);
		else
			sprintf_s( buf, str, w1, w2);
	//OutputDebugString( buf);
}


CalModel::CalModel(): 
	m_lookController(NULL),
	m_visemeController(NULL),
	MP_VECTOR_INIT(actiontype),
	MP_VECTOR_INIT(actionid),
	MP_VECTOR_INIT(m_vectorMesh),
	MP_VECTOR_INIT(binded_models),
	MP_MAP_INIT(m_mapEmotions)
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
  m_pFace = NULL;
  m_pointController = NULL;
  dwLex1 = dwLex2 = 0;
  fFacePhase = 0.0f;
  bLastRotPointDef = false;
  m_bStepDone = false;
  m_Scale = 1;
  m_pOwner = NULL;
  m_pSlaver = NULL;
  m_bLocalTrans = false;
  m_pFAPU = NULL;
  bboxIsRight = false;
  m_pfExternTransform = false;
  relative_pos_changed = true;
  __is_character = false;
  m_bNeedUpdateWhenVisible = false;
  pmExternTransform	= NULL;
  m_bNoSkel = false;
  fvf = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
  stride = sizeof(D3DVERTEX);
  externBuffers = false;
  m_bPrevModelVisible = false;
  unbinded_self_skeleton = NULL;
  binded_target_model = NULL;
  m_bFrozen = false;
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
	  MP_DELETE( m_pFAPU;*/
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

void CalModel::deleteAnimations(CalCoreAnimation* coreAnimation)
{
	CalMixer* pMixer = getMixer();
	if (!pMixer)
	{
		return;
	}

	std::vector<CalAnimation *>::iterator it = pMixer->m_vectorAnimation.begin();
	std::vector<CalAnimation *>::iterator itEnd = pMixer->m_vectorAnimation.end();

	for ( ; it != itEnd; it++)
	if (*it)
	if ((*it)->getCoreAnimation() == coreAnimation)
	{
		(*it)->destroy();
		//MP_DELETE( *it);
		*it = NULL;
	}

	{
		std::list<CalAnimationAction *>::iterator iter = pMixer->m_listAnimationAction.begin();

		for ( ; iter != pMixer->m_listAnimationAction.end(); iter++)
		if (*iter)
		if ((*iter)->getCoreAnimation() == coreAnimation)
		{
			(*iter)->destroy();
			(*iter)->release();
			pMixer->m_listAnimationAction.erase(iter);
			iter = pMixer->m_listAnimationAction.begin();
		}
	}

	{
		std::list<CalAnimationCycle *>::iterator iter = pMixer->m_listAnimationCycle.begin();
		
		for ( ; iter != pMixer->m_listAnimationCycle.end(); iter++)
		if (*iter)
		if ((*iter)->getCoreAnimation() == coreAnimation)
		{
			(*iter)->destroy();
			MP_DELETE( *iter);
			pMixer->m_listAnimationCycle.erase(iter);
			iter = pMixer->m_listAnimationCycle.begin();
		}
	}

	{
		std::list<CalAnimationMorph *>::iterator iter = pMixer->m_listAnimationMorph.begin();

		for ( ; iter != pMixer->m_listAnimationMorph.end(); iter++)
		if (*iter)
		if ((*iter)->getCoreAnimation() == coreAnimation)
		{
			(*iter)->destroy();
			MP_DELETE( *iter);
			pMixer->m_listAnimationMorph.erase(iter);
			iter = pMixer->m_listAnimationMorph.begin();
		}
	}
}

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
  CalMesh *pMesh = MP_NEW( CalMesh);
  if(pMesh == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return false;
  }

  // create the new mesh instance
  if(!pMesh->create(pCoreMesh))
  {
    MP_DELETE( pMesh);
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
  m_bPrevModelVisible = false;
  if(pCoreModel == 0)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  m_pCoreModel = pCoreModel;

  // allocate a new skeleton instance
  if( m_pCoreModel->getCoreSkeleton() != NULL){
	  CalSkeleton *pSkeleton = MP_NEW( CalSkeleton);
	  if(pSkeleton == 0)
	  {
		CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
		return false;
	  }

	  // create the skeleton from the core skeleton
	  if(!pSkeleton->create(pCoreModel->getCoreSkeleton()))
	  {
		MP_DELETE( pSkeleton);
		return false;
	  }

	  m_pSkeleton = pSkeleton;
	  m_pSkeleton->setUp();

	  if(pSkeleton->getCoreSkeleton()->containBone("Bip") ||
		pSkeleton->getCoreSkeleton()->containBone("bip"))
			__is_character = true;
  }else NoSkel( true);

  // allocate a new mixer instance
  /*if( is_character())
	pMixer = new CalCharMixer();
  else
	pMixer = new CalMixer();*/

  CalMixer *pMixer = MP_NEW( CalVUMixer);

  if(pMixer == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    m_pSkeleton->destroy();
	if( m_pSkeleton) MP_DELETE( m_pSkeleton);
    return false;
  }

  // create the mixer from this model
  if(!pMixer->create(this)) 
  {
	  if( m_pSkeleton){
		m_pSkeleton->destroy();
		MP_DELETE( m_pSkeleton);
	  }
    MP_DELETE( pMixer);
    return false;
  }

  m_pMixer = pMixer;
  //m_pMixer->m_pModel = this;

  // allocate a new physqiue instance
  CalPhysique *pPhysique = MP_NEW( CalPhysique);
  if(pPhysique == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    m_pMixer->destroy();
    MP_DELETE( m_pMixer);
	if( m_pSkeleton){
		m_pSkeleton->destroy();
		MP_DELETE( m_pSkeleton);
	}
    return false;
  }

  // create the physique from this model
  if(!pPhysique->create(this))
  {
    m_pMixer->destroy();
    MP_DELETE( m_pMixer);
	if( m_pSkeleton){
		m_pSkeleton->destroy();
		MP_DELETE( m_pSkeleton);
	}
    MP_DELETE( pPhysique);
    return false;
  }

  m_pPhysique = pPhysique;

  // allocate a new spring system instance
  CalSpringSystem *pSpringSystem = MP_NEW( CalSpringSystem);
  if(pSpringSystem == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    m_pPhysique->destroy();
    MP_DELETE( m_pPhysique);
    m_pMixer->destroy();
    MP_DELETE( m_pMixer);
	if( m_pSkeleton){
		m_pSkeleton->destroy();
		MP_DELETE( m_pSkeleton);
	}
    return false;
  }

  // create the spring system from this model
  if(!pSpringSystem->create(this))
  {
    m_pPhysique->destroy();
    MP_DELETE( m_pPhysique);
    m_pMixer->destroy();
    MP_DELETE( m_pMixer);
	if( m_pSkeleton){
		m_pSkeleton->destroy();
		MP_DELETE( m_pSkeleton);
	}
    MP_DELETE( pSpringSystem);
    return false;
  }

  m_pSpringSystem = pSpringSystem;

  // allocate a new renderer instance
  CalRenderer *pRenderer = MP_NEW( CalRenderer);
  if(pRenderer == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    m_pSpringSystem->destroy();
    MP_DELETE( m_pSpringSystem);
    m_pPhysique->destroy();
    MP_DELETE( m_pPhysique);
    m_pMixer->destroy();
    MP_DELETE( m_pMixer);
	if( m_pSkeleton){
		m_pSkeleton->destroy();
		MP_DELETE( m_pSkeleton);	
	}
    return false;
  }

  // create the renderer from this model
  if(!pRenderer->create(this))
  {
    m_pSpringSystem->destroy();
    MP_DELETE( m_pSpringSystem);
    m_pPhysique->destroy();
    MP_DELETE( m_pPhysique);
    m_pMixer->destroy();
    MP_DELETE( m_pMixer);
	if( m_pSkeleton){
		m_pSkeleton->destroy();
		MP_DELETE( m_pSkeleton);
	}
    MP_DELETE( pRenderer);
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

  std::vector<CalModel*> abinded;
  abinded.resize( binded_models.size());	copy( binded_models.begin(), binded_models.end(), abinded.begin());
  binded_models.clear();
  for( unsigned int i=0; i<abinded.size(); i++)
	  abinded[i]->unbindSkeleton();

  // destroy all active meshes
  int meshId;
  for(meshId = 0; meshId < (int)m_vectorMesh.size(); meshId++)
  {
    m_vectorMesh[meshId]->destroy();
    MP_DELETE( m_vectorMesh[meshId]);
  }
  m_vectorMesh.clear();

  // destroy the renderer instance
  if(m_pRenderer != 0)
  {
    m_pRenderer->destroy();
    MP_DELETE( m_pRenderer);
    m_pRenderer = 0;
  }

  // destroy the spring system instance
  if(m_pSpringSystem != 0)
  {
    m_pSpringSystem->destroy();
    MP_DELETE( m_pSpringSystem);
    m_pSpringSystem = 0;
  }

  // destroy the physique instance
  if(m_pPhysique != 0)
  {
    m_pPhysique->destroy();
    MP_DELETE( m_pPhysique);
    m_pPhysique = 0;
  }

  // destroy the mixer instance
  if(m_pMixer != 0)
  {
    m_pMixer->destroy();
    MP_DELETE( m_pMixer);
    m_pMixer = 0;
  }

  // destroy the skeleton instance
  if (!IsSharedSkeleton())
  if(m_pSkeleton != 0)
  {
    m_pSkeleton->destroy();
    MP_DELETE( m_pSkeleton);
    m_pSkeleton = 0;
  }

  //if( m_pStepCtrl)  MP_DELETE( m_pStepCtrl);

  m_pCoreModel = 0;
}

void CalModel::destroy_geometry()
{
  // destroy all active meshes
  int meshId;
  for(meshId = 0; meshId < (int)m_vectorMesh.size(); meshId++)
  {
    m_vectorMesh[meshId]->destroy();
    MP_DELETE( m_vectorMesh[meshId]);
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
      MP_DELETE( pMesh);

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

void CalModel::SetLookController(CalLookController* lookController)
{
	m_lookController = lookController;
}

void CalModel::SetVisemeController(CalVisemeController* visemeController)
{
	m_visemeController = visemeController;
}

 /*****************************************************************************/
/** Updates the model instance.
  *
  * This function updates the model instance for a given amount of time.
  *
  * @param deltaTime The elapsed time in seconds since the last update.
  *****************************************************************************/
bool CalModel::update(float deltaTime, bool isModelVisible)
{
	if( NoSkel()) return false;	//	если без скелета, то не апдейтим модель
	if( !m_pMixer)	return false;

	// работать должно, но может подтормаживать
	if( isFrozen() || (binded_target_model && binded_target_model->isFrozen()))
		deltaTime = 0;

	//m_bRecalcRootTM = false;
	// if no current animations exists
	bool isAnyAnimationWasUpdated = m_pMixer->updateAnimation(deltaTime);
	isAnyAnimationWasUpdated |= pointctrl.bEnable;
	if (m_lookController)
	{
		isAnyAnimationWasUpdated |= m_lookController->m_isEnabled;
	}
	if (m_visemeController)
	{
		isAnyAnimationWasUpdated |= m_visemeController->IsEnabled();
	}
	if( isAnyAnimationWasUpdated)
		for( unsigned int i=0; i<binded_models.size(); i++)
			binded_models[i]->needUpdateWhenVisible();

	isAnyAnimationWasUpdated |= m_bNeedUpdateWhenVisible;

	m_bPrevModelVisible = isModelVisible;

	if( !unbinded_self_skeleton)
		m_pMixer->updateSkeleton();

	if ( m_pFAPU ||	(isModelVisible && isAnyAnimationWasUpdated))
	{
//		updatePointCtrl( deltaTime);
		updateLookAtCtrl( deltaTime);

		if (m_visemeController)
		{
			m_visemeController->Update();
		}

		if (m_pointController)
		{
			m_pointController->Update();
		}

		m_pPhysique->update();
		m_pMixer->updateMorph();
		m_pSpringSystem->update(deltaTime);
		m_bNeedUpdateWhenVisible = false;
		return true;
	}
	return isAnyAnimationWasUpdated;
}

void CalModel::freeze()
{
	m_bFrozen = true;
}

void CalModel::unFreeze()
{
	m_bFrozen = false;
}

bool CalModel::isFrozen()
{
	return m_bFrozen;
}

void CalModel::needUpdateWhenVisible()
{
	m_bNeedUpdateWhenVisible = true;
}

//****************************************************************************//

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

  if(m_pCalcFace != NULL){
	  if( !m_pCalcFace->init( this))
		  return;
	  m_pCalcFace->calcVectors();
	  for(int meshId = 0; meshId < 1/*meshCount*/; meshId++){
		  for(unsigned short iSubmeshId=0; iSubmeshId<m_pCalcFace->header.dwSubmeshCount; iSubmeshId++){
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
/*void CalModel::getTransMatrix(D3DMATRIX*	pMatrix){

	assert(FALSE);
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
		return;
	}else{

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
}*/

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
	if( (int)pMixer->m_vectorAnimation.size() < coreSize){
		pMixer->m_vectorAnimation.push_back( nullAnim);
	}
}

void CalModel::attachVB(IDirect3DDevice8 * /*pD3D8*/)
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

/*CalVector CalModel::getRootPosition( bool buseextern)
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
}*/

void CalModel::setDoneFlag(ACTIONTYPE type, int id)
{
	actiontype.push_back( type);
	actionid.push_back( id);
}

//int CalModel::lookAt(DWORD adwDelay, CalModel *m_pModel, float trTime, bool bToCamera, bool bToPoint, CalVector camPos)
int CalModel::lookAt(LOOKATPARAMS params)
{
	if( NoSkel())	return 0;
	if (m_lookController)
	{
		m_lookController->Init( params);
	}
	return 0;
}

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
	std::string boneName = name;
	if( !parentSkel || !parentSkel->getCoreSkeleton()->containBone(boneName))	return;
	int	boneId = parentSkel->getCoreSkeleton()->getCoreBoneIdSubName(boneName.c_str());
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
						for( unsigned int i=1; i < vertexCount; i++)
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
	for( i=0; i < 8; i++){
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

	//getTransMatrix( (D3DMATRIX*)&matrix);
	//D3DX
	for( i=0; i < 8; i++){
		//D3DXVec3Transform( &boundBoxTransform[i], &boundBox[i], &matrix);
		boundBoxTransform[i].x = boundBox[i].x;
		boundBoxTransform[i].y = boundBox[i].y;
		boundBoxTransform[i].z = boundBox[i].z;
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
	for( i=0; i < 8; i++){
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
					vertexCount = pCalRenderer->m_pSelectedSubmesh->getVertexCount();//pCalRenderer->getVertList(&meshVertices);
					unptrVert = (float*)pCalRenderer->get_vertices(pCalRenderer->m_pSelectedSubmesh);
					D3DVERTEX*	pvert = (D3DVERTEX*)unptrVert;

					aboxMax.x = aboxMin.x = pvert->x;
					aboxMax.y = aboxMin.y = pvert->y;
					aboxMax.z = aboxMin.z = pvert->z;

					for( unsigned int i=1; i < vertexCount; i++)
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
					//D3DMATRIX matrix;
					//getTransMatrix( &matrix);
					//alignBBox(aboxMin, aboxMax, &matrix);
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

/*void CalModel::move3d(float x, float y, float z)
{
	assert(FALSE);
	CalVector	curPos = getRootPosition();
	CALMOVETYPE eCurrentMoveType = eMoveType;
	eMoveType = CMT_ABSOLUTE;

	vTranslation.x += x-curPos.x;
	vTranslation.y += y-curPos.y;
	vTranslation.z += z-curPos.z;
	eMoveType = eCurrentMoveType;
}*/

/*CalVector CalModel::transformVector(CalVector v)
{
	D3DMATRIX	matrix;
	D3DXVECTOR3  d3dxv(v.x, v.y, v.z);
	D3DXVECTOR4  d3dxv4;

	getTransMatrix( &matrix);
	D3DXVec3Transform(&d3dxv4, &d3dxv, (D3DXMATRIX*)&matrix);
	d3dxv4.x /= d3dxv4.w; d3dxv4.y /= d3dxv4.w; d3dxv4.z /= d3dxv4.w;

	return CalVector( d3dxv4.x, d3dxv4.y, d3dxv4.z);
}*/

bool CalModel::NoSkel(bool abSetNoSkel)
{
	if( abSetNoSkel)
		m_bNoSkel = true;
	return m_bNoSkel;
}

//void CalModel::PointAt(float x, float y, float z, DWORD hand, DWORD type)
void CalModel::PointAt(POINTATPARAMS params)
{
	std::string sprefix;	// префикс имени кости в зависимости от руки

	CalCoreSkeleton	*pcs = m_pSkeleton->getCoreSkeleton();	// базовый скелет

	if( params.m_dwHand == RIGHTHAND){ sprefix = "r ";}else{ sprefix = "l ";}

	if( !( params.m_dwHand == pointctrl.initHand)){	// инициализация номеров

		{std::string name; name += sprefix; name += "Clavicle";
		if(pcs->containBone(name))
			pointctrl.idClavicle = pcs->getCoreBoneIdSubName( name.c_str());
		}
		{std::string name; name += sprefix; name += "UpperArm";
		if(pcs->containBone(name))
			pointctrl.idUpper = pcs->getCoreBoneIdSubName( name.c_str());
		}
		{std::string name; name += sprefix; name += "Forearm";
		if(pcs->containBone(name))
			pointctrl.idLower = pcs->getCoreBoneIdSubName( name.c_str());
		}
		{std::string name; name += sprefix; name += "Hand";
		if(pcs->containBone(name))
			pointctrl.idHand = pcs->getCoreBoneIdSubName( name.c_str());
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

/*CalVector CalModel::getDirectionAbsolute(DWORD id, CalVector axes)
{

	CalQuaternion mrotation;
	CalVector	  result;
	result = m_pSkeleton->getDirection(id, axes);	//!!!NEW!!!
	mrotation=qRotation;
	mrotation.conjugate();		//??
	result *= mrotation;

	return result;
}*/

bool CalModel::updateLookAtCtrl( float /*deltaTime*/)
{
	if (m_lookController)
	{
		m_lookController->Update();
	}
	return true;
}

/*bool CalModel::updatePointCtrl( float deltaTime)
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

	if( pointctrl.p_target!=NULL)
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
}*/

 /*****************************************************************************/
/** Construct matrix to bring model to coord center.
  * 
  *****************************************************************************/
/*void CalModel::getCenterMatrix(D3DXMATRIX *pm)
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
}*/

 /*****************************************************************************/
/** Construct matrix to bring model to coord center.
  * 
  *****************************************************************************/
/*void CalModel::getRestoreMatrix(D3DXMATRIX *pm)
{
	CalVector root;

	if( m_pOwner != NULL){
		//assert(false);
		root = getSkeleton()->getRootPoint( true);
	}else{
		root = getSkeleton()->getRootPoint( true);
	}

	D3DXMatrixTranslation(pm, root.x, root.y, root.z);
}*/

 /*****************************************************************************/
/** Construct matrix to aplly moving transforms to model.
  * 
  *****************************************************************************/
/*void CalModel::getMovingMatrix(D3DXMATRIX *pm)
{
	D3DXMATRIX	rtn, trn;

	CalQuaternion	qlocalquat	= qRotation;
	D3DXQUATERNION	localrootquat = D3DXQUATERNION(qlocalquat.x, qlocalquat.y, qlocalquat.z, qlocalquat.w);

	D3DXMatrixRotationQuaternion((D3DXMATRIX*)&rtn, &localrootquat);
	D3DXMatrixTranslation((D3DXMATRIX*)&trn, vTranslation.x, vTranslation.y, vTranslation.z);

	MULTMATRIX( pm, &rtn, &trn);
}*/

 /*****************************************************************************/
/** Construct matrix to aplly transformations
  * 
  *****************************************************************************/
/*void CalModel::getTransformMatrix(D3DXMATRIX *pm)
{
	D3DXMatrixScaling(pm, vScale3D.x * m_Scale, vScale3D.y * m_Scale, vScale3D.z * m_Scale);
}*/



/*bool CalModel::WasMoved()
{
	if( vTranslation != CalVector(0,0,0))	return true;
	CalQuaternion	qZero(CalVector(0, 0, 1), 0);
	if( qRotation.x!=qZero.x||qRotation.y!=qZero.y||qRotation.z!=qZero.z||qRotation.w!=qZero.w)	return true;
	return false;
}

void CalModel::setFloor(float floor)
{
	assert( false);
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
}*/

/*void CalModel::stockFloor()
{
	return;
	if( m_floorLevel == FLT_MAX)	return;
	//
	float	bonesFloor = m_pSkeleton->getFloor();

	if( bonesFloor == FLT_MAX)	return;

	vTranslation.z += ( m_floorLevel - (bonesFloor + vTranslation.z));
}*/

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

bool CalModel::get_bbox_by_bones(CalVector *max, CalVector *min, std::vector<std::string>& names)
{
	if ( getPhysique()->pHitBoxes == NULL)	return false;
	getPhysique()->get_bbox_by_hitbs(max, min, names);
	return	true;
}

void CalModel::insert_EmPack(std::string name, FACEExpression **vector, int count)
{
	EM_PACK	pack;
	pack.emotions = vector;
	pack.count = count;
	m_mapEmotions.insert(std::make_pair(MAKE_MP_STRING(name), pack));
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

bool CalModel::basisChanged(CalVector &pos, CalQuaternion &quat, bool& relative)
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

void	CalModel::getInFileTranslation(CalVector& trn)
{
	getCoreModel()->get_worldTranslation(trn);
}

void	CalModel::getInFileRotation(CalQuaternion& rtn)
{
	getCoreModel()->get_worldRotation(rtn);
}

int CalModel::getFaceCount()
{ 
	return m_pRenderer?m_pRenderer->getFaceCountInModel():0;
}

int CalModel::getEdgesCount()
{ 
	return m_pRenderer?m_pRenderer->getEdgesCountInModel():0;
}

void CalModel::bindToModelSkeleton(CalModel* apModel)
{
	unbindSkeleton();
	if( apModel && apModel->getSkeleton())
	{
		unbinded_self_skeleton = m_pSkeleton;
		m_pSkeleton = apModel->getSkeleton();
		binded_target_model = apModel;
		binded_target_model->addBindedModel( this);
		m_bNoSkel = false;
	}
	needUpdateWhenVisible();
}

void CalModel::unbindSkeleton()
{
	if( unbinded_self_skeleton)
	{
		m_pSkeleton = unbinded_self_skeleton;		
	}
	else
	{
		m_pSkeleton = NULL;
	}

	if (binded_target_model)
	{
		binded_target_model->removeBindedModel( this);
		binded_target_model = NULL;
	}

	unbinded_self_skeleton = NULL;
	needUpdateWhenVisible();
}

void CalModel::addBindedModel(CalModel* aModel)
{
	std::vector<CalModel*>::iterator it = find(binded_models.begin(), binded_models.end(), aModel);
	if( it == binded_models.end())
		binded_models.push_back( aModel);
}

void CalModel::removeBindedModel(CalModel* aModel)
{
	std::vector<CalModel*>::iterator it = find(binded_models.begin(), binded_models.end(), aModel);
	if( it != binded_models.end())
		binded_models.erase( it);
}

void CalModel::SetPointController(CalPointController* pointController)
{
	m_pointController = pointController;
}