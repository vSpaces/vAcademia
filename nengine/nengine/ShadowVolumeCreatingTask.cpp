
#include "StdAfx.h"
#include <Assert.h>
#include "ShadowObject.h"
#include "ShadowVolumeCreatingTask.h"
#include "cal3d/memory_leak.h"

CShadowVolumeCreatingTask::CShadowVolumeCreatingTask():
	m_fs(NULL)
{
}

void CShadowVolumeCreatingTask::PrintInfo()
{
	g->lw.WriteLn("ShadowVolumeCreatingTask");
}

void CShadowVolumeCreatingTask::SetShadowObject(void* _shadowObject)
{
	CShadowObject* shadowObject = (CShadowObject*)_shadowObject;
	assert(shadowObject);
	assert(shadowObject->m_fs);
	assert(shadowObject->GetFaceCount() > 0);

	m_fs = shadowObject->m_fs;
	m_faceCount = shadowObject->GetFaceCount();

	/*if (shadowObject->GetFaceCount() == 3527)
	{
		int index1 = ((Face*)m_fs)[121 * 3].vertexIndices[0];
		int index2 = ((Face*)m_fs)[121 * 3].vertexIndices[1];
		int index3 = ((Face*)m_fs)[121 * 3].vertexIndices[2];
		int index4 = ((Face*)m_fs)[695 * 3].vertexIndices[0];
		int index5 = ((Face*)m_fs)[695 * 3].vertexIndices[1];
		int index6 = ((Face*)m_fs)[695 * 3].vertexIndices[2];

		float x1 = shadowObject->m_vx[index1].x;
		float y1 = shadowObject->m_vx[index1].y;
		float z1 = shadowObject->m_vx[index1].z;
		float x2 = shadowObject->m_vx[index2].x;
		float y2 = shadowObject->m_vx[index2].y;
		float z2 = shadowObject->m_vx[index2].z;
		float x3 = shadowObject->m_vx[index3].x;
		float y3 = shadowObject->m_vx[index3].y;
		float z3 = shadowObject->m_vx[index3].z;
		float x4 = shadowObject->m_vx[index4].x;
		float y4 = shadowObject->m_vx[index4].y;
		float z4 = shadowObject->m_vx[index4].z;
		float x5 = shadowObject->m_vx[index5].x;
		float y5 = shadowObject->m_vx[index5].y;
		float z5 = shadowObject->m_vx[index5].z;
		float x6 = shadowObject->m_vx[index6].x;
		float y6 = shadowObject->m_vx[index6].y;
		float z6 = shadowObject->m_vx[index6].z;
	}*/
}

void CShadowVolumeCreatingTask::Start()
{
	Face* fs = (Face*)m_fs;

	for (int i = 0; i < m_faceCount; i++)
	{
		int k;
		for (k = 0; k < 3; k++)
		{
			fs[i].neighbourIndices[k] = -1;
		}

		for (k = 0; k < m_faceCount; k++)
		if (i != k)
		if (IsFacesNeighbours(fs[i].vertexIndices, fs[k].vertexIndices))
		{
			if ((m_sovp[0] == 0) && (m_sovp[1] == 1))
			{
				fs[i].neighbourIndices[0] = k;
			}
			else
			if ((m_sovp[0] == 1) && (m_sovp[1] == 2))
			{
				fs[i].neighbourIndices[1] = k;
			}
			else
			if ((m_sovp[0] == 0) && (m_sovp[1] == 2))
			{
				fs[i].neighbourIndices[2] = k;
			}
		}
	}	
}

void CShadowVolumeCreatingTask::SelfDestroy()
{
	MP_DELETE_THIS;
}

CShadowVolumeCreatingTask::~CShadowVolumeCreatingTask()
{
}