#pragma once

namespace SyncManProtocol{
	class CSaveOut : public CCommonPacketOut
		{
	public:
		CSaveOut();
		CSaveOut(  const wchar_t* apwcAuthor, const wchar_t* apwcCreationTime, const wchar_t* apwcName, const wchar_t* apwcDescription);
	};
};