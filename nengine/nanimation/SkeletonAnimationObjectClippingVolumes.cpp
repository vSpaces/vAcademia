
#include "StdAfx.h"
#include "GlobalSingletonStorage.h"
#include "SkeletonAnimationObject.h"

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
#include <malloc.h>

#define GET_CHECK_RADIUS(r, x, y, z, cx, cy, cz, a, b, c) { float xx = (x - cx) / a; float yy = (y - cy) / b; \
	float zz = (z - cz) / c; r = xx * xx + yy * yy + zz * zz; }

#define AXE_X	1
#define AXE_Y	2
#define AXE_Z	3

#define MAX_USED_TEXTURE_COUNT		100

void CSkeletonAnimationObject::ApplyClippingVolumeIfNeeded()
{
	// in main thread

	if ((!m_model) || (!m_savedIndices))
	{
		return;
	}

	if (m_isClippingChanged)
	{
		unsigned int faceCount = 0;
		unsigned short* newIndices = (unsigned short*)_alloca(m_savedFaceCount * 3 * sizeof(unsigned short));

		int textureID = 0;
		int textureOffsets[MAX_USED_TEXTURE_COUNT];

		for (unsigned int i = 0; i < m_savedFaceCount; i++)
		{
			if ((unsigned int)m_model->GetTextureOffset(textureID) == i)
			{
				textureOffsets[textureID] = faceCount;
				textureID++;
			}

			float* vertex1 = m_savedVertices + m_savedIndices[i * 3 + 0] * 3;
			float* vertex2 = m_savedVertices + m_savedIndices[i * 3 + 1] * 3;
			float* vertex3 = m_savedVertices + m_savedIndices[i * 3 + 2] * 3;

			bool isFaceNeeded = true;

			for (int volumeID = 0; volumeID < CLIPPING_VOLUME_COUNT; volumeID++)
			if (m_volumes[volumeID].isEnabled)
			{
				if (m_volumes[volumeID].type == CLIPPING_VOLUME_TYPE_BOX)
				{
					if ((vertex1[0] >= m_volumes[volumeID].minX) && (vertex1[0] <= m_volumes[volumeID].maxX) &&
						(vertex1[1] >= m_volumes[volumeID].minY) && (vertex1[1] <= m_volumes[volumeID].maxY) &&
						(vertex1[2] >= m_volumes[volumeID].minZ) && (vertex1[2] <= m_volumes[volumeID].maxZ) &&
						(vertex2[0] >= m_volumes[volumeID].minX) && (vertex2[0] <= m_volumes[volumeID].maxX) &&
						(vertex2[1] >= m_volumes[volumeID].minY) && (vertex2[1] <= m_volumes[volumeID].maxY) &&
						(vertex2[2] >= m_volumes[volumeID].minZ) && (vertex2[2] <= m_volumes[volumeID].maxZ) &&
						(vertex3[0] >= m_volumes[volumeID].minX) && (vertex3[0] <= m_volumes[volumeID].maxX) &&
						(vertex3[1] >= m_volumes[volumeID].minY) && (vertex3[1] <= m_volumes[volumeID].maxY) &&
						(vertex3[2] >= m_volumes[volumeID].minZ) && (vertex3[2] <= m_volumes[volumeID].maxZ))
					{
						isFaceNeeded = false;
					}
				}
				else
				{
					float r1, r2, r3;
					GET_CHECK_RADIUS(r1, vertex1[0], vertex1[1], vertex1[2], m_volumes[volumeID].centerX,
						m_volumes[volumeID].centerY, m_volumes[volumeID].centerZ, m_volumes[volumeID].halfLengthX,
						m_volumes[volumeID].halfLengthY, m_volumes[volumeID].halfLengthZ)

					GET_CHECK_RADIUS(r2, vertex2[0], vertex2[1], vertex2[2], m_volumes[volumeID].centerX,
						m_volumes[volumeID].centerY, m_volumes[volumeID].centerZ, m_volumes[volumeID].halfLengthX,
						m_volumes[volumeID].halfLengthY, m_volumes[volumeID].halfLengthZ)

					GET_CHECK_RADIUS(r3, vertex3[0], vertex3[1], vertex3[2], m_volumes[volumeID].centerX,
						m_volumes[volumeID].centerY, m_volumes[volumeID].centerZ, m_volumes[volumeID].halfLengthX,
						m_volumes[volumeID].halfLengthY, m_volumes[volumeID].halfLengthZ)

					if ((r1 < 1.0f) && (r2 < 1.0f) && (r3 < 1.0f))
					{
						isFaceNeeded = false;
					}
				}
			}

			if (isFaceNeeded)
			{
				newIndices[faceCount * 3 + 0] = m_savedIndices[i * 3 + 0];
				newIndices[faceCount * 3 + 1] = m_savedIndices[i * 3 + 1];
				newIndices[faceCount * 3 + 2] = m_savedIndices[i * 3 + 2];

				faceCount++;
			}
		}

		textureOffsets[textureID] = faceCount;

		// Затычка
		// Иногда ATI-шные дрова апдейтят VBO с ошибкой, если не обновлять содержимое полностью, а только
		// Х вершин с начала. Поэтому на ATI добьем недостающие индексы нулями, все равно такие вырожденные
		// треугольники эффективно отсекутся
		if (g->gi.GetVendorID() == VENDOR_ATI)
		while (faceCount != m_savedFaceCount)
		{
			newIndices[faceCount * 3 + 0] = 0;
			newIndices[faceCount * 3 + 1] = 0;
			newIndices[faceCount * 3 + 2] = 0;

			faceCount++;
		}

		textureOffsets[textureID] = faceCount;

		if (m_model->m_vbo)
		{
			for (int i = 1; i < BUFFER_COUNT; i++)
			{
				m_vbo[(m_currentVBONum + i)%BUFFER_COUNT]->SetFaceArray(newIndices, faceCount);
			}
			m_vbo[m_currentVBONum]->SetFaceArray(newIndices, faceCount);
			for (int k = 0; k < m_model->GetTextureCount() - 1; k++)
			if (m_model->m_vbos[k])
			{
				if (textureOffsets[k] < 0)
				{
					textureOffsets[k] = 0;
				}
				if (textureOffsets[k] >= (int)m_savedFaceCount)
				{
					textureOffsets[k] = m_savedFaceCount - 1;
				}

				if (textureOffsets[k + 1] < 0)
				{
					textureOffsets[k + 1] = 0;
				}
				if (textureOffsets[k + 1] >= (int)m_savedFaceCount)
				{
					textureOffsets[k + 1] = m_savedFaceCount - 1;
				}

				m_model->m_vbos[k]->SetIndexArray(newIndices, textureOffsets[k], textureOffsets[k + 1] - textureOffsets[k]);
			}
		}

		m_isClippingChanged = false;
	}
}

void CSkeletonAnimationObject::DisableClippingVolume(const int volumeID)
{
	// in main thread

	if ((volumeID < 0) || (volumeID >= CLIPPING_VOLUME_COUNT))
	{
		return;
	}

	if (m_volumes[volumeID].isEnabled)
	{
		m_volumes[volumeID].isEnabled = false;
		m_isClippingChanged = true;
	}	
}

void CSkeletonAnimationObject::SetClipplingAxe(int volumeID, int coordID, float startValue, float endValue)
{
	// in main thread

	switch (coordID)
	{
	case AXE_X:
		if ((m_volumes[volumeID].minX != startValue) || (m_volumes[volumeID].maxX != endValue))
		{
			m_volumes[volumeID].minX = startValue;
			m_volumes[volumeID].maxX = endValue;
			m_isClippingChanged = true;
		}
		break;

	case AXE_Y:
		if ((m_volumes[volumeID].minY != startValue) || (m_volumes[volumeID].maxY != endValue))
		{
			m_volumes[volumeID].minY = startValue;
			m_volumes[volumeID].maxY = endValue;
			m_isClippingChanged = true;
		}
		break;

	case AXE_Z:
		if ((m_volumes[volumeID].minZ != startValue) || (m_volumes[volumeID].maxZ != endValue))
		{
			m_volumes[volumeID].minZ = startValue;
			m_volumes[volumeID].maxZ = endValue;
			m_isClippingChanged = true;
		}
		break;
	}
}

void CSkeletonAnimationObject::SetClippingVolume(const int type, const int volumeID, 
												 const int coordID, const float startValue, const float endValue,
												 const int coordID2, const float startValue2, const float endValue2,
												 const int coordID3, const float startValue3, const float endValue3)
{
	// in main thread

	if ((volumeID < 0) && (volumeID >= CLIPPING_VOLUME_COUNT))
	{
		return;
	}

	if ((CLIPPING_VOLUME_TYPE_SPHERE == type) && (coordID3 == -1))
	{
		return;
	}

	if (m_volumes[volumeID].type != type)
	{
		m_volumes[volumeID].type = (unsigned char)type;
		m_isClippingChanged = true;
	}

	if (!m_volumes[volumeID].isEnabled)
	{
		m_volumes[volumeID].isEnabled = true;
		m_isClippingChanged = true;
	}

	SetClipplingAxe(volumeID, coordID, startValue, endValue);
	SetClipplingAxe(volumeID, coordID2, startValue2, endValue2);
	if (coordID3 != -1)
	{
		SetClipplingAxe(volumeID, coordID3, startValue3, endValue3);
	}

	if (CLIPPING_VOLUME_TYPE_SPHERE == type)
	{
		m_volumes[volumeID].centerX = (m_volumes[volumeID].minX + m_volumes[volumeID].maxX) / 2.0f;
		m_volumes[volumeID].centerY = (m_volumes[volumeID].minY + m_volumes[volumeID].maxY) / 2.0f;
		m_volumes[volumeID].centerZ = (m_volumes[volumeID].minZ + m_volumes[volumeID].maxZ) / 2.0f;
		m_volumes[volumeID].halfLengthX = (-m_volumes[volumeID].minX + m_volumes[volumeID].maxX) / 2.0f;
		m_volumes[volumeID].halfLengthY = (-m_volumes[volumeID].minY + m_volumes[volumeID].maxY) / 2.0f;
		m_volumes[volumeID].halfLengthZ = (-m_volumes[volumeID].minZ + m_volumes[volumeID].maxZ) / 2.0f;
	}	
}

void CSkeletonAnimationObject::CopyClipboxsTo(CSkeletonAnimationObject* const sao)
{
	for (int k = 0; k < CLIPPING_VOLUME_COUNT; k++)
	{
		sao->m_volumes[k] = m_volumes[k]; 
	}

	sao->m_isClippingChanged = true;
}

void CSkeletonAnimationObject::ToInitialPose()
{
	m_vbo[m_currentVBONum]->SetVertexArray(m_savedVertices, m_model->GetVertexCount());
	for (int i = 1; i < BUFFER_COUNT; i++)
	{
		m_vbo[(m_currentVBONum + i)%2]->SetVertexArray(m_savedVertices, m_model->GetVertexCount());
	}
	m_oldQuat = m_quat;
	m_quat.Set(0, 0, 1, 0);
}

void CSkeletonAnimationObject::FromInitialPose()
{
	m_vbo[m_currentVBONum]->SetVertexArray(&m_model->vx[0], m_model->GetVertexCount());
	for (int i = 1; i < BUFFER_COUNT; i++)
	{
		m_vbo[(m_currentVBONum + i)%2]->SetVertexArray(&m_model->vx[0], m_model->GetVertexCount());
	}
	m_quat = m_oldQuat;
}