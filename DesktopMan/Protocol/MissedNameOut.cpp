#include "stdafx.h"
#include "MissedNameOut.h"

using namespace SharingManProtocol;

CMissedNameOut::CMissedNameOut( unsigned short key)
{
	GetDataBuffer().reset();
	
	USES_CONVERSION;
	GetDataBuffer().addData( &key, sizeof(key));
}

