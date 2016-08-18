
#pragma once

#pragma once

#include "RayTraceTrimesh.h"
#include "TriMeshConstructData.h"
#include "BasePhysicsPrimitive.h"
#include "PhysicsPrimitiveTypes.h"

class CStaticCollisionMesh : public CBasePhysicsPrimitive
{
public:
	CStaticCollisionMesh();
	~CStaticCollisionMesh();

	__forceinline int GetType()
	{
		return PHYSICS_OBJECT_TYPE_STATIC_COLLISION_MESH;
	}

	bool Create(dBodyID& body, dGeomID& geometry, dWorldID* world, dSpaceID* space);
	void SetNewMass(float newDensity);	

	void Destroy();

private:
	CRayTraceTriMesh* m_triMesh;
};