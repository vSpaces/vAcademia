#pragma once

namespace SyncManProtocol{
	class CRecordEditorSeekOut : public CCommonPacketOut
	{
	public:
		CRecordEditorSeekOut(unsigned int aSeekPos);
	
	};
};