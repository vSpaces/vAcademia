#pragma once

namespace SyncManProtocol{
	class CLeavedSeanceIn : public CCommonPacketIn
	{
	protected:
		unsigned int uiLogicalObjectID;

	public:
		unsigned int iError;
		
		CLeavedSeanceIn( BYTE *aData, int aDataSize);
		bool Analyse();
	};
};