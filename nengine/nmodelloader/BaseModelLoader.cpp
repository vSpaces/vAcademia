
#include "StdAfx.h"
#include <windows.h>
#include "Vector3D.h"
#include "IModelLoader.h"
#include "BaseModelLoader.h"
#include <malloc.h>
#include "GlobalSingletonStorage.h"
#include "../../cal3d/memory_leak.h"

CTimeProcessor* g_tp = NULL;

int g_maxFacePerVertexCount = 0;

#define MAX_FACES_PER_VERTEX	100

CBaseModelLoader::CBaseModelLoader():
	m_vertexCount(0),
	m_faceCount(0),	
	m_facesNormals(NULL),
	m_calculateNormalsTime(0)
{
	
}

void CBaseModelLoader::GetNormal(CVector3D& p1, CVector3D& p2, CVector3D& p3, CVector3D& n)
{
    CVector3D d1, d2;

	d1 = p2 - p1;
	d2 = p2 - p3;

	n = d1.Cross(d2);
    n.Normalize();
}

void CBaseModelLoader::CalculateNormals(IModel* model)
{
	unsigned int maxOffset = model->GetVertexCount();
	unsigned short* vertexFaces = (unsigned short*)_alloca(maxOffset * 2);	
	
	float* vx = model->GetVertexPointer();
	float* vn = model->GetNormalPointer();
	unsigned short* fs = model->GetIndexPointer();

	if (model->GetFaceCount() > m_faceCount)
	{
		if (m_facesNormals)
		{
			MP_DELETE_ARR(m_facesNormals);
		}

		m_facesNormals = MP_NEW_ARR(float, model->GetFaceCount() * 3);
		m_faceCount = model->GetFaceCount();
	}

	CVector3D p1, p2, p3, n;
	int offset = 0;

	int x;
	for (x = 0; x < model->GetFaceCount(); x++)
	{
		int k = fs[x * 3 + 0];
		p1.Set(vx[k * 3 + 0], vx[k * 3 + 1], vx[k * 3 + 2]);

		k = fs[x * 3 + 1];
		p2.Set(vx[k * 3 + 0], vx[k * 3 + 1], vx[k * 3 + 2]);

		k = fs[x * 3 + 2];
		p3.Set(vx[k * 3 + 0], vx[k * 3 + 1], vx[k * 3 + 2]);

		GetNormal(p1, p2, p3, n);

		m_facesNormals[offset++] = n.x;
		m_facesNormals[offset++] = n.y;
		m_facesNormals[offset++] = n.z;
	}
	
	memset(vertexFaces, 0,  model->GetVertexCount() * 2);

	CVector3D* vertexNormals = (CVector3D*)vn;

	for (x = 0; x < model->GetVertexCount(); x++)
	{
		vertexNormals[x].x = 0;
		vertexNormals[x].y = 0;
		vertexNormals[x].z = 0;
	}

	for (x = 0; x < model->GetFaceCount(); x++)
	{		
		int ofs = x * 3;
		for (int j = 0; j < 3; j++)
		{
			unsigned short k = fs[ofs + j];
			if (k < maxOffset)
			{
				vertexFaces[k]++;
				vertexNormals[k].x += m_facesNormals[ofs];
				vertexNormals[k].y += m_facesNormals[ofs + 1];
				vertexNormals[k].z += m_facesNormals[ofs + 2];
			}
		}
	}	

	for (x = 0; x < model->GetVertexCount(); x++)
	{
		vertexNormals[x] /= vertexFaces[x];
		vertexNormals[x].Normalize();
	}	

	FreeAll();
}

void CBaseModelLoader::FreeAll()
{
	if (m_facesNormals)
	{
		MP_DELETE_ARR(m_facesNormals);
		m_facesNormals = NULL;
	}
}

unsigned int CBaseModelLoader::GetCalculateTime()
{
	return m_calculateNormalsTime;
}

CBaseModelLoader::~CBaseModelLoader()
{
	FreeAll();
}