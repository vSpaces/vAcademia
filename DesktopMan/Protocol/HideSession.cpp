#include "StdAfx.h"
#include "./HideSession.h"

using namespace SharingManProtocol;

HideSession::HideSession(unsigned short sessionID, LPCSTR alpFileName, bool  sessionState)
{
	GetDataBuffer().reset();

	GetDataBuffer().add<unsigned short>( sessionID);
	USES_CONVERSION;
	GetDataBuffer().addString( A2W(alpFileName));

	byte sessionStateFlag = sessionState ? 1 : 0;

	GetDataBuffer().add<byte>( sessionStateFlag);
}

HideSession::~HideSession(void)
{
}
