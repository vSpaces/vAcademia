// LookController.cpp: implementation of the CalLookController class.
//
//////////////////////////////////////////////////////////////////////

#include "LookController.h"
#include "model.h"
#include "corebone.h"
#include "bone.h"
#include "coreskeleton.h"
#include "animation.h"
#include "skeleton.h"
#include <D3d8.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define	MAX_PLANE_ANGLE	80.0f*3.14f/180.0f
#define	MAX_UP_ANGLE	80.0f*3.14f/180.0f
#define	MAX_DOWN_ANGLE	45.0f*3.14f/180.0f
#define	LOOK_VEL_K		0.01f

float get_angle_time(float angle)
{
	//return 1.26*sqrt(angle>0?angle:-angle);	// 1.5 sec per 80 degrees
	//return 0.84*sqrt(angle>0?angle:-angle);	// 1 sec per 80 degrees
	return 0.63*sqrt(angle>0?angle:-angle);		// 0.75 sec per 80 degrees
}

CalLookController::CalLookController(CalModel* pModel)
{
	m_LookTarget = NULL;
	m_pModel = pModel;
	iNeckID			= -1;
	iHeadID			= -1;
	iLookAtHeadID	= -1;
	m_bEnabled		= false;
}

CalLookController::~CalLookController()
{

}

void CalLookController::Update(float delay)
{
/*//////////////////////////////////////////////////////////
// 1. Поворот по горизонтали и вертикали производится отдельно
// 2. Поворот по горизонтали делаем головой
// 3. Поворот по вертикали делаем головой
// 4. Все повороты выполняются выравненно с шеей
*///////////////////////////////////////////////////////////
	if( type==CalLookController::TT_MODEL && !m_LookTarget)	return;
	if( !m_bEnabled)	return;

	params.adwDelay += (int)(delay*1000);
	if(state==CalAnimation::STATE_STEADY && params.adwTime!=-1 && params.adwDelay>params.adwTime)
	{
		state = CalAnimation::STATE_OUT;
		params.adwDelay = (int)(delay*1000);
	}

	CalVector			translation;
	CalQuaternion		rotation;

	// Второй кадр
	CalQuaternion		headRotation;
	CalQuaternion		deltaRotation;
	CalVector			lookTranslation;
	CalVector			headDirection;
	CalVector			normal;

	CalVector	headTranslation = p_bHead->getTranslationAbsolute();
	headTranslation += p_bNeck->getTranslation()/2;

	// если смотрим не в камеру то вычисляем положение точки наблюдения
	if( type == CalLookController::TT_MODEL)
	{
		lookTranslation = p_bLook->getTranslationAbsolute();
		if (m_LookTarget->pmExternTransform != NULL)
			lookTranslation.transform((D3DXMATRIX*)m_LookTarget->pmExternTransform);
		//lookTranslation += m_LookTarget->vTranslation;
	}else{
		CalVector	m_pntTarget = pntTarget;
		if (m_pModel->pmExternTransform != NULL)
		{
			D3DXMATRIX	im;
			D3DXMatrixInverse(&im, NULL, (D3DXMATRIX*)m_pModel->pmExternTransform);
			m_pntTarget.transform(&im);
		}
		lookTranslation = m_pntTarget;
	}

	// Recalculate vectors to local coordinate system
	if (type==CalLookController::TT_MODEL && m_pModel->pmExternTransform != NULL)
	{
		D3DXMATRIX	im;
		D3DXMatrixInverse(&im, NULL, (D3DXMATRIX*)m_pModel->pmExternTransform);
		//headTranslation.transform(&im);
		lookTranslation.transform(&im);
	}

	// Calculate portion rotation
	if( params.m_LookPortion != 1.0 && p_bNeck)
	{
		CalQuaternion	absHeadRotation = start_look_rotation;
		absHeadRotation *= p_bNeck->getRotationAbsolute();
		CalVector	vLook(0, 1, 0);
		vLook *= absHeadRotation;
		CalVector	vNeedLook = lookTranslation - headTranslation;
		vLook *= vNeedLook.length();
		vLook.blend(params.m_LookPortion, vNeedLook);
		lookTranslation = headTranslation + vLook;
	}
#define QUATSAREEQ(Q,E)	(Q.x==E.x&&Q.y==E.y&&Q.z==E.z&&Q.w==E.w)
	// вычислим вектор направления взгляда
	/*CalQuaternion	mod_rtn = m_pModel->qRotation;
	lookTranslation	*= mod_rtn;*/
	CalQuaternion	current_head_rotation = p_bHead->getRotation();
	bool	actions_on_head = QUATSAREEQ(current_head_rotation,last_head_rotation)?
		false:true;

	// восстановим предыдущий вектор поворота
	p_bHead->setRotation( last_head_rotation);
	p_bHead->calculateState();

	// сделаем проекцию на плоскость
	CalVector	base_plane_vec = m_pModel->getSkeleton()->getDirection(iNeckID);
	CalVector	plane_normal = m_pModel->getSkeleton()->getDirection(iNeckID, CalVector(1,0,0));	plane_normal.normalize();
	float		plane_dist = plane_normal*headTranslation;		// поскость для шеи
	float		pp_dist = plane_normal*lookTranslation - plane_dist;	
	CalVector	ip = lookTranslation - plane_normal*pp_dist;		// проекция точки наблюдения на данную плоскость
	CalVector	plane_vec = ip - headTranslation;					//	вектор наблюдения в данной плоскости

	int stop_count = 0;

	//рассчитаем проекцию на плоскость от текущего направления головы
	CalVector	head_vec = m_pModel->getSkeleton()->getDirection(iHeadID) + headTranslation;
	float		head_dist = plane_normal*head_vec - plane_dist;	
	CalVector	head_proj = head_vec - plane_normal*head_dist;		// проекция точки наблюдения на данную плоскость
	head_proj -= headTranslation;
	float req_plane_angle = head_proj.getAngle( plane_vec);
	float req_plane_time = get_angle_time(req_plane_angle);
	if( req_plane_time<delay && state==CalAnimation::STATE_STEADY)
	{
		if( params.adwDelay < 0)
			params.adwDelay = 0;
	}
	float max_plane_time_angle = req_plane_time==0?0:delay/req_plane_time * 1.0;//LOOK_VEL_K;
	//float req_normal_angle = head_proj.getAngle( m_pModel->getSkeleton()->getDirection(iHeadID));

	// переведем plane_normal в локальную систему координат шеи
	//
	float	pl_a = base_plane_vec.getAngle(plane_vec);
	if( pl_a > MAX_PLANE_ANGLE)	pl_a = MAX_PLANE_ANGLE;
	else	if( pl_a < -MAX_PLANE_ANGLE)	pl_a = -MAX_PLANE_ANGLE;

	CalQuaternion	rt_d( CalVector(base_plane_vec.getNormal(plane_vec)*plane_normal>0?-1:1,0,0), pl_a);
	CalQuaternion	pr_q = p_bNeck->getRotationAbsolute();
	rt_d *= pr_q;	// глобальный поворот в плоскости
	// теперь подинимем ему голову
	CalVector	v_n = plane_vec.getNormal( lookTranslation-headTranslation);
	float		v_a = plane_vec.getAngle( lookTranslation-headTranslation);
	if( v_a > MAX_UP_ANGLE)	v_a = MAX_UP_ANGLE;
	else	if( v_a < -MAX_UP_ANGLE)	v_a = -MAX_UP_ANGLE;

	CalQuaternion v_q(v_n, -v_a);
	rt_d *= v_q;
	//
	pr_q.conjugate();	rt_d *= pr_q;		// глобальный поворот
	//
	CalQuaternion	cur_rtn = p_bHead->getRotation();

//char buf[255]; memset( &buf, 0, 255);
//sprintf( buf, "pl_a: %.2f, v_a: %.2f\n", pl_a, v_a);OutputDebugString( buf);

	if (max_plane_time_angle>1.0) max_plane_time_angle = 1.0;

	cur_rtn.blend( max_plane_time_angle, rt_d);
	cur_rtn.Normalize();
//	cur_rtn = rt_d;	// DEBUG for immediate rotations
	//
	if( state==CalAnimation::STATE_OUT)
	{
		if(params.adwDelay >= 1000)
		{
			params.adwDelay = 1000;
			stop_count++;
		}
		else	stop_count = 0;
		if( !actions_on_head)
			cur_rtn.blend(params.adwDelay/1000.0f, start_look_rotation);
		else
			cur_rtn.blend(params.adwDelay/1000.0f, current_head_rotation);
	}
	last_head_rotation = cur_rtn;

	p_bHead->setRotation( cur_rtn);
	p_bHead->calculateState();

	if (live_eyes)
	{
	/*	for (int i=0; i<1; i++) {
			CalBone* pEye = i==0?p_bLeftEye:p_bRightEye;
			//pEye->setRotation( last_ly_rotation);
			pEye->calculateState();

			if( i==0)
				pEye->setRotation(last_ly_rotation);
			else
				pEye->setRotation(last_ry_rotation);

			cur_rtn = pEye->getRotation();

			CalVector	eye_plane_normal = m_pModel->getSkeleton()->getDirection(iHeadID, CalVector(0,1,0));	plane_normal.normalize();
			CalVector	cur_dir = eye_plane_normal;

			CalVector	pos = pEye->getTranslationAbsolute();
			CalVector	need_dir = lookTranslation - pos;
			need_dir.normalize();

			// to local system
			CalQuaternion _c = p_bHead->getRotationAbsolute(), _y;


			CalVector	vn=cur_dir.getNormal(need_dir);
			vn *= pr_q;
			float al=cur_dir.getAngle(need_dir);
			CalQuaternion	nr(vn, -al);

			nr *= cur_rtn;

			//nr = cur_rtn;
			pEye->setRotation(nr);
			pEye->calculateState();
		}*/
	}

	if( stop_count==1)
		Enable(false);
	return;
}

void CalLookController::SetTarget(CalModel *pTarget)
{
	m_LookTarget = pTarget;
	type = CalLookController::TT_MODEL;
}

void CalLookController::SetTarget(CalVector pTarget)
{
	pntTarget = pTarget;
	type = CalLookController::TT_POINT;
}

bool CalLookController::Init(LOOKATPARAMS aparams)
{
	if( !aparams.m_pModel && !aparams.bToCamera)
	{
		params.adwTime = 1000;
		if( m_bEnabled && state==CalAnimation::STATE_STEADY)
		{
			params.adwDelay=params.adwTime;
		}
		return true;
	}


	iHeadID=-1;
	iNeckID=-1;
	iLookAtHeadID=-1;
	if( aparams.m_pModel != NULL)	SetTarget( aparams.m_pModel);
	if( aparams.bToCamera)			SetTarget( aparams.camPos);
	//
	if(m_pModel->getSkeleton()->getCoreSkeleton()->containBone("Neck"))
		iNeckID = m_pModel->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName( "Neck");
	if(m_pModel->getSkeleton()->getCoreSkeleton()->containBone("neck"))
		iNeckID = m_pModel->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName( "neck");

	if(m_pModel->getSkeleton()->getCoreSkeleton()->containBone("Head"))
		iHeadID = m_pModel->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName( "Head");
	if(m_pModel->getSkeleton()->getCoreSkeleton()->containBone("head"))
		iHeadID = m_pModel->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName( "head");

	if (live_eyes) {
		if(m_pModel->getSkeleton()->getCoreSkeleton()->containBone("Ponytail1"))
			iLeftEyeID = m_pModel->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName( "Ponytail1");
		if(m_pModel->getSkeleton()->getCoreSkeleton()->containBone("Ponytail2"))
			iRightEyeID = m_pModel->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName( "Ponytail2");
	}

		// -------------ВТОРОЙ СКЕЛЕТ---------------------------------------------
	if( m_LookTarget != NULL && m_LookTarget->getSkeleton()){
		if(m_LookTarget->getSkeleton()->getCoreSkeleton()->containBone("Neck"))
			iLookAtHeadID = m_LookTarget->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName( "Neck");
		if(m_LookTarget->getSkeleton()->getCoreSkeleton()->containBone("neck"))
			iLookAtHeadID = m_LookTarget->getSkeleton()->getCoreSkeleton()->getCoreBoneIdSubName( "neck");
		if( iLookAtHeadID == -1)
			iLookAtHeadID = 0;
	}

	if( (iHeadID==-1) || (iNeckID==-1) || (iLookAtHeadID==-1 && type==CalLookController::TT_MODEL))
		return false;

	if( iHeadID != -1) p_bHead = m_pModel->getSkeleton()->getBone( iHeadID);
	if( iNeckID != -1) p_bNeck = m_pModel->getSkeleton()->getBone( iNeckID);
	if( iLookAtHeadID != -1) p_bLook = m_LookTarget->getSkeleton()->getBone( iLookAtHeadID);
	if( p_bHead != NULL)
		start_look_rotation = p_bHead->getRotation();
	/*if( iLeftEyeID != -1)
	{
		p_bLeftEye = m_pModel->getSkeleton()->getBone( iLeftEyeID);
		if(!m_bEnabled)	last_ly_rotation = p_bLeftEye->getRotation();
	}
	if( iRightEyeID != -1)
	{
		p_bRightEye = m_pModel->getSkeleton()->getBone( iRightEyeID);
		if(!m_bEnabled)	last_ry_rotation = p_bRightEye->getRotation();
	}*/
	params = aparams;

	Enable(true);
	return true;
}

void CalLookController::Enable(bool ab_Enable)
{
	m_bEnabled = ab_Enable;
	last_head_rotation = p_bHead->getRotation();
	params.adwDelay = INT_MIN;
	state = CalAnimation::STATE_STEADY;
	/*if(!m_bEnabled)
	{
		last_ly_rotation = p_bLeftEye->getRotation();
		last_ry_rotation = p_bRightEye->getRotation();
	}*/
}
