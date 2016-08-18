
#include "StdAfx.h"
#include <Math.h>
#include <Assert.h>
#include <malloc.h>
#include "VBOSubObject.h"
#include "GlobalSingletonStorage.h"

#define INDICES_COUNT_IN_BILLBOARD			6
#define VERTICES_COUNT_IN_BILLBOARD			4
#define INV_VERTICES_COUNT_IN_BILLBOARD		(1.0f / (float)VERTICES_COUNT_IN_BILLBOARD)

CVBOSubObject::CVBOSubObject():
	m_vbo(NULL),
	m_faceCount(0),
	m_startOffset(0),
	m_indexMinValue(0),
	m_isCorrected(false),
	m_indexMaxValue(0xFFFFFFFF)
{
}

void CVBOSubObject::SetBillboardSize(float sizeXY, float sizeZ)
{
	m_sizeXY = sizeXY;
	m_sizeZ = sizeZ;
}

void CVBOSubObject::UnmarkCorrectness()
{
	m_isCorrected = false;	
}

bool CVBOSubObject::IsCorrected()const
{
	return m_isCorrected;
}

unsigned int CVBOSubObject::UpdateBillboardOrient(float* vertices, float* textureCoords, unsigned int vertexCount, unsigned short* indices, bool evenOnCPU)
{
	if (g->rs.GetBool(SHADERS_USING))
	if (m_isCorrected)
	{
		return BILLBOARD_PROCESSED_OK;
	}

	int operationStatus = BILLBOARD_PROCESSED_OK; 
	bool isMustUpdateVBO = !m_isCorrected;
	m_isCorrected = true;

	unsigned int pairCount = m_faceCount >> 1;
	bool* vxUsed = (bool *)_alloca(vertexCount);
	memset(vxUsed, 0, vertexCount);

	int vertexIDs[VERTICES_COUNT_IN_BILLBOARD];
	float centerX, centerY, centerZ;
	unsigned int vertexCountInBillboard;

	unsigned int indicesOffset = m_startOffset * 3;
	
	for (unsigned int faceOffset = 0; faceOffset < pairCount; faceOffset++)
	{	
		centerX = 0.0f;
		centerY = 0.0f;
		centerZ = 0.0f;
		vertexCountInBillboard = 0;

		unsigned int indicesOffsetEnd = indicesOffset + INDICES_COUNT_IN_BILLBOARD;
		
		for ( ; indicesOffset < indicesOffsetEnd; indicesOffset++)
		{
			unsigned short vertexID = indices[indicesOffset];

			bool isFound = false;

			for (unsigned int i = 0; i < vertexCountInBillboard; i++)
			if (vertexIDs[i] == vertexID)
			{
				isFound = true;
				break;
			}

			if (!isFound)
			{
				if (vertexCountInBillboard >= VERTICES_COUNT_IN_BILLBOARD)
				{					
					return BILLBOARD_PROCESSED_FAILED;
				}

				vertexIDs[vertexCountInBillboard] = vertexID;
				vertexCountInBillboard++;

				unsigned int vertexShift = vertexID * 3;
				centerX += vertices[vertexShift];
				vertexShift++;
				centerY += vertices[vertexShift];
				vertexShift++;
				centerZ += vertices[vertexShift];
			}
		}

		centerX *= INV_VERTICES_COUNT_IN_BILLBOARD;
		centerY *= INV_VERTICES_COUNT_IN_BILLBOARD;
		centerZ *= INV_VERTICES_COUNT_IN_BILLBOARD;

		indicesOffset -= INDICES_COUNT_IN_BILLBOARD;

		for ( ; indicesOffset < indicesOffsetEnd; indicesOffset++)
		{
			unsigned short vertexID = indices[indicesOffset];			

			if (vxUsed[vertexID])
			{
				operationStatus = BILLBOARD_PROCESSED_SHARED_VERTICES_WARNING;				
			}
			
			unsigned int vertexShift = vertexID * 3;
			vertices[vertexShift] = centerX;
			vertexShift++;
			vertices[vertexShift] = centerY;
			vertexShift++;
			vertices[vertexShift] = centerZ;
		}

		indicesOffset -= INDICES_COUNT_IN_BILLBOARD;

		for ( ; indicesOffset < indicesOffsetEnd; indicesOffset++)
		{
			unsigned short vertexID = indices[indicesOffset];
			vxUsed[vertexID] = true;
		}
	}

	if ((!g->rs.GetBool(SHADERS_USING)) && (g->ne.GetWorkMode() == WORK_MODE_NORMALRENDER) && (evenOnCPU))
	{
		operationStatus = BILLBOARD_PROCESSED_OK;

		if (isMustUpdateVBO)
		{
			m_vbo->SetVertexArray(vertices, vertexCount);
		}		

		CVector3D vec1 = g->br.m_particleVector1 * m_sizeZ;
		CVector3D vec2 = g->br.m_particleVector2 * m_sizeXY;
		int vertexCount3 = vertexCount * 3;
		int shiftT = 0;

		for (int shift = 0; shift < vertexCount3; shiftT += 2, shift += 3)
		{
			float tx = (textureCoords[shiftT] - 0.5f);
			float ty = (textureCoords[shiftT + 1] - 0.5f);

			vertices[shift + 0] = vertices[shift + 0] + tx * vec1.x - ty * vec2.x;
			vertices[shift + 1] = vertices[shift + 1] + tx * vec1.y - ty * vec2.y;
			vertices[shift + 2] = vertices[shift + 2] + tx * vec1.z - ty * vec2.z;
		}		
	}
	else
	{
		m_vbo->SetVertexArray(vertices, vertexCount);
	}

	return operationStatus;
}

void CVBOSubObject::SetIndexArray(unsigned short* indices, unsigned int startOffset, unsigned int faceCount)
{
	assert(startOffset >= 0);
	//assert(faceCount > 0);

	m_startOffset = startOffset;
	m_faceCount = faceCount;

	m_indexMinValue = 0xFFFFFFFF;
	m_indexMaxValue = 0;

	unsigned int endIndicesID = (startOffset + faceCount) * 3;

	for (unsigned int indicesID = startOffset * 3; indicesID < endIndicesID; indicesID++)
	{
		if (indices[indicesID] > m_indexMaxValue)
		{
			m_indexMaxValue = indices[indicesID];
		} 

		if (indices[indicesID] < m_indexMinValue)
		{
			m_indexMinValue = indices[indicesID];
		}
	}
}

void CVBOSubObject::SetBufferObject(CBaseBufferObject* vbo)
{
	assert(vbo);
	m_vbo = vbo;
}

void CVBOSubObject::Render()
{
	assert(m_vbo);	
	//assert(m_faceCount > 0);

	if (m_faceCount > 0)
	m_vbo->RenderRange(m_startOffset, m_faceCount, m_indexMinValue, m_indexMaxValue);
}

CVBOSubObject::~CVBOSubObject()
{
}