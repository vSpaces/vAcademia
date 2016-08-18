#pragma once

#include "CommonPacketIn.h"
using namespace Protocol;

namespace SyncManProtocol{
	class CRecordEditorSaveAsIn : public CCommonPacketIn
	{
	protected:
		int iErrorCode;

	public:
		CRecordEditorSaveAsIn( BYTE *aData, int aDataSize);
		bool Analyse();		
		const int GetErrorCode() const;
	};
};