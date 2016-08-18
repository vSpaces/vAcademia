
#include "StdAfx.h"
#include "SkeletonAttachTest.h"


CSkeletonAttachTest::CSkeletonAttachTest()
{
	parent = new C3DObject();
	child = new C3DObject();
}

CSkeletonAttachTest::~CSkeletonAttachTest()
{

}

void CSkeletonAttachTest::Execute()
{
	TestCalculateChildCoords();
}

void CSkeletonAttachTest::TestCalculateChildCoords()
{
	CVector3D childCoords;

	child->AttachTo( parent, "a");

	child->SetLocalCoords( 1, 1, 0);
	childCoords = child->GetCoords();	// 1.5, 1.5, 0
}