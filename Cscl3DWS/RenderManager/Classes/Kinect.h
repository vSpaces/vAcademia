#pragma once

#include "IKinect.h"

class CKinect
{
public:
	CKinect();
	~CKinect();

	IKinect* GetKinectObject(C3DObject* mainObject);

private:
	IKinect* m_kinect;
};