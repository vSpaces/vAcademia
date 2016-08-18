#include "CommonPacketIn.h"

using namespace Protocol;

namespace SyncManProtocol
{
	class ObjectCreateResultQueryIn : public CCommonPacketIn
	{
	protected:
		unsigned int uErrorCode; 
		unsigned int uObjectID; 
		unsigned int uRmmlHandlerID;
		unsigned int uBornRealityID;
		
	public:

		ObjectCreateResultQueryIn(BYTE *aData, int aDataSize);

		virtual bool Analyse();

		unsigned int GetObjectID()
		{
			return uObjectID;
		}
		unsigned int GetBornRealityID()
		{
			return uBornRealityID;
		}
		unsigned int GetCreationErrorCode()
		{
			return uErrorCode;
		}
		unsigned int GetRmmlHandlerID()
		{
			return uRmmlHandlerID;
		}
	};
};