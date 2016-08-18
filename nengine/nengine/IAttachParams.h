#pragma once

#ifdef	_WIN32
	#pragma	warning (disable:4786)
#endif

#include "CommonEngineHeader.h"

class C3DObject;

struct IAttachParams
{
	virtual C3DObject*	GetChild() = 0;
	virtual C3DObject*	GetParent() = 0;

	virtual const CVector3D&	GetAbsChildCoords( ) = 0;
	virtual const CQuaternion&	GetAbsChildRotation( ) = 0;
};

class CBaseAttachParams : IAttachParams
{
public:
	CBaseAttachParams( C3DObject *aParent, C3DObject *aChild)
	{
		child = aChild;
		parent = aParent;
	}

	virtual ~CBaseAttachParams( )
	{

	}

	C3DObject*	GetChild()
	{
		return child;
	}

	C3DObject*	GetParent()
	{
		return parent;
	}

	const CVector3D&		GetAbsChildCoords( )
	{
		CalculateChildCoords( position);
		return position;
	}

	const CQuaternion&	GetAbsChildRotation( )
	{
		CalculateChildRotation( rotation);
		return rotation;
	}

	virtual CVector3D	PositionAbsoluteToChild( const CVector3D& aPosition) = 0;

	virtual CQuaternion RotationAbsoluteToChild( const CQuaternion& aRotation) = 0;

private:
	virtual void CalculateChildCoords( CVector3D& aPosition) = 0;
	virtual void CalculateChildRotation( CQuaternion& aRotation) = 0;

private: 
	C3DObject		*child;
	C3DObject		*parent;
	CVector3D		position;
	CQuaternion		rotation;
};
