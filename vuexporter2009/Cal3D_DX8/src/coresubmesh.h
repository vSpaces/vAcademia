//****************************************************************************//
// coresubmesh.h                                                              //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_CORESUBMESH_H
#define CAL_CORESUBMESH_H

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "windows.h"
#include "global.h"
#pragma warning(once:4786)
#include "vector.h"

//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//

 /*****************************************************************************/
/** The core submesh class.
  *****************************************************************************/

class CAL3D_API CalCoreSubmesh
{
// misc
public:
  /// The core submesh TextureCoordinate.
  typedef struct
  {
    float u, v;
  } TxCoor;

  /// The core submesh Influence.
  typedef struct
  {
    int boneId;
    float weight;
  } Influence;

  /// The core submesh PhysicalProperty.
  typedef struct
  {
    float weight;
  } PhysicalProperty;

  /// The core submesh Vertex.
  typedef struct
  {
    CalVector position;
    CalVector normal;
    std::vector<Influence> vectorInfluence;
    int collapseId;
    int faceCollapseCount;
  } Vertex;

  /// The core submesh Edge.
  typedef struct _Edge
  {
    WORD	iv1, iv2;	// vertices numbers, which define edge
	WORD	if1, if2;	// faces numbers, lies on two sides of the face
	float   length;

	_Edge::_Edge(){}
	_Edge::_Edge(WORD i1, WORD i2, float al){iv1=i1; iv2=i2; length=al;}
	_Edge::_Edge(WORD i1, WORD i2, WORD f1, WORD f2,float al)
		{iv1=i1; iv2=i2; length=al; if1=f1; if2=f2;}
  } Edge;

  /// The core submesh Vertices neighbours.
  typedef struct _NDef
  {
	  int id;
	  float dist;

	  _NDef::_NDef(){}
	  _NDef::_NDef(int aid, float adist){ id=aid; dist=adist;}
  } NDef;

  /// The core submesh Vertices neighbours.
  typedef struct _Neighbour
  {
	  std::vector<NDef>	nbrs;
	  _Neighbour::_Neighbour(){}
	  bool _Neighbour::contain(int idx)
	  {
		  std::vector<NDef>::iterator	inbrs;
		  for( inbrs=nbrs.begin(); inbrs!=nbrs.end(); inbrs++)
			  if( (inbrs->id)==idx) return true;
		  return false;
	  }
	  void _Neighbour::add(NDef idx)
	  {
		  nbrs.push_back(idx);
	  }
  } Neighbour;

  /// The core submesh Face.
  typedef struct _Face
  {
#ifdef FACESIZEISWORD
    WORD vertexId[3];
#else
	int vertexId[3];
#endif
	void	makeIDsCW(WORD& id1, WORD& id2)
	{
		if( (id1==vertexId[1] && id2==vertexId[0]) ||
			(id1==vertexId[0] && id2==vertexId[2]) ||
			(id1==vertexId[2] && id2==vertexId[1])
			)
		{
			WORD itemp=id1;
			id1=id2;
			id2=itemp;
		}
	}

	int	getVertIDnum(WORD vid)
	{
		if( vid == vertexId[0])	return 0;
		if( vid == vertexId[1])	return 1;
		if( vid == vertexId[2])	return 2;
		return -1;
	}

	bool hasSameEdge(std::vector<Vertex>&	selfVerts, _Face* face, std::vector<Vertex>&	faceVerts, WORD& idx1, WORD& idx2, WORD* idx21=NULL, WORD* idx22=NULL);
	
  } Face;

  /// The core submesh Spring.
  typedef struct
  {
    int vertexId[2];
    float springCoefficient;
    float idleLength;
  } Spring;

// member variables
protected:
  std::vector<Vertex> m_vectorVertex;
  std::vector<std::vector<TxCoor> > m_vvTextCoord;
  std::vector<PhysicalProperty> m_vectorPhysicalProperty;
  std::vector<Face> m_vectorFace;
  std::vector<Spring> m_vectorSpring;
  std::vector<Edge> m_vectorEdge;
  std::vector<Neighbour> m_vectorNeighbour;
  int m_coreMaterialThreadId;
  int m_lodCount;

// constructors/destructor
public:
  CalCoreSubmesh();
	virtual ~CalCoreSubmesh();

// member functions	
public:
	//void createShadowData();
  void generate_edges();
  Vertex& get_vertex(int vId);
  bool NoSkel( bool apSetNoSkel = false);
  bool create();
  void destroy();
  int getCoreMaterialThreadId();
  int getFaceCount();
  int getLodCount();
  int getSpringCount();
  std::vector<Face>& getVectorFace();
  std::vector<Edge>& getVectorEdge();
  std::vector<Neighbour>& getVectorNeighbour();
  std::vector<PhysicalProperty>& getVectorPhysicalProperty();
  std::vector<Spring>& getVectorSpring();
  std::vector<std::vector<TxCoor> >& getVectorVectorTextureCoordinate();
  std::vector<Vertex>& getVectorVertex();
  int getVertexCount();
  bool reserve(int vertexCount, int textureCoordinateCount, int faceCount, int springCount);
  void setCoreMaterialThreadId(int coreMaterialThreadId);
  bool setFace(int faceId, const Face& face);
  void setLodCount(int lodCount);
  bool setPhysicalProperty(int vertexId, const PhysicalProperty& physicalProperty);
  bool setSpring(int springId, const Spring& spring);
  bool setTextureCoordinate(int vertexId, int textureCoordinateId, const TxCoor& textureCoordinate);
  bool setVertex(int vertexId, const Vertex& vertex);
  CalCoreSubmesh*	clone();

  static void copyVerticesInfls( Vertex& v1, Vertex& v2)
  {
	  v1.vectorInfluence.clear();
	  v1.vectorInfluence.assign( v2.vectorInfluence.begin(),v2.vectorInfluence.end());
  }

private:
	bool m_bNoSkel;
	bool is_face_identity(const CalCoreSubmesh::Face& face);
};

#endif

//****************************************************************************//
