//****************************************************************************//
// physique.cpp                                                               //
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
#include "physique.h"
#include "coremodel.h"
#include "model.h"
#include "mesh.h"
#include "submesh.h"
#include "skeleton.h"
#include "corebone.h"
#include "bone.h"
#include "coresubmesh.h"
#include "facedesc.h"
#include "mpeg4fdp.h"
#include "renderer.h"

 /*****************************************************************************/
/** Constructs the physique instance.
  *
  * This function is the default constructor of the physique instance.
  *****************************************************************************/

CalPhysique::CalPhysique()
{
  m_pModel = 0;
  pHitBoxes = NULL;
}

 /*****************************************************************************/
/** Destructs the physique instance.
  *
  * This function is the destructor of the physique instance.
  *****************************************************************************/

CalPhysique::~CalPhysique()
{

}

 /*****************************************************************************/
/** Calculates the transformed vertex data.
  *
  * This function calculates and returns the transformed vertex data of a
  * specific submesh.
  *
  * @param pSubmesh A pointer to the submesh from which the vertex data should
  *                 be calculated and returned.
  * @param pVertexBuffer A pointer to the user-provided buffer where the vertex
  *                      data is written to.
  *
  * @return The number of vertices written to the buffer.
  *****************************************************************************/

int CalPhysique::calculateVertices(CalSubmesh *pSubmesh, float *pVertexBuffer)
{
  // get bone vector of the skeleton
  std::vector<CalBone *>& vectorBone = m_pModel->getSkeleton()->getVectorBone();

  // get vertex vector of the core submesh
  std::vector<CalCoreSubmesh::Vertex>& vectorVertex = pSubmesh->getCoreSubmesh()->getVectorVertex();

  // get physical property vector of the core submesh
  std::vector<CalCoreSubmesh::PhysicalProperty>& vectorPhysicalProperty = pSubmesh->getCoreSubmesh()->getVectorPhysicalProperty();

  // get the number of vertices
  int vertexCount;
  vertexCount = pSubmesh->getVertexCount();

  CalVector vPosition;

  int stride = m_pModel->stride/sizeof(float);
  // calculate all submesh vertices
  int vertexId;

  /*ATLTRACE("Vertex count");
  char tmp[20];
  itoa(vertexCount, tmp, 10);
  ATLTRACE(tmp);	ATLTRACE("\n");*/

  for(vertexId = 0; vertexId < vertexCount; vertexId++)
  {
//#ifdef DEBUG
	  if( vertexId == 377)
		  int ii=0;
//#endif
    // get the vertex
    CalCoreSubmesh::Vertex& vertex = vectorVertex[vertexId];

    // initialize vertex
    float x, y, z;
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;

//	CalVector temp(-10.735, -4.628, 63.662);
//	if( (vertex.position-temp).length() < 0.05)
//		int i=0;

    // blend together all vertex influences
    int influenceId;
    for(influenceId = 0; influenceId < (int)vertex.vectorInfluence.size(); influenceId++)
    {
      // get the influence
      CalCoreSubmesh::Influence& influence = vertex.vectorInfluence[influenceId];

      // get the bone of the influence vertex
if(influence.boneId >= vectorBone.size()) // Tandy 04.06.08: затычка, чтобы не падало
continue;
      CalBone *pBone;
      pBone = vectorBone[influence.boneId];

      // transform vertex with current state of the bone
      //CalVector v(vertex.position);
	  vPosition.x = vertex.position.x;
	  vPosition.y = vertex.position.y;
	  vPosition.z = vertex.position.z;

      vPosition *= pBone->getRotationBoneSpace();
      vPosition += pBone->getTranslationBoneSpace();

      x += influence.weight * (vPosition.x - vertex.position.x);
      y += influence.weight * (vPosition.y - vertex.position.y);
      z += influence.weight * (vPosition.z - vertex.position.z);
    }

	x += vertex.position.x;
	y += vertex.position.y;
	z += vertex.position.z;

    // save vertex position
    if(pSubmesh->getCoreSubmesh()->getSpringCount() > 0)
    {
      // get the pgysical property of the vertex
      CalCoreSubmesh::PhysicalProperty& physicalProperty = vectorPhysicalProperty[vertexId];

      // assign new vertex position if there is no vertex weight
      if(physicalProperty.weight == 0.0f)
      {
        pVertexBuffer[0] = x;
        pVertexBuffer[1] = y;
        pVertexBuffer[2] = z;
      }
    }
    else
    {
      pVertexBuffer[0] = x;
      pVertexBuffer[1] = y;
      pVertexBuffer[2] = z;
    }

    // next vertex position in buffer
    pVertexBuffer += stride;
  }

  //
  // Calculate hit boxes
  for( int ihitboxes=0; ihitboxes<m_vectorHitBoxes.size(); ihitboxes++){
	  BOUNDVERTS	bv = m_vectorHitBoxes[ihitboxes];
	  if( !bv.inited) continue;
	  //
	  pHitBoxes[ihitboxes].min = m_vectorHitBoxes[ihitboxes].min.pos;
	  pHitBoxes[ihitboxes].max = m_vectorHitBoxes[ihitboxes].max.pos;
		CalBone *pBone;
		pBone = vectorBone[ihitboxes];
		for(int i=0;i<8;i++){
			pHitBoxes[ihitboxes].corners[i] = bv.corners[i];
			pHitBoxes[ihitboxes].corners[i] *= pBone->getRotationBoneSpace();
			pHitBoxes[ihitboxes].corners[i] += pBone->getTranslationBoneSpace();
		}
		if( bv.inited)
			pHitBoxes[ihitboxes].inited=true;
  }

  return vertexCount;
}

 /*****************************************************************************/
/** Calculates the transformed normal data.
  *
  * This function calculates and returns the transformed normal data of a
  * specific submesh.
  *
  * @param pSubmesh A pointer to the submesh from which the normal data should
  *                 be calculated and returned.
  * @param pNormalBuffer A pointer to the user-provided buffer where the normal
  *                      data is written to.
  *
  * @return The number of normals written to the buffer.
  *****************************************************************************/

int CalPhysique::calculateNormals(CalSubmesh *pSubmesh, float *pNormalBuffer)
{
  // get bone vector of the skeleton
  std::vector<CalBone *>& vectorBone = m_pModel->getSkeleton()->getVectorBone();

  // get vertex vector of the submesh
  std::vector<CalCoreSubmesh::Vertex>& vectorVertex = pSubmesh->getCoreSubmesh()->getVectorVertex();

  // get the number of vertices
  int vertexCount;
  vertexCount = pSubmesh->getVertexCount();

  CalVector	vNormal;
  int stride = m_pModel->stride/sizeof(float);
  // calculate normal for all submesh vertices
  int vertexId;
  for(vertexId = 0; vertexId < vertexCount; vertexId++)
  {
    // get the vertex
    CalCoreSubmesh::Vertex& vertex = vectorVertex[vertexId];

    // initialize normal
    float nx, ny, nz;
    nx = 0.0f;
    ny = 0.0f;
    nz = 0.0f;

    // blend together all vertex influences
	if( !m_pModel->NoSkel()){
		int influenceId;
		for(influenceId = 0; influenceId < (int)vertex.vectorInfluence.size(); influenceId++)
		{
		  // get the influence
		  CalCoreSubmesh::Influence& influence = vertex.vectorInfluence[influenceId];

		  // get the bone of the influence vertex
		  CalBone *pBone;
		  pBone = vectorBone[influence.boneId];

		  // transform normal with current state of the bone
		  //CalVector v(vertex.normal);
		  vNormal.x = vertex.normal.x;
		  vNormal.y = vertex.normal.y;
		  vNormal.z = vertex.normal.z;
		  vNormal *= pBone->getRotationBoneSpace();

		  nx += influence.weight * vNormal.x;
		  ny += influence.weight * vNormal.y;
		  nz += influence.weight * vNormal.z;
		}
	}
	else
	{
		//CalVector v(vertex.normal);
		//nx = v.x; ny = v.y; nz = v.z;
		nx = vertex.normal.x; ny = vertex.normal.y; nz = vertex.normal.z;
	}

    // re-normalize normal
    float scale;
    scale = 1.0f / sqrt(nx * nx + ny * ny + nz * nz);

    pNormalBuffer[3] = nx * scale;
    pNormalBuffer[4] = ny * scale;
    pNormalBuffer[5] = nz * scale;

    // next vertex position in buffer
    pNormalBuffer += stride;
  }

  return vertexCount;
}

 /*****************************************************************************/
/** Creates the physique instance.
  *
  * This function creates the physique instance.
  *
  * @param pModel A pointer to the model that should be managed with this
  *               physique instance.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalPhysique::create(CalModel *pModel)
{
  if(pModel == 0)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  m_pModel = pModel;

  CalCoreModel *pCoreModel;
  pCoreModel = m_pModel->getCoreModel();
  if(pCoreModel == 0)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  return true;
}

 /*****************************************************************************/
/** Destroys the physique instance.
  *
  * This function destroys all data stored in the physique instance and frees
  * all allocated memory.
  *****************************************************************************/

void CalPhysique::destroy()
{
  m_vectorHitBoxes.clear();
  if( pHitBoxes)
	free( pHitBoxes);
  m_pModel = 0;
}

 /*****************************************************************************/
/** Updates all the internally handled attached meshes.
  *
  * This function updates all the attached meshes of the model that are handled
  * internally.
  *****************************************************************************/

void CalPhysique::update()
{
  std::vector<CalMesh *>& vectorMesh = m_pModel->getVectorMesh();

  if (m_pModel->getRenderer()->lockVertices()) 
  {
  
	  std::vector<CalMesh *>::iterator iteratorMesh;
	  for(iteratorMesh = vectorMesh.begin(); iteratorMesh != vectorMesh.end(); ++iteratorMesh)
	  {
		std::vector<CalSubmesh *>& vectorSubmesh = (*iteratorMesh)->getVectorSubmesh();

		std::vector<CalSubmesh *>::iterator iteratorSubmesh;
		for(iteratorSubmesh = vectorSubmesh.begin(); iteratorSubmesh != vectorSubmesh.end(); ++iteratorSubmesh)
		{
		  float* pBaseVertexBuffer=(float*)m_pModel->getRenderer()->get_vertices((*iteratorSubmesh));
		  // вычисляем вершины
		  calculateVertices((*iteratorSubmesh), pBaseVertexBuffer);
		  // вычисляем нормали
		  calculateNormals((*iteratorSubmesh), pBaseVertexBuffer);
		  //(*iteratorSubmesh)->releaseVertices();
		}
	  }
	  m_pModel->getRenderer()->releaseVertices();
  }
  //
  update_FAP();
}

//****************************************************************************//

void CalPhysique::calculateFace(CFace *fd, float *pVertexBuffer, WORD dwSubmesh, int lex1, int lex2, float phase, bool bUseVB)
{
	assert(FALSE);	// надо учесть возможность изменения FVF и STRIDE

/*	D3DXVECTOR3	*pDeltas1 = fd->aDeltas[dwSubmesh];//+сдвиг для конкретной позиции
	D3DXVECTOR3	*pDeltas2 = fd->aDeltas[dwSubmesh];//+сдвиг для конкретной позиции
	WORD		*pIndexes = fd->aVertIndexes[dwSubmesh];
	WORD		dwNum = fd->aVertNum[dwSubmesh];

	if( lex1 > -1) pDeltas1 += dwNum*lex1;
	if( lex2 > -1) pDeltas2 += dwNum*lex2;

	if(lex1 == lex2){
		D3DXVECTOR3	res;
		for(register i=0; i < dwNum; i++){
			res = fd->vi*pDeltas1->x + fd->vj*pDeltas1->z + fd->vk*pDeltas1->y;
			res += fd->base;
			if( !bUseVB){
				D3DXVECTOR3*	d3dVertexBuffer = (D3DXVECTOR3*)pVertexBuffer;
				d3dVertexBuffer[pIndexes[i]] = res;
			}else{
				D3DVERTEX*	d3dVertexBuffer = (D3DVERTEX*)pVertexBuffer;
				d3dVertexBuffer[pIndexes[i]].x = res.x;
				d3dVertexBuffer[pIndexes[i]].y = res.y;
				d3dVertexBuffer[pIndexes[i]].z = res.z;
			}
			pDeltas1 += 1;
		}
	}else{
		D3DXVECTOR3	res;
		for(register i=0; i < dwNum; i++){
			if( lex1 > -1){
				res = fd->vi*(pDeltas1->x+(pDeltas2->x-pDeltas1->x)*phase) + 
					   fd->vj*(pDeltas1->z+(pDeltas2->z-pDeltas1->z)*phase) + 
						fd->vk*(pDeltas1->y+(pDeltas2->y-pDeltas1->y)*phase);
			    res += fd->base;
			}else{
				res = fd->vi*(pDeltas2->x) + 
					   fd->vj*(pDeltas2->z) + 
						fd->vk*(pDeltas2->y);
				res += fd->base;
				D3DVERTEX*	d3dVertexBuffer = (D3DVERTEX*)pVertexBuffer;
				res.x = (res.x - d3dVertexBuffer[pIndexes[i]].x)*phase+d3dVertexBuffer[pIndexes[i]].x;
				res.y = (res.y - d3dVertexBuffer[pIndexes[i]].y)*phase+d3dVertexBuffer[pIndexes[i]].y;
				res.z = (res.z - d3dVertexBuffer[pIndexes[i]].z)*phase+d3dVertexBuffer[pIndexes[i]].z;
			}
			
			if( !bUseVB){
				D3DXVECTOR3*	d3dVertexBuffer = (D3DXVECTOR3*)pVertexBuffer;
				d3dVertexBuffer [pIndexes[i]] = res;
			}else{
				D3DVERTEX*	d3dVertexBuffer = (D3DVERTEX*)pVertexBuffer;
				d3dVertexBuffer[pIndexes[i]].x = res.x;
				d3dVertexBuffer[pIndexes[i]].y = res.y;
				d3dVertexBuffer[pIndexes[i]].z = res.z;
			}
			pDeltas1++;
			pDeltas2++;
		}
	}*/
}

D3DVECTOR CalPhysique::getVertexPosition(CalSubmesh *pSubmesh, WORD vNum)
{
	std::vector<CalCoreSubmesh::Vertex>& vectorVertex = pSubmesh->getCoreSubmesh()->getVectorVertex();
	D3DVECTOR position;
	position.x = vectorVertex[vNum].position.x;
	position.y = vectorVertex[vNum].position.y;
	position.z = vectorVertex[vNum].position.z;
	return position;
}

void CalPhysique::createHitBoxes()
{
	if(!m_pModel->getSkeleton())
		return;

	std::vector<std::vector<HIBOXVERT>*> bonesIds;
	//
	//bonesIds.reserve( m_pModel->getSkeleton()->getVectorCoreBone().size();
	int bid;
	for(bid=0; bid < m_pModel->getSkeleton()->getVectorBone().size(); bid++){
		std::vector<HIBOXVERT>* vc = new std::vector<HIBOXVERT>;
		bonesIds.push_back( vc);
	}
	//
	pHitBoxes = (HITBOX*)malloc( sizeof(HITBOX)*m_pModel->getSkeleton()->getVectorBone().size());
	for( bid=0; bid < m_pModel->getSkeleton()->getVectorBone().size(); bid++){
		pHitBoxes[bid].inited=false;
	}
	//
	std::vector<CalMesh *>& vectorMesh = m_pModel->getVectorMesh();

	// get bone vector of the skeleton
	std::vector<CalBone *>& vectorBone = m_pModel->getSkeleton()->getVectorBone();

	std::vector<CalMesh *>::iterator iteratorMesh;
	for(iteratorMesh = vectorMesh.begin(); iteratorMesh != vectorMesh.end(); ++iteratorMesh)
	{
		std::vector<CalSubmesh *>& vectorSubmesh = (*iteratorMesh)->getVectorSubmesh();

		std::vector<CalSubmesh *>::iterator iteratorSubmesh;
		int subid;
		for(iteratorSubmesh = vectorSubmesh.begin(), subid=0; iteratorSubmesh != vectorSubmesh.end(); ++iteratorSubmesh, subid++)
		{
			//float* pBaseVertexBuffer=(*iteratorSubmesh)->getVertices();
			CalSubmesh* pSubmesh = (*iteratorSubmesh);
			// get vertex vector of the core submesh
			std::vector<CalCoreSubmesh::Vertex>& vectorVertex = pSubmesh->getCoreSubmesh()->getVectorVertex();
			// get physical property vector of the core submesh
			std::vector<CalCoreSubmesh::PhysicalProperty>& vectorPhysicalProperty = pSubmesh->getCoreSubmesh()->getVectorPhysicalProperty();
			// get the number of vertices
			int vertexCount;
			vertexCount = pSubmesh->getVertexCount();
			// calculate all submesh vertices
			int vertexId;
			for(vertexId = 0; vertexId < vertexCount; vertexId++)
			{
				// get the vertex
				CalCoreSubmesh::Vertex& vertex = vectorVertex[vertexId];
				
				HIBOXVERT vrt( subid, vertexId, vertex.position);
				

				// blend together all vertex influences
				int influenceId;
				for(influenceId = 0; influenceId < (int)vertex.vectorInfluence.size(); influenceId++)
				{
				  CalCoreSubmesh::Influence& influence = vertex.vectorInfluence[influenceId];
				  bonesIds[ influence.boneId]->push_back( vrt);
				}
			}
		}
	}
	//
	// go for all bones and get HitBoxes;
	for( int boneid=0; boneid < m_pModel->getSkeleton()->getVectorBone().size(); boneid++){
		std::vector<HIBOXVERT>* vc = bonesIds[boneid];
		//
		CalVector	min;
		CalVector	max;
		bool		init=false;	
		//
		BOUNDVERTS	boundVert;
		boundVert.min.subid = 0;
		boundVert.min.vertid = 0;
		boundVert.max.subid = 0;
		boundVert.max.vertid = 0;
		// get the bone of the influence vertex
		CalBone *pBone;
		pBone = vectorBone[boneid];
		//
		for(int hitvertid=0; hitvertid < vc->size(); hitvertid++){
			HIBOXVERT hitvert = (*vc)[hitvertid];
			//
			boundVert.inited = true;


			CalQuaternion	m_rotationBoneSpace;
			CalVector		m_translationBoneSpace;

			CalVector	vertexPos( hitvert.pos.x, hitvert.pos.y, hitvert.pos.z);
			CalVector	vertexPos1( hitvert.pos.x, hitvert.pos.y, hitvert.pos.z);


			m_translationBoneSpace = pBone->getCoreBone()->getTranslationBoneSpace();

			m_rotationBoneSpace = pBone->getCoreBone()->getRotationBoneSpace();

			vertexPos *= m_rotationBoneSpace;
			vertexPos += m_translationBoneSpace;

			if( !init)
			{
				boundVert.min.pos = vertexPos;
				boundVert.max.pos = vertexPos;
				init=true;
			}else
			{
				if( vertexPos.x < boundVert.min.pos.x){ boundVert.min.pos.x = vertexPos.x;}
				if( vertexPos.y < boundVert.min.pos.y){ boundVert.min.pos.y = vertexPos.y;}
				if( vertexPos.z < boundVert.min.pos.z){ boundVert.min.pos.z = vertexPos.z;}

				if( vertexPos.x > boundVert.max.pos.x){ boundVert.max.pos.x = vertexPos.x;}
				if( vertexPos.y > boundVert.max.pos.y){ boundVert.max.pos.y = vertexPos.y;}
				if( vertexPos.z > boundVert.max.pos.z){ boundVert.max.pos.z = vertexPos.z;}
			}
		}
		//
		boundVert.corners[0] = boundVert.min.pos;
		boundVert.corners[1] = boundVert.min.pos;  boundVert.corners[1].z = boundVert.max.pos.z; 
		boundVert.corners[2] = boundVert.max.pos;  boundVert.corners[2].y = boundVert.min.pos.y; 
		boundVert.corners[3] = boundVert.min.pos;  boundVert.corners[3].x = boundVert.max.pos.x; 
		boundVert.corners[4] = boundVert.min.pos;													boundVert.corners[4].y = boundVert.max.pos.y; 
		boundVert.corners[5] = boundVert.min.pos;  boundVert.corners[5].z = boundVert.max.pos.z;	boundVert.corners[5].y = boundVert.max.pos.y;
		boundVert.corners[6] = boundVert.max.pos;  boundVert.corners[6].y = boundVert.min.pos.y;	boundVert.corners[6].y = boundVert.max.pos.y;
		boundVert.corners[7] = boundVert.min.pos;  boundVert.corners[7].x = boundVert.max.pos.x;	boundVert.corners[7].y = boundVert.max.pos.y;
		//
		CalQuaternion	m_rotationBoneSpace;
		CalVector		m_translationBoneSpace;
		m_translationBoneSpace = pBone->getCoreBone()->getTranslationBoneSpace();
		m_rotationBoneSpace = pBone->getCoreBone()->getRotationBoneSpace();
		m_rotationBoneSpace.conjugate();
		boundVert.min.pos -= m_translationBoneSpace;
		boundVert.max.pos -= m_translationBoneSpace;
		boundVert.min.pos *= m_rotationBoneSpace;
		boundVert.max.pos *= m_rotationBoneSpace;
		//
		for(int i=0;i<8;i++){
			boundVert.corners[i] -= m_translationBoneSpace;
			boundVert.corners[i] *= m_rotationBoneSpace;
		}
		//
		m_vectorHitBoxes.push_back( boundVert);
	}
	//
	for( bid=0; bid < m_pModel->getSkeleton()->getVectorBone().size(); bid++){
		std::vector<HIBOXVERT>* vc = bonesIds[bid];
		delete vc;
	}
	bonesIds.clear();
	//
}	

int breakID = 155;
void CalPhysique::update_FAP()
{
	CMpeg4FDP*	face = m_pModel->m_pFAPU;
	if( !face)	return;
	if( !face->pBone)	return;
	if( face->cur_faps.empty() && face->expressions.empty())
		return;

	// get bone vector of the skeleton
	std::vector<CalBone *>& vectorBone = m_pModel->getSkeleton()->getVectorBone();

	// lock all vertices data
	CalRenderer* rend = m_pModel->getRenderer();
	std::vector<CalMesh *>& vectorMesh = m_pModel->getVectorMesh();
	int mc = vectorMesh.size();
	if( mc == 0)	return;
	// меш головы персонажа
	CalMesh*	headCalMesh = vectorMesh[0];
	std::vector<CalSubmesh *>& vectorSubmesh = headCalMesh->getVectorSubmesh();
	int sc  = vectorSubmesh.size();

	// get vertices array pointers
	LPVOID	v_array[50];
	if( !m_pModel->getRenderer()->lockVertices())	return;
	// lock vertices
	for( int i=0; i<sc; i++)
		v_array[i] = (LPVOID)m_pModel->getRenderer()->get_vertices(vectorSubmesh[i]);

	control_point*	base_point = face->n_points[AXE_CENTER_GROUP_ID][AXE_CENTER_SUBGROUP_ID];
	int				base_submesh_id = base_point->points[0].submesh_id;
	CalSubmesh*		base_submesh = vectorSubmesh[ base_submesh_id];

	float* padr = (float*)((DWORD)v_array[base_submesh_id] + m_pModel->stride*base_point->points[0].id);
	CalVector base = *(CalVector*)(padr);

	// get vertex vector of the core submesh
	int float_stride = m_pModel->stride/sizeof(float);
	std::vector<CalCoreSubmesh::Vertex>& vectorVertex = base_submesh->getCoreSubmesh()->getVectorVertex();
	CalQuaternion	head_rotation_absolute = ((CalBone*)face->pBone)->getRotationAbsolute();
	//CalQuaternion	head_rotation_bone_space = ((CalBone*)face->pBone)->getCoreBone()->getRotationBoneSpace();
	//CalQuaternion	head_rotation_bone_space_inverse = head_rotation_bone_space;
	//				head_rotation_bone_space_inverse.conjugate();

	// rotation - to transform MPEG4 FAPU coordinate system to 3DSMax4 coordinate system


	// calculate all other vertices
	std::vector<control_point*>::iterator			it;
	for( it=face->cur_cpoints.begin(); it!=face->cur_cpoints.end(); it++)
	{
		control_point*	cp = *it;
		if( cp->v_current.x==0 && cp->v_current.y==0 && cp->v_current.z==0)	continue;

#ifdef DEBUG
		if( cp->group_id == 8 && cp->id == 2)
			int iii=0;
#endif

		//cp->v_current *= head_rotation_bone_space_inverse;
		cp->v_current *= head_rotation_absolute;

		for( int inf=0; inf<cp->points.size(); inf++)
		{
			submesh_face_point*	ip = &cp->points[inf];
//#ifdef DEBUG
			if( ip->id == breakID)
				int ii=0;
//#endif
			if(ip->weight == 0)	continue;
			
			float*		ip_pos = (float*)v_array[ ip->submesh_id];
			ip_pos +=	float_stride*ip->id;

			(*ip_pos) += cp->v_current.x*ip->weight;	ip_pos++;
			(*ip_pos) += cp->v_current.y*ip->weight;	ip_pos++;
			(*ip_pos) += cp->v_current.z*ip->weight;
		}
	}

	// release vertices
	//for( i=0; i<sc; i++)	vectorSubmesh[i]->releaseVertices();
	m_pModel->getRenderer()->releaseVertices();
}

bool CalPhysique::get_bbox_by_hitbs(CalVector *max, CalVector *min, std::vector<Cal3DComString>& names)
{
	max->set( -FLT_MAX, -FLT_MAX, -FLT_MAX);
	min->set( FLT_MAX, FLT_MAX, FLT_MAX);
	// get bone vector of the skeleton
	std::vector<CalBone *>& vectorBone = m_pModel->getSkeleton()->getVectorBone();
	// Calculate hit boxes
	for( int ihitboxes=0; ihitboxes<m_vectorHitBoxes.size(); ihitboxes++)
	{
		CalBone *pBone;
		pBone = vectorBone[ihitboxes];
		if (true /*names.size() > 0*/) {
			Cal3DComString	name = pBone->getCoreBone()->getName();
			name.MakeLower();
			if( !(name.Find("spline") > 0 ||
				name.Find("pelvis") > 0 ||
				name.Find("thigh") > 0 ||
				name.Find("calf") > 0 ||
				//name.Find("foot") > 0 ||
				name.Find("head") > 0 ||
				name.Find("neck") > 0))	continue;
				
		}

		BOUNDVERTS	bv = m_vectorHitBoxes[ihitboxes];
		if( !pHitBoxes[ihitboxes].inited) continue;

		for(int i=0;i<8;i++)
		{
			CalVector	vpos = pHitBoxes[ihitboxes].corners[i];
			if( min->x > vpos.x)	min->x = vpos.x;
			if( min->y > vpos.y)	min->y = vpos.y;
			if( min->z > vpos.z)	min->z = vpos.z;
			if( max->x < vpos.x)	max->x = vpos.x;
			if( max->y < vpos.y)	max->y = vpos.y;
			if( max->z < vpos.z)	max->z = vpos.z;
		}
	}
	return true;
}
