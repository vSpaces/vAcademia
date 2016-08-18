
#include "CommonPacketIn.h"

using namespace Protocol;

namespace ResManProtocol{
	class CFileInfoIn : public CCommonPacketIn
	{
	protected:
		BYTE btExist;
		DWORD iFileSize;
		__int64 lDateTime;

	public:
		CFileInfoIn(BYTE *aData, int aDataSize);
		virtual bool Analyse();
		BYTE GetbtExist();
		DWORD GetFileSize();
		__int64 GetDateTime();
	};
};