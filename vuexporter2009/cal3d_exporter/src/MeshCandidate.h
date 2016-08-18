//----------------------------------------------------------------------------//
// MeshCandidate.h                                                            //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

#ifndef MESH_CANDIDATE_H
#define MESH_CANDIDATE_H

//----------------------------------------------------------------------------//
// Forward declarations                                                       //
//----------------------------------------------------------------------------//

class CSubmeshCandidate;
class CBaseMesh;
class CBaseNode;
class CSkeletonCandidate;
//class CFaceDesc;
class CMpeg4FDP;

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//
#include "skeletonfilepage.h"

class CBaseMaterial;

class CMeshCandidate
{
// member variables
protected:
	CBaseNode *m_pNode;
	CBaseMesh *m_pMesh;
	std::vector<CSubmeshCandidate *> m_vectorSubmeshCandidate;

// constructors/destructor
public:
	CMeshCandidate();
	virtual ~CMeshCandidate();

// member functions
public:
#ifdef MAX3DS_BUILD
	StdMat* GetMtrl(int id);
#endif
	void SetFlip( bool abFlip);
	virtual bool IsFlipped();
	bool CalculateLOD();
	bool CalculateSpringSystem();
	void Clear();
	bool Create(CSkeletonCandidate *pSkeletonCandidate, int maxBoneCount, float weightThreshold, int objId=-1);
	bool Create(CSkeletonCandidate *pSkeletonCandidate, int maxBoneCount, float weightThreshold, CBaseNode* m_pNode);
	bool DisableLOD();
	std::vector<CSubmeshCandidate *>& GetVectorSubmeshCandidate();
	CString			sName;
	CBaseMesh*	GetBaseMesh(){ return m_pMesh;}

private:
	bool bFlipped;
};

#endif

//----------------------------------------------------------------------------//
