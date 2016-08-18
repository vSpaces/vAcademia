#pragma once

#include "CommonPacketIn.h"
using namespace Protocol;

namespace SyncManProtocol{
	class CRecordEditorSaveIn : public CCommonPacketIn
	{
	protected:
		int iErrorCode;

	public:
		CRecordEditorSaveIn( BYTE *aData, int aDataSize);
		bool Analyse();		
		const int GetErrorCode() const;
	};
};