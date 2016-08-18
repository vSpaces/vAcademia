#pragma once

#include "CommonPacketIn.h"
using namespace Protocol;

namespace SyncManProtocol{
	class CRecordEditorSeekIn : public CCommonPacketIn
	{
	protected:
		int iErrorCode;

	public:
		CRecordEditorSeekIn( BYTE *aData, int aDataSize);
		bool Analyse();		
		const int GetErrorCode() const;
	};
};