
#pragma once

#include "CommonHeader.h"

class CMotion;

class IDestroyListener
{
public:
	virtual void OnDestroy(CMotion* const motion) = 0;
};