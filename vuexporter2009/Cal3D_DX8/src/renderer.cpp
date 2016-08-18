//****************************************************************************//
// renderer.cpp                                                               //
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
#include "renderer.h"
#include "coremodel.h"
#include "model.h"
#include "coremesh.h"
#include "mesh.h"
#include "submesh.h"
#include "skeleton.h"
#include "bone.h"
#include "corematerial.h"
#include "coresubmesh.h"
#include "physique.h"
#include "facedesc.h"
#include "Mpeg4FDP.h"
#include <algorithm>

 /*****************************************************************************/
/** Constructs the renderer instance.
  *
  * This function is the default constructor of the renderer instance.
  *****************************************************************************/

//! returns the bitwise representation of a floating point number
#define FG_FPBITS(fp)		(*(int *)&(fp))
//! returns the absolute value of a floating point number in bitwise form
#define FG_FPABSBITS(fp)	(FG_FPBITS(fp)&0x7FFFFFFF)
//! pi / 2
#define PIDIV2	1.57f

CalRenderer::CalRenderer()
{
  m_pSelectedSubmesh = 0;
  m_dwSelectedSubmesh = 0;
  pVertData = pIndexData = 0;
  pVertexBuffer = NULL;
  pIndexBuffer = NULL;
}

 /*****************************************************************************/
/** Destructs the renderer instance.
  *
  * This function is the destructor of the renderer instance.
  *****************************************************************************/

CalRenderer::~CalRenderer()
{
}

 /*****************************************************************************/
/** Initializes the rendering query phase.
  *
  * This function initializes the rendering query phase. It must be called
  * before any rendering queries are executed.
  *****************************************************************************/

bool CalRenderer::beginRendering()
{
  // get the attached meshes vector
  std::vector<CalMesh *>& vectorMesh = m_pModel->getVectorMesh();

  // check if there are any meshes attached to the model
  if(vectorMesh.size() == 0)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  // select the default submesh
  m_pSelectedSubmesh = vectorMesh[0]->getSubmesh(0);
  if(m_pSelectedSubmesh == 0) return false;

  return true;
}

 /*****************************************************************************/
/** Creates the renderer instance.
  *
  * This function creates the renderer instance.
  *
  * @param pModel A pointer to the model that should be managed with this
  *               renderer instance.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalRenderer::create(CalModel *pModel)
{
  if(pModel == 0)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  m_pModel = pModel;

  return true;
}

 /*****************************************************************************/
/** Destroys the renderer instance.
  *
  * This function destroys all data stored in the renderer instance and frees
  * all allocated memory.
  *****************************************************************************/

void CalRenderer::destroy()
{
  m_pModel = 0;
}

 /*****************************************************************************/
/** Finishes the rendering query phase.
  *
  * This function finishes the rendering query phase. It must be called
  * after all rendering queries have been executed.
  *****************************************************************************/

void CalRenderer::endRendering()
{
  // clear selected submesh ??
  //m_pSelectedSubmesh = 0;
}

 /*****************************************************************************/
/** Provides access to the ambient color.
  *
  * This function returns the ambient color of the material of the selected
  * mesh/submesh.
  *
  * @param pColorBuffer A pointer to the user-provided buffer where the color
  *                     data is written to.
  *****************************************************************************/

void CalRenderer::getAmbientColor(unsigned char *pColorBuffer)
{
  // get the core material
  CalCoreMaterial *pCoreMaterial;
  pCoreMaterial = m_pModel->getCoreModel()->getCoreMaterial(m_pSelectedSubmesh->getCoreMaterialId());
  if(pCoreMaterial == 0)
  {
    // write default values to the color buffer
    pColorBuffer[0] = 0;
    pColorBuffer[1] = 0;
    pColorBuffer[2] = 0;
    pColorBuffer[3] = 0;

    return;
  }

  // get the ambient color of the material
  CalCoreMaterial::Color& color = pCoreMaterial->getAmbientColor();

  // write it to the color buffer
  pColorBuffer[0] = color.red;
  pColorBuffer[1] = color.green;
  pColorBuffer[2] = color.blue;
  pColorBuffer[3] = color.alpha;
}

 /*****************************************************************************/
/** Provides access to the diffuse color.
  *
  * This function returns the diffuse color of the material of the selected
  * mesh/submesh.
  *
  * @param pColorBuffer A pointer to the user-provided buffer where the color
  *                     data is written to.
  *****************************************************************************/

void CalRenderer::getDiffuseColor(unsigned char *pColorBuffer)
{
  // get the core material
  CalCoreMaterial *pCoreMaterial;
  pCoreMaterial = m_pModel->getCoreModel()->getCoreMaterial(m_pSelectedSubmesh->getCoreMaterialId());
  if(pCoreMaterial == 0)
  {
    // write default values to the color buffer
    pColorBuffer[0] = 192;
    pColorBuffer[1] = 192;
    pColorBuffer[2] = 192;
    pColorBuffer[3] = 192;

    return;
  }

  // get the diffuse color of the material
  CalCoreMaterial::Color& color = pCoreMaterial->getDiffuseColor();

  // write it to the color buffer
  pColorBuffer[0] = color.red;
  pColorBuffer[1] = color.green;
  pColorBuffer[2] = color.blue;
  pColorBuffer[3] = color.alpha;
}

 /*****************************************************************************/
/** Provides access to the emissive color.
  *
  * This function returns the emissive color of the material of the selected
  * mesh/submesh.
  *
  * @param pColorBuffer A pointer to the user-provided buffer where the color
  *                     data is written to.
  *****************************************************************************/
void CalRenderer::getEmissiveColor(unsigned char *pColorBuffer)
{
  // get the core material
  CalCoreMaterial *pCoreMaterial;
  pCoreMaterial = m_pModel->getCoreModel()->getCoreMaterial(m_pSelectedSubmesh->getCoreMaterialId());
  if(pCoreMaterial == 0)
  {
    // write default values to the color buffer
    pColorBuffer[0] = 0;
    pColorBuffer[1] = 0;
    pColorBuffer[2] = 0;
    pColorBuffer[3] = 0;

    return;
  }

  // get the diffuse color of the material
  CalCoreMaterial::Color& color = pCoreMaterial->getEmissiveColor();

  // write it to the color buffer
  pColorBuffer[0] = color.red;
  pColorBuffer[1] = color.green;
  pColorBuffer[2] = color.blue;
  pColorBuffer[3] = color.alpha;
}

 /*****************************************************************************/
/** Returns the number of faces.
  *
  * This function returns the number of faces in the selected mesh/submesh.
  *
  * @return The number of faces.
  *****************************************************************************/

int CalRenderer::getFaceCount()
{
  return m_pSelectedSubmesh->getFaceCount();
}

 /*****************************************************************************/
/** Provides access to the face data.
  *
  * This function returns the face data (vertex indices) of the selected
  * mesh/submesh. The LOD setting is taken into account.
  *
  * @param pFaceBuffer A pointer to the user-provided buffer where the face
  *                    data is written to.
  *
  * @return The number of faces written to the buffer.
  *****************************************************************************/
int CalRenderer::getFaces(LPDIRECT3DINDEXBUFFER8 *pFaceBuffer)
{
  return m_pSelectedSubmesh->getFaces(pFaceBuffer);
}

 /*****************************************************************************/
/** Returns the number of maps.
  *
  * This function returns the number of maps in the selected mesh/submesh.
  *
  * @return The number of maps.
  *****************************************************************************/

int CalRenderer::getMapCount()
{
  // get the core material
  CalCoreMaterial *pCoreMaterial;
  pCoreMaterial = m_pModel->getCoreModel()->getCoreMaterial(m_pSelectedSubmesh->getCoreMaterialId());
  if(pCoreMaterial == 0) return 0;

  return pCoreMaterial->getMapCount();
}

 /*****************************************************************************/
/** Provides access to a specified map user data.
  *
  * This function returns the user data stored in the specified map of the
  * material of the selected mesh/submesh.
  *
  * @param mapId The ID of the map.
  *
  * @return One of the following values:
  *         \li the user data stored in the specified map
  *         \li \b 0 if an error happend
  *****************************************************************************/

Cal::UserData CalRenderer::getMapUserData(int mapId)
{
  if( !m_pSelectedSubmesh) return 0;
  // get the core material
  CalCoreMaterial *pCoreMaterial;
  pCoreMaterial = m_pModel->getCoreModel()->getCoreMaterial(m_pSelectedSubmesh->getCoreMaterialId());
  if(pCoreMaterial == 0) return 0;

  // get the map vector
  std::vector<CalCoreMaterial::Map>& vectorMap = pCoreMaterial->getVectorMap();

  // check if the map id is valid
  if((mapId < 0) || (mapId >= (int)vectorMap.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return 0;
  }

  return vectorMap[mapId].userData;
}

void CalRenderer::setMapUserData(int mapId, Cal::UserData data)
{
  if( !m_pSelectedSubmesh) return;
  // get the core material
  CalCoreMaterial *pCoreMaterial;
  pCoreMaterial = m_pModel->getCoreModel()->getCoreMaterial(m_pSelectedSubmesh->getCoreMaterialId());
  if(pCoreMaterial == 0) return;

  pCoreMaterial->setMapUserData( mapId, data);
}

 /*****************************************************************************/
/** Returns the number of attached meshes.
  *
  * This function returns the number of meshes attached to the renderer
  * instance.
  *
  * @return The number of attached meshes.
  *****************************************************************************/

int CalRenderer::getMeshCount()
{
  // get the attached meshes vector
  std::vector<CalMesh *>& vectorMesh = m_pModel->getVectorMesh();

  return vectorMesh.size();
}

 /*****************************************************************************/
/** Provides access to the normal data.
  *
  * This function returns the normal data of the selected mesh/submesh.
  *
  * @param pNormalBuffer A pointer to the user-provided buffer where the normal
  *                      data is written to.
  *
  * @return The number of normals written to the buffer.
  *****************************************************************************/

int CalRenderer::getNormals(D3DVERTEX *pNormalBuffer)
{
  // check if the submesh handles vertex data internally
  if(m_pSelectedSubmesh->hasInternalData())
  {
    // get the normal vector of the submesh
    std::vector<CalVector>& vectorNormal = m_pSelectedSubmesh->getVectorNormal();

    // get the number of normals (= number of vertices) in the submesh
    int normalCount;
    normalCount = m_pSelectedSubmesh->getVertexCount();

    // copy the internal normal data to the provided normal buffer
    memcpy(pNormalBuffer, &vectorNormal[0], normalCount * sizeof(CalVector));

    return normalCount;
  }

  m_pSelectedSubmesh->lockVertices();
  float* pBaseVertexBuffer=(float*)m_pSelectedSubmesh->getVertices();
  int normalCount = /*m_pSelectedSubmesh->getVertexCount();;/*/m_pModel->getPhysique()->calculateNormals(m_pSelectedSubmesh, pBaseVertexBuffer);
  m_pSelectedSubmesh->releaseVertices();
  return normalCount;
}

 /*****************************************************************************/
/** Returns the shininess factor.
  *
  * This function returns the shininess factor of the material of the selected
  * mesh/submesh..
  *
  * @return The shininess factor.
  *****************************************************************************/

float CalRenderer::getShininess()
{
  // get the core material
  CalCoreMaterial *pCoreMaterial;
  pCoreMaterial = m_pModel->getCoreModel()->getCoreMaterial(m_pSelectedSubmesh->getCoreMaterialId());
  if(pCoreMaterial == 0) return 50.0f;

  return pCoreMaterial->getShininess();
}

 /*****************************************************************************/
/** Provides access to the specular color.
  *
  * This function returns the specular color of the material of the selected
  * mesh/submesh.
  *
  * @param pColorBuffer A pointer to the user-provided buffer where the color
  *                     data is written to.
  *****************************************************************************/

void CalRenderer::getSpecularColor(unsigned char *pColorBuffer)
{
  // get the core material
  CalCoreMaterial *pCoreMaterial;
  pCoreMaterial = m_pModel->getCoreModel()->getCoreMaterial(m_pSelectedSubmesh->getCoreMaterialId());
  if(pCoreMaterial == 0)
  {
    // write default values to the color buffer
    pColorBuffer[0] = 255;
    pColorBuffer[1] = 255;
    pColorBuffer[2] = 255;
    pColorBuffer[3] = 0;

    return;
  }

  // get the specular color of the material
  CalCoreMaterial::Color& color = pCoreMaterial->getSpecularColor();

  // write it to the color buffer
  pColorBuffer[0] = color.red;
  pColorBuffer[1] = color.green;
  pColorBuffer[2] = color.blue;
  pColorBuffer[3] = color.alpha;
}

 /*****************************************************************************/
/** Returns the number of submeshes.
  *
  * This function returns the number of submeshes in a given mesh.
  *
  * @param meshId The ID of the mesh for which the number of submeshes should
  *               be returned..
  *
  * @return The number of submeshes.
  *****************************************************************************/

int CalRenderer::getSubmeshCount(int meshId)
{
  // get the attached meshes vector
  std::vector<CalMesh *>& vectorMesh = m_pModel->getVectorMesh();

  // check if the mesh id is valid
  if((meshId < 0) || (meshId >= (int)vectorMesh.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return 0;
  }

  return vectorMesh[meshId]->getSubmeshCount();
}

 /*****************************************************************************/
/** Provides access to the texture coordinate data.
  *
  * This function returns the texture coordinate data for a given map of the
  * selected mesh/submesh.
  *
  * @param mapId The ID of the map to get the texture coordinate data from.
  * @param pTextureCoordinateBuffer A pointer to the user-provided buffer where
  *                    the texture coordinate data is written to.
  *
  * @return The number of texture coordinates written to the buffer.
  *****************************************************************************/

int CalRenderer::getTextureCoordinates(int mapId, float *pTextureCoordinateBuffer, int imeshid, int isubmeshid)
{
  // get the texture coordinate vector vector
  std::vector<std::vector<CalCoreSubmesh::TxCoor> >& vvTxCoord = m_pSelectedSubmesh->getCoreSubmesh()->getVectorVectorTextureCoordinate();

  CalSubmesh*	am_pSelectedSubmesh = NULL;
  if( imeshid==-1 || isubmeshid==-1)
	am_pSelectedSubmesh = m_pSelectedSubmesh;
  else
  {
	std::vector<CalMesh *>& vectorMesh = m_pModel->getVectorMesh();
	am_pSelectedSubmesh = vectorMesh[imeshid]->getSubmesh(isubmeshid);
  }

  vvTxCoord = am_pSelectedSubmesh->getCoreSubmesh()->getVectorVectorTextureCoordinate();

  // check if the map id is valid
  if((mapId < 0) || (mapId >= (int)vvTxCoord.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return -1;
  }

  // get the number of texture coordinates to return
  int textureCoordinateCount;
  textureCoordinateCount = am_pSelectedSubmesh->getVertexCount();

  //am_pSelectedSubmesh->lockVertices();
  //float* pBaseVertexBuffer=am_pSelectedSubmesh->getVertices();
  float* pBaseVertexBuffer=(float*)get_vertices(am_pSelectedSubmesh);


  DWORD vstride = m_pModel->stride/sizeof(float) - 1;
  if( m_pModel->fvf&D3DFVF_DIFFUSE)	pBaseVertexBuffer += 7;
  else	pBaseVertexBuffer += 6;

  pBaseVertexBuffer += mapId*2;

  // copy the texture coordinate vector to the face buffer
  std::vector<CalCoreSubmesh::TxCoor>&	mapCoords = vvTxCoord[mapId];
  std::vector<CalCoreSubmesh::TxCoor>::iterator	coordIter = mapCoords.begin();
  for( register int i=0; i < textureCoordinateCount; i++){
	  *pBaseVertexBuffer = coordIter->u;	pBaseVertexBuffer++;
	  *pBaseVertexBuffer = coordIter->v;
	  pBaseVertexBuffer += vstride;
	  coordIter++;
  }
  //am_pSelectedSubmesh->releaseVertices();
  return textureCoordinateCount;
}

 /*****************************************************************************/
/** Returns the number of vertices.
  *
  * This function returns the number of vertices in the selected mesh/submesh.
  *
  * @return The number of vertices.
  *****************************************************************************/

int CalRenderer::getVertexCount()
{
  return m_pSelectedSubmesh->getVertexCount();
}

 /*****************************************************************************/
/** Provides access to the vertex data.
  *
  * This function returns the vertex data of the selected mesh/submesh.
  *
  * @param pVertexBuffer A pointer to the user-provided buffer where the vertex
  *                      data is written to.
  *
  * @return The number of vertices written to the buffer.
  *****************************************************************************/

int CalRenderer::getVertices(float **pVertexBuffer)
{
  // check if the submesh handles vertex data internally
  if(m_pSelectedSubmesh->hasInternalData())
  {
    // get the vertex vector of the submesh
    std::vector<CalVector>& vectorVertex = m_pSelectedSubmesh->getVectorVertex();

    // get the number of vertices in the submesh
    int vertexCount;
    vertexCount = m_pSelectedSubmesh->getVertexCount();

    // copy the internal vertex data to the provided vertex buffer
    memcpy(pVertexBuffer, &vectorVertex[0], vertexCount * sizeof(CalVector));

    return vertexCount;
  }

  // submesh does not handle the vertex data internally, so let the physique calculate it now
  int iVertCount=0;
  //обсчет вершин по скедету
  m_pSelectedSubmesh->lockVertices();
  float* pBaseVertexBuffer=m_pSelectedSubmesh->getVertices();
  iVertCount = /*m_pSelectedSubmesh->getVertexCount();/*/m_pModel->getPhysique()->calculateVertices(m_pSelectedSubmesh, pBaseVertexBuffer);

  //обсчет вершин по face`у
  *pVertexBuffer = pBaseVertexBuffer;
  m_pSelectedSubmesh->releaseVertices();
  return iVertCount;
}

 /*****************************************************************************/
/** Selects a mesh/submesh for rendering data queries.
  *
  * This function selects a mesh/submesh for further rendering data queries.
  *
  * @param meshId The ID of the mesh that should be used for further rendering
  *               data queries.
  * @param submeshId The ID of the submesh that should be used for further
  *                  rendering data queries.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalRenderer::selectMeshSubmesh(const int& meshId, const int& submeshId)
{
  // get the attached meshes vector
  std::vector<CalMesh *>& vectorMesh = m_pModel->getVectorMesh();

  // check if the mesh id is valid
  if((meshId < 0) || (meshId >= (int)vectorMesh.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  // get the core submesh
  m_pSelectedSubmesh = vectorMesh[meshId]->getSubmesh(submeshId);
  if(m_pSelectedSubmesh == 0) return false;
  m_dwSelectedSubmesh = submeshId;
  m_dwSelectedMesh = meshId;

  return true;
}

CalSubmesh* CalRenderer::getMeshSubmesh(int meshId, int submeshId)
{
  // get the attached meshes vector
  std::vector<CalMesh *>& vectorMesh = m_pModel->getVectorMesh();

  // check if the mesh id is valid
  if((meshId < 0) || (meshId >= (int)vectorMesh.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  // get the core submesh
  return vectorMesh[meshId]->getSubmesh(submeshId);
}

//****************************************************************************//

WORD CalRenderer::getVertList(float** ppVertBuffer, int imeshid, int isubmeshid)
{
	if( imeshid==-1 || isubmeshid==-1)
	{
	  *ppVertBuffer = m_pSelectedSubmesh->getVertices();
	  return (WORD)m_pSelectedSubmesh->getVertexCount();
	}else{
		std::vector<CalMesh *>& vectorMesh = m_pModel->getVectorMesh();
		*ppVertBuffer = vectorMesh[imeshid]->getSubmesh(isubmeshid)->getVertices();
		return (WORD)vectorMesh[imeshid]->getSubmesh(isubmeshid)->getVertexCount();
	}
}

WORD CalRenderer::getWireIndices(WORD** ppIndices, int imeshid, int isubmeshid)
{
	CalCoreSubmesh*	submesh = NULL;

	if( imeshid==-1 || isubmeshid==-1)
	{
	  submesh = m_pSelectedSubmesh->getCoreSubmesh();
	}else{
		std::vector<CalMesh *>& vectorMesh = m_pModel->getVectorMesh();
		submesh = vectorMesh[imeshid]->getSubmesh(isubmeshid)->getCoreSubmesh();
	}

	std::vector<CalCoreSubmesh::Edge>&	vectorEdge = submesh->getVectorEdge();
	std::vector<CalCoreSubmesh::Edge>::iterator	itEdge;

	*ppIndices = (WORD*)malloc(vectorEdge.size()*2*sizeof(WORD));
	WORD*	pIndices = *ppIndices;
	int count=0;

	for( itEdge=vectorEdge.begin(); itEdge!=vectorEdge.end(); itEdge++)
	{
		*pIndices = itEdge->iv1 + m_pSelectedSubmesh->vbStartIndex;	pIndices++;
		*pIndices = itEdge->iv2 + m_pSelectedSubmesh->vbStartIndex;	pIndices++;
		count++;
	}

	return vectorEdge.size();
}

bool CalRenderer::useVB()
{
	if( m_pSelectedSubmesh != NULL)
		return m_pSelectedSubmesh->bUseVB;

	selectMeshSubmesh(0, 0);
	if( m_pSelectedSubmesh != NULL)
		return m_pSelectedSubmesh->bUseVB;
	return false;
}


void CalRenderer::getVBs(IDirect3DVertexBuffer8 **ptr, IDirect3DVertexBuffer8 **puntr, int imeshid, int isubmeshid)
{
	assert(false);
	/*if( imeshid==-1 || isubmeshid==-1)
	{
		*ptr = NULL;	//m_pSelectedSubmesh->pVBTransformed;
		*puntr = m_pSelectedSubmesh->pVBUntransformed;
	}else{
		std::vector<CalMesh *>& vectorMesh = m_pModel->getVectorMesh();
		*ptr = NULL;	//vectorMesh[imeshid]->getSubmesh(isubmeshid)->pVBTransformed;
		*puntr = vectorMesh[imeshid]->getSubmesh(isubmeshid)->pVBUntransformed;
	}*/
}

int CalRenderer::getVertCountInModel()
{
	int	ivertcount=0;
	for( int i=0; i < getMeshCount(); i++){
		for( int j=0; j < getSubmeshCount( i); j++){
			selectMeshSubmesh(i, j);
			ivertcount += getVertexCount();
		}
	}
	return ivertcount;
}

int CalRenderer::getFaceCountInModel()
{
	int	ifacecount=0;
	for( int i=0; i < getMeshCount(); i++){
		for( int j=0; j < getSubmeshCount( i); j++){
			selectMeshSubmesh(i, j);
			ifacecount += getFaceCount();
		}
	}
	return ifacecount;
}

int CalRenderer::getEdgesCountInModel()
{
	int	iedgescount=0;
	for( int i=0; i < getMeshCount(); i++){
		std::vector<CalMesh *>& vectorMesh = m_pModel->getVectorMesh();
		if( vectorMesh[i]->getCoreMesh()->getShadowData())
			iedgescount += vectorMesh[i]->getCoreMesh()->getShadowData()->edges.size();
	}
	return iedgescount;
}

#define pi 3.141692
double arctg(double x, double y);

bool CalRenderer::getHitBoxes(HITBOX **ppHb, DWORD *count)
{
	CalSkeleton* pSkeleton;
	if( (pSkeleton=m_pModel->getSkeleton()) == NULL) return false;
	*count = pSkeleton->getVectorBone().size();
	*ppHb = m_pModel->getPhysique()->pHitBoxes;
	//
	return true;
}

//
// Calculates RAY intersection with MODEL
// params:
//  POINT		point	-	точка на экране
//  D3DMATRIX	matWrld	-	мировая матрица
//  D3DMATRIX	matProj	-	матрица проекции
//  D3DMATRIX	matView	-	матрица просмотра
//  DWORD		scrw	-	ширина экрана
//  DWORD		scrh	-	высота экрана
bool CalRenderer::isMouseOn( POINT apoint, D3DMATRIX	*matWrld, D3DMATRIX	*matProj, D3DMATRIX	*matView, DWORD		scrw, DWORD		scrh)
{
/*	Cal::CPOINT	point( apoint);

	if( m_pModel->getPickMode() == PM_FACES){

		DWORD meshCount = getMeshCount();
		int meshId;
		for(meshId = 0; meshId < meshCount; meshId++){
		int submeshCount;
		submeshCount = getSubmeshCount(meshId);
		int submeshId;
		for(submeshId = 0; submeshId < submeshCount; submeshId++)
		{
		  if(selectMeshSubmesh(meshId, submeshId))
		  {
			float*		meshVertices;
			WORD*		meshFaces;
			int			vertexCount;
			int			faceCount;

			vertexCount = getVertList(&meshVertices);
			faceCount = getFaces((WORD**)&meshFaces);
			D3DVERTEX* vertices = (D3DVERTEX*)meshVertices;

			if( useVB()){
				IDirect3DVertexBuffer7	*ptrVB;
				IDirect3DVertexBuffer7	*puntrVB;
				getVBs(&ptrVB, &puntrVB);

				D3DTLVERTEX	*ptrVert;
				if( SUCCEEDED( ptrVB->Lock( DDLOCK_WAIT | DDLOCK_READONLY , (void**)&ptrVert, NULL ) ) ) {
					for( int iface=0; iface < faceCount*3; iface+=3){
						Cal::CPOINT	pTri[3];
						pTri[0].x = ptrVert[meshFaces[iface]].sx;	 pTri[0].y = ptrVert[meshFaces[iface]].sy;
						pTri[1].x = ptrVert[meshFaces[iface+1]].sx; pTri[1].y = ptrVert[meshFaces[iface+1]].sy;
						pTri[2].x = ptrVert[meshFaces[iface+2]].sx; pTri[2].y = ptrVert[meshFaces[iface+2]].sy;
						if(!(pTri[0] == pTri[1] || pTri[0] == pTri[2] || pTri[1] == pTri[2])){
							if( IsPointInside((Cal::CPOINT*)&pTri, point, 3))	return true;
						}
					}
					ptrVB->Unlock();
				}
			}
		}}}
		endRendering();
		return false;
	}else{
	if( m_pModel->getPickMode() == PM_HITBOXES){

		D3DXVECTOR3 vPickRayDir;
		D3DXVECTOR3 vPickRayOrig;

		// Compute the vector of the pick ray in screen space
		D3DXVECTOR3 v;
		v.x =  ( ( ( 2.0f * point.x ) / scrw ) - 1 ) / matProj->_11;
		v.y = -( ( ( 2.0f * point.y ) / scrh ) - 1 ) / matProj->_22;
		v.z =  1.0f;

		// Get the inverse view matrix
		D3DMATRIX m;
		D3DXMatrixInverse( (D3DXMATRIX*)&m, NULL, (D3DXMATRIX*)matView );

		// Transform the screen space pick ray into 3D space
		vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
		vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
		vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
		vPickRayOrig.x = m._41;
		vPickRayOrig.y = m._42;
		vPickRayOrig.z = m._43;

		HITBOX*	pHits;
		DWORD	hitcount = 0;

		int facearray[12][3]={
			{0,3,1},	// bottom
			{3,2,1},
			{4,5,6},	// top
			{4,6,7},
			{1,6,5},	// back
			{1,2,6},
			{0,4,7},	// front
			{0,7,3},
			{0,5,4},	// left
			{0,1,5},
			{3,7,6},	// right
			{3,6,2}
		};

		if( getHitBoxes( &pHits, &hitcount)){
			//
			if( hitcount>0)
			{
				for( int i=0; i < hitcount; i++){
					if( !pHits[i].inited)
						continue;
					for(int j=0;j<12;j++){
						D3DXVECTOR3	v1,	v2, v3;
						D3DXVECTOR4	_v1, _v2, _v3;
						D3DXVECTOR3	resIntersect;

						v1.x = pHits[i].corners[facearray[j][0]].x; v1.y = pHits[i].corners[facearray[j][0]].y; v1.z = pHits[i].corners[facearray[j][0]].z;
						v2.x = pHits[i].corners[facearray[j][1]].x; v2.y = pHits[i].corners[facearray[j][1]].y; v2.z = pHits[i].corners[facearray[j][1]].z;
						v3.x = pHits[i].corners[facearray[j][2]].x; v3.y = pHits[i].corners[facearray[j][2]].y; v3.z = pHits[i].corners[facearray[j][2]].z;

						// Check if the pick ray passes through this point
						D3DXVec3Transform(&_v1, &v1, (D3DXMATRIX*)matWrld);
						D3DXVec3Transform(&_v2, &v2, (D3DXMATRIX*)matWrld);
						D3DXVec3Transform(&_v3, &v3, (D3DXMATRIX*)matWrld);
						_v1.x /= _v1.w; _v2.x /= _v2.w; _v3.x /= _v3.w;
						_v1.y /= _v1.w; _v2.y /= _v2.w; _v3.y /= _v3.w;
						_v1.z /= _v1.w; _v2.z /= _v2.w; _v3.z /= _v3.w;

						v1.x = _v1.x; v1.y = _v1.y; v1.z = _v1.z;
						v2.x = _v2.x; v2.y = _v2.y; v2.z = _v2.z;
						v3.x = _v3.x; v3.y = _v3.y; v3.z = _v3.z;

						if( isIntersectTriangle( vPickRayOrig, vPickRayDir, v1, v2, v3, resIntersect) )
						{
							// ?? set return parameters to valid values
							return true;
						}
					}
				}
			}
		}
		return false;
	}}*/
	return false;
}

bool CalRenderer::isIntersectTriangle( const D3DXVECTOR3& orig,
                                       const D3DXVECTOR3& dir, D3DXVECTOR3& v0,
                                       D3DXVECTOR3& v1, D3DXVECTOR3& v2, const D3DXVECTOR3& vres)
{
	float u, v;
    // Find vectors for two edges sharing vert0
    D3DXVECTOR3 edge1 = v1 - v0;
    D3DXVECTOR3 edge2 = v2 - v0;

    // Begin calculating determinant - also used to calculate U parameter
    D3DXVECTOR3 pvec;
    D3DXVec3Cross( &pvec, &dir, &edge2 );

    // If determinant is near zero, ray lies in plane of triangle
    FLOAT det = D3DXVec3Dot( &edge1, &pvec );
    if( det < 0.0001f )
        return FALSE;

    // Calculate distance from vert0 to ray origin
    D3DXVECTOR3 tvec = orig - v0;

    // Calculate U parameter and test bounds
    u = D3DXVec3Dot( &tvec, &pvec );
    if( u < 0.0f || u > det )
        return FALSE;

    // Prepare to test V parameter
    D3DXVECTOR3 qvec;
    D3DXVec3Cross( &qvec, &tvec, &edge1 );

    // Calculate V parameter and test bounds
    v = D3DXVec3Dot( &dir, &qvec );
    if( v < 0.0f || u + v > det )
        return FALSE;

    // Calculate t, scale parameters, ray intersects triangle
	D3DXVECTOR3 endPoint = orig;
	endPoint += dir;

	D3DXPLANE	plane;
	D3DXPlaneFromPoints(&plane, &v0, &v1, &v2);
	D3DXPlaneIntersectLine((D3DXVECTOR3*)&vres, &plane, (D3DXVECTOR3*)&orig, &endPoint);
    return TRUE;
}


#define SINGULAR_LINE		0
#define NORMAL_INTERSECTION 1
#define PARALLEL_LINES		2
#define COLLINEAR_LINES		3
#define NO_INTERSECTION		4
#define TOUCH_LINE			5

int CalRenderer::CLine::GetDX(){
	return (p1.x - p0.x);
}

int CalRenderer::CLine::GetDY(){
	return (p1.y - p0.y);
}

double CalRenderer::CLine::GetCosine(CFloatVector v1,CFloatVector v2){
	double len1 = v1.x*v1.x + v1.y*v1.y;
	double len2 = v2.x*v2.x + v2.y*v2.y;
	if ((len1<1e-20)||(len2<1e-20)) return 1;
	return DotProduct(v1,v2)/sqrt(len1*len2);
}

double CalRenderer::CLine::DotProduct(CFloatVector v1, CFloatVector v2){
	return v1.x*v2.x + v1.y*v2.y;
}

bool CalRenderer::CLine::IsPointOnLine(Cal::CPOINT p){
	double cosine;
	if (IsSingular())  return false;
	if ((p == p0)||(p == p1)) return true;
	CFloatVector pv(p-p0);
	CFloatVector lv(Vectorize());
	cosine = GetCosine(pv,lv);
	if ((cosine>0.99999999)&&(pv.GetLength()<=lv.GetLength())) return true;
	return false;
}

CalRenderer::CFloatVector CalRenderer::CFloatVector::operator =(CFloatVector v){
	x = v.x;
	y = v.y;
	return *this;
}

CalRenderer::CFloatVector CalRenderer::CFloatVector::operator *(CFloatVector v){
	return CFloatVector(x*v.x, y* v.y);
}

CalRenderer::CFloatVector::CFloatVector(Cal::CPOINT point){
	x = point.x;
	y = point.y;
}

CalRenderer::CFloatVector::CFloatVector(double xx, double yy){
	x = xx;
	y = yy;
}


CalRenderer::CFloatVector CalRenderer::CFloatVector::operator *(double scale){
	return CFloatVector(x*scale,y*scale);
}

CalRenderer::CFloatVector CalRenderer::CFloatVector::operator *=(CFloatVector v){
	x *= v.x;
	y *= v.y;
	return *this;
}



CalRenderer::CFloatVector CalRenderer::CFloatVector::operator *=(double t){
	x *= t;
	y *= t;
	return *this;
}

double CalRenderer::CFloatVector::GetLength(){
	return sqrt(x*x+y*y);
}

bool CalRenderer::CFloatVector::IsVertical(){
	return (x==0);
}

bool CalRenderer::CFloatVector::IsHorizontal(){
	return (y==0);
}

CalRenderer::CFloatVector CalRenderer::CFloatVector::operator =(Cal::CPOINT point){
	x = point.x;
	y = point.y;
	return *this;
}

double CalRenderer::GetCosine(CFloatVector v1,CFloatVector v2){
	double len1 = v1.x*v1.x + v1.y*v1.y;
	double len2 = v2.x*v2.x + v2.y*v2.y;
	if ((len1<1e-20)||(len2<1e-20)) return 1;
	return DotProduct(v1,v2)/sqrt(len1*len2);
}

double CalRenderer::DotProduct(CFloatVector v1, CFloatVector v2){
	return v1.x*v2.x + v1.y*v2.y;
}


CalRenderer::CFloatVector CalRenderer::CFloatVector::operator +(CFloatVector v){
	return CFloatVector(x+v.x, y+v.y);
}

CalRenderer::CFloatVector CalRenderer::CFloatVector::operator -(CFloatVector v){
	return CFloatVector(x-v.x, y-v.y);
}

void CalRenderer::CFloatVector::MakeSine(int angle){
	double radian;
	radian = ((double)(angle&0xffff))*9.58737992428525768573804743432465e-5;
	x = cos(radian);
	y = sin(radian);
}

CalRenderer::CLine::CLine(){
	p0.x = p1.x = p0.y = p1.y = 0;
}

CalRenderer::CLine::~CLine()
{

}

CalRenderer::CLine::CLine(Cal::CPOINT const& v0, Cal::CPOINT const& v1){
	p0 = v0;
	p1 = v1;
}

CalRenderer::CLine::CLine(int x0, int y0, int x1, int y1){
	Cal::CPOINT v0(x0,y0);
	Cal::CPOINT v1(x1,y1);
	p0 = v0;
	p1 = v1;
}

double CalRenderer::CLine::GetLength(){
	double dx = p1.x - p0.x;
	double dy = p1.y - p0.y;
	return sqrt(dx*dx + dy*dy);
}


CalRenderer::CFloatVector const CalRenderer::CLine::Vectorize(){
	return CFloatVector(p1-p0);
}



CalRenderer::CFloatVector CalRenderer::CLine::GetPoint(double t){
	return CFloatVector((CFloatVector)(p1-p0)*t + p0);
}

bool CalRenderer::CLine::IsVertical(float tolerance){
	if( tolerance==0.0)
		return (!IsSingular()&&(p0.x == p1.x));
	else{
		float angle = arctg( p0.x-p1.x, p0.y-p1.y);
		if( angle < 0) angle*=-1;
		if( angle > 3.14 /*180*/)
			angle = angle - 3.14;
		if( angle <= 1.57 + tolerance*3.14/180.0f || angle >= 1.57 + tolerance*3.14/180.0f)
			return !IsSingular();
		else
			return false;
	}
}

bool CalRenderer::CLine::IsSingular(){
	return ((p0==p1)!=0);
}

bool CalRenderer::CLine::IsHorizontal(float tolerance){
	if( tolerance==0.0)
		return (!IsSingular()&&(p0.y == p1.y));
	else{
		float angle = arctg( p0.x-p1.x, p0.y-p1.y);
		if( angle < 0) angle*=-1;
		if( angle > 3.14 /*180*/)
			angle = angle - 3.14;
		if( angle > 1.57 /*90*/)
			angle = 3.14 - angle;
		if( angle < tolerance*3.14/180.0f)
			return !IsSingular();
		else
			return false;
	}
}


bool CalRenderer::IsPointInside(Cal::CPOINT *lpPoints, Cal::CPOINT CheckPoint, int PointCount){
	int i;
	int intersection_count;
	CLine line;
	bool FoundResult;
	CFloatVector v;


	if (PointCount<3) return false;

	CLine *lpLines = new CLine [PointCount];

	CLine *pLine = lpLines;
	for (i = 0; i<PointCount; i++)
	{
		line.p0 = lpPoints[i];
		line.p1 = lpPoints[(i+1)%PointCount];
		(*pLine) = line;
		pLine++;
	}

	//
	pLine = lpLines;
	//
	/*if( pLine[0].IsHorizontal(10.0) && pLine[2].IsHorizontal(10.0) && pLine[1].IsVertical(10.0) && pLine[3].IsVertical(10.0)){
		Cal::CRECT	rbound;
		rbound.left = pLine[0].p0.x<=pLine[0].p1.x?pLine[0].p0.x:pLine[0].p1.x;
		rbound.right = pLine[0].p0.x>pLine[0].p1.x?pLine[0].p0.x:pLine[0].p1.x;
		rbound.top = pLine[1].p0.y<=pLine[1].p1.y?pLine[1].p0.y:pLine[1].p1.y;
		rbound.bottom = pLine[1].p0.y>pLine[1].p1.y?pLine[1].p0.y:pLine[1].p1.y;

		if( rbound.PtInRect( CheckPoint))
			return true;
	}
	if( pLine[1].IsHorizontal(10.0) && pLine[3].IsHorizontal(10.0) && pLine[0].IsVertical(10.0) && pLine[2].IsVertical(10.0)){
		Cal::CRECT	rbound;
		rbound.left = pLine[1].p0.x<=pLine[1].p1.x?pLine[1].p0.x:pLine[1].p1.x;
		rbound.right = pLine[1].p0.x>pLine[1].p1.x?pLine[1].p0.x:pLine[1].p1.x;
		rbound.top = pLine[0].p0.y<=pLine[0].p1.y?pLine[0].p0.y:pLine[0].p1.y;
		rbound.bottom = pLine[0].p0.y>pLine[0].p1.y?pLine[0].p0.y:pLine[0].p1.y;

		if( rbound.PtInRect( CheckPoint))
			return true;
	}*/
	//
	for (i = 0; i<PointCount;i++,pLine++)
	{
		if (pLine->IsPointOnLine(CheckPoint))
		{
			delete [] lpLines;
			return true;
		}
	}

	int irep=0;
	do
	{
		double t;
		int rv;
		v.MakeSine(rand()); 
		FoundResult = true;
		intersection_count = 0;
		pLine = lpLines;
		for (i = 0; i<PointCount; i++,pLine++)
		{                                                                                                                                 
			if (!Intersection(*pLine,CheckPoint,v,&t,rv))
			{
				FoundResult = false;
				break;
			}
			if (rv == NORMAL_INTERSECTION) intersection_count++;
		}
	}while(!FoundResult && ++irep < PointCount);
	
	delete [] lpLines;
	return !FoundResult ? false : ((intersection_count&1)==1);
}

//true if no error
//false ig was error
bool CalRenderer::Intersection (CLine line, Cal::CPOINT BasePoint, CFloatVector v, double *t1, int &rValue){
	CFloatVector lv = line.Vectorize();
	CFloatVector pnt(BasePoint);
	double t = 0;

	if (line.IsSingular()){
		rValue = SINGULAR_LINE;
		*t1 = t;
		return false;
	}

	if (line.IsHorizontal()){
		if (v.y == 0){
			if (BasePoint.y == line.p0.y){
				rValue = COLLINEAR_LINES;
				*t1 = t;
				return false;
			}
			rValue = PARALLEL_LINES;
			*t1 = t;
			return true;
		}
		if (abs(GetCosine(v,lv)) > 0.999999999){
			rValue = PARALLEL_LINES;
			*t1 = t;
			return true;
		}
		double hx = v.x*((double)(line.p0.y - BasePoint.y))/v.y;
		double int_X = BasePoint.x + hx;
		t = (int_X - line.p0.x)/line.GetDX();
		if (DotProduct(line.GetPoint(t)-pnt,v)<0){
			*t1 = t;
			rValue = NO_INTERSECTION;
			return true;
		}
		if ((t<0)||(t>1)){
			rValue = NO_INTERSECTION;
			*t1 = t;
			return true;
		}
		if ((t == 0)||(t == 1))
		if (DotProduct(line.GetPoint(t)-pnt,v)<0){
			*t1 = t;
			rValue = NO_INTERSECTION;
			return true;
		}
		{
			rValue = TOUCH_LINE;
			*t1 = t;
			return false;
		}
		rValue = NORMAL_INTERSECTION;
		*t1 = t;
		return true;
	}

	if (line.IsVertical()){
		if (v.x == 0){
			if (BasePoint.x == line.p0.x){
				rValue = COLLINEAR_LINES;
				*t1 = t;
				return false;
			}
			rValue = PARALLEL_LINES;
			*t1 = t;
			return true;
		}

		if (abs(GetCosine(v,lv)) > 0.999999999){
			rValue = PARALLEL_LINES;
			*t1 = t;
			return true;
		}

		double hy = v.y*((double)(line.p0.x - BasePoint.x))/v.x;
		double int_Y = BasePoint.y + hy;
		
		t = (int_Y - line.p0.y)/line.GetDY();
		if (DotProduct(line.GetPoint(t)-pnt,v)<0){
			*t1 = t;
			rValue = NO_INTERSECTION;
			return true;
		}
		if ((t<0)||(t>1)){
			rValue = NO_INTERSECTION;
			*t1 = t;
			return true;
		}
		if ((t == 0)||(t == 1)){
			rValue = TOUCH_LINE;
			*t1 = t;
			return false;
		}
		rValue = NORMAL_INTERSECTION;
		*t1 = t;
		return true;
	}

	if (abs(GetCosine(v,lv)) > 0.999999999){
		rValue = PARALLEL_LINES;
		*t1 = t;
		return true;
	}

	
	if (v.IsVertical()){
		t = ((BasePoint.x - (double)line.p0.x)/(double)line.GetDX());

		if (DotProduct(line.GetPoint(t)-pnt,v)<0){
			*t1 = t;
			rValue = NO_INTERSECTION;
			return true;
		}
		if ((t<0)||(t>1)){
			rValue = NO_INTERSECTION;
			*t1 = t;
			return true;
		}
		if ((t == 0)||(t == 1)){
			rValue = TOUCH_LINE;
			*t1 = t;
			return false;
		}
		rValue = NORMAL_INTERSECTION;
		*t1 = t;
		return true;
	}
	if (v.IsHorizontal()){
		t = ((double)(BasePoint.y - (double)line.p0.y)/(double)line.GetDY());

		if (DotProduct(line.GetPoint(t)-pnt,v)<0){
			*t1 = t;
			rValue = NO_INTERSECTION;
			return true;
		}
		if ((t<0)||(t>1)){
			rValue = NO_INTERSECTION;
			*t1 = t;
			return true;
		}
		if ((t == 0)||(t == 1)){
			rValue = TOUCH_LINE;
			*t1 = t;
			return false;
		}
		rValue = NORMAL_INTERSECTION;
		return true;
	}

	CFloatVector bp(BasePoint);
	bp.y += v.y*((double)line.p0.x - bp.x)/v.x;

	CFloatVector lBase(0,(double)line.p0.y - bp.y);
	double xi = lBase.y/(v.y/v.x - lv.y/lv.x);
	t = xi/lv.x;

	CFloatVector vec = line.GetPoint(t) - pnt;
		
	if (DotProduct(vec,v)<0){
		*t1 = t;
		rValue = NO_INTERSECTION;
		return true;
	}
	if ((t<0)||(t>1)){
		rValue = NO_INTERSECTION;
		*t1 = t;
		return true;
	}
	if ((t == 0)||(t == 1))
	{
		rValue = TOUCH_LINE;
		*t1 = t;
		return false;
	}
	rValue = NORMAL_INTERSECTION;
	*t1 = t;
	return true;

}

double arctg(double x, double y){
	double _arctg=0;
	if((x>0)&&(y==0)) _arctg=0; else
	if((x<0)&&(y==0)) _arctg=pi; else
	if((x==0)&&(y>0)) _arctg=0.5*pi; else
	if((x==0)&&(y<0)) _arctg=1.5*pi; else
	if((x>0)&&(y>0)) _arctg=atan2(abs(y),abs(x)); else
	if((x<0)&&(y>0)) _arctg=pi-atan2(abs(y),abs(x)); else
	if((x<0)&&(y<0)) _arctg=pi+atan2(abs(y),abs(x)); else
	if((x>0)&&(y<0)) _arctg=2*pi-atan2(abs(y),abs(x));
	return _arctg;
}

CalCoreMaterial* CalRenderer::getMaterial()
{
	//return &m_pSelectedSubmesh->material;
	CalCoreMaterial *pCoreMaterial;
	pCoreMaterial = m_pModel->getCoreModel()->getCoreMaterial(m_pSelectedSubmesh->getCoreMaterialId());
	return pCoreMaterial;
}

void CalRenderer::get_FAP_ctrl_points(LPVOID *array, DWORD &count)
{
	count = 0;

	CMpeg4FDP*	face = m_pModel->m_pFAPU;
	if( !face)	return;

	CalRenderer* rend = m_pModel->getRenderer();

	int mc, sc;
	std::vector<CalMesh *>& vectorMesh = m_pModel->getVectorMesh();
	mc = vectorMesh.size();

	std::vector<CalSubmesh *>& vectorSubmesh = vectorMesh[0]->getVectorSubmesh();
	sc = vectorSubmesh.size();

	LPVOID	v_array[50];

	if( !m_pModel->getRenderer()->lockVertices())	return;
	// lock vertices
	int i;
	for(i=0; i<sc; i++)
	{
		//vectorSubmesh[i]->lockVertices();
		//v_array[i] = (LPVOID)vectorSubmesh[i]->pVertData;
		v_array[i] = (LPVOID)m_pModel->getRenderer()->get_vertices(vectorSubmesh[i]);
	}
	//

	for( i=0; i<MAX_CONTROL_GROUP; i++)
	{
		int gid = i;
		//
		for( int in=0; in<MAX_CONTROL_POINTS_INGROUP; in++)
		{
			if( !face->n_points[gid][in])	continue;

			control_point*	cp = face->n_points[gid][in];

			for( int isp=0; isp<cp->points.size(); isp++)
			{
				submesh_face_point sp = cp->points[isp];
				//
				LPVOID	rpa = v_array[ sp.submesh_id];
				rpa = (LPVOID)((DWORD)rpa + sp.id*m_pModel->stride);
				//
				array[cp->group_id*MAX_CONTROL_POINTS_INGROUP+cp->id] = rpa;
//				array++;
				count++;
			}
		}
		//
	}
	//for( i=0; i<sc; i++)	vectorSubmesh[i]->releaseVertices();
	m_pModel->getRenderer()->releaseVertices();
}

// ... даже не буду вспоминать как это работает. Бесполезно!
// Надо переделать
bool CalRenderer::create_influences(int istep, CMpeg4FDP *face)
{
	int iistep = 1;

	if( istep != -1)
		iistep = istep;

	std::vector<CalMesh *>& vectorMesh = m_pModel->getVectorMesh();
	CalSubmesh* smesh  = NULL;
	CalCoreSubmesh* coresmesh = NULL;

	//
	// запомним число вершин
	int mc = 1;
	int	submeshvcount[20];
	submeshvcount[0] = 0;
	DWORD meshCount = getMeshCount();
	int meshId;
	for(meshId = 0; meshId < meshCount; meshId++){
		int submeshCount;
		submeshCount = getSubmeshCount(meshId);
		int submeshId;
		for(submeshId = 0; submeshId < submeshCount; submeshId++)
		{
			submeshvcount[mc] = submeshvcount[mc-1]+getMeshSubmesh(meshId, submeshId)->getVertexCount();
			mc++;
		}
	}

	int*	free_vertices = (int*)calloc(1, submeshvcount[mc-1]*sizeof(int));
	for( int k=0; k<submeshvcount[mc-1]; k++)	free_vertices[k] = -1;

	while(true)
	{
		// добавили все контрольные точки
		if(iistep==1)
		{
			for( int ig=0; ig<MAX_CONTROL_GROUP; ig++)
			{
				for( int in=0; in<MAX_CONTROL_POINTS_INGROUP; in++)
				{
					if( !face->n_points[ig][in])	continue;
					control_point* pcp = face->n_points[ig][in];

					for( int i=0; i<pcp->points.size(); i++)
					{
						submesh_face_point	s = pcp->points[i];
						if (s.weight > 0.99f || s.weight == 0.0f)
						{
							s.influences.push_back(ctrlp_infl(s.parent_gid, s.parent_id, 0.0));
							face->infl_array.push_back( s);
							(&face->infl_array[face->infl_array.size()-1])->weight = 0;
							free_vertices[submeshvcount[pcp->points[i].submesh_id]+pcp->points[i].id] = face->infl_array.size()-1;
						}
					}

				}
			}
			face->lastindex=0;
			iistep++;
			if( istep != -1)	return false;
			continue;
		}
		// алгоритм определения зон
		// 1. по всем последним добавленным точкам
		// 	1.1 по всем соселям точки
		// 	  1.1.1 если сосед уже в данном массиве, то повторяем итерацию
		// 	  1.1.2 если сосед не в массиве, то присваиваем ему контрольную точку, такую же как и у его соседа, 
		// 			и к расстоянию прибавляем расстояние от него до соседа
		// 	1.2 запоминаем длину массива массива из 1.1 для вычисления числа новых точек
		// 2. если не добавили ни одной точки, то выходим
		 // get the attached meshes vector
int aaaaaaa=0;
		submesh_face_point	fp;
		CalCoreSubmesh::Neighbour*	pneibs;
		//m_pSelectedSubmesh = vectorMesh[meshId]->getSubmesh(submeshId);

		int cur_size=face->lastindex;
		face->lastindex = face->infl_array.size();
		for( int i=cur_size; i<face->lastindex; i++)
		{

/*{
CalCoreSubmesh*	submesh = getMeshSubmesh(0, 0)->getCoreSubmesh();
CalCoreSubmesh::Neighbour* pnbrs = &submesh->getVectorNeighbour()[165];
if( free_vertices[submeshvcount[0]+165] != -1)
{
	for( int icnbr1=0; icnbr1<pnbrs->nbrs.size(); icnbr1++)
	{
		int indx=submeshvcount[0]+pnbrs->nbrs[icnbr1].id;
		if( free_vertices[indx] == -1) continue;
		submesh_face_point*	sfpnb2 = &face->infl_array[free_vertices[indx]];
//		char buf[255]; memset( &buf, 0, 255);
		sprintf( buf, "neightbour = %d, %.2f\n", sfpnb2->id, pnbrs->nbrs[icnbr1].dist);OutputDebugString( buf);
	}
	char buf[255]; memset( &buf, 0, 255);
	sprintf( buf, "----------------------\n");OutputDebugString( buf);
	int iiiii=0;
}
}*/


			fp = face->infl_array[i];

			if( fp.influences.size()>1)	continue;

			smesh = getMeshSubmesh(fp.mesh_id,fp.submesh_id);
			coresmesh = smesh->getCoreSubmesh();
			std::vector<CalCoreSubmesh::Neighbour>&	nbrs = coresmesh->getVectorNeighbour();
//			std::vector<CalCoreSubmesh::Edge>&	edges = coresmesh->getVectorEdge();

			pneibs = &nbrs[fp.id];	// список соседей данной точки

			for( int in=0; in<pneibs->nbrs.size(); in++)
			{
				CalCoreSubmesh::NDef nbrnum = pneibs->nbrs[in];

				if(nbrnum.id == 165)
					int iiii=0;

				if( face->is_control_point(fp.mesh_id,fp.submesh_id,nbrnum.id)) continue;

				bool exist=false;
				if(free_vertices[submeshvcount[fp.submesh_id]+nbrnum.id] != -1)	// если такая точка уже была, то
				{
					exist=true;

					submesh_face_point*	arfp;
					for( int ia=0; ia<face->infl_array.size(); ia++)
					{
						arfp = &face->infl_array[ia];

						if( arfp->mesh_id!=fp.mesh_id || arfp->submesh_id!=fp.submesh_id || arfp->id!=nbrnum.id)
							continue;

						bool samesparents=false;
						for(int iai=0; iai<arfp->influences.size(); iai++)
							// если эта точка принадлежит другой контрольной точке
							if(arfp->influences[iai].parent_gid==fp.influences[0].parent_gid && 
								arfp->influences[iai].parent_id==fp.influences[0].parent_id)
								samesparents = true;

						if( !samesparents){
							// если не нашли такого родителя, то
							//	1. У всех точек, соседних с найденной и имеющих вес 0.0 добавим новую контрольную точку
							CalCoreSubmesh*	submesh = getMeshSubmesh(fp.mesh_id, fp.submesh_id)->getCoreSubmesh();
							CalCoreSubmesh::Neighbour* pnbrs = &submesh->getVectorNeighbour()[arfp->id];

							for( int icnbr=0; icnbr<pnbrs->nbrs.size(); icnbr++)
							{
								int nidx = pnbrs->nbrs[icnbr].id;
								if( pnbrs->nbrs[icnbr].dist != 0.0)	continue;
								int idx = free_vertices[submeshvcount[fp.submesh_id]+nidx];

								if( idx == -1)
								{
									int ii=0;
									continue;
								}
								submesh_face_point*	sfpnb = &face->infl_array[idx];
								/*if( sfpnb->id == 165)
								{
									int ii=0;
									for( int icnbr1=0; icnbr1<pnbrs->nbrs.size(); icnbr1++)
									{
										submesh_face_point*	sfpnb2 = &face->infl_array[free_vertices[submeshvcount[fp.submesh_id]+pnbrs->nbrs[icnbr1].id]];
										char buf[255]; memset( &buf, 0, 255);
										//sprintf( buf, "neightbour = %d, %.2f\n", sfpnb2->id, pnbrs->nbrs[icnbr1].dist);OutputDebugString( buf);
									}
								}*/
								//for(int iai=0; iai<sfpnb->influences.size(); iai++)
								//{
									// add neighbours to control points
									//face->n_points[fp.influences[0].parent_gid][fp.influences[0].parent_id]->add_neighbour(arfp->influences[iai].parent_gid,arfp->influences[iai].parent_id,arfp->influences[iai].weight+fp.influences[0].weight);
									//face->n_points[arfp->influences[iai].parent_gid][arfp->influences[iai].parent_id]->add_neighbour(fp.influences[0].parent_gid,fp.influences[0].parent_id,arfp->influences[iai].weight+fp.influences[0].weight);
									sfpnb->influences.push_back(ctrlp_infl(fp.influences[0].parent_gid,fp.influences[0].parent_id,fp.influences[0].weight + nbrnum.dist));
								//}
							}
							if( arfp->id == 165)
								int ii=0;
							arfp->influences.push_back(ctrlp_infl(fp.influences[0].parent_gid,fp.influences[0].parent_id,fp.influences[0].weight + nbrnum.dist));
						}
						break;		// found same vertex
					}
				}

				if( !exist)		// add new point
				{
					submesh_face_point	new_point(fp.mesh_id,fp.submesh_id,nbrnum.id,0);
					new_point.influences.push_back(ctrlp_infl(fp.influences[0].parent_gid,fp.influences[0].parent_id,fp.influences[0].weight + nbrnum.dist));
					int iid = nbrnum.id;
						if( iid == 165)
							int ii=0;
					aaaaaaa++;
					if(aaaaaaa == 165)
						int k=0;
					face->infl_array.push_back(new_point);
					free_vertices[submeshvcount[fp.submesh_id]+nbrnum.id] = face->infl_array.size()-1;

					CalCoreSubmesh*	submesh = getMeshSubmesh(fp.mesh_id, fp.submesh_id)->getCoreSubmesh();
					CalCoreSubmesh::Neighbour* pnbrs = &submesh->getVectorNeighbour()[nbrnum.id];

					for( int icnbr=0; icnbr<pnbrs->nbrs.size(); icnbr++)
					{
						if( pnbrs->nbrs[icnbr].dist != 0.0)	continue;
						submesh_face_point	new_point(fp.mesh_id,fp.submesh_id,pnbrs->nbrs[icnbr].id,0);
						int iid = pnbrs->nbrs[icnbr].id;
						if( iid == 267)
							int ii=0;
						aaaaaaa++;
						if(aaaaaaa == 25)
						int k=0;
						new_point.influences.push_back(ctrlp_infl(fp.influences[0].parent_gid,fp.influences[0].parent_id,fp.influences[0].weight + nbrnum.dist));
						face->infl_array.push_back(new_point);
						free_vertices[submeshvcount[fp.submesh_id]+iid] = face->infl_array.size()-1;
						int k1 = pnbrs->nbrs[4].id;
						int k2 = pnbrs->nbrs[5].id;
					}
				}
			}
		}
		if(face->lastindex==face->infl_array.size() || istep != -1/*DEBUG*/)	break;
	}
	//
	if( istep!=-1 && face->lastindex!=face->infl_array.size())
		return true;
	
	// 0. очистим массивы влияния ключевых точек
	// для каждой неключевой точки из face->infl_array
	//  1. определим ключевую точку к которой она принадлежит
	//  2. рассчитаем коэффициенты влияния на данную точку
	//  3. добавим в массив ключевой точки данную точку
	int ig;
	for(ig=0; ig<MAX_CONTROL_GROUP; ig++)
	{
		for( int in=0; in<MAX_CONTROL_POINTS_INGROUP; in++)
		{
			if( !face->n_points[ig][in])	continue;
			//
			for( int i=0; i<face->n_points[ig][in]->points.size(); i++)
			{
				//(&face->n_points[ig][in]->points[i])->weight = 1.0;;
				face->n_points[ig][in]->points.clear();
			}
		}
	}

	for( int i=0; i<face->infl_array.size(); i++)
	{
		control_point*	cp;
		submesh_face_point*	fp;

		fp = &face->infl_array[i];

		if( fp->parent_gid == 8 && fp->parent_id == 2)
			int iasd=0;
		if( fp->id == 14)
			int i=0;

		int inf_count = 0;
		float	weight_sum = 0.0;
		float	max_weight = -FLT_MAX;
		float	max_inverse_weight = 0;
		int		ii=0;
		for( ii=0; ii<fp->influences.size(); ii++)	
			if(fp->influences[ii].weight < 6)
			{
				weight_sum += fp->influences[ii].weight;
				if( max_weight < fp->influences[ii].weight)
					max_weight = fp->influences[ii].weight;
				inf_count++;
			}
		for( ii=0; ii<fp->influences.size(); ii++)	
			if(fp->influences[ii].weight < 6)
			{
				if( fp->influences[ii].weight != 0)
					max_inverse_weight += (1 / fp->influences[ii].weight);
			}

		/*if(weight_sum == 0.0)			// if this is a control point it self
			continue;*/

		for( ii=0; ii<fp->influences.size(); ii++)
		{
			ctrlp_infl	influence = fp->influences[ii];	// control point infl

			if(fp->influences.size() > 1)
				int iii=0;

			assert(face->n_points[influence.parent_gid][influence.parent_id]);
			// control point for this point
			cp =   face->n_points[influence.parent_gid][influence.parent_id];

			smesh = vectorMesh[fp->mesh_id]->getSubmesh(fp->submesh_id);
			coresmesh = smesh->getCoreSubmesh();

			float wn = inf_count - weight_sum/max_weight + 0.1;

			if(influence.weight < 6.0)
			{
				submesh_face_point	face_point;
				if( influence.weight == 0.0)	face_point.weight = 1.0f;
				else	{
					face_point.weight = 1/influence.weight;
					face_point.weight /= max_inverse_weight;

					//if( max_weight=influence.weight)	face_point.weight = 1;

					face_point.weight *= (1 - max_weight/20.0f);
				}

				face_point.id = fp->id;
				face_point.submesh_id = fp->submesh_id;
				face_point.mesh_id = fp->mesh_id;

				std::vector<submesh_face_point>::iterator	cpit;
				for( cpit=cp->points.begin(); cpit!=cp->points.end(); cpit++)
					if(cpit->id == fp->id)
						break;
				if(cpit==cp->points.end() || cp->points.empty())
					cp->points.push_back( face_point);
			}
		}
	}

	// добавим в массив сами ключевые точки
	/*control_point*	cp=NULL;
	for( ig=0; ig<MAX_CONTROL_GROUP; ig++)
	{
		for( int in=0; in<MAX_CONTROL_POINTS_INGROUP; in++)
		{
			if( !face->n_points[ig][in])	continue;
			cp =  face->n_points[ig][in];
			//
			submesh_face_point	face_point;
			// mpeg data
			face_point.weight = 1.0;
			// mesh data
			face_point.mesh_id = cp->mesh_id;
			face_point.submesh_id = cp->submesh_id;
			face_point.id = cp->id;
			cp->points.push_back( face_point);
		}
	}*/

	for( ig=0; ig<MAX_CONTROL_GROUP; ig++)
	{
		for( int in=0; in<MAX_CONTROL_POINTS_INGROUP; in++)
		{
			if( !face->n_points[ig][in])	continue;
			//
			control_point*	cp = face->n_points[ig][in];
int iii;
for(iii=0; iii<cp->points.size(); iii++)
{
	submesh_face_point*	ffp = &cp->points[iii];
}
			std::sort(cp->points.begin(), cp->points.end());
for( iii=0; iii<cp->points.size(); iii++)
{
	submesh_face_point*	ffp = &cp->points[iii];
}
		}
	}

	return true;
}

int CalRenderer::getMapType(int mapId)
{
 if( !m_pSelectedSubmesh) return 0;
  // get the core material
  CalCoreMaterial *pCoreMaterial;
  pCoreMaterial = m_pModel->getCoreModel()->getCoreMaterial(m_pSelectedSubmesh->getCoreMaterialId());
  if(pCoreMaterial == 0) return 0;

  // get the map vector
  std::vector<CalCoreMaterial::Map>& vectorMap = pCoreMaterial->getVectorMap();

  // check if the map id is valid
  if((mapId < 0) || (mapId >= (int)vectorMap.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return 0;
  }

  return vectorMap[mapId].type;
}

bool CalRenderer::lockVertices()
{
	if( !pVertexBuffer)	return false;
    if( FAILED( pVertexBuffer->Lock( 0, 0, (BYTE**)&pVertData, 0)))
	{
		assert(false);
		return false;
	}
	return true;
}

void CalRenderer::releaseVertices()
{
	if( !pVertexBuffer)	return;
	pVertexBuffer->Unlock();
}

LPVOID CalRenderer::get_vertices(CalSubmesh *psm)
{
	return (LPVOID)((DWORD)pVertData + psm->vbStartIndex*psm->stride);
}

bool CalRenderer::lockIndices()
{
    if( FAILED( pIndexBuffer->Lock( 0, 0, (BYTE**)&pIndexData, 0)))
		assert(false);
	return true;
}

void CalRenderer::releaseIndices()
{
	pIndexBuffer->Unlock();
}

LPVOID CalRenderer::get_indices(CalSubmesh *psm)
{
	return (LPVOID)((DWORD)pIndexData + psm->ibStartIndex*sizeof(WORD));
}


CalShadowData* CalRenderer::getShadowData(int id)
{
  // get the attached meshes vector
  std::vector<CalMesh *>& vectorMesh = m_pModel->getVectorMesh();

  // check if the mesh id is valid
  if((id < 0) || (id >= (int)vectorMesh.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }
  return vectorMesh[id]->getCoreMesh()->getShadowData();
}

int CalRenderer::getSubmeshVertCount(int meshId, int submeshId)
{
	CalSubmesh*	sm = getMeshSubmesh(meshId, submeshId);
	if(!sm)
	{
		return 0;
	}
	return sm->getVertexCount();
}

void CalRenderer::recalculateUVW(const int& meshId, const int& submeshId, const CalVector& xTr, const CalVector& yTr, const int& txCoords)
{
	if( !selectMeshSubmesh(meshId, submeshId))	return;
	if( !m_pSelectedSubmesh)	return;
	int stride = m_pModel->stride/sizeof(float);
	if (m_pModel->getRenderer()->lockVertices()) 
	{
		float* pBaseVertexBuffer=(float*)m_pModel->getRenderer()->get_vertices(m_pSelectedSubmesh);
		int	vertCount = m_pSelectedSubmesh->getVertexCount();
		int vertexId;
		int vertexCount = m_pSelectedSubmesh->getVertexCount();
		int addIndex = txCoords*2;
		for(vertexId = 0; vertexId < vertexCount; vertexId++)
		{
			pBaseVertexBuffer[6 + addIndex] = (pBaseVertexBuffer[6 + addIndex]-xTr.x)*xTr.y + xTr.z;
			pBaseVertexBuffer[7 + addIndex] = (pBaseVertexBuffer[7 + addIndex]-yTr.x)*yTr.y + yTr.z;

			// next vertex position in buffer
			pBaseVertexBuffer += stride;
		}
		m_pModel->getRenderer()->releaseVertices();
	}
}
