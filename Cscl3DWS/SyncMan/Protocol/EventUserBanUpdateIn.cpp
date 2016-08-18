#include "stdafx.h"
#include "EventUserBanUpdateIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CEventUserBanUpdateIn::CEventUserBanUpdateIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize),
	MP_WSTRING_INIT(sEventName),
	MP_WSTRING_INIT(sEventUrl)
{
}

CEventUserBanUpdateIn::~CEventUserBanUpdateIn()
{

}



bool CEventUserBanUpdateIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_string(pData, idx, sEventName))
		return false;
	if(!memparse_string(pData, idx, sEventUrl))
		return false;
	if (!memparse_object(pData, idx, bBan))
		return false;
	return EndOfData( idx);
}

const wchar_t* CEventUserBanUpdateIn::GetEventName() const
{
	return sEventName.c_str();
}

const wchar_t* CEventUserBanUpdateIn::GetEventUrl() const
{
	return sEventUrl.c_str();
}

