#include "stdafx.h"
#include "EventLeavedNotifyIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CEventLeavedNotifyIn::CEventLeavedNotifyIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

CEventLeavedNotifyIn::~CEventLeavedNotifyIn(){
}



bool CEventLeavedNotifyIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_object(pData, idx, uiEventID))
		return false;
	if(!memparse_object(pData, idx, uiPlayingRecordID))
		return false;
	return EndOfData( idx);
}

