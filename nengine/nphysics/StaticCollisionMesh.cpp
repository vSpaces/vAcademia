
#include "StdAfx.h"
#include <Assert.h>
#include "StaticCollisionMesh.h"
#include "cal3d/memory_leak.h"

#define VERY_SMALL_SIZE		0.01f

CStaticCollisionMesh::CStaticCollisionMesh():
	m_triMesh(NULL)
{
}

bool CStaticCollisionMesh::Create(dBodyID& body, dGeomID& geometry, dWorldID* /*world*/, dSpaceID* space)
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

	dGeomDisable(	geometry);

	if (m_triMesh)
	{
		MP_DELETE(m_triMesh);
	}

	m_triMesh = MP_NEW(CRayTraceTriMesh);
	m_triMesh->SetModel(trimeshData);	

	SetUserData((void *)m_triMesh);

	return true;
}

void CStaticCollisionMesh::SetNewMass(float /*newDensity*/)
{
	// nothing to do
}

void CStaticCollisionMesh::Destroy()
{
	MP_DELETE_THIS;
}

CStaticCollisionMesh::~CStaticCollisionMesh()
{
	if (m_triMesh)
	{
		MP_DELETE(m_triMesh);	
	}
}