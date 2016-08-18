
#include "StdAfx.h"
#include <math.h>
#include <assert.h>
#include "TimeOfDayController.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"

CTimeOfDayController::CTimeOfDayController():
	m_isBumpLightNeeded(false)
{
}

CVector3D CTimeOfDayController::GetSunPosition()
{
	float sunAngle = (g->dtc.GetTimeOfDay() - 5 * 60) / 60.0f / 16.0f * 180.0f + 0.0f;
	return CVector3D(150.0f * cosf(sunAngle / 57.0f), 0, 150.0f * sinf(sunAngle / 57.0f));
}

int CTimeOfDayController::GetShadowIntensivity()
{
	int max = 0;//g->rs.GetInt(SHADOWS_MAX_INTENSIVITY);
	int min = 0;//g->rs.GetInt(SHADOWS_MIN_INTENSIVITY);
	return (int)(sinf(g->dtc.GetTimeOfDay() / 60.0f / 24.0f * 3.14628f) * (max - min)) + min;
}

float CTimeOfDayController::GetTimeOfDay()
{
	if (g->rs.GetBool(FIX_TIME_OF_DAY_ENABLED))
	{
		return (float)g->rs.GetInt(TIME_OF_DAY_START_TIME);
	}

	float time = (float)g->ne.time;
	float timeOfDay = fmodf(((float)g->rs.GetInt(TIME_OF_DAY_START_TIME) + 60.0f * 24.0f * fmodf(time, 1000.0f * (float)GetDayDuration()) / 1000.0f / (float)GetDayDuration()), 60.0f * 24.0f);
	return timeOfDay;
}

void CTimeOfDayController::SetDayDuration(int seconds)
{
	m_duration = seconds;
}

__int64 CTimeOfDayController::GetDayDuration()
{
	if (g->rs.GetInt(DAY_LENGTH) == 0)
	{
		return m_duration;
	}
	else
	{
		return g->rs.GetInt(DAY_LENGTH);
	}
}

void CTimeOfDayController::BumpLight(bool status)
{
	m_isBumpLightNeeded = status;
}

unsigned short CTimeOfDayController::GetColorIntensivityHDR()
{
	float time = g->dtc.GetTimeOfDay();

	float _lightness = 0.0f;
	if ((time >= 4 * 60) && (time <= 22 * 60))
	{
		_lightness = sinf((time - 4.0f * 60.0f) / 60.0f / 18.0f * 3.14628f) * 0.9f + 0.2f;
	}
	
	if (time < 4 * 60)
	{
		_lightness = sinf((time + 2.0f * 60.0f) / 60.0f / 6.0f * 3.14628f) * 0.2f + 0.2f;
	}

	if (time > 22 * 60)
	{
		_lightness = sinf((time - 22.0f * 60.0f) / 60.0f / 6.0f * 3.14628f) * 0.2f + 0.2f;
	}

	unsigned short color = (unsigned short)(_lightness * 255.0f);

	if (m_isBumpLightNeeded)
	{
		color = (unsigned short)(color * 2.3f);
	}

	return color;
}

unsigned char CTimeOfDayController::GetColorIntensivity()
{
	float time = g->dtc.GetTimeOfDay();

	float _lightness = 0.0f;
	if ((time >= 4 * 60) && (time <= 22 * 60))
	{
		_lightness = sinf((time - 4.0f * 60.0f) / 60.0f / 18.0f * 3.14628f) * 0.8f + 0.2f;
	}
	
	if (time < 4 * 60)
	{
		_lightness = sinf((time + 2.0f * 60.0f) / 60.0f / 6.0f * 3.14628f) * 0.2f + 0.2f;
	}

	if (time > 22 * 60)
	{
		_lightness = sinf((time - 22.0f * 60.0f) / 60.0f / 6.0f * 3.14628f) * 0.2f + 0.2f;
	}

	unsigned char color = (unsigned char)(_lightness * 255.0f);

	return color;
}

CVector4D CTimeOfDayController::GetLightColor()
{
	float lights[25 * 4] = {0.05f, 0.05f, 0.05f, 1.0f, // 0
							0.05f, 0.05f, 0.05f, 1.0f, // 1
							0.05f, 0.05f, 0.05f, 1.0f, // 2
							0.03f, 0.03f, 0.03f, 1.0f, // 3
							0.02f, 0.02f, 0.02f, 1.0f, // 4
							0.5f, 0.3f, 0.1f, 1.0f, // 5
							0.4f, 0.24f, 0.08f, 1.0f, // 6
							0.3f, 0.18f, 0.06f, 1.0f, // 7
							0.3f, 0.18f, 0.06f, 1.0f, // 8
							0.3f, 0.18f, 0.06f, 1.0f, // 9
							0.3f, 0.18f, 0.06f, 1.0f, // 10
							0.3f, 0.18f, 0.06f, 1.0f, // 11
							0.3f, 0.20f, 0.10f, 1.0f, // 12
							0.3f, 0.25f, 0.15f, 1.0f, // 13
							0.3f, 0.20f, 0.10f, 1.0f, // 14
							0.3f, 0.18f, 0.06f, 1.0f, // 15
							0.3f, 0.18f, 0.06f, 1.0f, // 16
							0.3f, 0.18f, 0.06f, 1.0f, // 17
							0.3f, 0.18f, 0.06f, 1.0f, // 18
							0.3f, 0.18f, 0.06f, 1.0f, // 19
							0.4f, 0.24f, 0.08f, 1.0f, // 20
							0.5f, 0.3f, 0.1f, 1.0f, // 21
							0.02f, 0.02f, 0.02f, 1.0f, // 22
							0.03f, 0.03f, 0.03f, 1.0f, // 23
							0.05f, 0.05f, 0.05f, 1.0f}; // 24

	float time = GetTimeOfDay();
	int hour = (int)(time / 60);
	int hour2 = hour + 1;

	float koef = fmodf(time, 60.0f) / 60.0f;
	float koef2 = 1.0f - koef;

	CVector4D color;
	color.x = lights[hour * 4 + 0] * koef2 + lights[hour2 * 4 + 0] * koef;
	color.y = lights[hour * 4 + 1] * koef2 + lights[hour2 * 4 + 1] * koef;
	color.z = lights[hour * 4 + 2] * koef2 + lights[hour2 * 4 + 2] * koef;
	color.w = lights[hour * 4 + 3] * koef2 + lights[hour2 * 4 + 3] * koef;
	color.y = 1.0f;
	color.x = 1.0f;
	color.z = 1.0f;

	return color;
}

float CTimeOfDayController::GetBlurness()
{
	/*int time = GetTimeOfDay();
	if ((time >= 0) && (time <= 60 * 6))
	{
		return 0.5f * g->rs.GetInt(POSTEFFECT_MAX_POWER) / 10.0f;
	}

	if ((time >= 20 * 60))
	{
		return 0.5f * g->rs.GetInt(POSTEFFECT_MAX_POWER) / 10.0f;
	}

	//return (sinf((float)(time - 60 * 6) / 60.0f / 14.0f * 3.14628f) * g->rs.GetInt(POSTEFFECT_MAX_POWER) / 10.0f);*/
	return (g->rs.GetInt(POSTEFFECT_MAX_POWER) / 10.0f);
}

CTimeOfDayController::~CTimeOfDayController()
{

}