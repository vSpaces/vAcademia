#pragma once

#include "CommonPacketIn.h"
using namespace Protocol;

namespace SyncManProtocol{
	class CRecordEditorPauseIn : public CCommonPacketIn
	{
	protected:
		int iErrorCode;

	public:
		CRecordEditorPauseIn( BYTE *aData, int aDataSize);
		bool Analyse();		
		const int GetErrorCode() const;
	};
};