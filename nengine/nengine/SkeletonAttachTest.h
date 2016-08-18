#pragma once

#include "CommonEngineHeader.h"

class CSkeletonAttachTest
{
public:
	CSkeletonAttachTest();
	virtual ~CSkeletonAttachTest();

	void Execute();
	
private:
	void TestCalculateChildCoords();
	
	C3DObject*	parent;
	C3DObject*	child;
};