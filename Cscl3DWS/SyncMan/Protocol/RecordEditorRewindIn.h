#pragma once

#include "CommonPacketIn.h"
using namespace Protocol;

namespace SyncManProtocol{
	class CRecordEditorRewindIn : public CCommonPacketIn
	{
	protected:
		int iErrorCode;

	public:
		CRecordEditorRewindIn( BYTE *aData, int aDataSize);
		bool Analyse();		
		const int GetErrorCode() const;
	};
};