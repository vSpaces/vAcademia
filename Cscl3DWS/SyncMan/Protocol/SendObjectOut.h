#pragma once

namespace SyncManProtocol{
	class CSendObjectOut : public CCommonPacketOut
	{
	protected:
		BYTE sysPropCount;
		unsigned int sysPropCountOffset;
		unsigned short propertiesCount;
		unsigned int propertiesCountOffset;

	public:
		CSendObjectOut();
		void Create(const syncObjectState& aState);
		unsigned char* GetData();
		unsigned int GetDataSize();

	private:
		void AddHeader(const syncObjectState& aState);
		void CreatePacket(const syncObjectState& aState);
		void AddSysProperties(const syncObjectState& aState);
		void AddProperties(const syncObjectState& aState);
		void AddSubObjects(const syncObjectState& aState);

	};
};