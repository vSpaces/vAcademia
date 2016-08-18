#include "stdafx.h"
#include "EventUserRightUpdateIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CEventUserRightUpdateIn::CEventUserRightUpdateIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize),
	MP_WSTRING_INIT(sJSONEventUserCaps)
{
}

CEventUserRightUpdateIn::~CEventUserRightUpdateIn(){
}



bool CEventUserRightUpdateIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_object(pData, idx, uiEventID))
		return false;
	if (!memparse_string(pData, idx, sJSONEventUserCaps))
		return false;
	return EndOfData( idx);
}

const wchar_t* CEventUserRightUpdateIn::GetChangedEventUserCaps() const
{
	return sJSONEventUserCaps.c_str();
}

