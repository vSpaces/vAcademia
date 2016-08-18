#pragma once

#include "vector3d.h"
#include "quaternion3d.h"

class Math3D
{
public:
	Math3D(void);
	~Math3D(void);

public:
	// Return position in child coordinate system, transformed from global coordinate system
	static CVector3D	WorldToObject( const CVector3D& aPosition
		, const CVector3D& aParentAbsolutePosition, const CQuaternion& aParentAbsoluteRotation);

	// Return global position, transformed from child coordinate system
	static CVector3D	ObjectToWorld( const CVector3D& aLocalChildPosition
		, const CVector3D& aParentAbsolutePosition, const CQuaternion& aParentAbsoluteRotation);

	// Return child coordinate system rotation, transformed from global rotation
	static CQuaternion	WorldToObject( const CQuaternion& aAbsoluteChildRotation
										, const CQuaternion& aParentAbsoluteRotation);

	// Return global rotation, transformed from child coordinate system
	static CQuaternion	ObjectToWorld( const CQuaternion& aLocalChildRotation
		, const CQuaternion& aParentAbsoluteRotation);

};
