#include "CommonPacketIn.h"
#include "DataBuffer2.h"

using namespace Protocol;

namespace ResManProtocol{
	class CQueryMessageIn //: public CCommonPacketIn
	{
	protected:
		// данные удаляется объектом, вызвавшем этот класс.
		CDataBuffer2 *data;
		unsigned int iMessageData;
	public:
		CQueryMessageIn(CDataBuffer2 *aData);
		virtual ~CQueryMessageIn(){};
		virtual bool Analyse();
		unsigned int GetMessageSize();
		BYTE *GetMessageData(BOOL *bError);
	};
};