//#include "DataBuffer.h"
#pragma once

#include "CommonPacketIn.h"
#include <string>
using namespace Protocol;

namespace SyncManProtocol{
	class CEventLeavedNotifyIn : public CCommonPacketIn
	{
	protected:
		unsigned int uiEventID;
		unsigned int uiPlayingRecordID;

	public:
		CEventLeavedNotifyIn( BYTE *aData, int aDataSize);
		~CEventLeavedNotifyIn();
		bool Analyse();
		inline unsigned int GetEventID() {return uiEventID; }
		inline unsigned int GetPlayingRecordID() {return uiPlayingRecordID; }
	};
};