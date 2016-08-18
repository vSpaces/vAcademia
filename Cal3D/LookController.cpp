
#include "StdAfx.h"
#include "LookController.h"
#include "model.h"
#include "corebone.h"
#include "coremodel.h"
#include "coretrack.h"
#include "bone.h"
#include "coreskeleton.h"
#include "animation.h"
#include "skeleton.h"
#include <D3d8.h>
#include "memory_leak.h"

#define MAX_EYE_ANGLE	(20.0f / 57.0f)
#define	MAX_PLANE_ANGLE	45.0f*3.14f/180.0f
#define	MAX_UP_ANGLE	2.0f*3.14f/180.0f
#define	MAX_DOWN_ANGLE	2.0f*3.14f/180.0f
#define	LOOK_VEL_K		0.01f
#define LOOK_TIME		1000.0f

#define QUATSAREEQ(Q,E)	(Q.x == E.x && Q.y == E.y && Q.z == E.z && Q.w == E.w)

float GetAngleTime(float angle)
{
	return 0.63f * sqrt(angle > 0 ? angle : -angle);		// 0.75 sec per 80 degrees
}

CalLookController::CalLookController(CalModel* pModel):
	m_model(pModel),
	m_headBone(NULL),
	m_neckBone(NULL),
	m_lastAngle(0),
	m_lastTime(0),
	m_isEnabled(false),
	m_leftEyePoseID(-1),
	m_leftEyeBone(NULL),
	m_rightEyePoseID(-1),
	m_rightEyeBone(NULL),
	m_neckBoneID(Cal::NO_SUCH_BONE_ID),
	m_headBoneID(Cal::NO_SUCH_BONE_ID),
	m_timeOut(1),
	m_lastHeadRotation(0.0f, 0.0f, 0.0f, 1.0f)
{
}

bool CalLookController::IsEnabled()
{
	return m_isEnabled;
}

void CalLookController::SetEyePoses(int motionID1, int motionID2)
{
	m_leftEyePoseID = motionID1;
	m_rightEyePoseID = motionID2;
}

void CalLookController::SetRotations(float * rotation, float * translation)
{
	const unsigned int maxTimeOut = 15;
	m_timeOut = maxTimeOut;
#define M_PI 3.14159265f
	const float maxPitch = 45.0f;//20
	const float maxRoll = 60.0f;
	const float maxYaw = 45.0f;
	m_rotation[0] = (rotation[0] > maxPitch) ? maxPitch : ((rotation[0] < -maxPitch) ? -maxPitch : rotation[0]);	
	m_rotation[1] = (rotation[1] > maxYaw) ? maxYaw : ((rotation[1] < -maxYaw) ? -maxYaw : rotation[1]);	
	m_rotation[2] = (rotation[2] > maxRoll) ? maxRoll : ((rotation[2] < -maxRoll) ? -maxRoll : rotation[2]);	
	for(unsigned int i = 0; i < 3; ++i)
	{
		m_rotation[i] *= M_PI/ 360.0f; //half angle in radians
	}
	memcpy(m_translation, translation, sizeof(m_translation));
}

CalQuaternion QuaternionDiff(CalQuaternion const& q1, CalQuaternion const& q2)
{
	return CalQuaternion(q1.x - q2.x, q1.y - q2.y, q1.z - q2.z, q1.w - q2.w);
}

void CalLookController::Update()
{
	if (!m_isEnabled)	
	{
		return;
	}

	--m_timeOut;
	if(m_timeOut == 0)
	{
		memset(m_rotation, 0, sizeof(m_rotation));
		memset(m_translation, 0, sizeof(m_translation));
	}

	m_headBone->calculateState();

	CalVector translation(-m_translation[2], -m_translation[0], m_translation[1]);

	//преобразуем углы эйлера, полученные с кинекта в квантерион
	CalQuaternion q1(sin(-m_rotation[1]), 0.0f, 0.0f, cos(-m_rotation[1]));
	CalQuaternion q2(0.0f, sin(m_rotation[2]), 0.0f, cos(m_rotation[2]));
	CalQuaternion q3(0.0f, 0.0f, sin(m_rotation[0]), cos(m_rotation[0]));
	CalQuaternion rotation(q1 * q2 * q3);

	rotation.blend(0.5f, m_lastHeadRotation);
	rotation.Normalize();

	m_lastHeadRotation = rotation;

	//установить повороты как в методе ниже
	m_headBone->setRotation(rotation);
	//m_headBone->setTranslation(translation);
	m_headBone->calculateState();
}

/*void CalLookController::Update(float delay)
{

	if (!m_isEnabled)	
	{
		return;
	}

	m_params.delay = INT_MIN;

	m_params.delay += (int)(delay * 1000);
	if ((m_state == CalAnimation::STATE_STEADY) && (m_params.time != -1) && (m_params.delay > m_params.time))
	{
		m_state = CalAnimation::STATE_OUT;
		m_params.delay = (int)(delay * 1000);
	}

	CalVector			translation;
	CalQuaternion		rotation;

	// Второй кадр
	CalQuaternion		headRotation;
	CalQuaternion		deltaRotation;
	CalVector			lookTranslation;
	CalVector			headDirection;
	CalVector			normal;

	CalVector	headTranslation = m_headBone->getTranslationAbsolute();
	headTranslation += m_neckBone->getTranslation()/2;
	lookTranslation = m_target;

	// вычислим вектор направления взгляда
	CalQuaternion	current_head_rotation = m_headBone->getRotation();
	bool	actions_on_head = QUATSAREEQ(current_head_rotation,m_lastHeadRotation)?
		false:true;
	actions_on_head = true;

	// восстановим предыдущий вектор поворота
//	m_headBone->setRotation( m_lastHeadRotation);
	m_headBone->calculateState();

	// сделаем проекцию на плоскость
	CalVector	base_plane_vec = m_model->getSkeleton()->getDirection(m_neckBoneID);
	base_plane_vec.z = 0;
	CalVector	plane_normal = m_model->getSkeleton()->getDirection(m_neckBoneID, CalVector(1,0,0));	
	plane_normal = CalVector(0, 0, 1);
	plane_normal.normalize();
	float		plane_dist = plane_normal*headTranslation;		// поскость для шеи
	float		pp_dist = plane_normal * lookTranslation - plane_dist;	
	CalVector	ip = lookTranslation - plane_normal * pp_dist;		// проекция точки наблюдения на данную плоскость
	CalVector	plane_vec = ip - headTranslation;					//	вектор наблюдения в данной плоскости

	int stop_count = 0;

	//рассчитаем проекцию на плоскость от текущего направления головы
	CalVector	head_vec = m_model->getSkeleton()->getDirection(m_headBoneID) + headTranslation;
	float		head_dist = plane_normal*head_vec - plane_dist;	
	CalVector	head_proj = head_vec - plane_normal*head_dist;		// проекция точки наблюдения на данную плоскость
	head_proj -= headTranslation;
	float req_plane_angle = head_proj.getAngle( plane_vec);
	if (req_plane_angle > MAX_PLANE_ANGLE)
	{
		req_plane_angle = MAX_PLANE_ANGLE;
	}
	else if (req_plane_angle < -MAX_PLANE_ANGLE)
	{
		req_plane_angle = -MAX_PLANE_ANGLE;
	}
	float req_plane_time = GetAngleTime(req_plane_angle);
	if( req_plane_time<delay && m_state==CalAnimation::STATE_STEADY)
	{
		if( m_params.delay < 0)
			m_params.delay = 0;
	}
	float max_plane_time_angle = req_plane_time==0?0:delay/req_plane_time * 1.0;//LOOK_VEL_K;
	//float req_normal_angle = head_proj.getAngle( m_model->getSkeleton()->getDirection(m_headBoneID));

	// переведем plane_normal в локальную систему координат шеи
	//
	float	pl_a = base_plane_vec.getAngle(plane_vec);
	float maxAngle = pl_a;
	if( pl_a > MAX_PLANE_ANGLE)	pl_a = MAX_PLANE_ANGLE;
	else	if( pl_a < -MAX_PLANE_ANGLE)	pl_a = -MAX_PLANE_ANGLE;

	float direction = base_plane_vec.getNormal(plane_vec) * plane_normal;
	//CalQuaternion	rt_d( CalVector(direction>0?-1:1, 0, 0), pl_a);	
	CalQuaternion	rt_d(CalVector(0, 0, 1), 0.3);
	CalQuaternion	pr_q = m_neckBone->getRotationAbsolute();
	rt_d *= pr_q;	// глобальный поворот в плоскости
	rt_d.Normalize();
	// теперь подинимем ему голову
	CalVector	v_n = plane_vec.getNormal( lookTranslation-headTranslation);
	float		v_a = plane_vec.getAngle( lookTranslation-headTranslation);
	if( v_a > MAX_UP_ANGLE)	v_a = MAX_UP_ANGLE;
	else	if( v_a < -MAX_UP_ANGLE)	v_a = -MAX_UP_ANGLE;

	CalQuaternion v_q(v_n, -v_a);
	//rt_d *= v_q;
	pr_q.conjugate();	
	rt_d *= pr_q;		// глобальный поворот
	CalQuaternion	cur_rtn = m_headBone->getRotation();

	if (max_plane_time_angle > 1.0f) 
	{
		max_plane_time_angle = 1.0f;
	}

	cur_rtn.blend( max_plane_time_angle, rt_d);
	cur_rtn = rt_d;
	cur_rtn.Normalize();
//	cur_rtn = rt_d;	// DEBUG for immediate rotations
	//
	if( m_state==CalAnimation::STATE_OUT)
	{
		if(m_params.delay >= 1000)
		{
			m_params.delay = 1000;
			stop_count++;
		}
		else	stop_count = 0;
		if( !actions_on_head)
			cur_rtn.blend(m_params.delay/1000.0f, m_startLookRotation);
		else
			cur_rtn.blend(m_params.delay/1000.0f, current_head_rotation);
	}
	m_lastHeadRotation = cur_rtn;

	//m_headBone->setRotation( cur_rtn);

	float angle = pl_a * (direction>0?-1.0f:1.0f);
	if (0 == m_lastTime)
	{
		m_lastTime = GetTickCount();
	}
	DWORD delta = m_lastTime;
	DWORD currentTime = GetTickCount();
	delta = currentTime - m_lastTime;
	float maxDeltaAngle = (float)delta / LOOK_TIME;
	float deltaAngle = angle - m_lastAngle;
	if (fabsf(deltaAngle) > fabsf(maxDeltaAngle))
	{
		deltaAngle = deltaAngle * maxDeltaAngle / fabsf(deltaAngle);
		angle = m_lastAngle + deltaAngle;
	}

	m_lastAngle = angle;
	m_lastTime = currentTime;

	CalQuaternion q = m_headRotation;
	// CalVector(0, 1, 0) - наклон вбок
	q *= CalQuaternion( CalVector(1, 0, 0), angle);

	m_headBone->setRotation(q);
	m_headBone->calculateState();

	float eyeAngle = maxAngle - pl_a;
	
	if ((m_leftEyePoseID != -1) && (m_rightEyePoseID != -1))
	{
		int eyePoseID = m_rightEyePoseID;
		if (direction > 0)
		{
			eyePoseID = m_leftEyePoseID;
		}

		float koef = eyeAngle / MAX_EYE_ANGLE;
		if (koef > 1.0f)
		{
			koef = 1.0;
		}
        
		CalCoreAnimation* anim = m_model->getCoreModel()->getCoreAnimation(eyePoseID);

		if (!anim)
		{
			return;
		}

		std::list<CalCoreTrack *>&	tracks = anim->getListCoreTrack();

		std::list<CalCoreTrack *>::iterator	it = tracks.begin(),
										end = tracks.end();
		for ( ; it != end; it++)
		if ((*it)->getCoreBoneId() == m_leftEyeBoneID)
		{
			CalVector translation;
			CalQuaternion rotation;
			(*it)->getState(0, 1000, translation, rotation);		

			CalQuaternion r = m_startLeftEyeRotation;
			r.blend(koef, rotation);

			CalVector t = m_startLeftEyeTranslation;
			t.blend(koef, translation);

			m_leftEyeBone->setTranslation(t);
			m_leftEyeBone->setRotation(r);
			m_leftEyeBone->calculateState();
		}
		else if ((*it)->getCoreBoneId() == m_rightEyeBoneID)
		{
			CalVector translation;
			CalQuaternion rotation;
			(*it)->getState(0, 1000, translation, rotation);		

			CalQuaternion r = m_startRightEyeRotation;
			r.blend(koef, rotation);

			CalVector t = m_startRightEyeTranslation;
			t.blend(koef, translation);

			m_rightEyeBone->setTranslation(t);
			m_rightEyeBone->setRotation(r);
			m_rightEyeBone->calculateState();
		}
	}
	
	/*if (stop_count == 1)
	{
		Enable(false);
	}*/

	//return;
//}

void CalLookController::SetTarget(CalVector target)
{
	m_target = target;
}

bool CalLookController::Init(LOOKATPARAMS params)
{
	m_headBoneID = Cal::NO_SUCH_BONE_ID;
	m_neckBoneID = Cal::NO_SUCH_BONE_ID;
	m_leftEyeBoneID = -1;
	m_rightEyeBoneID = -1;

	if (!m_model->getSkeleton())
	{
		return false;
	}

	if (m_model->getSkeleton()->getCoreSkeleton()->containBone("Neck"))
	{
		m_neckBoneID = m_model->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName("Neck");
	}

	if (m_model->getSkeleton()->getCoreSkeleton()->containBone("neck"))
	{
		m_neckBoneID = m_model->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName("neck");
	}

	if (m_model->getSkeleton()->getCoreSkeleton()->containBone("Head"))
	{
		m_headBoneID = m_model->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName("Head");
	}

	if (m_model->getSkeleton()->getCoreSkeleton()->containBone("head"))
	{
		m_headBoneID = m_model->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName("head");
	}

	if (m_model->getSkeleton()->getCoreSkeleton()->containBone("BoneEyeCenterLeft"))
	{
		m_leftEyeBoneID = m_model->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName( "BoneEyeCenterLeft");
	}

	if (m_model->getSkeleton()->getCoreSkeleton()->containBone("BoneEyeCenterRight"))
	{
		m_rightEyeBoneID = m_model->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName( "BoneEyeCenterRight");
	}

	if ((m_headBoneID == Cal::NO_SUCH_BONE_ID) || (m_neckBoneID == Cal::NO_SUCH_BONE_ID))
	{
		return false;
	}

	m_headBone = m_model->getSkeleton()->getBone(m_headBoneID);
	m_neckBone = m_model->getSkeleton()->getBone(m_neckBoneID);

	if (m_headBone != NULL)
	{
		m_startLookRotation = m_headBone->getRotation();
	}

	if (m_leftEyeBoneID != -1)
	{
		m_leftEyeBone = m_model->getSkeleton()->getBone(m_leftEyeBoneID);
		m_startLeftEyeTranslation = m_leftEyeBone->getCoreBone()->getTranslation();
		m_startLeftEyeRotation = m_leftEyeBone->getCoreBone()->getRotation();
	}

	if (m_rightEyeBoneID != -1)
	{
		m_rightEyeBone = m_model->getSkeleton()->getBone(m_rightEyeBoneID);
		m_startRightEyeTranslation = m_rightEyeBone->getCoreBone()->getTranslation();
		m_startRightEyeRotation = m_rightEyeBone->getCoreBone()->getRotation();
	}

	m_neckRotation = m_neckBone->getCoreBone()->getRotation();
	m_headRotation = m_headBone->getCoreBone()->getRotation();

	m_params = params;
	Enable(true);

	return true;
}

void CalLookController::Enable(bool ab_Enable)
{
	if ((!m_isEnabled) && (ab_Enable))
	{
		m_lastTime = GetTickCount();
	}
	m_isEnabled = ab_Enable;
	if (m_headBone)
	{
		m_lastHeadRotation = m_headBone->getRotation();
	}
	else
	{
		m_isEnabled = false;
	}
	m_params.delay = 1000;
	m_state = CalAnimation::STATE_STEADY;
}

CalLookController::~CalLookController()
{
}