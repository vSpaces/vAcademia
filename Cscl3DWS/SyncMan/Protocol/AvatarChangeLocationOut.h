#pragma once

namespace SyncManProtocol{
	class CAvatarChangeLocationOut : public CCommonPacketOut
	{
	public:
		CAvatarChangeLocationOut(const wchar_t* apwcLocationName,  unsigned int auiObjectID, unsigned int auiBornRealityID);

	
	};
};