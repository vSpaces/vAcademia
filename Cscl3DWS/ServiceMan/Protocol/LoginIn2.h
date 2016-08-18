#pragma once

#include "CommonPacketIn.h"
#include <string>
using namespace Protocol;

namespace ServiceManProtocol{
	class CLoginIn2 : public CCommonPacketIn
	{
	protected:
		const BYTE* bsResult;
		unsigned int rmmlID;
		unsigned int bsResultSize;
		unsigned int typeSendClientLog;
		CComString sessionKey;
		CComString lastVersion;
		byte updateParam;
		CComString updaterKey;
		byte btTO;
		CComString downloadURL;
	public:
		CLoginIn2( BYTE *aData, int aDataSize);
		~CLoginIn2();
		bool Analyse();
		const BYTE* GetMethodResult() const;
		unsigned int GetMethodResultSize() const;
		unsigned int getRmmlID() const;
		CComString GetSessionKey() const;
		CComString GetUpdaterKey() const;
		CComString GetLastVersion() const;
		int GetUpdateParam() const;
		int GetTOParam() const;
		CComString GetDownloadURL() const;
		//unsigned int getTypeSendClientLog() const;
	};
};