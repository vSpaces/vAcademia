
#include "StdAfx.h"
#include <Assert.h>
#include "Sphere.h"
#include "cal3d/memory_leak.h"

#define PHYSICS_STATIC_BOX_DENSITY		35.0

CSphere::CSphere()
{
}

bool CSphere::Create(dBodyID& /*body*/, dGeomID& /*geometry*/, dWorldID* /*world*/, dSpaceID* /*space*/)
{
	//assert(world);
	//assert(space);



	return true;
}

void CSphere::SetNewMass(float /*newDensity*/)
{
}

void CSphere::Destroy()
{
	MP_DELETE_THIS;
}

CSphere::~CSphere()
{
}