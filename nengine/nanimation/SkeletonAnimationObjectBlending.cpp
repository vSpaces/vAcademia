
#include "StdAfx.h"
#include "GlobalSingletonStorage.h"
#include "SkeletonAnimationObject.h"
#include "PlatformDependent.h"
#include "FileFunctions.h"

#include "cal3d\animation_action.h"
#include "cal3d\animation_cycle.h"
#include "cal3d\CoreMaterial.h"
#include "cal3d\CoreSubmesh.h"
#include "cal3d\Submesh.h"
#include "cal3d\Skeleton.h"
#include "cal3d\Renderer.h"
#include "cal3d\Mixer.h"
#include "cal3d\Mesh.h"
#include "cal3d\Bone.h"

#include "ThreadAffinity.h"

#define VERTEX_EPS				0.1f

#define MAKE_MASK_AND_FIND_BOUNDS	0
#define PERFORM_INTERPOLATION		1
   
template <int operationType>
void CSkeletonAnimationObject::StartBlendCycle (SBlendOptimizationInfo* const blendOptimizationInfo, const float blendWeight, CSkeletonAnimationObject* const sao)
{
#pragma warning(push)
#pragma warning(disable : 4127)
		if (!sao)
		{
			return;
		}

		unsigned int vertexID = 0;
		int vertexOffset = 0;
		unsigned int currentVertexOffset = 0; 
		unsigned int meshCount = m_calCoreModel->getCoreMeshCount();
		
		bool isOptimizationEnabled = (blendOptimizationInfo->startVertex != -1);		
		bool isMaskMustBeRebuilded = false;
		unsigned char* mask = sao->GetMask(); 
		FILE* fl = NULL;
        
		if (MAKE_MASK_AND_FIND_BOUNDS == operationType)
		{
			USES_CONVERSION;
			std::wstring filePath = A2W(sao->GetDir().c_str());  filePath += L"isChange";
			MakeFullPathForUI(filePath);

			FILE* file = FileOpen(filePath, L"r");
			if (!file)
			{
				isMaskMustBeRebuilded = false; 
			}
			else
			{
				fclose(file);
				_wremove(filePath.c_str());
				isMaskMustBeRebuilded = true;

				filePath = A2W(sao->GetDir().c_str()); filePath += MASK_FILENAMEW;
				MakeFullPathForUI(filePath);

				fl = FileOpen(filePath, L"wb");
				if (!fl)
				{
					return;
				}

				mask = MP_NEW_ARR(unsigned char,m_vertexCount);//new unsigned char[m_vertexCount];
			}
		}

		float* blendVertices = sao->m_savedVertices;

		if ((isMaskMustBeRebuilded) || (MAKE_MASK_AND_FIND_BOUNDS != operationType))
   		for (unsigned int meshID = 0; meshID < meshCount; meshID++)
		{
			CalCoreMesh* mesh = m_calCoreModel->getCoreMesh(meshID);			
			if (!mesh)
			{
				continue;
			}

			int submeshCount = mesh->getCoreSubmeshCount();

			for (int submeshID = 0; submeshID < submeshCount; submeshID++)
			{
				CalCoreSubmesh* submesh = mesh->getCoreSubmesh(submeshID);				
				if (!submesh)
				{
					continue;
				}

				std::vector<CalCoreSubmesh::Vertex>& vx1 = submesh->getVectorVertex();
				std::vector<CalCoreSubmesh::Vertex>::iterator it = vx1.begin();
				std::vector<CalCoreSubmesh::Vertex>::iterator itEnd = vx1.end();
						
				if (isOptimizationEnabled)
				{
					if ((vertexOffset >= blendOptimizationInfo->endVertex) ||
						(vertexOffset + (int)vx1.size() < blendOptimizationInfo->startVertex))
					{
						vertexOffset += vx1.size();
						continue;
					}
					if (blendOptimizationInfo->startVertex > vertexOffset)
					{
						it += blendOptimizationInfo->startVertex - vertexOffset;
						blendVertices += (blendOptimizationInfo->startVertex - vertexOffset) * 3;
						currentVertexOffset += (blendOptimizationInfo->startVertex - vertexOffset) * 3;
						vertexID +=  (blendOptimizationInfo->startVertex - vertexOffset);
					}
					if (blendOptimizationInfo->endVertex < vertexOffset + (int)vx1.size())
					{
						itEnd -= vertexOffset + vx1.size() - blendOptimizationInfo->endVertex;
					}
				}

   				for ( ; it != itEnd; it++, blendVertices += 3)
				{
					if (MAKE_MASK_AND_FIND_BOUNDS == operationType)
					{
						if (((*(blendVertices + 0)) != m_savedVertices[currentVertexOffset + 0]) || 
							((*(blendVertices + 1)) != m_savedVertices[currentVertexOffset + 1]) || 
							((*(blendVertices + 2)) != m_savedVertices[currentVertexOffset + 2]))
						{
							if ((fabsf((*(blendVertices + 0)) - m_savedVertices[currentVertexOffset + 0]) > VERTEX_EPS) || 
								(fabsf((*(blendVertices + 1)) - m_savedVertices[currentVertexOffset + 1]) > VERTEX_EPS) || 
								(fabsf((*(blendVertices + 2)) - m_savedVertices[currentVertexOffset + 2]) > VERTEX_EPS))
							{
								mask[vertexID] = 1;								
							}
							else
							{
								mask[vertexID] = 0;								
							}
						}
						else
						{
							mask[vertexID] = 0;
						}

						currentVertexOffset += 3;
					}
					else if (PERFORM_INTERPOLATION == operationType)
					{						
						if (1 == mask[vertexID])						
						{
							(*it).position.x += ((*(blendVertices + 0)) - m_savedVertices[currentVertexOffset++]) * blendWeight;
							(*it).position.y += ((*(blendVertices + 1)) - m_savedVertices[currentVertexOffset++]) * blendWeight;
							(*it).position.z += ((*(blendVertices + 2)) - m_savedVertices[currentVertexOffset++]) * blendWeight;
						}
						else 
						{
							currentVertexOffset += 3;
						}
					}

					vertexID++;			
				} 

				vertexOffset += vx1.size();
			} 
		}

		if (MAKE_MASK_AND_FIND_BOUNDS == operationType)
		{
			if (isMaskMustBeRebuilded)
			{
				fwrite(mask, sizeof(char), vertexID, fl);
				fclose(fl); 
				sao->SetMask(mask, vertexID);
			}

			for (unsigned int i = 0; i < vertexID; i++)
			{
				if (1 == mask[i])
				if (-1 == blendOptimizationInfo->startVertex)
				{
					blendOptimizationInfo->startVertex = i;					
				}
				else
				{
					blendOptimizationInfo->endVertex = i + 1;
				}
			}
		}
#pragma warning(pop)
}

float CSkeletonAnimationObject::GetBlendWeight(CSkeletonAnimationObject* const sao)const
{
	if (!sao)
	{
		return 0.0f;
	}

	if (m_compatabilityID != sao->m_compatabilityID)
	{
		return 0.0f;
	}

	std::map<CSkeletonAnimationObject*, float>::const_iterator it = m_blends.find(sao);

	if (it == m_blends.end())
	{
		return 0.0f;
	}
	else
	{
		return (*it).second;
	}
}

void CSkeletonAnimationObject::SetBlendWeight(CSkeletonAnimationObject* const sao, const float _weight)
{
	// asynch execution

	CheckNonMainThread();

	if (!sao)
	{
		return;
	}

	if (m_compatabilityID != sao->m_compatabilityID)
	{
		return;
	}

	float weight = _weight;
	if (weight < -1.0f)
	{
		weight = -1.0f;
	}
	if (weight > 1.0f)
	{
		weight = 1.0f;
	}

	std::map<CSkeletonAnimationObject*, float>::iterator it = m_blends.find(sao);

	if (it == m_blends.end())
	{
		m_blends.insert(std::map<CSkeletonAnimationObject*, float>::value_type(sao, weight));
		m_isNeedUpdateBlending = true;
	}
	else
	{
		if (weight != 0.0f)
		{
			if ((*it).second != weight)
			{
				m_isNeedUpdateBlending = true;
				(*it).second = weight;
			}
		}
		else
		{
			m_blends.erase(it);
			m_isNeedUpdateBlending = true;
		}
	}
}

void CSkeletonAnimationObject::UpdateBlending()
{
	CheckNonMainThread();
	// asynch execution

	if ((!m_calCoreModel) || (!m_calModel) || (!m_savedVertices))
	{
		return;
	}

	int currentVertexOffset = 0;

	int meshCount = m_calCoreModel->getCoreMeshCount();
		
	for (int meshID = 0; meshID < meshCount; meshID++)
	{
		CalCoreMesh* mesh = m_calCoreModel->getCoreMesh(meshID);

		int submeshCount = mesh->getCoreSubmeshCount();

		for (int submeshID = 0; submeshID < submeshCount; submeshID++)
		{
			CalCoreSubmesh* submesh = mesh->getCoreSubmesh(submeshID);
			std::vector<CalCoreSubmesh::Vertex>& vx = submesh->getVectorVertex();
			
			std::vector<CalCoreSubmesh::Vertex>::iterator iter = vx.begin();
			std::vector<CalCoreSubmesh::Vertex>::iterator iterEnd = vx.end();

			for ( ; iter != iterEnd; iter++)
            {
				(*iter).position.x = m_savedVertices[currentVertexOffset++];
				(*iter).position.y = m_savedVertices[currentVertexOffset++];
				(*iter).position.z = m_savedVertices[currentVertexOffset++];
			}
		}
	}

	std::map<CSkeletonAnimationObject*, float>::iterator iter = m_blends.begin();
	std::map<CSkeletonAnimationObject*, float>::iterator iterEnd = m_blends.end();

	for ( ; iter != iterEnd; iter++)
	{
		float* blendVertices = NULL;
		blendVertices = (*iter).first->m_savedVertices;
		if (!blendVertices)
		{
			continue;
		}

		SBlendOptimizationInfo* blendOptimizationInfo = (*iter).first->GetBlendOptimizationInfo();
		
		float blendWeight = (*iter).second;
		if (!blendOptimizationInfo) //для этого бленда границы от первой измененной вершины до последней мы еще не посчитали, когда посчитаем - создадим эту структуру и запишем их туда
		{
			blendOptimizationInfo = MP_NEW(SBlendOptimizationInfo);//new SBlendOptimizationInfo();
			StartBlendCycle<MAKE_MASK_AND_FIND_BOUNDS>(blendOptimizationInfo, blendWeight, (*iter).first);
			(*iter).first->SetBlendOptimizationInfo(blendOptimizationInfo);
		}

		StartBlendCycle<PERFORM_INTERPOLATION>(blendOptimizationInfo, blendWeight, (*iter).first);
	}

	m_isNeedToUpdate = true;
}

void CSkeletonAnimationObject::SetBlendOptimizationInfo(SBlendOptimizationInfo* const info)
{
	// in main thread

	m_blendOptimizationInfo = info;
}

SBlendOptimizationInfo* CSkeletonAnimationObject::GetBlendOptimizationInfo()const
{
	// in main thread

	return m_blendOptimizationInfo;
}

void CSkeletonAnimationObject::SetMask(unsigned char* const mask, const unsigned int maskSize)
{
	// in main thread
	if ((m_mask != NULL)&& (m_mask != mask)) 
	{
		MP_DELETE_ARR(m_mask);
	}

	m_mask = mask;
	m_maskSize = maskSize;
}

unsigned int CSkeletonAnimationObject::GetMaskSize()const
{
	return m_maskSize;
}

unsigned char* CSkeletonAnimationObject::GetMask()
{
	// in main thread

	if (!m_mask)
	{
		m_mask = MP_NEW_ARR(unsigned char,m_vertexCount);//new unsigned char[m_vertexCount];
		memset(m_mask, 1, m_vertexCount);
		m_maskSize = m_vertexCount;
	}

	return m_mask;}