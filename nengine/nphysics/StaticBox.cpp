
#include "StdAfx.h"
#include <Assert.h>
#include "StaticBox.h"
#include "cal3d/memory_leak.h"

#define PHYSICS_STATIC_BOX_DENSITY		35.0

CStaticBox::CStaticBox()
{
}

bool CStaticBox::Create(dBodyID& body, dGeomID& geometry, dWorldID* /*world*/, dSpaceID* space)
{
	assert(space);

	float rx, ry, rz, rw;
	GetInitialDimensions(rx, ry, rz, rw);
	if (rx <= 0) rx = 0.01f;
	if (ry <= 0) ry = 0.01f;
	if (rz <= 0) rz = 0.01f;

	geometry = dCreateBox(*space, rx, ry, rz);
	body = NULL;
	dGeomSetBody(geometry, body);			

	float x, y, z;
	GetInitialCoords(x, y, z);
	dGeomSetPosition(geometry, x, y, z);

	return true;
}

void CStaticBox::SetNewMass(float /*newDensity*/)
{
}

void CStaticBox::Destroy()
{
	MP_DELETE_THIS;
}

CStaticBox::~CStaticBox()
{
}