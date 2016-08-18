
#include "StdAfx.h"
#include "Vector3D.h"
#include "ShadowObject.h"
#include "GlobalSingletonStorage.h"

CShadowObject::CShadowObject()
{
	m_lightPosition.x = 0;
	m_lightPosition.y = 1;
	m_lightPosition.z = 0;
	m_isCasted = false;
	m_isCreated = false;
	m_infinity = INFINITY;
	m_shadowPower = 1.3f;

	m_faceCount = 0;
	m_vertexCount = 0;

	m_vx = NULL;
	m_vn = NULL;
	m_fs = NULL;
	m_task.OnDone();
}

void CShadowObject::SetShadowSource(CVector3D& pos)
{
	m_lightPosition.x = pos.x;
	m_lightPosition.y = pos.y;
	m_lightPosition.z = pos.z;
}

void CShadowObject::SetShadowSource(float x, float y, float z)
{
	m_lightPosition.x = x;
	m_lightPosition.y = y;
	m_lightPosition.z = z;
}

void CShadowObject::SetShadowPower(float power)
{
	m_shadowPower = power;
}

void CShadowObject::SetInfinity(float inf)
{
	m_infinity = inf;	
}

void CShadowObject::SetVertexCount(int count)
{
	m_vertexCount = count;
}

int CShadowObject::GetVertexCount()const
{
	return m_vertexCount;
}

void CShadowObject::SetFaceCount(int count)
{
	if (m_faceCount != count)
	{
		if (!m_isCreated)
		{
			while (!m_task.IsPerformed())
			{
				Sleep(1);
			}
		}

		if (m_fs)
		{
			MP_DELETE_ARR(m_fs);
		}

		m_fs = MP_NEW_ARR(Face, count);
	}

	m_faceCount = count;
}

int CShadowObject::GetFaceCount()const
{
	return m_faceCount;
}

void CShadowObject::SetModelHeight(float modelHeight)
{
	m_modelHeight = modelHeight;
}

float CShadowObject::GetModelHeight()const
{
	return m_modelHeight;
}

float CShadowObject::GetShadowPower()const
{
	return m_shadowPower;
}

float CShadowObject::GetInfinity()const
{
	return m_infinity;
}

void CShadowObject::SetCastedStatus(bool isCasted)
{
	m_isCasted = isCasted;
}

bool CShadowObject::IsCasted()const
{
	return m_isCasted;
}

CVector3D& CShadowObject::GetLightPosition()
{
	return m_lightPosition;
}

void CShadowObject::Create()
{
#ifndef SHADOWMAPS
	if (!m_isCreated)
	{
		while (!m_task.IsPerformed())
		{
			Sleep(1);
		}
	}

	m_task.Rewind();
	m_task.SetShadowObject(this);
	g->taskm.AddTask(&m_task, MUST_BE_MULTITHREADED);
	m_isCreated = false;
#else
	m_isCreated = false;
#endif
}

bool CShadowObject::IsCreated()
{
#ifndef SHADOWMAPS
	if (!m_isCreated)
	{
		m_isCreated = m_task.IsPerformed();
	}

	return m_isCreated;
#else
	return false;
#endif
}

void CShadowObject::FreeResources()
{
	if (m_fs)
	{
		MP_DELETE_ARR(m_fs);
		m_fs = NULL;
	}

	m_vx = NULL;
	m_vn = NULL;
	m_faceCount = 0;
	m_vertexCount = 0;
}

CShadowObject::~CShadowObject()
{
	FreeResources();
}