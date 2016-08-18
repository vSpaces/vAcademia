
#include "StdAfx.h"
#include <Assert.h>
#include "StaticSphere.h"
#include "cal3d/memory_leak.h"

CStaticSphere::CStaticSphere()
{
}

bool CStaticSphere::Create(dBodyID& body, dGeomID& geometry, dWorldID* /*world*/, dSpaceID* space)
{
	assert(space);
	
	float rx, ry, rz, rw;
	GetInitialDimensions(rx, ry, rz, rw);
	if (rx <= 0) rx = 0.01f;
	if (ry <= 0) ry = 0.01f;
	if (rz <= 0) rz = 0.01f;

	geometry = dCreateSphere(*space, rx * 0.5f);
	body = NULL;
	dGeomSetBody(geometry, body);			

	float x, y, z;
	GetInitialCoords(x, y, z);
	dGeomSetPosition(geometry, x, y, z);

	return true;
}

void CStaticSphere::SetNewMass(float /*newDensity*/)
{
}

void CStaticSphere::Destroy()
{
	MP_DELETE_THIS;
}

CStaticSphere::~CStaticSphere()
{
}