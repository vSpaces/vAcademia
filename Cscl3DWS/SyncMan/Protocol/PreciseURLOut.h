#pragma once

namespace SyncManProtocol
{
	class CPreciseURLOut : public CCommonPacketOut
	{
	public:
		CPreciseURLOut();
		CPreciseURLOut(const wchar_t *aURL);
	};
};