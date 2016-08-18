
#pragma once

#include "bone.h"
#include "model.h"
#include "vector.h"
#include "skeleton.h"
#include "quaternion.h"

class CalBone;

class CAL3D_API CalPointController  
{
public:
	CalPointController(CalModel* pModel);
	~CalPointController();

	void SetPoint(CalVector& pnt);
	void SetCurrentObjectRotation(CalQuaternion& rtn);
	void Update();

private:
	CalModel*	m_model;

	CalBone*	m_armBone;
	CalBone*	m_forearmBone;

	CalVector	m_targetPoint;	
	CalQuaternion m_objectRotation;
};