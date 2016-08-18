#include ".\math3d.h"

Math3D::Math3D(void)
{
	
}

Math3D::~Math3D(void)
{
}

// Return position in object coordinate system, transformed from global coordinate system
CVector3D	Math3D::WorldToObject( const CVector3D& aPosition
								 , const CVector3D& aParentAbsolutePosition, const CQuaternion& aParentAbsoluteRotation)
{
	CVector3D result = aPosition;

	CQuaternion parentRotationConjugate = aParentAbsoluteRotation;
	parentRotationConjugate.Conjugate();

	result -= aParentAbsolutePosition;
	result *= parentRotationConjugate;

	return result;
}

// Return global position, transformed from object coordinate system
CVector3D	Math3D::ObjectToWorld( const CVector3D& aLocalChildPosition
								 , const CVector3D& aParentAbsolutePosition, const CQuaternion& aParentAbsoluteRotation)
{
	CVector3D result = aLocalChildPosition;

	result *= aParentAbsoluteRotation;
	result += aParentAbsolutePosition;

	return result;
}

CQuaternion	Math3D::ObjectToWorld( const CQuaternion& aLocalChildRotation
										  , const CQuaternion& aParentAbsoluteRotation)
{
	CQuaternion result = aLocalChildRotation;
	result *= aParentAbsoluteRotation;
	return result;
}

// Return child coordinate system rotation, transformed from global rotation
CQuaternion	Math3D::WorldToObject( const CQuaternion& aAbsoluteChildRotation
								  , const CQuaternion& aParentAbsoluteRotation)
{
	CQuaternion result = aAbsoluteChildRotation;
	CQuaternion aParentAbsoluteRotationConjugate = aParentAbsoluteRotation;
	aParentAbsoluteRotationConjugate.Conjugate();
	result *= aParentAbsoluteRotation;
	return result;
}