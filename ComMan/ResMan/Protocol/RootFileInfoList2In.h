#include "CommonPacketIn.h"
#include "..\..\infoqueryman.h"
#include <vector>

using namespace std;
using namespace Protocol;

namespace ResManProtocol{
	class CRootFileInfoList2In : public CCommonPacketIn
	{
	protected:
		__int64 lDateTime;
		int idx;
		vector<int> vPosFileInfo;
		BYTE *unpackBuffer;		
		CInfoQueryMan *pQueryMan;
		
		bool ReadDataRec( const CWComString &asDir, bool abGetData = true);
		bool ReadData( bool abGetData = true);
		bool GetFileInfo( bool bGetData = true);
		bool GetDirInfo( bool bGetData = true);

	public:
		CRootFileInfoList2In(BYTE *aData, int aDataSize);
		~CRootFileInfoList2In();
		virtual bool Analyse();
		bool AnalyseSubDir();
		bool ReadData( CInfoQueryMan *apQueryMan);		
		BYTE GetError();
	};
};