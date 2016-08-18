#include "CommonPacketOut.h"

using namespace Protocol;

class CClientLogOut : public CCommonPacketOut
{
public:
	CClientLogOut( const char* apcLogData, bool bTimeWrite);	
};