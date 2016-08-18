#include "stdafx.h"
#include "SessionStartOut.h"

using namespace SharingManProtocol;

CSessionStartOut::CSessionStartOut( LPCSTR alpName, bool canRemoteAccess)
{
	GetDataBuffer().reset();
	
	USES_CONVERSION;
	GetDataBuffer().addString( A2W(alpName));
	GetDataBuffer().add<bool>( canRemoteAccess);
	GetDataBuffer().add<bool>( false);
}
CSessionStartOut::CSessionStartOut( LPCSTR alpName, LPCSTR alpFileName, unsigned int aRecordID, bool isPaused, unsigned int aSeekTime, unsigned int aServerVersion)
{
	GetDataBuffer().reset();

	USES_CONVERSION;
	GetDataBuffer().addString( A2W(alpName));
	GetDataBuffer().add<bool>( false);
	GetDataBuffer().add<bool>( true);
	GetDataBuffer().addString( A2W(alpFileName));
	GetDataBuffer().add<unsigned int>( aRecordID);
	GetDataBuffer().add<bool>( isPaused);
	GetDataBuffer().add<unsigned int>( aSeekTime);
	GetDataBuffer().add<unsigned int>( aServerVersion);
}

