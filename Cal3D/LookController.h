
#pragma once

#include "global.h"
#include "vector.h"
#include "quaternion.h"

class CalModel;

typedef struct _LOOKATPARAMS
{
	int			delay;
	int			time;
	float		trTime;
	
	_LOOKATPARAMS()
	{	delay = 0; 
		trTime = 0.0; 
		time=0;
	}

	void operator=(const _LOOKATPARAMS& lp)
	{	
		delay = lp.delay; 
		trTime = lp.trTime; 
		time = lp.time;
	}
} LOOKATPARAMS;

class CalBone;

class CAL3D_API CalLookController  
{
public:
	CalLookController(CalModel* pModel);
	virtual ~CalLookController();

	void Enable( bool ab_Enable);
	bool Init(LOOKATPARAMS aparams);
	void Update();

	void SetTarget(CalVector pTarget);
	void SetEyePoses(int motionID1, int motionID2);

	bool IsEnabled();

	CalVector	m_target;
	CalModel*	m_model;

	CalBone*	m_headBone;
	CalBone*	m_neckBone;
	CalBone*	m_leftEyeBone;
	CalBone*	m_rightEyeBone;

	int			m_leftEyePoseID;
	int			m_rightEyePoseID;

	int			m_neckBoneID;
	int			m_headBoneID;
	int			m_leftEyeBoneID, m_rightEyeBoneID;

	bool		m_isEnabled;

	DWORD		m_state;
	void SetRotations(float * rotation, float * translation);

private:
	CalQuaternion	m_lastHeadRotation;	
	CalQuaternion	m_startLookRotation;	

	CalQuaternion	m_neckRotation;
	CalQuaternion	m_headRotation;

	CalVector		m_startLeftEyeTranslation, m_startRightEyeTranslation;
	CalQuaternion	m_startLeftEyeRotation, m_startRightEyeRotation;

	LOOKATPARAMS	m_params;
	float m_lastAngle;
	DWORD m_lastTime;
	unsigned int m_timeOut;

	float m_rotation[3];
	float m_translation[3];
};
