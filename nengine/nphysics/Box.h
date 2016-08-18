#pragma once

#include "BasePhysicsPrimitive.h"
#include "PhysicsPrimitiveTypes.h"

class CBox : public CBasePhysicsPrimitive
{
public:
	CBox();
	~CBox();

	__forceinline int GetType()
	{
		return PHYSICS_OBJECT_TYPE_BOX;
	}

	bool Create(dBodyID& body, dGeomID& geometry, dWorldID* world, dSpaceID* space);
	void SetNewMass(float newDensity);

	void Destroy();

private:
	dMass m_mass;
};