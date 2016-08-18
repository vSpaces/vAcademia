// WeightVertexManager.cpp: implementation of the CWeightVertexManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Exporter.h"
#include "WeightVertexManager.h"
#include "WeightVertex.h"
#include "SkeletonCandidate.h"
#include "VertexCandidate.h"
#include "MaxMesh.h"
#include "BoneCandidate.h"
#include "BaseNode.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


TCHAR *translateNodeNameToOtherSideName( TCHAR *old_node_name, TCHAR *new_node_name, TCHAR *leftID, TCHAR *rightID)
{
	assert( old_node_name);
	assert( new_node_name);
	if ( old_node_name == NULL || new_node_name == NULL)
		return NULL;
	Cal3DComString node_name = Cal3DComString( old_node_name);
	int strPos = -1;
	if (( strPos = node_name.Find( leftID)) > -1)
	{
		node_name.Replace( leftID, rightID);
	}
	else if ((strPos = node_name.Find( rightID)) > -1)
	{
		node_name.Replace( rightID, leftID);
	}
	strcpy( new_node_name, node_name.GetBuffer());
	return new_node_name;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWeightVertexManager::CWeightVertexManager()
{
	pSkeletonCandidate = NULL;
	pVertexCandidate = NULL;
	pModifier = NULL;
}

CWeightVertexManager::~CWeightVertexManager()
{
	clear();
}

//////////////////////////////////////////////////////////////////////////

int CWeightVertexManager::getCount()
{
	return weightVertexList.size();
}

BOOL CWeightVertexManager::isEmpty()
{
	return weightVertexList.empty();
}

CWeightVertex *CWeightVertexManager::getWeightVertex( int id)
{
	if ( id < 0 && id >= weightVertexList.size())
		return NULL;
	return weightVertexList[ id];
}


void CWeightVertexManager::setSkeletonCandidate( CSkeletonCandidate *pSkeletonCandidate)
{
	this->pSkeletonCandidate = pSkeletonCandidate;
}

void CWeightVertexManager::setVertexCandidate( CVertexCandidate *pVertexCandidate)
{
	this->pVertexCandidate = pVertexCandidate;
}

void CWeightVertexManager::setModifier( Modifier *pModifier)
{
	this->pModifier = pModifier;
}

void CWeightVertexManager::setMaxMesh( CMaxMesh *pMasMesh)
{
	this->pMasMesh = pMasMesh;
}

void CWeightVertexManager::clear()
{
	int weightVertexId;
	for(weightVertexId = 0; weightVertexId < weightVertexList.size(); weightVertexId++)
	{
		delete weightVertexList[ weightVertexId];
	}
	
	weightVertexList.clear();
}


//----------------------------------------------------------------------------------------//
// Find the symmetric vertex candidate for a given vertex of a given face of the mesh    //
//----------------------------------------------------------------------------------------//

int CWeightVertexManager::FindSymmetricVertex( CVertexCandidate *pVertexCandidate, float valueExact)
{
	int vertexId = -1;

	int faceCount = pMasMesh->GetFaceCount();
	
	CalVector pos;
	pVertexCandidate->GetPosition( pos);

	// отладочное явление
	if (( pos.x > 1.99f && pos.x < 2.01f
			&& pos.y > 2.29f && pos.y < 2.31f 
			&& pos.z > 35.9f && pos.z < 36.1f)
		|| ( pos.x > 1.99f && pos.x < 2.01f
			&& pos.y > -1.71f && pos.y < -1.69f 
			&& pos.z > 35.9f && pos.z < 36.1f))	
	{
		int id = valueExact;
		int k = id;
		theExporter.SetLastError( "CWeightVertexManager::FindSymmetricVertex::Debug information bug faces", __FILE__, __LINE__);
	}

	if ( CExporter::calexprotUseLeftAssigment2Right)
	{
		if ( pos.x < 0.0f + valueExact)
			return vertexId;
	}
	else if ( CExporter::calexprotUseRigthAssigment2Left)
	{
		if ( pos.x > 0.0f - valueExact)
			return vertexId;	
	}	

	Box3 box = pMasMesh->GetIMesh()->getBoundingBox( pMasMesh->GetMatrix3());
	Point3 maxPoint = box.Max();
	Point3 minPoint = box.Min();	
	if ( minPoint.x > 0.0f - valueExact || maxPoint.x < 0.0f + valueExact)
		return vertexId;

	pos.x = -pos.x;
	CalVector minPos = pos;
	CalVector maxPos = pos;
	minPos -= valueExact;
	maxPos += valueExact;
	
	if ( maxPos.x > 0.0f && minPos.x < 0.0f)
		return vertexId;

	Mesh *m_pIMesh = pMasMesh->GetIMesh();
	for ( int faceId = 0; faceId < faceCount; faceId++) 
	{
		if ( faceId >= m_pIMesh->getNumFaces())
			return -1;

		for ( int faceVertexId = 0; faceVertexId < 3; faceVertexId++)
		{			
			vertexId = m_pIMesh->faces[ faceId].v[ faceVertexId];
			Point3 vertex;
			vertex = m_pIMesh->getVert(vertexId) * (*pMasMesh->GetMatrix3());

			if ( CExporter::calexprotUseLeftAssigment2Right)
			{
				if ( vertex.x > 0.0f - valueExact)
					continue;
			}
			else if ( CExporter::calexprotUseRigthAssigment2Left)
			{
				if ( vertex.x < 0.0f + valueExact)
					continue;	
			}

			if ( vertex.x > minPos.x && vertex.x < maxPos.x
				&& vertex.y > minPos.y && vertex.y < maxPos.y
				&& vertex.z > minPos.z && vertex.z < maxPos.z)
			{
				return vertexId;
			}
		}		
	}
	
	return -1;
}


int CWeightVertexManager::createVertex_PHYSIQUE( int vertexId, int vertexType)
{
	if ( !CExporter::calexprotIsUseSide2SideEffect)
		CExporter::calexprotIsUseSide2SideEffect = TRUE;
	// create a physique import interface	
	IPhysiqueImport *pPhysiqueImport;
	pPhysiqueImport = (IPhysiqueImport *)pModifier->GetInterface(I_PHYIMPORT);
	if(pPhysiqueImport == 0)
	{			
		theExporter.SetLastError("Physique modifier interface not found.", __FILE__, __LINE__);
		return 0;
	}

	// create a context export interface
	IPhyContextImport *pContextImport;
	pContextImport = (IPhyContextImport *)pPhysiqueImport->GetContextInterface(pMasMesh->GetNode());
	if(pContextImport == 0)
	{
		pModifier->ReleaseInterface(I_PHYIMPORT, pContextImport);		
		theExporter.SetLastError("Context import interface not found.", __FILE__, __LINE__);
		return 0;
	}	
	
	// set the vertex import interface			
	IPhyVertexImport *pVertexImport;
	pVertexImport = (IPhyVertexImport *)pContextImport->SetVertexInterface(vertexId, vertexType);
	if(pVertexImport == 0)
	{
		pPhysiqueImport->ReleaseContextInterface(pContextImport);
		pModifier->ReleaseInterface(I_PHYIMPORT, pPhysiqueImport);				
		theExporter.SetLastError("Vertex export interface not found.", __FILE__, __LINE__);
		return 0;
	}

	if ( vertexType == RIGID_BLENDED_TYPE)
	{
		IPhyBlendedRigidVertexImport *pTypeVertexImport;
		pTypeVertexImport = ( IPhyBlendedRigidVertexImport *) pVertexImport;

		BOOL bFirst = TRUE;
		for ( int i = 0; i < weightVertexList.size(); i++)
		{			
			pVertexImport->LockVertex( TRUE);
			CWeightVertex *pWeightVertex = weightVertexList[ i];
			INode *node = pWeightVertex->getNode();
			if ( node != NULL)
			{
				if ( pTypeVertexImport->SetWeightedNode( node, pWeightVertex->getWeight(), bFirst) == 0)
				{						
					theExporter.SetLastError("Vertex import SetWeightedNode is not valid.", __FILE__, __LINE__);
					break;
				}
			}
			else
			{
				std::vector<CBoneCandidate*> boneCandList = pSkeletonCandidate->GetVectorBoneCandidate();
				for ( int j = 0; j < boneCandList.size(); j++)
				{
					CBaseNode *baseNode = boneCandList[ j]->GetNode();
					if ( baseNode == NULL)
						continue;
					if ( strcmp( pWeightVertex->getNodeName(), baseNode->GetName().data()) == 0)
					{
						if ( pTypeVertexImport->SetWeightedNode( boneCandList[ j]->GetNode()->GetINode(), pWeightVertex->getWeight(), bFirst) == 0)
						{
							theExporter.SetLastError("Vertex import SetWeightedNode is not valid.", __FILE__, __LINE__);						
							break;
						}
						break;
					}
				}
			}
			if ( bFirst)
				bFirst = FALSE;
			pVertexImport->LockVertex( FALSE);
		}
	}
	else if ( vertexType == RIGID_TYPE)
	{
		IPhyRigidVertexImport *pTypeVertexImport;
		pTypeVertexImport = ( IPhyRigidVertexImport *) pVertexImport;
		for ( int i = 0; i < weightVertexList.size(); i++)
		{			
			pVertexImport->LockVertex( TRUE);
			CWeightVertex *pWeightVertex = weightVertexList[ i];
			INode *node = pWeightVertex->getNode();
			if ( node != NULL)
			{
				if ( !pTypeVertexImport->SetNode( node))
				{						
					theExporter.SetLastError("Vertex import SetNode is not valid.", __FILE__, __LINE__);
					break;
				}
			}
			else
			{
				std::vector<CBoneCandidate*> boneCandList = pSkeletonCandidate->GetVectorBoneCandidate();
				for ( int j = 0; j < boneCandList.size(); j++)
				{
					CBaseNode *baseNode = boneCandList[ j]->GetNode();
					if ( baseNode == NULL)
						continue;
					if ( strcmp( pWeightVertex->getNodeName(), baseNode->GetName().data()) == 0)
					{
						if ( pTypeVertexImport->SetNode( boneCandList[ j]->GetNode()->GetINode()) == 0)
						{
							theExporter.SetLastError("Vertex import SetNode is not valid.", __FILE__, __LINE__);						
							break;
						}
						break;
					}
				}
			}
			pVertexImport->LockVertex( FALSE);
		}
	}

	pPhysiqueImport->ReleaseContextInterface(pContextImport);
	pModifier->ReleaseInterface(I_PHYIMPORT, pPhysiqueImport);
	return 1;
}

int CWeightVertexManager::createVertex_SKIN( int vertexId)
{
	if ( !CExporter::calexprotIsUseSide2SideEffect)
		CExporter::calexprotIsUseSide2SideEffect = TRUE;
#if MAX_RELEASE >= 4000
	// check for skin modifier
	// create a skin interface
	ISkin *pSkin;
	pSkin = (ISkin*)pModifier->GetInterface(I_SKIN);
	if( pSkin == 0)
	{		
		theExporter.SetLastError("Skin modifier interface not found.", __FILE__, __LINE__);
		return 0;
	}

	// create a skin import data interface
	ISkinImportData *pSkinImportData;
	pSkinImportData = (ISkinImportData *)pModifier->GetInterface(I_SKINIMPORTDATA);
	if(pSkinImportData == 0)
	{
		pModifier->ReleaseInterface(I_SKIN, pSkin);		
		theExporter.SetLastError("Skin import data interface not found.", __FILE__, __LINE__);
		return 0;
	}

	//pSkinImportData->AddBoneEx( node, TRUE);

	Tab<INode*> boneNodeList;
	Tab<float> weights;

	for ( int i = 0; i < weightVertexList.size(); i++)
	{		
		CWeightVertex *pWeightVertex = weightVertexList[ i];
		INode *node = pWeightVertex->getNode();
		if ( node != NULL)
		{			
			//iskinImport->AddBoneEx(boneNodeList[i],TRUE);

			boneNodeList.Append( 1, &node);
			float fW = pWeightVertex->getWeight();
			weights.Append( 1, &fW);
			
		}
		else
		{
			std::vector<CBoneCandidate*> boneCandList = pSkeletonCandidate->GetVectorBoneCandidate();
			for ( int j = 0; j < boneCandList.size(); j++)
			{
				CBaseNode *baseNode = boneCandList[ j]->GetNode();
				if ( baseNode == NULL)
					continue;
				if ( strcmp( pWeightVertex->getNodeName(), baseNode->GetName().data()) == 0)
				{
					INode *node = baseNode->GetINode();
					boneNodeList.Append( 1, &node);
					float fW = pWeightVertex->getWeight();
					weights.Append( 1, &fW);
					break;
				}
			}
		}
	}

	if ( !pSkinImportData->AddWeights( pMasMesh->GetNode(), vertexId, boneNodeList, weights))
	{
		theExporter.SetLastError("Vertex import pSkinImportData->AddWeights() is not valid.", __FILE__, __LINE__);						
		return 0;
	}
#endif

	return 1;
}

//----------------------------------------------------------------------------//
// Get the weights of symmetric vertex candidate for a given vertex of a given face of the mesh    //
//----------------------------------------------------------------------------//

int CWeightVertexManager::ExecuteAssigmentSide2Side_PHYSIQUE( int currVertexId, float valueExact, IPhyVertexExport *new_pVertexExport)
{
	assert( pSkeletonCandidate);
	assert( pVertexCandidate);
	assert( pModifier);
	assert( pMasMesh);

	if ( pSkeletonCandidate == NULL || pVertexCandidate == NULL || pModifier == NULL || pMasMesh == NULL)
		return FALSE;	

	if ( !( CExporter::calexprotUseLeftAssigment2Right || CExporter::calexprotUseRigthAssigment2Left))
		return TRUE;

	int vertexId;
	vertexId = FindSymmetricVertex( pVertexCandidate, CExporter::calexprotPosCompareValueExact);
	if ( vertexId == -1)
		return TRUE;
	
	// create a physique export interface
	IPhysiqueExport *pPhysiqueExport;
	pPhysiqueExport = (IPhysiqueExport *)pModifier->GetInterface(I_PHYINTERFACE);
	if( pPhysiqueExport == 0)
	{			
		theExporter.SetLastError("Physique modifier interface not found.", __FILE__, __LINE__);
		return FALSE;
	}

	// create a context export interface
	IPhyContextExport *pContextExport;
	pContextExport = (IPhyContextExport *)pPhysiqueExport->GetContextInterface(pMasMesh->GetNode());
	if( pContextExport == 0)
	{
		pModifier->ReleaseInterface(I_PHYINTERFACE, pPhysiqueExport);
		//delete pVertexCandidate;
		theExporter.SetLastError("Context export interface not found.", __FILE__, __LINE__);
		return FALSE;
	}

	// set the flags in the context export interface
	pContextExport->ConvertToRigid(TRUE);
	pContextExport->AllowBlending(TRUE);

	// get the vertex export interface
	IPhyVertexExport *pVertexExport;
	pVertexExport = (IPhyVertexExport *)pContextExport->GetVertexInterface(vertexId);
	if( pVertexExport == 0)
	{
		pPhysiqueExport->ReleaseContextInterface(pContextExport);
		pModifier->ReleaseInterface(I_PHYINTERFACE, pPhysiqueExport);			
		theExporter.SetLastError("Vertex export interface not found.", __FILE__, __LINE__);
		return FALSE;
	}

	// get the vertex type
	int vertexType;
	vertexType = pVertexExport->GetVertexType();

	if( vertexType == RIGID_BLENDED_TYPE)
	{
		// typecast to blended vertex
		IPhyBlendedRigidVertex *pTypeVertex;
		pTypeVertex = (IPhyBlendedRigidVertex *)pVertexExport;
				
		if ( vertexType != new_pVertexExport->GetVertexType())
		{
			TCHAR nodeName[ MAX_PATH];
			for( int nodeId = 0; nodeId < pTypeVertex->GetNumberNodes(); nodeId++)
			{	
				CWeightVertex *pWeightVertex = new CWeightVertex( translateNodeNameToOtherSideName( pTypeVertex->GetNode( nodeId)->GetName(), nodeName, " L ", " R "), 
																	pTypeVertex->GetWeight( nodeId));				
				weightVertexList.push_back( pWeightVertex);					
			}	
		}
		else
		{	
			IPhyBlendedRigidVertex *new_pTypeVertex;
			new_pTypeVertex = (IPhyBlendedRigidVertex *)new_pVertexExport;

			int count = new_pTypeVertex->GetNumberNodes();
			
			// loop through all influencing bones			
			TCHAR nodeName[ MAX_PATH];
			if ( count > pTypeVertex->GetNumberNodes())
			{
				for( int nodeId = 0; nodeId < pTypeVertex->GetNumberNodes(); nodeId++)
				{	
					CWeightVertex *pWeightVertex = new CWeightVertex( new_pTypeVertex->GetNode( nodeId), pTypeVertex->GetWeight( nodeId));
					weightVertexList.push_back( pWeightVertex);					
				}			
			}
			else if ( count <= pTypeVertex->GetNumberNodes())
			{
				int nodeId;
				for(nodeId = 0; nodeId < count; nodeId++)
				{	
					CWeightVertex *pWeightVertex = new CWeightVertex( new_pTypeVertex->GetNode( nodeId), pTypeVertex->GetWeight( nodeId));
					weightVertexList.push_back( pWeightVertex);
				}
				for ( nodeId = count; nodeId < pTypeVertex->GetNumberNodes(); nodeId++)
				{					
					// преобразование имени
					CWeightVertex *pWeightVertex = new CWeightVertex( translateNodeNameToOtherSideName( pTypeVertex->GetNode( nodeId)->GetName(), nodeName, " L ", " R "), 
																		pTypeVertex->GetWeight( nodeId));
					weightVertexList.push_back( pWeightVertex);
				}
			}
		}		
	
		if ( createVertex_PHYSIQUE( currVertexId, vertexType) == 0)
		{			
			return FALSE;
		}
	}
	else if ( vertexType == RIGID_TYPE)
	{
		IPhyRigidVertex *pTypeVertex;
		pTypeVertex = (IPhyRigidVertex *)pVertexExport;
		if ( vertexType != new_pVertexExport->GetVertexType())
		{
			TCHAR nodeName[ MAX_PATH];
			CWeightVertex *pWeightVertex = new CWeightVertex( translateNodeNameToOtherSideName( pTypeVertex->GetNode()->GetName(), nodeName, " L ", " R "), 1.0f);
			weightVertexList.push_back( pWeightVertex);
		}
		else
		{
			IPhyRigidVertex *new_pTypeVertex;
			new_pTypeVertex = (IPhyRigidVertex *)new_pVertexExport;
			CWeightVertex *pWeightVertex = new CWeightVertex( new_pTypeVertex->GetNode(), 1.0f);
			weightVertexList.push_back( pWeightVertex);
		}

		if ( createVertex_PHYSIQUE( currVertexId, vertexType) == 0)
		{			
			return FALSE;
		}
	}

	// release all interfaces
	pPhysiqueExport->ReleaseContextInterface(pContextExport);
	pModifier->ReleaseInterface(I_PHYINTERFACE, pPhysiqueExport);
	
	return TRUE;
}

//----------------------------------------------------------------------------//
// Get the weights of symmetric vertex candidate for a given vertex of a given face of the mesh    //
//----------------------------------------------------------------------------//

int CWeightVertexManager::ExecuteAssigmentSide2Side_SKIN( int currVertexId, float valueExact, ISkin *new_pSkin, ISkinContextData *new_pSkinContextData)
{
	assert( pSkeletonCandidate);
	assert( pVertexCandidate);
	assert( pModifier);
	assert( pMasMesh);

	if ( pSkeletonCandidate == NULL || pVertexCandidate == NULL || pModifier == NULL || pMasMesh == NULL)
		return FALSE;

	int count = new_pSkinContextData->GetNumAssignedBones(currVertexId);
	if ( count == 0)
		return TRUE;

	if ( !( CExporter::calexprotUseLeftAssigment2Right || CExporter::calexprotUseRigthAssigment2Left))
		return TRUE;

	int vertexId;
	vertexId = FindSymmetricVertex( pVertexCandidate, CExporter::calexprotPosCompareValueExact);
	if ( vertexId == -1)
		return TRUE;
		
#if MAX_RELEASE >= 4000
	// check for skin modifier
	// create a skin interface
	ISkin *pSkin;
	pSkin = (ISkin*)pModifier->GetInterface(I_SKIN);
	if( pSkin == 0)
	{		
		theExporter.SetLastError("Skin modifier interface not found.", __FILE__, __LINE__);
		return FALSE;
	}

	// create a skin context data interface
	ISkinContextData *pSkinContextData;
	pSkinContextData = (ISkinContextData *)pSkin->GetContextInterface(pMasMesh->GetNode());
	if(pSkinContextData == 0)
	{
		pModifier->ReleaseInterface(I_SKIN, pSkin);		
		theExporter.SetLastError("Skin context data interface not found.", __FILE__, __LINE__);
		return FALSE;
	}

	// loop through all influencing bones	
	TCHAR nodeName[ MAX_PATH];
	if ( count > pSkinContextData->GetNumAssignedBones(vertexId))
	{
		for( int nodeId = 0; nodeId < pSkinContextData->GetNumAssignedBones(vertexId); nodeId++)
		{
			int boneId = new_pSkinContextData->GetAssignedBone( vertexId, nodeId);
			CWeightVertex *pWeightVertex = new CWeightVertex( new_pSkin->GetBone( boneId), pSkinContextData->GetBoneWeight( vertexId, nodeId));
			weightVertexList.push_back( pWeightVertex);
		}		
	}
	else if ( count <= pSkinContextData->GetNumAssignedBones(vertexId))
	{
		int nodeId;
		for(nodeId = 0; nodeId < count; nodeId++)
		{
			int boneId = new_pSkinContextData->GetAssignedBone( currVertexId, nodeId);
			CWeightVertex *pWeightVertex = new CWeightVertex( new_pSkin->GetBone( boneId), pSkinContextData->GetBoneWeight( vertexId, nodeId));
			weightVertexList.push_back( pWeightVertex);
		}
		for ( nodeId = count; nodeId < pSkinContextData->GetNumAssignedBones(vertexId); nodeId++)
		{
			int boneId = pSkinContextData->GetAssignedBone( vertexId, nodeId);				
			// преобразование имени				
			CWeightVertex *pWeightVertex = new CWeightVertex( translateNodeNameToOtherSideName( pSkin->GetBone( boneId)->GetName(), nodeName, " L ", " R "), 
																pSkinContextData->GetBoneWeight( vertexId, nodeId));
			weightVertexList.push_back( pWeightVertex);
		}
	}
	
	// release all interfaces
	pModifier->ReleaseInterface(I_SKIN, pSkin);	
#endif
	
	if ( createVertex_SKIN( currVertexId) == 0)
	{
		return FALSE;
	}

	return TRUE;
}
