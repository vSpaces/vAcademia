#include "CommonPacketInForMapManager.h"
#include "NatureSettings.h"

using namespace Protocol;

namespace MapManagerProtocol{
	class CNatureSettingsIn : public CCommonPacketInForMapManager
	{
	private:
		int idx;
	public:
		CNatureSettingsIn(BYTE *aData, int aDataSize) :
			CCommonPacketInForMapManager(aData, aDataSize)
		{
		};
		virtual bool Analyse();

	public:
		bool GetNatureInfo(SERVER_NATURE_SETTINGS_INFO *info, bool bGetData = true);
	};
};
