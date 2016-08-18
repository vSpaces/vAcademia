#pragma once

namespace SyncManProtocol{
	class CQueryObjectOut  : public CCommonPacketOut
	{
	public:
		CQueryObjectOut(unsigned int aObjectID, unsigned int aBornRealityID, unsigned int aRealityID, long long aCreatingTime, bool checkFrame=true);
	};
};