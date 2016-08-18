//****************************************************************************//
// coremesh.h                                                                 //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_COREMESH_H
#define CAL_COREMESH_H\

#include "CommonCal3DHeader.h"

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "global.h"
#include "coresubmesh.h"
#include "quaternion.h"
#include "vector.h"
#include "calshadowdata.h"

//****************************************************************************//
// Forward declarations                                                       //
//****************************************************************************//

class CalShadowData;
/*class CalCoreSubmesh::Vertex;*/

//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//

 /*****************************************************************************/
/** The core mesh class.
  *****************************************************************************/

class CAL3D_API CalCoreMesh
{
// member variables
protected:
  CalShadowData*	m_pShadowData;
  CalVector		m_worldTranslation;
  CalQuaternion	m_worldRotation;
  MP_VECTOR<CalCoreSubmesh *> m_vectorCoreSubmesh;
  MP_VECTOR<CalCoreSubmesh::Edge> m_vectorEdge;

// constructors/destructor
public:
  CalCoreMesh();
	virtual ~CalCoreMesh();

// member functions	
public:
	CalShadowData*	getShadowData(){ return m_pShadowData;}
	void	setShadowData(CalShadowData* sd){ m_pShadowData=sd;}
	void generate_edges();
	void createShadowData();
  CalVector		get_worldTranslation(){	return	m_worldTranslation;}
  CalQuaternion	get_worldRotation(){	return	m_worldRotation;}
  void			set_worldTranslation(CalVector trn){	m_worldTranslation = trn;}
  void			set_worldRotation(CalQuaternion rtn){ m_worldRotation = rtn;}
  CalCoreSubmesh::Vertex& get_vertex(int submeshId, int vId);
  bool NoSkel( bool apSetNoSkel = false);
  int addCoreSubmesh(CalCoreSubmesh *pCoreSubmesh);
  std::vector<CalCoreSubmesh::Edge>& getVectorEdge();
  bool create();
  void destroy();
  CalCoreSubmesh *getCoreSubmesh(int id);
  int getCoreSubmeshCount();
  std::vector<CalCoreSubmesh *>& getVectorCoreSubmesh();
  CalCoreMesh*	clone();
  void replaceInfluences( CalCoreSubmesh::Vertex& source, CalCoreSubmesh::Vertex& dest);
private:
	bool m_bNoSkel;
};

#endif

//****************************************************************************//
