
#include "StdAfx.h"
#include <Assert.h>
#include "Box.h"
#include "cal3d/memory_leak.h"

#define PHYSICS_BOX_DENSITY		35.0

CBox::CBox()
{
}

bool CBox::Create(dBodyID& body, dGeomID& geometry, dWorldID* world, dSpaceID* space)
{
	assert(world);
	assert(space);

	body = dBodyCreate(*world);

	float x, y, z;
	GetInitialCoords(x, y, z);
	dBodySetPosition(body, x, y, z); 

	float rx, ry, rz, rw;
	GetInitialDimensions(rx, ry, rz, rw);
	if (rx <= 0) rx = 0.01f;
	if (ry <= 0) ry = 0.01f;
	if (rz <= 0) rz = 0.01f;
	/*rx /= 4.0f;
	ry /= 4.0f;
	rz /= 4.0f;*/

	dMassSetZero(&m_mass);
	dMassSetBox(&m_mass, PHYSICS_BOX_DENSITY, rx, ry, rz);
	dBodySetMass(body, &m_mass);			

	geometry = dCreateBox(*space, rx, ry, rz);
	dGeomSetBody(geometry, body);

	return true;
}

void CBox::SetNewMass(float /*newDensity*/)
{
}

void CBox::Destroy()
{
	MP_DELETE_THIS;
}

CBox::~CBox()
{
}