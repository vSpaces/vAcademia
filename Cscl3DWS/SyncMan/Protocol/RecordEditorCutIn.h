#pragma once

#include "CommonPacketIn.h"
using namespace Protocol;

namespace SyncManProtocol{
	class CRecordEditorCutIn : public CCommonPacketIn
	{
	protected:
		int iErrorCode;
		int iRecordDuration;

	public:
		CRecordEditorCutIn( BYTE *aData, int aDataSize);
		bool Analyse();		
		const int GetErrorCode() const;
		const int GetRecordDuration() const;
	};
};