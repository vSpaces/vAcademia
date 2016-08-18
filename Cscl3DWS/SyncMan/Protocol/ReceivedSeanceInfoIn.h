#include "CommonPacketIn.h"
#include "..\SyncMan.h"

using namespace Protocol;
using namespace sync;
namespace SyncManProtocol{

	class CReceivedSeanceInfoIn : public CCommonPacketIn
	{
		unsigned int iSeancesCount;
		unsigned int IDLogObject;
	
	public:
		CReceivedSeanceInfoIn( BYTE *aData, int aDataSize);
		~CReceivedSeanceInfoIn();
		bool Analyse();
		unsigned int GetIDLogObject()  const;
		syncSeanceInfo pSeanceInfo;
	};
};