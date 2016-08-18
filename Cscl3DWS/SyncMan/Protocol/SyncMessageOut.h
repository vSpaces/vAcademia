#pragma once

namespace SyncManProtocol{
	class CSyncMessageOut : public CCommonPacketOut
	{
	public:
		CSyncMessageOut(unsigned int anObjectID, unsigned int aBornRealityID,
			unsigned char* senderName, unsigned short senderNameLength,
			unsigned char* aData, unsigned short aDataSize);
	};
};