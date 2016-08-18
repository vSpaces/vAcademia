#pragma once

namespace SyncManProtocol{
	class CRecordEditorCutOut : public CCommonPacketOut
	{
	public:
		CRecordEditorCutOut(unsigned int aStartPos, unsigned int aEndPos);

	
	};
};