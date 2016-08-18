
#pragma once

#include "IGestureHandler.h"
#include "Vector3D.h"

typedef struct _SGestureFrame
{
	CVector3D point;
	__int64 time;

	_SGestureFrame(const float x, const float y, const float z, const __int64 _time)
	{
		point.x = x;
		point.y = y;
		point.z = z;
		time = _time;
	}
} SGestureFrame;

class CKinectGestureDetector
{
public:
	CKinectGestureDetector();
	~CKinectGestureDetector();

	void ProcessPoints(float* coords);

	void AddGestureHandler(int gestureID, IGestureHandler* gestureHandler);
	void RemoveGestureHandler(int gestureID, IGestureHandler* gestureHandler);
	void OnGestureDetected(int gestureID);

private:
	void DetectSwipeLeft(float* coords);
	void DetectSwipeRight(float* coords);
	bool DetectSwipe(float* coords, int sign, int gestureID,  std::vector<SGestureFrame*>& arr);
	bool DetectSwipeForTime(float* coords, int sign, int gestureID,  std::vector<SGestureFrame*>& arr);

	void AccumulatePoints(float* coords, int pointIndex, std::vector<SGestureFrame*>& arr,
		int maxTime);
	std::vector<SGestureFrame*> GetPointsForTime(std::vector<SGestureFrame*>& arr, int maxTime);
	void ClearAllPoints();

	void CheckForSwipes(float* coords);
	void CheckForStaying(float* coords);

	void UpdateBipPosition(float* coords);

	MP_VECTOR<SGestureFrame *> m_leftHandPoints;
	MP_VECTOR<SGestureFrame *> m_rightHandPoints;

	MP_MAP<int, IGestureHandler*> m_gesturesHandlers;

	__int64 m_stopGesturesTime;

	CVector3D m_bipPosition;
	bool m_isStay;
};