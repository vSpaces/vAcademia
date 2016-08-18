#include "stdafx.h"
#include "ReceivedPrecisedURLIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CReceivedPrecisedURLIn::CReceivedPrecisedURLIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize),
	MP_WSTRING_INIT(sURL)
{
}

CReceivedPrecisedURLIn::~CReceivedPrecisedURLIn(){
}



bool CReceivedPrecisedURLIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if (!memparse_string(pData, idx, sURL))
		return false;
	return EndOfData( idx);
}

const wchar_t* CReceivedPrecisedURLIn::GetURL() const{
	return sURL.c_str();
}

