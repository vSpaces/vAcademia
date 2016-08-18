#pragma once

namespace SyncManProtocol{
	class CDeleteRecordSegmentOut : public CCommonPacketOut
	{
	public:
		CDeleteRecordSegmentOut();
		CDeleteRecordSegmentOut( unsigned long begPos, unsigned long endPos);
	};
};