//#include "DataBuffer.h"
#pragma once

#include "CommonPacketIn.h"
#include <string>
using namespace Protocol;

namespace SyncManProtocol{
	class CEventNotifyIn : public CCommonPacketIn
	{
	protected:
		std::wstring sJSONDescription;
		std::wstring sEventLocationID;
		unsigned int uiEventID;
		unsigned int uiEventRealityID;
		unsigned int uiPlayingRecordID;
		unsigned int uiBaseRecordID;
		unsigned int uiEventRecordID;
		bool		 ubIsEventRecording;

	public:
		CEventNotifyIn( BYTE *aData, int aDataSize);
		~CEventNotifyIn();
		bool Analyse();
		const wchar_t* GetJSONDescription() const;
		const wchar_t* GetEventLocationID() const;
		inline unsigned int GetEventID() {return uiEventID; }
		inline unsigned int GetEventRealityID() {return uiEventRealityID; }
		inline unsigned int GetPlayingRecordID() {return uiPlayingRecordID; }
		inline unsigned int GetBaseRecordID() {return uiBaseRecordID; }
		inline unsigned int GetEventRecordID() {return uiEventRecordID; }
		inline bool GetIsEventRecording() {return ubIsEventRecording; }
	};
};