
#include "StdAfx.h"
#include "Box.h"
#include "Sphere.h"
#include "StaticBox.h"
#include "StaticSphere.h"
#include "StaticTrimesh.h"
#include "StaticCylinder.h"
#include "StaticCollisionMesh.h"
#include "BasePhysicsPrimitive.h"
#include "cal3d/memory_leak.h"

CBasePhysicsPrimitive* CreatePhysicsPrimitive(int type)
{
	switch (type)
	{
	case PHYSICS_OBJECT_TYPE_NONE:
		return NULL;

	case PHYSICS_OBJECT_TYPE_BOX:
		return MP_NEW(CBox);

	case PHYSICS_OBJECT_TYPE_SPHERE:
		return MP_NEW(CSphere);

	case PHYSICS_OBJECT_TYPE_CYLINDER:
		return NULL;

	case PHYSICS_OBJECT_TYPE_STATIC_PLANE:
		return NULL;

	case PHYSICS_OBJECT_TYPE_STATIC_BOX:
		return MP_NEW(CStaticBox);

	case PHYSICS_OBJECT_TYPE_STATIC_SPHERE:
		return MP_NEW(CStaticSphere);

	case PHYSICS_OBJECT_TYPE_STATIC_CYLINDER:
		return MP_NEW(CStaticCylinder);

	case PHYSICS_OBJECT_TYPE_STATIC_TRIMESH:
		return MP_NEW(CStaticTrimesh);

	case PHYSICS_OBJECT_TYPE_STATIC_COLLISION_MESH:
		return MP_NEW(CStaticCollisionMesh);
	}

	return NULL;
}