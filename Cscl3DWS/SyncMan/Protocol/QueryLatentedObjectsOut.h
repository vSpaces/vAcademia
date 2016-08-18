#pragma once

namespace SyncManProtocol{
	class CQueryLatentedObjectsOut  : public CCommonPacketOut
	{
		unsigned int count;
	public:
		CQueryLatentedObjectsOut();
		void SaveCount();
		void AddObject(unsigned int aObjectID, unsigned int aBornRealityID, unsigned int aCurrentRealityID, unsigned int auiHash);
		//void Clear();
	};
};