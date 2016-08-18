#pragma once

#include "../CommonRenderManagerHeader.h"

#include "CommonPacketInForMapManager.h"
#include "MapManagerStructures2.h"

using namespace Protocol;

namespace MapManagerProtocol{
	class CGetZonesDescQueryIn : public CCommonPacketInForMapManager
	{
	protected:
		unsigned short globalZoneCount; 
		unsigned short localZoneCount;
		unsigned short bbCount;
		unsigned short visCount;
		unsigned int realityID;
		unsigned int currentZoneID;
		unsigned char currentZoneGround;
		struct __ObjectPos{
			int iPosObject;
		};
		MP_VECTOR<unsigned int> vGlobalZones;
		MP_VECTOR<__ObjectPos> vLocalZonePos;
		MP_VECTOR<__ObjectPos> vBoundingBoxPos;
		MP_VECTOR<__ObjectPos> vVisibilityPos;
	public:
		CGetZonesDescQueryIn(BYTE *aData, int aDataSize);		

		virtual bool Analyse();

		__forceinline unsigned int GetRealityID(){return realityID;}
		__forceinline unsigned int GetCurrentZoneID(){return currentZoneID;}
		__forceinline bool CurrentZoneHasGround(){return currentZoneGround != 0;}
		bool GetGlobalZoneInfo(short index, unsigned int &zoneID);
		__forceinline unsigned short GetCountGlobalZone(){return globalZoneCount;}
		__forceinline unsigned short GetCountLocalZone(){return localZoneCount;}
		__forceinline unsigned short GetCountBoundingBox(){return bbCount;}
		__forceinline unsigned short GetCountVisibility(){return visCount;}
	};
};