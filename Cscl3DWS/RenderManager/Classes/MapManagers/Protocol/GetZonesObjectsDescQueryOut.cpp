#include "stdafx.h"
#include "GetZonesObjectsDescQueryOut.h"
#include <algorithm>

using namespace MapManagerProtocol;

#define ZQF_CURRENT_ZONE 0x01
#define ZQF_GET_STATIC_OBJECTS 0x02
#define ZQF_GET_SCRIPTED_OBJECTS 0x04

struct CQueryZone 
{
	ZoneID zoneID;
	unsigned char flags;
	bool operator ==(const CQueryZone& aZone) const
	{
		if (zoneID.id != aZone.zoneID.id)
			return false;
		return true;
	}
	bool operator !=(const CQueryZone& aZone) const
	{
		return !(*this == aZone);
	}
};

typedef std::vector<CQueryZone> CQueryZoneVector;

void AddQueryZone( const CQueryZone& aZone, CQueryZoneVector& aQueryZoneVector)
{
	CQueryZoneVector::iterator it = find(aQueryZoneVector.begin(), aQueryZoneVector.end(), aZone);
	if (it == aQueryZoneVector.end())
		aQueryZoneVector.push_back( aZone);
	else
		it->flags |= aZone.flags;
}

void AddQueryZones( const std::vector<ZoneID>& aZones, unsigned char aZoneFlags, CQueryZoneVector& aQueryZoneVector)
{
	CQueryZone queryZone;
	std::vector<ZoneID>::const_iterator itZone = aZones.begin();
	for (;  itZone != aZones.end();  itZone++)
	{
		queryZone.zoneID = *itZone;
		queryZone.flags = aZoneFlags;
		AddQueryZone( queryZone, aQueryZoneVector);
	}
}

CGetZonesObjectsDescQueryOut::CGetZonesObjectsDescQueryOut( unsigned int aPrevRealityID, unsigned int aRealityID,
	ZoneID aCurrentZone, const std::vector<ZoneID>& aFullQueryZones)
{
	Add(aPrevRealityID);
	Add(aRealityID);
	Add(aCurrentZone.id);

	CQueryZoneVector queryZones;
	CQueryZone zone;
	zone.zoneID = aCurrentZone;
	zone.flags = ZQF_CURRENT_ZONE;
	AddQueryZone( zone, queryZones);
	AddQueryZones( aFullQueryZones, ZQF_GET_STATIC_OBJECTS | ZQF_GET_SCRIPTED_OBJECTS, queryZones);

	Add( queryZones.size());

	CQueryZoneVector::const_iterator it = queryZones.begin();
	for (;  it != queryZones.end();  it++)
	{
		Add( it->zoneID.id);
		Add( it->flags);
	}
}
