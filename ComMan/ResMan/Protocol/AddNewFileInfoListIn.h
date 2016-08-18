#include "CommonPacketIn.h"
#include "..\..\infoqueryman.h"
#include <vector>

using namespace std;
using namespace Protocol;

namespace ResManProtocol{
	class CAddNewFileInfoListIn : public CCommonPacketIn
	{
	protected:
		__int64 lDateTime;
		int idx;
		unsigned int count;
		vector<int> vPosFileInfo;
		BYTE *unpackBuffer;

	public:
		CAddNewFileInfoListIn(BYTE *aData, int aDataSize);
		~CAddNewFileInfoListIn();
		virtual bool Analyse( ILogWriter* apLW);
		bool GetFileInfo(int index, CResInfoSaver *resInfoSaver, bool bGetData = true);
		DWORD GetCount();
		BYTE GetError();
	};
};