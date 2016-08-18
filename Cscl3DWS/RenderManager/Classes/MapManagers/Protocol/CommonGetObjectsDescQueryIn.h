#pragma once

#include "../CommonRenderManagerHeader.h"

#include "CommonPacketInForMapManager.h"
#include "MapManagerStructures2.h"

using namespace Protocol;

namespace MapManagerProtocol{
	class CCommonGetObjectsDescQueryIn : public CCommonPacketInForMapManager
	{
	protected:
		struct __ObjectPos{
			int iPosObject;
			bool bExist;
		};
	protected:
		byte protocolID;
		unsigned int prevRealityID;
		unsigned int currentRealityID;
		unsigned int currentZoneID;
		unsigned short objectCount;
		int idx;
		MP_VECTOR<__ObjectPos> vObjectPos;
		bool old_server;
		
	public:
		CCommonGetObjectsDescQueryIn(BYTE *aData, int aDataSize, bool aOld_server);
		bool IsObjectExist(short index, unsigned int &notFoundObjectID);
		bool GetZoneObjectInfo(short index, ZONE_OBJECT_INFO& info1, bool bGetData = true);
		unsigned int GetPrevRealityID();
		unsigned int GetCurrentRealityID();
		ZoneIdentifier GetCurrentZoneID();
		unsigned short GetObjectCount();
		virtual bool Analyse();
		virtual bool AnalyseObject();
	};
};