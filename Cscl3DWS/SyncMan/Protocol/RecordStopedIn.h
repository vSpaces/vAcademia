#include "CommonPacketIn.h"
#include "..\SyncMan.h"

using namespace Protocol;

namespace SyncManProtocol{
	class CRecordStopedIn  : public CCommonPacketIn
	{
		unsigned int muRecordID;
		unsigned int muDuration;
		MP_WSTRING mDescription;
		MP_WSTRING mTags;
	public:
		CRecordStopedIn( BYTE* aData, int aDataSize);
		~CRecordStopedIn();
		bool Analyse();
		unsigned int getRecordID() const  {return muRecordID; } 
		unsigned int getDuration() const {return muDuration; } 
		const wchar_t *getDescription() const  {return mDescription.c_str(); } 
		const wchar_t *getTags() const {return mTags.c_str(); } 
	};
};