
#pragma once

#include "Vector3D.h"
#include "Vector4D.h"

class CTimeOfDayController
{
friend class CGlobalSingletonStorage;

public:
	float GetTimeOfDay();

	void SetDayDuration(int seconds);
	__int64 GetDayDuration();

	unsigned short GetColorIntensivityHDR();
	unsigned char GetColorIntensivity();
	float GetBlurness();

	int GetShadowIntensivity();

	CVector4D GetLightColor();

	CVector3D GetSunPosition();

	void BumpLight(bool status);

private:
	CTimeOfDayController();
	CTimeOfDayController(const CTimeOfDayController&);
	CTimeOfDayController& operator=(const CTimeOfDayController&);
	~CTimeOfDayController();

	__int64 m_duration;
	bool m_isBumpLightNeeded;
};