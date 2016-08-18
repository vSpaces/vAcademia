#include "CommonPacketIn.h"
/*#include <string>
using namespace std;*/

using namespace Protocol;

namespace SyncManProtocol{
	class CSeancePausedIn : public CCommonPacketIn
	{
	protected:
		unsigned int currentRealityID;

	public:
		CSeancePausedIn(BYTE *aData, int aDataSize): CCommonPacketIn(aData, aDataSize)
		{
		};
		~CSeancePausedIn();
		bool Analyse();
		unsigned int GetCurrentRealityID() const;

	};
};