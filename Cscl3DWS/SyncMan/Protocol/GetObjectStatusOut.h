#pragma once

namespace SyncManProtocol{
	class CGetObjectStatusOut : public CCommonPacketOut
	{
	public:
		CGetObjectStatusOut(unsigned int aiObjectID, unsigned int aiBornRealityID);
	};
};