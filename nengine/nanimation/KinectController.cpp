
#include "StdAfx.h"
#include <Assert.h>
#include "IniFile.h"
#include "KinectController.h"
#include "PlatformDependent.h"
#include "../../cal3d/coreskeleton.h"
#include "../../cal3d/corebone.h"
#include "GlobalSingletonStorage.h"
#include "GlobalKinect.h"
#include "KinectPowerPointManipulator.h"

#define POSE_1	1	// руки в стороны
#define POSE_2	2	// руки в бока

#define GET_BONE_BY_NAME(a)			GetBoneByName(skeleton, coreSkeleton, a, "");
#define GET_BONE_BY_NAMES(a, b)		GetBoneByName(skeleton, coreSkeleton, a, b);
#define UNLOCK_BONE(b)				if (b) b->unlockBone();
#define CALC_STATE(b)				if (b) b->calculateState();
#define UPDATE_BONE(b, isEndBone)	if (b) b->calculateState(true, isEndBone);

// минимальное расстояние до кинекта, с которого он стабильно работает в стоячем режиме
#define	MIN_KINECT_Z				1.8f
// минимальное расстояние до кинекта, с которого он стабильно работает в сидячем режиме
#define	MIN_SEATED_KINECT_Z			1.3f

// время, которое кость может находится в последнем распознанном состоянии
// по истечении него кость сбрасывается в исходное состояние
#define MAX_UNDEFINED_TIME			500

// задержка до реального включения кинекта после нажатия кнопка включения кинекта
#define ENABLE_KINECT_DELAY			5000

// расстояние, на которое отодвигаем аватара от доски, чтобы он не залезал в нее руками
#define DISTANCE_FROM_DESK			100.0f

// если все части тела распознаются хуже, чем этот порог, то считаем скелет нераспознанным полностью
#define MIN_QUALITY					0.7f

// коэф-т для масштабирования перемещения из кинектовских координат в координаты мира
#define AVATAR_MOVING_SCALE			50.0f

// максимальный угол поворота аватара
#define MAX_AVATAR_ANGLE			30

// длина "опасного" диапазона, в котором засчитывается возврат из "слишком повернутого" состояния
#define AVATAR_ANGLE_DANGER_RANGE	10

// количество видимых символов, начиная с которого аватар считается повернутым лицом к кинекту
unsigned int MARKER_PIXELS_COUNT_THRESHOLD = 2000;

CKinectController::CKinectController(CSkeletonAnimationObject* sao):
	m_sao(sao),	
	m_headBone(NULL),
	m_neckBone(NULL),
	m_neckBone2(NULL),
	m_spine(NULL),
	m_spine1(NULL),
	m_spine2(NULL),
	m_kinectVideo(NULL),
	m_oldSynchData(NULL),
	m_shoulderCenterLeft(NULL),
	m_shoulderLeft(NULL),
	m_elbowLeft(NULL),
	m_handLeft(NULL),
	m_twistLeft(NULL),
	m_shoulderCenterRight(NULL),
	m_shoulderRight(NULL),
	m_elbowRight(NULL),
	m_handRight(NULL),
	m_twistRight(NULL),
	m_thighLeft(NULL),
	m_calfLeft(NULL),
	m_footLeft(NULL),
	m_thighRight(NULL),
	m_calfRight(NULL),
	m_footRight(NULL),
	m_pelvis(NULL),
	m_isEnabled(false),
	m_isSeated(false),
	m_lastBipPosition(0, 0, 0),
	m_lastDirection(0, 0, 0),
	m_lastFrameTracked(false),
	m_bipTracked(true),
	m_headTracked(true),	
	m_enableKinectTime(0),
	m_leftArmTracked(true),
	m_rightArmTracked(true),
	m_leftFootTracked(true),
	m_rightFootTracked(true),
	m_isKinectDeskUsed(false),
	m_bipNotTrackedStartTime(0),
	m_headNotTrackedStartTime(0),
	m_leftArmNotTrackedStartTime(0),
	m_rightArmNotTrackedStartTime(0),
	m_leftFootNotTrackedStartTime(0),
	m_rightFootNotTrackedStartTime(0),
	m_isRotatedToBack(false),
	m_kinectDeskPlane(NULL),
	m_kinectDeskObj(NULL),
	m_areMarkersNotFound(false)
{
	assert(m_sao);
	InitBonePointers();

	int i;
	for (i = 0; i < KINECT_JOINT_COUNT * 3; i++)
	{
		m_lastRightCoords[i] = 0;
		m_lastRightHandEndCoords[i] = 0;
	}	

	m_gestureDetector.AddGestureHandler(GESTURE_SWIPE_LEFT, &m_ppMan);
	m_gestureDetector.AddGestureHandler(GESTURE_SWIPE_RIGHT, &m_ppMan);

	m_markerType = MARKER_NO_MARKER;

	for (i = 0; i < COLOR_MARKER_FRAMES; i++)
	{
		m_markersPixelCountByFrame[i] = MARKER_PIXELS_COUNT_THRESHOLD;
	}

	m_kinectPointer.SetPowerPointManipulator(&m_ppMan);

	ReloadSettings();
}

void CKinectController::ReloadSettings()
{
	std::wstring fileName = GetApplicationRootDirectory();
	fileName += L"player.ini";
	CIniFile ini(fileName);
	std::string threshold = ini.GetString("kinect", "threshold");
	if (threshold != "")
	{
		MARKER_PIXELS_COUNT_THRESHOLD = rtl_atoi(threshold.c_str());
	}

	std::string debugMode = ini.GetString("kinect", "debugMode");
	if (debugMode != "")
	{
		if (m_sao->GetKinectObject())
		{
			m_sao->GetKinectObject()->SetDebugMode(rtl_atoi(debugMode.c_str()));
		}
	}

	std::string mainColorKoef = ini.GetString("kinect", "mainColorKoef");
	std::string otherColorKoef1 = "";
	std::string otherColorKoef2 = "";

	otherColorKoef1 = ini.GetString("kinect", "blueColorKoef1");
	otherColorKoef2 = ini.GetString("kinect", "blueColorKoef2");
	if ((otherColorKoef1 != "") && (otherColorKoef2 != ""))
	{
		m_otherColorKoef1[0] = rtl_atoi(otherColorKoef1.c_str());
		m_otherColorKoef2[0] = rtl_atoi(otherColorKoef2.c_str());
	}
	else
	{
		m_otherColorKoef1[0] = 7;
		m_otherColorKoef2[0] = 9;
	}
	otherColorKoef1 = ini.GetString("kinect", "greenColorKoef1");
	otherColorKoef2 = ini.GetString("kinect", "greenColorKoef2");
	if ((otherColorKoef1 != "") && (otherColorKoef2 != ""))
	{
		m_otherColorKoef1[1] = rtl_atoi(otherColorKoef1.c_str());
		m_otherColorKoef2[1] = rtl_atoi(otherColorKoef2.c_str());
	}
	else
	{
		m_otherColorKoef1[1] = 7;
		m_otherColorKoef2[1] = 9;
	}

	otherColorKoef1 = ini.GetString("kinect", "redColorKoef1");
	otherColorKoef2 = ini.GetString("kinect", "redColorKoef2");
	if ((otherColorKoef1 != "") && (otherColorKoef2 != ""))
	{
		m_otherColorKoef1[2] = rtl_atoi(otherColorKoef1.c_str());
		m_otherColorKoef2[2] = rtl_atoi(otherColorKoef2.c_str());
	}
	else
	{
		m_otherColorKoef1[2] = 7;
		m_otherColorKoef2[2] = 9;
	}


	if (mainColorKoef != "")
	{
		if (m_sao->GetKinectObject())
		{
			m_sao->GetKinectObject()->SetColorDetectionKoefs(rtl_atoi(mainColorKoef.c_str()), m_otherColorKoef1, m_otherColorKoef2);
		}
	}
}

/* У кинекта:
X - вправо
Y - вверх
Z - от нас
Человек стоит лицом смотря в положительном направлении оси Z

// У Cal3D:
X - вправо
Y - от нас
Z - вверх
Человек стоит лицом смотря в положительном направлении оси X
*/
CVector3D CKinectController::KinectCoordsToCal3D( CVector3D aKinectVector)
{
	CVector3D res = CVector3D( -aKinectVector.z,  -aKinectVector.x, aKinectVector.y);
	if (!m_isSeated)
	{
		res *= CQuaternion(m_shiftRotation.x, m_shiftRotation.y, m_shiftRotation.z, -m_shiftRotation.w);
	}
	//res *= CQuaternion(m_qlobalRotation.x, m_qlobalRotation.y, m_qlobalRotation.z, -m_qlobalRotation.w);
	return res;
}

bool CKinectController::IsRightCoordsForRightHandEndExists()
{
	for (int k = 0; k < 3; k++)
	{
		if (m_lastRightHandEndCoords[SKELETON_POSITION_HAND_RIGHT * 3 + k] != 0.0f)
		{
			return true;
		}

		if (m_lastRightHandEndCoords[SKELETON_POSITION_WRIST_RIGHT * 3 + k] != 0.0f)
		{
			return true;
		}
	}

	return false;
}

bool CKinectController::IsRightCoordsForLeftHandEndExists()
{
	for (int k = 0; k < 3; k++)
	{
		if (m_lastRightHandEndCoords[SKELETON_POSITION_HAND_LEFT * 3 + k] != 0.0f)
		{
			return true;
		}

		if (m_lastRightHandEndCoords[SKELETON_POSITION_WRIST_LEFT * 3 + k] != 0.0f)
		{
			return true;
		}
	}

	return false;
}

bool CKinectController::IsAnythingTracked()
{	
	return m_lastFrameTracked;
}

bool CKinectController::IsHeadTracked()
{
	return m_headTracked;
}

bool CKinectController::IsHipTracked()
{
	return m_bipTracked;
}

bool CKinectController::IsLeftArmTracked()
{
	return m_leftArmTracked;
}

bool CKinectController::IsRightArmTracked()
{
	return m_rightArmTracked;
}

bool CKinectController::IsLeftFootTracked()
{
	if (m_isSeated)
	{
		return true;
	}

	return m_leftFootTracked;
}

bool CKinectController::IsRightFootTracked()
{
	if (m_isSeated)
	{
		return true;
	}

	return m_rightFootTracked;
}

CalBone* GetBoneByName(CalSkeleton* skeleton, CalCoreSkeleton* coreSkeleton, std::string boneName, std::string altBoneName)
{
	int boneID = -1;
	CalBone* bone = NULL;
	if (coreSkeleton->containBone(boneName))
	{
		boneID = coreSkeleton->getCoreBoneIdSubName(boneName.c_str());
	}
	if ((-1 == boneID) && (coreSkeleton->containBone(altBoneName)))
	{
		boneID = coreSkeleton->getCoreBoneIdSubName(altBoneName.c_str());
	}
	bone = skeleton->getBone(boneID);

	return bone;
}

void CKinectController::UnlockRightFootBones()
{
	UNLOCK_BONE(m_thighRight);
	UNLOCK_BONE(m_calfRight);
}

void CKinectController::UnlockLeftFootBones()
{
	UNLOCK_BONE(m_thighLeft);
	UNLOCK_BONE(m_calfLeft);
}

void CKinectController::UnlockRightArmBones()
{
	//UNLOCK_BONE(m_shoulderCenterRight);
	UNLOCK_BONE(m_shoulderRight);
	UNLOCK_BONE(m_elbowRight);
	UNLOCK_BONE(m_handRight);
	UNLOCK_BONE(m_twistRight);	
}

void CKinectController::UnlockLeftArmBones()
{
	//UNLOCK_BONE(m_shoulderCenterLeft);
	UNLOCK_BONE(m_shoulderLeft);
	UNLOCK_BONE(m_elbowLeft);
	UNLOCK_BONE(m_handLeft);
	UNLOCK_BONE(m_twistLeft);
}

void CKinectController::UnlockHeadBones()
{
	UNLOCK_BONE(m_headBone);	
}

void CKinectController::UpdateRightFootBones()
{
	UPDATE_BONE(m_thighRight, false);
	UPDATE_BONE(m_calfRight, true);
}

void CKinectController::UpdateLeftFootBones()
{
	UPDATE_BONE(m_thighLeft, false);
	UPDATE_BONE(m_calfLeft, true);
}

void CKinectController::UpdateRightArmBones()
{	
	UPDATE_BONE(m_shoulderRight, false);
	UPDATE_BONE(m_elbowRight, false);
	UPDATE_BONE(m_handRight, false);
	UPDATE_BONE(m_twistRight, true);	
}

void CKinectController::UpdateLeftArmBones()
{	
	UPDATE_BONE(m_shoulderLeft, false);
	UPDATE_BONE(m_elbowLeft, false);
	UPDATE_BONE(m_handLeft, false);
	UPDATE_BONE(m_twistLeft, true);
}

void CKinectController::UpdateHeadBones()
{
	UPDATE_BONE(m_headBone, false);	
}

void CKinectController::UnlockFootBones()
{
	UnlockLeftFootBones();
	UnlockRightFootBones();
}

void CKinectController::UnlockArmBones()
{
	UnlockLeftArmBones();
	UnlockRightArmBones();
}

void CKinectController::UnlockBones()
{
	UnlockArmBones();
	UnlockFootBones();
	UnlockHeadBones();
	UNLOCK_BONE(m_pelvis);
}

void CKinectController::InitBonePointers()
{
	if (!m_sao)
	{
		return;
	}

	CalModel* calModel = m_sao->GetCalModel();
	if (!calModel)
	{
		return;
	}

	CalSkeleton* skeleton = calModel->getSkeleton();
	if (!skeleton)
	{
		return;
	}

	CalCoreSkeleton* coreSkeleton = skeleton->getCoreSkeleton();
	if (!coreSkeleton)
	{
		return;
	}

	m_headBone = GET_BONE_BY_NAMES("Head", "head");
	m_neckBone = GET_BONE_BY_NAME("Neck");
	m_neckBone2 = GET_BONE_BY_NAME("Neck 1");
	m_headBone = GET_BONE_BY_NAMES("Head", "head");
	m_spine = GET_BONE_BY_NAME("Spine");
	m_spine1 = GET_BONE_BY_NAME("Spine 1");
	m_spine2 = GET_BONE_BY_NAME("Spine 2");
	m_shoulderCenterLeft = GET_BONE_BY_NAME("L Clavicle");
	m_shoulderLeft = GET_BONE_BY_NAME("L UpperArm");
	m_elbowLeft = GET_BONE_BY_NAME("L Forearm");
	m_handLeft = GET_BONE_BY_NAME("L Hand");
	m_twistLeft = GET_BONE_BY_NAME("L ForeTwist");
	m_shoulderCenterRight = GET_BONE_BY_NAME("R Clavicle");
	m_shoulderRight = GET_BONE_BY_NAME("R UpperArm");
	m_elbowRight = GET_BONE_BY_NAME("R Forearm");
	m_handRight = GET_BONE_BY_NAME("R Hand");
	m_twistRight = GET_BONE_BY_NAME("R ForeTwist");	
	m_thighRight = GET_BONE_BY_NAME("R Thigh");	
	m_calfRight = GET_BONE_BY_NAME("R Calf");	
	m_footRight = GET_BONE_BY_NAME("R Foot");	
	m_thighLeft = GET_BONE_BY_NAME("L Thigh");	
	m_calfLeft = GET_BONE_BY_NAME("L Calf");	
	m_footLeft = GET_BONE_BY_NAME("L Foot");	
	m_pelvis = GET_BONE_BY_NAME("Pelvis");	
}

bool CKinectController::IsKinectDeskModeEnabled()const
{
	return m_isKinectDeskUsed;
}

CVector3D CKinectController::GetExtendedHandDirection(CVector3D& pos, CVector3D& direction)
{	
	if ((!m_kinectDeskPlane) || (!m_isKinectDeskUsed))
	{	
		return CVector3D(0, 0, 0);
	}

	CVector3D intersectedPoint;
	if (!m_kinectDeskPlane->GetIntersection(pos, direction, intersectedPoint))
	{		
		return CVector3D(0, 0, 0);
	}

	if ((m_kinectDeskObj) && (m_kinectDeskObj->GetLODGroup()) &&
		(m_kinectDeskObj->GetLODGroup()->GetLodLevel(0)))
	{
		int modelID = m_kinectDeskObj->GetLODGroup()->GetLodLevel(0)->GetModelID();
		CModel* model = g->mm.GetObjectPointer(modelID);
		float tx = -1, ty = -1;
		int ttx = 0, tty = 0;
		model->GetTextureCoordsForPoint(m_kinectDeskObj->GetCoords(), 
			m_kinectDeskObj->GetRotation()->GetAsDXQuaternion(), m_kinectDeskObj->GetScale3D(),
			intersectedPoint.x, intersectedPoint.y, intersectedPoint.z, tx, ty, ttx, tty);
		if ((tx < 0) || (ty < 0) || (tx > 1) || (ty > 1))
		{	
			return CVector3D(0, 0, 0);
		}
		else
		{
			m_kinectPointer.ShowPointer(tx, ty);
		}
	}

	CVector3D extendedDirection = intersectedPoint - pos;
	return extendedDirection;
}

void CKinectController::OnHandsNotRecognized()
{
	m_kinectPointer.StopShow();
}

void CKinectController::SetSecondaryLastRightCoords(float* coords)
{
	for (int k = 0; k < 3; k++)
	{
		m_lastRightHandEndCoords[SKELETON_POSITION_HAND_RIGHT * 3 + k] = coords[67+k];
		m_lastRightHandEndCoords[SKELETON_POSITION_WRIST_RIGHT * 3 + k] = coords[70+k];
		m_lastRightHandEndCoords[SKELETON_POSITION_HAND_LEFT * 3 + k] = coords[73+k];
		m_lastRightHandEndCoords[SKELETON_POSITION_WRIST_LEFT * 3 + k] = coords[76+k];
		m_lastRightHandEndCoords[SKELETON_POSITION_SHOULDER_CENTER * 3 + k] = coords[79+k];
		m_lastRightHandEndCoords[SKELETON_POSITION_HEAD * 3 + k] = coords[82+k];
	}
}

void CKinectController::SetKinectDeskPoints(CVector3D start, CVector3D end, C3DObject* obj)
{
	m_isKinectDeskUsed = true;

	if (obj)
	{
		if (!m_isSeated)
		{
			m_kinectVideo = (CKinectVideoTexture*)m_sao->GetVideoDynamicTexture();
			m_kinectVideo->EnableAnalyzeMode(true);
			m_sao->GetKinectObject()->SetSpecificPixelsChannel((char)m_markerType);	
		}	
	}

	m_kinectDeskStart = start;
	m_kinectDeskObj = obj;
	m_kinectDeskEnd = end;	

	if ((m_kinectDeskObj) && (m_kinectDeskObj->GetLODGroup()) &&
		(m_kinectDeskObj->GetLODGroup()->GetLodLevel(0)))
	{
		int modelID = m_kinectDeskObj->GetLODGroup()->GetLodLevel(0)->GetModelID();
		CModel* model = g->mm.GetObjectPointer(modelID);
		model->GetTraceablePlanePoints(m_kinectDeskObj->GetCoords(), 
			m_kinectDeskObj->GetRotation()->GetAsDXQuaternion(), m_kinectDeskObj->GetScale3D(),
			&m_kinectDeskPoints[0]);

		MP_NEW_V3(m_kinectDeskPlane, CPlane3D, m_kinectDeskPoints[0], m_kinectDeskPoints[1],
			m_kinectDeskPoints[2]);		
	}

	m_sao->SetTrackingRightHand(true);
	m_sao->SetTrackingLeftHand(true);

	if (obj)
	{
		m_ppMan.Prepare();
	}
}

void CKinectController::DisableKinectDesk()
{
	m_isKinectDeskUsed = false;
	m_kinectDeskObj = NULL;
	m_ppMan.EndWork();
}

bool CKinectController::GetSynchData(unsigned char* data, int* size)
{
	if (g->ne.time - m_enableKinectTime < ENABLE_KINECT_DELAY)
	{
		*size = 0;
		return false;
	}

	if (!m_sao)
	{
		return false;
	}

	if (!data)
	{
		*size = KINECT_SYNCHDATA_SIZE;
		return false;
	}
	
	IKinect* kinect = m_sao->GetKinectObject();		

	float* _data = (float*)data;
	_data[0] = kinect->GetBipTrackingQuality();
	_data[1] = kinect->GetHeadTrackingQuality();
	_data[2] = kinect->GetLeftArmTrackingQuality();
	_data[3] = kinect->GetRightArmTrackingQuality();
	_data[4] = kinect->GetLeftFootTrackingQuality();
	_data[5] = kinect->GetRightFootTrackingQuality();
	memcpy(&_data[6], m_lastRightCoords, 20 * 3 * 4);
	data[SKELETON_SIZE - 4] = kinect->IsFoundSkeleton();
	data[SKELETON_SIZE - 3] = m_isSeated;
	data[SKELETON_SIZE - 2] = (kinect->GetBipsTrackingQuality() == 1.0f);
	data[SKELETON_SIZE - 1] = m_isRotatedToBack;
	if (!m_isEnabled)
	{
		data[SKELETON_SIZE - 4] = 0;
	}
	for (int k = 0; k < 3; k++)
	{
		_data[67+k] = m_lastRightHandEndCoords[SKELETON_POSITION_HAND_RIGHT * 3 + k];
		_data[70+k] = m_lastRightHandEndCoords[SKELETON_POSITION_WRIST_RIGHT * 3 + k];
		_data[73+k] = m_lastRightHandEndCoords[SKELETON_POSITION_HAND_LEFT * 3 + k];
		_data[76+k] = m_lastRightHandEndCoords[SKELETON_POSITION_WRIST_LEFT * 3 + k];
		_data[79+k] = m_lastRightHandEndCoords[SKELETON_POSITION_SHOULDER_CENTER * 3 + k];
		_data[82+k] = m_lastRightHandEndCoords[SKELETON_POSITION_HEAD * 3 + k];
	}
	if (m_isKinectDeskUsed)
	{
		_data[85] = m_kinectDeskStart.x;
		_data[86] = m_kinectDeskStart.y;
		_data[87] = m_kinectDeskStart.z;
		_data[88] = m_kinectDeskEnd.x;
		_data[89] = m_kinectDeskEnd.y;
		_data[90] = m_kinectDeskEnd.z;
	}
	else
	{
		_data[85] = 0.0f;
		_data[86] = 0.0f;
		_data[87] = 0.0f;
		_data[88] = 0.0f;
		_data[89] = 0.0f;
		_data[90] = 0.0f;
	}
	kinect->GetHeadRotation(&_data[91], &_data[94]);
	unsigned int count;
	kinect->GetAUCoeffs(&_data[97], &count);
	data[103 * 4] = kinect->IsFTSuccessful();
	data[103 * 4 + 1] = (unsigned char)count;
	*size = KINECT_SYNCHDATA_SIZE;
	if (!m_oldSynchData)
	{
		m_oldSynchData = MP_NEW_ARR(unsigned char, *size);
	}
	bool isNewState = (memcmp(m_oldSynchData, data, *size) != 0);
	if (isNewState)
	{
		memcpy(m_oldSynchData, data, *size);
		return true;
	}
	else
	{
		return false;
	}
}

void CKinectController::SetRotatedToBack(bool isEnabled)
{
	m_isRotatedToBack = isEnabled;
}

bool CKinectController::Update()
{	
	if (!m_sao)
	{
		m_lastFrameTracked = false;
		return false;
	}

	if (!m_pelvis)
	{
		InitBonePointers();
	}

	if (!m_isEnabled)
	{
		UnlockBones();
		m_lastFrameTracked = true;
		return false;
	}
	
	IKinect* kinect = m_sao->GetKinectObject();		
	if (!kinect)
	{
		m_lastFrameTracked = false;
		return false;
	}

	if ((kinect->GetHeadTrackingQuality() <= MIN_QUALITY) && (kinect->GetLeftArmTrackingQuality() <= MIN_QUALITY)
		&& (kinect->GetRightArmTrackingQuality() <= MIN_QUALITY) && (kinect->GetLeftFootTrackingQuality() <= MIN_QUALITY)
		&& (kinect->GetRightFootTrackingQuality() <= MIN_QUALITY))
	{
		UnlockBones();
		m_lastFrameTracked = false;
		return false;
	}

	if ((kinect->IsSkeletonClippedLeft()) || (kinect->IsSkeletonClippedRight()))
	{
		UnlockBones();
		m_lastFrameTracked = false;
		return false;
	}

	float* coords = kinect->GetSkeletonXYZ();

	if (IsTooNear(coords))
	{
		UnlockBones();
		m_lastFrameTracked = false;
		return false;
	}

	if (kinect)
	{
		if (kinect->IsFoundSkeleton())
		{
			m_lastFrameTracked = true;
			UpdateImpl(coords);
			return true; 
		}

		UnlockBones();
		m_lastFrameTracked = false;
		return false;
	}

	m_lastFrameTracked = false;
	return false;
}

void CKinectController::PatchCoordsForPredefinedPose(float* coords, const int poseID)
{
	if (!coords)
	{
		return;
	}

	if (poseID == POSE_1)
	{
		// руки в стороны
		coords[SKELETON_POSITION_SHOULDER_CENTER * 3 + 0] = 0;
		coords[SKELETON_POSITION_SHOULDER_CENTER * 3 + 1] = 0;
		coords[SKELETON_POSITION_SHOULDER_CENTER * 3 + 2] = 0;

		coords[SKELETON_POSITION_SHOULDER_LEFT * 3 + 0] = -1;
		coords[SKELETON_POSITION_SHOULDER_LEFT * 3 + 1] = 0;
		coords[SKELETON_POSITION_SHOULDER_LEFT * 3 + 2] = 0;
		coords[SKELETON_POSITION_ELBOW_LEFT * 3 + 0] = -2;
		coords[SKELETON_POSITION_ELBOW_LEFT * 3 + 1] = 0;
		coords[SKELETON_POSITION_ELBOW_LEFT * 3 + 2] = 0;
		coords[SKELETON_POSITION_WRIST_LEFT * 3 + 0] = -3;
		coords[SKELETON_POSITION_WRIST_LEFT * 3 + 1] = 0;
		coords[SKELETON_POSITION_WRIST_LEFT * 3 + 2] = 0;
		coords[SKELETON_POSITION_HAND_LEFT * 3 + 0] = -4;
		coords[SKELETON_POSITION_HAND_LEFT * 3 + 1] = 0;
		coords[SKELETON_POSITION_HAND_LEFT * 3 + 2] = 0;

		coords[SKELETON_POSITION_SHOULDER_RIGHT * 3 + 0] = 1;
		coords[SKELETON_POSITION_SHOULDER_RIGHT * 3 + 1] = 0;
		coords[SKELETON_POSITION_SHOULDER_RIGHT * 3 + 2] = 0;
		coords[SKELETON_POSITION_ELBOW_RIGHT * 3 + 0] = 2;
		coords[SKELETON_POSITION_ELBOW_RIGHT * 3 + 1] = 0;
		coords[SKELETON_POSITION_ELBOW_RIGHT * 3 + 2] = 0;
		coords[SKELETON_POSITION_WRIST_RIGHT * 3 + 0] = 3;
		coords[SKELETON_POSITION_WRIST_RIGHT * 3 + 1] = 0;
		coords[SKELETON_POSITION_WRIST_RIGHT * 3 + 2] = 0;
		coords[SKELETON_POSITION_HAND_RIGHT * 3 + 0] = 4;
		coords[SKELETON_POSITION_HAND_RIGHT * 3 + 1] = 0;
		coords[SKELETON_POSITION_HAND_RIGHT * 3 + 2] = 0;
	}
	else if (poseID == POSE_2)
	{
		coords[SKELETON_POSITION_SPINE * 3 + 0] = 0;
		coords[SKELETON_POSITION_SPINE * 3 + 1] = -1;
		coords[SKELETON_POSITION_SPINE * 3 + 2] = 0;

		coords[SKELETON_POSITION_SHOULDER_CENTER * 3 + 0] = 0;
		coords[SKELETON_POSITION_SHOULDER_CENTER * 3 + 1] = 0;
		coords[SKELETON_POSITION_SHOULDER_CENTER * 3 + 2] = 0;

		coords[SKELETON_POSITION_SHOULDER_LEFT * 3 + 0] = -1;
		coords[SKELETON_POSITION_SHOULDER_LEFT * 3 + 1] = 0;
		coords[SKELETON_POSITION_SHOULDER_LEFT * 3 + 2] = 0;
		coords[SKELETON_POSITION_ELBOW_LEFT * 3 + 0] = -2;
		coords[SKELETON_POSITION_ELBOW_LEFT * 3 + 1] = -1;
		coords[SKELETON_POSITION_ELBOW_LEFT * 3 + 2] = 0;
		coords[SKELETON_POSITION_WRIST_LEFT * 3 + 0] = -1;
		coords[SKELETON_POSITION_WRIST_LEFT * 3 + 1] = -2;
		coords[SKELETON_POSITION_WRIST_LEFT * 3 + 2] = 0;
		coords[SKELETON_POSITION_HAND_LEFT * 3 + 0] = -1;
		coords[SKELETON_POSITION_HAND_LEFT * 3 + 1] = -3;
		coords[SKELETON_POSITION_HAND_LEFT * 3 + 2] = 0;

		coords[SKELETON_POSITION_SHOULDER_RIGHT * 3 + 0] = 1;
		coords[SKELETON_POSITION_SHOULDER_RIGHT * 3 + 1] = 0;
		coords[SKELETON_POSITION_SHOULDER_RIGHT * 3 + 2] = 0;
		coords[SKELETON_POSITION_ELBOW_RIGHT * 3 + 0] = 2;
		coords[SKELETON_POSITION_ELBOW_RIGHT * 3 + 1] = -1;
		coords[SKELETON_POSITION_ELBOW_RIGHT * 3 + 2] = 0;
		coords[SKELETON_POSITION_WRIST_RIGHT * 3 + 0] = 1;
		coords[SKELETON_POSITION_WRIST_RIGHT * 3 + 1] = -2;
		coords[SKELETON_POSITION_WRIST_RIGHT * 3 + 2] = 0;
		coords[SKELETON_POSITION_HAND_RIGHT * 3 + 0] = 1;
		coords[SKELETON_POSITION_HAND_RIGHT * 3 + 1] = -3;
		coords[SKELETON_POSITION_HAND_RIGHT * 3 + 2] = 0;
	}
}

void CKinectController::MoveAvatar(const float* coords)
{
	if (!coords)
	{
		return;
	}

	if (m_isSeated)
	{
		return;
	}

	CVector3D bipPosition(coords[SKELETON_POSITION_SPINE * 3], coords[SKELETON_POSITION_SPINE * 3 + 1],
		coords[SKELETON_POSITION_SPINE * 3 + 2]);

	if (m_lastBipPosition != CVector3D(0, 0, 0))
	if (m_sao->GetObject3D())
	{
		const float scaleKoef = AVATAR_MOVING_SCALE;

		CVector3D faceVector = m_sao->GetRotatedFaceVector();
		float deltaX = -(bipPosition.z - m_lastBipPosition.z) * scaleKoef;
		float deltaY = -(bipPosition.x - m_lastBipPosition.x) * scaleKoef;
		float deltaZ = -(bipPosition.y - m_lastBipPosition.y) * scaleKoef;
		faceVector.Normalize();

		CVector3D avatarCoords = m_sao->GetObject3D()->GetCoords();

		if (!m_isKinectDeskUsed)
		{			
			avatarCoords += faceVector * deltaX;

			faceVector.RotateZ(90);
			avatarCoords += faceVector * deltaY;
		}
		else if (m_kinectDeskStart != m_kinectDeskEnd)
		{
			CVector3D leftCorner = CGlobalKinect::GetInstance()->GetDeskLeftCorner();
			CVector3D rightCorner = CGlobalKinect::GetInstance()->GetDeskRightCorner();			
		
			float scale = (bipPosition.x - leftCorner.x) / (leftCorner.x - rightCorner.x);
			CVector3D shiftX = (m_kinectDeskEnd - m_kinectDeskStart) * scale;
			
			float middleZ = (leftCorner.z + rightCorner.z) / 2;
			scale = (bipPosition.z - middleZ) / (leftCorner.x - rightCorner.x);
			CVector3D shiftY(0, 0, 0);
			if (scale < 0)
			{
				shiftY = (m_kinectDeskEnd - m_kinectDeskStart);
				shiftY.RotateZ(-90);
				shiftY *= scale;
			}

			CVector3D addY = (m_kinectDeskEnd - m_kinectDeskStart);
			addY.RotateZ(90);
			addY.Normalize();
			addY *= DISTANCE_FROM_DESK;
			
			CVector3D res = m_kinectDeskEnd + shiftX + shiftY + addY;
		
			avatarCoords.x = res.x;
			avatarCoords.y = res.y;
		}

		avatarCoords.z -= deltaZ;
		
		m_sao->GetObject3D()->SetCoords(avatarCoords);
	}

	m_lastBipPosition = bipPosition;
}

void CKinectController::RotateAvatar(const float* coords, IKinect* kinect, float& angle)
{
	if (!coords)
	{
		return;
	}

	if (m_isSeated)
	{
		return;
	}

	CVector3D hipLeftPosition(coords[SKELETON_POSITION_HIP_LEFT * 3 + 2], coords[SKELETON_POSITION_HIP_LEFT * 3 + 0],
		coords[SKELETON_POSITION_HIP_LEFT * 3 + 1]);
	CVector3D hipRightPosition(coords[SKELETON_POSITION_HIP_RIGHT * 3 + 2], coords[SKELETON_POSITION_HIP_RIGHT * 3 + 0],
		coords[SKELETON_POSITION_HIP_RIGHT * 3 + 1]);
	CVector3D direction = hipRightPosition - hipLeftPosition;
	direction.z = 0;

	if (m_lastDirection != CVector3D(0, 0, 0))
	if (m_sao->GetObject3D())
	{
		const float mulKoef = 1.75f;
		CQuaternion q(m_lastDirection, direction, mulKoef);
		q.Normalize();
		
		if (!m_isKinectDeskUsed)
		{
			CVector3D axis;		
			q.Extract(axis, angle);
			angle *= 57.0f / mulKoef;
			if (axis.z < 0)
			{
				angle *= -1.0f;
			}

			if (angle < -MAX_AVATAR_ANGLE)
			{
				m_isRotatedToBack = true;
			}
			else if (angle > MAX_AVATAR_ANGLE)
			{
				m_isRotatedToBack = true;
			}

			if (kinect->GetBipsTrackingQuality() > 0.92f)
			if (kinect->GetHeadTrackingQuality() >= 0.9f)
			if (IsLeftArmTrackedNow(kinect, (float*)coords))
			if (IsRightArmTrackedNow(kinect, (float*)coords))
			if ((angle >= -MAX_AVATAR_ANGLE + 1) && (angle <= -MAX_AVATAR_ANGLE + AVATAR_ANGLE_DANGER_RANGE))
			{
				m_isRotatedToBack = false;
			}

			if (kinect->GetBipsTrackingQuality() > 0.92f)
			if (kinect->GetHeadTrackingQuality() >= 0.9f)
			if (IsLeftArmTrackedNow(kinect, (float*)coords))
			if (IsRightArmTrackedNow(kinect, (float*)coords))
			if ((angle >= MAX_AVATAR_ANGLE - AVATAR_ANGLE_DANGER_RANGE) && (angle <= MAX_AVATAR_ANGLE - 1))
			{
				m_isRotatedToBack = false;
			}

			if (m_isRotatedToBack)
			{
				
				return;
			}
		}

		CQuaternion rot = m_qlobalRotation;
		rot *= q;
		rot.Normalize();
		CRotationPack rotation;
		rotation.SetAsDXQuaternion((float)rot.x, (float)rot.y, (float)rot.z, (float)rot.w);
		m_sao->GetObject3D()->SetRotation(&rotation);

		m_shiftRotation = q;
	}
}

CQuaternion CKinectController::GetBodyPartQuaternion(float* coords, int start, int center, int end)
{
	CVector3D armLeft = KinectCoordsToCal3D(CVector3D(coords[start * 3] - coords[center * 3],
			coords[start * 3 + 1] - coords[center * 3 + 1],
			coords[start * 3 + 2] - coords[center * 3 + 2]));
	CVector3D armLeft2 = KinectCoordsToCal3D(CVector3D(coords[center * 3] - coords[end * 3],
			coords[center * 3 + 1] - coords[end * 3 + 1],
			coords[center * 3 + 2] - coords[end * 3 + 2]));
	CQuaternion q(armLeft, armLeft2);

	return q;
}

bool CKinectController::IsLeftArmTrackedNow(IKinect* kinect, float* coords)
{
	if (kinect->GetLeftArmTrackingQuality() < 0.8f)
	{
		return false;
	}

	CQuaternion q = GetBodyPartQuaternion(coords, SKELETON_POSITION_SHOULDER_CENTER, SKELETON_POSITION_SHOULDER_LEFT, SKELETON_POSITION_ELBOW_LEFT);
	float x, y, z;
	q.GetEulerAnglesInDegrees(x, y, z);
	if (y < -20)
	{
		return false;
	}
	
	return true;
}

bool CKinectController::IsLeftArmEndTrackedNow(IKinect* /*kinect*/, float* coords)
{
	CQuaternion q = GetBodyPartQuaternion(coords, SKELETON_POSITION_ELBOW_LEFT, SKELETON_POSITION_WRIST_LEFT, SKELETON_POSITION_HAND_LEFT);
	float x, y, z;
	q.GetEulerAnglesInDegrees(x, y, z);
	if (x <= -10)
	{
		return false;
	}
	if (x >= 10)
	{
		return false;
	}
	if (y <= -5)
	{
		return false;
	}
	if (y >= 7)
	{
		return false;
	}
	
	return true;
}

bool CKinectController::IsRightArmTrackedNow(IKinect* kinect, float* coords)
{
	if (kinect->GetRightArmTrackingQuality() < 0.8f)
	{
		return false;
	}

	CQuaternion q = GetBodyPartQuaternion(coords, SKELETON_POSITION_SHOULDER_CENTER, SKELETON_POSITION_SHOULDER_RIGHT, SKELETON_POSITION_ELBOW_RIGHT);
	float x, y, z;
	q.GetEulerAnglesInDegrees(x, y, z);
	if (y > 20)
	{
		return false;
	}

	return true;
}

bool CKinectController::IsRightArmEndTrackedNow(IKinect* /*kinect*/, float* coords)
{
	CQuaternion q = GetBodyPartQuaternion(coords, SKELETON_POSITION_ELBOW_RIGHT, SKELETON_POSITION_WRIST_RIGHT, SKELETON_POSITION_HAND_RIGHT);
	float x, y, z;
	q.GetEulerAnglesInDegrees(x, y, z);
	if (x <= -10)
	{
		return false;
	}
	if (x >= 10)
	{
		return false;
	}
	if (y <= -5)
	{
		return false;
	}
	if (y >= 7)
	{
		return false;
	}
	
	return true;
}


bool CKinectController::IsRightFootTrackedNow(IKinect* kinect, float* coords, bool isSeated)
{
	if ((isSeated) || (kinect->GetRightFootTrackingQuality() < 0.7f))
	{
		return false;
	}

	CQuaternion q = GetBodyPartQuaternion(coords, SKELETON_POSITION_HIP_RIGHT, SKELETON_POSITION_KNEE_RIGHT, SKELETON_POSITION_ANKLE_RIGHT);
	float x, y, z;
	q.GetEulerAnglesInDegrees(x, y, z);
	if (x > 9)
	{
		return false;
	}

	CVector3D footMiddlePoint = KinectCoordsToCal3D(CVector3D(coords[SKELETON_POSITION_KNEE_RIGHT * 3],
		coords[SKELETON_POSITION_KNEE_RIGHT * 3 + 1],
		coords[SKELETON_POSITION_KNEE_RIGHT * 3 + 2]));

	CVector3D footBottomPoint = KinectCoordsToCal3D(CVector3D(coords[SKELETON_POSITION_ANKLE_RIGHT * 3],
		coords[SKELETON_POSITION_ANKLE_RIGHT * 3 + 1],
		coords[SKELETON_POSITION_ANKLE_RIGHT * 3 + 2]));

	if (footMiddlePoint.z < footBottomPoint.z)
	{
		return false;
	}

	return true;
}

bool CKinectController::IsLeftFootTrackedNow(IKinect* kinect, float* coords, bool isSeated)
{
	if ((isSeated) || (kinect->GetLeftFootTrackingQuality() < 0.7f))
	{
		return false;
	}

	CQuaternion q = GetBodyPartQuaternion(coords, SKELETON_POSITION_HIP_LEFT, SKELETON_POSITION_KNEE_LEFT, SKELETON_POSITION_ANKLE_LEFT);
	float x, y, z;
	q.GetEulerAnglesInDegrees(x, y, z);
	if (x > 9)
	{
		return false;
	}

	CVector3D footMiddlePoint = KinectCoordsToCal3D(CVector3D(coords[SKELETON_POSITION_KNEE_LEFT * 3],
		coords[SKELETON_POSITION_KNEE_LEFT * 3 + 1],
		coords[SKELETON_POSITION_KNEE_LEFT * 3 + 2]));

	CVector3D footBottomPoint = KinectCoordsToCal3D(CVector3D(coords[SKELETON_POSITION_ANKLE_LEFT * 3],
		coords[SKELETON_POSITION_ANKLE_LEFT * 3 + 1],
		coords[SKELETON_POSITION_ANKLE_LEFT * 3 + 2]));

	if (footMiddlePoint.z < footBottomPoint.z)
	{
		return false;
	}

	return true;
}

bool CKinectController::IsHeadTrackedNow(IKinect* kinect, float* coords, bool isSeated)
{
	if (kinect->GetHeadTrackingQuality() < 0.9f)
	{
		return false;
	}

	if (!isSeated)
	{
		CQuaternion q = GetBodyPartQuaternion(coords, SKELETON_POSITION_SPINE, SKELETON_POSITION_SHOULDER_CENTER, SKELETON_POSITION_HEAD);
		float x, y, z;
		q.GetEulerAnglesInDegrees(x, y, z);

		if (x <= -30)
		{
			return false;
		}

		if (x > 20)
		{
			return false;
		}

		if (z <= -10)
		{
			return false;
		}

		if (z > 10)
		{
			return false;
		}
	}

	return true;
}

bool CKinectController::IsTooNear(float* coords)
{
	if (!coords)
	{
		return true;
	}

	if (!m_isSeated)
	if (coords[SKELETON_POSITION_SPINE * 3 + 2] < MIN_KINECT_Z)
	{
		return true;
	}
		
	if (coords[SKELETON_POSITION_SHOULDER_LEFT * 3 + 2] +
		coords[SKELETON_POSITION_SHOULDER_RIGHT * 3 + 2] < MIN_SEATED_KINECT_Z * 2.0f)
	{		
		return true;
	}
 
	return false;
}

bool CKinectController::IsTooNearForFT(float* coords)
{
	if (!coords)
	{
		return false;
	}

	if (!m_isSeated)
	{
		if (coords[SKELETON_POSITION_HEAD * 3 + 2] < MIN_KINECT_Z)
		{
			return true;
		}
	}
	else
	{
		if (coords[SKELETON_POSITION_HEAD * 3 + 2] < MIN_SEATED_KINECT_Z)
		{
			return true;
		}
	}

	return false;
}

void CKinectController::UpdateImpl(float* coords)
{	
	if ((!m_sao) || (!m_sao->GetKinectObject()))
	{
		return;
	}

	if (!coords)
	{
		return;
	}

	m_gestureDetector.ProcessPoints(coords);		
	
	m_areMarkersNotFound = false;

	if (g->ne.time - m_enableKinectTime < ENABLE_KINECT_DELAY)
	{
		return;
	}

//-------------------------------------------------------------

	IKinect* kinect = m_sao->GetKinectObject();	

	m_sao->GetKinectObject()->GetSpecificPixelsCount();

	float headRotation[3] = {0, 0, 0};
	float headTranslation[3] = {0, 0, 0};
	float auCoeffs[6] = {0};
	unsigned int auCoeffsCount = 0;
	kinect->GetHeadRotation(&headRotation[0],  &headTranslation[0]);
	kinect->GetAUCoeffs((float *) auCoeffs, &auCoeffsCount);

	if(kinect->IsFTSuccessful())
	{
		CMimicController* mimicC = m_sao->GetMimicController();
		mimicC->SetAUCoeffs(&auCoeffs[0]);
	}

	if ((m_isKinectDeskUsed) && (m_kinectVideo))	
	{
		int count = MARKER_PIXELS_COUNT_THRESHOLD + 1;
		if (m_markerType != MARKER_NO_MARKER)
		{
			count = m_sao->GetKinectObject()->GetSpecificPixelsCount();
		}


		int i;
		for (i = 0; i < COLOR_MARKER_FRAMES - 1; i++)
		{
			m_markersPixelCountByFrame[i] = m_markersPixelCountByFrame[i + 1];
		}

		m_markersPixelCountByFrame[COLOR_MARKER_FRAMES - 1] = count;

		count = 0;
		for (i = 0; i < COLOR_MARKER_FRAMES; i++)
		{
			count += m_markersPixelCountByFrame[i];
		}
        count /= COLOR_MARKER_FRAMES;

		m_isRotatedToBack = ((unsigned int)count < MARKER_PIXELS_COUNT_THRESHOLD);
		if (m_isRotatedToBack)
		{
			m_areMarkersNotFound = true;
			m_headTracked = true;
			m_leftArmTracked = true;
			m_rightArmTracked = true;
			m_leftFootTracked = true;
			m_rightFootTracked = true;
			m_bipTracked = true;
		}
	}

	if ((m_isKinectDeskUsed) && (m_isRotatedToBack))
	{
		return;
	}
//-------------------------------------------------------------

	float angle;
	if ((!m_isSeated) && (kinect->GetBipTrackingQuality() > 0.5f))
	{
		MoveAvatar(coords);
		RotateAvatar(coords, kinect, angle);

		if (m_isRotatedToBack)
		{
			return;
		}

		SetRotationFromKinect(coords, SKELETON_POSITION_SPINE, SKELETON_POSITION_SHOULDER_CENTER, m_pelvis, false, 0);
		m_bipTracked = true;
	}
	else if (m_bipTracked)
	{		
		m_bipTracked = false;
	}	

	if (m_isRotatedToBack)
	{
		return;
	}
	
	if (m_isSeated)
	{
		UNLOCK_BONE(m_pelvis);
		CALC_STATE(m_pelvis);
	}

	CALC_STATE(m_spine);
	CALC_STATE(m_spine1);
	CALC_STATE(m_spine2);
	CALC_STATE(m_neckBone);
	CALC_STATE(m_neckBone2);
	
	float* crd = NULL;
	if (IsHeadTrackedNow(kinect, coords, m_isSeated))
	{
		crd = coords;		
		m_headTracked = true;
	}
	else if (m_headTracked)
	{
		m_headNotTrackedStartTime = g->ne.time;		
		m_headTracked = false;
		crd = m_lastRightHandEndCoords;		
	}
	else if (g->ne.time - m_headNotTrackedStartTime > MAX_UNDEFINED_TIME)
	{
		UnlockHeadBones();		
	}
	else
	{
		crd = m_lastRightCoords;		
	}

	if (m_isKinectDeskUsed)
	{
		UnlockHeadBones();		
		crd = NULL;
	}

	CalLookController * lookc = m_sao->GetLookController();
	if(lookc && lookc->IsEnabled())
	{
		if(kinect->IsFTSuccessful()/* && !IsTooNearForFT(coords)*/) //IsTooNearForFT don't work with KinectEmulator
		{
			lookc->SetRotations(&headRotation[0], &headTranslation[0]);
		}
	}
	else
	{
		if (crd)
		{
			SetRotationFromKinect(crd, SKELETON_POSITION_SHOULDER_CENTER, SKELETON_POSITION_HEAD, m_headBone, false);
			for (int k = 0; k < 3; k++)
			{
				m_lastRightHandEndCoords[SKELETON_POSITION_SHOULDER_CENTER * 3 + k] = coords[SKELETON_POSITION_SHOULDER_CENTER * 3 + k];
				m_lastRightHandEndCoords[SKELETON_POSITION_HEAD * 3 + k] = coords[SKELETON_POSITION_HEAD * 3 + k];
			}
		}	
	}
	
	crd = NULL;
	if (IsLeftArmTrackedNow(kinect, coords))
	{				
		crd = coords;
		m_leftArmTracked = true;
	}
	else if (m_leftArmTracked)
	{		
		m_leftArmNotTrackedStartTime = g->ne.time;
		m_leftArmTracked = false;		
		crd = m_lastRightCoords;
	}
	else if (g->ne.time - m_leftArmNotTrackedStartTime > MAX_UNDEFINED_TIME)
	{
		UnlockLeftArmBones();		
	}
	else
	{
		crd = m_lastRightCoords;
	}

	if (crd)
	{
		CALC_STATE(m_shoulderCenterLeft);
		SetRotationFromKinect(crd, SKELETON_POSITION_SHOULDER_LEFT , SKELETON_POSITION_ELBOW_LEFT, m_shoulderLeft, false);
		SetRotationFromKinect(crd, SKELETON_POSITION_ELBOW_LEFT , SKELETON_POSITION_WRIST_LEFT , m_elbowLeft, true);
		if (IsLeftArmEndTrackedNow(kinect, coords))
		{
			//SetRotationFromKinect(crd, SKELETON_POSITION_WRIST_LEFT, SKELETON_POSITION_HAND_LEFT , m_handLeft, true);
			for (int k = 0; k < 3; k++)
			{
				m_lastRightHandEndCoords[SKELETON_POSITION_WRIST_LEFT * 3 + k] = coords[SKELETON_POSITION_WRIST_LEFT * 3 + k];
				m_lastRightHandEndCoords[SKELETON_POSITION_HAND_LEFT * 3 + k] = coords[SKELETON_POSITION_HAND_LEFT * 3 + k];
			}
		}
		else if (IsRightCoordsForLeftHandEndExists())
		{
			//SetRotationFromKinect(m_lastRightHandEndCoords, SKELETON_POSITION_WRIST_LEFT, SKELETON_POSITION_HAND_LEFT , m_handLeft, true);
		}
		else
		{
			//SetRotationFromKinect(crd, SKELETON_POSITION_WRIST_LEFT, SKELETON_POSITION_HAND_LEFT , m_handLeft, true);
		}
		if (m_twistLeft)
		{
			SetRotationFromKinect(crd, SKELETON_POSITION_ELBOW_LEFT, SKELETON_POSITION_WRIST_LEFT , m_twistLeft, true);
		}
	}	

	crd = NULL;
	if (IsLeftFootTrackedNow(kinect, coords, m_isSeated))
	{				
		crd = coords;
		m_leftFootTracked = true;
	}
	else if (m_leftFootTracked)
	{	
		m_leftFootTracked = false;
		m_leftFootNotTrackedStartTime = g->ne.time;
		crd = m_lastRightCoords;
	}
	else if (g->ne.time - m_leftFootNotTrackedStartTime > MAX_UNDEFINED_TIME)
	{		
		UnlockLeftFootBones();		
	}
	else
	{		
		crd = m_lastRightCoords;		
	}

	if (crd)
	{
		SetRotationFromKinect(crd, SKELETON_POSITION_HIP_LEFT, SKELETON_POSITION_KNEE_LEFT, m_thighLeft, false);
		SetRotationFromKinect(crd, SKELETON_POSITION_KNEE_LEFT, SKELETON_POSITION_ANKLE_LEFT, m_calfLeft, true);
	}

	crd = NULL;		
	if (IsRightArmTrackedNow(kinect, coords))
	{
		crd = coords;
		m_rightArmTracked = true;
	}
	else if (m_rightArmTracked)
	{		
		m_rightArmNotTrackedStartTime = g->ne.time;
		m_rightArmTracked = false;
		crd = m_lastRightCoords;
	}
	else if (g->ne.time - m_rightArmNotTrackedStartTime > MAX_UNDEFINED_TIME)
	{		
		UnlockRightArmBones();		
	}
	else
	{
		crd = m_lastRightCoords;		
	}

	if (crd)
	{
		CALC_STATE(m_shoulderCenterRight);		
		SetRotationFromKinect(crd, SKELETON_POSITION_SHOULDER_RIGHT , SKELETON_POSITION_ELBOW_RIGHT, m_shoulderRight, false);
		SetRotationFromKinect(crd, SKELETON_POSITION_ELBOW_RIGHT , SKELETON_POSITION_WRIST_RIGHT , m_elbowRight, true);
		if (IsRightArmEndTrackedNow(kinect, coords))
		{
			//SetRotationFromKinect(crd, SKELETON_POSITION_WRIST_RIGHT, SKELETON_POSITION_HAND_RIGHT , m_handRight, true);
			for (int k = 0; k < 3; k++)
			{
				m_lastRightHandEndCoords[SKELETON_POSITION_WRIST_RIGHT * 3 + k] = coords[SKELETON_POSITION_WRIST_RIGHT * 3 + k];
				m_lastRightHandEndCoords[SKELETON_POSITION_HAND_RIGHT * 3 + k] = coords[SKELETON_POSITION_HAND_RIGHT * 3 + k];
			}
		}
		else if (IsRightCoordsForRightHandEndExists())
		{
			//SetRotationFromKinect(m_lastRightHandEndCoords, SKELETON_POSITION_WRIST_RIGHT, SKELETON_POSITION_HAND_RIGHT , m_handRight, true);			
		}
		else
		{
			//SetRotationFromKinect(crd, SKELETON_POSITION_WRIST_RIGHT, SKELETON_POSITION_HAND_RIGHT , m_handRight, true);			
		}

		if (m_twistRight)
		{
			SetRotationFromKinect(crd, SKELETON_POSITION_ELBOW_RIGHT, SKELETON_POSITION_WRIST_RIGHT , m_twistRight, true);
		}
	}
	
	crd = NULL;
	if (IsRightFootTrackedNow(kinect, coords, m_isSeated))
	{				
		m_rightFootTracked = true;
		crd = coords;
	}
	else if (m_rightFootTracked)
	{			
		m_rightFootNotTrackedStartTime = g->ne.time;
		m_rightFootTracked = false;
		crd = m_lastRightCoords;
	}
	else if (g->ne.time - m_rightFootNotTrackedStartTime > MAX_UNDEFINED_TIME)
	{		
		UnlockRightFootBones();		
	}
	else
	{		
		crd = m_lastRightCoords;
	}

	if (crd)
	{
		SetRotationFromKinect(crd, SKELETON_POSITION_HIP_RIGHT, SKELETON_POSITION_KNEE_RIGHT, m_thighRight, false);
		SetRotationFromKinect(crd, SKELETON_POSITION_KNEE_RIGHT, SKELETON_POSITION_ANKLE_RIGHT, m_calfRight, true);
	}
}

void CKinectController::SetRotationFromKinect(const float* coords, const int firstPntIndex, const int secondPntIndex, CalBone* bone, bool isEndBone, int maxChildCount)
{	
	if ((!coords) || (!bone))
	{
		return;
	}

	m_lastRightCoords[firstPntIndex * 3 + 0] = coords[firstPntIndex * 3 + 0];
	m_lastRightCoords[firstPntIndex * 3 + 1] = coords[firstPntIndex * 3 + 1];
	m_lastRightCoords[firstPntIndex * 3 + 2] = coords[firstPntIndex * 3 + 2];

	m_lastRightCoords[secondPntIndex * 3 + 0] = coords[secondPntIndex * 3 + 0];
	m_lastRightCoords[secondPntIndex * 3 + 1] = coords[secondPntIndex * 3 + 1];
	m_lastRightCoords[secondPntIndex * 3 + 2] = coords[secondPntIndex * 3 + 2];

	bone->lockBone();

#define PI 3.14159265f
	// поворот вектора из кинекта в плоскость аватара
	CVector3D vStart = KinectCoordsToCal3D( CVector3D(coords[firstPntIndex*3+0],  coords[firstPntIndex*3+1], coords[firstPntIndex*3+2]));
	CVector3D vEnd = KinectCoordsToCal3D( CVector3D(coords[secondPntIndex*3+0],  coords[secondPntIndex*3+1], coords[secondPntIndex*3+2]));
	CVector3D vDirection = vEnd - vStart; vDirection.Normalize();
		
	// Изначально Core модуль смотрит в направлении оси -Y. Надо развернуть в положительном X
	CalQuaternion rtnCalCoreToModel( CalVector(0,0,1), -PI/2);

	CalQuaternion boneBoneSpace = /*(bone != m_pelvis) ?*/ bone->getCoreBone()->getRotationBoneSpace()/* : CalQuaternion(0, 0, 1, 0)*/;
	boneBoneSpace.conjugate();
	boneBoneSpace *= rtnCalCoreToModel;
	boneBoneSpace.Normalize();

	CalVector coreBoneDir(1, 0, 0);
	coreBoneDir *= boneBoneSpace;
	 
	CalVector boneDir(vDirection.x,vDirection.y,vDirection.z); boneDir.normalize();
	CalQuaternion r(coreBoneDir, boneDir);
	r.Normalize();

	boneBoneSpace *= r;
	boneBoneSpace.Normalize();

	bone->setRotationAbsolute( boneBoneSpace);
	bone->calculateState(true, isEndBone, maxChildCount);	
}

void CKinectController::Enable()
{
	if (!m_isEnabled)
	{
		m_enableKinectTime = g->ne.time;
		m_qlobalRotation.Set(0, 0, 0, 1);
		m_lastDirection.Set(0, 1, 0);
		if (m_isKinectDeskUsed)
		{
			CRotationPack rotation;
			g->lw.WriteLn("m_kinectDeskStart.x = ", m_kinectDeskStart.x);
			g->lw.WriteLn("m_kinectDeskStart.y = ", m_kinectDeskStart.y);
			g->lw.WriteLn("m_kinectDeskStart.z = ", m_kinectDeskStart.z);
			g->lw.WriteLn("m_kinectDeskEnd.x = ", m_kinectDeskEnd.x);
			g->lw.WriteLn("m_kinectDeskEnd.y = ", m_kinectDeskEnd.y);
			g->lw.WriteLn("m_kinectDeskEnd.z = ", m_kinectDeskEnd.z);
			CVector3D s(m_kinectDeskStart-m_kinectDeskEnd);
			s.Normalize();
			CQuaternion q(CVector3D(0, 1, 0), s);
			q.Normalize();
			rotation.SetAsDXQuaternion(q.x, q.y, q.z, q.w);
			if ((m_sao) && (m_sao->GetObject3D()) && (!m_isSeated))
			{
				m_sao->GetObject3D()->SetRotation(&rotation);			
			}			
			m_qlobalRotation = q;
		}
		else if ((m_sao) && (m_sao->GetObject3D()))
		{
			m_qlobalRotation = m_sao->GetObject3D()->GetRotation()->GetAsDXQuaternion();
		}
	}
	m_isEnabled = true;
}

void CKinectController::Disable()
{
	m_isEnabled = false;
	UnlockBones();		
}

bool CKinectController::IsEnabled()const 
{
	return m_isEnabled;
}

void CKinectController::SetSeatedMode(const bool isSeated)
{
	if ((!m_sao) || (!m_sao->GetKinectObject()))
	{
		return;
	}

	m_isSeated = isSeated;
	m_sao->GetKinectObject()->SetSeatedMode(isSeated);		
}

bool CKinectController::IsSeatedMode()const
{
	return m_isSeated;
}

void CKinectController::DumpKinectInfoToLog()
{
	if ((!m_sao) || (!m_sao->GetKinectObject()))
	{
		return;
	}

	IKinect* kinect = m_sao->GetKinectObject();
	g->lw.WriteLn("BIP tracking quality: ", kinect->GetBipTrackingQuality());
	g->lw.WriteLn("Head tracking quality: ", kinect->GetHeadTrackingQuality());
	g->lw.WriteLn("Left arm tracking quality: ", kinect->GetLeftArmTrackingQuality());
	g->lw.WriteLn("Right arm tracking quality: ", kinect->GetRightArmTrackingQuality());
	g->lw.WriteLn("Left foot tracking quality: ", kinect->GetLeftFootTrackingQuality());
	g->lw.WriteLn("Right foot tracking quality: ", kinect->GetRightFootTrackingQuality());
	float* coords = kinect->GetSkeletonXYZ();
	g->lw.WriteLn("Is too near: ", IsTooNear(coords));
	g->lw.WriteLn("min Z: ", coords[SKELETON_POSITION_SPINE * 3 + 2]);
	g->lw.WriteLn("skeleton found: ", kinect->IsFoundSkeleton() ? "true" : "false");	
}

bool CKinectController::AreTwoSkeletonsFound()
{
	if ((!m_sao) || (!m_sao->GetKinectObject()))
	{
		return false;
	}

	return m_sao->GetKinectObject()->IsTwoSkeletonsFound();
}

bool CKinectController::AreMarkersNotFound()
{
	return m_areMarkersNotFound;
}

void CKinectController::SetMarkerType(const int markerType)
{
	if ((!m_sao) || (!m_sao->GetKinectObject()))
	{
		return;
	}

	m_markerType = markerType;
	m_sao->GetKinectObject()->SetSpecificPixelsChannel( static_cast<char>(m_markerType) );
}

void CKinectController::EnableFaceTracking()
{
	if ((!m_sao) || (!m_sao->GetKinectObject()))
	{
		return;
	}

	m_sao->GetKinectObject()->EnableFaceTracking();
	m_sao->InitLookControllerIfNeeded();
	m_sao->GetLookController()->Enable(true);
}

void CKinectController::DisableFaceTracking()
{
	if ((!m_sao) || (!m_sao->GetKinectObject()))
	{
		return;
	}

	m_sao->GetKinectObject()->DisableFaceTracking();
	m_sao->InitLookControllerIfNeeded();
	m_sao->GetLookController()->Enable(false);
}

CKinectController::~CKinectController()
{
	m_ppMan.EndWork();	
}