#include "CommonPacketIn.h"
/*#include <string>
using namespace std;*/

using namespace Protocol;

namespace SyncManProtocol{
	class CPrepareToPlayIn : public CCommonPacketIn
	{
	protected:
		std::wstring wsUrl;
		unsigned short iDataLength;

	public:
		CPrepareToPlayIn(BYTE *aData, int aDataSize): CCommonPacketIn(aData, aDataSize)
		{
		};
		~CPrepareToPlayIn();
		bool Analyse();
		const wchar_t* GetWsURL();
		unsigned short GetDataLength();
	};
};