#pragma once

namespace SyncManProtocol{
	class CListenZonesOut : public CCommonPacketOut
	{
	public:
		CListenZonesOut( unsigned int aRealityID,
			unsigned int aZoneCount, unsigned long* aZones);
	};
};