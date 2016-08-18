#pragma once
#include "oms_context.h"

namespace SyncManProtocol{

	class CSeancesInfoIn : public CCommonPacketIn
	{

	public:
		CSeancesInfoIn( BYTE *aData, int aDataSize);
		~CSeancesInfoIn();

	public:
		bool Analyse();
		const wchar_t* GetSeancesInfo() const;
		bool GetIsFull();

	protected:
		MP_WSTRING	sSeancesInfo;
		byte btFull;
	};

};