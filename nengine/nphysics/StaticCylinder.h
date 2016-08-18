#pragma once

#include "BasePhysicsPrimitive.h"
#include "PhysicsPrimitiveTypes.h"

class CStaticCylinder : public CBasePhysicsPrimitive
{
public:
	CStaticCylinder();
	~CStaticCylinder();

	__forceinline int GetType()
	{
		return PHYSICS_OBJECT_TYPE_STATIC_CYLINDER;
	}

	bool Create(dBodyID& body, dGeomID& geometry, dWorldID* world, dSpaceID* space);
	void SetNewMass(float newDensity);

	void Destroy();
};