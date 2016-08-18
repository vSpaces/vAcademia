#pragma once

#include "CommonPacketIn.h"
using namespace Protocol;

namespace SyncManProtocol{
	class CRecordEditorCopyIn : public CCommonPacketIn
	{
	protected:
		int iErrorCode;

	public:
		CRecordEditorCopyIn( BYTE *aData, int aDataSize);
		bool Analyse();		
		const int GetErrorCode() const;
	};
};