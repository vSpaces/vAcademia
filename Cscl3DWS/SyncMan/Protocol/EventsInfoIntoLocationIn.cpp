#include "stdafx.h"
#include "EventsInfoIntoLocationIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CEventsInfoIntoLocationIn::CEventsInfoIntoLocationIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize),
		MP_WSTRING_INIT(sEventsInfoIntoLocation)
{
}

CEventsInfoIntoLocationIn::~CEventsInfoIntoLocationIn(){
}



bool CEventsInfoIntoLocationIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;

	if (!memparse_string(pData, idx, sEventsInfoIntoLocation))
		return false;

	return EndOfData( idx);
}

const wchar_t* CEventsInfoIntoLocationIn::GetEventsInfoIntoLocation() const
{
	return sEventsInfoIntoLocation.c_str();
}