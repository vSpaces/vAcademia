#pragma once

#include "CommonPacketIn.h"
using namespace Protocol;

namespace SyncManProtocol{
	class CRecordEditorUndoIn : public CCommonPacketIn
	{
	protected:
		int iErrorCode;
		int iRecordDuration;

	public:
		CRecordEditorUndoIn( BYTE *aData, int aDataSize);
		bool Analyse();		
		const int GetErrorCode() const;
		const int GetRecordDuration() const;
	};
};