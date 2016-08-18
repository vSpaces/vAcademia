#pragma once

namespace SyncManProtocol{
	class CRestoreSyncronizationOut : public CCommonPacketOut
	{
	public:
		CRestoreSyncronizationOut();
		void Create( const wchar_t *apwcRequestedUrl, const syncObjectState* mpcAvatarState);
	};
};