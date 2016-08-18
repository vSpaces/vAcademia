#include "stdafx.h"
#include "ServiceLoadingFailedIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

CServiceLoadingFailedIn::CServiceLoadingFailedIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

CServiceLoadingFailedIn::~CServiceLoadingFailedIn(){
}



bool CServiceLoadingFailedIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_object(pData, idx, rmmlID))
		return false;
	if(!memparse_object(pData, idx, errorCode))
		return false;
	return EndOfData( idx);
}

unsigned int CServiceLoadingFailedIn::getRmmlID() const
{
	return rmmlID;
}

unsigned int CServiceLoadingFailedIn::getErrorCode() const
{
	return errorCode;
}
