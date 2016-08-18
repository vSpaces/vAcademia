
#pragma once

#include "BasePhysicsPrimitive.h"
#include "PhysicsPrimitiveTypes.h"

class CStaticSphere : public CBasePhysicsPrimitive
{
public:
	CStaticSphere();
	~CStaticSphere();

	__forceinline int GetType()
	{
		return PHYSICS_OBJECT_TYPE_STATIC_SPHERE;
	}

	bool Create(dBodyID& body, dGeomID& geometry, dWorldID* world, dSpaceID* space);
	void SetNewMass(float newDensity);

	void Destroy();
};