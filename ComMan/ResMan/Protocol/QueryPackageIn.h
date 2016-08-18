#include "CommonPacketIn.h"
#include "DataBuffer2.h"

using namespace Protocol;

namespace ResManProtocol{
	class CQueryPackageIn : public CCommonPacketIn
	{
	private:
		//unsigned int queryNum;
		// данные удаляется объектом, вызвавшем этот класс.
		CDataBuffer2 *pPacketData;
		unsigned int sizePackage;
	public:
		CQueryPackageIn(BYTE *aData, int aDataSize);
		virtual ~CQueryPackageIn();
		virtual bool Analyse();
		//unsigned int GetQueryNum();
		CDataBuffer2 *DetachPacketData();
		unsigned int GetPackageSize();
	};
};