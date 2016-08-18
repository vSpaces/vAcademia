//----------------------------------------------------------------------------//
// MaxMesh.cpp                                                                //
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
#include "MaxMesh.h"
#include "BaseInterface.h"
#include "SkeletonCandidate.h"
#include "VertexCandidate.h"
#include "WeightVertexManager.h"
#include "WeightVertex.h"

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
CMaxMesh::CMaxMesh()
{
	m_pINode = 0;
	m_pIMesh = 0;
	m_bDelete = false;
	m_modifierType = MODIFIER_NONE;
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

CMaxMesh::~CMaxMesh()
{
/*	try
	{
		if(m_bDelete) delete m_pIMesh;
	}
	catch (...) {
		int iouh=0;
	}*/
}

//----------------------------------------------------------------------------//
// Add a bone influence to a given vertex candidate                           //
//----------------------------------------------------------------------------//

bool CMaxMesh::AddBoneInfluence(CSkeletonCandidate *pSkeletonCandidate, CVertexCandidate *pVertexCandidate, INode *pNode, float weight)
{
	// get the bone id of the bone from the skeleton candidate
	if ( pNode == NULL) 
		return false;

	/*int boneId;
	TCHAR *name = pNode->GetName();
	boneId = pSkeletonCandidate->GetBoneId(name);
	if(boneId == -1) 
		return false;

	// add the influence to the vertex candidate
	pVertexCandidate->AddInfluence(boneId, weight);*/
	return _AddBoneInfluence(pSkeletonCandidate, pVertexCandidate, pNode->GetName(), weight);

	//return true;
}

bool CMaxMesh::_AddBoneInfluence(CSkeletonCandidate *pSkeletonCandidate, CVertexCandidate *pVertexCandidate, TCHAR *nodeName, float weight)
{
	int boneId;	
	boneId = pSkeletonCandidate->GetBoneId(nodeName);
	if(boneId == -1) 
		return false;

	// add the influence to the vertex candidate
	pVertexCandidate->AddInfluence(boneId, weight);

	return true;
}

//----------------------------------------------------------------------------//
// Create a max node instance                                                 //
//----------------------------------------------------------------------------//

bool CMaxMesh::Create(INode *pINode, Mesh *pIMesh, bool bDelete)
{
	// check for valid mesh
	if(pIMesh == 0)
	{
		theExporter.SetLastError("Invalid handle.", __FILE__, __LINE__);
		return false;
	}

	m_pINode = pINode;
	m_pIMesh = pIMesh;
	m_bDelete = bDelete;

	// recursively create materials
	if(!CreateMaterial(m_pINode->GetMtl())) return false;

	// build all normals if necessary
	m_pIMesh->checkNormals(TRUE);

	// precalculate the object transformation matrix
	m_tm = m_pINode->GetObjectTM(SecToTicks(theExporter.GetInterface()->GetCurrentTime()));

	// try to get the physique modifier
	m_pModifier = FindPhysiqueModifier(pINode);
	if(m_pModifier == 0)
	{
		// try to get the skin modifier
		m_pModifier = FindSkinModifier(pINode);
		if(m_pModifier == 0)
		{
			//theExporter.SetLastError("No physique/skin modifier found.", __FILE__, __LINE__);
			//return false;
			m_modifierType = MODIFIER_MESHALONE;
			return true;	// если не нашли skin или physique, то экспортим меш без привязки
		}
		else
		{
			m_modifierType = MODIFIER_SKIN;
		}
	}
	else
	{
		m_modifierType = MODIFIER_PHYSIQUE;
	}

	return true;
}

//----------------------------------------------------------------------------//
// Create all materials for the max mesh                                     //
//----------------------------------------------------------------------------//

bool CMaxMesh::CreateMaterial(Mtl *pMtl, LPCTSTR alpName, int realID)
{
	Cal3DComString	namePrefix;
	if( alpName)	namePrefix = alpName;

	// check for valid material
	if(pMtl == 0)
	{
		theExporter.SetLastError("Invalid material.", __FILE__, __LINE__);
		return false;
	}

	if( alpName)
	{
		Cal3DComString	mtrlName = namePrefix + "_";
		mtrlName += pMtl->GetName();
		pMtl->SetName( mtrlName.GetBuffer());
	}


	// check if we have a standard material
	if(pMtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
	{
		// insert new material
		SubmeshMaterial	mtrlDesc;
		mtrlDesc.pStdMat = (StdMat *)pMtl;
		mtrlDesc.realID = realID;
		m_vectorStdMat.push_back(mtrlDesc);
		//m_vectorStdMat.push_back((StdMat *)pMtl);
	}
	else	if( CExporter::calexpoptUseBakedIfExists)
	{
		Mtl*	mtlBaked = NULL;
		if( pMtl->ClassID() == Class_ID(BAKE_SHELL_CLASS_ID,0))
		{
			mtlBaked = pMtl;
		}
		if( mtlBaked)
		{
			namePrefix = mtlBaked->GetName();
			ATLASSERT( mtlBaked->NumSubMtls() == 2);
			if( mtlBaked->NumSubMtls() == 2)
			{
				Mtl* bakedMaterial = mtlBaked->GetSubMtl(1);	// Get baked material
				ATLASSERT( bakedMaterial->ClassID() == Class_ID(DMTL_CLASS_ID, 0) ||
							bakedMaterial->ClassID() == Class_ID(MULTI_CLASS_ID, 0));
				/*if( bakedMaterial->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
				{
					pMtl = bakedMaterial;
					m_vectorStdMat.push_back((StdMat *)pMtl);
					return true;
				}
				else	if( bakedMaterial->ClassID() == Class_ID(MULTI_CLASS_ID, 0))
				{
					pMtl = bakedMaterial;
				}*/
				return CreateMaterial( bakedMaterial, namePrefix.GetBuffer(), realID);
			}
		}
	}

	// handle all submaterials
	int subId;
	int subCount = pMtl->NumSubMtls();
	for(subId = 0; subId < subCount; subId++)
	{
		TSTR	slotName = pMtl->GetSubMtlSlotName(subId);
		CreateMaterial(pMtl->GetSubMtl(subId), namePrefix.IsEmpty()?NULL:namePrefix.GetBuffer(), subId);
	}

	return true;
}

//----------------------------------------------------------------------------//
// Find the physique modifier for a given node                                //
//----------------------------------------------------------------------------//

Modifier *CMaxMesh::FindPhysiqueModifier(INode *pINode)
{
	// get the object reference of the node
	Object *pObject;
	pObject = pINode->GetObjectRef();
	if(pObject == 0) return 0;

	// loop through all derived objects
	while(pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		IDerivedObject *pDerivedObject;
		pDerivedObject = static_cast<IDerivedObject *>(pObject);

		// loop through all modifiers
		int stackId;
		for(stackId = 0; stackId < pDerivedObject->NumModifiers(); stackId++)
		{
			// get the modifier
			Modifier *pModifier;
			pModifier = pDerivedObject->GetModifier(stackId);

			// check if we found the physique modifier
			if(pModifier->ClassID() == Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B)) return pModifier;
		}

		// continue with next derived object
		pObject = pDerivedObject->GetObjRef();
	}

	return 0;
}

//----------------------------------------------------------------------------//
// Find the skin modifier for a given node                                    //
//----------------------------------------------------------------------------//

Modifier *CMaxMesh::FindSkinModifier(INode *pINode)
{
#if MAX_RELEASE >= 4000
	// get the object reference of the node
	Object *pObject;
	pObject = pINode->GetObjectRef();
	if(pObject == 0) return 0;

	// loop through all derived objects
	while(pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		IDerivedObject *pDerivedObject;
		pDerivedObject = static_cast<IDerivedObject *>(pObject);

		// loop through all modifiers
		int stackId;
		for(stackId = 0; stackId < pDerivedObject->NumModifiers(); stackId++)
		{
			// get the modifier
			Modifier *pModifier;
			pModifier = pDerivedObject->GetModifier(stackId);

			// check if we found the skin modifier
			if(pModifier->ClassID() == SKIN_CLASSID) return pModifier;
		}

		// continue with next derived object
		pObject = pDerivedObject->GetObjRef();
	}
#endif

	return 0;
}

//----------------------------------------------------------------------------//
// Get the number of faces of the mesh                                        //
//----------------------------------------------------------------------------//

int CMaxMesh::GetFaceCount()
{
	// check for valid mesh
	if(m_pIMesh == 0)
	{
		theExporter.SetLastError("Invalid handle.", __FILE__, __LINE__);
		return false;
	}

	return m_pIMesh->getNumFaces();
}

//----------------------------------------------------------------------------//
// Get the internal max mesh of the mesh                                      //
//----------------------------------------------------------------------------//

Mesh *CMaxMesh::GetIMesh()
{
	return m_pIMesh;
}

//----------------------------------------------------------------------------//
// Get the number of materials of the mesh                                    //
//----------------------------------------------------------------------------//

int CMaxMesh::GetMaterialCount()
{
	return m_vectorStdMat.size();
}

//----------------------------------------------------------------------------//
// Get the material id for a given face of the mesh                           //
//----------------------------------------------------------------------------//

int CMaxMesh::GetFaceMaterialId(int faceId)
{
	// check for valid mesh
	if(m_pIMesh == 0)
	{
		theExporter.SetLastError("Invalid handle.", __FILE__, __LINE__);
		return -1;
	}

	// get the material id of the face
	int materialId;
	materialId = m_pIMesh->getFaceMtlIndex(faceId);

	// get the material count of this mesh
	int materialCount;
	materialCount = m_vectorStdMat.size();

	for (int i=0; i<materialCount; i++)
	{
		if( m_vectorStdMat[i].realID == materialId)
			return i;
	}

	// make sure we return a valid material id in all cases:
	// 3ds max assigns predefined material ids to primitives. these material
	// ids can be higher than the actual material count of the mesh itself.
	// the official workaround is to calculate the modulo of the value to the
	// number of existing materials.
	if(materialCount > 0)
	{
		materialId %= materialCount;
	}
	else
	{
		materialId = 0;
	}

	return materialId;
}

//----------------------------------------------------------------------------//
// Get the number of maps in a given submesh of the mesh                      //
//----------------------------------------------------------------------------//

int CMaxMesh::GetSubmeshMapCount(int submeshId)
{
	// check if the submesh id is valid
	if((submeshId < 0) || (submeshId >= m_vectorStdMat.size()))
	{
		theExporter.SetLastError("Invalid handle.", __FILE__, __LINE__);
		return -1;
	}

	// get the material of the submesh
	StdMat *pStdMat;
	pStdMat = m_vectorStdMat[submeshId].pStdMat;
	//pStdMat = m_vectorStdMat[submeshId];

	// count all the mapping channels in this material
	int mapCount;
	mapCount = 0;

	int mapId;
	for(mapId = 0; mapId < pStdMat->NumSubTexmaps(); mapId++)
	{
		// get texture map
		Texmap *pTexMap;
		pTexMap = pStdMat->GetSubTexmap(mapId);

		// check if map is valid
		if((pTexMap != 0) && (pStdMat->MapEnabled(mapId)))
		{
			// check if we have a valid texture coordinate
			if((m_pIMesh->mapSupport(pTexMap->GetMapChannel())) || (m_pIMesh->numTVerts > 0))
			{
				mapCount++;
			}
		}
	}

	return mapCount;
}

//----------------------------------------------------------------------------//
// Get the material thead id for a given submesh of the mesh                  //
//----------------------------------------------------------------------------//

int CMaxMesh::GetSubmeshMaterialThreadId(int submeshId)
{
	//return submeshId;	//	Material[]

	// check if the submesh id is valid
	if((submeshId < 0) || (submeshId >= m_vectorStdMat.size()))
	{
		theExporter.SetLastError("Invalid handle.", __FILE__, __LINE__);
		return -1;
	}

	// get the material of the submesh
	StdMat *pStdMat;
	pStdMat = m_vectorStdMat[submeshId].pStdMat;
	//pStdMat = m_vectorStdMat[submeshId];

	// get name of the material
	std::string strName;
	strName = pStdMat->GetName();

	// get positions of the material thread id
	std::string::size_type openPos;
	openPos = strName.find_last_of("[");
	std::string::size_type closePos;
	closePos = strName.find_last_of("]");
	if((openPos == std::string::npos) || (closePos == std::string::npos) || (++openPos >= closePos))
	{
		//theExporter.SetLastError("Invalid material thread id in material.", __FILE__, __LINE__);
		//return -1;
		return submeshId;	//	Material[]
	}

	// extract material thread id from material name
	std::string strMaterialThreadId;
	strMaterialThreadId = strName.substr(openPos, closePos - openPos);

	int materialThreadId;
	materialThreadId = atoi(strMaterialThreadId.c_str());

	return materialThreadId;
}

//----------------------------------------------------------------------------//
// Get the vertex candidate for a given vertex of a given face of the mesh    //
//----------------------------------------------------------------------------//

CVertexCandidate *CMaxMesh::GetVertexCandidate(CSkeletonCandidate *pSkeletonCandidate, int faceId, int faceVertexId)
{
	// check for valid mesh and physique modifier
	if((m_pIMesh == 0) || ((m_pModifier == 0)&&( m_modifierType != MODIFIER_MESHALONE)))
	{
		theExporter.SetLastError("Invalid handle.", __FILE__, __LINE__);
		return 0;
	}	

	// check if face id is valid
	if((faceId < 0) || (faceId >= m_pIMesh->getNumFaces()))
	{
		theExporter.SetLastError("Invalid face id found.", __FILE__, __LINE__);
		return 0;
	}

	// check if face vertex id is valid
	if((faceVertexId < 0) || (faceVertexId >= 3))
	{
		theExporter.SetLastError("Invalid face vertex id found.", __FILE__, __LINE__);
		return 0;
	}

	// allocate a new vertex candidate
	CVertexCandidate *pVertexCandidate;
	pVertexCandidate = new CVertexCandidate();
	if(pVertexCandidate == 0)
	{
		theExporter.SetLastError("Memory allocation failed.", __FILE__, __LINE__);
		return 0;
	}

	// create the new vertex candidate
	if(!pVertexCandidate->Create())
	{
		delete pVertexCandidate;
		return 0;
	}

	// get vertex id
	int vertexId;
	vertexId = m_pIMesh->faces[faceId].v[faceVertexId];

	/*static Color white(1,1,1), black(0,0,0);

	BOOL init=FALSE;
	Color col=white;

	TVFace *cf = GetMesh().mapFaces(mp);
	UVVert *cv = GetMesh().mapVerts(mp);
	if (!cf || !cv) return col;

	for (int i=0; i<GetMesh().getNumFaces(); i++) {
		if (!GetMesh().faceSel[i]) continue;
		DWORD *tt = cf[i].t;
		for (int j=0; j<3; j++) {
			if (!init) {
				col = cv[tt[j]];
				init = TRUE;
			} else {
				Color ac = cv[tt[j]];
				if (ac!=col) return black;
			}
		}
	}
	return col;*/

	//TVFace *cf = m_pIMesh->mapFaces(MAP_SHADING);
	//UVVert *cv = m_pIMesh->mapVerts(MAP_SHADING);

	// get the absolute vertex position
	Point3 vertex;
	vertex = m_pIMesh->getVert(vertexId) * m_tm;

	/*if( vertexId==420 || vertexId==419)
		int iwfe=0;*/

	// отладочное явление
	if (( vertex.x > 0.786f && vertex.x < 0.790f 
			&& vertex.y > -1.700f && vertex.y < -1.696f 
			&& vertex.z > 19.511f && vertex.z < 19.515f)
		|| (vertex.x > 0.412f && vertex.x < 0.416f
			&& vertex.y > -1.700f && vertex.y < -1.696f 
			&& vertex.z > 19.511f && vertex.z < 19.515f))
	{
		int id = vertexId;
		int k = id;
		theExporter.SetLastError( "Debug information bug faces", __FILE__, __LINE__);
	}
	// set the vertex candidate position
	pVertexCandidate->SetPosition(vertex.x, vertex.y, vertex.z);

	if(m_pIMesh->getRVertPtr(0) == NULL)
	{
		theExporter.SetLastError("Произошла внутренняя ошибка 3ds Max и модель не экспортировалась.\nПопробуйте перезапустить программу и повторить попытку.","", 666);
		if (faceId == 0)
		{
			AfxMessageBox(theExporter.GetELastError().c_str(), MB_OK | MB_ICONEXCLAMATION);
		}
		return 0;
	}

	// get the absolute vertex normal
	Point3 normal;
	normal = GetVertexNormal(faceId, vertexId);
	normal = normal * Inverse(Transpose(m_tm));
	normal = normal.Normalize();
	
	// set the vertex candidate normal
	pVertexCandidate->SetNormal(normal.x, normal.y, normal.z);

	// get the vertex weight array
	float *pVertexWeights;
	pVertexWeights = m_pIMesh->getVertexWeights();

	// get the vertex weight (if possible)
	float weight;
	if(pVertexWeights != 0)
	{
		weight = pVertexWeights[vertexId];
	}
	else
	{
		weight = 0.0f;
	}

	// another 3ds max weird behaviour:
	// zero out epsilon weights
	if(weight < 0.0005f) weight = 0.0f;

	// set the vertex candidate weight
	pVertexCandidate->SetPhysicalProperty(weight);

	// get the material id of the face
	int materialId;
	materialId = GetFaceMaterialId(faceId);
	if((materialId < 0) || (materialId >= m_vectorStdMat.size()))
	{
		delete pVertexCandidate;
		theExporter.SetLastError("Invalid material id found.", __FILE__, __LINE__);
		return 0;
	}

	// get the material of the face
	StdMat *pStdMat;
	pStdMat = m_vectorStdMat[materialId].pStdMat;
	//pStdMat = m_vectorStdMat[materialId];

	// loop through all the mapping channels and extract texture coordinates
	int mapId;
	for(mapId = 0; mapId < pStdMat->NumSubTexmaps(); mapId++)
	{
		// get texture map
		Texmap *pTexMap;
		pTexMap = pStdMat->GetSubTexmap(mapId);

		// check if map is valid
		if((pTexMap != 0) && (pStdMat->MapEnabled(mapId)))
		{
			// get the mapping channel
			int channel;
			channel = pTexMap->GetMapChannel();

			bool bValidUV;
			bValidUV = false;

			// extract the texture coordinate
			UVVert uvVert;
			if(m_pIMesh->mapSupport(channel))
			{
				TVFace *pTVFace;
				pTVFace = m_pIMesh->mapFaces(channel);

				UVVert *pUVVert;
				pUVVert = m_pIMesh->mapVerts(channel);

				uvVert = pUVVert[pTVFace[faceId].t[faceVertexId]];
				bValidUV = true;
			}
			else if(m_pIMesh->numTVerts > 0)
			{
				uvVert = m_pIMesh->tVerts[m_pIMesh->tvFace[faceId].t[faceVertexId]];
				bValidUV = true;
			}

			// if we found valid texture coordinates, add them to the vertex candidate
			if(bValidUV)
			{
				// apply a possible uv generator
				StdUVGen *pStdUVGen;
				pStdUVGen = (StdUVGen *)pTexMap->GetTheUVGen();
				if(pStdUVGen != 0)
				{
					Matrix3 tmUV;
					pStdUVGen->GetUVTransform(tmUV);
					uvVert = uvVert * tmUV;
				}

				// add texture coordinate to the vertex candidate
				pVertexCandidate->AddTextureCoordinate(uvVert.x, uvVert.y);
			}
		}
	}

	// check for physique modifier
	if(m_modifierType == MODIFIER_PHYSIQUE)
	{
		// create a physique export interface
		IPhysiqueExport *pPhysiqueExport;
		pPhysiqueExport = (IPhysiqueExport *)m_pModifier->GetInterface(I_PHYINTERFACE);
		if(pPhysiqueExport == 0)
		{
			delete pVertexCandidate;
			theExporter.SetLastError("Physique modifier interface not found.", __FILE__, __LINE__);
			return 0;
		}

		// create a context export interface
		IPhyContextExport *pContextExport;
		pContextExport = (IPhyContextExport *)pPhysiqueExport->GetContextInterface(m_pINode);
		if(pContextExport == 0)
		{
			m_pModifier->ReleaseInterface(I_PHYINTERFACE, pPhysiqueExport);
			delete pVertexCandidate;
			theExporter.SetLastError("Context export interface not found.", __FILE__, __LINE__);
			return 0;
		}

		// set the flags in the context export interface
		pContextExport->ConvertToRigid(TRUE);
		pContextExport->AllowBlending(TRUE);

		// get the vertex export interface
		IPhyVertexExport *pVertexExport;
		pVertexExport = (IPhyVertexExport *)pContextExport->GetVertexInterface(vertexId);
		if(pVertexExport == 0)
		{
			pPhysiqueExport->ReleaseContextInterface(pContextExport);
			m_pModifier->ReleaseInterface(I_PHYINTERFACE, pPhysiqueExport);
			delete pVertexCandidate;
			theExporter.SetLastError("Vertex export interface not found.", __FILE__, __LINE__);
			return 0;
		}

		// get the vertex type
		int vertexType;
		vertexType = pVertexExport->GetVertexType();

		CWeightVertexManager *pWeightVertexManager = new CWeightVertexManager();
		pWeightVertexManager->setSkeletonCandidate( pSkeletonCandidate);
		pWeightVertexManager->setVertexCandidate( pVertexCandidate);
		pWeightVertexManager->setModifier( m_pModifier);
		pWeightVertexManager->setMaxMesh( this);

		// handle the specific vertex type
		if(vertexType == RIGID_TYPE)
		{
			// typecast to rigid vertex
			IPhyRigidVertex *pTypeVertex;
			pTypeVertex = (IPhyRigidVertex *)pVertexExport;			
			// add the influence to the vertex candidate
			if ( !pWeightVertexManager->ExecuteAssigmentSide2Side_PHYSIQUE( vertexId, CExporter::calexprotPosCompareValueExact, pTypeVertex))
			{
				delete pVertexCandidate;
				return 0;
			}		
			// get the influencing bone
			if(!AddBoneInfluence(pSkeletonCandidate, pVertexCandidate, pTypeVertex->GetNode(), 1.0f))
			{
				pPhysiqueExport->ReleaseContextInterface(pContextExport);
				m_pModifier->ReleaseInterface(I_PHYINTERFACE, pPhysiqueExport);
				delete pVertexCandidate;
				theExporter.SetLastError("Invalid bone assignment.", __FILE__, __LINE__);
				return 0;
			}
		}
		else if(vertexType == RIGID_BLENDED_TYPE)
		{
			// typecast to blended vertex
			IPhyBlendedRigidVertex *pTypeVertex;
			pTypeVertex = (IPhyBlendedRigidVertex *)pVertexExport;

			// loop through all influencing bones			
			int count = pTypeVertex->GetNumberNodes();			

			if ( !pWeightVertexManager->ExecuteAssigmentSide2Side_PHYSIQUE( vertexId, CExporter::calexprotPosCompareValueExact, pTypeVertex))
			{
				delete pVertexCandidate;
				return 0;
			}
			int nodeId;

			if ( pWeightVertexManager->isEmpty())
			{
				count = pTypeVertex->GetNumberNodes();
				for(nodeId = 0; nodeId < count; nodeId++)
				{
					float aWeight;					
					aWeight = pTypeVertex->GetWeight(nodeId);										
					// add the influence to the vertex candidate
					if(!AddBoneInfluence(pSkeletonCandidate, pVertexCandidate, pTypeVertex->GetNode(nodeId), aWeight))
					{
						pPhysiqueExport->ReleaseContextInterface(pContextExport);
						m_pModifier->ReleaseInterface(I_PHYINTERFACE, pPhysiqueExport);
						delete pVertexCandidate;
						theExporter.SetLastError("Invalid bone assignment.", __FILE__, __LINE__);
						return 0;
					}				
				}
			}
			else
			{				
				for(nodeId = 0; nodeId < pWeightVertexManager->getCount(); nodeId++)
				{
					CWeightVertex *pWeightVertex;
					pWeightVertex = pWeightVertexManager->getWeightVertex( nodeId);
									
					// add the influence to the vertex candidate
					if(!_AddBoneInfluence(pSkeletonCandidate, pVertexCandidate, pWeightVertex->getNodeName(), pWeightVertex->getWeight()))
					{
						pPhysiqueExport->ReleaseContextInterface(pContextExport);
						m_pModifier->ReleaseInterface(I_PHYINTERFACE, pPhysiqueExport);
						delete pVertexCandidate;
						theExporter.SetLastError("Invalid bone assignment.", __FILE__, __LINE__);
						return 0;
					}				
				}
			}			
		}
		
		delete pWeightVertexManager;

		// release all interfaces
		pPhysiqueExport->ReleaseContextInterface(pContextExport);
		m_pModifier->ReleaseInterface(I_PHYINTERFACE, pPhysiqueExport);
	}
#if MAX_RELEASE >= 4000
	// check for skin modifier
	else if(m_modifierType == MODIFIER_SKIN)
	{
		// create a skin interface
		ISkin *pSkin;
		pSkin = (ISkin*)m_pModifier->GetInterface(I_SKIN);
		if(pSkin == 0)
		{
			delete pVertexCandidate;
			theExporter.SetLastError("Skin modifier interface not found.", __FILE__, __LINE__);
			return 0;
		}

		// create a skin context data interface
		ISkinContextData *pSkinContextData;
		pSkinContextData = (ISkinContextData *)pSkin->GetContextInterface(m_pINode);
		if(pSkinContextData == 0)
		{
			m_pModifier->ReleaseInterface(I_SKIN, pSkin);
			delete pVertexCandidate;
			theExporter.SetLastError("Skin context data interface not found.", __FILE__, __LINE__);
			return 0;
		}

		// loop through all influencing bones
		std::vector<CWeightVertex*> weightVertexList;
		int count = pSkinContextData->GetNumAssignedBones(vertexId);
		CWeightVertexManager *pWeightVertexManager = new CWeightVertexManager();
		pWeightVertexManager->setSkeletonCandidate( pSkeletonCandidate);
		pWeightVertexManager->setVertexCandidate( pVertexCandidate);
		pWeightVertexManager->setModifier( m_pModifier);
		pWeightVertexManager->setMaxMesh( this);
		if ( !pWeightVertexManager->ExecuteAssigmentSide2Side_SKIN( count, CExporter::calexprotPosCompareValueExact, pSkin, pSkinContextData))
		{
			delete pVertexCandidate;
			return 0;
		}
		int nodeId;
		if ( pWeightVertexManager->isEmpty())
		{
			for(nodeId = 0; nodeId < count; nodeId++)
			{
				// get the bone id
				int boneId;
				boneId = pSkinContextData->GetAssignedBone(vertexId, nodeId);
				if(boneId < 0) continue;

				float aWeight;				
				aWeight = pSkinContextData->GetBoneWeight(vertexId, nodeId);				

				// add the influence to the vertex candidate
				if(!AddBoneInfluence(pSkeletonCandidate, pVertexCandidate, pSkin->GetBone(boneId), aWeight))
				{
					m_pModifier->ReleaseInterface(I_SKIN, pSkin);
					delete pVertexCandidate;
					theExporter.SetLastError("Invalid bone assignment.", __FILE__, __LINE__);
					return 0;
				}
			}
		}
		else
		{
			for(nodeId = 0; nodeId < pWeightVertexManager->getCount(); nodeId++)
			{
				// get the bone id				
				CWeightVertex *pWeightVertex;
				pWeightVertex = pWeightVertexManager->getWeightVertex( nodeId);

				// add the influence to the vertex candidate
				if(!_AddBoneInfluence(pSkeletonCandidate, pVertexCandidate, pWeightVertex->getNodeName(), pWeightVertex->getWeight()))
				{
					m_pModifier->ReleaseInterface(I_SKIN, pSkin);
					delete pVertexCandidate;
					theExporter.SetLastError("Invalid bone assignment.", __FILE__, __LINE__);
					return 0;
				}
			}
		}

		int weightVertexId;
		for(weightVertexId = 0; weightVertexId < weightVertexList.size(); weightVertexId++)
		{
			delete weightVertexList[ weightVertexId];
		}

		weightVertexList.clear();

		// release all interfaces
		m_pModifier->ReleaseInterface(I_SKIN, pSkin);
	}
#endif
  /*else
	{
		theExporter.SetLastError("No physique/skin modifier found.", __FILE__, __LINE__);
		return 0;
	}*/

	return pVertexCandidate;
}

//----------------------------------------------------------------------------//
// Get the absolute normal of a given vertex of a given face                  //
//----------------------------------------------------------------------------//

Point3 CMaxMesh::GetVertexNormal(int faceId, int vertexId)
{
	// get the "rendered" vertex
	RVertex *pRVertex;
	pRVertex = m_pIMesh->getRVertPtr(vertexId);

	// get the face
	Face *pFace;
	pFace = &m_pIMesh->faces[faceId];

	// get the smoothing group of the face
	DWORD smGroup;
	smGroup = pFace->smGroup;

	// get the number of normals
	int normalCount;
	normalCount = pRVertex->rFlags & NORCT_MASK;

	// check if the normal is specified ...
	if(pRVertex->rFlags & SPECIFIED_NORMAL)
	{
		return pRVertex->rn.getNormal();
	}
	// ... otherwise, check for a smoothing group
	else if((normalCount > 0) && (smGroup != 0))
	{
		// If there is only one vertex is found in the rn member.
		if(normalCount == 1)
		{
			return pRVertex->rn.getNormal();
		}
		else
		{
			int normalId;
			for(normalId = 0; normalId < normalCount; normalId++)
			{
				if(pRVertex->ern[normalId].getSmGroup() & smGroup)
				{
					return pRVertex->ern[normalId].getNormal();
				}
			}
		}
	}

	// if all fails, return the face normal
	return m_pIMesh->getFaceNormal(faceId);
}

//----------------------------------------------------------------------------//
// Get the transpose of a given matrix                                        //
//----------------------------------------------------------------------------//

Matrix3 CMaxMesh::Transpose(Matrix3& matrix)
{
	float transpose[4][3];

	int row;
	for(row = 0; row < 3; row++)
	{
		int column;
		for(column = 0; column < 3; column++)
		{
			transpose[column][row] = matrix.GetAddr()[row][column];
		}
	}

	int column;
	for(column = 0; column < 3; column++)
	{
		transpose[3][column] = 0;
	}

	Matrix3 transposeMatrix(transpose);
	transposeMatrix.NoTrans();

	return transposeMatrix;
}

int CMaxMesh::GetCal3dVertexIndex(int faceId, int faceVertexId){
	return m_pIMesh->faces[faceId].v[faceVertexId];
}
//----------------------------------------------------------------------------//

StdMat* CMaxMesh::GetMtrl(int id)
{
	ASSERT(id < m_vectorStdMat.size());
	return m_vectorStdMat[id].pStdMat;
	//return m_vectorStdMat[id];
}

Matrix3 *CMaxMesh::GetMatrix3()
{
	return &m_tm;
}

INode *CMaxMesh::GetNode()
{
	return m_pINode;
}
