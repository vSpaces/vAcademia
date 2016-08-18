#include "stdafx.h"
#include "../../../include/RecordServer/Protocol/PlaySessionConnectedQueryIn.h"


CPlaySessionConnectedQueryIn::CPlaySessionConnectedQueryIn(BYTE *aData, int aDataSize) : 
		CCommonPacketIn(aData, aDataSize)
			, realityId(-1)
			, recordId(-1)
{
}

bool CPlaySessionConnectedQueryIn::Analyse()
{
	if ( !CCommonPacketIn::Analyse())
		return false;

	int idx = 0;

	// режим сессии 1 байт
	int	aiPlaySessionMode = 0;
	if ( !memparse_object( pData, idx, aiPlaySessionMode))
		return false;
	ATLASSERT( aiPlaySessionMode <= EPM_LAST);
	playSessionMode = (EPLAYSESIONMODE)aiPlaySessionMode;

	// реальность
	if ( !memparse_object( pData, idx, realityId))
		return false;

	// запись
	if ( !memparse_object( pData, idx, recordId))
		return false;

	return EndOfData( idx);
}

EPLAYSESIONMODE	CPlaySessionConnectedQueryIn::GetPlaySessionMode()
{
	return playSessionMode;
}

unsigned int	CPlaySessionConnectedQueryIn::GetRealityID()
{
	return realityId;
}

unsigned int	CPlaySessionConnectedQueryIn::GetRecordID()
{
	return recordId;
}