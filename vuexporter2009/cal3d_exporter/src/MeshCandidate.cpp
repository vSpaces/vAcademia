//----------------------------------------------------------------------------//
// MeshCandidate.cpp                                                          //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//----------------------------------------------------------------------------//
// This program is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU General Public License as published by the Free //
// Software Foundation; either version 2 of the License, or (at your option)  //
// any later version.                                                         //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "StdAfx.h"
#include "Exporter.h"
#include "MeshCandidate.h"
#include "SubmeshCandidate.h"
#include "BaseInterface.h"
#include "BaseMesh.h"
#include "BaseNode.h"
#include "VertexCandidate.h"
#include "FaceDesc.h"
#include "Mpeg4FDP.h"
#ifdef MAX3DS_BUILD
	#include "MaxMesh.h"
#endif

//----------------------------------------------------------------------------//
// Debug                                                                      //
//----------------------------------------------------------------------------//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//

CMeshCandidate::CMeshCandidate()
{
	m_pNode = 0;
	m_pMesh = 0;
	bFlipped = false;
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

CMeshCandidate::~CMeshCandidate()
{
	Clear();
}

//----------------------------------------------------------------------------//
// Calculate the LOD for the whole mesh candidate                             //
//----------------------------------------------------------------------------//

bool CMeshCandidate::CalculateLOD()
{
	// calculate the LOD for all the submesh candidates
	int submeshCandidateId;
	for(submeshCandidateId = 0; submeshCandidateId < m_vectorSubmeshCandidate.size(); submeshCandidateId++)
	{
		// only handle non-empty submesh candidates
		if(m_vectorSubmeshCandidate[submeshCandidateId]->GetFaceCount() > 0)
		{
			if(!m_vectorSubmeshCandidate[submeshCandidateId]->CalculateLOD()) return false;
		}
	}
	
	return true;
}

//----------------------------------------------------------------------------//
// Calculate possible spring systems for the whole mesh candidate             //
//----------------------------------------------------------------------------//

bool CMeshCandidate::CalculateSpringSystem()
{
	// calculate the LOD for all the submesh candidates
	int submeshCandidateId;
	for(submeshCandidateId = 0; submeshCandidateId < m_vectorSubmeshCandidate.size(); submeshCandidateId++)
	{
		// only handle non-empty submesh candidates
		if(m_vectorSubmeshCandidate[submeshCandidateId]->GetFaceCount() > 0)
		{
			if(!m_vectorSubmeshCandidate[submeshCandidateId]->CalculateSpringSystem()) return false;
		}
	}
	
	return true;
}

//----------------------------------------------------------------------------//
// Clear this mesh candidate                                                  //
//----------------------------------------------------------------------------//

void CMeshCandidate::Clear()
{
	// destroy all submesh candidates stored in this mesh candidate
	int submeshCandidateId;
	for(submeshCandidateId = 0; submeshCandidateId < m_vectorSubmeshCandidate.size(); submeshCandidateId++)
	{
		delete m_vectorSubmeshCandidate[submeshCandidateId];
	}
	m_vectorSubmeshCandidate.clear();

	delete m_pNode;
	delete m_pMesh;
}

//----------------------------------------------------------------------------//
// Create a mesh candidate                                                    //
//----------------------------------------------------------------------------//

bool CMeshCandidate::Create(CSkeletonCandidate *pSkeletonCandidate, int maxBoneCount, float weightThreshold, CBaseNode* m_pNode)
{
	// get the mesh
	m_pMesh = theExporter.GetInterface()->GetMesh(m_pNode);
	if(m_pMesh == 0) return false;

	// get the number of materials of the mesh
	int materialCount;
	materialCount = m_pMesh->GetMaterialCount();

	// create all the submesh candidates
	int submeshId;
	for(submeshId = 0; submeshId < materialCount; submeshId++)
	{
		// allocate a new submesh candidate
		CSubmeshCandidate *pSubmeshCandidate;
		pSubmeshCandidate = new CSubmeshCandidate();
		if(pSubmeshCandidate == 0)
		{
			theExporter.SetLastError("Memory allocation failed!", __FILE__, __LINE__);
			return false;
		}

		// create the new submesh candidate
		if(!pSubmeshCandidate->Create(m_pMesh->GetSubmeshMaterialThreadId(submeshId), m_pMesh->GetSubmeshMapCount(submeshId))) return false;

		// add submesh candidate to the submesh candidate vector
		m_vectorSubmeshCandidate.push_back(pSubmeshCandidate);
	}

	bool		flipFaces = bFlipped;
	CalVector	scale;
	theExporter.GetInterface()->GetScale( m_pNode, 0, scale);
	if( scale.x < 0) flipFaces = !flipFaces;
	if( scale.y < 0) flipFaces = !flipFaces;
	if( scale.z < 0) flipFaces = !flipFaces;

	// get the number of faces of the mesh
	int faceCount;
	faceCount = m_pMesh->GetFaceCount();

	// start the progress info
	theExporter.GetInterface()->StartProgressInfo("Analyzing the mesh...");
	
	CExporter::calexprotIsUseSide2SideEffect = FALSE;
	// loop through all faces of the mesh and put it into the corresponding submesh
	int faceId;
	for(faceId = 0; faceId < faceCount; faceId++)
	{
		// update the progress info
		theExporter.GetInterface()->SetProgressInfo(100.0f * (float)faceId / (float)faceCount);
						

		// get the material id of the face
		int materialId;
		try
		{
			materialId = m_pMesh->GetFaceMaterialId(faceId);
		}catch (...) {
			materialId = -1;
		}

		if(materialId == -1)
		{
			theExporter.GetInterface()->StopProgressInfo();
			return false;
		}

		// check if the material id is valid
		if((materialId < 0) || (materialId >= materialCount))
		{
			theExporter.GetInterface()->StopProgressInfo();
			theExporter.SetLastError("Invalid material id found!", __FILE__, __LINE__);
			return false;
		}

		// get the first vertex of the face
		CVertexCandidate *pVertexCandidate1;
		CVertexCandidate *pVertexCandidate2;
		CVertexCandidate *pVertexCandidate3;
		CalVector	pos1, pos2, pos3;

		pVertexCandidate1 = m_pMesh->GetVertexCandidate(pSkeletonCandidate, faceId, 0);
		if(pVertexCandidate1 == 0) continue;
		pVertexCandidate1->GetPosition(pos1);
		// add it to the corresponding submesh
		int vertexId1;
		vertexId1 = m_vectorSubmeshCandidate[materialId]->AddVertexCandidate(pVertexCandidate1);

		// get the second vertex of the face
		pVertexCandidate2 = m_pMesh->GetVertexCandidate(pSkeletonCandidate, faceId, 1);
		if(pVertexCandidate2 == 0) continue;
		pVertexCandidate2->GetPosition(pos2);

		// add it to the corresponding submesh
		int vertexId2;
		vertexId2 = m_vectorSubmeshCandidate[materialId]->AddVertexCandidate(pVertexCandidate2);

		// get the third vertex of the face
		pVertexCandidate3 = m_pMesh->GetVertexCandidate(pSkeletonCandidate, faceId, 2);
		if(pVertexCandidate3 == 0) continue;
		pVertexCandidate3->GetPosition(pos3);

		// add it to the corresponding submesh
		int vertexId3;
		vertexId3 = m_vectorSubmeshCandidate[materialId]->AddVertexCandidate(pVertexCandidate3);

		if((vertexId1 == -1) ||(vertexId2 == -1) ||(vertexId3 == -1))
		{
			theExporter.GetInterface()->StopProgressInfo();
			return false;
		}

		int	cnt = 0;
		for (int ifc=0; ifc<3; ifc++){
			if( fabs(pos1[ifc] - pos2[ifc]) < 0.01f &&
				fabs(pos1[ifc] - pos3[ifc]) < 0.01f)
			{
				cnt++;
			}
		}
		// add the face to the corresponding submesh
		if( cnt < 2)
		{
			if( pos3.z>286 && pos3.z<287)
				int sjfhasd=0;

			if( flipFaces)
			{
				if(!m_vectorSubmeshCandidate[materialId]->AddFace(vertexId3, vertexId2, vertexId1))
				{
					theExporter.GetInterface()->StopProgressInfo();
					return false;
				}
			}
			else
			{
				if(!m_vectorSubmeshCandidate[materialId]->AddFace(vertexId1, vertexId2, vertexId3))
				{
					theExporter.GetInterface()->StopProgressInfo();
					return false;
				}
			}
		}else
		{
			int sklfjghsdf=0;
		}
	}

	// stop the progress info
	theExporter.GetInterface()->StopProgressInfo();

	// adjust all bone assignments in the submesh candidates
	for(submeshId = 0; submeshId < m_vectorSubmeshCandidate.size(); submeshId++)
	{
		m_vectorSubmeshCandidate[submeshId]->AdjustBoneAssignment(maxBoneCount, weightThreshold);
	}

	return true;
}

bool CMeshCandidate::Create(CSkeletonCandidate *pSkeletonCandidate, int maxBoneCount, float weightThreshold, int objId)
{
	// clear current content
	try
	{
		Clear();
	}
	catch(...)
	{
	}

	// check if one (and only one!) node/mesh is selected
	if(theExporter.GetInterface()->GetSelectedNodeCount() != 1 && objId==-1)
	{
		theExporter.SetLastError("Select one (and only one) mesh to export!", __FILE__, __LINE__);
		return false;
	}

	// get the selected node
	if(objId == -1)
		m_pNode = theExporter.GetInterface()->GetSelectedNode(0);
	else
		m_pNode = theExporter.GetInterface()->GetSelectedNode(objId);
	assert( m_pNode);

	if( m_pNode->GetName().c_str())
		sName = CString(m_pNode->GetName().c_str());

	// check if the node is a mesh candidate
	if((m_pNode == 0) || !theExporter.GetInterface()->IsMesh(m_pNode))
	{
		theExporter.SetLastError("No mesh selected!", __FILE__, __LINE__);		
		return false;
	}

	return Create( pSkeletonCandidate, maxBoneCount, weightThreshold, m_pNode);
}

//----------------------------------------------------------------------------//
// Disable the LOD for the whole mesh candidate                               //
//----------------------------------------------------------------------------//

bool CMeshCandidate::DisableLOD()
{
	// disable the LOD for all the submesh candidates
	int submeshCandidateId;
	for(submeshCandidateId = 0; submeshCandidateId < m_vectorSubmeshCandidate.size(); submeshCandidateId++)
	{
		// only handle non-empty submesh candidates
		if(m_vectorSubmeshCandidate[submeshCandidateId]->GetFaceCount() > 0)
		{
			if(!m_vectorSubmeshCandidate[submeshCandidateId]->DisableLOD()) return false;
		}
	}
	
	return true;
}

//----------------------------------------------------------------------------//
// Get the submesh candidate vector                                           //
//----------------------------------------------------------------------------//

std::vector<CSubmeshCandidate *>& CMeshCandidate::GetVectorSubmeshCandidate()
{
	return m_vectorSubmeshCandidate;
}

//----------------------------------------------------------------------------//

/*void CMeshCandidate::SetFaceDesc(FACEDESCARRAY *fd)
{
//	m_pFaceDesc = fd;
}*/

void CMeshCandidate::SetFlip( bool abFlip)
{
	bFlipped = abFlip;
}

bool CMeshCandidate::IsFlipped()
{
	return bFlipped;
}

#ifdef MAX3DS_BUILD
StdMat* CMeshCandidate::GetMtrl(int id)
{
	return ((CMaxMesh*)m_pMesh)->GetMtrl(id);
}
#endif