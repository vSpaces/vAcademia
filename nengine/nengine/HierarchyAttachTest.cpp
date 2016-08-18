
#include "StdAfx.h"
#include "HierarchyAttachTest.h"


CHierarchyAttachTest::CHierarchyAttachTest()
{
	parent = new C3DObject();
	child = new C3DObject();
}

CHierarchyAttachTest::~CHierarchyAttachTest()
{

}

void CHierarchyAttachTest::Execute()
{
	TestCalculateChildCoords();
}

void CHierarchyAttachTest::TestCalculateChildCoords()
{
	CVector3D childCoords;

	child->AttachTo( parent);

	child->SetLocalCoords( 1, 1, 1);
	childCoords = child->GetCoords();	// 1, 1, 1

	parent->SetLocalCoords( 1, 1, 1);
	childCoords = child->GetCoords();	// 2, 2, 2

	child->SetLocalCoords( 0, 1, 0);
	parent->SetLocalCoords( 1, 0, 0);
	CRotationPack* rotationPack = new CRotationPack();
	CQuaternion q(CVector3D(1, 0, 0), CVector3D(0, 1, 0));
	rotationPack->SetAsDXQuaternion(q.x, q.y, q.z, q.w);
	parent->SetRotation( rotationPack);
	childCoords = child->GetCoords();	// -1, 1, 0

	child->SetLocalCoords( 0, 0, 0);
	parent->SetLocalCoords( 1, 0, 0);
	child->SetCoords(0, 0, 0);
	childCoords = child->GetLocalCoords();	// 0, 1, 0
	childCoords = child->GetCoords();	// 0, 0, 0

	child->SetLocalCoords( 0, 0, 0);
	q.Clear();
	rotationPack->SetAsDXQuaternion(q.x, q.y, q.z, q.w);
	parent->SetRotation( rotationPack);
	parent->SetLocalCoords( 1, 0, 0);
	child->SetCoords(0, 0, 0);
	childCoords = child->GetLocalCoords();	// -1, 0, 0

	child->AttachTo( NULL);
}