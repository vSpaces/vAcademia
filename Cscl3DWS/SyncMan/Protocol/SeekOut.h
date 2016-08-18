#pragma once

namespace SyncManProtocol{
	class CSeekOut : public CCommonPacketOut
	{
	public:
		CSeekOut();
		CSeekOut( unsigned int seekPos);
	};
};