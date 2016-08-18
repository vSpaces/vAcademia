
#include "StdAfx.h"
#include "HierarchyAttach.h"

/*
 Класс реализует обычную иерархию.

 Каждый объект реализует методы: 
	- GetLocalCoords()
	- GetCoords()
	-  GetLocalRotation()
	- GetRotation()

Координаты и поворот рассчитываются так:
 AbsoluteChildPosition = child.GetLocalCoords() * parent.GetRotation() + parent.GetCoords();
 AbsoluteChildRotation = child.GetLocalRotation() * parent.GetRotation();

*/

CHierarchyAttach::CHierarchyAttach( C3DObject *aParent, C3DObject *aChild) : CBaseAttachParams( aParent, aChild)
{

}

CHierarchyAttach::~CHierarchyAttach()
{

}

void CHierarchyAttach::CalculateChildCoords( CVector3D& aPosition)
{
	CVector3D localChildCoords;
	if (GetChild())
	{
		localChildCoords = GetChild()->GetLocalCoords( );
	}

	if (GetParent())
	{
		CVector3D absParentCoords = GetParent()->GetCoords( );
		CRotationPack* absParentRotation = GetParent()->GetRotation();

		localChildCoords *= absParentRotation->GetAsDXQuaternion();
		localChildCoords += absParentCoords;
	}

	aPosition = localChildCoords;
}

void CHierarchyAttach::CalculateChildRotation( CQuaternion& aRotation)
{
	CQuaternion parentRotation;
	if (GetParent())
	{
		GetParent()->GetRotation()->GetAsDXQuaternion( &parentRotation.x, &parentRotation.y, &parentRotation.z, &parentRotation.w);
	}

	if (GetChild())
	{
		aRotation = GetChild()->GetLocalRotation()->GetAsDXQuaternion();
		aRotation *= parentRotation;
	}
	else
	{
		aRotation = parentRotation;
	}
}

CVector3D	CHierarchyAttach::PositionAbsoluteToChild( const CVector3D& aPosition)
{
	CQuaternion parentRotation;
	if (GetParent())
	{
		GetParent()->GetRotation()->GetAsDXQuaternion( &parentRotation.x, &parentRotation.y, &parentRotation.z, &parentRotation.w);
	}
	parentRotation.Conjugate();

	CVector3D parentPosition;
	if (GetParent())
	{
		parentPosition = GetParent()->GetCoords( );
	}

	CVector3D result = aPosition;
	result -= parentPosition;
	result *= parentRotation;

	return result;
	
}

CQuaternion CHierarchyAttach::RotationAbsoluteToChild( const CQuaternion& /*aRotation*/)
{
	CQuaternion result;
	return result;
}