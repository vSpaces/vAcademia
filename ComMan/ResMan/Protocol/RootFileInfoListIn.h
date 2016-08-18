#include "CommonPacketIn.h"
#include "..\..\infoqueryman.h"
#include <vector>

using namespace std;
using namespace Protocol;

namespace ResManProtocol{
	class CRootFileInfoListIn : public CCommonPacketIn
	{
	protected:
		__int64 lDateTime;
		int idx;
		unsigned int count;
		vector<int> vPosFileInfo;
		BYTE *unpackBuffer;

	public:
		CRootFileInfoListIn(BYTE *aData, int aDataSize);
		~CRootFileInfoListIn();
		virtual bool Analyse();
		bool GetFileInfo(int index, CResInfoSaver *resInfoSaver, bool bGetData = true);
		DWORD GetCount();
		BYTE GetError();
	};
};