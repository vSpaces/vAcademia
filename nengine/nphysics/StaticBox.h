
#pragma once

#include "BasePhysicsPrimitive.h"
#include "PhysicsPrimitiveTypes.h"

class CStaticBox : public CBasePhysicsPrimitive
{
public:
	CStaticBox();
	~CStaticBox();

	__forceinline int GetType()
	{
		return PHYSICS_OBJECT_TYPE_STATIC_BOX;
	}

	bool Create(dBodyID& body, dGeomID& geometry, dWorldID* world, dSpaceID* space);
	void SetNewMass(float newDensity);

	void Destroy();
};