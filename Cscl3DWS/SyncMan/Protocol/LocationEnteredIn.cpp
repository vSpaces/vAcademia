#include "stdafx.h"
#include "LocationEnteredIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CLocationEnteredIn::CLocationEnteredIn( BYTE *apData, int apDataSize) 
	: CCommonPacketIn( apData, apDataSize)
{
	//??
}

CLocationEnteredIn::~CLocationEnteredIn()
{
}

bool CLocationEnteredIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx = 0;
	if(!memparse_string(pData, idx, wsLocationID))
		return false;
	if(!memparse_string(pData, idx, wsJSONDescription))
		return false;
	return EndOfData( idx);
}

const wchar_t* CLocationEnteredIn::GetLocationID() const
{
	return wsLocationID.c_str();
}

const wchar_t* CLocationEnteredIn::GetJSONDescription() const
{
	return wsJSONDescription.c_str();
}