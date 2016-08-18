
#include "StdAfx.h"
#include "CoreSkeleton.h"
#include "PointController.h"
#include <mmsystem.h>

CalPointController::CalPointController(CalModel* model):
	m_model(model),
	m_armBone(NULL),
	m_forearmBone(NULL),
	m_targetPoint(0.0f, 0.0f, 0.0f),
	m_objectRotation(0.0f, 0.0f, 1.0f, 0.0f)
{
	if ((m_model->getSkeleton()) && (m_model->getSkeleton()->getCoreSkeleton()))
	{
		int boneID = m_model->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName("r upperarm");
		if (boneID != -1)
		{
			m_armBone = m_model->getSkeleton()->getBone(boneID);
		}

		boneID = m_model->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName("r forearm");
		if (boneID != -1)
		{
			m_forearmBone = m_model->getSkeleton()->getBone(boneID);
		}
	}
}

void CalPointController::SetPoint(CalVector& pnt)
{
	m_targetPoint = pnt;
}

void CalPointController::SetCurrentObjectRotation(CalQuaternion& rtn)
{
	m_objectRotation = rtn;
}

void CalPointController::Update()
{
	if (!m_model)
	{
		return;
	}

	if (!m_armBone)
	{
		return;
	}

	if (m_targetPoint == CalVector(0.0f, 0.0f, 0.0f))
	{
		return;
	}

	CalQuaternion frameRtn = m_model->getSkeleton()->getBone(0)->getRotation();

	CalVector realFaceVec(1.0f, 0.0f, 0.0f);
    if (frameRtn.w != 0.0f)
	{
		realFaceVec *= frameRtn;
	}
	realFaceVec *= m_objectRotation;
	realFaceVec = realFaceVec.normalized();


	CalVector firstTargetPoint(0, -1, 0);
	firstTargetPoint *= frameRtn;	
	firstTargetPoint *= m_objectRotation;
	firstTargetPoint = firstTargetPoint.normalized();

	CalVector armVec(0, -1, 0);
	armVec *= frameRtn;	
	armVec *= m_objectRotation;
	armVec = armVec.normalized();
			
	float minLen = 100.0f;
	CalVector minTestVec;
	CalQuaternion resultQ(0, 0, 0, 1);

	int i = 0;
	for (i = 0; i < 360; i++)
	{
		CalQuaternion newRotation(CalVector(0, 0, 1), (float)i * 2.0f * 3.14f / 360.0f); 
		m_armBone->setRotation(newRotation);
		m_armBone->calculateState();

		CalQuaternion testQ = m_armBone->getRotationAbsolute();
		CalVector testVec = CalVector(1, 0, 0) * testQ;					
		testVec *= frameRtn;
		testVec *= m_objectRotation;
		testVec = testVec.normalized();

		CalVector testVec2 = testVec - realFaceVec/*m_targetPoint*/;
		testVec2.z = 0;
		float len = testVec2.length();

		if (realFaceVec.dot(testVec) > 0)
		if ((realFaceVec.getAngle(testVec) < (3.14f * 30.0f / 180.0f)) || (armVec.dot(testVec) > 0))
		if (len < minLen)
		{
			minLen = len;
			minTestVec = testVec;

			resultQ = newRotation;
		}
	}

	m_armBone->setRotation(resultQ);
	m_armBone->calculateState();

	resultQ.set(0, 0, 0, 1);

	minLen = 100.0f;

	for (i = 0; i < 360; i++)
	{
		CalQuaternion newRotation(CalVector(0, 0, 1), (float)i * 2.0f * 3.14f / 360.0f); 
		m_armBone->setRotation(newRotation);
		m_armBone->calculateState();

		CalQuaternion testQ = m_armBone->getRotationAbsolute();
		CalVector testVec = CalVector(1, 0, 0) * testQ;					
		testVec *= frameRtn;
		testVec *= m_objectRotation;
		testVec = testVec.normalized();

		CalVector testVec2 = testVec - m_targetPoint;
		testVec2.z = 0;
		float len = testVec2.length();

		if (realFaceVec.dot(testVec) > 0)
		if ((realFaceVec.getAngle(testVec) < (3.14f * 70.0f / 180.0f)) || (armVec.dot(testVec) > 0))
		if (len < minLen)
		{
			minLen = len;
			minTestVec = testVec;

			resultQ = newRotation;
		}
	}

	int time = timeGetTime();
	float angle = time / 1000.0f;

	CalVector v(m_targetPoint.x, m_targetPoint.y, 0);
	//angle = CalVector(m_targetPoint.x, m_targetPoint.y, 0.0f).getAngle(v);
	//v.set(cosf(angle), sinf(angle), 0.0f);
	angle = v.getAngle(m_targetPoint);

	if (m_targetPoint.z > 0)
	{
		angle *= -1;
	}
	
	resultQ = resultQ * CalQuaternion(CalVector(0, 1, 0), angle);

	m_forearmBone->setRotation(/*CalQuaternion(0, 0, 0, 1)*/resultQ);
	m_forearmBone->calculateState();
}

CalPointController::~CalPointController()
{
}