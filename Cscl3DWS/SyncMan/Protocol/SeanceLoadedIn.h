#include "CommonPacketIn.h"
/*#include <string>
using namespace std;*/

using namespace Protocol;

namespace SyncManProtocol{
	class CSeanceLoadedIn : public CCommonPacketIn
	{
	protected:
		unsigned int recordID;
		unsigned int seanceID;
		int m_minutes;
		byte isAuthor;
		byte isSuspended;

	public:
		CSeanceLoadedIn(BYTE *aData, int aDataSize): CCommonPacketIn(aData, aDataSize)
		{
		};
		~CSeanceLoadedIn();
		bool Analyse();
		unsigned int GetRecordID() const;
		unsigned int GetSeanceID() const;
		unsigned char IsAuthor() const;
		unsigned char IsSuspended() const;
		int GetMinutes() const;
	};
};