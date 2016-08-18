#include "stdafx.h"
#include "ServerErrorIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CServerErrorIn::CServerErrorIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

CServerErrorIn::~CServerErrorIn(){
}



bool CServerErrorIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if (!memparse_object<unsigned int>(pData, idx, mErrorCode))
		return false;
	return EndOfData( idx);
}

unsigned int CServerErrorIn::GetErrorCode() const{
	return mErrorCode;
}

