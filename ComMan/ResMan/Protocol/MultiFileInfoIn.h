#include "CommonPacketIn.h"
#include "..\..\infoqueryman.h"
#include <vector>

using namespace std;
using namespace Protocol;

namespace ResManProtocol{
	class CMultiFileInfoIn : public CCommonPacketIn
	{
	protected:
		__int64 lDateTime;
		int idx;
		BYTE errorID;
		DWORD count;
		vector<int> vPosFileInfo;
		int indexFirstFile;
		BYTE *unpackBuffer;

	public:
		CMultiFileInfoIn(BYTE *aData, int aDataSize);
		~CMultiFileInfoIn();
		virtual bool Analyse();
		bool GetFirstFileInfo(DWORD aFlags, CResInfo *aResInfo, bool bGetData = true);
		bool GetFileInfo(int index, DWORD aFlags, CResInfoSaver *resInfoSaver, bool bGetData = true);
		DWORD GetCount();
		BYTE GetError();
	};
};