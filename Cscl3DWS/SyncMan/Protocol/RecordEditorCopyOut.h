#pragma once

namespace SyncManProtocol{
	class CRecordEditorCopyOut : public CCommonPacketOut
	{
	public:
		CRecordEditorCopyOut(unsigned int aStartPos, unsigned int aEndPos);

	
	};
};