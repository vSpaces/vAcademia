//****************************************************************************//
// coresubmesh.cpp                                                            //
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

#include "coresubmesh.h"

 /*****************************************************************************/
/** Constructs the core submesh instance.
  *
  * This function is the default constructor of the core submesh instance.
  *****************************************************************************/

CalCoreSubmesh::CalCoreSubmesh()
{
  m_coreMaterialThreadId = 0;
  m_lodCount = 0;
  m_bNoSkel = false;
}

 /*****************************************************************************/
/** Destructs the core submesh instance.
  *
  * This function is the destructor of the core submesh instance.
  *****************************************************************************/

CalCoreSubmesh::~CalCoreSubmesh()
{
  assert(m_vectorFace.empty());
  assert(m_vectorVertex.empty());
  assert(m_vectorPhysicalProperty.empty());
  assert(m_vvTextCoord.empty());
  assert(m_vectorSpring.empty());
}

 /*****************************************************************************/
/** Creates the core submesh instance.
  *
  * This function creates the core submesh instance.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreSubmesh::create()
{
  return true;
}

 /*****************************************************************************/
/** Destroys the core submesh instance.
  *
  * This function destroys all data stored in the core submesh instance and
  * frees all allocated memory.
  *****************************************************************************/

void CalCoreSubmesh::destroy()
{
  // destroy all data
  m_vectorFace.clear();
  m_vectorVertex.clear();
  m_vectorPhysicalProperty.clear();
  m_vvTextCoord.clear();
  m_vectorSpring.clear();
}

 /*****************************************************************************/
/** Returns the ID of the core material thread.
  *
  * This function returns the ID of the core material thread of this core
  * submesh instance.
  *
  * @return The ID of the core material thread.
  *****************************************************************************/

int CalCoreSubmesh::getCoreMaterialThreadId()
{
  return m_coreMaterialThreadId;
}

 /*****************************************************************************/
/** Returns the number of faces.
  *
  * This function returns the number of faces in the core submesh instance.
  *
  * @return The number of faces.
  *****************************************************************************/

int CalCoreSubmesh::getFaceCount()
{
  return m_vectorFace.size();
}

 /*****************************************************************************/
/** Returns the number of LOD steps.
  *
  * This function returns the number of LOD steps in the core submesh instance.
  *
  * @return The number of LOD steps.
  *****************************************************************************/

int CalCoreSubmesh::getLodCount()
{
  return m_lodCount;
}

 /*****************************************************************************/
/** Returns the number of springs.
  *
  * This function returns the number of springs in the core submesh instance.
  *
  * @return The number of springs.
  *****************************************************************************/

int CalCoreSubmesh::getSpringCount()
{
  return m_vectorSpring.size();
}

 /*****************************************************************************/
/** Returns the face vector.
  *
  * This function returns the vector that contains all faces of the core submesh
  * instance.
  *
  * @return A reference to the face vector.
  *****************************************************************************/

std::vector<CalCoreSubmesh::Face>& CalCoreSubmesh::getVectorFace()
{
  return m_vectorFace;
}

 /*****************************************************************************/
/** Returns the edge vector.
  *
  * This function returns the vector that contains all edges of the core submesh
  * instance.
  *
  * @return A reference to the edge vector.
  *****************************************************************************/

std::vector<CalCoreSubmesh::Edge>& CalCoreSubmesh::getVectorEdge()
{
  return m_vectorEdge;
}

 /*****************************************************************************/
/** Returns the neighbour vector.
  *
  * This function returns the vector that contains all vertices neighbours edges of the core submesh
  * instance.
  *
  * @return A reference to the neighbour vector.
  *****************************************************************************/

std::vector<CalCoreSubmesh::Neighbour>& CalCoreSubmesh::getVectorNeighbour()
{
  return m_vectorNeighbour;
}

 /*****************************************************************************/
/** Returns the physical property vector.
  *
  * This function returns the vector that contains all physical properties of
  * the core submesh instance.
  *
  * @return A reference to the physical property vector.
  *****************************************************************************/

std::vector<CalCoreSubmesh::PhysicalProperty>& CalCoreSubmesh::getVectorPhysicalProperty()
{
  return m_vectorPhysicalProperty;
}

 /*****************************************************************************/
/** Returns the spring vector.
  *
  * This function returns the vector that contains all springs of the core
  * submesh instance.
  *
  * @return A reference to the spring vector.
  *****************************************************************************/

std::vector<CalCoreSubmesh::Spring>& CalCoreSubmesh::getVectorSpring()
{
  return m_vectorSpring;
}

 /*****************************************************************************/
/** Returns the texture coordinate vector-vector.
  *
  * This function returns the vector that contains all texture coordinate
  * vectors of the core submesh instance. This vector contains another vector
  * because there can be more than one texture map at each vertex.
  *
  * @return A reference to the texture coordinate vector-vector.
  *****************************************************************************/

std::vector<std::vector<CalCoreSubmesh::TxCoor> > & CalCoreSubmesh::getVectorVectorTextureCoordinate()
{
  return m_vvTextCoord;
}

 /*****************************************************************************/
/** Returns the vertex vector.
  *
  * This function returns the vector that contains all vertices of the core
  * submesh instance.
  *
  * @return A reference to the vertex vector.
  *****************************************************************************/

std::vector<CalCoreSubmesh::Vertex>& CalCoreSubmesh::getVectorVertex()
{
  return m_vectorVertex;
}

 /*****************************************************************************/
/** Returns the number of vertices.
  *
  * This function returns the number of vertices in the core submesh instance.
  *
  * @return The number of vertices.
  *****************************************************************************/

int CalCoreSubmesh::getVertexCount()
{
  return m_vectorVertex.size();
}

 /*****************************************************************************/
/** Reserves memory for the vertices, faces and texture coordinates.
  *
  * This function reserves memory for the vertices, faces, texture coordinates
  * and springs of the core submesh instance.
  *
  * @param vertexCount The number of vertices that this core submesh instance
  *                    should be able to hold.
  * @param textureCoordinateCount The number of texture coordinates that this
  *                               core submesh instance should be able to hold.
  * @param faceCount The number of faces that this core submesh instance should
  *                  be able to hold.
  * @param springCount The number of springs that this core submesh instance
  *                  should be able to hold.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreSubmesh::reserve(int vertexCount, int textureCoordinateCount, int faceCount, int springCount)
{
  // reserve the space needed in all the vectors
  m_vectorVertex.reserve(vertexCount);
  m_vectorVertex.resize(vertexCount);

  m_vvTextCoord.reserve(textureCoordinateCount);
  m_vvTextCoord.resize(textureCoordinateCount);

  int textureCoordinateId;
  for(textureCoordinateId = 0; textureCoordinateId < textureCoordinateCount; textureCoordinateId++)
  {
    m_vvTextCoord[textureCoordinateId].reserve(vertexCount);
    m_vvTextCoord[textureCoordinateId].resize(vertexCount);
  }

  m_vectorFace.reserve(faceCount);
  m_vectorFace.resize(faceCount);

  m_vectorSpring.reserve(springCount);
  m_vectorSpring.resize(springCount);

  // reserve the space for the physical properties if we have springs in the core submesh instance
  if(springCount > 0)
  {
    m_vectorPhysicalProperty.reserve(vertexCount);
    m_vectorPhysicalProperty.resize(vertexCount);
  }

  return true;
}

 /*****************************************************************************/
/** Sets the ID of the core material thread.
  *
  * This function sets the ID of the core material thread of the core submesh
  * instance.
  *
  * @param coreMaterialThreadId The ID of the core material thread that should
  *                             be set.
  *****************************************************************************/

void CalCoreSubmesh::setCoreMaterialThreadId(int coreMaterialThreadId)
{
  m_coreMaterialThreadId = coreMaterialThreadId;
}

 /*****************************************************************************/
/** Sets a specified face.
  *
  * This function sets a specified face in the core submesh instance.
  *
  * @param faceId  The ID of the face.
  * @param face The face that should be set.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreSubmesh::setFace(int faceId, const Face& face)
{
  if((faceId < 0) || (faceId >= (int)m_vectorFace.size())) return false;

  m_vectorFace[faceId] = face;

  return true;
}

 /*****************************************************************************/
/** Sets the number of LOD steps.
  *
  * This function sets the number of LOD steps of the core submesh instance.
  *
  * @param lodCount The number of LOD steps that should be set.
  *****************************************************************************/

void CalCoreSubmesh::setLodCount(int lodCount)
{
  m_lodCount = lodCount;
}

 /*****************************************************************************/
/** Sets a specified physical property.
  *
  * This function sets a specified physical property in the core submesh
  * instance.
  *
  * @param vertexId  The ID of the vertex.
  * @param physicalProperty The physical property that should be set.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreSubmesh::setPhysicalProperty(int vertexId, const PhysicalProperty& physicalProperty)
{
  if((vertexId < 0) || (vertexId >= (int)m_vectorPhysicalProperty.size())) return false;

  m_vectorPhysicalProperty[vertexId] = physicalProperty;

  return true;
}

 /*****************************************************************************/
/** Sets a specified spring.
  *
  * This function sets a specified spring in the core submesh instance.
  *
  * @param springId  The ID of the spring.
  * @param spring The spring that should be set.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreSubmesh::setSpring(int springId, const Spring& spring)
{
  if((springId < 0) || (springId >= (int)m_vectorSpring.size())) return false;

  m_vectorSpring[springId] = spring;

  return true;
}

 /*****************************************************************************/
/** Sets a specified texture coordinate.
  *
  * This function sets a specified texture coordinate in the core submesh
  * instance.
  *
  * @param vertexId  The ID of the vertex.
  * @param textureCoordinateId  The ID of the texture coordinate.
  * @param textureCoordinate The texture coordinate that should be set.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreSubmesh::setTextureCoordinate(int vertexId, int textureCoordinateId, const TxCoor& textureCoordinate)
{
  if((textureCoordinateId < 0) || (textureCoordinateId >= (int)m_vvTextCoord.size())) return false;
  if((vertexId < 0) || (vertexId >= (int)m_vvTextCoord[textureCoordinateId].size())) return false;

  m_vvTextCoord[textureCoordinateId][vertexId] = textureCoordinate;

  return true;
}

 /*****************************************************************************/
/** Sets a specified vertex.
  *
  * This function sets a specified vertex in the core submesh instance.
  *
  * @param vertexId  The ID of the vertex.
  * @param vertex The vertex that should be set.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreSubmesh::setVertex(int vertexId, const Vertex& vertex)
{
  if((vertexId < 0) || (vertexId >= (int)m_vectorVertex.size())) return false;

  m_vectorVertex[vertexId] = vertex;

  return true;
}

//****************************************************************************//

bool CalCoreSubmesh::NoSkel(bool apSetNoSkel)
{
	if( apSetNoSkel) m_bNoSkel = true;
	return m_bNoSkel;
}

CalCoreSubmesh::Vertex& CalCoreSubmesh::get_vertex(int vId)
{
	return m_vectorVertex[vId];
}

 /*****************************************************************************/
/** Generate neighbour data for every all vertices.
  *
  * This function find and store all vertex neighbours in an array.
  *	 Не учитывает того, что одна грань может разделяться на 2 из-за дублирования вершин
  *****************************************************************************/
bool CalCoreSubmesh::is_face_identity(const CalCoreSubmesh::Face& face)
{
	Vertex* pvertex1 = &m_vectorVertex[face.vertexId[0]];
	Vertex* pvertex2 = &m_vectorVertex[face.vertexId[1]];
	Vertex* pvertex3 = &m_vectorVertex[face.vertexId[2]];
	if( pvertex1->position==pvertex2->position)	return true;
	if( pvertex1->position==pvertex3->position)	return true;
	if( pvertex3->position==pvertex2->position)	return true;
	return false;
}

void CalCoreSubmesh::generate_edges()
{
#define OLD_EDGING_METHOD
	
#ifdef OLD_EDGING_METHOD
	for(int iiii=0; iiii<m_vectorVertex.size(); iiii++)
	{
		CalVector pos = m_vectorVertex[iiii].position;
	}
	int iter=0;
	std::vector<Neighbour>::iterator m_iteratorNeighbour;
	int i;
	for(i=0; i<m_vectorVertex.size(); i++)
		m_vectorNeighbour.push_back(Neighbour());

	std::vector<Face>::iterator m_iteratorFace;
	std::vector<Face>::iterator m_iteratorFace2;
	for( m_iteratorFace=m_vectorFace.begin(); m_iteratorFace!=m_vectorFace.end(); m_iteratorFace++)
	{
		int index1 = m_iteratorFace->vertexId[0];
		int index2 = m_iteratorFace->vertexId[1];
		int index3 = m_iteratorFace->vertexId[2];

		if((index1==165 && index2==165) || (index2==165 && index3==165) || (index1==165 && index3==165))
			int ii=0;	

		Vertex* pvertex1 = &m_vectorVertex[m_iteratorFace->vertexId[0]];
		Vertex* pvertex2 = &m_vectorVertex[m_iteratorFace->vertexId[1]];
		Vertex* pvertex3 = &m_vectorVertex[m_iteratorFace->vertexId[2]];

		float dist12 = CalVector(pvertex1->position-pvertex2->position).length();
		float dist23 = CalVector(pvertex3->position-pvertex2->position).length();
		float dist31 = CalVector(pvertex1->position-pvertex3->position).length();

		m_iteratorNeighbour = m_vectorNeighbour.begin() + index1;
		//m_iteratorNeighbour = &m_vectorNeighbour[index1];
		if( !m_iteratorNeighbour->contain(index2))
			m_iteratorNeighbour->add(NDef(index2,dist12));

		if( !m_iteratorNeighbour->contain(index3))
			m_iteratorNeighbour->add(NDef(index3,dist31));

		//m_iteratorNeighbour = &m_vectorNeighbour[index2];
		m_iteratorNeighbour = m_vectorNeighbour.begin() + index2;
		if( !m_iteratorNeighbour->contain(index1))
			m_iteratorNeighbour->add(NDef(index1,dist12));

		if( !m_iteratorNeighbour->contain(index3))
			m_iteratorNeighbour->add(NDef(index3,dist23));

		//m_iteratorNeighbour = &m_vectorNeighbour[index3];
		m_iteratorNeighbour = m_vectorNeighbour.begin() + index3;
		if( !m_iteratorNeighbour->contain(index1))
			m_iteratorNeighbour->add(NDef(index1,dist31));

		if( !m_iteratorNeighbour->contain(index2))
			m_iteratorNeighbour->add(NDef(index2,dist23));
	}

	std::vector<int>	m_facesEdges;
	m_facesEdges.resize(m_vectorFace.size()*3);
	for( i=0; i<m_vectorFace.size()*3; i++)	m_facesEdges[i] = 0;
	
	// Учтем то, что одна грань может разделяться на 2 из-за дублирования вершин
	int	if1=0, if2=0;
	int ind1 = 0, ind2 = 0;

	
	for( m_iteratorFace=m_vectorFace.begin(); m_iteratorFace!=m_vectorFace.end(); m_iteratorFace++, if1++)
	{
CalVector v1 = m_vectorVertex[m_iteratorFace->vertexId[0]].position;
CalVector v2 = m_vectorVertex[m_iteratorFace->vertexId[1]].position;
CalVector v3 = m_vectorVertex[m_iteratorFace->vertexId[2]].position;
		if( is_face_identity(*m_iteratorFace))	continue;

		ind2 = 0;
		if2 = if1+1;
		for( m_iteratorFace2=m_iteratorFace+1; m_iteratorFace2!=m_vectorFace.end(); m_iteratorFace2++, if2++)
		{
CalVector _v1 = m_vectorVertex[m_iteratorFace2->vertexId[0]].position;
CalVector _v2 = m_vectorVertex[m_iteratorFace2->vertexId[1]].position;
CalVector _v3 = m_vectorVertex[m_iteratorFace2->vertexId[2]].position;
		if( is_face_identity(*m_iteratorFace2))	continue;

			WORD	ind11, ind12, ind21, ind22;
			if( if1 == 2249 && if2 == 2255)
				int i=0;
			if( m_iteratorFace->hasSameEdge(m_vectorVertex, &*m_iteratorFace2, m_vectorVertex, ind11, ind12, &ind21, &ind22))
			{
				int pv1 = (int)pow((double)2, m_iteratorFace->getVertIDnum(ind11));
				int pv2 = (int)pow((double)2, m_iteratorFace2->getVertIDnum(ind21));

				Vertex* pvertex1 = &m_vectorVertex[ind11];
				Vertex* pvertex2 = &m_vectorVertex[ind21];
				if( !(m_facesEdges[if1] & pv1) || !(m_facesEdges[if2] & pv2))
				{
					if( !(m_facesEdges[if1] & pv1) && !(m_facesEdges[if2] & pv2))
						m_vectorEdge.push_back(Edge(ind11, ind12, if1, if2, (pvertex1->position-pvertex2->position).length()));
					else	if( !(m_facesEdges[if2] & pv2))
						//m_vectorEdge.push_back(Edge(ind21, ind22, -1, if2, (pvertex1->position-pvertex2->position).length()));
						m_vectorEdge.push_back(Edge(ind21, ind22, if2, -1, (pvertex1->position-pvertex2->position).length()));
					else
						m_vectorEdge.push_back(Edge(ind11, ind12, if1, -1, (pvertex1->position-pvertex2->position).length()));
				}

				// Учтановка флагов, что грань уже обработана
				m_facesEdges[if1]	|= pv1;
				m_facesEdges[if2]	|= pv2;
				//
				{
					int	n[4][2]={{ind11,ind21},{ind11,ind22},{ind12,ind21},{ind12,ind22}};
					for( int in=0; in<4; in++)
					{
						if( n[in][0]==n[in][1] || n[in][1]==-0 || n[in][1]==-1)	continue;
						Vertex* pvertex1 = &m_vectorVertex[n[in][0]];
						Vertex* pvertex2 = &m_vectorVertex[n[in][1]];
						if( (pvertex1->position-pvertex2->position).length() < 0.0001)
						{
							if( !m_vectorNeighbour[n[in][0]].contain(n[in][1]))
								m_vectorNeighbour[n[in][0]].add(NDef(n[in][1],0.0));
							if( !m_vectorNeighbour[n[in][1]].contain(n[in][0]))
								m_vectorNeighbour[n[in][1]].add(NDef(n[in][0],0.0));
						}
					}
				}
			}
			else
			{
				for( int i=0; i<3; i++)
					for( int j=0; j<3; j++)
					{
						int index1 = m_iteratorFace->vertexId[i];
						int index2 = m_iteratorFace2->vertexId[j];
						if( index1 >= m_vectorVertex.size() || index2 >= m_vectorVertex.size())
							continue;
						Vertex* pvertex1 = &m_vectorVertex[index1];
						Vertex* pvertex2 = &m_vectorVertex[index2];
						if( (pvertex1->position-pvertex2->position).length() < 0.0001)
						{
							if( !m_vectorNeighbour[index1].contain(index2))
								m_vectorNeighbour[index1].add(NDef(index2,0.0));
							if( !m_vectorNeighbour[index2].contain(index1))
								m_vectorNeighbour[index2].add(NDef(index1,0.0));
						}
					}
			}
			ind2++;
		}
		for( int ie=0; ie<3; ie++)
		{
			if( !(m_facesEdges[if1] & (int)pow((double)2, ie)))
			{
				m_vectorEdge.push_back(Edge(m_iteratorFace->vertexId[ie], m_iteratorFace->vertexId[(ie+1)%3], if1, -1, 0));
			}
		}
		ind1++;
	}

#else

	//////////////////////////////////////////////////////////////////////////
	std::vector<Neighbour>::iterator m_iteratorNeighbour;
	for(int i=0; i<m_vectorVertex.size(); i++)
		m_vectorNeighbour.push_back(Neighbour());

	std::vector<Face>::iterator m_iteratorFace;
	std::vector<Face>::iterator m_iteratorFace2;
	for( m_iteratorFace=m_vectorFace.begin(); m_iteratorFace!=m_vectorFace.end(); m_iteratorFace++)
	{
		int index1 = m_iteratorFace->vertexId[0];
		int index2 = m_iteratorFace->vertexId[1];
		int index3 = m_iteratorFace->vertexId[2];

		Vertex* pvertex1 = &m_vectorVertex[m_iteratorFace->vertexId[0]];
		Vertex* pvertex2 = &m_vectorVertex[m_iteratorFace->vertexId[1]];
		Vertex* pvertex3 = &m_vectorVertex[m_iteratorFace->vertexId[2]];

		float dist12 = CalVector(pvertex1->position-pvertex2->position).length();
		float dist23 = CalVector(pvertex3->position-pvertex2->position).length();
		float dist31 = CalVector(pvertex1->position-pvertex3->position).length();

		m_iteratorNeighbour = &m_vectorNeighbour[index1];
		if( !m_iteratorNeighbour->contain(index2))
			m_iteratorNeighbour->add(NDef(index2,dist12));
		if( !m_iteratorNeighbour->contain(index3))
			m_iteratorNeighbour->add(NDef(index3,dist31));
		m_iteratorNeighbour = &m_vectorNeighbour[index2];
		if( !m_iteratorNeighbour->contain(index1))
			m_iteratorNeighbour->add(NDef(index1,dist12));
		if( !m_iteratorNeighbour->contain(index3))
			m_iteratorNeighbour->add(NDef(index3,dist23));
		m_iteratorNeighbour = &m_vectorNeighbour[index3];
		if( !m_iteratorNeighbour->contain(index1))
			m_iteratorNeighbour->add(NDef(index1,dist31));
		if( !m_iteratorNeighbour->contain(index2))
			m_iteratorNeighbour->add(NDef(index2,dist23));
	}
	// Учтем то, что одна грань может разделяться на 2 из-за дублирования вершин
	int ind1 = 0, ind2 = 0;
	for( m_iteratorFace=m_vectorFace.begin(); m_iteratorFace!=m_vectorFace.end(); m_iteratorFace++)
	{
		ind2 = 0;
		if( ind1 == 23)
			int j=0;
		if( ind1 == 839)
			int j=0;
		for( m_iteratorFace2=m_iteratorFace+1; m_iteratorFace2!=m_vectorFace.end(); m_iteratorFace2++)
		{
			if( ind1 == 0 && ind2 == 7)
				int j=0;
			int same_vcount = 0;
			int	verts1[2];		// Индексы первой вершины грани (могут быть индекс из разных фейсов)
			int	verts2[2];		// Индексы второй вершины грани (могут быть индекс из разных фейсов)
			int vc1 = 0;
			int vc2 = 0;
			int index1, index2;

			for( int i=0; i<3; i++)
			{
				for( int j=0; j<3; j++)
				{
					if( i==1 && j==1)
						int eghlgh=0;
					index1 = m_iteratorFace->vertexId[i];
					index2 = m_iteratorFace2->vertexId[j];

					if( index1 >= m_vectorVertex.size() || index2 >= m_vectorVertex.size())
						continue;

					Vertex* pvertex1 = &m_vectorVertex[index1];
					Vertex* pvertex2 = &m_vectorVertex[index2];

					if( index1 == index2)
					{
						if( same_vcount == 0)
						{
							verts1[vc1] = index1;	vc1++;
						}
						else
						{
							verts2[vc2] = index1;	vc2++;
						}
						same_vcount++;
						continue;
					}

					if( pvertex1->position == pvertex2->position)
					{
						if( !m_vectorNeighbour[index1].contain(index2))
							m_vectorNeighbour[index1].add(NDef(index2,0.0));
						if( !m_vectorNeighbour[index2].contain(index1))
							m_vectorNeighbour[index2].add(NDef(index1,0.0));

						if( same_vcount == 0)
						{
							verts1[vc1] = index1;	vc1++;
							verts1[vc1] = index2;	vc1++;
						}
						else
						{
							verts2[vc2] = index1;	vc2++;
							verts2[vc2] = index2;	vc2++;
						}
						same_vcount++;
					}
				}
			}
			if( same_vcount == 2)
			{
				int min = vc1>vc2?vc1:vc2;
				int max = vc1<vc2?vc1:vc2;
				for( int i=0; i<min; i++)
				{
					index1 = verts1[i % vc1];
					index2 = verts2[i % vc2];
					Vertex* pvertex1 = &m_vectorVertex[index1];
					Vertex* pvertex2 = &m_vectorVertex[index2];
					m_vectorEdge.push_back(Edge(index1, index2, (pvertex1->position-pvertex2->position).length()));
				}

			}
			ind2++;
		}
		ind1++;
	}
#endif
}

bool CalCoreSubmesh::Face::hasSameEdge(std::vector<Vertex>&	selfVerts, _Face* face, std::vector<Vertex>&	faceVerts, WORD& idx1, WORD& idx2, WORD* idx21, WORD* idx22)
{
	int vcount=0;
	WORD	vIDs[2] = {-1, -1};
	WORD	vIDs2[2] = {-1, -1};
	for( int i=0; i<3; i++)
	{
		for( int j=0; j<3; j++)
		{
			CalVector	vPos = selfVerts[vertexId[i]].position;
			if( selfVerts[vertexId[i]].position == faceVerts[face->vertexId[j]].position)
			{
				vIDs[vcount] = vertexId[i];
				vIDs2[vcount] = face->vertexId[j];
				vcount++;
				if( vcount == 2)	break;
			}
		}
		if( vcount == 2)	break;
	}
	if( vcount != 2)	return false;
	makeIDsCW(vIDs[0], vIDs[1]);
	face->makeIDsCW(vIDs2[0], vIDs2[1]);
	// Проверим на то, что фейсы стыкуются лицевыми гранями в одну сторону
	CalVector	n1 = (selfVerts[vertexId[1]].position-selfVerts[vertexId[0]].position).getNormal(selfVerts[vertexId[2]].position-selfVerts[vertexId[0]].position);
	CalVector	n2 = (faceVerts[face->vertexId[1]].position-faceVerts[face->vertexId[0]].position).getNormal(faceVerts[face->vertexId[2]].position-faceVerts[face->vertexId[0]].position);
	CalVector	e1 = selfVerts[vIDs[1]].position-selfVerts[vIDs[0]].position;
	CalVector	e2 = faceVerts[vIDs2[1]].position-faceVerts[vIDs2[0]].position;
	//
	if( e1.cross(n1).dot(e2.cross(n1)) > 0)
		return false;

	idx1 = vIDs[0];
	idx2 = vIDs[1];
	if( idx21)	*idx21 = vIDs2[0];
	if( idx22)	*idx22 = vIDs2[1];
	return true;
}

CalCoreSubmesh*	CalCoreSubmesh::clone()
{
	CalCoreSubmesh*	submesh = new CalCoreSubmesh();
	submesh->m_vectorVertex.assign( m_vectorVertex.begin(), m_vectorVertex.end());
	submesh->m_vvTextCoord.assign( m_vvTextCoord.begin(), m_vvTextCoord.end());
	submesh->m_vectorPhysicalProperty.assign( m_vectorPhysicalProperty.begin(), m_vectorPhysicalProperty.end());
	submesh->m_vectorFace.assign( m_vectorFace.begin(), m_vectorFace.end());
	submesh->m_vectorSpring.assign( m_vectorSpring.begin(), m_vectorSpring.end());
	submesh->m_vectorEdge.assign( m_vectorEdge.begin(), m_vectorEdge.end());
	submesh->m_vectorNeighbour.assign( m_vectorNeighbour.begin(), m_vectorNeighbour.end());
	submesh->m_coreMaterialThreadId = m_coreMaterialThreadId;
	submesh->m_lodCount = m_lodCount;
	submesh->m_bNoSkel = m_bNoSkel;
	return submesh;
}