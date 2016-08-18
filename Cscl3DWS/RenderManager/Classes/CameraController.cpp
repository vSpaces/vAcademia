
#include "StdAfx.h"
#include <userdata.h>
#include "CameraController.h"
#include "CameraInLocation.h"

#include "GlobalSingletonStorage.h"
#include "RMContext.h"
#include "nrmCamera.h"
#include "PlatformDependent.h"

// МАГИЧЕСКИЕ ЧИСЛА
// сдвиг камеры над землей
#define ADD_RAYTRACE_Z						60.0f
// разница между вектором камеры и взгляда по умолчанию
#define DEFAULT_DELTA						-700.0f, 800.0f, 250.0f
// минимальное расстояние, на которое надо сдвинуть камеру, для инициации пересчета
#define MIN_CHANGE_DIST						5.0f
// максимальное количество итераций для попыток придвинуть камеру ближе к аватару
#define MAX_SEARCH_ITERATIONS				30
// минимальное нормальное расстояние до аватара
#define MIN_DISTANCE_TO_AVATAR				40.0f
// максимальное время, в течение которого камера может не пересчитываться
#define DELTA_RECHECK_TIME					15000
// максимальное расстояние от аватара до точки взгляда, считающееся правильным
#define MAX_DIST_FROM_TARGET_TO_LOOK_AT		10.0f
// максимальное расстояние от аватара до точки взгляда по z, считающееся правильным
#define MAX_DIST_FROM_TARGET_TO_LOOK_AT_Z	10.0f
// минимальное приближение/удаление камеры, которое считается успешным
#define MIN_DISTANCE_INCREASE				1.0f
// сдвиг точки куда смотрим от аватара (чтобы не получилась, что эта точка находится внутри коллизионной геометрии)
#define	SHIFT_LOOK_AT_FROM_TARGET			20.0f
// сдвиг точки глаз от аватара (чтобы победить неточности при расчете коллизий)
#define	SHIFT_EYE_FROM_TARGET				50.0f
// максимальное возможное отклонение от заданного пользователем расстояния глаза камеры до аватара, когда
// контроллер не пытается привести текущее расстояние к заданному
#define MAX_DIFF_WITH_LINK_DISTANCE			15.0f
// максимальная разница между z аватара и z земли в той же точке, чтобы быть уверенным
// что аватар стоит на земле
#define Z_AVATAR_EPS						3.0f

// сложная проверка, не столкнулась ли текущая камера с чем-нибудь
#define COLLISION_APPEARS ( ((gRM->cameraInLocation->CheckForIntersection(GetCameraDeltaLookAt(SHIFT_LOOK_AT_FROM_TARGET), __delta)) && (__delta.GetLengthSq() > 2500.0f) && (iterationCount < MAX_SEARCH_ITERATIONS)) || ((g->phom.CheckRayIntersect(GetCameraDeltaLookAt(SHIFT_LOOK_AT_FROM_TARGET), __delta)) && (iterationCount < MAX_SEARCH_ITERATIONS)))

// пол числа ПИ
#define HALF_PI								(3.1415926535f * 0.5f)
// домножитель для смещения при сидение на стуле в духе Second Life
#define SECOND_LIFE_OFFSET_MUL_KOEF			0.35f
// допуск, чтобы можно было вращать камерой, когда она уперлась в землю и не может опуститься
#define Z_UNDERGROUND_EPS					5.0f
// максимум отклонения точки взгляда от объекта взгляда при камере от 3 лица
#define MAX_LOOK_AT_EPS						5.0f

// если дефайн определен, то корректируем позицию камеры плавно,
// невзирая на то, что в часть моментов времени камера будет находиться внутри объектов
#define CAMERA_SLOW_RETURN_TO_NORMAL_POSITION	1
// скорость возврата камеры в нормальное положение
#define CAMERA_RETURN_SPEED_AVATAR_MOVED	5.0f
#define CAMERA_RETURN_SPEED					20.0f
#define CAMERA_RETURN_FPS					100.0f

// колесико мыши
// дефолтное удаление от аватара
#define DEFAULT_MAX_LINK_DISTANCE			200
#define MIN_MAX_LINK_DISTANCE				80.0f
#define MAX_MAX_LINK_DISTANCE				3000.0f
#define COMMON_WHEEL_SPEED_KOEF				0.2f
#define WHEEL_FROM_SPEED_KOEF				0.04f
#define WHEEL_TO_SPEED_KOEF					0.01f

#define DIRECTION_UP						1
#define DIRECTION_RIGHT						2
#define DIRECTION_DOWN						3
#define DIRECTION_LEFT						4

CCameraController::CCameraController():
	m_sittingCameraMode(SITTING_CAMERA_MODE_UNDEFINED),
	m_maxLinkDistance(DEFAULT_MAX_LINK_DISTANCE),
	m_isViewTargetMovedOnLastFrame(false),
	m_viewTargetAttachedTime(0),
	m_viewTargetParent(NULL),
	m_isFixedPosition(false),
	m_lastViewTarget(NULL),
	m_isAltPressed(false),
	m_isCtrlPressed(false),
	m_isShiftPressed(false),
	m_isZoomAroundSelectedPoint(false),
	m_isNeedUpdate(false),	
	m_cameraTrack(NULL),
	m_frameDeltaTime(0),
	m_lastUpdateTime(0),
	m_cameraTarget(NULL),
	m_isInHouse(false),
	m_camera(NULL),
	m_deltaZ(0.0f),
	m_lastX(0),
	m_lastY(0),
	m_oldMaxLinkDistance(0)
{
	gRM->cameraInLocation->AddChangesListener(this);	
	gRM->mouseCtrl->AddMouseListener(this);
}

bool CCameraController::IsActive()
{
	if (g->scm.GetActiveScene())
	{
		if (g->scm.GetActiveScene()->GetViewport())
		{
			if (g->scm.GetActiveScene()->GetViewport()->GetCamera())
			{
				if (m_camera != g->scm.GetActiveScene()->GetViewport()->GetCamera())
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{ 
		return false;
	}

	return true;
}

void CCameraController::OnLMouseDown(int x, int y)
{
	if (!IsFirstView())
	if (m_isAltPressed || m_isCtrlPressed)
	if (m_cameraTarget)
	{
		if (m_cameraTarget->GetObject())
		{
			m_lastViewTarget = m_cameraTarget->GetObject();
		}

		m_cameraTarget->SetViewPoint(g->scm.GetActiveScene()->GetViewport()->GetSelected3DCoords());

		CVector3D pos = m_camera->GetEyePosition();

		CVector3D delta(0.0f, 0.0f, 0.0f);
		if (m_cameraTarget)
		{
			delta = GetViewTargetPosition() - m_lastPos;
			m_lastPos = GetViewTargetPosition();
		}		

		CVector3D eye = m_camera->GetEyePosition();
		CVector3D at = m_camera->GetLookAt();

		CVector3D view = at - eye;

		m_oldMaxLinkDistance = GetMaxLinkDistance();
		SetMaxLinkDistance(view.GetLength());

		m_isZoomAroundSelectedPoint = true;
		m_lastX = x;
		m_lastY = y;

		m_camera->SetZOffset(GetOffsetZ());
		m_camera->SetEyePosition(m_camera->GetEyePosition() + delta);
		m_camera->SetLookAt(m_camera->GetLookAt() + delta);

		CVector3D __delta = GetCameraDelta(SHIFT_EYE_FROM_TARGET) + CVector3D(0, 0, GetOffsetZ());
		int iterationCount = 0;

		if (m_maxLinkDistance - GetDistanceFromCamera() > MAX_DIFF_WITH_LINK_DISTANCE)
		{
			m_cameraTarget->SetViewOffset(CVector3D(0, 0, 0));		
		}
		else if (COLLISION_APPEARS)
		{
			m_cameraTarget->SetViewOffset(CVector3D(0, 0, 0));		
		}
		else if (!CheckForCameraUnderground(true))
		{
			m_cameraTarget->SetViewOffset(CVector3D(0, 0, 0));		
		}
	}
}

void CCameraController::OnLMouseUp(int x, int y)
{
	if (m_isZoomAroundSelectedPoint)
	{
		m_isZoomAroundSelectedPoint = false;
		SetMaxLinkDistance(m_oldMaxLinkDistance);
	}
}

void CCameraController::OnMouseMove(int x, int y)
{
	if (m_isAltPressed  && m_isZoomAroundSelectedPoint)
	{
		int deltaX = m_lastX - x;
		int deltaY = m_lastY - y;

		m_lastX = x;
		m_lastY = y;

		if (m_isCtrlPressed && m_isShiftPressed)
		{
			if (deltaX > 0)
			{
				Move(DIRECTION_RIGHT, abs(deltaX));
			}
			else
			{
				Move(DIRECTION_LEFT, abs(deltaX));
			}

			if (deltaY < 0)
			{
				Move(DIRECTION_UP, abs(deltaY));
			}
			else
			{
				Move(DIRECTION_DOWN, abs(deltaY));
			}

			return;
		}

		CVector3D eye = m_camera->GetEyePosition();
		CVector3D at = m_camera->GetLookAt();

		CVector3D view = at - eye;

		CVector3D result = view;

		float alfa = deltaX*0.01;
		
		view.x = result.x*cos(alfa) - result.y*sin(alfa);
		view.y = result.x*sin(alfa) + result.y*cos(alfa);

		m_camera->SetEyePosition(at - view);

		if (m_isAltPressed && !m_isCtrlPressed)
		{		
			SetMaxLinkDistance(GetMaxLinkDistance() - deltaY*10);
		}
		else if (m_isCtrlPressed)
		{
			eye = m_camera->GetEyePosition();

			view = at - eye;

			float oldAlfa = atan(view.z/(sqrt(view.x*view.x + view.y*view.y)));

			CVector3D rotAxis = CVector3D(view.y,-view.x,0);

			rotAxis.Normalize();

			CQuaternion q = CQuaternion(rotAxis, -deltaY*0.01);
			view *=q;

			alfa = atan(view.z/(sqrt(view.x*view.x + view.y*view.y)));

			if ((alfa < - 1.5 || alfa > 1.5) || ( (oldAlfa < 0 && alfa > 0) || (oldAlfa > 0 && alfa < 0 ) )) 
			{
				return;
			}

			m_camera->SetEyePosition(at - view);
		}		
	}
}

void CCameraController::OnMouseWheel(const int delta)
{
	if (m_isFixedPosition)
	{
		return;
	}

#define MAX(a, b) ((a < b) ? b : a)

	SetMaxLinkDistance(GetDistanceFromCamera());

	float koef = MAX(m_maxLinkDistance - DEFAULT_MAX_LINK_DISTANCE, 0) * ((delta < 0) ? WHEEL_FROM_SPEED_KOEF : WHEEL_TO_SPEED_KOEF) + 1.0f;
	float newMaxLinkDistance = m_maxLinkDistance - delta * koef * COMMON_WHEEL_SPEED_KOEF;
	if (newMaxLinkDistance < MIN_MAX_LINK_DISTANCE)
	{
		newMaxLinkDistance = MIN_MAX_LINK_DISTANCE;
	}
	if (newMaxLinkDistance > MAX_MAX_LINK_DISTANCE)
	{
		newMaxLinkDistance = MAX_MAX_LINK_DISTANCE;
	}

	SetMaxLinkDistance(newMaxLinkDistance);
}

void CCameraController::PlayCameraTrack()
{
	if (m_cameraTrack)
	{
		MP_DELETE(m_cameraTrack);
	}

	m_cameraTrack = MP_NEW(CCameraTrackController);
	m_cameraTrack->LoadTrack(GetApplicationRootDirectory() + L"camera.xml");
	m_cameraTrack->StartTrack();
}

void CCameraController::PlayCameraTrack(  void* apData, int auDataSize)
{
	if (m_cameraTrack)
	{
		MP_DELETE(m_cameraTrack);
	}

	m_cameraTrack = MP_NEW(CCameraTrackController);
	m_cameraTrack->LoadTrack(  apData, auDataSize);
	m_cameraTrack->StartTrack();
}

void	CCameraController::StopTrackPlaying()
{
	if (m_cameraTrack)
	{
		MP_DELETE(m_cameraTrack);
		m_cameraTrack = NULL;
	}
}

float CCameraController::GetCameraReturnSpeed()const
{
	return (m_frameDeltaTime / 1000.0f * CAMERA_RETURN_FPS * (m_isViewTargetMovedOnLastFrame ? CAMERA_RETURN_SPEED_AVATAR_MOVED : CAMERA_RETURN_SPEED));
}

void CCameraController::OnChanged(int /*eventID*/)
{
	m_isNeedUpdate = true;
}

void CCameraController::RepairCamera()
{
	m_camera->SetLookAt(m_camera->GetLookAt() + CVector3D(1.0f, 0.0f, 0.0f));
}

CVector3D CCameraController::GetDelta()
{
	CVector3D delta(DEFAULT_DELTA);
	delta.Normalize();
	delta *= m_maxLinkDistance;

	return delta;
}

CVector3D CCameraController::GetCameraDeltaLookAt(float add)
{
	CVector3D pos = m_camera->GetEyePosition();
	CVector3D lookAt = m_camera->GetLookAt();
	CVector3D delta = pos - lookAt;
	CVector3D delta2 = delta;
	delta2.Normalize();
	lookAt += delta2 * add;

	return lookAt;
}

CVector3D CCameraController::GetCameraDelta(float add)
{
	CVector3D pos = m_camera->GetEyePosition();
	CVector3D lookAt = m_camera->GetLookAt();
	CVector3D delta = pos - lookAt;
	CVector3D delta2 = delta;
	delta2.Normalize();
	delta += delta2 * add;

	return delta;
}

bool CCameraController::IsThirdView()
{
	if (m_isFixedPosition)
	{
		return false;
	}

	if (IsFirstView())
	{
		return false;
	}

	if ((!m_cameraTarget) || (!m_cameraTarget->GetObject()))
	{
		return false;
	}

	return true;
}

bool CCameraController::IsFirstView()
{
	nrmCamera* rmCamera = (nrmCamera *)m_camera->GetUserData(USER_DATA_NRMOBJECT);
    if (rmCamera)
	if (rmCamera->GetMLMedia())
	if (rmCamera->GetMLMedia()->GetBooleanProp("firstView") == true)
	{
		return true;
	}

	return false;
}

float CCameraController::GetOffsetZ()
{
#define MAX_Z_OFFSET		180.0f
#define SITTING_DIST_SCALE	0.1f
#define TO_HEAD_KOEF		0.06f
#define TO_HEAD_KOEF_AMF	-0.1f 

	if (m_isFixedPosition)
	{
		return 0.0f;
	}

	if (IsFirstView())
	{
		return 0.0f;
	}

	if ((m_cameraTarget) && (m_cameraTarget->GetType() == CAMERA_TARGET_OBJECT))
	{
		if (SITTING_CAMERA_MODE_UNDEFINED == m_sittingCameraMode)
		{
			m_sittingCameraMode = SITTING_CAMERA_MODE_A_LA_SECOND_LIFE;

			nrmCamera* rmCamera = (nrmCamera *)m_camera->GetUserData(USER_DATA_NRMOBJECT);
            if (rmCamera)
			{
				if (rmCamera->GetMLMedia())
				{
					if (rmCamera->GetMLMedia()->GetBooleanProp("normal_sitting") == true)
					{
						m_sittingCameraMode = SITTING_CAMERA_MODE_NORMAL;
					}
				}
			}			
		}

		if ((m_cameraTarget->GetAttachObject()) && ((SITTING_CAMERA_MODE_A_LA_SECOND_LIFE == m_sittingCameraMode) ||
			(SITTING_CAMERA_MODE_AMFITEATR == m_sittingCameraMode)))
		{
			if (gRM->cameraInLocation->IsCameraInAmfiteatr())
			{
				m_sittingCameraMode = SITTING_CAMERA_MODE_AMFITEATR;
			}
			else
			{
				m_sittingCameraMode = SITTING_CAMERA_MODE_A_LA_SECOND_LIFE;
			}

			float offset = GetDelta().GetLength() * SITTING_DIST_SCALE;

			if (SITTING_CAMERA_MODE_A_LA_SECOND_LIFE == m_sittingCameraMode)
			{
				offset *= SECOND_LIFE_OFFSET_MUL_KOEF; 
			}

			if (offset < MAX_Z_OFFSET)
			{
				if (SITTING_CAMERA_MODE_AMFITEATR == m_sittingCameraMode)
				{
					offset = sinf(offset / MAX_Z_OFFSET * HALF_PI ) * MAX_Z_OFFSET;
				}
				else
				{
					offset = MAX_Z_OFFSET - sqrtf(sinf(HALF_PI - offset / MAX_Z_OFFSET * HALF_PI)) * MAX_Z_OFFSET;
				}
			}
			else
			{
				offset = MAX_Z_OFFSET;								
			}
			
			return offset + (MAX_Z_OFFSET * ((SITTING_CAMERA_MODE_AMFITEATR == m_sittingCameraMode) ? TO_HEAD_KOEF_AMF : TO_HEAD_KOEF));			
		}
		else
		{
			return 0.0f;
		}
	}
	else
	{
		return 0.0f;
	}
}

CVector3D CCameraController::GetViewTargetPosition()
{
	CVector3D pos = m_cameraTarget ? m_cameraTarget->GetViewPosition() : CVector3D(0, 0, 0);	

	return pos;
}

void CCameraController::CheckForEqualEyeAndLookAt()
{
	if ((m_camera->GetEyePosition().x == m_camera->GetLookAt().x) &&
		(m_camera->GetEyePosition().y == m_camera->GetLookAt().y))
	{
		RepairCamera();
	}
}

void CCameraController::CheckBrokenCamera()
{
	if ((m_cameraTarget) && (m_cameraTarget->GetType() == CAMERA_TARGET_OBJECT))
	if ((m_camera->GetUserData(USER_DATA_TYPE) == NULL) && (!m_isFixedPosition))
	{
		CVector3D pos = GetViewTargetPosition();

		CVector3D lookAt = m_camera->GetLookAt();
		
		pos -= lookAt;
	
		if ((pos.GetLength() > MAX_DIST_FROM_TARGET_TO_LOOK_AT) && (fabsf(pos.z) > MAX_DIST_FROM_TARGET_TO_LOOK_AT_Z))
		{
			pos = GetViewTargetPosition();
			
			m_camera->SetEyePosition(pos + GetDelta());
			m_camera->SetLookAt(pos.x, pos.y, pos.z);
			m_camera->SetUp(0, 0, 1);
		}

		pos = m_camera->GetLookAt();
		CVector3D cameraPos = m_camera->GetEyePosition();
		if ((pos - cameraPos).GetLength() > m_maxLinkDistance)
		{
			CVector3D delta = cameraPos - pos;
			delta.Normalize();
			delta *= m_maxLinkDistance;
			cameraPos = pos + delta;
			m_camera->SetEyePosition(cameraPos);
			m_isNeedUpdate = true;
		}
	}

	CheckForEqualEyeAndLookAt();
}

void	CCameraController::Update(unsigned int deltaTime)
{
	if (!m_camera)
	{
		return;
	}

	if (!IsActive())
	{
		return;
	}

	if (m_cameraTrack)
	{
		CVector3D eye, lookAt;

		int resultCode = m_cameraTrack->GetCameraParams(eye, lookAt);
		if (resultCode == PLAYING_OK)
		{
			m_camera->SetEyePosition(eye);
			m_camera->SetLookAt(lookAt);
			return;
		}
		else
		{
			if (resultCode == PLAYING_ENDED)
			{
				if (m_camera)
				{
					nrmCamera* camera = (nrmCamera *)m_camera->GetUserData(USER_DATA_NRMOBJECT);
					if (camera)
					{
						if (camera->GetMLMedia())
						{
							mlICamera*	pmlCamera = (mlICamera*)(camera->GetMLMedia()->GetICamera());
							if (pmlCamera)
							{
								pmlCamera->onTrackFilePlayed();
							}
						}
					}
				}
				
			}
			MP_DELETE(m_cameraTrack);
			m_cameraTrack = NULL;
		}
	}


	m_frameDeltaTime = deltaTime;

	g->pm.CustomBlockStart(CUSTOM_BLOCK_CAMERA_COLLISION);

	m_camera->SetZOffset(GetOffsetZ());

	bool isInHouse = gRM->cameraInLocation->IsCameraInTheHouse();
	if (m_isInHouse != isInHouse)
	{
		std::string jsCommand = "scene.objects.main_camera.SetInTheHouseMode(";
		if (isInHouse)
		{
			jsCommand += "true);";			
		}
		else
		{
			jsCommand += "false);";			
		}
		gRM->mapMan->ExecJS(jsCommand.c_str());

		m_isInHouse = isInHouse;
	}	

	if ((m_cameraTarget) && (m_cameraTarget->GetType() == CAMERA_TARGET_OBJECT))
	{
		if (m_viewTargetParent != m_cameraTarget->GetAttachObject())
		{
			m_viewTargetParent = m_cameraTarget->GetAttachObject();

			std::string jsCommand = "scene.objects.main_camera.SetSittingMode(";
			if (m_viewTargetParent)
			{
				jsCommand += "true);";
			}
			else
			{
				jsCommand += "false);";
			}

			if (m_cameraTarget)
			{
				m_cameraTarget->SetViewOffset(CVector3D(0.0f, 0.0f, 0.0f));
			}

			gRM->mapMan->ExecJS(jsCommand.c_str());

			if (m_viewTargetParent)
			{
				OnViewTargetAttached();					
			}
		}
	}

	//CheckBrokenCamera();
	UpdateTargetPositions();
	CheckBrokenCamera();

	g->pm.CustomBlockEnd(CUSTOM_BLOCK_CAMERA_COLLISION);
}

bool CCameraController::CheckForInvalidCamera(CVector3D& pos)
{
	CVector3D eyePos = m_camera->GetEyePosition();
	CVector3D lookPos = m_camera->GetLookAt();

	if ((_isnan(eyePos.x)) || (_isnan(eyePos.y)) || (_isnan(eyePos.z)) ||
		(_isnan(lookPos.x)) || (_isnan(lookPos.y)) || (_isnan(lookPos.z)) ||
		(eyePos == lookPos))
	{
		if (m_cameraTarget)
		{
			pos.z += m_cameraTarget->GetAddZ();
			m_deltaZ += m_cameraTarget->GetAddZ();
		}
		m_lastPos = pos;

		CVector3D delta = GetDelta();

		m_camera->SetEyePosition(pos + delta);
		m_camera->SetLookAt(pos.x, pos.y, pos.z);

		CheckForEqualEyeAndLookAt();

		return false;	
	}

	return true;
}

bool CCameraController::IsCameraChanged()
{
	if (m_isNeedUpdate)
	{
		m_isNeedUpdate = false;
		return true;
	}

	if (g->ne.time - m_lastUpdateTime > DELTA_RECHECK_TIME)
	{
		m_lastUpdateTime = g->ne.time;
		return true;
	}

	return (((m_lastEyePos - m_camera->GetEyePosition()).GetLengthSq() > MIN_CHANGE_DIST) ||
		((m_lastLookAt - m_camera->GetLookAt()).GetLengthSq() > MIN_CHANGE_DIST));
}

bool CCameraController::CheckForCameraUnderground(const bool isOnlyCheck)
{	
	static CVector3D lastRightCoords;

	if ((m_cameraTarget) && (m_cameraTarget->GetObject()))
	{
		float z;
		CVector3D crd = m_cameraTarget->GetObject()->GetCoords();		
		g->phom.TraceRayZ(crd.x, crd.y, &z);
		if (z - crd.z > Z_AVATAR_EPS)
		{
			return true;
		}
	}

	float z;
	g->phom.TraceRayZ(m_camera->GetEyePosition().x, m_camera->GetEyePosition().y, &z);
	z += ADD_RAYTRACE_Z;

	if ((!m_camera->GetLinkedToObject()) || ((z >= 0.0f) && (m_camera->GetLinkedToObject()->GetCoords().z >= 0.0f)))
	if (m_camera->GetEyePosition().z < z)
	{
		if (isOnlyCheck)
		{
			return false;
		}

		float z2;
		g->phom.TraceRayZ(m_camera->GetEyePosition().x, m_camera->GetEyePosition().y, &z2);
		z2 += ADD_RAYTRACE_Z;

		if (m_lastEyePos.z - Z_UNDERGROUND_EPS >= z2)
		{
			m_camera->SetEyePosition(lastRightCoords);
		}
		else
		{
			m_camera->SetEyePosition(m_camera->GetEyePosition().x, m_camera->GetEyePosition().y, z);
		}
	}

	lastRightCoords = m_camera->GetEyePosition();

	return true;
}

float CCameraController::GetDistanceFromCamera()
{
	CVector3D pos = m_camera->GetEyePosition();
	CVector3D lookAt = m_camera->GetLookAt();

	return (pos - lookAt).GetLength();
}

bool CCameraController::IncDistanceFromCamera(float value)
{
	CVector3D pos = m_camera->GetEyePosition();
	CVector3D lookAt = m_camera->GetLookAt();
	CVector3D delta(pos.x - lookAt.x, pos.y - lookAt.y, 0.0f);
	
	float length = delta.GetLength();
	float newLength = length + value;
	if (newLength < MIN_DISTANCE_TO_AVATAR)
	{
		newLength = MIN_DISTANCE_TO_AVATAR;
	}

	float r = delta.GetLength();
	delta.x /= r;
	delta.y /= r;
	delta.z = (pos.z - lookAt.z) / r;

	pos = lookAt + delta * newLength;
	m_camera->SetEyePosition(pos);

	return (fabsf(newLength - length) > MIN_DISTANCE_INCREASE);
}

void CCameraController::UpdateTargetPositions()
{
	CVector3D oldCoords = m_camera->GetEyePosition();
		
	CVector3D pos = m_camera->GetEyePosition();

	CVector3D delta(0.0f, 0.0f, 0.0f);
	if (m_cameraTarget)
	{
		pos = GetViewTargetPosition();
	}
	
	if ((_isnan(pos.x)) || (_isnan(pos.y)) || (_isnan(pos.z)))
	{
		return;
	}

	CheckForEqualEyeAndLookAt();

	if (!CheckForInvalidCamera(pos))	
	{
		return;
	}
	
	// Иначе фиксированная камера проваливается под землю
	if (m_isFixedPosition)
		return;

	if (m_cameraTarget)
	{
		delta = pos - m_lastPos;

		if ((IsThirdView()) && (m_cameraTarget) && (m_cameraTarget->IsEmptyViewOffset()))
		{
			if ((fabsf(m_lastPos.x - m_camera->GetLookAt().x) > MAX_LOOK_AT_EPS) || (fabsf(m_lastPos.y - m_camera->GetLookAt().y) > MAX_LOOK_AT_EPS))
			{
				delta = pos - m_camera->GetLookAt();
			}
		}

		CVector3D delta2 = m_camera->GetLookAt() - pos;
		if (delta2.GetLength() < 0.05f)
		{
			delta.Set(0, 0, 0);
		}

		if (m_isFixedPosition)
		{
			delta.Set(0, 0, 0);
		}
	}

	m_isViewTargetMovedOnLastFrame = (delta.GetLengthSq() > 0);

	m_camera->SetZOffset(GetOffsetZ());
	m_camera->SetEyePosition(m_camera->GetEyePosition() + delta);
	m_camera->SetLookAt(m_camera->GetLookAt() + delta);
	m_camera->SetUp(0, 0, 1);

/*	if (m_cameraTarget->GetObject())
	{
		CVector3D deltaXY = m_camera->GetLookAt() - m_camera->GetEyePosition();
		float diffZ = deltaXY.z;
		deltaXY.z = 0;
		float lenXY = deltaXY.GetLength();
		CVector3D newOfs = CVector3D(-lenXY, 0, 0);
		newOfs *= m_cameraTarget->GetObject()->GetRotation()->GetAsDXQuaternion();
		newOfs.z = -diffZ;
		m_camera->SetEyePosition(m_camera->GetLookAt() + newOfs);
	}*/



	m_lastPos = pos;	

	if (IsFirstView())
	{
		CheckForEqualEyeAndLookAt();

		m_lastRightPos = m_camera->GetEyePosition();
		m_lastRightLookAt = m_camera->GetLookAt();

		return;
	}

	if (IsCameraChanged())
	{
		CheckForCameraUnderground(false);

		CVector3D __delta = GetCameraDelta(SHIFT_EYE_FROM_TARGET) + CVector3D(0, 0, GetOffsetZ());
		
#pragma warning(push)
#pragma warning(disable : 4239)
		int iterationCount = 0;

		if (!m_isFixedPosition)
		if (m_cameraTarget)
		{
			if (m_maxLinkDistance - GetDistanceFromCamera() > MAX_DIFF_WITH_LINK_DISTANCE)
			{
				CVector3D oldPos = m_camera->GetEyePosition();
				CVector3D oldLookAt = m_camera->GetLookAt();
				CVector3D oldDelta = __delta;

				if (!COLLISION_APPEARS)
				{
					float incDistanceValue = m_maxLinkDistance - GetDistanceFromCamera();
#ifdef CAMERA_SLOW_RETURN_TO_NORMAL_POSITION					
					float returnSpeed = GetCameraReturnSpeed();
					if (incDistanceValue > returnSpeed)
					{
						incDistanceValue = returnSpeed;
					}					
#endif
					IncDistanceFromCamera(incDistanceValue);

					__delta = GetCameraDelta(SHIFT_EYE_FROM_TARGET) + CVector3D(0, 0, GetOffsetZ());					

#ifdef CAMERA_SLOW_RETURN_TO_NORMAL_POSITION
					if (COLLISION_APPEARS)
					{
						m_camera->SetEyePosition(oldPos);
						m_camera->SetLookAt(oldLookAt);							
						__delta = oldDelta;
					}					
#endif					
				}
				else
				{
					m_camera->SetEyePosition(oldPos);
					m_camera->SetLookAt(oldLookAt);	
				}
			}

			while COLLISION_APPEARS
			{
				float decDistanceValue = -__delta.GetLength() * 0.1f;

#ifdef CAMERA_SLOW_RETURN_TO_NORMAL_POSITION					
				if (decDistanceValue < -GetCameraReturnSpeed())
				{
					decDistanceValue = -GetCameraReturnSpeed();
				}
#endif

				if (!IncDistanceFromCamera(decDistanceValue))
				{
					iterationCount = MAX_SEARCH_ITERATIONS;
					break;
				}

				__delta = GetCameraDelta(SHIFT_EYE_FROM_TARGET) + CVector3D(0, 0, GetOffsetZ());		
				iterationCount++;

#ifdef CAMERA_SLOW_RETURN_TO_NORMAL_POSITION					
				break;
#endif
			}

			if (MAX_SEARCH_ITERATIONS == iterationCount)
			{
#define EPS 5.1
				if ((fabsf(m_lastRightLookAt.x - m_camera->GetLookAt().x) < EPS) &&
					(fabsf(m_lastRightLookAt.y - m_camera->GetLookAt().y) < EPS) &&
					(fabsf(m_lastRightLookAt.z - m_camera->GetLookAt().z) < EPS) &&
					((m_lastRightPos.x != oldCoords.x) ||
					 (m_lastRightPos.y != oldCoords.y) ||
					 (m_lastRightPos.z != oldCoords.z)))					
				{
					m_camera->SetEyePosition(m_lastRightPos);
					m_camera->SetLookAt(m_lastRightLookAt);
				}
				else
				{
					/*CVector3D delta = m_camera->GetLookAt();
					delta -= m_camera->GetEyePosition();
					float startAngle = delta.GetAngleZ(CVector3D(1,  0, 0));

					for (int k = 1; k <= 4; k++)
					for (int m = 0; m < 2; m++)
					{
						float angle = startAngle + HALF_PI * (float)k / 4.0f * (2 * m - 1);
						m_camera->SetEyePosition(m_viewTarget->GetCoords() + CVector3D(cos(angle) * 100.0f, sin(angle) * 100.0f, GetAddZ()));
						m_camera->SetLookAt(m_viewTarget->GetCoords() + CVector3D(0, 0, GetAddZ()));

						__delta = GetCameraDelta(SHIFT_EYE_FROM_TARGET);

						bool isNewPositionFound = true;
						if (((gRM->cameraInLocation->CheckForIntersection(GetCameraDeltaLookAt(SHIFT_LOOK_AT_FROM_TARGET), __delta)) && (__delta.GetLengthSq() > 2500.0f)) ||
							(g->phom.CheckRayIntersect(GetCameraDeltaLookAt(SHIFT_LOOK_AT_FROM_TARGET), __delta)))
						{
							isNewPositionFound = false;
						}

						if (isNewPositionFound)
						{
							break;
						}
					}*/
				}
			}
		}
		else
		{
			if COLLISION_APPEARS
			{
				m_camera->SetEyePosition(m_lastEyePos);
				m_camera->SetLookAt(m_lastLookAt);

				iterationCount++;
			}
		}

		m_isNeedUpdate = (iterationCount > 0);
#pragma warning(pop)

		m_lastEyePos = m_camera->GetEyePosition();
		m_lastLookAt = m_camera->GetLookAt();
	}

	CheckForEqualEyeAndLookAt();

	m_lastRightPos = m_camera->GetEyePosition();
	m_lastRightLookAt = m_camera->GetLookAt();
}

void CCameraController::OnMaxLinkDistanceChanged(float dist)
{
	float oldMaxLinkDistance = m_maxLinkDistance;
	m_maxLinkDistance = dist;

	if (m_cameraTarget)
	{
		CVector3D delta = m_camera->GetLookAt() - m_camera->GetEyePosition();

		if ((oldMaxLinkDistance - delta.GetLength() < MAX_DIFF_WITH_LINK_DISTANCE) ||
			(delta.GetLength() > m_maxLinkDistance))
		{
			delta.Normalize();
			delta *= m_maxLinkDistance;

			m_camera->SetEyePosition(m_camera->GetLookAt() - delta);

			CheckForEqualEyeAndLookAt();
		}
		else
		{
			m_maxLinkDistance = oldMaxLinkDistance;
		}
	}
}

float CCameraController::GetMaxLinkDistance()const
{
	return m_maxLinkDistance;
}

void CCameraController::SetCamera(CCamera3D* camera)
{
	m_camera = camera;
}

CCamera3D* CCameraController::GetCamera()const
{
	return m_camera;
}

void CCameraController::OnViewTargetAttached()
{
	return;
	if (SITTING_CAMERA_MODE_AMFITEATR != m_sittingCameraMode)
	{
		return;
	}

	// не надо ставить камеру за спину, когда садишься.
	if ((m_cameraTarget) && (m_cameraTarget->GetType() == CAMERA_TARGET_OBJECT))
	{
		CVector3D pos = GetViewTargetPosition();
		m_lastPos = pos;
		CVector3D delta(-93.0f, 0.0f, 0.0f);
		delta *= m_cameraTarget->GetObject()->GetRotation()->GetAsDXQuaternion();
		delta.z = 18.0f;

		if (SITTING_CAMERA_MODE_AMFITEATR == m_sittingCameraMode)
		{
			m_camera->SetEyePosition(pos + delta);
			m_camera->SetLookAt(pos.x, pos.y, pos.z);
		}
		else
		{
			m_camera->SetEyePosition(pos + delta);
			m_camera->SetLookAt(pos.x, pos.y, pos.z);
		}
		
		m_camera->SetEyePosition(pos + delta);
		m_camera->SetLookAt(pos.x, pos.y, pos.z);
	}

	SetMaxLinkDistance(80);
}

void CCameraController::SetMaxLinkDistance(float distance)
{
	if (m_camera)
	{
		nrmCamera* camera = (nrmCamera *)m_camera->GetUserData(USER_DATA_NRMOBJECT);
		if (camera)
		{
			if (camera->GetMLMedia())
			{
				camera->GetMLMedia()->SetProp("maxLinkDistance", distance);
			}
		}

	}
}

void CCameraController::SetCameraTarget(CCameraTarget* const cameraTarget)
{
	if (cameraTarget)
	{
		if (!m_cameraTarget)
		{
			m_cameraTarget = cameraTarget;
			CVector3D pos = GetViewTargetPosition();
			m_lastPos = pos;

			CVector3D delta = GetDelta();

			m_camera->SetEyePosition(pos + delta);
			m_camera->SetLookAt(pos.x, pos.y, pos.z);
			m_camera->SetUp(0, 0, 1);
		}

		m_cameraTarget = cameraTarget;
		m_camera->LinkToObject(m_cameraTarget->GetObject());

		CheckForEqualEyeAndLookAt();
	}
	else
	{
		m_cameraTarget = NULL;
	}
}

CCameraTarget* CCameraController::GetCameraTarget()const
{
	return m_cameraTarget;
}

void CCameraController::SetFixedPosition(bool isFixedPosition)
{
	m_isFixedPosition = isFixedPosition;
}

bool CCameraController::IsFixedPosition()const
{
	return m_isFixedPosition;
}

void CCameraController::OnCameraChangedTo(CCameraController* newCameraController)
{
	// correct camera position if thirdview camera of sitting avatar is changed to free camera
	if ((!newCameraController->m_cameraTarget) && (m_cameraTarget) && (m_viewTargetParent) 
		&& (newCameraController->m_camera))
	{		
		if (IsFirstView())
		{
			return;
		}

		if (m_isFixedPosition)
		{
			return;
		}		

		newCameraController->m_camera->SetEyePosition(newCameraController->m_camera->GetEyePosition() + CVector3D(0, 0, m_camera->GetZOffset()));
		newCameraController->m_camera->SetLookAt(newCameraController->m_camera->GetLookAt() + CVector3D(0, 0, 3 * m_camera->GetZOffset()));
	}	

	if (m_cameraTarget)
	{
		CVector3D offset = CVector3D(0,0,0);
		m_cameraTarget->SetViewOffset(offset);
		m_lastPos = m_cameraTarget->GetViewPosition();
	}
}

void CCameraController::OnKeyDown(const unsigned char keycode, const unsigned char scancode, const bool isAlt, const bool isCtrl, const bool isShift)
{
	m_isAltPressed = isAlt;
	m_isCtrlPressed = isCtrl;
	m_isShiftPressed = isShift;

	if( gRM->mouseCtrl->IsKeyboardInputNeeded())
	{
		// фильтрация управления камерой в режиме рисования и управления чужим шарингом
		return;
	}

	if (m_isCtrlPressed)
	{
		if (scancode == 57)
		{
			g->rs.SetInt(FOV, 35);
		}
		else if (scancode == 48)
		{
			if (g->rs.GetInt(FOV) > 20)
				g->rs.SetInt(FOV, g->rs.GetInt(FOV) - 10);
		}
		else if (scancode == 56)
		{
			if (g->rs.GetInt(FOV) < 70)
				g->rs.SetInt(FOV, g->rs.GetInt(FOV) + 10);
		}
	}	

	if (m_isCtrlPressed && m_isAltPressed && scancode == 107)
	{
		g->rs.SetInt(FOV, 90);
	}

	// Alt+Ctrl+стрелочки - поворот вокруг аватара и наклон
	// Alt+стрелочки - поворот вокруг аватара и приближение/удаление
	// Alt+Ctrl+Shift+стрелочки - сдвиг по плоскости
	//
	// Когда сидишь:
	// стрелочки - камера вокруг аватара, когда сидит
	// Page up/down - наклоняют/опускают камеру

	bool isSit = false;
	if (m_cameraTarget)
	{
		isSit = (m_cameraTarget->GetAttachObject() != NULL)? true : false; 
	}

	if (((scancode == 33) || (scancode == 34)) && (!isSit))
	if (!IsFirstView())
	{
		OnMouseWheel((scancode == 33) ? 200 : -200);
	}

	if (!IsFirstView())
	if ( ((m_isAltPressed || isSit)
		 &&  (scancode == 37 || scancode == 38 || scancode == 39 || scancode == 40)) 
		|| ( (scancode == 33 || scancode == 34) && isSit) )
	{
		C3DObject* obj = m_camera->GetLinkedToObject();

		if (!obj)
		{
			return;
		}

		CVector3D eye = m_camera->GetEyePosition();
		CVector3D at = m_camera->GetLookAt();

		CVector3D view = at - eye;

		float alfa = 0;

		switch (scancode)
		{
		case 33:
			{
				Rotate(DIRECTION_UP);
			}
			break;
		case 34:
			{
				Rotate(DIRECTION_DOWN);
			}
			break;
		case 37:
			{
				if (m_isShiftPressed)
				{
					Move(DIRECTION_LEFT);
				}
				else
				{
					Rotate(DIRECTION_LEFT);
				}
			}
			break;
		case 38:
			{
				if (m_isShiftPressed && m_isCtrlPressed)
				{
					Move(DIRECTION_UP);
				}
				else if (!m_isShiftPressed) 
				{
					if (!m_isCtrlPressed && !isSit)
					{
						SetMaxLinkDistance(GetMaxLinkDistance() - 80);
					}
					else
					{
						Rotate(DIRECTION_UP);
					}
				}
			}
			break;
		case 39:
			{
				if (m_isShiftPressed)
				{
					Move(DIRECTION_RIGHT);
				}
				else
				{
					Rotate(DIRECTION_RIGHT);
				}
			}
			break;
		case 40:
			{
				if (m_isShiftPressed && m_isCtrlPressed)
				{
					Move(DIRECTION_DOWN);
				}
				else if (!m_isShiftPressed) 
				{
					if (!m_isCtrlPressed && !isSit)
					{
						SetMaxLinkDistance(GetMaxLinkDistance() + 80);
					}
					else
					{
						Rotate(DIRECTION_DOWN);
					}
				}
			}
			break;
		}	
	}	
}

void CCameraController::OnKeyUp(const unsigned char keycode, const bool isAlt, const bool isCtrl, const bool isShift)
{
	m_isAltPressed = isAlt;
	m_isCtrlPressed = isCtrl;
	m_isShiftPressed = isShift;

	if( gRM->mouseCtrl->IsKeyboardInputNeeded())
	{
		// фильтрация управления камерой в режиме рисования и управления чужим шарингом
		return;
	}

	if (!IsFirstView())
	if ((m_cameraTarget) && (m_cameraTarget->GetType() == CAMERA_TARGET_POINT) && (!isAlt))
	{
		m_cameraTarget->SetViewTarget(m_lastViewTarget);
		m_isZoomAroundSelectedPoint = false;
	}
}


void CCameraController::Rotate(int aiDirection, float koef)
{
	if (!m_camera)
	{
		return;
	}

	C3DObject* obj = m_camera->GetLinkedToObject();

	if (!obj)
	{
		return;
	}

	CVector3D eye = m_camera->GetEyePosition();
	CVector3D at = m_camera->GetLookAt();

	CVector3D view = at - eye;

	CVector3D result = view;

	float alfa = 0.15 * koef;

	if (aiDirection == DIRECTION_LEFT || aiDirection == DIRECTION_DOWN)
		alfa = - 0.15 * koef;

	if (aiDirection == DIRECTION_RIGHT ||  aiDirection  == DIRECTION_LEFT)
	{
		view.x = result.x*cos(alfa) - result.y*sin(alfa);
		view.y = result.x*sin(alfa) + result.y*cos(alfa);

		m_camera->SetEyePosition(at - view);
	}
	else
	{
		// 1. спроецируем вектор взгляда на плоскость XOY
		// 2. повернем на 90 градусов - получим ось вращения
		// 3. определим кватернион
		CVector3D rotAxis = CVector3D(view.y,-view.x,0);

		rotAxis.Normalize();

		float angle = (aiDirection == DIRECTION_UP)? 0.06f : -0.06f;

		CQuaternion q = CQuaternion(rotAxis, angle);
		view *=q;

		alfa = atan(view.z/(sqrt(view.x*view.x + view.y*view.y)));

		if (alfa < -1.0f || alfa > 1.0f) 
		{
			return;
		}

		m_camera->SetEyePosition(at - view);
	}
}

void CCameraController::Move(int aiDirection, int aiOffset)
{
	if (!m_cameraTarget)
	{
		return;
	}

	CVector3D offset = CVector3D(0,0,0);

	if (aiDirection == 3 || aiDirection == 1)
	{
		offset = m_camera->GetMainVector2();
	}
	else
	{
		offset = m_camera->GetMainVector1();
	}

	if (aiOffset == -1)
		offset *= 50;
	else
	{
		aiOffset *=0.5;
		offset *= aiOffset;
	}

	CVector3D oldOffset = m_cameraTarget->GetViewOffset();

	if ((aiDirection != 1) && (aiDirection != 2))
	{
		offset *= -1;
	}

	m_cameraTarget->SetViewOffset(oldOffset + offset);

	CVector3D __delta = GetCameraDelta(SHIFT_EYE_FROM_TARGET) + CVector3D(0, 0, GetOffsetZ());
	int iterationCount = 0;

	CCamera3D* oldState = m_camera->SaveState();

	m_camera->SetEyePosition(m_camera->GetEyePosition() + offset);
	m_camera->SetLookAt(m_camera->GetLookAt() + offset);

	if (m_maxLinkDistance - GetDistanceFromCamera() > MAX_DIFF_WITH_LINK_DISTANCE)
	{
		m_cameraTarget->SetViewOffset(oldOffset);		
	}
	else if (COLLISION_APPEARS)
	{
		m_cameraTarget->SetViewOffset(oldOffset);
	}
	else if (!CheckForCameraUnderground(true))
	{
		m_cameraTarget->SetViewOffset(oldOffset);
	}

	//m_camera->RestoreState(oldState);
	m_camera->SetEyePosition(m_camera->GetEyePosition() - offset);
	m_camera->SetLookAt(m_camera->GetLookAt() - offset);
	MP_DELETE(oldState);
}

CCameraController::~CCameraController()
{
	gRM->cameraInLocation->DeleteChangesListener(this);
	gRM->mouseCtrl->DeleteMouseListener(this);

	if (m_cameraTrack)
	{
		MP_DELETE(m_cameraTrack);
	}
}