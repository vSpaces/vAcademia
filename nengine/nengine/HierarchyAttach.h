#pragma once

#include "CommonEngineHeader.h"
#include "IAttachParams.h"

class CHierarchyAttach : public CBaseAttachParams
{
public:
	CHierarchyAttach( C3DObject *aParent, C3DObject *aChild);
	virtual ~CHierarchyAttach();

	CVector3D	PositionAbsoluteToChild( const CVector3D& aPosition);

	CQuaternion RotationAbsoluteToChild( const CQuaternion& aRotation);

private:
	void CalculateChildCoords( CVector3D& aPosition);
	void CalculateChildRotation( CQuaternion& aRotation);
	
};