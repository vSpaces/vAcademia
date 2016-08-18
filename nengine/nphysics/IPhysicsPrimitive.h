
#pragma once

class IPhysicsPrimitive
{
public:
	virtual int GetType() = 0;
	virtual bool Create(dBodyID& body, dGeomID& geometry, dWorldID* world, dSpaceID* space) = 0;
	virtual void SetInitialDimensions(float rx, float ry, float rz, float rw) = 0;
	virtual void SetInitialCoords(float x, float y, float z) = 0;
	virtual void SetUserData( void* value) = 0;
	virtual void SetNewMass(float newDensity) = 0;
	virtual void Destroy() = 0;
};