#include "stdafx.h"
#include "GetZonesDescQueryIn.h"

using namespace MapManagerProtocol;

CGetZonesDescQueryIn::CGetZonesDescQueryIn(BYTE *aData, int aDataSize): 
	CCommonPacketInForMapManager(aData, aDataSize),
	MP_VECTOR_INIT(vGlobalZones),
	MP_VECTOR_INIT(vLocalZonePos),
	MP_VECTOR_INIT(vBoundingBoxPos),
	MP_VECTOR_INIT(vVisibilityPos)
{
};

bool CGetZonesDescQueryIn::Analyse()
{
	if(!CCommonPacketIn::Analyse())
		return false;
	int idx = 0;
	
	unsigned int zoneID;

	if(!memparse_object(pData, idx, realityID))
		return false;

	if(!memparse_object(pData, idx, currentZoneID))
		return false;

	if(!memparse_object(pData, idx, currentZoneGround))
		return false;

	if(!memparse_object(pData, idx, globalZoneCount))
		return false;
	
	for (unsigned short k = 0; k < globalZoneCount; k++)
	{
		if(!memparse_object(pData, idx, zoneID))
			return false;
		vGlobalZones.push_back( zoneID);
	}

	return true;
}

bool CGetZonesDescQueryIn::GetGlobalZoneInfo(short index, unsigned int &zoneID)
{
	if(vGlobalZones.size()<=(unsigned int)index)
		return false;
	zoneID = vGlobalZones.at(index);
	return true;
}
