#include "stdafx.h"
#include "SessionParamsIn.h"

unsigned int CSessionParamsQueryIn::PLAY_SESSION_TYPE = 1;
unsigned int CSessionParamsQueryIn::RECORD_SESSION_TYPE = 2;
unsigned int CSessionParamsQueryIn::REMOTEACCESS_SESSION_TYPE = 3;


CSessionParamsQueryIn::CSessionParamsQueryIn(BYTE *aData, int aDataSize) : 
		CCommonPacketIn(aData, aDataSize)
{
}

bool CSessionParamsQueryIn::Analyse()
{
	if ( !CCommonPacketIn::Analyse())
		return false;

	int idx = 0;

	// 1. session name
	if ( !memparse_string_unicode( pData, idx, sessionName))
		return false;

	// 2. session type
	if ( !memparse_object( pData, idx, sessionType))
		return false;

	return EndOfData( idx);
}

LPCSTR	CSessionParamsQueryIn::GetSessionName()
{
	return sessionName.GetBuffer();
}

unsigned int	CSessionParamsQueryIn::GetSessionType()
{
	return sessionType;
}
