#pragma once

namespace SyncManProtocol{
	class CDeleteObjectOut : public CCommonPacketOut
	{
	public:
		CDeleteObjectOut(unsigned int aiObjectID, unsigned int aiBornRealityID);
	};
};