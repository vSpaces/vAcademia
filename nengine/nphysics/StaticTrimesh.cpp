
#include "StdAfx.h"
#include <Assert.h>
#include "StaticTrimesh.h"
#include "cal3d/memory_leak.h"

#define VERY_SMALL_SIZE		0.01f

CStaticTrimesh::CStaticTrimesh():
	m_triMesh(NULL)
{
}

bool CStaticTrimesh::Create(dBodyID& body, dGeomID& geometry, dWorldID* /*world*/, dSpaceID* space)
{
	//assert(world);
	assert(space);

	CModel* trimeshData = (CModel*)GetUserData();
	assert(trimeshData);
	if (!trimeshData)
	{
		return false;
	}

	body = NULL;		
	geometry = dCreateBox(*space, VERY_SMALL_SIZE, VERY_SMALL_SIZE, VERY_SMALL_SIZE);
	dGeomSetBody(geometry, body);			
	dGeomSetPosition(geometry, 0, 0, 0);

	dGeomDisable(geometry);

	if (m_triMesh)
	{
		MP_DELETE(m_triMesh);
	}

	m_triMesh = MP_NEW(CRayTraceTriMesh);
	m_triMesh->SetModel(trimeshData);	

	SetUserData((void *)m_triMesh);

	return true;
}

void CStaticTrimesh::SetNewMass(float /*newDensity*/)
{
	// nothing to do
}

void CStaticTrimesh::Destroy()
{
	MP_DELETE_THIS;
}

CStaticTrimesh::~CStaticTrimesh()
{
	if (m_triMesh)
	{
		MP_DELETE(m_triMesh);	
	}
}