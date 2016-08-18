#pragma once

namespace SyncManProtocol{
	class CRecordEditorSaveAsOut : public CCommonPacketOut
	{
	public:
		CRecordEditorSaveAsOut(const wchar_t* apwcName, const wchar_t* apwcDescription, bool abOverwrite, bool abPublicate);
	
	};
};