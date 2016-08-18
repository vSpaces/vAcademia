#pragma once

#include "BasePhysicsPrimitive.h"
#include "PhysicsPrimitiveTypes.h"

class CSphere : public CBasePhysicsPrimitive
{
public:
	CSphere();
	~CSphere();

	__forceinline int GetType()
	{
		return PHYSICS_OBJECT_TYPE_SPHERE;
	}

	bool Create(dBodyID& body, dGeomID& geometry, dWorldID* world, dSpaceID* space);
	void SetNewMass(float newDensity);

	void Destroy();
};