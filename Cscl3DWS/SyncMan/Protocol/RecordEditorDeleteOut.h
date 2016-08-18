#pragma once

namespace SyncManProtocol{
	class CRecordEditorDeleteOut : public CCommonPacketOut
	{
	public:
		CRecordEditorDeleteOut(unsigned int aStartPos, unsigned int aEndPos);

	
	};
};