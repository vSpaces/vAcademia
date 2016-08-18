
#include "CommonPacketIn.h"

using namespace Protocol;

namespace ResManProtocol{
	class CUploadFileInfoIn : public CCommonPacketIn
	{
	protected:
		BYTE btStatus;
		MP_WSTRING wsErrorText;

	public:
		CUploadFileInfoIn(BYTE *aData, int aDataSize);
		virtual bool Analyse();
		BYTE GetStatus();
		const wchar_t *GetErrorText();
	};
};