//----------------------------------------------------------------------------//
// MaxMesh.h                                                                  //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

#ifndef MAX_MESH_H
#define MAX_MESH_H

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "BaseMesh.h"

//----------------------------------------------------------------------------//
// Forward declarations                                                       //
//----------------------------------------------------------------------------//

class CWeightVertex;

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//

class CMaxMesh : public CBaseMesh
{
// misc
public:
	typedef enum
	{
		MODIFIER_NONE,
		MODIFIER_SKIN,
		MODIFIER_PHYSIQUE,
		MODIFIER_MESHALONE
	} ModifierType;

	typedef struct _SubmeshMaterial
	{
		StdMat *pStdMat;
		int		realID;

		_SubmeshMaterial()
		{
			pStdMat = NULL;	realID = -1;
		}

		_SubmeshMaterial(const _SubmeshMaterial& item)
		{
			pStdMat = item.pStdMat;	realID = item.realID;
		}
	} SubmeshMaterial;

// member variables
protected:
	INode *m_pINode;
	Mesh *m_pIMesh;
	bool m_bDelete;
	//std::vector<StdMat *> m_vectorStdMat;
	std::vector<SubmeshMaterial> m_vectorStdMat;
	Matrix3 m_tm;
	Modifier *m_pModifier;
	ModifierType m_modifierType;

// constructors/destructor
public:
	CMaxMesh();
	virtual ~CMaxMesh();

// member functions
public:
	StdMat* GetMtrl(int id);
	bool Create(INode *pINode, Mesh *pIMesh, bool bDelete);
	int GetFaceCount();
	Mesh *GetIMesh();
	int GetMaterialCount();
	int GetFaceMaterialId(int faceId);
	int GetSubmeshMapCount(int submeshId);
	int GetSubmeshMaterialThreadId(int submeshId);	
	CVertexCandidate *GetVertexCandidate(CSkeletonCandidate *pSkeletonCandidate, int faceId, int faceVertexId);
	virtual int GetCal3dVertexIndex(int faceId, int faceVertexId);
	Matrix3 *GetMatrix3();
	INode *GetNode();

protected:
	bool AddBoneInfluence(CSkeletonCandidate *pSkeletonCandidate, CVertexCandidate *pVertexCandidate, INode *pNode, float weight);
	bool _AddBoneInfluence(CSkeletonCandidate *pSkeletonCandidate, CVertexCandidate *pVertexCandidate, TCHAR *nodeName, float weight);
	bool CreateMaterial(Mtl *pMtl, LPCTSTR alpName=NULL, int realID=0);
	Modifier *FindPhysiqueModifier(INode *pINode);
	Modifier *FindSkinModifier(INode *pINode);
	Point3 GetVertexNormal(int faceId, int vertexId);
	Matrix3 Transpose(Matrix3& matrix);
};

#endif

//----------------------------------------------------------------------------//
