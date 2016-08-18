#pragma once

#include "RayTraceTrimesh.h"
#include "TriMeshConstructData.h"
#include "BasePhysicsPrimitive.h"
#include "PhysicsPrimitiveTypes.h"

class CStaticTrimesh : public CBasePhysicsPrimitive
{
public:
	CStaticTrimesh();
	~CStaticTrimesh();

	__forceinline int GetType()
	{
		return PHYSICS_OBJECT_TYPE_STATIC_TRIMESH;
	}

	bool Create(dBodyID& body, dGeomID& geometry, dWorldID* world, dSpaceID* space);
	void SetNewMass(float newDensity);	

	void Destroy();

private:
	CRayTraceTriMesh* m_triMesh;
};