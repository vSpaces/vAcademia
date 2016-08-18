#pragma once
#include <XInput.h>

enum XBOXButtons
{
	XBOX_NONE = 0,
	XBOX_A,
	XBOX_B,
	XBOX_X,
	XBOX_Y,
	XBOX_DPAD_DOWN,
	XBOX_DPAD_RIGHT,
	XBOX_DPAD_UP,
	XBOX_DPAD_LEFT,
	XBOX_LEFT_SHOULDER,
	XBOX_RIGHT_SHOULDER,
	XBOX_LEFT_THUMB,
	XBOX_RIGHT_THUMB,
	XBOX_BACK,
	XBOX_START,
	XBOX_UNKNOWN = -1
};

enum ThumbState
{
	LEFT = XBOXButtons::XBOX_START + 1,
	RIGHT,
	DOWN,
	UP,
	NEUTRAL,
	NONE
};

class IXBOXController
{
public:
    virtual XINPUT_STATE GetState() = 0;
    virtual bool IsConnected() = 0;
    virtual void Vibrate(int leftVal = 0, int rightVal = 0) = 0;
};