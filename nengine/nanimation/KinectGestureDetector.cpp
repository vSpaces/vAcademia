
#include "StdAfx.h"
#include "KinectGestureDetector.h"

#define SWIPE_MAX_TIME					1000
#define SWIPE_MIN_TIME					500
#define SWIPE_TIME_INTERVAL				100
#define SWIPE_MIN_LENGTH				0.5f
#define SWIPE_MAX_HEIGHT_DELTA			0.25f
#define SWIPE_MAX_DELTA_FROM_AVG_Y		0.2f
#define COORDS_EPS						0.03f
#define DELAY_AFTER_GESTURE				1500
#define MAX_BIP_DELTA_FOR_STAY			0.05f
#define UNKNOWN_BIP_POSITION_COMPONENT	-1000

CKinectGestureDetector::CKinectGestureDetector():
	MP_VECTOR_INIT(m_leftHandPoints),
	MP_VECTOR_INIT(m_rightHandPoints),
	MP_MAP_INIT(m_gesturesHandlers),
	m_stopGesturesTime(0),
	m_bipPosition(UNKNOWN_BIP_POSITION_COMPONENT, UNKNOWN_BIP_POSITION_COMPONENT, UNKNOWN_BIP_POSITION_COMPONENT),
	m_isStay(false)
{
}

void CKinectGestureDetector::UpdateBipPosition(float* coords)
{
	m_bipPosition.x = coords[SKELETON_POSITION_HIP_CENTER * 3 + 0];
	m_bipPosition.y = coords[SKELETON_POSITION_HIP_CENTER * 3 + 1];
	m_bipPosition.z = coords[SKELETON_POSITION_HIP_CENTER * 3 + 2];
}

void CKinectGestureDetector::CheckForStaying(float* coords)
{
	if (UNKNOWN_BIP_POSITION_COMPONENT == m_bipPosition.x)
	{
		UpdateBipPosition(coords);
	}

	m_isStay = true;
	if ((fabsf(m_bipPosition.x - coords[SKELETON_POSITION_HIP_CENTER * 3 + 0]) > MAX_BIP_DELTA_FOR_STAY) ||
		(fabsf(m_bipPosition.y - coords[SKELETON_POSITION_HIP_CENTER * 3 + 1]) > MAX_BIP_DELTA_FOR_STAY) ||
		(fabsf(m_bipPosition.z - coords[SKELETON_POSITION_HIP_CENTER * 3 + 2]) > MAX_BIP_DELTA_FOR_STAY))
	{
		m_isStay = false;
		UpdateBipPosition(coords);
	}
}

void CKinectGestureDetector::CheckForSwipes(float* coords)
{
	if (m_isStay)
	{
		if (coords)
		{
			DetectSwipeLeft(coords);
			DetectSwipeRight(coords);
		}
	}
	else
	{
		ClearAllPoints();
	}
}

void CKinectGestureDetector::ProcessPoints(float* coords)
{
	if (g->ne.time < m_stopGesturesTime)
	{
		return;
	}

	CheckForStaying(coords);
    CheckForSwipes(coords);	
}

void CKinectGestureDetector::AddGestureHandler(int gestureID, IGestureHandler* gestureHandler)
{
	if (gestureHandler)
	{
		m_gesturesHandlers.insert(MP_MAP<int, IGestureHandler*>::value_type(gestureID, gestureHandler));
	}
}

void CKinectGestureDetector::RemoveGestureHandler(int gestureID, IGestureHandler* gestureHandler)
{
	MP_MAP<int, IGestureHandler*>::iterator it = m_gesturesHandlers.find(gestureID);
	if (it != m_gesturesHandlers.end())
	if ((*it).second == gestureHandler)
	{
		m_gesturesHandlers.erase(it);
	}
}

void CKinectGestureDetector::AccumulatePoints(float* coords, int pointIndex, 
											  std::vector<SGestureFrame*>& arr, int maxTime)
{
	int pointOffset = pointIndex * 3;
	MP_NEW_P4(frame, SGestureFrame, coords[pointOffset], coords[pointOffset + 1], 
		coords[pointOffset + 2], g->ne.time);
	arr.push_back(frame);
	
	while (g->ne.time - (*arr.begin())->time > maxTime)
	{
		SGestureFrame* frame = *arr.begin();
		MP_DELETE(frame);

		arr.erase(arr.begin());
	}
}

void CKinectGestureDetector::DetectSwipeLeft(float* coords)
{
	AccumulatePoints(coords, SKELETON_POSITION_HAND_LEFT, m_leftHandPoints, SWIPE_MAX_TIME);
	
	DetectSwipeForTime(coords, -1, GESTURE_SWIPE_LEFT, m_leftHandPoints);
}

bool CKinectGestureDetector::DetectSwipeForTime(float* coords, int sign, int gestureID,  std::vector<SGestureFrame*>& arr)
{
	for (int time = SWIPE_MAX_TIME; time >= SWIPE_MIN_TIME; time -= SWIPE_TIME_INTERVAL)
	{
		std::vector<SGestureFrame*> newArr = GetPointsForTime(arr, time);
		bool res = DetectSwipe(coords, sign, gestureID, newArr);
		if (res)
		{
			return true;
		}
	}

	return false;
}

bool CKinectGestureDetector::DetectSwipe(float* coords, int sign, int gestureID,  std::vector<SGestureFrame*>& arr)
{
	if (arr.size() > 1)
	{
		SGestureFrame* firstFrame = *arr.begin();
		SGestureFrame* endFrame = *(arr.end() - 1);		

		if ((-endFrame->point.x * sign + firstFrame->point.x * sign > SWIPE_MIN_LENGTH) &&
			(fabsf(endFrame->point.y - firstFrame->point.y) < SWIPE_MAX_HEIGHT_DELTA))
		{
			float avgY = (firstFrame->point.y + endFrame->point.y) * 0.5f;

			// движение ниже таза - не мб, что это Swipe
			if (avgY < coords[SKELETON_POSITION_HIP_CENTER * 3 + 1])
			{
				return false;
			}

			// движение выше шеи - не мб, что это Swipe
			if (avgY > coords[SKELETON_POSITION_SHOULDER_CENTER * 3 + 1])
			{
				return false;
			}

			MP_VECTOR<SGestureFrame *>::iterator it =  arr.begin();
			MP_VECTOR<SGestureFrame *>::iterator itPrev =  arr.begin();
			MP_VECTOR<SGestureFrame *>::iterator itEnd =  arr.end();
			it++;

			for ( ; it != itEnd; it++, itPrev++)
			{
				if (((*it)->point.x * sign > (*itPrev)->point.x * sign) && (fabsf((*it)->point.x - (*itPrev)->point.x) > COORDS_EPS))
				{
					return false;
				}

				if (fabsf((*it)->point.y - avgY) > SWIPE_MAX_DELTA_FROM_AVG_Y)
				{
					return false;
				}
			}

			OnGestureDetected(gestureID);
		}
	}

	return true;
}

std::vector<SGestureFrame*> CKinectGestureDetector::GetPointsForTime(std::vector<SGestureFrame*>& arr, int maxTime)
{
	std::vector<SGestureFrame*> res;
	if (arr.size() > 0)
	{
		std::vector<SGestureFrame*>::iterator it = arr.end();
		it--;

		__int64 lastTime = (*it)->time;
	
		it = arr.begin();
		std::vector<SGestureFrame*>::iterator itEnd = arr.end();

		for ( ; it != itEnd; it++)
		if ((*it)->time >= lastTime - maxTime)
		{
			res.push_back(*it);
		}

		return res;
	}
	else
	{
		return res;
	}
}

void CKinectGestureDetector::DetectSwipeRight(float* coords)
{
	AccumulatePoints(coords, SKELETON_POSITION_HAND_RIGHT, m_rightHandPoints, SWIPE_MAX_TIME);

	DetectSwipeForTime(coords, 1, GESTURE_SWIPE_RIGHT, m_rightHandPoints);
}

void CKinectGestureDetector::OnGestureDetected(int gestureID)
{
	MP_MAP<int, IGestureHandler*>::iterator it = m_gesturesHandlers.find(gestureID);
	if (it != m_gesturesHandlers.end())
	{
		(*it).second->OnGestureDetected(gestureID);
	}

	ClearAllPoints();
	m_stopGesturesTime = g->ne.time + DELAY_AFTER_GESTURE;
}

void CKinectGestureDetector::ClearAllPoints()
{
	m_leftHandPoints.clear();
	m_rightHandPoints.clear();
}

CKinectGestureDetector::~CKinectGestureDetector()
{
}