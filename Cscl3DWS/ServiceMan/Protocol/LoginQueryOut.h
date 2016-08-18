#include "CommonPacketOut.h"

using namespace Protocol;

namespace ServiceManProtocol
{
	class CLoginQueryOut : public CCommonPacketOut
	{
	public:
		CLoginQueryOut(int requestID, const wchar_t *aLogin, const wchar_t *aPassword, int source, bool isAnonymous, const wchar_t* apwcLanguage);
	};
};