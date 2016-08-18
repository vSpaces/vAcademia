
#include "StdAfx.h"
#include <Assert.h>
#include "StaticCylinder.h"
#include "cal3d/memory_leak.h"

#define PHYSICS_STATIC_BOX_DENSITY		35.0

CStaticCylinder::CStaticCylinder()
{
}

bool CStaticCylinder::Create(dBodyID& /*body*/, dGeomID& /*geometry*/, dWorldID* /*world*/, dSpaceID* /*space*/)
{
	//assert(world);
	//assert(space);

	// to do

	return true;
}

void CStaticCylinder::SetNewMass(float /*newDensity*/)
{
}

void CStaticCylinder::Destroy()
{
	MP_DELETE_THIS;
}

CStaticCylinder::~CStaticCylinder()
{
}