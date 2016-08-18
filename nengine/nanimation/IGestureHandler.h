
#pragma once

#define GESTURE_SWIPE_LEFT		1
#define GESTURE_SWIPE_RIGHT		2

class IGestureHandler
{
public:
	virtual void OnGestureDetected(int gestureID) = 0;
};