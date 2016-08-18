// WeightVertexManager.h: interface for the CWeightVertexManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEIGHTVERTEXMANAGER_H__76175BA9_B108_4380_A0A5_67A2197EA7BE__INCLUDED_)
#define AFX_WEIGHTVERTEXMANAGER_H__76175BA9_B108_4380_A0A5_67A2197EA7BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWeightVertex;
class CSkeletonCandidate;
class CVertexCandidate;
class Modifier;
class CMaxMesh;

class CWeightVertexManager  
{
public:
	CWeightVertexManager();
	virtual ~CWeightVertexManager();

public:
	int getCount();
	BOOL isEmpty();
	CWeightVertex *getWeightVertex( int id);
	void setSkeletonCandidate( CSkeletonCandidate *pSkeletonCandidate);
	void setVertexCandidate( CVertexCandidate *pVertexCandidate);
	void setModifier( Modifier *pModifier);
	void setMaxMesh( CMaxMesh *pMasMesh);
	void clear();

public:
	int createVertex_PHYSIQUE( int vertexId, int vertexType);
	int createVertex_SKIN( int vertexId);

public:
	int FindSymmetricVertex( CVertexCandidate *pVertexCandidate, float valueExact);
	int ExecuteAssigmentSide2Side_PHYSIQUE( int currVertexId, float valueExact, IPhyVertexExport *new_pVertexExport);
	int ExecuteAssigmentSide2Side_SKIN( int currVertexId, float valueExact, ISkin *new_pSkin, ISkinContextData *new_pSkinContextData);

protected:
	std::vector<CWeightVertex*> weightVertexList;
	CSkeletonCandidate *pSkeletonCandidate;
	CVertexCandidate *pVertexCandidate;
	Modifier *pModifier;
	CMaxMesh *pMasMesh;
};

#endif // !defined(AFX_WEIGHTVERTEXMANAGER_H__76175BA9_B108_4380_A0A5_67A2197EA7BE__INCLUDED_)
