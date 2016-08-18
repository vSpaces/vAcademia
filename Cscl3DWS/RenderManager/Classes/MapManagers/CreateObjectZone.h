#pragma once
#include "mapmanagerzone.h"

class CCreateObjectZone :
	public CMapManagerZone
{
public:
	CCreateObjectZone(oms::omsContext* aContext);
	virtual ~CCreateObjectZone();
	void SetMaxPriority();
};
