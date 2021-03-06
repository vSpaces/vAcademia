
#pragma once

#define USER_SETTING_QUALITY_MODE			0
#define USER_SETTING_VSYNC_ENABLED			1
#define USER_SETTING_VOICE_ENABLED			2
#define USER_SETTING_V2DSOUND_ENABLED		3
#define USER_SETTING_V3DSOUND_ENABLED		4
#define USER_SETTING_SCREEN_WIDTH			5
#define USER_SETTING_SCREEN_HEIGHT			6
#define USER_SETTING_ONLINE_EVENTS_INFO		7
#define USER_SETTING_HIHT_WINDOW_SHOW		8
#define USER_SETTING_AUDIO_AUTO_GAIN_CONTROL_ENABLED	9
#define USER_SETTING_AUDIO_DENOISING_ENABLED			10
#define USER_SETTING_AUDIO_INPUT_DEVICE_GUID			11
#define USER_SETTING_AUDIO_INPUT_DEVICE_NAME			12
#define USER_SETTING_AUDIO_INPUT_DEVICE_LINE			13
#define USER_SETTING_AUDIO_IN_MODE						14
#define USER_SETTING_AUDIO_IN_VOLUME_GAIN				15
#define USER_SETTING_AUDIO_AUTO_VOLUME					16
#define USER_SETTING_AUDIO_VOICE_GAIN_LEVEL				17
#define USER_SETTING_AUDIO_VOICE_ACTIVATION_LEVEL		18
#define USER_SETTING_AUDIO_VOICE_QUALITY				19
#define USER_SETTING_AUDIO_HANDS_FREE_ENABLED			20
#define USER_SETTING_DISTANCE_KOEF						21
#define USER_SETTING_AUDIO_BACK_VOLUME					22
#define USER_SETTING_AUDIO_EFFECTS_VOLUME				23
#define USER_SETTING_NAVIGATION_A_LA_SL_ENABLED			24
#define USER_SETTING_MULTISAMPLE_POWER					25
#define USER_SETTING_ALLOW_AVATAR_MOVE_BY_MOUSE			26

#define USER_SETTING_AUDIO_INPUT_MIX_DEVICE_GUID		27
#define USER_SETTING_AUDIO_INPUT_MIX_DEVICE_NAME		28
#define USER_SETTING_AUDIO_INPUT_MIX_DEVICE_LINE		29
#define USER_SETTING_AUDIO_OUTPUT_DEVICE_GUID			30
#define USER_SETTING_AUDIO_OUTPUT_DEVICE_NAME			31
#define USER_SETTING_AUDIO_OUTPUT_DEVICE_LINE			32
#define USER_SETTING_SHOW_CLOUDS 						33

// общее число не глобальных пользовательских настроек
#define SETTING_COUNT									34

// нумерация для глобальных пользовательских настроек
#define USER_SETTING_LOGIN								(SETTING_COUNT)
#define USER_SETTING_PASSWORD							(SETTING_COUNT+1)
// Possible value for USER_SETTING_PROXY_MODE
// 0 - auto-detect (from browser)
// 1 - don`t use proxy 
// 2 - use proxy
#define USER_SETTING_PROXY_MODE							(SETTING_COUNT+2)
#define USER_SETTING_PROXY_ADDRESS						(SETTING_COUNT+3)
#define USER_SETTING_PROXY_PORT							(SETTING_COUNT+4)
#define USER_SETTING_PROXY_USE_USER						(SETTING_COUNT+5)
#define USER_SETTING_PROXY_USERNAME						(SETTING_COUNT+6)
#define USER_SETTING_PROXY_PASS							(SETTING_COUNT+7)

// Possible value for USER_SETTING_SOCKS_MODE
// 0 - auto-detect (from browser)
// 1 - don`t use socks 
// 2 - use socks
#define USER_SETTING_SOCKS_MODE							(SETTING_COUNT+8)
#define USER_SETTING_SOCKS_ADDRESS						(SETTING_COUNT+9)
#define USER_SETTING_SOCKS_PORT							(SETTING_COUNT+10)
#define USER_SETTING_SOCKS_USE_USER						(SETTING_COUNT+11)
#define USER_SETTING_SOCKS_USERNAME						(SETTING_COUNT+12)
#define USER_SETTING_SOCKS_PASS							(SETTING_COUNT+13)
#define USER_SETTING_SOCKS_VER							(SETTING_COUNT+14)
#define USER_SETTING_SLIDE_SHOW							(SETTING_COUNT+15)
#define USER_SETTING_DISABLE_BETADRIVER					(SETTING_COUNT+16)
#define USER_SETTING_DISABLE_OLDDRIVER					(SETTING_COUNT+17)
#define USER_SETTING_DISABLE_MINSYSREQ					(SETTING_COUNT+18)

#define USER_SETTING_HTTPS_MODE							(SETTING_COUNT+19)
#define USER_SETTING_HTTPS_ADDRESS						(SETTING_COUNT+20)
#define USER_SETTING_HTTPS_PORT							(SETTING_COUNT+21)
#define USER_SETTING_HTTPS_USE_USER						(SETTING_COUNT+22)
#define USER_SETTING_HTTPS_USERNAME						(SETTING_COUNT+23)
#define USER_SETTING_HTTPS_PASS							(SETTING_COUNT+24)

#define USER_SETTING_LS_ADDRESS							(SETTING_COUNT+25)
#define USER_SETTING_LS_PORT							(SETTING_COUNT+26)
#define USER_SETTING_LS_USE_USER						(SETTING_COUNT+27)
#define USER_SETTING_LS_USERNAME						(SETTING_COUNT+28)
#define USER_SETTING_LS_PASSWORD						(SETTING_COUNT+29)
#define USER_SETTING_LS_TYPE							(SETTING_COUNT+30)
#define USER_SETTING_LS_TUNNEL							(SETTING_COUNT+31)
#define USER_SETTING_LS_ENABLED							(SETTING_COUNT+32)
#define USER_SETTING_LS_NAME							(SETTING_COUNT+33)
#define USER_SETTING_INTERACTIVEBOARD_MONITOR			(SETTING_COUNT+34)

#define USER_SETTING_KINECT_LEFT_CORNER_X				(SETTING_COUNT+35)
#define USER_SETTING_KINECT_LEFT_CORNER_Y				(SETTING_COUNT+36)
#define USER_SETTING_KINECT_LEFT_CORNER_Z				(SETTING_COUNT+37)
#define USER_SETTING_KINECT_RIGHT_CORNER_X				(SETTING_COUNT+38)
#define USER_SETTING_KINECT_RIGHT_CORNER_Y				(SETTING_COUNT+39)
#define USER_SETTING_KINECT_RIGHT_CORNER_Z				(SETTING_COUNT+40)

#define USER_SETTING_DISPLAYID_FOR_INTERACTIVE_BOARD	(SETTING_COUNT+41)

#define USER_SETTING_AUDIO_MASTER_VOLUME				(SETTING_COUNT+42)
#define USER_SETTING_ECHO_CANCELLATION					(SETTING_COUNT+43)

#define USER_SETTING_LAST								(SETTING_COUNT+44)


// общее число глобальных пользовательских настроек
#define SETTING_GLOBAL_COUNT							(USER_SETTING_LAST-SETTING_COUNT)


#include <string>

class IDefaultSettingValueProvider
{
public:
	virtual std::string GetDefaultValue(const unsigned int settingID) = 0;};