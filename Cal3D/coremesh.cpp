//****************************************************************************//
// coremesh.cpp                                                               //
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

#include "coremesh.h"
#include "error.h"
#include "memory_leak.h"

 /*****************************************************************************/
/** Constructs the core mesh instance.
  *
  * This function is the default constructor of the core mesh instance.
  *****************************************************************************/

CalCoreMesh::CalCoreMesh():
	MP_VECTOR_INIT(m_vectorCoreSubmesh),
	MP_VECTOR_INIT(m_vectorEdge)
{
	m_bNoSkel = false;
	m_worldTranslation.set(0,0,0);
	m_worldRotation = CalQuaternion(0,0,0,1);
	m_pShadowData = NULL;
}

 /*****************************************************************************/
/** Destructs the core mesh instance.
  *
  * This function is the destructor of the core mesh instance.
  *****************************************************************************/

CalCoreMesh::~CalCoreMesh()
{
  assert(m_vectorCoreSubmesh.empty());
  assert(m_pShadowData==NULL);
}

 /*****************************************************************************/
/** Adds a core submesh.
  *
  * This function adds a core submesh to the core mesh instance.
  *
  * @param pCoreSubmesh A pointer to the core submesh that should be added.
  *
  * @return One of the following values:
  *         \li the assigned submesh \b ID of the added core submesh
  *         \li \b -1 if an error happend
  *****************************************************************************/

int CalCoreMesh::addCoreSubmesh(CalCoreSubmesh *pCoreSubmesh)
{
  // get next bone id
  int submeshId;
  submeshId = m_vectorCoreSubmesh.size();

  m_vectorCoreSubmesh.push_back(pCoreSubmesh);

  return submeshId;
}

 /*****************************************************************************/
/** Creates the core mesh instance.
  *
  * This function creates the core mesh instance.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreMesh::create()
{
  return true;
}

 /*****************************************************************************/
/** Destroys the core mesh instance.
  *
  * This function destroys all data stored in the core mesh instance and frees
  * all allocated memory.
  *****************************************************************************/

void CalCoreMesh::destroy()
{
  // destroy all core submeshes
  std::vector<CalCoreSubmesh *>::iterator iteratorCoreSubmesh;
  for(iteratorCoreSubmesh = m_vectorCoreSubmesh.begin(); iteratorCoreSubmesh != m_vectorCoreSubmesh.end(); ++iteratorCoreSubmesh)
  {
    (*iteratorCoreSubmesh)->destroy();
    MP_DELETE( *iteratorCoreSubmesh);
  }
  m_vectorCoreSubmesh.clear();
  if( m_pShadowData != NULL)
	  MP_DELETE( m_pShadowData);
  m_pShadowData = NULL;
}

 /*****************************************************************************/
/** Provides access to a core submesh.
  *
  * This function returns the core submesh with the given ID.
  *
  * @param id The ID of the core submesh that should be returned.
  *
  * @return One of the following values:
  *         \li a pointer to the core submesh
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreSubmesh *CalCoreMesh::getCoreSubmesh(int id)
{
  if((id < 0) || (id >= (int)m_vectorCoreSubmesh.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return 0;
  }

  return m_vectorCoreSubmesh[id];
}

std::vector<CalCoreSubmesh::Edge>& CalCoreMesh::getVectorEdge()
{
  return m_vectorEdge;
}

 /*****************************************************************************/
/** Returns the number of core submeshes.
  *
  * This function returns the number of core submeshes in the core mesh
  * instance.
  *
  * @return The number of core submeshes.
  *****************************************************************************/

int CalCoreMesh::getCoreSubmeshCount()
{
  return m_vectorCoreSubmesh.size();
}

 /*****************************************************************************/
/** Returns the core submesh vector.
  *
  * This function returns the vector that contains all core submeshes of the
  * core mesh instance.
  *
  * @return A reference to the core submesh vector.
  *****************************************************************************/

std::vector<CalCoreSubmesh *>& CalCoreMesh::getVectorCoreSubmesh()
{
  return m_vectorCoreSubmesh;
}

//****************************************************************************//

bool CalCoreMesh::NoSkel( bool apSetNoSkel)
{
	if( apSetNoSkel) m_bNoSkel = true;
	return m_bNoSkel;
}

CalCoreSubmesh::Vertex& CalCoreMesh::get_vertex(int submeshId, int vId)
{
	return m_vectorCoreSubmesh[submeshId]->get_vertex(vId);
}

void CalCoreMesh::createShadowData()
{
  /*
  std::vector<CalCoreSubmesh *>::iterator iteratorCoreSubmesh;
  for(iteratorCoreSubmesh = m_vectorCoreSubmesh.begin(); iteratorCoreSubmesh != m_vectorCoreSubmesh.end(); ++iteratorCoreSubmesh)
	(*iteratorCoreSubmesh)->createShadowData();
  */

	if( m_pShadowData)
		MP_DELETE( m_pShadowData);
	m_pShadowData = MP_NEW( CalShadowData);
	m_pShadowData->createOn(this);
}

void CalCoreMesh::generate_edges()
{
	std::vector<CalCoreSubmesh *>::iterator iteratorCoreSubmesh;
	for(iteratorCoreSubmesh = m_vectorCoreSubmesh.begin(); iteratorCoreSubmesh != m_vectorCoreSubmesh.end(); ++iteratorCoreSubmesh)
		(*iteratorCoreSubmesh)->generate_edges();

	int	edgesCount = 0;
	int	faceCount = 0;
	int	eCount = 0;
	for(iteratorCoreSubmesh = m_vectorCoreSubmesh.begin(); iteratorCoreSubmesh != m_vectorCoreSubmesh.end(); ++iteratorCoreSubmesh)
	{
		faceCount += (*iteratorCoreSubmesh)->getFaceCount();
		edgesCount += (*iteratorCoreSubmesh)->getVectorEdge().size();
	}

	std::vector<bool>	edgesProcessed;
	edgesProcessed.resize(edgesCount);
	for(int e=0; e<edgesCount; e++)
		edgesProcessed[e] = false;

	unsigned short	vcount=0, fcount=0;
	std::vector<CalCoreSubmesh::Face> m_vectorFace;
	m_vectorFace.reserve(faceCount);
	m_vectorEdge.reserve(edgesCount);
	for(iteratorCoreSubmesh = m_vectorCoreSubmesh.begin(); iteratorCoreSubmesh != m_vectorCoreSubmesh.end(); ++iteratorCoreSubmesh)
	{
		std::vector<CalCoreSubmesh::Edge>& m_subvectorEdge = (*iteratorCoreSubmesh)->getVectorEdge();
		std::vector<CalCoreSubmesh::Edge>::iterator	eit;
		int	eid=0;
		for(eit = m_subvectorEdge.begin(); eit != m_subvectorEdge.end(); ++eit, eid++)
		{
			if( eit->if2 == 0xFFFF)
			{
				if(edgesProcessed[eid+eCount])
					continue;

				unsigned short	vcount2 = vcount + (unsigned short)(*iteratorCoreSubmesh)->getVertexCount(),
					fcount2 = fcount + (unsigned short)(*iteratorCoreSubmesh)->getFaceCount(),
					eCount2 = (unsigned short)eCount + (unsigned short)(*iteratorCoreSubmesh)->getVectorEdge().size();
				bool freeNeibEdge=false;
				// попробовать найти фейс со свободной гранью из другого меша
				std::vector<CalCoreSubmesh *>::iterator iteratorCoreSubmesh2 = iteratorCoreSubmesh;
				iteratorCoreSubmesh2++;
				for(; !freeNeibEdge && iteratorCoreSubmesh2 != m_vectorCoreSubmesh.end(); ++iteratorCoreSubmesh2)
				{
					std::vector<CalCoreSubmesh::Edge>& m_subvectorEdge2 = (*iteratorCoreSubmesh2)->getVectorEdge();
					std::vector<CalCoreSubmesh::Edge>::iterator	eit2;
					int	eid2 = 0;
					for(eit2 = m_subvectorEdge2.begin(); !freeNeibEdge && eit2 != m_subvectorEdge2.end(); ++eit2, eid2++)
					{
						if(eit2->if1 != 0xFFFF || eit2->if2 != 0xFFFF)	continue;


						int	faceID = eit2->if1;
						if( eit2->if1 == 0xFFFF)
							faceID = eit2->if2;

						WORD	idx1, idx2, _idx1, _idx2;
						std::vector<CalCoreSubmesh::Face>&		faces1 = (*iteratorCoreSubmesh)->getVectorFace();
						std::vector<CalCoreSubmesh::Vertex>&	verts1 = (*iteratorCoreSubmesh)->getVectorVertex();
						std::vector<CalCoreSubmesh::Face>&		faces2 = (*iteratorCoreSubmesh2)->getVectorFace();
						std::vector<CalCoreSubmesh::Vertex>&	verts2 = (*iteratorCoreSubmesh2)->getVectorVertex();

//CalVector	_v1 = verts2[faces2[eit2->if1].vertexId[0]].position;
//CalVector	_v2 = verts2[faces2[eit2->if1].vertexId[1]].position;
//CalVector	_v3 = verts2[faces2[eit2->if1].vertexId[2]].position;

						if( faces1[eit->if1].hasSameEdge(
							verts1,
							&faces2[faceID],
							verts2,
							idx1, idx2,
							&_idx1, &_idx2
							))
						{
							if((eit->iv1==idx1 && eit->iv2==idx2)||
								(eit->iv1==idx2 && eit->iv2==idx1))
							{
								if((eit2->iv1==_idx1 && eit2->iv2==_idx2)||
								(eit2->iv1==_idx2 && eit2->iv2==_idx1))
								{
									CalCoreSubmesh::Edge	edge = *eit;
									edge.if1 += fcount;
									edge.if2 = eit2->if1 + fcount2;
									edge.iv1 = idx1 + vcount;
									edge.iv2 = idx2 + vcount;
									m_vectorEdge.push_back(edge);
									edgesProcessed[eid2+eCount2] = true;
									edgesProcessed[eid+eCount] = true;
									freeNeibEdge = true;
								}
							}
						}
					}
					vcount2 += (unsigned short)(*iteratorCoreSubmesh2)->getVertexCount();
					fcount2 += (unsigned short)(*iteratorCoreSubmesh2)->getFaceCount();
					eCount2 += (unsigned short)(*iteratorCoreSubmesh2)->getVectorEdge().size();
				}
				if( !freeNeibEdge)
				{
					CalCoreSubmesh::Edge	edge = *eit;
					edge.if1 += fcount;
					edge.iv1 += vcount;
					edge.iv2 += vcount;
					edgesProcessed[eid+eCount] = true;
					m_vectorEdge.push_back(edge);
				}
				continue;
			}
			CalCoreSubmesh::Edge	edge = *eit;
			edge.if1 += fcount;
			edge.if2 += fcount;
			edge.iv1 += vcount;
			edge.iv2 += vcount;
			m_vectorEdge.push_back(edge);
			edgesProcessed[eid+eCount] = true;
		}
		vcount += (unsigned short)(*iteratorCoreSubmesh)->getVertexCount();
		fcount += (unsigned short)(*iteratorCoreSubmesh)->getFaceCount();
		eCount += (*iteratorCoreSubmesh)->getVectorEdge().size();
	}
}


CalCoreMesh*	CalCoreMesh::clone()
{
	CalCoreMesh*	coreMesh = MP_NEW( CalCoreMesh);
	if( m_pShadowData)
		coreMesh->m_pShadowData = m_pShadowData->clone();

	coreMesh->m_worldTranslation = m_worldTranslation;
	coreMesh->m_worldRotation = m_worldRotation;

	coreMesh->m_vectorCoreSubmesh.reserve( m_vectorCoreSubmesh.size());
	for (unsigned int i=0; i<m_vectorCoreSubmesh.size(); i++)
		coreMesh->m_vectorCoreSubmesh.push_back( m_vectorCoreSubmesh[i]->clone());
	coreMesh->m_vectorEdge.assign( m_vectorEdge.begin(), m_vectorEdge.end());
	return coreMesh;
}

void CalCoreMesh::replaceInfluences( CalCoreSubmesh::Vertex& source, CalCoreSubmesh::Vertex& dest)
{
	dest.vectorInfluence.clear();
	dest.vectorInfluence.resize( source.vectorInfluence.size());
	dest.vectorInfluence.assign( source.vectorInfluence.begin(), source.vectorInfluence.end());
}