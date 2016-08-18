#include "stdafx.h"
#include "SeancesInfoIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CSeancesInfoIn::CSeancesInfoIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize),
	MP_WSTRING_INIT(sSeancesInfo)
{
}

CSeancesInfoIn::~CSeancesInfoIn(){
}



bool CSeancesInfoIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;

	if (!memparse_string(pData, idx, sSeancesInfo))
		return false;

	if (!memparse_object(pData, idx, btFull))
		return false;

	return EndOfData( idx);
}

const wchar_t* CSeancesInfoIn::GetSeancesInfo() const
{
	return sSeancesInfo.c_str();
}

bool CSeancesInfoIn::GetIsFull()
{
	return btFull!=0;
}