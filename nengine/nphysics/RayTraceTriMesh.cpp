
#include "StdAfx.h"
#include <float.h>
#include <Assert.h>
#include "RayTraceTriMesh.h"
#include "cal3d/memory_leak.h"
#include "GlobalSingletonStorage.h"

#define MAX_Z				5000.0f
#define COLLISION_EPSILON	0.00001f

int stat_rayCount = 0;

CRayTraceTriMesh::CRayTraceTriMesh():
	m_tc(NULL),
	m_vx(NULL),
	m_fs(NULL),
	m_edges(NULL),
	m_scale(1.0f),
	m_bounds(NULL),
	m_faceCount(0),	
	m_minX(FLT_MAX),
	m_minY(FLT_MAX),
	m_maxX(-FLT_MAX),
	m_maxY(-FLT_MAX),
	m_vertexCount(0),
	m_radiusSq(0.0f),
	m_isDataChanged(false),
	m_center(0.0f, 0.0f, 0.0f)
{
}

void CRayTraceTriMesh::SetScale(float scale)
{
	if (m_scale != scale)
	{
		m_scale = scale;
		m_isDataChanged = true;
	}
}

void CRayTraceTriMesh::SetModel(CModel* model)
{
	m_model = model;

	UpdateModelDataIfNeeded();
}

void CRayTraceTriMesh::UpdateModelDataIfNeeded()
{
	if (!m_vx)
	if (m_model->GetLoadingState() == MODEL_LOADED)
	{
		SetVertexPointer(m_model->vx, m_model->GetVertexCount());
		SetFacePointer(m_model->fs, m_model->GetFaceCount());
		SetTextureCoordsPointer(m_model->tx[0]);
	}
}

int CRayTraceTriMesh::GetTriCount()
{
	return stat_rayCount;
}

bool CRayTraceTriMesh::FastRaytraceTriangle(int v0Index, int edgeStartIndex, float *t)
{
	//stat_rayCount++;
	
	CVector3D v0 = ((CVector3D*)m_vx)[v0Index] * m_scale;
	//CVector3D& v1 = ((CVector3D*)m_vx)[index1];
	//CVector3D& v2 = ((CVector3D*)m_vx)[index2];

	CVector3D edge1, edge2, vecT, vecP, vecQ;
	float det, u, v;

	//edge1 = v1 - v0;
	//edge2 = v2 - v0;

	vecP.Cross(m_rayDirection, /*edge2*/m_edges[edgeStartIndex + 1]);
	det = m_edges[edgeStartIndex].Dot(vecP);
	vecT = m_rayOrigin - v0;
	vecQ.Cross(vecT, m_edges[edgeStartIndex]);
      
	if (det > COLLISION_EPSILON)
	{
		u = vecT.Dot(vecP);
		if ((u < 0.0f) || (u > det))
		{
			return false;
		}
            
		v = m_rayDirection.Dot(vecQ);
		if ((v < 0.0f) || (u + v > det))
		{
			return false;
		}
   }
   else if (det < -COLLISION_EPSILON)
   {
		u = vecT.Dot(vecP);
		if ((u > 0.0f) || (u < det))
		{
			return false;
		}
      
		v = m_rayDirection.Dot(vecQ) ;
		if ((v > 0.0) || (u + v < det))
		{
			return false;
		}
   }
   else
   {
	   return false;
   }

   *t = m_edges[edgeStartIndex + 1].Dot(vecQ) / det;

   return true;
}

bool CRayTraceTriMesh::RaytraceTriangle(int index0, int index1, int index2, float *t, float* u, float* v)
{
	CVector3D v0 = ((CVector3D*)m_vx)[index0] * m_scale;
	CVector3D v1 = ((CVector3D*)m_vx)[index1] * m_scale;
	CVector3D v2 = ((CVector3D*)m_vx)[index2] * m_scale;

	CVector3D edge1, edge2, vecT, vecP, vecQ;
	float det, invDet;

	edge1 = v1 - v0;
	edge2 = v2 - v0;

	vecP = m_rayDirection.Cross(edge2);
	det = edge1.Dot(vecP);
	vecT = m_rayOrigin - v0;
	invDet = 1.0f / det;
	vecQ = vecT.Cross(edge1);
      
	if (det > COLLISION_EPSILON)
	{
		*u = vecT.Dot(vecP);
		if ((*u < 0.0f) || (*u > det))
		{
			return false;
		}
            
		*v = m_rayDirection.Dot(vecQ);
		if ((*v < 0.0f) || (*u + *v > det))
		{
			return false;
		}
   }
   else if (det < -COLLISION_EPSILON)
   {
		*u = vecT.Dot(vecP);
		if ((*u > 0.0f) || (*u < det))
		{
			return false;
		}
      
		*v = m_rayDirection.Dot(vecQ) ;
		if ((*v > 0.0) || (*u + *v < det))
		{
			return false;
		}
   }
   else
   {
	   return false;
   }

   *t = edge2.Dot(vecQ) * invDet;
   *u *= invDet;
   *v *= invDet;

   return true;
}

void CRayTraceTriMesh::SetVertexPointer(float* vx, int vertexCount)
{
	assert(vx);
	assert(vertexCount > 0);

	m_vx = vx;
	m_vertexCount = vertexCount;

	m_isDataChanged = true;
}

void CRayTraceTriMesh::SetFacePointer(unsigned short* fs, int faceCount)
{
	assert(fs);
	assert(faceCount > 0);

	m_fs = fs;
	m_faceCount = faceCount;

	m_isDataChanged = true;
}

void CRayTraceTriMesh::SetTextureCoordsPointer(float* tc)
{
	assert(tc);

	m_tc = tc;
}

void CRayTraceTriMesh::RecountBounds()
{
	UpdateModelDataIfNeeded();

	if (!m_vx)
	{
		return;
	}

	FreeMemory();

	m_bounds = MP_NEW_ARR(float, m_faceCount * 4);
	m_edges = MP_NEW_ARR(CVector3D, m_faceCount * 2);

	m_minX = FLT_MAX;
	m_minY = FLT_MAX;
	m_maxX = -FLT_MAX;
	m_maxY = -FLT_MAX;

	float minZ = FLT_MAX, maxZ = -FLT_MAX;

	float x, y, z;
	for (int i = 0; i < m_faceCount; i++)
	{
		float minX = FLT_MAX, minY = FLT_MAX, maxX = -FLT_MAX, maxY = -FLT_MAX;

		CVector3D& v0 = ((CVector3D*)m_vx)[m_fs[i * 3 + 0]];
		CVector3D& v1 = ((CVector3D*)m_vx)[m_fs[i * 3 + 1]];
		CVector3D& v2 = ((CVector3D*)m_vx)[m_fs[i * 3 + 2]];

		m_edges[i * 2] = (v1 - v0) * m_scale;
		m_edges[i * 2 + 1] = (v2 - v0) * m_scale;
		
		for (int k = 0; k < 3; k++)
		{
			int index = m_fs[i * 3 + k];
			x = m_vx[index * 3];
			y = m_vx[index * 3 + 1];
			z = m_vx[index * 3 + 2];

			if (minX > x)
			{
				minX = x;
			}

			if (maxX < x)
			{
				maxX = x;
			}

			if (minY > y)
			{
				minY = y;
			}

			if (maxY < y)
			{
				maxY = y;
			}

			if (m_minX > x)
			{
				m_minX = x;
			}

			if (m_maxX < x)
			{
				m_maxX = x;
			}

			if (m_minY > y)
			{
				m_minY = y;
			}

			if (m_maxY < y)
			{
				m_maxY = y;
			}

			if (minZ > z)
			{
				minZ = z;
			}

			if (maxZ < z)
			{
				maxZ = z;
			}
		}

		m_bounds[i * 4 + 0] = minX * m_scale;
		m_bounds[i * 4 + 1] = minY * m_scale;
		m_bounds[i * 4 + 2] = maxX * m_scale;
		m_bounds[i * 4 + 3] = maxY * m_scale;
	}

	m_minX *= m_scale;
	m_minY *= m_scale;
	m_maxX *= m_scale;
	m_maxY *= m_scale;

	m_center.Set((m_minX + m_maxX) / 2.0f, (m_minY + m_maxY) / 2.0f, (minZ + maxZ) / 2.0f);

	m_radius = m_maxX - m_minX;
	if (m_radius < m_maxY - m_minY)
	{
		m_radius = m_maxY - m_minY;
	}
	if (m_radius < maxZ - minZ)
	{
		m_radius = maxZ - minZ;
	}

	m_radius /= 2.0f;
	m_radiusSq = m_radius * m_radius;
}

bool CRayTraceTriMesh::RayTraceZ(float x, float y, float* z)
{
	UpdateModelDataIfNeeded();

	if (!m_vx)
	{
		return false;
	}

	assert(m_vx);
	assert(m_fs);
	assert(z);

	if (m_isDataChanged)
	{
		RecountBounds();
		m_isDataChanged = false;
	}

	if ((x < m_minX) || (x > m_maxX) || (y < m_minY) || (y > m_maxY))
	{
		return false;
	}

	m_rayOrigin.Set(x, y, MAX_Z);
	m_rayDirection.Set(0, 0, -MAX_Z * 2.0f);

	int faceIndex = 0;
	int index1;

	float resZ = -MAX_Z;
	bool res = false;

	for (int i = 0; i < m_faceCount; i++)
	if ((x >= m_bounds[i * 4 + 0]) && (x <= m_bounds[i * 4 + 2]) &&
		(y >= m_bounds[i * 4 + 1]) && (y <= m_bounds[i * 4 + 3]))
	{
		index1 = m_fs[faceIndex];
		faceIndex += 3;

		float t;
		if (FastRaytraceTriangle(index1, i * 2, &t))
		{
			resZ = max(resZ, m_rayOrigin.z + m_rayDirection.z * t);

			res = true;
		}
	}
	else
	{
		faceIndex += 3;		
	}

	/*float minX = 500000.0f;
	float minY = 500000.0f;
	float maxX = -500000.0f;
	float maxY = -500000.0f;

	for (int i = 0; i < m_faceCount; i++)
	for (int k = 0; k < 2; k++)
	{
		if (m_bounds[i * 4 + k * 2] < minX)
			minX = m_bounds[i * 4 + k * 2];

		if (m_bounds[i * 4 + k * 2] > maxX)
			maxX = m_bounds[i * 4 + k * 2];

		if (m_bounds[i * 4 + k * 2 + 1] < minY)
			minY = m_bounds[i * 4 + k * 2 + 1];

		if (m_bounds[i * 4 + k * 2 + 1] > maxY)
			maxY = m_bounds[i * 4 + k * 2 + 1];
	}*/

	*z = resZ;

	return res;
}

bool CRayTraceTriMesh::RayTrace(CVector3D& rayOrigin, CVector3D& rayDirection, CVector3D* pos, float* tu, float* tv)
{
	UpdateModelDataIfNeeded();

	if (!m_vx)
	{
		return false;
	}

	assert(m_vx);
	assert(m_fs);
	assert(m_tc);

	assert(pos);
	assert(tu);
	assert(tv);

	int index1, index2, index3;
	int faceIndex = 0;
	m_rayOrigin = rayOrigin;
	m_rayDirection = rayDirection;

	for (int i = 0; i < m_faceCount; i++)
	{
		index1 = m_fs[faceIndex++];
		index2 = m_fs[faceIndex++];
		index3 = m_fs[faceIndex++];

		float t, u, v;
		if (RaytraceTriangle(index1, index2, index3, &t, &u, &v))
		{
			*pos = m_rayOrigin + m_rayDirection * t;

			float uvInv = 1.0f - u - v;

			index1 *= 2;
			index2 *= 2;
			index3 *= 2;

			*tu = m_tc[index1] * uvInv  + m_tc[index2] * u + m_tc[index3] * v;
			*tv = m_tc[index1 + 1] * uvInv + m_tc[index2 + 1] * u + m_tc[index3 + 1] * v;

			return true;
		}
	}

	return false;
}

bool CRayTraceTriMesh::CheckTrimeshSphereIntersect(CVector3D& rayOrigin, CVector3D& rayDirection)const
{
	CVector3D pnt = rayOrigin - m_center; 
	float a = (pnt.GetLengthSq() - m_radiusSq);
	float b = 2.0f * (pnt.x * rayDirection.x + pnt.y * rayDirection.y + pnt.z * rayDirection.z);
	float c = rayDirection.GetLengthSq();
	 
	float d = b * b - 4.0f * a * c;

	if (d < 0.0f)
	{
		return false;
	}

	return true;
}

bool CRayTraceTriMesh::CheckRayIntersect(CVector3D& rayOrigin, CVector3D& rayDirection)
{
	UpdateModelDataIfNeeded();

	if (!m_vx)
	{
		return false;
	}

	if (m_isDataChanged)
	{
		RecountBounds();
		m_isDataChanged = false;
	}

	if ((rayOrigin - m_center).GetLength() - m_radius > rayDirection.GetLength())
	{
		return false;
	}
	
	assert(m_vx);
	assert(m_fs);

	int index1;
	int faceIndex = 0;
	m_rayOrigin = rayOrigin;
	m_rayDirection = rayDirection;

	float t;
	for (int i = 0; i < m_faceCount; i++)
	{
		index1 = m_fs[faceIndex];
		faceIndex += 3;
		
		//if (RaytraceTriangle(index1, index2, index3, &t, &u, &v))
		if (FastRaytraceTriangle(index1, i * 2, &t))
		{
			if ((t >= 0.0f) && (t <= 1.0f))
			{
				return true;
			}
		}
	}

	return false;
}

void CRayTraceTriMesh::FreeMemory()
{
	if (m_bounds)
	{
		MP_DELETE_ARR(m_bounds);		
		m_bounds = NULL;
	}

	if (m_edges)
	{
		MP_DELETE_ARR(m_edges);				
		m_edges = NULL;
	}
}

CRayTraceTriMesh::~CRayTraceTriMesh()
{
	FreeMemory();
}
