#include "CommonPacketIn.h"
#include "..\SyncMan.h"

using namespace Protocol;

namespace SyncManProtocol{
	class CUpdatePlayingTimeIn  : public CCommonPacketIn
	{
		unsigned int muTime;
	public:
		CUpdatePlayingTimeIn( BYTE* aData, int aDataSize);
		~CUpdatePlayingTimeIn();
		bool Analyse();
		unsigned int getTime() {return muTime; } 
	};
};