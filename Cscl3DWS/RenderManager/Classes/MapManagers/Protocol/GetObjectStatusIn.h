#include "CommonPacketInForMapManager.h"

using namespace Protocol;

namespace MapManagerProtocol{
	class CGetObjectStatusIn : public CCommonPacketInForMapManager
	{
		unsigned int objectID;
		unsigned int bornRealityID;
		int status;
		unsigned int zoneID;
	public:
		CGetObjectStatusIn(BYTE *aData, int aDataSize) :
			CCommonPacketInForMapManager(aData, aDataSize)
		{
		};
		virtual bool Analyse();

	public:
		unsigned int GetObjectID(){ return objectID; }
		unsigned int GetBornRealityID(){ return bornRealityID; }
		int GetStatus(){ return status; }
		unsigned int GetZoneID(){ return zoneID; }
	};
};
