
#pragma once 

#include "IKinect.h"
#include "KinectPointer.h"
#include "KinectGestureDetector.h"
#include "KinectPowerPointManipulator.h"

#define MARKER_BLUE			0
#define MARKER_GREEN		1
#define MARKER_RED			2
#define MARKER_NO_MARKER	3

#define KINECT_JOINT_COUNT					20
#define SKELETON_SIZE						66 * 4 + 4
#define TWO_SKELETONS_SIZE					(SKELETON_SIZE + 6 * 4 * 3)
#define KINECT_SYNCHDATA_SIZE				(TWO_SKELETONS_SIZE + 4 * 2 * 3 + 12 * 4 + 2)//(TWO_SKELETONS_SIZE + 4 * 2 * 3)

#define COLOR_MARKER_FRAMES					2

class CSkeletonAnimationObject;

class CKinectController
{
public:
	CKinectController(CSkeletonAnimationObject* sao);
	~CKinectController();

	bool Update();

	bool GetSynchData(unsigned char* data, int* size);

	void Enable();
	void Disable();
	bool IsEnabled()const;

	void DumpKinectInfoToLog();

	void SetSeatedMode(const bool isSeated);
	bool IsSeatedMode()const;

	bool IsAnythingTracked();
	bool IsHeadTracked();
	bool IsHipTracked();
	bool IsLeftArmTracked();
	bool IsRightArmTracked();
	bool IsLeftFootTracked();
	bool IsRightFootTracked();
	bool AreTwoSkeletonsFound();
	bool AreMarkersNotFound();

	void SetRotatedToBack(bool isEnabled);
	void SetSecondaryLastRightCoords(float* coords);

	void SetKinectDeskPoints(CVector3D start, CVector3D end, C3DObject* obj);
	bool IsKinectDeskModeEnabled()const;

	CVector3D GetExtendedHandDirection(CVector3D& pos, CVector3D& direction);

	void SetMarkerType(const int markerType);
	void EnableFaceTracking();
	void DisableFaceTracking();

	void ReloadSettings();

	void DisableKinectDesk();

	void OnHandsNotRecognized();

private:
	void UpdateImpl(float* coords);
	void InitBonePointers();
	void MoveAvatar(const float* coords);
	void RotateAvatar(const float* coords, IKinect* kinect, float& angle);
	void PatchCoordsForPredefinedPose(float* coords, const int poseID);
	void SetRotationFromKinect(const float* coords, const int firstPntIndex, const int secondPntIndex, CalBone* bone, bool isEndBone, int maxChildCount = -1);
	bool IsTooNear(float* coords);
	bool IsTooNearForFT(float* coords);

	void UnlockRightFootBones();
	void UnlockLeftFootBones();
	void UnlockRightArmBones();
	void UnlockLeftArmBones();
	void UnlockHeadBones();
	void UnlockFootBones();
	void UnlockArmBones();
	void UnlockBones();

	void UpdateRightFootBones();
	void UpdateLeftFootBones();
	void UpdateRightArmBones();
	void UpdateLeftArmBones();
	void UpdateHeadBones();

	bool IsRightCoordsForRightHandEndExists();
	bool IsRightCoordsForLeftHandEndExists();

	CVector3D KinectCoordsToCal3D( CVector3D aKinectVector);

	CQuaternion GetBodyPartQuaternion(float* coords, int start, int center, int end);

	bool IsLeftArmTrackedNow(IKinect* kinect, float* coords);
	bool IsLeftArmEndTrackedNow(IKinect* kinect, float* coords);
	bool IsRightArmTrackedNow(IKinect* kinect, float* coords);
	bool IsRightArmEndTrackedNow(IKinect* kinect, float* coords);
	bool IsRightFootTrackedNow(IKinect* kinect, float* coords, bool isSeated);
	bool IsLeftFootTrackedNow(IKinect* kinect, float* coords, bool isSeated);
	bool IsHeadTrackedNow(IKinect* kinect, float* coords, bool isSeated);

	CSkeletonAnimationObject* m_sao;

	CalBone* m_headBone;
	CalBone* m_neckBone;
	CalBone* m_neckBone2;
	CalBone* m_spine;
	CalBone* m_spine1;
	CalBone* m_spine2;
	CalBone* m_shoulderCenterLeft;
	CalBone* m_shoulderLeft;
	CalBone* m_elbowLeft;
	CalBone* m_handLeft;
	CalBone* m_twistLeft;
	CalBone* m_shoulderCenterRight;
	CalBone* m_shoulderRight;
	CalBone* m_elbowRight;
	CalBone* m_handRight;
	CalBone* m_twistRight;
	CalBone* m_thighLeft;
	CalBone* m_calfLeft;
	CalBone* m_footLeft;
	CalBone* m_thighRight;
	CalBone* m_calfRight;
	CalBone* m_footRight;
	CalBone* m_pelvis;

	CVector3D m_lastDirection;
	CVector3D m_lastBipPosition;

	CQuaternion m_qlobalRotation;
	CQuaternion m_kinectDeskRotation;

	bool m_isEnabled;
	bool m_isSeated;
	bool m_lastFrameTracked;
	bool m_bipTracked;
	bool m_headTracked;
	bool m_leftArmTracked;
	bool m_rightArmTracked;
	bool m_leftFootTracked;
	bool m_rightFootTracked;
	bool m_isKinectDeskUsed;
	bool m_areMarkersNotFound;

	__int64 m_bipNotTrackedStartTime;
	__int64 m_headNotTrackedStartTime;
	__int64 m_leftArmNotTrackedStartTime;
	__int64 m_rightArmNotTrackedStartTime;
	__int64 m_leftFootNotTrackedStartTime;
	__int64 m_rightFootNotTrackedStartTime;

	__int64 m_enableKinectTime;

	float m_lastRightCoords[KINECT_JOINT_COUNT * 3];
	float m_lastRightHandEndCoords[KINECT_JOINT_COUNT * 3];


	unsigned char* m_oldSynchData;

	bool m_isRotatedToBack;

	CVector3D m_kinectDeskStart;
	CVector3D m_kinectDeskEnd;
	CPlane3D* m_kinectDeskPlane;
	CVector3D m_kinectDeskPoints[4];

	C3DObject* m_kinectDeskObj;

	CQuaternion m_shiftRotation;

	CKinectVideoTexture* m_kinectVideo;

	CKinectGestureDetector m_gestureDetector;
	CKinectPowerPointManipulator m_ppMan;

	int m_markerType;

	int m_markersPixelCountByFrame[COLOR_MARKER_FRAMES];

	CKinectPointer m_kinectPointer;

	int m_otherColorKoef1[4];
	int m_otherColorKoef2[4];
};