#pragma once

#include "CommonPacketIn.h"
using namespace Protocol;

namespace SyncManProtocol{
	class CRecordEditorPlayIn : public CCommonPacketIn
	{
	protected:
		int iErrorCode;

	public:
		CRecordEditorPlayIn( BYTE *aData, int aDataSize);
		bool Analyse();		
		const int GetErrorCode() const;
	};
};