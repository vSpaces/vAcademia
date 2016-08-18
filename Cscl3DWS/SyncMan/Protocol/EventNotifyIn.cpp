#include "stdafx.h"
#include "EventNotifyIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CEventNotifyIn::CEventNotifyIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

CEventNotifyIn::~CEventNotifyIn(){
}



bool CEventNotifyIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_object(pData, idx, uiEventID))
		return false;
	if (!memparse_string(pData, idx, sEventLocationID))
		return false;
	if(!memparse_object(pData, idx, uiEventRealityID))
		return false;
	if(!memparse_object(pData, idx, uiPlayingRecordID))
		return false;
	if(!memparse_object(pData, idx, uiBaseRecordID))
		return false;
	if(!memparse_object(pData, idx, uiEventRecordID))
		return false;
	if(!memparse_object(pData, idx, ubIsEventRecording))
		return false;
	if (!memparse_string(pData, idx, sJSONDescription))
		return false;
	
	return EndOfData( idx);
}

const wchar_t* CEventNotifyIn::GetJSONDescription() const
{
	return sJSONDescription.c_str();
}

const wchar_t* CEventNotifyIn::GetEventLocationID() const
{
	return sEventLocationID.c_str();
}

