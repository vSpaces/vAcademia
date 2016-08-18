#include "CommonPacketIn.h"
#include "..\..\infoqueryman.h"

using namespace Protocol;

namespace ResManProtocol{
	class CPackClientSetupIn : public CCommonPacketIn
	{
	protected:
		int iStatus;
		__int64 lDateTime;
		int idx;
		int indexFirstFile;

	public:
		CPackClientSetupIn(BYTE *aData, int aDataSize);
		virtual bool Analyse();
		bool GetInfo(CResInfoSaver *resInfoSaver, bool bGetData = true);
		int GetStatus();
	};
};