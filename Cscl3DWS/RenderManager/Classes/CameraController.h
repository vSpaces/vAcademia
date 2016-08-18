
#pragma once

#include "camera.h"
#include "icontroller.h"
#include "CameraTarget.h"
#include "MouseController.h"
#include "CameraTrackController.h"

class nrmCamera;

#define SITTING_CAMERA_MODE_UNDEFINED			0
#define SITTING_CAMERA_MODE_A_LA_SECOND_LIFE	1
#define SITTING_CAMERA_MODE_NORMAL				2
#define SITTING_CAMERA_MODE_AMFITEATR			3

class CCameraController : public IController, public IChangesListener, public ISimpleEventsListener
{
	friend class nrmCamera;
public:
	CCameraController();
	virtual ~CCameraController();

	bool	IsActive();

	void	OnLMouseDown(const int x, const int y);
	void	OnLMouseUp(const int x, const int y);
	void	OnMouseMove(const int x, const int y);
	void	OnKeyDown(const unsigned char keycode, const unsigned char scancode, const bool isAlt, const bool isCtrl, const bool isShift);
	void	OnKeyUp(const unsigned char keycode, const bool isAlt, const bool isCtrl, const bool isShift);
	void	OnMouseWheel(const int delta);

	void	PlayCameraTrack();
	void	PlayCameraTrack(  void* apData, int auDataSize);
	void	StopTrackPlaying();

	void	Update(unsigned int	deltaTime);

	float	GetOffsetZ();

	void	SetFixedPosition(bool isFixedPosition);
	bool	IsFixedPosition()const;

	void	SetCamera(CCamera3D* camera);
	CCamera3D* GetCamera()const;

	void	SetCameraTarget(CCameraTarget* const cameraTarget);
	CCameraTarget* GetCameraTarget()const;

	void	OnMaxLinkDistanceChanged(float dist);
	float	GetMaxLinkDistance()const;

	void	OnChanged(int eventID);

	void	OnCameraChangedTo(CCameraController* newCameraController);

	bool	IsThirdView();
	bool	IsFirstView();

private:	
	void	SetMaxLinkDistance(float distance);

	void	RepairCamera();

	void	CheckBrokenCamera();
	void	CheckForEqualEyeAndLookAt();
	bool	CheckForCameraUnderground(const bool isOnlyCheck);
	bool	CheckForInvalidCamera(CVector3D& pos);

	void	OnViewTargetAttached();

	CVector3D GetDelta();
	float GetCameraReturnSpeed()const;
	CVector3D GetViewTargetPosition();

	bool	IsCameraChanged();

	float	GetDistanceFromCamera();
	CVector3D GetCameraDelta(float add);
	CVector3D GetCameraDeltaLookAt(float add);
	bool	IncDistanceFromCamera(float value);

	void	UpdateTargetPositions();

	void	Rotate(int aiDirection, float koef = 1.0f);
	void	Move(int aiDirection, int aiOffset = -1);

private:
	C3DObject*		m_viewTargetParent;
	C3DObject*		m_lastViewTarget;
	CCameraTarget*	m_cameraTarget;
	CCamera3D*		m_camera;

	__int64			m_lastUpdateTime;
	int				m_frameDeltaTime;
	int				m_viewTargetAttachedTime;

	float			m_maxLinkDistance;	
	float			m_deltaZ;

	bool			m_isAltPressed;
	bool			m_isCtrlPressed;
	bool			m_isShiftPressed;

	bool			m_isZoomAroundSelectedPoint;

	bool			m_isInHouse;
	bool			m_isNeedUpdate;
	bool			m_isFixedPosition;
	bool			m_isViewTargetMovedOnLastFrame;

	int				m_sittingCameraMode;

	CVector3D		m_lastPos;
	CVector3D		m_lastEyePos;
	CVector3D		m_lastLookAt;	

	CVector3D		m_lastRightPos;
	CVector3D		m_lastRightLookAt;

	CCameraTrackController* m_cameraTrack;

	int				m_lastX;
	int				m_lastY;

	float			m_oldMaxLinkDistance;
};
