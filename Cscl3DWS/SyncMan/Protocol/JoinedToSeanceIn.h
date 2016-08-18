#include "CommonPacketIn.h"

using namespace Protocol;
namespace SyncManProtocol{
	class CJoinedToSeanceIn : public CCommonPacketIn
	{
	protected:
		unsigned int uiSeanceMode;
		unsigned int uiLogicalObjectID;

	public:
		unsigned int iError;
		
		CJoinedToSeanceIn( BYTE *aData, int aDataSize);
		bool Analyse();
		unsigned int GetSeanceMode() const;
		unsigned int GetLogicalObjectID() const;
	};
};