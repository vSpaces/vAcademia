#pragma once
#include "oms_context.h"

namespace SyncManProtocol{

	class CEventsInfoIntoLocationIn : public CCommonPacketIn
	{

	public:
		CEventsInfoIntoLocationIn( BYTE *aData, int aDataSize);
		~CEventsInfoIntoLocationIn();

	public:
		bool Analyse();
		const wchar_t* GetEventsInfoIntoLocation() const;

	protected:
		MP_WSTRING	sEventsInfoIntoLocation;
	};

};