#include "stdafx.h"
#include "createobjectzone.h"
#include "ZoneID.h"

CCreateObjectZone::CCreateObjectZone(oms::omsContext* aContext) : CMapManagerZone(aContext)
{
	m_zoneID.id = 0xFFFFFFFF;
	
	int zoneU, zoneV;
	ZoneID::get_zoneuv_from_zone_id(m_zoneID, zoneU, zoneV);
	float zoneX, zoneY;
	ZoneID::get_zone_center_from_xy(zoneU, zoneV, zoneX, zoneY);
	//oobb ob(CalVector( zoneX, zoneY, 0), CalVector( 20000, 20000, 2000000));
	//set_bounds( ob);
}

void CCreateObjectZone::SetMaxPriority()
{
}

CCreateObjectZone::~CCreateObjectZone(void)
{
}