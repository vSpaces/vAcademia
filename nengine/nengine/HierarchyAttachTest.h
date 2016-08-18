#pragma once

#include "CommonEngineHeader.h"
#include "HierarchyAttach.h"

class CHierarchyAttachTest
{
public:
	CHierarchyAttachTest();
	virtual ~CHierarchyAttachTest();

	void Execute();
	
private:
	void TestCalculateChildCoords();
	
	C3DObject*	parent;
	C3DObject*	child;
	CHierarchyAttach* attach;
};