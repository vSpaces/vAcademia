
#pragma once

#include "CommonHeader.h"

typedef struct _SIdlesPackDesc
{
	MP_VECTOR<int> motionIDs;
	MP_VECTOR<int> between;
	MP_VECTOR<int> freq;

	_SIdlesPackDesc(int idleCount);

	void AddIdle(int motionID, int _between, int _freq)
	{
		motionIDs.push_back(motionID);
		between.push_back(_between);
		freq.push_back(_freq);
	}
} SIdlesPackDesc;